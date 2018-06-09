//------------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma warn -8004
#endif

#include "hanlib.h"
#include "Johab.h"
#include "CP949Table.h"
#include "KS2JohabTable.h"
#include "CompleteHan.h"
#include "CompleteKS.h"
//------------------------------------------------------------------------------
byte _HanFont_Hangul[2350][32];			// KS완성형2350
byte _HanFont_Unicode[17304 + 67][32];	// 유니코드 17304 = 128(0~127) + 128(128~255, 사용안함) + 11172 + 988 + 4888 + 첫가끝67
//------------------------------------------------------------------------------
void CompleteKS2350(THanFont *AHanFont)
{
	int i;

	for (i = 0; i < 2350; i++) {
    	THangul _Hangul;
		bool flag = true;
	    byte bitmap32[32];
		int F1, F2, F3, F3B, F2B, F1B;

		_Hangul.HanByte.Byte0 = (byte)(_KS2JohabTable[i] >> 8);
	    _Hangul.HanByte.Byte1 = (byte)(_KS2JohabTable[i]);

	    F1 = _CodeTable[0][_Hangul.HanCode.F1];
	    F2 = _CodeTable[1][_Hangul.HanCode.F2];
	    F3 = _CodeTable[2][_Hangul.HanCode.F3];

	    F3B = AHanFont->pF3B[F2];
	    F2B = AHanFont->pF2B[F1 * 2 + (F3 != 0)];
	    F1B = AHanFont->pF1B[F2 * 2 + (F3 != 0)];

	    if (F1) HanComplete(true, bitmap32, AHanFont->F1[F1B][F1], 32), flag = false;
	    if (F2) HanComplete(flag, bitmap32, AHanFont->F2[F2B][F2], 32), flag = false;
	    if (F3)	HanComplete(flag, bitmap32, AHanFont->F3[F3B][F3], 32), flag = false;

		memcpy(_HanFont_Hangul[i], bitmap32, 32);
	}
}
//------------------------------------------------------------------------------
void CompleteExtKS11172(THanFont *AHanFont)
{
	int i;

	for (i = 256; i < 17304 + 67; i++) {	// 0.55 수정
		if (CP949CodeTable[i].CodeType == CODETABLE_HANGUL) {
			THangul _Hangul;
			bool flag = true;
		    byte bitmap32[32];
			int F1, F2, F3, F3B, F2B, F1B;

			_Hangul.HanByte.Byte0 = (byte)(CP949CodeTable[i].KSSM >> 8);
			_Hangul.HanByte.Byte1 = (byte)(CP949CodeTable[i].KSSM);

			F1 = _CodeTable[0][_Hangul.HanCode.F1];
			F2 = _CodeTable[1][_Hangul.HanCode.F2];
			F3 = _CodeTable[2][_Hangul.HanCode.F3];

			F3B = AHanFont->pF3B[F2];
			F2B = AHanFont->pF2B[F1 * 2 + (F3 != 0)];
			F1B = AHanFont->pF1B[F2 * 2 + (F3 != 0)];

			if (F1) HanComplete(true, bitmap32, AHanFont->F1[F1B][F1], 32), flag = false;
			if (F2) HanComplete(flag, bitmap32, AHanFont->F2[F2B][F2], 32), flag = false;
			if (F3)	HanComplete(flag, bitmap32, AHanFont->F3[F3B][F3], 32), flag = false;

			memcpy(_HanFont_Unicode[i], bitmap32, 32);
		}
	}
    for (i = 0; i < 51; i++) 
		memcpy(_HanFont_Unicode[912 + 67 + i], AHanFont->pHangulJamo[i], 32);

	// 0.55 추가
    for (i = 0; i < 19; i++) 
		memcpy(_HanFont_Unicode[427 + i], AHanFont->F1[0][AHanFont->F_SKIP + i], 32);
    for (i = 0; i < 21; i++) 
		memcpy(_HanFont_Unicode[427 + 19 + i], AHanFont->F2[0][AHanFont->F_SKIP + i], 32);
    for (i = 0; i < 27; i++) 
		memcpy(_HanFont_Unicode[427 + (19+21) + i], AHanFont->F3[0][AHanFont->F_SKIP + i], 32);
}
//------------------------------------------------------------------------------
