#include "windef.h"
#include "HanOut.h"

#include "hanlib.h"      // 한글 라이브러리 0.17
#include "hancode\hancode.h"    // 한라프로 3.0 한글코드 변환 라이브러리 0.1
#include "hancode\hanin.h"      // "

#include "SkyInterface.h"
extern SkyMockInterface g_mockInterface;


//---------------------------------------------------------------------------
static void PutBitmap8x16_OVERWRITE(int x, int y, byte *ABitmap16)
{
    for (int i = 0; i < 16; i++) {
       // byte *p = (byte *)EngBitmap->ScanLine[i];
        //p[0] = ~ABitmap16[i];
		char p = ~ABitmap16[i];
		char c = 'a';

		if ((p & 0x80) != 0) { g_mockInterface.g_printInterface.sky_printf(" "); }
		else { g_mockInterface.g_printInterface.sky_printf("%c", c); }
		if ((p & 0x40) != 0) { g_mockInterface.g_printInterface.sky_printf(" "); }
		else { g_mockInterface.g_printInterface.sky_printf("%c", c); }
		if ((p & 0x20) != 0) { g_mockInterface.g_printInterface.sky_printf(" "); }
		else { g_mockInterface.g_printInterface.sky_printf("%c", c); }
		if ((p & 0x10) != 0) { g_mockInterface.g_printInterface.sky_printf(" "); }
		else { g_mockInterface.g_printInterface.sky_printf("%c", c); }
		if ((p & 0x08) != 0) { g_mockInterface.g_printInterface.sky_printf(" "); }
		else { g_mockInterface.g_printInterface.sky_printf("%c", c); }
		if ((p & 0x04) != 0) { g_mockInterface.g_printInterface.sky_printf(" "); }
		else { g_mockInterface.g_printInterface.sky_printf("%c", c); }
		if ((p & 0x02) != 0) { g_mockInterface.g_printInterface.sky_printf(" "); }
		else { g_mockInterface.g_printInterface.sky_printf("%c", c); }
		if ((p & 0x01) != 0) { g_mockInterface.g_printInterface.sky_printf(" "); }
		else { g_mockInterface.g_printInterface.sky_printf("%c", c); }

		g_mockInterface.g_printInterface.sky_printf("\n");
    }

	//g_mockInterface.g_printInterface.sky_printf("\n");
    //ACanvas->Draw(x, y, EngBitmap);
}


//---------------------------------------------------------------------------
static void PutBitmap16x16_OVERWRITE(int x, int y, byte *ABitmap32)
{
    for (int i = 0; i < 16; i++) {
        //byte *p = (byte *)HanBitmap->ScanLine[i];
       // p[0] = ~ABitmap32[2*i];
        //p[1] = ~ABitmap32[2*i+1];
		char p[2];
		
		char c = 'a';
		for (i = 0; i < 16; i++) {
		
			p[0] = ~ABitmap32[2 * i];
			p[1] = ~ABitmap32[2 * i + 1];
			
			if ((p[0] & 0x80) != 0) { g_mockInterface.g_printInterface.sky_printf(" "); }
			else { g_mockInterface.g_printInterface.sky_printf("%c", c);}
			if ((p[0] & 0x40) != 0) { g_mockInterface.g_printInterface.sky_printf(" "); }
			else { g_mockInterface.g_printInterface.sky_printf("%c", c);}
			if ((p[0] & 0x20) != 0) { g_mockInterface.g_printInterface.sky_printf(" "); }
			else { g_mockInterface.g_printInterface.sky_printf("%c", c);}
			if ((p[0] & 0x10) != 0) { g_mockInterface.g_printInterface.sky_printf(" "); }
			else { g_mockInterface.g_printInterface.sky_printf("%c", c);}
			if ((p[0] & 0x08) != 0) { g_mockInterface.g_printInterface.sky_printf(" "); }
			else { g_mockInterface.g_printInterface.sky_printf("%c", c);}
			if ((p[0] & 0x04) != 0) { g_mockInterface.g_printInterface.sky_printf(" "); }
			else { g_mockInterface.g_printInterface.sky_printf("%c", c);}
			if ((p[0] & 0x02) != 0) { g_mockInterface.g_printInterface.sky_printf(" ");}
			else { g_mockInterface.g_printInterface.sky_printf("%c", c);}
			if ((p[0] & 0x01) != 0) { g_mockInterface.g_printInterface.sky_printf(" ");}
			else { g_mockInterface.g_printInterface.sky_printf("%c", c);}


			if ((p[1] & 0x80) != 0) { g_mockInterface.g_printInterface.sky_printf(" "); }
			else { g_mockInterface.g_printInterface.sky_printf("%c", c); }
			if ((p[1] & 0x40) != 0) { g_mockInterface.g_printInterface.sky_printf(" "); }
			else { g_mockInterface.g_printInterface.sky_printf("%c", c); }
			if ((p[1] & 0x20) != 0) { g_mockInterface.g_printInterface.sky_printf(" "); }
			else { g_mockInterface.g_printInterface.sky_printf("%c", c); }
			if ((p[1] & 0x10) != 0) { g_mockInterface.g_printInterface.sky_printf(" "); }
			else { g_mockInterface.g_printInterface.sky_printf("%c", c); }
			if ((p[1] & 0x08) != 0) { g_mockInterface.g_printInterface.sky_printf(" "); }
			else { g_mockInterface.g_printInterface.sky_printf("%c", c); }
			if ((p[1] & 0x04) != 0) { g_mockInterface.g_printInterface.sky_printf(" "); }
			else { g_mockInterface.g_printInterface.sky_printf("%c", c); }
			if ((p[1] & 0x02) != 0) { g_mockInterface.g_printInterface.sky_printf(" "); }
			else { g_mockInterface.g_printInterface.sky_printf("%c", c); }
			if ((p[1] & 0x01) != 0) { g_mockInterface.g_printInterface.sky_printf(" "); }
			else { g_mockInterface.g_printInterface.sky_printf("%c", c); }

			g_mockInterface.g_printInterface.sky_printf("\n");
		}


    }

	//g_mockInterface.g_printInterface.sky_printf("\n");
    //ACanvas->Draw(x, y, HanBitmap);
}


//---------------------------------------------------------------------------
static void PutBitmap8x16_OVERLAP(int x, int y, byte *ABitmap16)
{   // 투명하게 그리기
    for (int i = 0; i < 16; i++) {
        //byte *p = (byte *)EngBitmap->ScanLine[i];
        //p[0] = ~ABitmap16[i];
    }
    //캔버스의 브러시에 설정된 색으로 8x16 크기만큼 지운다
    //ACanvas->FillRect(TRect(x, y, x + 8, y + 16));

    //EngBitmap->Transparent = true;
    //ACanvas->Draw(x, y, EngBitmap);
    //EngBitmap->Transparent = false; // 이거 꼭 해줘야 함
}
//---------------------------------------------------------------------------
static void PutBitmap16x16_OVERLAP(int x, int y, byte *ABitmap32)
{   // 투명하게 그리기
    for (int i = 0; i < 16; i++) {
        //byte *p = (byte *)HanBitmap->ScanLine[i];
       // p[0] = ~ABitmap32[2*i];
       // p[1] = ~ABitmap32[2*i+1];
    }
    // 캔버스의 브러시에 설정된 색으로 16x16 크기만큼 지운다
    //ACanvas->FillRect(TRect(x, y, x + 16, y + 16));

    //HanBitmap->Transparent = true;
    //ACanvas->Draw(x, y, HanBitmap);
    //HanBitmap->Transparent = false;  // 이거 꼭 해줘야 함
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void (*PutBitmap8x16)(int x, int y, byte *ABitmap32) = PutBitmap8x16_OVERWRITE;
void (*PutBitmap16x16)(int x, int y, byte *ABitmap32) = PutBitmap16x16_OVERWRITE;
//---------------------------------------------------------------------------
void SetOutputMode(int mode)
{
    switch (mode) {
    case OVERWRITE:
        PutBitmap8x16 = PutBitmap8x16_OVERWRITE;
        PutBitmap16x16 = PutBitmap16x16_OVERWRITE;
        break;
    case OVERLAP:
        PutBitmap8x16 = PutBitmap8x16_OVERLAP;
        PutBitmap16x16 = PutBitmap16x16_OVERLAP;
        break;
    }
}
/*
//---------------------------------------------------------------------------
void HanTextOut(int left, int top, byte *s)
{
    byte bitmap32[32], johab[2];

    int x = 0;
    for (unsigned i = 0; i < strlen((char*)s); ) {
        if (ishangul1st(s, i)) {
            // CP949 확장완성형 → 상용 조합형으로 변환
            code_cnvtcs(CODE_KSSM, CODE_CP949, johab, s[i], s[i+1]);

            codeidx_t codeidx;
            _code->getidx(&codeidx, johab[0], johab[1]);
            switch (codeidx.kind) {
            case CODEKIND_SPECIAL:
                PutBitmap16x16(left + x, top, pDefSpcFont->Spc[s[i] - 0xA1][s[i+1] - 0xA1]);
                break;
            case CODEKIND_HANGUL:
                CompleteHanChar(bitmap32, johab, pDefHanFont);
                PutBitmap16x16(left + x, top, bitmap32);
                break;
            case CODEKIND_HANJA:
                PutBitmap16x16(left + x, top, pDefHanjaFont->Hanja[s[i] - 0xCA][s[i+1] - 0xA1]);
                break;
            }
            x += 16, i += 2;
        } else {
            PutBitmap8x16(left + x, top, pDefEngFont->Eng[s[i]]);
            x += 8, i++;
        }
    }
}*/
//---------------------------------------------------------------------------

