#include <string.h>
int main()
{
	char* ptr = strdup("abcdefgh");
	__asm__ __volatile__(
		"ldrexd r0, r1, [%[ptr]]\n\t"
		"strexd r2, r0, r1, [%[ptr]]\n\t"
		: 
		: [ptr]"r"(ptr)
		: "r0", "r1", "r2", "cc", "memory");
}
