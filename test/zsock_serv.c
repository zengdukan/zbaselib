#include "zbaselib_socket.h"

#define PORT  9000
#if 1
int main(int argc, char *argv[])
{
	struct sockaddr_in svr_addr;
	zbaselib_socket_t svr;

	zbaselib_socket_init();

	memset(&svr_addr, 0, sizeof(svr_addr));
	zbaselib_socket_setaddr(&svr_addr, htonl(INADDR_ANY), PORT);
	svr = zbaselib_socket_create_tcpserver(&svr_addr);
	if(!zbaselib_socket_isvalid(svr))
	{
		goto EXITOUT;
	}

	for (; ;)
	{
		zbaselib_socket_t acpt_sock = INVALID_SOCKET;
		acpt_sock = accept(svr, NULL, NULL);
		if(zbaselib_socket_isvalid(acpt_sock))
		{
			char buf[100];
			for(; ;)
			{
				int ret = 0;
				memset(buf, 0, 100);
				ret = zbaselib_socket_recv(acpt_sock, buf, 99);
				if(ret < 0)
				{
					printf("error: %d\n", zbaselib_socket_geterror());
					zbaselib_socket_close(acpt_sock);
					break;
				}
				
				if(ret > 0)
				{
					printf("svr recv : %s\n", buf);
					zbaselib_socket_sendn(acpt_sock, buf, ret);
				}
			}
		}
	}

EXITOUT:
	zbaselib_socket_deinit();
	
	return 0;
}
#endif
