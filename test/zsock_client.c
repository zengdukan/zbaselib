#include "zbaselib_socket.h"

#define PORT  9000
#if 1
int main(int argc, char *argv[])
{
	zbaselib_socket_t clnt;
	struct sockaddr_in addr;
	char buf[512];
	int n = 0;
	
	zbaselib_socket_init();

	zbaselib_socket_setaddr(&addr, inet_addr("127.0.0.1"), PORT);
CONN:
	clnt = zbaselib_socket_create_tcpclient(&addr);
	
	if(zbaselib_socket_isvalid(clnt))
	{
		char str[] = "every day good luck";
		fd_set rd_set;
		fd_set wr_set;
		struct timeval timeout;

		if(zbaselib_socket_waitforconnect(clnt, 2000) < 0)
		{
			zbaselib_socket_close(clnt);
			printf("wait form connect fail\n");
			goto CONN;
		}

		printf("connect to server ok\n");

		for (; ;)
		{
			int ret = 0;

			FD_ZERO(&rd_set);
			FD_ZERO(&wr_set);

			FD_SET(clnt, &rd_set);
			FD_SET(clnt, &wr_set);

			timeout.tv_sec = 1;
			timeout.tv_usec = 0;

			ret = select(clnt + 1, &rd_set, &wr_set, NULL, &timeout);
			if(ret > 0)
				printf("ret = %d\n", ret);
			
			if(zbaselib_socket_sendn(clnt, str, strlen(str)) < 0)
			{
				printf("error:%d\n", zbaselib_socket_geterror());
				zbaselib_socket_close(clnt);
				break;
			}

			if((n = zbaselib_socket_recv(clnt, buf, 512)) < 0)
			{
				printf("error:%d\n", zbaselib_socket_geterror());
				zbaselib_socket_close(clnt);
				break;
			}
			else 
				printf("recv size = %d\n", n);

			zbaselib_sleep_msec(1000);
		}
	}
	else
	{
		printf("error:%d\n", zbaselib_socket_geterror());
	}

	zbaselib_socket_deinit();

	return 0;
}
#endif

