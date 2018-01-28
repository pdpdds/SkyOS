//stdlib.cpp
#include "OStream.h"
#include "console.h"

extern Console console;

namespace std
{
	OStream cout;		//OStream cout

	OStream& OStream::operator<<(char *cp)
	{
		console.Write(cp) ;
		return *this;
	}

	OStream& OStream::operator<<(char c)
	{
		console.WriteChar(c) ;
		return *this;
	}

	OStream& OStream::operator<<(unsigned char *cq)
	{
		console.Write((char*)cq);
		return *this;
	}

	OStream& OStream::operator<<(unsigned char c1)
	{
		console.WriteChar(c1) ;
		return *this;
	}
}

