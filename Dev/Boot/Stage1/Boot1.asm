;*********************************************
;	플로피 디스크로부터의 부팅
;*********************************************

%define KERNEL_LOADER_ADDRESS 0x050

;리얼 모드에서 시작
[BITS 16]

;부트코드는 0x07c0 번지에 로드되므로
;주소 재조정이 필요하다
[ORG 0x0000]	

		nop
		nop
		nop
;*********************************************
;	BIOS Parameter Block
;*********************************************
bpbOEM			db "Sky OS  "
bpbBytesPerSector:  	DW 512
bpbSectorsPerCluster: 	DB 1
bpbReservedSectors: 	DW 1
bpbNumberOfFATs: 	DB 2
bpbRootEntries: 	DW 224
bpbTotalSectors: 	DW 2880
bpbMedia: 		DB 0xf0  ;; 0xF1
bpbSectorsPerFAT: 	DW 9
bpbSectorsPerTrack: 	DW 18
bpbHeadsPerCylinder: 	DW 2
bpbHiddenSectors: 	DD 0
bpbTotalSectorsBig:     DD 0
bsDriveNumber: 	        DB 0
bsUnused: 		DB 0
bsExtBootSignature: 	DB 0x29
bsSerialNumber:	        DD 0xa0a1a2a3
bsVolumeLabel: 	        DB "MOS FLOPPY "
bsFileSystem: 	        DB "FAT12   "

BOOTMAIN:

;세그먼트 레지스터 값들을 0x07C0로 초기화
;현재 메모리상의 코드 위치는 0x07C0:IP이다.
          mov     ax, 0x07C0				
          mov     ds, ax
          mov     es, ax
          mov     fs, ax
          mov     gs, ax

 ;스택 설정 0x0000:0000~0x0000:FFFF 64KB 크기로 생성
          mov     ax, 0x0000				; set the stack
          mov     ss, ax
          mov     sp, 0xFFFE

;부팅 드라이브 번호 저장		  
          mov  [BootDevice], dl
    
;루트 디렉토리 테이블을 읽는다
LOAD_ROOT: 
; 루트 디렉토리 테이블의 크기를 CX 레지스터에 저장한다
     
          xor     cx, cx
          xor     dx, dx
          mov     ax, 0x0020                           ; 32 byte directory entry
          mul     WORD [bpbRootEntries]                ; total size of directory
          div     WORD [bpbBytesPerSector]             ; sectors used by directory
          xchg    ax, cx
          
; 루트 디렉토리 테이블의 위치를 AX 레지스터에 저장한다
     
          mov     al, BYTE [bpbNumberOfFATs]            ; number of FATs
          mul     WORD [bpbSectorsPerFAT]               ; sectors used by FATs
          add     ax, WORD [bpbReservedSectors]         ; adjust for bootsector
          mov     WORD [datasector], ax                 ; base of root directory
          add     WORD [datasector], cx
          
; 부트섹터 코드 바로 다음에(512바이트) 루트 디렉토리를 읽어들인다 (07C0:0200)
     
          mov     bx, 0x0200                            ; copy root dir above bootcode
          call    ReadSectors

;루트 디렉토리를 검색해서 커널 로더 파일 정보를 찾는다
; 루트 디렉토리에는 최대 224개의 엔트리가 존재할 수 있다
; 파일 이름은 FAT12에서 최대 12자. 우리의 커널로더와 같은 이름의 루트 디렉토리 엔트리를 찾는다
          mov     cx, WORD [bpbRootEntries]             ; load loop counter
          mov     di, 0x0200                            ; locate first root entry
     .LOOP:
          push    cx
          mov     cx, 0x000B                            ; eleven character name
          mov     si, ImageName                         ; image name to find
          push    di
     rep  cmpsb                                         ; test for entry match
          pop     di
          je      LOAD_FAT
          pop     cx
          add     di, 0x0020                            ; queue next directory entry
          loop    .LOOP
          jmp     FAILURE

;----------------------------------------------------
; FAT 정보 로드. 앞단계에서 di 레지스터에 이미지 파일의 루트 디렉토리 엔트리 옵셋을 구했다
;----------------------------------------------------

     LOAD_FAT:
     
     ; 커널 로더의 시작 클러스터 값을 dx 레지스터에 저장한다
          mov     dx, WORD [di + 0x001A]
          mov     WORD [cluster], dx                  ; 파일의 첫번째 클러스터 값 저장
          
     ; cx 레지스터에 FAT의 크기를 저장한다
     
          xor     ax, ax
          mov     al, BYTE [bpbNumberOfFATs]          ; number of FATs
          mul     WORD [bpbSectorsPerFAT]             ; sectors used by FATs
          mov     cx, ax

     ; AX 레지스터에 FAT의 위치를 지정한다. 부트섹터 다음 섹터가 FAT 시작위치. 즉 1

          mov     ax, WORD [bpbReservedSectors]       ; adjust for bootsector
          
     ; FAT 정보를 메모리로 읽어들인다 (07C0:0200)

          mov     bx, 0x0200                          ; copy FAT above bootcode
          call    ReadSectors
		  
	; 커널 로더 파일을 메모리속으로 읽어들일 준비를 한다 (0A00:0000)
     
          mov     ax, KERNEL_LOADER_ADDRESS
          mov     es, ax                              ; destination for image
          mov     bx, 0x0000                          ; destination for image
          push    bx

     ;----------------------------------------------------
     ; 파일을 읽어들인다
     ;----------------------------------------------------

     LOAD_IMAGE:
     
          mov     ax, WORD [cluster]                  ; cluster to read
          pop     bx                                  ; buffer to read into
          call    ClusterLBA                          ; convert cluster to LBA
          xor     cx, cx
          mov     cl, BYTE [bpbSectorsPerCluster]     ; sectors to read
          call    ReadSectors
          push    bx
          
     ; compute next cluster
     
          mov     ax, WORD [cluster]                  ; identify current cluster
          mov     cx, ax                              ; copy current cluster
          mov     dx, ax                              ; copy current cluster
          shr     dx, 0x0001                          ; divide by two
          add     cx, dx                              ; sum for (3/2)
		  
          mov     bx, 0x0200                          ; location of FAT in memory
          add     bx, cx                              ; index into FAT
          mov     dx, WORD [bx]                       ; read two bytes from FAT
          test    ax, 0x0001
          jnz     .ODD_CLUSTER
          
     .EVEN_CLUSTER:
     
          and     dx, 0000111111111111b               ; take low twelve bits
         jmp     .DONE
         
     .ODD_CLUSTER:
     
          shr     dx, 0x0004                          ; take high twelve bits
          
     .DONE:
     
          mov     WORD [cluster], dx                  ; store new cluster
          cmp     dx, 0x0FF0                          ; test for end of file
          jb      LOAD_IMAGE
          
     DONE:
     
          mov     si, msgCRLF
          call    Print
		  
		  mov     si, msgComplete
          call    Print
        
	  mov	  dl, [BootDevice]
          push    WORD KERNEL_LOADER_ADDRESS
          push    WORD 0x0000
          retf
          
     FAILURE:
     
          mov     si, msgFailure
          call    Print
          mov     ah, 0x00
          int     0x16                                ; await keypress
          int     0x19                                ; warm boot computer
		  
		 ;************************************************;
;	Prints a string
;	DS=>SI: 0 terminated string
;************************************************;
Print:
	lodsb						; load next byte from string from SI to AL
	or	al, al					; Does AL=0?
	jz	PrintDone				; Yep, null terminator found-bail out
	mov	ah, 0eh					; Nope-Print the character
	int	10h
	jmp	Print					; Repeat until null terminator found
PrintDone:
	ret						; we are done, so return


absoluteSector db 0x00
absoluteHead   db 0x00
absoluteTrack  db 0x00

;************************************************;
; Convert CHS to LBA
; LBA = (cluster - 2) * sectors per cluster
;************************************************;

ClusterLBA:
          sub     ax, 0x0002                          ; zero base cluster number
          xor     cx, cx
          mov     cl, BYTE [bpbSectorsPerCluster]     ; convert byte to word
          mul     cx
          add     ax, WORD [datasector]               ; base data sector
          ret

;************************************************;
; Convert LBA to CHS
; AX=>LBA Address to convert
;
; absolute sector = (logical sector / sectors per track) + 1
; absolute head   = (logical sector / sectors per track) MOD number of heads
; absolute track  = logical sector / (sectors per track * number of heads)
;
;************************************************;

LBACHS:
          xor     dx, dx                              ; prepare dx:ax for operation
          div     WORD [bpbSectorsPerTrack]           ; calculate
          inc     dl                                  ; adjust for sector 0
          mov     BYTE [absoluteSector], dl
          xor     dx, dx                              ; prepare dx:ax for operation
          div     WORD [bpbHeadsPerCylinder]          ; calculate
          mov     BYTE [absoluteHead], dl
          mov     BYTE [absoluteTrack], al
          ret


;************************************************;
; Reads a series of sectors
; CX=>Number of sectors to read
; AX=>Starting sector
; ES:BX=>Buffer to read to
;************************************************;

ReadSectors:
     .MAIN
          mov     di, 0x0005                          ; five retries for error
     .SECTORLOOP
          push    ax
          push    bx
          push    cx
          call    LBACHS                              ; convert starting sector to CHS
          mov     ah, 0x02                            ; BIOS read sector
          mov     al, 0x01                            ; read one sector
          mov     ch, BYTE [absoluteTrack]            ; track
          mov     cl, BYTE [absoluteSector]           ; sector
          mov     dh, BYTE [absoluteHead]             ; head
          mov     dl, BYTE [bsDriveNumber]            ; drive
          int     0x13                                ; invoke BIOS
          jnc     .SUCCESS                            ; test for read error
          xor     ax, ax                              ; BIOS reset disk
          int     0x13                                ; invoke BIOS
          dec     di                                  ; decrement error counter
          pop     cx
          pop     bx
          pop     ax
          jnz     .SECTORLOOP                         ; attempt to read again
          int     0x18
     .SUCCESS
          mov     si, msgProgress
          call    Print
          pop     cx
          pop     bx
          pop     ax
          add     bx, WORD [bpbBytesPerSector]        ; queue next buffer
          inc     ax                                  ; queue next sector
          loop    .MAIN                               ; read next sector
          ret

;*********************************************
;	Bootloader Entry Point
;********************************************* 


     BootDevice  db 0
     datasector  dw 0x0000
     cluster     dw 0x0000
     ImageName   db "KRNLDR  SYS"
     msgCRLF     db 0x0D, 0x0A, 0x00
     msgProgress db ".", 0x00
     msgFailure  db 0x0D, 0x0A, "MISSING OR CURRUPT KRNLDR. Press Any Key to Reboot", 0x0D, 0x0A, 0x00
	 msgComplete  db 0x0D, 0x0A, "Loading Complete", 0x0D, 0x0A, 0x00
     
          TIMES 510-($-$$) DB 0
          DW 0xAA55
