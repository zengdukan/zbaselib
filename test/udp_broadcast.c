#include "zbaselib_socket.h"

#define SERV_PORT 	18600
#define CLI_PORT	18601
#define SOCKADDR_LEN	sizeof(struct sockaddr)

int main(int argc, char* argv[])
{
	zbaselib_socket_t fd = 0;
	fd_set rfds;
	struct sockaddr_in addr = {0};
	struct timeval tv = {0};
	int ret = 0;
	struct sockaddr_in bcaddr = {0};
	struct sockaddr_in recvaddr = {0};
	int addrlen = SOCKADDR_LEN;
	char buf[128];
	int i = 0;
	
	zbaselib_socket_init();

	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(!zbaselib_socket_isvalid(fd))
	{
		printf("socket error:%d\n", zbaselib_socket_geterror());
		return -1;
	}

	zbaselib_socket_setaddr(&bcaddr, 0xFFFFFFFF, CLI_PORT);
	zbaselib_socket_setaddr(&addr, INADDR_ANY, SERV_PORT);	
	bind(fd, (struct sockaddr*)&addr, SOCKADDR_LEN);

	zbaselib_socket_udp_broadcast(fd, 1);

	

	while(1)
	{
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);

		tv.tv_sec = 5;
		tv.tv_usec = 0;

		printf("sendmsg:%d\n", i++);
		sendto(fd, "a", 1, 0, (struct sockaddr*)&bcaddr, SOCKADDR_LEN);
#ifdef WIN32		
		ret = select(0, &rfds, NULL, NULL, &tv);
#else
		ret = select(fd + 1, &rfds, NULL, NULL, &tv);
#endif
		if(ret > 0)
		{
			memset(buf, 0, sizeof(buf));
			ret = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr*)&recvaddr, (socklen_t*)&addrlen);
			printf("%s\n", buf);
		}
	}

	zbaselib_socket_deinit();

	return 0;
}

