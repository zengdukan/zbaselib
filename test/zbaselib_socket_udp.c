#include "zbaselib_socket.h"
#include "zbaselib_thread.h"
#include <assert.h>
#include <time.h>

#define PORT 9001
#define SOCKADDR_LEN	sizeof(struct sockaddr)
int client_flag;
int svr_flag;

zbaselib_thread_result ZBASELIB_THREAD_PROCSPEC udp_client_th(zbaselib_thread_param param)
{
	char send_str[100] = {0};
	struct sockaddr_in svr_addr;
	time_t long_time;
	struct tm* current_time;

	zbaselib_socket_t client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(!zbaselib_socket_isvalid(client))
	{
		printf("socket error:%d\n", zbaselib_socket_geterror());
		return (zbaselib_thread_result)NULL;
	}

	

	zbaselib_socket_setaddr(&svr_addr, inet_addr("127.0.0.1"), htons(PORT));
	while(client_flag)
	{
		memset(send_str, 0, 100);
		time(&long_time);
		current_time = localtime(&long_time);
		sprintf(send_str, "good boy, %d:%d:%d", current_time->tm_hour,
			current_time->tm_min, current_time->tm_sec);

#ifdef WIN32
		if(sendto(client, send_str, strlen(send_str), 0, (struct sockaddr*)&svr_addr, SOCKADDR_LEN) <= 0)
#else
		if(sendto(client, send_str, strlen(send_str), MSG_NOSIGNAL, (struct sockaddr*)&svr_addr, SOCKADDR_LEN) <= 0)
#endif
		{
			printf("send to error:%d\n", zbaselib_socket_geterror());
			break;
		}
		else
			zbaselib_sleep_msec(1000);
	}

	zbaselib_socket_close(client);
	return (zbaselib_thread_result)NULL;
}

zbaselib_thread_result ZBASELIB_THREAD_PROCSPEC udp_svr_th(zbaselib_thread_param param)
{
	char recv_buf[1024] = {0};
	struct sockaddr_in svr_addr, client_addr;
	socklen_t addr_len = SOCKADDR_LEN;
	
	zbaselib_socket_t svr = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(!zbaselib_socket_isvalid(svr))
	{
		printf("socket error:%d\n", zbaselib_socket_geterror());
		return (zbaselib_thread_result)NULL;
	}

	zbaselib_socket_setaddr(&svr_addr, htonl(INADDR_ANY), htons(PORT));
	if(bind(svr, (struct sockaddr*)&svr_addr, SOCKADDR_LEN) != 0)
	{
		printf("bind error:%d\n", zbaselib_socket_geterror());
		zbaselib_socket_close(svr);
		return (zbaselib_thread_result)NULL;
	}

	while(svr_flag)
	{
		memset(recv_buf, 0, 1024);
		if(recvfrom(svr, recv_buf, 1024, 0, (struct sockaddr*)&client_addr, &addr_len) <= 0)
		{
			printf("send to error:%d\n", zbaselib_socket_geterror());
			break;
		}
		else
		{
			printf("recv:%s\n", recv_buf);
			zbaselib_sleep_msec(1000);
		}
	}

	zbaselib_socket_close(svr);
	return (zbaselib_thread_result)NULL;
}

int main(int argc, char* argv[])
{
	zbaselib_thread_t svr_tht, client_tht;

	svr_flag = 1;
	client_flag = 1;
	zbaselib_socket_init();

	svr_tht = zbaselib_thread_create(udp_svr_th, NULL);
	if(svr_tht == ZBASELIB_INVALID_THREAD)
	{
		printf("create svr_th fail\n");
		return 1;
	}

	client_tht = zbaselib_thread_create(udp_client_th, NULL);
	if(client_tht == ZBASELIB_INVALID_THREAD)
	{
		printf("create client th fail\n");
		return 1;
	}

	while(getchar() != 'q')
	{
		zbaselib_sleep_msec(1000);
	}

	svr_flag = 0;
	client_flag = 0;

	zbaselib_thread_join(client_tht);
	zbaselib_thread_join(svr_tht);

	zbaselib_socket_deinit();

	return 0;
}