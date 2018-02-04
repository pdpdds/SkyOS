//stdlib.cpp
#include "OStream.h"

extern void printf(const char* str, ...);

namespace std
{
	OStream cout;		//OStream cout

	OStream& OStream::operator<<(char *cp)
	{
		printf(cp);
		return *this;
	}	

	OStream& OStream::operator<<(const char c)
	{
		printf("%c", c);
		return *this;
	}

	OStream& OStream::operator<<(int value)
	{
		printf("%d", value);
		return *this;
	}

	OStream& OStream::operator<<(unsigned char *cq)
	{
		printf((char*)cq);		
		return *this;
	}

	OStream& OStream::operator<<(unsigned char c1)
	{
		printf("%c", (char)c1);
		return *this;
	}
}

