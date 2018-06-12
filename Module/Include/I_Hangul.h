#pragma once

//32비트 한글 그래픽 엔진
class I_Hangul
{
public:
	virtual bool Initialize() = 0; //한글폰트 로드 및 초기화
	virtual void PutFonts(char *vram, int xsize, int x, int y, unsigned int, unsigned char *s) = 0; //제공된 그래픽 선형 버퍼에 문자열을 출력한다. 
};