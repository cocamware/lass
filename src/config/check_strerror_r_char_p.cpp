#include <errno.h>
#include <string.h>

int main()
{
	const int errnum = 0;
        const int bufferLength = 256;
        char buffer[bufferLength + 1];
        errno = 0;
	const char* result = ::strerror_r(errnum, buffer, bufferLength);
}
