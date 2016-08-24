#include "zbaselib_socket.h"

#define PORT 16000
#define SOCKADDR_LEN	sizeof(struct sockaddr)

#define CMD_GET	"get"

int main(int argc, char* argv[])
{
	char buf[1024] = {0};
	struct sockaddr_in svr_addr, client_addr;
	socklen_t addr_len = SOCKADDR_LEN;
	struct sockaddr_in boundaddr = {0};
//	fd_set rd_set;
//	fd_set wr_set;
//	struct timeval timeout;
//	int ret = 0;

	if(argc != 3)
	{
		printf("udp_client ip port\n");
		return 1;
	}
	
	zbaselib_socket_t sk = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(!zbaselib_socket_isvalid(sk))
	{
		printf("socket error:%d\n", zbaselib_socket_geterror());
		return 0;
	}

	zbaselib_socket_setaddr(&boundaddr, htonl(INADDR_ANY), 0);
	bind(sk, (struct sockaddr*)&boundaddr, SOCKADDR_LEN);
	
	getsockname(sk, (struct sockaddr*)&boundaddr, &addr_len);
	printf("boundaddr is %s:%d\n", inet_ntoa(boundaddr.sin_addr), ntohs(boundaddr.sin_port));
	zbaselib_socket_setaddr(&svr_addr, inet_addr(argv[1]), atoi(argv[2]));

#ifdef WIN32
	if(sendto(sk, CMD_GET, strlen(CMD_GET), 0, (struct sockaddr*)&svr_addr, SOCKADDR_LEN) <= 0)
#else
	if(sendto(sk, CMD_GET, strlen(CMD_GET), MSG_NOSIGNAL, (struct sockaddr*)&svr_addr, SOCKADDR_LEN) <= 0)
#endif
	{
		printf("send to error:%d\n", zbaselib_socket_geterror());
		return 0;
	}
		
//	FD_ZERO(&rd_set);
//	FD_SET(sk, &rd_set);

//	timeout.tv_sec = 5;
//	timeout.tv_usec = 0;

//	ret = select(sk + 1, &rd_set, NULL, NULL, &timeout);
//	if(ret > 0)
	while(1)
	{
		memset(buf, 0, 1024);
		if(recvfrom(sk, buf, sizeof(buf), 0, (struct sockaddr*)&client_addr, &addr_len) <= 0)
		{
			printf("send to error:%d\n", zbaselib_socket_geterror());
			break;
		}
		else
			printf("recvfrom %s:%d, data is %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buf);
	}
	
	zbaselib_socket_close(sk);
	return 0;
}
