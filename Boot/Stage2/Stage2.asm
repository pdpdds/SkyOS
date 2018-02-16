
;*******************************************************
;
;	Stage2.asm
;		Stage2 Bootloader
;
;	OS Development Series
;*******************************************************

bits	16

org 0x5000

jmp	main				; go to start

;*******************************************************
;	Preprocessor directives
;*******************************************************

%include "stdio.inc"			; basic i/o routines
;%include "Gdt.inc"			; Gdt routines
%include "A20.inc"			; A20 enabling
%include "Common.inc"
%include "bootinfo.inc"
%include "memory.inc"

%define ROOT_OFFSET 0x7e00
%define FAT_OFFSET 0x7c00



%define NULL_DESC 0
%define CODE_DESC 0x8
%define DATA_DESC 0x10


;*******************************************************
;	Data Section
;*******************************************************

LoadingMsg db 0x0D, 0x0A, "Searching for Operating System...", 0x00
msgFailure db 0x0D, 0x0A, "*** AAA: Missing or corrupt KRNL32.EXE. Press Any Key to Reboot.", 0x0D, 0x0A, 0x0A, 0x00
msgrrrrrr db 0x0D, 0x0A, "*** ssssss", 0x0D, 0x0A, 0x0A, 0x00
msgssssss db 0x0D, 0x0A, "*** fghj", 0x0D, 0x0A, 0x0A, 0x00
; kernel name
ImageName     db "KRNL32  EXE"

ImageSize     db 0

boot_info:
istruc multiboot_info
	at multiboot_info.flags,			dd 0
	at multiboot_info.memoryLo,			dd 0
	at multiboot_info.memoryHi,			dd 0
	at multiboot_info.bootDevice,			dd 0
	at multiboot_info.cmdLine,			dd 0
	at multiboot_info.mods_count,			dd 0
	at multiboot_info.mods_addr,			dd 0
	at multiboot_info.syms0,			dd 0
	at multiboot_info.syms1,			dd 0
	at multiboot_info.syms2,			dd 0
	at multiboot_info.mmap_length,			dd 0
	at multiboot_info.mmap_addr,			dd 0
	at multiboot_info.drives_length,		dd 0
	at multiboot_info.drives_addr,			dd 0
	at multiboot_info.config_table,			dd 0
	at multiboot_info.bootloader_name,		dd 0
	at multiboot_info.apm_table,			dd 0
	at multiboot_info.vbe_control_info,		dd 0
	at multiboot_info.vbe_mode_info,		dw 0
	at multiboot_info.vbe_interface_seg,		dw 0
	at multiboot_info.vbe_interface_off,		dw 0
	at multiboot_info.vbe_interface_len,		dw 0
iend
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

 BootDevice  db 0
     datasector  dw 0x0000
     cluster     dw 0x0000
     msgCRLF     db 0x0D, 0x0A, 0x00
     msgProgress db ".", 0x00
	 msgComplete  db 0x0D, 0x0A, "Loading Complete", 0x0D, 0x0A, 0x00
	 
	 
	 
		  
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
     .MAINA
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
          loop    .MAINA                              ; read next sector
          ret

main:

	;-------------------------------;
	;   Setup segments and stack	;
	;-------------------------------;

	cli	                   ; clear interrupts
	mov ax, 0
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	; create stack
	mov ax, 0x0000
	mov ss, ax
	mov sp, 0xFFFF
	sti	                   ; enable interrupts

	mov     	[boot_info+multiboot_info.bootDevice], dl
	
	
	call		_EnableA20
	sti

	xor		eax, eax
	xor		ebx, ebx
	call		BiosGetMemorySize64MB

	push		eax
	mov		eax, 64
	mul		ebx
	mov		ecx, eax
	pop		eax
	add		eax, ecx
	add		eax, 1024		; the routine doesnt add the KB between 0-1MB; add it

	mov		dword [boot_info+multiboot_info.memoryHi], 0
	mov		dword [boot_info+multiboot_info.memoryLo], eax
	
	mov		eax, 0x0
	;mov		ds, ax
	mov		di, 0x2000
	call		BiosGetMemoryMap
	
	
	
	

	
;---------------------------------------------------------	
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
     
          mov     bx, ROOT_OFFSET                            ; copy root dir above bootcode
          call    ReadSectors
		  
		  	


;루트 디렉토리를 검색해서 커널 로더 파일 정보를 찾는다
; 루트 디렉토리에는 최대 224개의 엔트리가 존재할 수 있다
; 파일 이름은 FAT12에서 최대 12자. 우리의 커널로더와 같은 이름의 루트 디렉토리 엔트리를 찾는다
          mov     cx, WORD [bpbRootEntries]             ; load loop counter
          mov     di, ROOT_OFFSET                            ; locate first root entry
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
          mov     dx, WORD [di + 0x501A]
          mov     WORD [cluster], dx                  ; 파일의 첫번째 클러스터 값 저장
          
     ; cx 레지스터에 FAT의 크기를 저장한다
     
          xor     ax, ax
          mov     al, BYTE [bpbNumberOfFATs]          ; number of FATs
          mul     WORD [bpbSectorsPerFAT]             ; sectors used by FATs
          mov     cx, ax

     ; AX 레지스터에 FAT의 위치를 지정한다. 부트섹터 다음 섹터가 FAT 시작위치. 즉 1

          mov     ax, WORD [bpbReservedSectors]       ; adjust for bootsector
          
     ; FAT 정보를 메모리로 읽어들인다 (07C0:0200)

          mov     bx, FAT_OFFSET                          ; copy FAT above bootcode
          call    ReadSectors
		  
	; 커널 로더 파일을 메모리속으로 읽어들일 준비를 한다 (0A00:0000)
 ;------------------Reading Kernel DLL into memory------------------
	mov ax, IMAGE_RMODE_BASE			; destination of image CS
	mov es, ax
	mov bx, 0x0000                          ; destination for image IP
	push bx
	
LOAD_IMAGE:
	mov ax, WORD [cluster]                  ; cluster to read
	pop bx                                  ; buffer to read into
	call ClusterLBA                         ; convert cluster to LBA
	xor cx, cx
	mov cl, BYTE [bpbSectorsPerCluster]        ; sectors to read
	call ReadSectors
;comptute kernelSize
	push ecx
	mov ecx,ImageSize
	inc ecx
	mov dword [ImageSize], ecx
	pop ecx

	cmp bx, 1024				; Segment override (64kb)
	jne .SegmentOk
	mov ax, es				; next segment ( add 64)
	add ax, 64
	mov es, ax
	mov bx, 0
.SegmentOk:

	push    bx						    	;compute next cluster
	mov ax, WORD [cluster]                  ; identify current cluster
	mov cx, ax                              ; copy current cluster
	mov dx, ax                              ; copy current cluster
	shr dx, 0x0001                          ;
	;divide by two
	add cx, dx                              ; sum for (3/2)
	mov bx, 0x7c00			     		; location of FAT in memory
	add bx, cx                              ; index into FAT
	mov dx, WORD [bx]                       ; read two bytes from FAT
	test ax, 0x0001
	jnz .ODD_CLUSTER
.EVEN_CLUSTER:
	and dx, 0000111111111111b               ; take low twelve bits
	jmp .DONE
.ODD_CLUSTER:
	shr dx, 0x0004                          ; take high twelve bits
.DONE:
	mov WORD [cluster], dx                  ; store new cluster
	cmp dx, 0x0FF0                          ; test for end of file
	jb LOAD_IMAGE
          
     DONE:
		
     
          mov     si, msgCRLF
          call    Print
		  
		  mov     si, msgComplete
          call    Print
		  
		  
        
	
;---------------------------------------------------------	
   	mov   		cx, ImageSize
	cmp		cx, 0
	
	jne		EnterStage3
FAILURE:	
	mov		si, msgFailure
	call   		Puts16
	mov		ah, 0
	int     	0x16                    ; await keypress
	int     	0x19                    ; warm boot computer

	;-------------------------------;
	;   Go into pmode               ;
	;-------------------------------;

EnterStage3:

	
	
	cli	                           ; clear interrupts
		
	lgdt 	[toc]        ; load GDT into GDTR
	
	mov	eax, cr0                   ; set bit 0 in cr0--enter pmode
	or	eax, 1
	mov	cr0, eax


	jmp	CODE_DESC:Stage3              ; far jump to fix CS. Remember that the code selector is 0x8!

	; Note: Do NOT re-enable interrupts! Doing so will triple fault!
	; We will fix this in Stage 3.

;******************************************************
;	ENTRY POINT FOR STAGE 3
;******************************************************



%define NULL_DESC 0
%define CODE_DESC 0x8
%define DATA_DESC 0x10

bits 32

%include "Paging.inc"

BadImage db "*** FATAL: Invalid or corrupt kernel image. Halting system.", 0

;*******************************************
; Global Descriptor Table (GDT)
;*******************************************

gdt_data: 
	dd 0                ; null descriptor
	dd 0 

; gdt code:	            ; code descriptor
	dw 0FFFFh           ; limit low
	dw 0                ; base low
	db 0                ; base middle
	db 10011010b        ; access
	db 11001111b        ; granularity
	db 0                ; base high

; gdt data:	            ; data descriptor
	dw 0FFFFh           ; limit low (Same as code)10:56 AM 7/8/2007
	dw 0                ; base low
	db 0                ; base middle
	db 10010010b        ; access
	db 11001111b        ; granularity
	db 0                ; base high
	
end_of_gdt:
toc: 
	dw end_of_gdt - gdt_data - 1 	; limit (Size of GDT)
	dd gdt_data			; base of GDT

Stage3:
		
	;-------------------------------;
	;   Set registers				;
	;-------------------------------;

	mov	ax, DATA_DESC		; set data segments to data selector (0x10)
	mov	ds, ax
	mov	ss, ax
	mov	es, ax
	mov	esp, 9000h		; stack begins from 90000h

	call	ClrScr32

	call	EnablePaging

	
	

CopyImage:
  	 mov	eax, 113
  	 movzx	ebx, word [bpbBytesPerSector]
  	 mul	ebx
  	 mov	ebx, 4
  	 div	ebx
   	 cld
   	 mov    esi, IMAGE_RMODE_BASE
   	 mov	edi, IMAGE_PMODE_BASE
   	 mov	ecx, eax
   	 rep	movsd                   ; copy image to its protected mode address

TestImage:
  	  mov    ebx, [IMAGE_PMODE_BASE+60]
  	  add    ebx, IMAGE_PMODE_BASE    ; ebx now points to file sig (PE00)
  	  mov    esi, ebx
  	  mov    edi, ImageSig
  	  cmpsw
  	  je     EXECUTE
  	  mov	ebx, BadImage
  	  call	Puts32
  	  cli
  	  hlt

ImageSig db 'PE'

EXECUTE:
mov	ebx, ImageSig
  	  call	Puts32
  	  cli
  	  hlt
	;---------------------------------------;
	;   Execute Kernel
	;---------------------------------------;

    ; parse the programs header info structures to get its entry point

	add		ebx, 24
	mov		eax, [ebx]			; _IMAGE_FILE_HEADER is 20 bytes + size of sig (4 bytes)
	add		ebx, 20-4			; address of entry point
	mov		ebp, dword [ebx]		; get entry point offset in code section	
	add		ebx, 12				; image base is offset 8 bytes from entry point
	mov		eax, dword [ebx]		; add image base
	add		ebp, eax
	cli

	mov		eax, 0x2badb002			; multiboot specs say eax should be this
	mov		ebx, 0
	mov		edx, [ImageSize]
	


	push		dword boot_info
	call		ebp               	      ; Execute Kernel
	add		esp, 4

    	cli
	hlt

;-- header information format for PE files -------------------

;typedef struct _IMAGE_DOS_HEADER {  // DOS .EXE header
;    USHORT e_magic;         // Magic number (Should be MZ
;    USHORT e_cblp;          // Bytes on last page of file
;    USHORT e_cp;            // Pages in file
;    USHORT e_crlc;          // Relocations
;    USHORT e_cparhdr;       // Size of header in paragraphs
;    USHORT e_minalloc;      // Minimum extra paragraphs needed
;    USHORT e_maxalloc;      // Maximum extra paragraphs needed
;    USHORT e_ss;            // Initial (relative) SS value
;    USHORT e_sp;            // Initial SP value
;    USHORT e_csum;          // Checksum
;    USHORT e_ip;            // Initial IP value
;    USHORT e_cs;            // Initial (relative) CS value
;    USHORT e_lfarlc;        // File address of relocation table
;    USHORT e_ovno;          // Overlay number
;    USHORT e_res[4];        // Reserved words
;    USHORT e_oemid;         // OEM identifier (for e_oeminfo)
;    USHORT e_oeminfo;       // OEM information; e_oemid specific
;    USHORT e_res2[10];      // Reserved words
;    LONG   e_lfanew;        // File address of new exe header
;  } IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

;<<------ Real mode stub program -------->>

;<<------ Here is the file signiture, such as PE00 for NT --->>

;typedef struct _IMAGE_FILE_HEADER {
;    USHORT  Machine;
;    USHORT  NumberOfSections;
;    ULONG   TimeDateStamp;
;    ULONG   PointerToSymbolTable;
;    ULONG   NumberOfSymbols;
;    USHORT  SizeOfOptionalHeader;
;    USHORT  Characteristics;
;} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

;struct _IMAGE_OPTIONAL_HEADER {
;    //
;    // Standard fields.
;    //
;    USHORT  Magic;
;    UCHAR   MajorLinkerVersion;
;    UCHAR   MinorLinkerVersion;
;    ULONG   SizeOfCode;
;    ULONG   SizeOfInitializedData;
;    ULONG   SizeOfUninitializedData;
;    ULONG   AddressOfEntryPoint;			<< IMPORTANT!
;    ULONG   BaseOfCode;
;    ULONG   BaseOfData;
;    //
;    // NT additional fields.
;    //
;    ULONG   ImageBase;
;    ULONG   SectionAlignment;
;    ULONG   FileAlignment;
;    USHORT  MajorOperatingSystemVersion;
;    USHORT  MinorOperatingSystemVersion;
;    USHORT  MajorImageVersion;
;    USHORT  MinorImageVersion;
;    USHORT  MajorSubsystemVersion;
;    USHORT  MinorSubsystemVersion;
;    ULONG   Reserved1;
;    ULONG   SizeOfImage;
;    ULONG   SizeOfHeaders;
;    ULONG   CheckSum;
;    USHORT  Subsystem;
;    USHORT  DllCharacteristics;
;    ULONG   SizeOfStackReserve;
;    ULONG   SizeOfStackCommit;
;    ULONG   SizeOfHeapReserve;
;    ULONG   SizeOfHeapCommit;
;    ULONG   LoaderFlags;
;    ULONG   NumberOfRvaAndSizes;
;    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
;} IMAGE_OPTIONAL_HEADER, *PIMAGE_OPTIONAL_HEADER;

