#include <string.h>
int main()
{
	char* ptr = strdup("a");
	__asm__ __volatile__(
		"ldrexb r0, [%[ptr]]\n\t"
		"strexb r1, r0, [%[ptr]]\n\t"
		: 
		: [ptr]"r"(ptr)
		: "r0", "r1", "cc", "memory");
}
