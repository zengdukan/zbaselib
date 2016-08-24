#include "zbaselib_socket.h"

#define PORT 9001
#define SOCKADDR_LEN	sizeof(struct sockaddr)


int main(int argc, char* argv[])
{
	char recv_buf[1024] = {0};
	char send_buf[1024] = {0};
	struct sockaddr_in svr_addr, client_addr;
	socklen_t addr_len = SOCKADDR_LEN;
	fd_set rd_set;
	struct timeval timeout;
	int ret = 0;
	
	zbaselib_socket_t svr = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(!zbaselib_socket_isvalid(svr))
	{
		printf("socket error:%d\n", zbaselib_socket_geterror());
		return 0;
	}

	zbaselib_socket_setaddr(&svr_addr, htonl(INADDR_ANY), PORT);
	if(bind(svr, (struct sockaddr*)&svr_addr, SOCKADDR_LEN) != 0)
	{
		printf("bind error:%d\n", zbaselib_socket_geterror());
		zbaselib_socket_close(svr);
		return 0;
	}
	

	
	while(1)
	{
		memset(recv_buf, 0, 1024);
		FD_ZERO(&rd_set);
		FD_SET(svr, &rd_set);

		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		ret = select(svr + 1, &rd_set, NULL, NULL, &timeout);
		if(ret > 0)
		{
			if(recvfrom(svr, recv_buf, 1024, 0, (struct sockaddr*)&client_addr, &addr_len) <= 0)
			{
				printf("send to error:%d\n", zbaselib_socket_geterror());
				break;
			}
			else
				printf("recvfrom %s:%d, data is %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), recv_buf);
			
			if(strcmp(recv_buf, "get") == 0)
			{
				memset(send_buf, 0, sizeof(send_buf));
				strcpy(send_buf, "hello, lucky man");
#ifdef WIN32
				if(sendto(svr, send_buf, strlen(send_buf), 0, (struct sockaddr*)&client_addr, addr_len) <= 0)
#else
				if(sendto(svr, send_buf, strlen(send_buf), MSG_NOSIGNAL, (struct sockaddr*)&client_addr, addr_len) <= 0)
#endif
				{
					printf("send to error:%d\n", zbaselib_socket_geterror());
				}
				
			}
		}
		else
			printf("select error or timeout\n");
	}

	zbaselib_socket_close(svr);
	return 0;
}
