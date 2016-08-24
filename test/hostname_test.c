#include "zbaselib_socket.h"

int main(int argc, char* argv[])
{
	char strip[20] = {0};
	int ip = 0;

	zbaselib_socket_init();
	/*
	if(argc != 2)
	{
		printf("format:%s domain\n", argv[0]);
		return 0;	
	}
	*/
	// get ip by host or ip, return 0=ok, -1=fail
	if(zbaselib_socket_getipbyname("www.baidu.com", strip, &ip) == 0)
	{
		printf("ip=%s, %x\n",  strip, ip);
	}
	else
	{
		printf("err = %d\n", zbaselib_socket_geterror());
	}
	
	getchar();

	zbaselib_socket_deinit();

	return 0;
}