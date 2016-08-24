#include "zbaselib_socket.h"
#include <sys/epoll.h>

#define PORT  9000
#define MAX_EVENTS 10
static char data[100] = {0};

#if 1
int main(int argc, char *argv[])
{
	struct sockaddr_in svr_addr;
	zbaselib_socket_t svr;
	struct epoll_event ev, events[MAX_EVENTS];
	int listen_sock, conn_sock, nfds, epollfd;
	int n = 0;
	char buf[100];
	
	zbaselib_socket_init();

	memset(&svr_addr, 0, sizeof(svr_addr));
	zbaselib_socket_setaddr(&svr_addr, htonl(INADDR_ANY), PORT);
	svr = zbaselib_socket_create_tcpserver(&svr_addr);
	if(!zbaselib_socket_isvalid(svr))
	{
		goto EXITOUT;
	}
	
	listen_sock = svr;
	epollfd = epoll_create(10);
	if(epollfd < 0)
	{
		perror("epoll_create:");
		goto EXITOUT;
	}
#if 0
	typedef union epoll_data {
               void        *ptr;
               int          fd;
               __uint32_t   u32;
               __uint64_t   u64;
           } epoll_data_t;

           struct epoll_event {
               __uint32_t   events;      /* Epoll events */
               epoll_data_t data;        /* User data variable */
           };

#endif
	ev.events = EPOLLIN;
	ev.data.fd = listen_sock;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, svr, &ev) == -1) {
	   perror("epoll_ctl: listen_sock");
	  goto EXITOUT;
	}
	
	for (;;) {
	   nfds = epoll_wait(epollfd, events, MAX_EVENTS, 500);
	   if (nfds == -1) {
	       perror("epoll_pwait");
	       exit(EXIT_FAILURE);
	   }
	
	   for (n = 0; n < nfds; ++n) {
	       if (events[n].data.fd == listen_sock) {
	           conn_sock = accept(listen_sock,
	                           NULL, NULL);
	           if(zbaselib_socket_isvalid(conn_sock))
				{
						 zbaselib_socket_nonblocking(conn_sock);
		           ev.events = EPOLLIN | EPOLLET | EPOLLOUT;
		           ev.data.fd = conn_sock;
		           if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,
		                       &ev) == -1) {
		               perror("epoll_ctl: conn_sock");
		               exit(EXIT_FAILURE);
		           }
	           }
	       } else {
	           if(events[n].events &  EPOLLIN)
	           	{
	           		int ret = 0;
					memset(buf, 0, 100);
					ret = zbaselib_socket_recv(events[n].data.fd, buf, 99);
					if(ret < 0)
					{
						printf("error: %d\n", zbaselib_socket_geterror());
						zbaselib_socket_close(events[n].data.fd);
						break;
					}
					
					printf("%d recv : %s\n", events[n].data.fd, buf);
	           	}
	           	else if(events[n].events & EPOLLOUT)
           		{
           			int ret = zbaselib_socket_sendn(events[n].data.fd, data, 100);
					if(ret < 0)
					{
						printf("error:%d\n", zbaselib_socket_geterror());
						zbaselib_socket_close(events[n].data.fd);
						break;
					}
					else
					{
						printf("%d send %d\n", events[n].data.fd, ret);
					}
           		}
	       }
	   }
	}

EXITOUT:
	zbaselib_socket_deinit();
	
	return 0;
}
#endif
