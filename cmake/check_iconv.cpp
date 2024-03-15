#include <iconv.h>

int main()
{
    iconv_t handle = iconv_open("UTF-16LE", "UTF-8");
    iconv_close(handle);
    return 0;
}