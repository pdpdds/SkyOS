#include "wchar.h"
#include "stdint.h"

size_t wcstombs(char *mbstr, const wchar_t *wcstr, size_t count)
{
    int written, bytes;
    uint16_t w;

    for (written = 0; *wcstr && written < (int)count;)
    {
	w = *wcstr;
	
	if (w <= 0x7f)
	    bytes = 1;
	else if (w <= 0x7ff)
	    bytes = 2;
	else if (w <= 0xffff)
	    bytes = 3;
	else
	{
	    return 0;
	}

	if (written + bytes > (int)count)
	    return written;

	if (mbstr != 0)
	    switch (bytes)
	    {
	    case 1:
		mbstr[0] = w & 0x7f;
		break;

	    case 2:
		mbstr[1] = 0x80 | (w & 0x3f);
		w >>= 6;
		mbstr[0] = 0xC0 | (w & 0x1f);
		break;

	    case 3:
		mbstr[2] = 0x80 | (w & 0x3f);
		w >>= 6;
		mbstr[1] = 0x80 | (w & 0x3f);
		w >>= 6;
		mbstr[0] = 0xE0 | (w & 0x0f);
		break;
	    }
	
	written += bytes;
	if (mbstr != 0)
	    mbstr += bytes;
	wcstr++;
    }

    /*if (mbstr != 0)
	*mbstr = 0;*/
    return written;
}

