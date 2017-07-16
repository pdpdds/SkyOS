#include <ctype.h>

unsigned short _ctype[257] = 
{
  0,                      // -1 EOF
  CT_CTL,               // 00 (NUL)
  CT_CTL,               // 01 (SOH)
  CT_CTL,               // 02 (STX)
  CT_CTL,               // 03 (ETX)
  CT_CTL,               // 04 (EOT)
  CT_CTL,               // 05 (ENQ)
  CT_CTL,               // 06 (ACK)
  CT_CTL,               // 07 (BEL)
  CT_CTL,               // 08 (BS)
  _SPACE+CT_CTL,        // 09 (HT)
  _SPACE+CT_CTL,        // 0A (LF)
  _SPACE+CT_CTL,        // 0B (VT)
  _SPACE+CT_CTL,        // 0C (FF)
  _SPACE+CT_CTL,        // 0D (CR)
  CT_CTL,               // 0E (SI)
  CT_CTL,               // 0F (SO)
  CT_CTL,               // 10 (DLE)
  CT_CTL,               // 11 (DC1)
  CT_CTL,               // 12 (DC2)
  CT_CTL,               // 13 (DC3)
  CT_CTL,               // 14 (DC4)
  CT_CTL,               // 15 (NAK)
  CT_CTL,               // 16 (SYN)
  CT_CTL,               // 17 (ETB)
  CT_CTL,               // 18 (CAN)
  CT_CTL,               // 19 (EM)
  CT_CTL,               // 1A (SUB)
  CT_CTL,               // 1B (ESC)
  CT_CTL,               // 1C (FS)
  CT_CTL,               // 1D (GS)
  CT_CTL,               // 1E (RS)
  CT_CTL,               // 1F (US)
  _SPACE+_BLANK,          // 20 SPACE
  _PUNCT,                 // 21 !
  _PUNCT,                 // 22 "
  _PUNCT,                 // 23 #
  _PUNCT,                 // 24 $
  _PUNCT,                 // 25 %
  _PUNCT,                 // 26 &
  _PUNCT,                 // 27 '
  _PUNCT,                 // 28 (
  _PUNCT,                 // 29 )
  _PUNCT,                 // 2A *
  _PUNCT,                 // 2B +
  _PUNCT,                 // 2C ,
  _PUNCT,                 // 2D -
  _PUNCT,                 // 2E .
  _PUNCT,                 // 2F /
  _DIGIT+_HEX,            // 30 0
  _DIGIT+_HEX,            // 31 1
  _DIGIT+_HEX,            // 32 2
  _DIGIT+_HEX,            // 33 3
  _DIGIT+_HEX,            // 34 4
  _DIGIT+_HEX,            // 35 5
  _DIGIT+_HEX,            // 36 6
  _DIGIT+_HEX,            // 37 7
  _DIGIT+_HEX,            // 38 8
  _DIGIT+_HEX,            // 39 9
  _PUNCT,                 // 3A :
  _PUNCT,                 // 3B ;
  _PUNCT,                 // 3C <
  _PUNCT,                 // 3D =
  _PUNCT,                 // 3E >
  _PUNCT,                 // 3F ?
  _PUNCT,                 // 40 @
  _UPPER+_HEX,            // 41 A
  _UPPER+_HEX,            // 42 B
  _UPPER+_HEX,            // 43 C
  _UPPER+_HEX,            // 44 D
  _UPPER+_HEX,            // 45 E
  _UPPER+_HEX,            // 46 F
  _UPPER,                 // 47 G
  _UPPER,                 // 48 H
  _UPPER,                 // 49 I
  _UPPER,                 // 4A J
  _UPPER,                 // 4B K
  _UPPER,                 // 4C L
  _UPPER,                 // 4D M
  _UPPER,                 // 4E N
  _UPPER,                 // 4F O
  _UPPER,                 // 50 P
  _UPPER,                 // 51 Q
  _UPPER,                 // 52 R
  _UPPER,                 // 53 S
  _UPPER,                 // 54 T
  _UPPER,                 // 55 U
  _UPPER,                 // 56 V
  _UPPER,                 // 57 W
  _UPPER,                 // 58 X
  _UPPER,                 // 59 Y
  _UPPER,                 // 5A Z
  _PUNCT,                 // 5B [
  _PUNCT,                 // 5C \ 
  _PUNCT,                 // 5D ]
  _PUNCT,                 // 5E ^
  _PUNCT,                 // 5F _
  _PUNCT,                 // 60 `
  _LOWER+_HEX,            // 61 a
  _LOWER+_HEX,            // 62 b
  _LOWER+_HEX,            // 63 c
  _LOWER+_HEX,            // 64 d
  _LOWER+_HEX,            // 65 e
  _LOWER+_HEX,            // 66 f
  _LOWER,                 // 67 g
  _LOWER,                 // 68 h
  _LOWER,                 // 69 i
  _LOWER,                 // 6A j
  _LOWER,                 // 6B k
  _LOWER,                 // 6C l
  _LOWER,                 // 6D m
  _LOWER,                 // 6E n
  _LOWER,                 // 6F o
  _LOWER,                 // 70 p
  _LOWER,                 // 71 q
  _LOWER,                 // 72 r
  _LOWER,                 // 73 s
  _LOWER,                 // 74 t
  _LOWER,                 // 75 u
  _LOWER,                 // 76 v
  _LOWER,                 // 77 w
  _LOWER,                 // 78 x
  _LOWER,                 // 79 y
  _LOWER,                 // 7A z
  _PUNCT,                 // 7B {
  _PUNCT,                 // 7C |
  _PUNCT,                 // 7D }
  _PUNCT,                 // 7E ~
  CT_CTL,               // 7F (DEL)
  // and the rest are 0...
};

unsigned short *_pctype = _ctype + 1; // pointer to table for char's

int _isctype(int c, int mask)
{
  if (((unsigned) (c + 1)) <= 256)
    return _pctype[c] & mask;
  else
    return 0;
}

int isalpha(int c)
{
  return _pctype[c] & (_UPPER | _LOWER);
}

int isupper(int c)
{
  return _pctype[c] & _UPPER;
}

int islower(int c)
{
  return _pctype[c] & _LOWER;
}

int isdigit(int c)
{
  return _pctype[c] & _DIGIT;
}

int isxdigit(int c)
{
  return _pctype[c] & _HEX;
}

int isspace(int c)
{
  return _pctype[c] & _SPACE;
}

int ispunct(int c)
{
  return _pctype[c] & _PUNCT;
}

int isalnum(int c)
{
  return _pctype[c] & (_UPPER | _LOWER | _DIGIT);
}

int isprint(int c)
{
  return _pctype[c] & (_BLANK | _PUNCT | _UPPER | _LOWER | _DIGIT);
}

int isgraph(int c)
{
  return _pctype[c] & (_PUNCT | _UPPER | _LOWER | _DIGIT);
}

int iscntrl(int c)
{
  return _pctype[c] & CT_CTL;
}

int isleadbyte(int c)
{
  return _pctype[c] & _LEADBYTE;
}

int toupper(int c)
{
  if (_pctype[c] & _LOWER)
    return c - ('a' - 'A');
  else
    return c;
}

int tolower(int c)
{
  if (_pctype[c] & _UPPER)
    return c + ('a' - 'A');
  else
    return c;
}

