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
			ostream& operator<<(unsigned char *cq);
			ostream& operator<<(unsigned char c1);

		private:
	};

	extern ostream cout;
}

#endif

