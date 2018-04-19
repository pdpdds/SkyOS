#ifndef __ISTREAM_H__
#define __ISTREAM_H__

#include "OStream.h"

namespace std
{
//---------------IStream:------------------------

	const int BUFFSIZE = 256;

	class istream
	{
		public:
			istream & operator >> (char *cp);
			istream& operator >> (char &c);
			istream& operator >> (unsigned char *ucp);
			istream& operator >> (unsigned char &uc);

			unsigned int getString();
			void getWord(char *str1, char *p1);

		private:
			char buffer[BUFFSIZE];			
	};

	extern istream cin;
}

#endif

