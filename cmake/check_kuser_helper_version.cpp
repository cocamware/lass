#ifdef __ARM_EABI__

#include <stdint.h>
#define __kuser_helper_version (*(int32_t *)0xffff0ffc)
int main()
{
	return __kuser_helper_version;
}

#endif
