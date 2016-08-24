#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

void iprintf(const char *fmt, ...)
{
    char buf[1024] = {0};
    va_list   args;

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    printf("%s", buf);
}


int main(int argc, char* argv[])
{
	iprintf("a=%d, b=%x\n", 100, 64);
	iprintf("%s:%d\n", __FUNCTION__, __LINE__);
	
	return 0;
}

