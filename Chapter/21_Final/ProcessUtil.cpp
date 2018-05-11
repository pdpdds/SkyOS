#include "SkyOS.h"

//32비트 PE파일 이미지 유효성 검사
bool ValidatePEImage(void* image) 
{
    IMAGE_DOS_HEADER* dosHeader = 0;
    IMAGE_NT_HEADERS* ntHeaders = 0;
    
    dosHeader = (IMAGE_DOS_HEADER*)image;
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)             
		return false;
    
	if (dosHeader->e_lfanew == 0)
		return false;
    
    //NT Header 체크
    ntHeaders = (IMAGE_NT_HEADERS*)(dosHeader->e_lfanew + (uint32_t)image);
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)            
		return false;    

    /* only supporting for i386 archs */
    if (ntHeaders->FileHeader.Machine != IMAGE_FILE_MACHINE_I386)             
		return false;    

    /* only support 32 bit executable images */
    if (! (ntHeaders->FileHeader.Characteristics & (IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_32BIT_MACHINE)))             
		return false;
    
    /*
            Note: 1st 4 MB remains idenitity mapped as kernel pages as it contains
            kernel stack and page directory. If you want to support loading below 1MB,
            make sure to move these into kernel land
    */

	//로드되는 프로세스의 베이스 주소는 0x00400000다. 
	//비쥬얼 스튜디오에서 속성=> 링커 => 고급의 기준주소 항목에서 확인 가능하다
    if ( (ntHeaders->OptionalHeader.ImageBase < 0x400000) || (ntHeaders->OptionalHeader.ImageBase > 0x80000000))
		return false;
    
    /* only support 32 bit optional header format */
    if (ntHeaders->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC) 
            return false;

//유효한 32비트 PE 파일이다.
	return true;
}

void PrintCurrentTime()
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	char buffer[256];
	sprintf(buffer, "Current Time : %d/%d/%d %d:%d:%d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	SkyConsole::Print("%s", buffer);
}