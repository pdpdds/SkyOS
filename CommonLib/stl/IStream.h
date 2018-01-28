#ifndef __ISTREAM_H__
#define __ISTREAM_H__

#include "OStream.h"

namespace std
{
//---------------IStream:------------------------

	const int BUFFSIZE = 256;

	class IStream
	{
		public:
			IStream& operator >> (char *cp);
			IStream& operator >> (char &c);
			IStream& operator >> (unsigned char *ucp);
			IStream& operator >> (unsigned char &uc);

		private:
			char buffer[BUFFSIZE];

			unsigned int getString();
			void getWord(char *str1, char *p1);
	};

	extern IStream cin;
}

#endif

