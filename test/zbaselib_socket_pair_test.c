#include "zbaselib_socket.h"
#include <assert.h>

int main(int argc, char* argv[])
{
	zbaselib_socket_t pair[2];
	char str[] = "everyday good luck!";
	char buf[100];

	zbaselib_socket_init();

	if(zbaselib_socket_pair(pair) != 0)
	{
		perror("pair:");
		return 0;
	}


	for(; ;)
	{
		assert(zbaselib_socket_sendn(pair[0], str, strlen(str)) > 0);

		memset(buf, 0, 100);

		zbaselib_socket_recv(pair[1], buf, 99);
		printf("recv : %s\n", buf);

		zbaselib_sleep_msec(100);
	}

	zbaselib_socket_deinit();

	return 0;
}