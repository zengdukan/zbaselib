#include "zbaselib_socket.h"
#include <assert.h>

#define PORT 9000

zbaselib_socket_t zbaselib_socket_create_tcpclient11(const struct sockaddr_in* addr)
{
	zbaselib_socket_t sock = 0;

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (!zbaselib_socket_isvalid(sock))
		return INVALID_SOCKET;

	assert(zbaselib_socket_nonblocking(sock) == 0);
	assert(zbaselib_socket_keepalive(sock, 5000, 3000) == 0);

	if (connect(sock, (const struct sockaddr *) addr, sizeof(*addr)) != 0 
		&& !zbaselib_socket_connect_wouldblock())
	{
		zbaselib_socket_close(sock); 
		return INVALID_SOCKET;
	}

	return sock;
}

int main(int argc, char* argv[])
{
	zbaselib_socket_t clnt;
	struct sockaddr_in addr;

	assert(zbaselib_socket_init() == 0);

	zbaselib_socket_setaddr(&addr, inet_addr("192.168.1.44"), htons(PORT));
CONN:
	clnt = zbaselib_socket_create_tcpclient11(&addr);

	if(zbaselib_socket_waitforconnect(clnt, 2000) < 0)
	{
		zbaselib_socket_close(clnt);
		goto CONN;
	}

	if(zbaselib_socket_isvalid(clnt))
	{
		printf("connect ok\n");
	}
	else
	{
		printf("error:%d\n", zbaselib_socket_geterror());

	}

	zbaselib_socket_close(clnt);
	zbaselib_socket_deinit();

	return 0;
}