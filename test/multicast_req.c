#include "zbaselib_socket.h"

#define MULTI_IP	"239.199.199.199"
#define MULTI_PORT	18600

#define SOCKADDR_LEN	sizeof(struct sockaddr)


int main(int argc, char* argv[])
{
	zbaselib_socket_t fd = 0;
	struct sockaddr_in bindaddr;
	struct ip_mreq mreq;                                    /*加入多播组*/
	int ret = 0;
	char buf[128];
	int loop = 0;
	struct sockaddr_in multiaddr;
	fd_set rfds;
	struct timeval tv = {0};
	struct sockaddr_in recvaddr = {0};
	int addrlen = SOCKADDR_LEN;
	int i = 0;	
	
	zbaselib_socket_init();

	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(!zbaselib_socket_isvalid(fd))
	{
		printf("socket error:%d\n", zbaselib_socket_geterror());
		return -1;
	}

	zbaselib_socket_setaddr(&multiaddr, inet_addr(MULTI_IP), MULTI_PORT);
	zbaselib_socket_setaddr(&bindaddr, INADDR_ANY, MULTI_PORT);	
	if(bind(fd, (struct sockaddr*)&bindaddr, SOCKADDR_LEN) < 0)
	{		
        printf("bind:%d\n", zbaselib_socket_geterror());
        return -2;
	}

	loop = 0;
    ret = setsockopt(fd,IPPROTO_IP, IP_MULTICAST_LOOP,&loop, sizeof(loop));
    if(ret < 0)
    {
        printf("setsockopt():IP_MULTICAST_LOOP:%d\n", zbaselib_socket_geterror());
        return -3;
    }

	mreq.imr_multiaddr.s_addr = inet_addr(MULTI_IP); /*多播地址*/
    mreq.imr_interface.s_addr = inet_addr(argv[1]); /*网络接口为默认*/
                                                        /*将本机加入多播组*/
    ret = setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,&mreq, sizeof
    		(mreq));
    if (ret < 0)
    {
        printf("setsockopt():IP_ADD_MEMBERSHIP:%d\n", zbaselib_socket_geterror());
        return -4;
    }

	while(1)
	{
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);

		tv.tv_sec = 5;
		tv.tv_usec = 0;

		printf("sendmsg:%d\n", i++);
		sendto(fd, argv[2], strlen(argv[2]), 0, (struct sockaddr*)&multiaddr, SOCKADDR_LEN);
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



