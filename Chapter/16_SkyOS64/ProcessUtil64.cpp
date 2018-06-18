#include <string.h>
#include "PEImage64.h"
#include "ProcessUtil64.h"
#include "SkyConsole.h"
#include "defines.h"

//64비트 PE파일 이미지 유효성 검사
bool ValidatePEImage64(void* image) 
{
    IMAGE_DOS_HEADER* dosHeader = 0;
	IMAGE_NT_HEADERS64* ntHeaders = 0;
    
    dosHeader = (IMAGE_DOS_HEADER*)image;
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)             
		return false;
    
	if (dosHeader->e_lfanew == 0)
		return false;
	
    //NT Header 체크
    ntHeaders = (IMAGE_NT_HEADERS64*)(dosHeader->e_lfanew + (uint32_t)image);
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)            
		return false;    
	
    if (ntHeaders->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64)
		return false;    
	
    /* only support 64 bit executable images */
    if (! (ntHeaders->FileHeader.Characteristics & (IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_MACHINE_AMD64)))
		return false;
	
	//비쥬얼 스튜디오에서 속성=> 링커 => 고급의 기준주소 항목에서 확인 가능하다
    //if ( (ntHeaders->OptionalHeader.ImageBase < 0x200000) || (ntHeaders->OptionalHeader.ImageBase > 0x80000000))
		//return false;

	
    /* only support 64 bit optional header format */
    if (ntHeaders->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
            return false;
	
//유효한 64비트 PE 파일이다.
	return true;
}

uint32_t FindKernel64Entry(const char* szFileName, char* buf, uint32_t& imageBase)
{
	if (!ValidatePEImage64(buf)) {
		SkyConsole::Print("Invalid PE Format!! %s\n", szFileName);
		return 0;
	}

	IMAGE_DOS_HEADER* dosHeader = 0;
	IMAGE_NT_HEADERS64* ntHeaders = 0;

	SkyConsole::Print("Valid PE Format %s\n", szFileName);

	dosHeader = (IMAGE_DOS_HEADER*)buf;
	ntHeaders = (IMAGE_NT_HEADERS64*)(dosHeader->e_lfanew + (uint32_t)buf);
	SkyConsole::Print("sizeofcode 0x%x\n", ntHeaders->OptionalHeader.Magic);

	uint32_t entryPoint = (uint32_t)ntHeaders->OptionalHeader.AddressOfEntryPoint + ntHeaders->OptionalHeader.ImageBase;
	imageBase = ntHeaders->OptionalHeader.ImageBase;
	return 	entryPoint;
}

