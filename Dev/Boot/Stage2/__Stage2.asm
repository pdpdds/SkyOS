
;*******************************************************
;
;	Stage2.asm
;		Stage2 Bootloader
;
;	OS Development Series
;*******************************************************

bits	16

org 0x500

jmp	main				; go to start

;*******************************************************
;	Preprocessor directives
;*******************************************************

%include "stdio.inc"			; basic i/o routines
%include "Gdt.inc"			; Gdt routines
%include "A20.inc"			; A20 enabling
%include "Fat12.inc"			; FAT12 driver. Kinda :)
%include "Common.inc"
%include "bootinfo.inc"
%include "memory.inc"

;*******************************************************
;	Data Section
;*******************************************************

LoadingMsg db 0x0D, 0x0A, "Searching for Operating System...", 0x00
msgFailure db 0x0D, 0x0A, "*** FATAL: Missing or corrupt KRNL32.EXE. Press Any Key to Reboot.", 0x0D, 0x0A, 0x0A, 0x00

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
	
;*************************************************************************
;Data starts here
	KernelSize      dw 95	
	msgLoading  	db 0x0D, 0x0A, "Loading Kernel ",  0x00
	msgCRLF     	db 0x0D, 0x0A, 0x00
	msgProgress 	db ".", 0x00	
	msgRelocate	db 0x0D, 0x0A, "Relocating kernel code...",0x00
	msgSwitch   	db 0x0D, 0x0A, "Switching to kernel...",0x00
	ShowProgress	db 0

	OEM_ID		      db "ACE   OS"
	BytesPerSector        dw 0x0200
	SectorsPerCluster     db 0x01
	ReservedSectors       dw 0x0001
	TotalFATs             db 0x02
	MaxRootEntries        dw 0x00E0
	TotalSectorsSmall     dw 0x0B40
	MediaDescriptor       db 0xF0
	SectorsPerFAT         dw 0x0009
	SectorsPerTrack       dw 0x0012
	NumHeads              dw 0x0002
	HiddenSectors         dd 0x00000000
	TotalSectorsLarge     dd 0x00000000
	DriveNumber           db 0x00
	Flags                 db 0x00
	Signature             db 0x29
	VolumeID              dd 0xFFFFFFFF
	VolumeLabel           db "ACE    BOOT"
	SystemID              db "FAT12   "	
	
	KernelAddress 	EQU 0x19000
KernelSegment 	EQU 0x3000
KernelLow	EQU 0x9000
KernelMedium	EQU 0x1

OSLoaderAddress EQU 0x800
OSLoaderSegment EQU 0x80

RootDirOffset	EQU 0xC800
FATOffset		EQU 0x7800

main:

	;-------------------------------;
	;   Setup segments and stack	;
	;-------------------------------;

	cli	                   ; clear interrupts
	xor		ax, ax             ; null segments
	mov		ds, ax
	mov		es, ax
	mov		ax, 0x0000         ; stack begins at 0x9000-0xffff
	mov		ss, ax
	mov		sp, 0xFFFF
	sti	                   ; enable interrupts

	mov     	[boot_info+multiboot_info.bootDevice], dl

	call		_EnableA20
	call		InstallGDT
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
	mov		ds, ax
	mov		di, 0x1000
	call		BiosGetMemoryMap

	LOAD_ROOT:
	;compute size of root directory and store in 멵x?
	xor cx, cx
	xor dx, dx
	mov ax, 0x0020                          ; 32 byte directory entry
	mul WORD [MaxRootEntries]               ; total size of directory
	div WORD [BytesPerSector]               ; sectors used by directory
	xchg ax, cx
	;compute location of root directory and store in 멲x?
	mov al, BYTE [TotalFATs]                ; number of FATs
	mul WORD [SectorsPerFAT]                ; sectors used by FATs
	add ax, WORD [ReservedSectors]          ; adjust for bootsector
	mov WORD [datasector], ax               ; base of root directory
	add WORD [datasector], cx
	
	;read root directory into memory 
	mov bx, RootDirOffset			; copy root dir above code
	call ReadSectorsA

	;browse root directory for binary image
	mov cx, WORD [MaxRootEntries]           ; load loop counter
	mov di, RootDirOffset			; locate first root entry
.LOOP:
	push cx
	mov cx, 0x000B                          ; eleven character name
	mov si, ImageName                       ; image name to find
	push di
	rep cmpsb                                       ; test for entry match
	pop di

	je LOAD_FAT
	pop cx
	add di, 0x0020                          ; queue next directory entry
	loop .LOOP
	jmp     FAILURE

LOAD_FAT:
	mov dx, WORD [di + 0x001A]
	mov WORD [cluster], dx                  ; file뭩 first cluster
	;compute size of FAT and store in 멵x?
	xor ax, ax
	mov al, BYTE [TotalFATs]                ; number of FATs
	mul WORD [SectorsPerFAT]                ; sectors used by FATs
	mov cx, ax
	;compute location of FAT and store in 멲x?
	mov ax, WORD [ReservedSectors]          ; adjust for bootsector
	;read FAT into memory 
	mov bx, FATOffset			     ; copy FAT above bootcode
	call ReadSectorsA

	
	;------------------Reading Kernel DLL into memory------------------
	mov ax, KernelSegment			; destination of image CS
	mov es, ax
	mov bx, 0x0000                          ; destination for image IP
	push bx
	mov byte [ShowProgress],1
LOAD_IMAGE:
	mov ax, WORD [cluster]                  ; cluster to read
	pop bx                                  ; buffer to read into
	call ClusterLBAA                         ; convert cluster to LBA
	xor cx, cx
	mov cl, BYTE [SectorsPerCluster]        ; sectors to read
	call ReadSectorsA

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
	mov bx, FATOffset			     		; location of FAT in memory
	add bx, cx                              ; index into FAT
	mov dx, WORD [bx]                       ; read two bytes from FAT
	test ax, 0x0001
	jnz .ODD_CLUSTER
.EVEN_CLUSTER:
	and dx, 0000111111111111b               ; take low twelve bits
	jmp .DONEB
.ODD_CLUSTER:
	shr dx, 0x0004                          ; take high twelve bits
.DONEB
	mov WORD [cluster], dx                  ; store new cluster
	cmp dx, 0x0FF0                          ; test for end of file
	jb LOAD_IMAGE

	mov si, msgRelocate
	call DisplayMessage
RELOCATE:	
	cld
	cli
	mov ax, KernelSegment
	mov bx, [KernelSize]
	.Loop:
		mov es, ax			;Destination
		mov edi,	0
		mov ds, ax			;Source
		mov esi, 0x2c0
		mov ecx, 1024			; one segment
		rep movsb
		.Debug
		add ax, 64
		cmp bx, 0		; Check for next segment 
		dec bx
	jne .Loop
		
	je		EnterStage3
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
	mov	eax, cr0                   ; set bit 0 in cr0--enter pmode
	or	eax, 1
	mov	cr0, eax

	jmp	CODE_DESC:Stage3                ; far jump to fix CS. Remember that the code selector is 0x8!

	; Note: Do NOT re-enable interrupts! Doing so will triple fault!
	; We will fix this in Stage 3.
	FAILURE:
	mov si, msgFailure
	call DisplayMessage
	mov ah, 0x00
	int     0x16                                ; await keypress
	int     0x19                                ; warm boot computer

;*************************************************************************
; PROCEDURE DisplayMessage
; display ASCIIZ string at ds:si via BIOS
;*************************************************************************
DisplayMessage:
	lodsb                                       ; load next character
	or al, al                              ; test for NUL character
	jz .DONEA
	mov ah, 0x0E                            ; BIOS teletype
	mov bh, 0x00                            ; display page 0
	mov bl, 0x07                            ; text attribute
	int 0x10                                ; invoke BIOS
	jmp DisplayMessage
.DONEA
	ret
	;*************************************************************************
; PROCEDURE ReadSectorsA
; reads 멵x?sectors from disk starting at 멲x?into
;memory location 멷s:bx?
;*************************************************************************
ReadSectorsA:
.MAIN:
	mov     di, 0x0005                          ; five retries for error
.SECTORLOOP:
	push    ax
	push    bx
	push    cx
	call    LBACHSA
	mov     ah, 0x02                            ; BIOS read sector
	mov     al, 0x01                            ; read one sector
	mov     ch, BYTE [absoluteTrack]            ; track
	mov     cl, BYTE [absoluteSector]           ; sector
	mov     dh, BYTE [absoluteHead]             ; head
	mov     dl, BYTE [DriveNumber]              ; drive
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
.SUCCESS:
	cmp byte [ShowProgress],0
	je .PROGRESSDONE
	mov     si, msgProgress
	call    DisplayMessage
	.PROGRESSDONE:
	pop     cx
	pop     bx
	pop     ax
	add bx, WORD [BytesPerSector]           ; queue next buffer
	inc     ax                                  ; queue next sector
loop    .MAIN                               ; read next sector
	ret

;*************************************************************************
; PROCEDURE ClusterLBA
; convert FAT cluster into LBA addressing scheme
; LBA = (cluster - 2) * sectors per cluster
;*************************************************************************
ClusterLBAA:
	sub     ax, 0x0002                          ; zero base cluster number
	xor     cx, cx
	mov     cl, BYTE [SectorsPerCluster]        ; convert byte to word
	mul     cx
	add     ax, WORD [datasector]               ; base data sector
	ret

;*************************************************************************
; PROCEDURE LBACHS
; convert 멲x?LBA addressing scheme to CHS addressing scheme
; absolute sector = (logical sector / sectors per track) + 1
; absolute head   = (logical sector / sectors per track) MOD number of heads
; absolute track  = logical sector / (sectors per track * number of heads)
;*************************************************************************
LBACHSA:
	xor     dx, dx                              ; prepare dx:ax for operation
	div     WORD [SectorsPerTrack]              ; calculate
	inc     dl                                  ; adjust for sector 0
	mov     BYTE [absoluteSector], dl
	xor     dx, dx                              ; prepare dx:ax for operation
	div     WORD [NumHeads]                     ; calculate
	mov     BYTE [absoluteHead], dl
	mov     BYTE [absoluteTrack], al
	ret

;******************************************************
;	ENTRY POINT FOR STAGE 3
;******************************************************

bits 32

%include "Paging.inc"

BadImage db "*** FATAL: Invalid or corrupt kernel image. Halting system.", 0

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
  	 mov	eax,  200
  	 movzx	ebx, word [bpbBytesPerSector]
  	 mul	ebx
  	 mov	ebx, 4
  	 div	ebx
   	 cld
   	 mov    esi, KernelSegment
   	 mov	edi, IMAGE_PMODE_BASE
   	 mov	ecx, eax
   	 rep	movsd                   ; copy image to its protected mode address
	


ImageSig db 'PE'

EXECUTE:

	;---------------------------------------;
	;   Execute Kernel
	;---------------------------------------;

    ; parse the programs header info structures to get its entry point
mov    ebx, [IMAGE_PMODE_BASE+60]
  	  add    ebx, IMAGE_PMODE_BASE    ; ebx now points to file sig (PE00)
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
	mov		edx, 200

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

