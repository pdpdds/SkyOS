//stdlib.cpp
#include "OStream.h"

extern void printf(const char* str, ...);

namespace std
{
	ostream cout;		//OStream cout

	ostream& ostream::operator<<(char *cp)
	{
		printf(cp);
		return *this;
	}

	ostream& ostream::operator<<(const char *cp)
	{
		printf("%s", cp);
		return *this;
	}

	ostream& ostream::operator<<(const char c)
	{
		printf("%c", c);
		return *this;
	}

	ostream& ostream::operator<<(int value)
	{
		printf("%d", value);
		return *this;
	}

	ostream& ostream::operator<<(unsigned long value)
	{
		printf("%d", value);
		return *this;
	}

	ostream& ostream::operator<<(unsigned char *cq)
	{
		printf((char*)cq);		
		return *this;
	}

	ostream& ostream::operator<<(unsigned char c1)
	{
		printf("%c", (char)c1);
		return *this;
	}
}

