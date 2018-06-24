#ifndef __OSTREAM_H__
#define __OSTREAM_H__

namespace std
{
//---------------OStream:------------------------

	const char endl = '\n';

	class ostream
	{
		public:
			ostream & operator<<(char *cp);
			ostream& operator<<(char c);
			ostream& operator<<(int value);
			ostream& operator<<(unsigned long value);
			ostream& operator<<(unsigned char *cq);
			ostream& operator<<(unsigned char c1);
			ostream& ostream::operator<<(const char *cp);

		private:
	};

	extern ostream cout;
}

#endif

