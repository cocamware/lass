#include <string.h>
int main()
{
	char* ptr = strdup("abcdefgh");
	__asm__ __volatile__(
		"ldrexh r0, [%[ptr]]\n\t"
		"strexh r2, r0, [%[ptr]]\n\t"
		: 
		: [ptr]"r"(ptr)
		: "r0", "r1", "r2", "cc", "memory");
}
