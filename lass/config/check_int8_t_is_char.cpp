#include <stdint.h>

template <typename T> struct Test;
template <> struct Test<char> { typedef char type; };

int main()
{
	Test<int8_t>::type a = 5;
	return 0;
}