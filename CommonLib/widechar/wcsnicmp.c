#include "wchar.h"

#define isupper(ch)	((ch) >= L'A' && (ch) <= L'Z')
#define tolower(ch)	(isupper(ch) ? (ch) - L'A' + L'a' : (ch))

int
_wcsnicmp(const wchar_t *s1, const wchar_t *s2, size_t n)
{

  if (n == 0)
    return 0;
  do {
    if (tolower(*s1) != tolower(*s2))
      return *s1 - *s2;
	s2++;
    if (*s1++ == 0)
      break;
  } while (--n != 0);
  return 0;
}
