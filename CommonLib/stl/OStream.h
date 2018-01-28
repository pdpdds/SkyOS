#ifndef __OSTREAM_H__
#define __OSTREAM_H__

namespace std
{
//---------------OStream:------------------------

	const char endl = '\n';

	class OStream
	{
		public:
			OStream& operator<<(char *cp);
			OStream& operator<<(char c);
			OStream& operator<<(unsigned char *cq);
			OStream& operator<<(unsigned char c1);

		private:
	};

	extern OStream cout;
}

#endif

