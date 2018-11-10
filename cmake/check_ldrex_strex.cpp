#include <string.h>
int main()
{
	char* ptr = strdup("ab");
	__asm__ __volatile__(
		"ldrex r0, [%[ptr]]\n\t"
		"strex r1, r0, [%[ptr]]\n\t"
		: 
		: [ptr]"r"(ptr)
		: "r0", "r1", "cc", "memory");
}
