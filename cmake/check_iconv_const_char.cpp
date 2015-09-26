#include <string.h>
#include <iconv.h>

int main()
{
    const char* inbuf = "this is a string";
    size_t inbytesleft = strlen(inbuf) + 1;
    char outbuf[100];
    size_t outbytesleft = 100;
    
    iconv_t handle = iconv_open("UTF-16LE", "UTF-8");
    const char* pin = inbuf;
    char* pout = outbuf;
    iconv(handle, &pin, &inbytesleft, &pout, &outbytesleft);
    iconv_close(handle);
}
