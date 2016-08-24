#include "zbaselib_socket.h"

#define UV__INET_ADDRSTRLEN         16
#define UV__INET6_ADDRSTRLEN        46


#ifdef WIN32

// 初始化
int zbaselib_socket_init()
{
	WORD versionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;

	if (WSAStartup(versionRequested, &wsaData) != 0) 
		return -1;

	return 0;
}

// 注销
void zbaselib_socket_deinit()
{
	WSACleanup();
}

int zbaselib_socket_close(zbaselib_socket_t sock)
{
	int ret = closesocket(sock);
	sock = INVALID_SOCKET;
	return ret;
}

int zbaselib_socket_nonblocking(zbaselib_socket_t sock)
{
	ulong nonblocking = 1;
	if (ioctlsocket(sock, FIONBIO, &nonblocking) != 0) 
	{
		return -1;
	}

	return 0;
}

int zbaselib_socket_reusebale(zbaselib_socket_t sock)
{
	return 0;
}

int zbaselib_socket_keepalive(zbaselib_socket_t sock, uint idlemsec, uint keepmsec)
{
	int ret;
	struct tcp_keepalive inKeepAlive = {0};
	struct tcp_keepalive outKeepAlive = {0};
	unsigned long optSize = sizeof(struct tcp_keepalive);        
	unsigned long ulBytesReturn = 0;    

	inKeepAlive.onoff             = TRUE;      
	inKeepAlive.keepaliveinterval = (keepmsec / 5); 
	inKeepAlive.keepalivetime     = idlemsec;

	ret = WSAIoctl(sock,      
		SIO_KEEPALIVE_VALS, 
		(LPVOID) &inKeepAlive, optSize, 
		(LPVOID) &outKeepAlive, optSize,    
		&ulBytesReturn, NULL, NULL);    
	if (0 != ret)    
		return -1;

	return 0;
}

int zbaselib_socket_rw_wouldblock()
{
	int e = zbaselib_socket_geterror();
	return (e == WSAEWOULDBLOCK	|| e == WSAEINTR);
}

int zbaselib_socket_connect_wouldblock()
{
	int e = zbaselib_socket_geterror();
	return (e == WSAEWOULDBLOCK ||					
		e == WSAEINTR ||						
		e == WSAEINPROGRESS ||					
		e == WSAEINVAL);
}

int zbaselib_socket_accept_wouldblock()
{
	int e = zbaselib_socket_geterror();

	return zbaselib_socket_rw_wouldblock();
}

int zbaselib_socket_pair(zbaselib_socket_t pair[2])
{
	zbaselib_socket_t listener = -1;
	zbaselib_socket_t connector = -1;
	zbaselib_socket_t acceptor = -1;
	struct sockaddr_in listen_addr;
	struct sockaddr_in connect_addr;
	socklen_t size;
	int saved_errno = -1;

	if (!pair) 
	{
		return -1;
	}

	listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (!zbaselib_socket_isvalid(listener))
	{
		return -1;
	}
	
	memset(&listen_addr, 0, sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	listen_addr.sin_port = 0;	/* kernel chooses port.	 */
	if (bind(listener, (struct sockaddr *) &listen_addr, sizeof (listen_addr))
		== -1)
		goto tidy_up_and_fail;
	if (listen(listener, 1) == -1)
		goto tidy_up_and_fail;

	connector = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (!zbaselib_socket_isvalid(connector))
		goto tidy_up_and_fail;
	/* We want to find out the port number to connect to.  */
	size = sizeof(connect_addr);
	if (getsockname(listener, (struct sockaddr *) &connect_addr, &size) == -1)
		goto tidy_up_and_fail;
	if (size != sizeof (connect_addr))
		goto abort_tidy_up_and_fail;
	if (connect(connector, (struct sockaddr *) &connect_addr,
				sizeof(connect_addr)) == -1)
		goto tidy_up_and_fail;

	size = sizeof(listen_addr);
	acceptor = accept(listener, (struct sockaddr *) &listen_addr, &size);
	if (!zbaselib_socket_isvalid(acceptor))
		goto tidy_up_and_fail;
	if (size != sizeof(listen_addr))
		goto abort_tidy_up_and_fail;
	zbaselib_socket_close(listener);
	/* Now check we are talking to ourself by matching port and host on the
	   two sockets.	 */
	if (getsockname(connector, (struct sockaddr *) &connect_addr, &size) == -1)
		goto tidy_up_and_fail;
	if (size != sizeof (connect_addr)
		|| listen_addr.sin_family != connect_addr.sin_family
		|| listen_addr.sin_addr.s_addr != connect_addr.sin_addr.s_addr
		|| listen_addr.sin_port != connect_addr.sin_port)
		goto abort_tidy_up_and_fail;
	pair[0] = connector;
	pair[1] = acceptor;

	return 0;

 abort_tidy_up_and_fail:
	saved_errno = WSAECONNABORTED;
 tidy_up_and_fail:
	if (saved_errno < 0)
		saved_errno = zbaselib_socket_geterror();
	if (listener != -1)
		zbaselib_socket_close(listener);
	if (connector != -1)
		zbaselib_socket_close(connector);
	if (acceptor != -1)
		zbaselib_socket_close(acceptor);

	return -1;
}

zbaselib_socket_t zbaselib_socket_create_tcpclient(const struct sockaddr_in* addr)
{
	zbaselib_socket_t sock = 0;

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (!zbaselib_socket_isvalid(sock))
		return INVALID_SOCKET;

	if (zbaselib_socket_nonblocking(sock) != 0
		|| (connect(sock, (const struct sockaddr *) addr, sizeof(*addr)) != 0 
		&& !zbaselib_socket_connect_wouldblock()))
	{
		zbaselib_socket_close(sock); 
		return INVALID_SOCKET;
	}

	return sock;
}

zbaselib_socket_t zbaselib_socket_create_tcpserver(const struct sockaddr_in* addr)
{
	zbaselib_socket_t sock = 0;
	ulong nonblocking = 1;
	int reuse = 1;

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (!zbaselib_socket_isvalid(sock))
		return INVALID_SOCKET;

	if (zbaselib_socket_nonblocking(sock) != 0
		|| zbaselib_socket_reusebale(sock) != 0
		|| bind(sock, (const struct sockaddr *) addr, sizeof(*addr)) != 0
		|| listen(sock, 128) != 0)
	{
		zbaselib_socket_close(sock); 
		return INVALID_SOCKET;
	}

	return sock;
}

static int translate_option(sock_option opt, int* slevel, int* sopt) {
  switch (opt) {
    /*case OPT_DONTFRAGMENT:
      *slevel = IPPROTO_IP;
      *sopt = IP_DONTFRAGMENT;
      break;*/
    case OPT_RCVBUF:
      *slevel = SOL_SOCKET;
      *sopt = SO_RCVBUF;
      break;
    case OPT_SNDBUF:
      *slevel = SOL_SOCKET;
      *sopt = SO_SNDBUF;
      break;
    case OPT_NODELAY:
      *slevel = IPPROTO_TCP;
      *sopt = TCP_NODELAY;
      break;
    default:
      return -1;
  }
  
  return 0;
}

int zbaselib_socket_getoption(zbaselib_socket_t sock, sock_option opt, int* value)
{
	int slevel = 0;
	int sopt = 0;
	int optlen = 0;
	char* p = NULL;
	
	if (translate_option(opt, &slevel, &sopt) == -1)
		return -1;

	p = (char*)value;
	optlen = sizeof(value);
	return getsockopt(sock, slevel, sopt, p, &optlen);
}

int zbaselib_socket_setoption(zbaselib_socket_t sock, sock_option opt, int value)
{
	int slevel;
	int sopt;
	const char* p = (const char*)(&value);

	if (translate_option(opt, &slevel, &sopt) == -1)
		return -1;
	
	return setsockopt(sock, slevel, sopt, p, sizeof(value));
}

int zbaselib_socket_udp_broadcast(zbaselib_socket_t fd, int value)
{
	BOOL optval = (BOOL) value;

	if (setsockopt(fd,
	             SOL_SOCKET,
	             SO_BROADCAST,
	             (char*) &optval,
	             sizeof optval)) {
		return -1;
	}

	return 0;
}

#else
#include <net/route.h>



// 初始化
int zbaselib_socket_init()
{
	return 0;
}

// 注销
void zbaselib_socket_deinit()
{
	
}

int zbaselib_socket_close(zbaselib_socket_t sock)
{
	int ret = close(sock);
	sock = INVALID_SOCKET;
	return ret;
}

int zbaselib_socket_nonblocking(zbaselib_socket_t sock)
{
	int flags;
	if ((flags = fcntl(sock, F_GETFL, NULL)) < 0) 
	{
		return -1;
	}
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		return -1;
	}

	return 0;
}

int zbaselib_socket_reusebale(zbaselib_socket_t sock)
{
	int one = 1;

	return setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*) &one,
	    (socklen_t)sizeof(one));
}

int zbaselib_socket_keepalive(zbaselib_socket_t sock, uint32_t idlemsec, uint32_t keepmsec)
{
	int bKeepAlive = 1;
	int keepCnt = 3;
	int keepIntvl = keepmsec / keepCnt / 1000;		// 发3包侦测包
	idlemsec /= 1000;
	if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &bKeepAlive, sizeof(bKeepAlive)) != 0 ||
		setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCnt, sizeof(keepCnt)) != 0 ||
		setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idlemsec, sizeof(idlemsec)) != 0 ||
		setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepIntvl, sizeof(keepIntvl)) != 0)    
	{
		return -1;    
	}

	return 0;
}

int zbaselib_socket_rw_wouldblock()
{
	return (errno == EWOULDBLOCK 
		|| errno == EAGAIN
		|| errno == EINTR);
}

int zbaselib_socket_connect_wouldblock()
{
	return (errno == EINTR || errno == EINPROGRESS);
}

int zbaselib_socket_accept_wouldblock()
{
	return (errno == EINTR || errno == EAGAIN || errno== ECONNABORTED);
}

int zbaselib_socket_pair(zbaselib_socket_t pair[2])
{
	return socketpair(AF_UNIX, SOCK_STREAM, 0, pair);
}

zbaselib_socket_t zbaselib_socket_create_tcpclient(const struct sockaddr_in* addr)
{
	zbaselib_socket_t sock = 0;

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (!zbaselib_socket_isvalid(sock))
		return INVALID_SOCKET;

	if (zbaselib_socket_nonblocking(sock) != 0
		|| (connect(sock, (const struct sockaddr *) addr, sizeof(*addr)) < 0 
		&& !zbaselib_socket_connect_wouldblock()))
	{
		zbaselib_socket_close(sock); 
		return INVALID_SOCKET;
	}

	return sock;
}

zbaselib_socket_t zbaselib_socket_create_tcpserver(const struct sockaddr_in* addr)
{
	zbaselib_socket_t sock = 0;

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (!zbaselib_socket_isvalid(sock))
		return INVALID_SOCKET;
	
	if (zbaselib_socket_nonblocking(sock) != 0
		|| zbaselib_socket_reusebale(sock) != 0
		|| bind(sock, (const struct sockaddr *) addr, sizeof(*addr)) < 0
		|| listen(sock, 128) < 0)
	{
		zbaselib_socket_close(sock); 
		return INVALID_SOCKET;
	}

	return sock;
}

static int translate_option(sock_option opt, int* slevel, int* sopt) 
{
    switch (opt) {
      case OPT_DONTFRAGMENT:
        *slevel = IPPROTO_IP;
        *sopt = IP_MTU_DISCOVER;
        break;
      case OPT_RCVBUF:
        *slevel = SOL_SOCKET;
        *sopt = SO_RCVBUF;
        break;
      case OPT_SNDBUF:
        *slevel = SOL_SOCKET;
        *sopt = SO_SNDBUF;
        break;
      case OPT_NODELAY:
        *slevel = IPPROTO_TCP;
        *sopt = TCP_NODELAY;
        break;
      default:
        return -1;
    }
    return 0;
}

int zbaselib_socket_getoption(zbaselib_socket_t sock, sock_option opt, int* value)
{
	int slevel;
    int sopt;
	
    if (translate_option(opt, &slevel, &sopt) == -1)
      return -1;
	
    socklen_t optlen = sizeof(*value);
    int ret = getsockopt(sock, slevel, sopt, (void*)value, &optlen);
    if (ret != -1 && opt == OPT_DONTFRAGMENT) {
      *value = (*value != IP_PMTUDISC_DONT) ? 1 : 0;
    }
    return ret;
}

int zbaselib_socket_setoption(zbaselib_socket_t sock, sock_option opt, int value)
{
	int slevel;
	int sopt;
	if (translate_option(opt, &slevel, &sopt) == -1)
	  	return -1;
	if (opt == OPT_DONTFRAGMENT) {
	  value = (value) ? IP_PMTUDISC_DO : IP_PMTUDISC_DONT;
	}
	return setsockopt(sock, slevel, sopt, (void*)&value, sizeof(value));
}


int zbaselib_socket_udp_broadcast(zbaselib_socket_t fd, int value)
{
	if (setsockopt(fd,
                 SOL_SOCKET,
                 SO_BROADCAST,
                 &value,
                 sizeof(value))) {
    return -1;
  }

  return 0;
}

static int inet_set_flag(char *ifname, short flag)
{
	struct ifreq ifr;
	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		//DBG_ERR("socket error");
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0) {
		//DBG_ERR("net_set_flag: ioctl SIOCGIFFLAGS");
		close(skfd);
		return (-1);
	}
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_flags |= flag;
	if (ioctl(skfd, SIOCSIFFLAGS, &ifr) < 0) {
		//DBG_ERR("net_set_flag: ioctl SIOCSIFFLAGS");
		close(skfd);
		return -1;
	}
	close(skfd);
	return (0);
}

static int inet_clr_flag(char *ifname, short flag)
{
	struct ifreq ifr;
	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0) {
		close(skfd);
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_flags &= ~flag;
	if (ioctl(skfd, SIOCSIFFLAGS, &ifr) < 0) {
		close(skfd);
		return -1;
	}
	close(skfd);
	return (0);
}

int zbaselib_inet_nic_up(char *ifname)
{
	return inet_set_flag(ifname, (IFF_UP | IFF_RUNNING));
}

int zbaselib_inet_nic_down(char *ifname)
{
	return inet_clr_flag(ifname, IFF_UP);
}

int zbaselib_inet_get_flag(char *ifname)
{
	struct ifreq ifr;
	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		return -1;
	}
	
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0) {
		close(skfd);
		return -1;
	}
	close(skfd);
	return ifr.ifr_flags;
}

in_addr_t zbaselib_inet_get_ifaddr(char *ifname)
{
	struct ifreq ifr;
	int skfd;
	struct sockaddr_in *saddr;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		//DBG_ERR("socket error");
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		//DBG_ERR("net_get_ifaddr: ioctl SIOCGIFADDR");
		close(skfd);
		return -1;
	}
	close(skfd);
	
	saddr = (struct sockaddr_in *) &ifr.ifr_addr;
	return saddr->sin_addr.s_addr;
}

struct in_addr zbaselib_inet_get_ip(int skfd, char *ifname)
{
	struct ifreq ifr;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		//DBG_ERR("net_get_ip: ioctl SIOCGIFADDR");
		return (struct in_addr){-1};
	}
	return ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr;
}

int zbaselib_inet_set_ifaddr(char *ifname, in_addr_t addr)
{
	struct ifreq ifr;
	int skfd;
	struct sockaddr_in sa = {0};
	
	sa.sin_family = PF_INET;
	sa.sin_port = 0;
	
	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		return -1;
	}
	sa.sin_addr.s_addr = addr;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	memcpy((char *) &ifr.ifr_addr, (char *) &sa, sizeof(struct sockaddr));
	if (ioctl(skfd, SIOCSIFADDR, &ifr) < 0) {
		close(skfd);
		return -1;
	}
	close(skfd);
	return 0;
}

in_addr_t zbaselib_inet_get_netmask(char *ifname)
{
	struct ifreq ifr;
	int skfd;
	struct sockaddr_in *saddr;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0) {
		close(skfd);
		return -1;
	}
	close(skfd);
	
	saddr = (struct sockaddr_in *) &ifr.ifr_addr;
	return saddr->sin_addr.s_addr;
}

int zbaselib_inet_set_netmask(char *ifname, in_addr_t addr)
{
	struct ifreq ifr;
	int skfd;
	struct sockaddr_in sa = {0};
	
	sa.sin_family = PF_INET;
	sa.sin_port = 0;
	
	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		return -1;
	}
	sa.sin_addr.s_addr = addr;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	memcpy((char *) &ifr.ifr_addr, (char *) &sa, sizeof(struct sockaddr));
	if (ioctl(skfd, SIOCSIFNETMASK, &ifr) < 0) {
		close(skfd);
		return -1;
	}
	close(skfd);
	return 0;
}

int zbaselib_inet_get_hwaddr(char *ifname, unsigned char *mac)
{
	struct ifreq ifr;
	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
		close(skfd);
		return -1;
	}
	close(skfd);

	memcpy(mac, ifr.ifr_ifru.ifru_hwaddr.sa_data, IFHWADDRLEN);
	return 0;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
static int inet_search_gateway(char* ifname, char *buf, in_addr_t *gate_addr)
{
	char iface[16];
	unsigned long dest, gate;
	int iflags;

	sscanf(buf, "%s\t%08lX\t%08lX\t%8X\t", iface, &dest, &gate, &iflags);
	//printf("net_search_gateway:%s, %lX, %lX, %X\n", iface, dest, gate, iflags);
	if ( (iflags & (RTF_UP | RTF_GATEWAY)) == (RTF_UP | RTF_GATEWAY) 
		&& strcmp(ifname, iface) == 0) {
		*gate_addr = gate;
		return 0;
	}
	return -1;
}

#define PROCNET_ROUTE_PATH		"/proc/net/route"

int zbaselib_inet_get_gateway(char* ifname, in_addr_t* gw)
{
	char buff[132];
	FILE *fp = fopen(PROCNET_ROUTE_PATH, "r");

	if (!fp) {
		return -1;
	}
	
	fgets(buff, 130, fp);
	while (fgets(buff, 130, fp) != NULL) {
		if (inet_search_gateway(ifname, buff, gw) == 0) {
			fclose(fp);
			return 0;
		}
	}
	fclose(fp);
	return -1;
}

#if 0
static int inet_add_gateway(in_addr_t addr)
{
	struct rtentry rt;
	int skfd;
	struct sockaddr_in sa = {0};
	
	sa.sin_family = PF_INET;
	sa.sin_port = 0;
	
	/* Clean out the RTREQ structure. */
	memset((char *) &rt, 0, sizeof(struct rtentry));

	/* Fill in the other fields. */
	rt.rt_flags = (RTF_UP | RTF_GATEWAY);

	rt.rt_dst.sa_family = PF_INET;
	rt.rt_genmask.sa_family = PF_INET;

	sa.sin_addr.s_addr = addr;
	memcpy((char *) &rt.rt_gateway, (char *) &sa, sizeof(struct sockaddr));

	/* Create a socket to the INET kernel. */
	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		return -1;
	}
	/* Tell the kernel to accept this route. */
	if (ioctl(skfd, SIOCADDRT, &rt) < 0) {
		close(skfd);
		return -1;
	}
	/* Close the socket. */
	close(skfd);
	return (0);
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
static int inet_del_gateway(in_addr_t addr)
{
	struct rtentry rt;
	int skfd;
	struct sockaddr_in sa = {0};
	
	sa.sin_family = PF_INET;
	sa.sin_port = 0;
	
	/* Clean out the RTREQ structure. */
	memset((char *) &rt, 0, sizeof(struct rtentry));

	/* Fill in the other fields. */
	rt.rt_flags = (RTF_UP | RTF_GATEWAY);

	rt.rt_dst.sa_family = PF_INET;
	rt.rt_genmask.sa_family = PF_INET;

	sa.sin_addr.s_addr = addr;
	memcpy((char *) &rt.rt_gateway, (char *) &sa, sizeof(struct sockaddr));

	/* Create a socket to the INET kernel. */
	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		DBG_ERR("socket error");
		return -1;
	}
	/* Tell the kernel to accept this route. */
	if (ioctl(skfd, SIOCDELRT, &rt) < 0) {
		DBG_ERR("net_del_gateway: ioctl SIOCDELRT");
		close(skfd);
	return -1;
	}
	/* Close the socket. */
	close(skfd);
	return (0);
}

int zbaselib_inet_set_gateway(in_addr_t addr)
{
	in_addr_t gate_addr;
	char buff[132];
	FILE *fp = fopen(PROCNET_ROUTE_PATH, "r");

	if (!fp) {
		return -1;
	}
	fgets(buff, 130, fp);
	while (fgets(buff, 130, fp) != NULL) {
		if (inet_search_gateway(buff, &gate_addr) == 0) {
			inet_del_gateway(gate_addr);
		}
	}
	fclose(fp);
	
	return inet_add_gateway(addr);
}

int zbaselib_inet_clean_gateway(void)
{

}
#endif

#define RESOLV_CONF				"/etc/resolv.conf"

int zbaselib_inet_set_dns(char *dnsname1, char* dnsname2)
{
	FILE *fp;

	fp = fopen(RESOLV_CONF, "w");
	if ( fp ) {
		if(dnsname1)
			fprintf(fp, "nameserver %s\n", dnsname1);
		
		if(dnsname2)
			fprintf(fp, "nameserver %s\n", dnsname2);
		
		fclose(fp);
		return 0;
	}

	return -1;
}

int zbaselib_inet_get_dns(in_addr_t dns[2])
{
	int size = 0;
	FILE *fp;	
	char dnsname[80];	
	char line[256];	
	fp = fopen(RESOLV_CONF, "r");	
	if ( fp ) 
	{
		while(fgets(line, sizeof(line), fp) != NULL)
		{
			if(sscanf(line, "nameserver %s", dnsname) == 1)
			{
				dns[size++] = inet_addr(dnsname);
			}

			if(size >= 2)
				break;
		}

		fclose(fp);
		return size;		
	}	

	return -1;
}



#endif

/*****************************************************************************
函数功能:判断域名或者IP字符串
输入参数:src_server: 传入的域名或IP字符串
返  回   值:-1-失败，0-IP字符串，1-域名
******************************************************************************/
static int check_ip_or_domain(const char *src_server)
{
	int len = 0;
	int num = 0;

	if(src_server == NULL)
	{
		return -1;
	}

	len = strlen(src_server);
	if(len <= 0)
	{
		return -1;
	}

	for(num = 0; num < len; num++)
	{
		if((src_server[num] < '0' || src_server[num] > '9') && src_server[num] != '.')
		{
			return 1;
		}
	}

	return 0;
}

/*
* 获取域名的ip地址
* src_server = 传入的域名或IP字符串
* dst_server_ip=以点分十进制的ip地址
* dst_ip = 一个整型数的IP 地址
*/
int zbaselib_socket_getipbyname(const char* src_server, char* dst_server_ip, int* dst_ip)
{
#if 0
	struct in_addr addr = {0};
	struct hostent tmp = {0}; 
	struct hostent *h = NULL; 
	
	int is_ip = check_ip_or_domain(src_server);
	if(is_ip < 0)
	{
		return -1;
	}
	else if(is_ip == 0)
	{
		if(dst_server_ip != NULL)
		{
			strncpy(dst_server_ip, src_server, 15);
		}
		
		if(dst_ip != NULL)
		{
			// modify hyzeng 2013-4-16，inet_addr失败返回-1转化成255.255.255.255的地址
#ifdef WIN32
			unsigned long ip = inet_addr(src_server);
			if(ip == INADDR_NONE)
				return -1;
			else
				*dst_ip = (int)ip;
#else
			if(inet_aton(src_server, &addr) != 0)
			{
				*dst_ip = addr.s_addr;
			}
			else
				return -1;
#endif
		}
		return 0;
	}

	h = gethostbyname(src_server);
	if(h == NULL)
		return -1; 

	tmp = *h;
	if(dst_ip != NULL)
		*dst_ip = *(int*)tmp.h_addr_list[0];
	addr.s_addr = *(int*)tmp.h_addr_list[0];

	if(dst_server_ip != NULL)
		strncpy(dst_server_ip, inet_ntoa(addr), 19);
	
	return 0;
#else
	struct in_addr addr = {0};
	struct sockaddr_in* addrin = NULL;
	struct addrinfo aiHints;
	struct addrinfo *aiList = NULL;
	int ret = 0;

	int is_ip = check_ip_or_domain(src_server);
	if(is_ip < 0)
	{
		return -1;
	}
	else if(is_ip == 0)
	{
		if(dst_server_ip != NULL)
		{
			strncpy(dst_server_ip, src_server, 15);
		}

		if(dst_ip != NULL)
		{
			// modify hyzeng 2013-4-16，inet_addr失败返回-1转化成255.255.255.255的地址
#ifdef WIN32
			unsigned long ip = inet_addr(src_server);
			if(ip == INADDR_NONE)
				return -1;
			else
				*dst_ip = (int)ip;
#else
			if(inet_aton(src_server, &addr) != 0)
			{
				*dst_ip = addr.s_addr;
			}
			else
				return -1;
#endif
		}
		return 0;
	}

	memset(&aiHints, 0, sizeof(aiHints));
	aiHints.ai_family = AF_UNSPEC;
	aiHints.ai_socktype = SOCK_STREAM;
	aiHints.ai_protocol = 0;

	if ((ret = getaddrinfo(src_server, NULL, &aiHints, &aiList)) != 0 || aiList == NULL)
	{
		return -1;
	}

	addrin = (struct sockaddr_in*)aiList->ai_addr;
	if(dst_ip != NULL)
		*dst_ip = addrin->sin_addr.s_addr;

	if(dst_server_ip != NULL)
		strncpy(dst_server_ip, inet_ntoa(addrin->sin_addr), 19);

	freeaddrinfo(aiList);
	return 0;
#endif
}

int zbaselib_socket_shutdown(zbaselib_socket_t sock, int how)
{
	return shutdown(sock, how);
}

int zbaselib_socket_waitforconnect(zbaselib_socket_t sock, int msec)
{
	fd_set fs;
	struct timeval tv = {msec / 1000, (msec % 1000) * 1000};
	int ret;

	FD_ZERO(&fs);
	FD_SET(sock, &fs);

#ifdef WIN32
	ret = select(0, NULL, &fs, NULL, &tv);
	return (ret > 0 && FD_ISSET(sock, &fs)) ? 0 : -1;
#else
	ret = select(sock + 1, NULL, &fs, NULL, &tv);
	int sockErr = 0;
	socklen_t len = sizeof(int);
	int ret1 = getsockopt(sock, SOL_SOCKET, SO_ERROR, (char *)&sockErr, &len);
	return (ret > 0 && FD_ISSET(sock, &fs) 
		&& ret1 == 0 
		&& sockErr == 0) ?
		0 : -1;
#endif
}

int zbaselib_socket_isvalid(zbaselib_socket_t sock)
{
#ifdef WIN32
	return (sock != INVALID_SOCKET);
#else
	return (sock >= 0);
#endif
}

void zbaselib_socket_setaddr(struct sockaddr_in* addr, uint ip, ushort port)
{
	memset(addr, 0, sizeof(struct sockaddr_in));
	addr->sin_family      = AF_INET;
	addr->sin_addr.s_addr = ip;
	addr->sin_port        = htons(port);
}

// 完全发送size字节数据
int zbaselib_socket_sendn(zbaselib_socket_t sock, const char* buf, uint size)
{
	int		nleft, nwritten;
	const char	*ptr;

	ptr = buf;	/* can't do pointer arithmetic on void* */
	nleft = size;
	while (nleft > 0) 
	{
#ifdef WIN32
		if ((nwritten = send(sock, ptr, nleft, 0)) <= 0)
#else
		if ((nwritten = send(sock, ptr, nleft, MSG_NOSIGNAL)) <= 0)
#endif
		{
			if(!zbaselib_socket_rw_wouldblock())
				return -1;		/* error */
			else
			{
				zbaselib_sleep_msec(10);
				continue;
			}
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	
	return(size);
}

// >=0:send size, 小于size表示发送阻塞没完成, -1:error
int zbaselib_socket_sendn_retry(zbaselib_socket_t sock, const char* buf, uint size, int retry)
{
	int 	nleft, nwritten;
	const char	*ptr;
	
	ptr = buf;	/* can't do pointer arithmetic on void* */
	nleft = size;
	while (nleft > 0) 
	{
#ifdef WIN32
		if ((nwritten = send(sock, ptr, nleft, 0)) <= 0)
#else
		if ((nwritten = send(sock, ptr, nleft, MSG_NOSIGNAL)) <= 0)
#endif
		{
			if(!zbaselib_socket_rw_wouldblock())
				return -1;		/* error */
			else
			{
				retry--;

				if(retry <= 0)
					return (size - nleft);
				else
				{
					zbaselib_sleep_msec(10);
					continue;
				}
			}
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	
	return (size - nleft);
}



int zbaselib_socket_udp_sendn(zbaselib_socket_t sock, const char* buf, uint size, 
	const struct sockaddr_in* svr_addr)
{
	int		nleft, nwritten;
	const char	*ptr;

	ptr = buf;	/* can't do pointer arithmetic on void* */
	nleft = size;
	while (nleft > 0) 
	{
#ifdef WIN32
		if ((nwritten = sendto(sock, ptr, nleft, 0, (const struct sockaddr*)svr_addr, sizeof(struct sockaddr_in
			))) <= 0)
#else
		if ((nwritten = sendto(sock, ptr, nleft, MSG_NOSIGNAL, (const struct sockaddr*)svr_addr, sizeof(struct sockaddr_in
			))) <= 0)
#endif
		{
			if(!zbaselib_socket_rw_wouldblock())
				return -1;		/* error */
			else
				continue;
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}

	return(size);
}

int zbaselib_socket_recv(zbaselib_socket_t sock, char* buf, uint size)
{
#ifdef WIN32
	int ret = recv(sock, buf, size, 0);
#else
	int ret = recv(sock, buf, size, MSG_NOSIGNAL);
#endif
	if(ret == 0 || (ret < 0 && !zbaselib_socket_rw_wouldblock()))
	{
		return -1;
	}
	
	if(ret < 0)
		ret = 0;

	return ret;
}

static int inet_pton4(const char *src, unsigned char *dst) {
  static const char digits[] = "0123456789";
  int saw_digit, octets, ch;
  unsigned char tmp[sizeof(struct in_addr)], *tp;

  saw_digit = 0;
  octets = 0;
  *(tp = tmp) = 0;
  while ((ch = *src++) != '\0') {
    const char *pch;

    if ((pch = strchr(digits, ch)) != NULL) {
      unsigned int nw = *tp * 10 + (pch - digits);

      if (saw_digit && *tp == 0)
        return -1;
      if (nw > 255)
        return -1;
      *tp = nw;
      if (!saw_digit) {
        if (++octets > 4)
          return -1;
        saw_digit = 1;
      }
    } else if (ch == '.' && saw_digit) {
      if (octets == 4)
        return -1;
      *++tp = 0;
      saw_digit = 0;
    } else
      return -1;
  }
  if (octets < 4)
    return -1;
  memcpy(dst, tmp, sizeof(struct in_addr));
  return 0;
}


static int inet_pton6(const char *src, unsigned char *dst) {
  static const char xdigits_l[] = "0123456789abcdef",
                    xdigits_u[] = "0123456789ABCDEF";
  unsigned char tmp[sizeof(struct in6_addr)], *tp, *endp, *colonp;
  const char *xdigits, *curtok;
  int ch, seen_xdigits;
  unsigned int val;

  memset((tp = tmp), '\0', sizeof tmp);
  endp = tp + sizeof tmp;
  colonp = NULL;
  /* Leading :: requires some special handling. */
  if (*src == ':')
    if (*++src != ':')
      return -1;
  curtok = src;
  seen_xdigits = 0;
  val = 0;
  while ((ch = *src++) != '\0') {
    const char *pch;

    if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
      pch = strchr((xdigits = xdigits_u), ch);
    if (pch != NULL) {
      val <<= 4;
      val |= (pch - xdigits);
      if (++seen_xdigits > 4)
        return -1;
      continue;
    }
    if (ch == ':') {
      curtok = src;
      if (!seen_xdigits) {
        if (colonp)
          return -1;
        colonp = tp;
        continue;
      } else if (*src == '\0') {
        return -1;
      }
      if (tp + sizeof(uint16_t) > endp)
        return -1;
      *tp++ = (unsigned char) (val >> 8) & 0xff;
      *tp++ = (unsigned char) val & 0xff;
      seen_xdigits = 0;
      val = 0;
      continue;
    }
    if (ch == '.' && ((tp + sizeof(struct in_addr)) <= endp)) {
      int err = inet_pton4(curtok, tp);
      if (err == 0) {
        tp += sizeof(struct in_addr);
        seen_xdigits = 0;
        break;  /*%< '\\0' was seen by inet_pton4(). */
      }
    }
    return -1;
  }
  if (seen_xdigits) {
    if (tp + sizeof(uint16_t) > endp)
      return -1;
    *tp++ = (unsigned char) (val >> 8) & 0xff;
    *tp++ = (unsigned char) val & 0xff;
  }
  if (colonp != NULL) {
    /*
     * Since some memmove()'s erroneously fail to handle
     * overlapping regions, we'll do the shift by hand.
     */
    const int n = tp - colonp;
    int i;

    if (tp == endp)
      return -1;
    for (i = 1; i <= n; i++) {
      endp[- i] = colonp[n - i];
      colonp[n - i] = 0;
    }
    tp = endp;
  }
  if (tp != endp)
    return -1;
  memcpy(dst, tmp, sizeof tmp);
  return 0;
}

// 移植至libuv
int zbaselib_inet_pton(int af, const char *src, void *dst)
{
	if (src == NULL || dst == NULL)
		return -1;

	switch (af) 
	{
	case AF_INET:
		return (inet_pton4(src, dst));
	case AF_INET6: 
	{
		int len;
		char tmp[UV__INET6_ADDRSTRLEN], *s, *p;
		s = (char*) src;
		p = strchr(src, '%');
		if (p != NULL) 
		{
			s = tmp;
			len = p - src;
			if (len > UV__INET6_ADDRSTRLEN-1)
				return -1;
			memcpy(s, src, len);
			s[len] = '\0';
		}
		return inet_pton6(s, dst);
	}
	default:
		return -1;
	}
	/* NOTREACHED */
}

static int inet_ntop4(const unsigned char *src, char *dst, socklen_t size) {
  static const char fmt[] = "%u.%u.%u.%u";
  char tmp[UV__INET_ADDRSTRLEN];
  int l;

#ifndef _WIN32
  l = snprintf(tmp, sizeof(tmp), fmt, src[0], src[1], src[2], src[3]);
#else
  l = _snprintf(tmp, sizeof(tmp), fmt, src[0], src[1], src[2], src[3]);
#endif
  if (l <= 0 || (size_t) l >= size) {
    return -1;
  }
  strncpy(dst, tmp, size);
  dst[size - 1] = '\0';
  return 0;
}


static int inet_ntop6(const unsigned char *src, char *dst, socklen_t size) {
  /*
   * Note that int32_t and int16_t need only be "at least" large enough
   * to contain a value of the specified size.  On some systems, like
   * Crays, there is no such thing as an integer variable with 16 bits.
   * Keep this in mind if you think this function should have been coded
   * to use pointer overlays.  All the world's not a VAX.
   */
  char tmp[UV__INET6_ADDRSTRLEN], *tp;
  struct { int base, len; } best, cur;
  unsigned int words[sizeof(struct in6_addr) / sizeof(uint16_t)];
  int i;

  /*
   * Preprocess:
   *  Copy the input (bytewise) array into a wordwise array.
   *  Find the longest run of 0x00's in src[] for :: shorthanding.
   */
  memset(words, '\0', sizeof words);
  for (i = 0; i < (int) sizeof(struct in6_addr); i++)
    words[i / 2] |= (src[i] << ((1 - (i % 2)) << 3));
  best.base = -1;
  best.len = 0;
  cur.base = -1;
  cur.len = 0;
  for (i = 0; i < (int) ZBASE_ARRAY_SIZE(words); i++) {
    if (words[i] == 0) {
      if (cur.base == -1)
        cur.base = i, cur.len = 1;
      else
        cur.len++;
    } else {
      if (cur.base != -1) {
        if (best.base == -1 || cur.len > best.len)
          best = cur;
        cur.base = -1;
      }
    }
  }
  if (cur.base != -1) {
    if (best.base == -1 || cur.len > best.len)
      best = cur;
  }
  if (best.base != -1 && best.len < 2)
    best.base = -1;

  /*
   * Format the result.
   */
  tp = tmp;
  for (i = 0; i < (int) ZBASE_ARRAY_SIZE(words); i++) {
    /* Are we inside the best run of 0x00's? */
    if (best.base != -1 && i >= best.base &&
        i < (best.base + best.len)) {
      if (i == best.base)
        *tp++ = ':';
      continue;
    }
    /* Are we following an initial run of 0x00s or any real hex? */
    if (i != 0)
      *tp++ = ':';
    /* Is this address an encapsulated IPv4? */
    if (i == 6 && best.base == 0 && (best.len == 6 ||
        (best.len == 7 && words[7] != 0x0001) ||
        (best.len == 5 && words[5] == 0xffff))) {
      int err = inet_ntop4(src+12, tp, sizeof tmp - (tp - tmp));
      if (err)
        return err;
      tp += strlen(tp);
      break;
    }
    tp += sprintf(tp, "%x", words[i]);
  }
  /* Was it a trailing run of 0x00's? */
  if (best.base != -1 && (best.base + best.len) == ZBASE_ARRAY_SIZE(words))
    *tp++ = ':';
  *tp++ = '\0';

  /*
   * Check for overflow, copy, and we're done.
   */
  if ((size_t)(tp - tmp) > size) {
    return -1;
  }
  strcpy(dst, tmp);
  return 0;
}


int zbaselib_inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
	switch (af) 
	{
	case AF_INET:
		return (inet_ntop4(src, dst, size));
	case AF_INET6:
		return (inet_ntop6(src, dst, size));
	default:
		return -1;
	}
  	/* NOTREACHED */
}




/************************************************************************/
/* socket buffer:线性、可自增（不减小），与zbaselib_buffer有点相似          */
/************************************************************************/
#include "zbaselib_thread.h"

#define ZBASELIB_SKBUFFER_MIN_NUM	64

#define ZBASELIB_SKBUFFER_LOCK(buffer)	if(buffer->islock)	zbaselib_mutex_lock(buffer->mutex)
#define ZBASELIB_SKBUFFER_UNLOCK(buffer)	if(buffer->islock)	zbaselib_mutex_unlock(buffer->mutex)

struct _zbaselib_skbuffer
{
	zbaselib_socket_t sock;
	int islock;
	zbaselib_mutex_t mutex;
	char* buf;
	uint buf_size;
	uint buf_max_size;
	uint data_size;
	uint rpos;
	uint wpos;

	int sock_snd_buf_size;
};

zbaselib_skbuffer* zbaselib_skbuffer_create(uint buf_size, uint buf_max_size, int lock, zbaselib_socket_t sock)
{
	zbaselib_skbuffer* thiz = (zbaselib_skbuffer*) calloc(1, sizeof(zbaselib_skbuffer));
	if(thiz != NULL)
	{
		thiz->sock = sock;
		thiz->islock = lock;
		thiz->buf_size = (buf_size == 0 ? 128 : buf_size);
		thiz->buf_max_size = (buf_max_size > thiz->buf_size ? buf_max_size : (thiz->buf_size<<2));
		thiz->buf = (char*) calloc(thiz->buf_size, 1);

		if(zbaselib_socket_getoption(sock, OPT_SNDBUF, &thiz->sock_snd_buf_size) < 0)
			thiz->sock_snd_buf_size = 4096;
		
		if(lock)
			thiz->mutex = zbaselib_mutex_create();
	}

	return thiz;
}

void zbaselib_skbuffer_destroy(zbaselib_skbuffer* thiz)
{
	if(thiz != NULL)
	{
		if(thiz->islock)
			zbaselib_mutex_destroy(thiz->mutex);

		if(thiz->buf != NULL)
		{
			free(thiz->buf);
			thiz->buf = NULL;
		}

		free(thiz);
	}
}

// -2:参数错误
// -1:socket recv出错
// -3:buffer达到最大
//  0:成功
int zbaselib_skbuffer_recv(zbaselib_skbuffer* thiz)
{
	int recv_size = 0;
	int writable_size = 0;

	if(thiz == NULL || thiz->buf == NULL
		|| !zbaselib_socket_isvalid(thiz->sock))
	{
		return -2;
	}

	ZBASELIB_SKBUFFER_LOCK(thiz);

	//do
	//{
	// 写指针移到尾部了
	if(thiz->wpos >= thiz->buf_size)
	{
		// 前部有空闲空间先考虑往前挪
		if(thiz->rpos > 0)
		{
			memmove(thiz->buf, thiz->buf + thiz->rpos, thiz->data_size);
			thiz->wpos -= thiz->rpos;
			thiz->rpos = 0;
		}
		else
		{
			if(thiz->buf_size > thiz->buf_max_size)
			{
				MESSAGE("skbuffer size reach max size, buf_size = %d, max_size = %d\n",
					thiz->buf_size, thiz->buf_max_size);

				ZBASELIB_SKBUFFER_UNLOCK(thiz);
				return -3;
			}
			else
			{
				// 全部塞满扩容1.5倍
				int buf_size = thiz->buf_size + (thiz->buf_size>>1) + ZBASELIB_SKBUFFER_MIN_NUM;
				char* data = (char*)realloc(thiz->buf, buf_size);				
				thiz->buf = data;
				thiz->buf_size = buf_size;

				MESSAGE("expand data_size = %d, buf_size = %d\n", thiz->data_size, thiz->buf_size);
			}
		}
	}

	writable_size = thiz->buf_size - thiz->wpos;

#ifdef WIN32
	recv_size = recv(thiz->sock, thiz->buf + thiz->wpos, writable_size, 0);
#else
	recv_size = recv(thiz->sock, thiz->buf + thiz->wpos, writable_size, MSG_NOSIGNAL);//|MSG_DONTWAIT
#endif

	if(recv_size > 0)
	{
		thiz->wpos += recv_size;
		thiz->data_size += recv_size;
	}
	//}while(recv_size > 0/*!(出错或无数据可读)*/);
	
	ZBASELIB_SKBUFFER_UNLOCK(thiz);
	
	if(recv_size == 0 || (recv_size < 0 && !zbaselib_socket_rw_wouldblock()))
	{
		return -1;
	}
	
	return 0;
}

int zbaselib_skbuffer_udp_recv(zbaselib_skbuffer* thiz, struct sockaddr_in* addr)
{
	int recv_size = 0;
	int writable_size = 0;

	if(thiz == NULL || thiz->buf == NULL
		|| !zbaselib_socket_isvalid(thiz->sock))
	{
		return -2;
	}

	ZBASELIB_SKBUFFER_LOCK(thiz);

	do
	{
		// 写指针移到尾部了
		if(thiz->wpos >= thiz->buf_size)
		{
			// 前部有空闲空间先考虑往前挪
			if(thiz->rpos > 0)
			{
				memmove(thiz->buf, thiz->buf + thiz->rpos, thiz->data_size);
				thiz->wpos -= thiz->rpos;
				thiz->rpos = 0;
			}
			else
			{
				if(thiz->buf_size > thiz->buf_max_size)
				{
					MESSAGE("skbuffer size reach max size, buf_size = %d, max_size = %d\n",
						thiz->buf_size, thiz->buf_max_size);

					ZBASELIB_SKBUFFER_UNLOCK(thiz);
					return -3;
				}
				else
				{
					// 全部塞满扩容1.5倍
					int buf_size = thiz->buf_size + (thiz->buf_size>>1) + ZBASELIB_SKBUFFER_MIN_NUM;
					char* data = (char*)realloc(thiz->buf, buf_size);				
					thiz->buf = data;
					thiz->buf_size = buf_size;

					MESSAGE("expand data_size = %d, buf_size = %d\n", thiz->data_size, thiz->buf_size);
				}
			}
		}

		writable_size = thiz->buf_size - thiz->wpos;

#ifdef WIN32
		recv_size = recvfrom(thiz->sock, thiz->buf + thiz->wpos, writable_size, 0, (struct sockaddr*)addr, NULL);
#else
		recv_size = recvfrom(thiz->sock, thiz->buf + thiz->wpos, writable_size, MSG_NOSIGNAL, (struct sockaddr*)addr, NULL);//|MSG_DONTWAIT
#endif

		if(recv_size > 0)
		{
			thiz->wpos += recv_size;
			thiz->data_size += recv_size;
		}
	}while(recv_size > 0/*!(出错或无数据可读)*/);

	ZBASELIB_SKBUFFER_UNLOCK(thiz);

	if(recv_size == 0 || (recv_size < 0 && !zbaselib_socket_rw_wouldblock()))
	{
		return -1;
	}

	return 0;
}
// -2:参数错误
// -1:socket send出错
//  0:成功
int zbaselib_skbuffer_send(zbaselib_skbuffer* thiz)
{
	int send_size = 0;

	if(thiz == NULL || thiz->buf == NULL
		|| !zbaselib_socket_isvalid(thiz->sock))
	{
		return -2;
	}

	ZBASELIB_SKBUFFER_LOCK(thiz);

	if(thiz->data_size == 0)
	{
		ZBASELIB_SKBUFFER_UNLOCK(thiz);
		return 0;
	}

	send_size = (thiz->data_size < thiz->sock_snd_buf_size) ? thiz->data_size : thiz->sock_snd_buf_size;
#ifdef WIN32
	//send_size = send(thiz->sock, thiz->buf + thiz->rpos, thiz->data_size, 0);
	send_size = zbaselib_socket_sendn(thiz->sock, thiz->buf + thiz->rpos, send_size);
#else
	//send_size = send(thiz->sock, thiz->buf + thiz->rpos, thiz->data_size, MSG_NOSIGNAL);
	send_size = zbaselib_socket_sendn(thiz->sock, thiz->buf + thiz->rpos, send_size);
#endif

	if(send_size > 0)
	{
		thiz->rpos += send_size;
		thiz->data_size -= send_size;

		// 数据为空读写指针都移到首部
		if(thiz->data_size == 0 && thiz->rpos == thiz->wpos)
		{
			thiz->rpos = 0;
			thiz->wpos = 0;
		}
	
		ZBASELIB_SKBUFFER_UNLOCK(thiz);
		return 0;
	}
	else
	{
		ZBASELIB_SKBUFFER_UNLOCK(thiz);
		return -1;
	}
}

/*
0-成功
-1=失败
-2=剩余空间不够
*/
int zbaselib_skbuffer_write(zbaselib_skbuffer* thiz, const char* data ,uint size)
{
	if(thiz == NULL || thiz->buf == NULL || data == NULL)
	{
		return -1;
	}

	ZBASELIB_SKBUFFER_LOCK(thiz);
	
	if((thiz->wpos + size) > thiz->buf_size
		&& thiz->rpos > 0)
	{
		// 前部有空闲空间先考虑往前挪
		memmove(thiz->buf, thiz->buf + thiz->rpos, thiz->data_size);
		thiz->wpos -= thiz->rpos;
		thiz->rpos = 0;
	}

	if((thiz->wpos + size) > thiz->buf_size)
	{
		if(thiz->buf_size > thiz->buf_max_size)
		{
			MESSAGE("skbuffer size reach max size, buf_size = %d, max_size = %d\n",
				thiz->buf_size, thiz->buf_max_size);

			ZBASELIB_SKBUFFER_UNLOCK(thiz);
			return -2;
		}
		else
		{
			// 全部塞满扩容1.5倍
			int buf_size = thiz->buf_size + (thiz->buf_size>>1) + size;
			char* data = (char*)realloc(thiz->buf, buf_size);				
			thiz->buf = data;
			thiz->buf_size = buf_size;

			MESSAGE("expand data_size = %d, buf_size = %d\n", thiz->data_size, thiz->buf_size);
		}
	}

	memcpy(thiz->buf + thiz->wpos, data, size);
	thiz->wpos += size;
	thiz->data_size += size;

	ZBASELIB_SKBUFFER_UNLOCK(thiz);
	return 0;
}


// -1:失败
// >=0:返回读取的长度
int zbaselib_skbuffer_read(zbaselib_skbuffer* thiz, char* data, uint size)
{
	if(thiz == NULL || data == NULL || thiz->buf == NULL)
		return -1;
	
	ZBASELIB_SKBUFFER_LOCK(thiz);

	size = size < thiz->data_size ? size : thiz->data_size;
	if(size == 0)
	{
		ZBASELIB_SKBUFFER_UNLOCK(thiz);
		return 0;
	}

	memcpy(data, thiz->buf + thiz->rpos, size);
	thiz->rpos += size;
	thiz->data_size -= size;
	// 数据为空读写指针都移到首部
	if(thiz->data_size == 0 && thiz->rpos == thiz->wpos)
	{
		thiz->rpos = 0;
		thiz->wpos = 0;
	}

	ZBASELIB_SKBUFFER_UNLOCK(thiz);
	return size;
}

// -1:失败
// >0:返回读取的长度
// 与read函数相似，但不修改读指针
int zbaselib_skbuffer_get(zbaselib_skbuffer* thiz, char* data, uint size)
{
	if(thiz == NULL || data == NULL || thiz->buf == NULL)
		return -1;

	ZBASELIB_SKBUFFER_LOCK(thiz);

	size = size < thiz->data_size ? size : thiz->data_size;
	if(size == 0)
	{
		ZBASELIB_SKBUFFER_UNLOCK(thiz);
		return 0;
	}

	memcpy(data, thiz->buf + thiz->rpos, size);

	ZBASELIB_SKBUFFER_UNLOCK(thiz);
	return size;
}

// 跳过size字节的数据
// -1:失败
// >=0:返回跳过的长度
int zbaselib_skbuffer_skip(zbaselib_skbuffer* thiz, uint size)
{
	if(thiz == NULL || thiz->buf == NULL)
		return -1;

	ZBASELIB_SKBUFFER_LOCK(thiz);

	size = size < thiz->data_size ? size : thiz->data_size;
	if(size == 0)
	{
		ZBASELIB_SKBUFFER_UNLOCK(thiz);
		return 0;
	}

	thiz->rpos += size;
	thiz->data_size -= size;
	// 数据为空读写指针都移到首部
	if(thiz->data_size == 0 && thiz->rpos == thiz->wpos)
	{
		thiz->rpos = 0;
		thiz->wpos = 0;
	}
	
		
	ZBASELIB_SKBUFFER_UNLOCK(thiz);
	return size;
}


int zbaselib_skbuffer_datasize(zbaselib_skbuffer* thiz)
{
	int data_size = 0;

	if(thiz == NULL || thiz->buf == NULL)
		return -1;

	ZBASELIB_SKBUFFER_LOCK(thiz);
	data_size = thiz->data_size;
	ZBASELIB_SKBUFFER_UNLOCK(thiz);

	return data_size;
}

int zbaselib_skbuffer_empty(zbaselib_skbuffer* thiz)
{
	int ret = 0;

	if(thiz == NULL || thiz->buf == NULL)
		return 1;

	ZBASELIB_SKBUFFER_LOCK(thiz);
	ret = (thiz->data_size == 0);
	ZBASELIB_SKBUFFER_UNLOCK(thiz);
	return ret;
}


void zbaselib_skbuffer_clear(zbaselib_skbuffer* thiz)
{
	if(thiz == NULL || thiz->buf == NULL)
		return;

	ZBASELIB_SKBUFFER_LOCK(thiz);
	thiz->rpos = 0;
	thiz->wpos = 0;
	thiz->data_size = 0;
	ZBASELIB_SKBUFFER_UNLOCK(thiz);
}

const char* zbaselib_skbuffer_read_pos(zbaselib_skbuffer* thiz)
{
	char* read_pos = NULL;

	if(thiz == NULL || thiz->buf == NULL)
		return NULL;

	ZBASELIB_SKBUFFER_LOCK(thiz);
	read_pos = thiz->buf + thiz->rpos;
	ZBASELIB_SKBUFFER_UNLOCK(thiz);

	return read_pos;
}


/************************************************************************/
// 网卡管理, 移植libuv
/************************************************************************/
#ifdef WIN32


int uv_interface_addresses(uv_interface_address_t** addresses_ptr,
  int* count_ptr)
{
	IP_ADAPTER_ADDRESSES* win_address_buf;
	ULONG win_address_buf_size;
	IP_ADAPTER_ADDRESSES* win_address;

	uv_interface_address_t* uv_address_buf;
	char* name_buf;
	size_t uv_address_buf_size;
	uv_interface_address_t* uv_address;

	int count;

	/* Fetch the size of the adapters reported by windows, and then get the */
	/* list itself. */
	win_address_buf_size = 0;
	win_address_buf = NULL;

	for (;;) {
		ULONG r;

		/* If win_address_buf is 0, then GetAdaptersAddresses will fail with */
		/* ERROR_BUFFER_OVERFLOW, and the required buffer size will be stored in */
		/* win_address_buf_size. */
		r = GetAdaptersAddresses(AF_UNSPEC,
								 GAA_FLAG_INCLUDE_PREFIX,
								 NULL,
								 win_address_buf,
								 &win_address_buf_size);

		if (r == ERROR_SUCCESS)
		  break;

		free(win_address_buf);

		switch (r) {
		  case ERROR_BUFFER_OVERFLOW:
			/* This happens when win_address_buf is NULL or too small to hold */
			/* all adapters. */
			win_address_buf = malloc(win_address_buf_size);
			if (win_address_buf == NULL)
			  return -1;

			continue;

		  case ERROR_NO_DATA: {
			/* No adapters were found. */
			uv_address_buf = malloc(1);
			if (uv_address_buf == NULL)
			  return -1;

			*count_ptr = 0;
			*addresses_ptr = uv_address_buf;

			return 0;
		  }

		  case ERROR_ADDRESS_NOT_ASSOCIATED:
			return -1;

		  case ERROR_INVALID_PARAMETER:
			/* MSDN says:
			 *   "This error is returned for any of the following conditions: the
			 *   SizePointer parameter is NULL, the Address parameter is not
			 *   AF_INET, AF_INET6, or AF_UNSPEC, or the address information for
			 *   the parameters requested is greater than ULONG_MAX."
			 * Since the first two conditions are not met, it must be that the
			 * adapter data is too big.
			 */
			return -1;

		  default:
			/* Other (unspecified) errors can happen, but we don't have any */
			/* special meaning for them. */
			//assert(r != ERROR_SUCCESS);
			return -1;
		}
	}

	/* Count the number of enabled interfaces and compute how much space is */
	/* needed to store their info. */
	count = 0;
	uv_address_buf_size = 0;

	for (win_address = win_address_buf;
	   win_address != NULL;
	   win_address = win_address->Next) {
		/* Use IP_ADAPTER_UNICAST_ADDRESS_XP to retain backwards compatibility */
		/* with Windows XP */
		IP_ADAPTER_UNICAST_ADDRESS_XP* unicast_address;
		int name_size;

		/* Interfaces that are not 'up' should not be reported. Also skip */
		/* interfaces that have no associated unicast address, as to avoid */
		/* allocating space for the name for this interface. */
		if (win_address->OperStatus != IfOperStatusUp ||
			win_address->FirstUnicastAddress == NULL)
		  continue;

		/* Compute the size of the interface name. */
		name_size = WideCharToMultiByte(CP_UTF8,
										0,
										win_address->FriendlyName,
										-1,
										NULL,
										0,
										NULL,
										FALSE);
		if (name_size <= 0) {
		  free(win_address_buf);
		  return -1;
		}
		uv_address_buf_size += name_size;

		/* Count the number of addresses associated with this interface, and */
		/* compute the size. */
		for (unicast_address = (IP_ADAPTER_UNICAST_ADDRESS_XP*)
							   win_address->FirstUnicastAddress;
			 unicast_address != NULL;
			 unicast_address = unicast_address->Next) {
		  count++;
		  uv_address_buf_size += sizeof(uv_interface_address_t);
		}
	}

	/* Allocate space to store interface data plus adapter names. */
	uv_address_buf = malloc(uv_address_buf_size);
	if (uv_address_buf == NULL) {
		free(win_address_buf);
		return -1;
	}

	/* Compute the start of the uv_interface_address_t array, and the place in */
	/* the buffer where the interface names will be stored. */
	uv_address = uv_address_buf;
	name_buf = (char*) (uv_address_buf + count);

	/* Fill out the output buffer. */
	for (win_address = win_address_buf;
	   win_address != NULL;
	   win_address = win_address->Next) {
		IP_ADAPTER_UNICAST_ADDRESS_XP* unicast_address;
		IP_ADAPTER_PREFIX* prefix;
		int name_size;
		size_t max_name_size;

		if (win_address->OperStatus != IfOperStatusUp ||
			win_address->FirstUnicastAddress == NULL)
		  continue;

		/* Convert the interface name to UTF8. */
		max_name_size = (char*) uv_address_buf + uv_address_buf_size - name_buf;
		if (max_name_size > (size_t) INT_MAX)
		  max_name_size = INT_MAX;
		name_size = WideCharToMultiByte(CP_UTF8,
										0,
										win_address->FriendlyName,
										-1,
										name_buf,
										(int) max_name_size,
										NULL,
										FALSE);
		if (name_size <= 0) {
		  free(win_address_buf);
		  free(uv_address_buf);
		  return -1;
		}

		prefix = win_address->FirstPrefix;

		/* Add an uv_interface_address_t element for every unicast address. */
		/* Walk the prefix list in tandem with the address list. */
		for (unicast_address = (IP_ADAPTER_UNICAST_ADDRESS_XP*)
							   win_address->FirstUnicastAddress;
			 unicast_address != NULL && prefix != NULL;
			 unicast_address = unicast_address->Next, prefix = prefix->Next) {
		  struct sockaddr* sa;
		  ULONG prefix_len;

		  sa = unicast_address->Address.lpSockaddr;
		  prefix_len = prefix->PrefixLength;

		  memset(uv_address, 0, sizeof *uv_address);

		  uv_address->name = name_buf;

		  if (win_address->PhysicalAddressLength == sizeof(uv_address->phys_addr)) {
			memcpy(uv_address->phys_addr,
				   win_address->PhysicalAddress,
				   sizeof(uv_address->phys_addr));
		  }

		  uv_address->is_internal =
			  (win_address->IfType == IF_TYPE_SOFTWARE_LOOPBACK);

		  if (sa->sa_family == AF_INET6) {
			uv_address->address.address6 = *((struct sockaddr_in6 *) sa);

			uv_address->netmask.netmask6.sin6_family = AF_INET6;
			memset(uv_address->netmask.netmask6.sin6_addr.s6_addr, 0xff, prefix_len >> 3);
			uv_address->netmask.netmask6.sin6_addr.s6_addr[prefix_len >> 3] =
				0xff << (8 - prefix_len % 8);

		  } else {
			uv_address->address.address4 = *((struct sockaddr_in *) sa);

			uv_address->netmask.netmask4.sin_family = AF_INET;
			uv_address->netmask.netmask4.sin_addr.s_addr =
				htonl(0xffffffff << (32 - prefix_len));
		  }

		  uv_address++;
		}

		name_buf += name_size;
	}

	free(win_address_buf);

	*addresses_ptr = uv_address_buf;
	*count_ptr = count;

	return 0;
}


void uv_free_interface_addresses(uv_interface_address_t* addresses,
    int count) {
  free(addresses);
}

#else

int uv_interface_addresses(uv_interface_address_t** addresses,
  int* count)
{
	struct ifaddrs *addrs, *ent;
	uv_interface_address_t* address;
	int i;
	struct sockaddr_ll *sll;

	if (getifaddrs(&addrs))
		return -1;

	*count = 0;

	/* Count the number of interfaces */
	for (ent = addrs; ent != NULL; ent = ent->ifa_next) {
		if (!((ent->ifa_flags & IFF_UP) && (ent->ifa_flags & IFF_RUNNING)) ||
		    (ent->ifa_addr == NULL) ||
		    (ent->ifa_addr->sa_family == PF_PACKET)) {
		  continue;
		}

		(*count)++;
	}

	*addresses = malloc(*count * sizeof(**addresses));
	if (!(*addresses))
		return -1;

	address = *addresses;

	for (ent = addrs; ent != NULL; ent = ent->ifa_next) {
		if (!((ent->ifa_flags & IFF_UP) && (ent->ifa_flags & IFF_RUNNING)))
		  continue;

		if (ent->ifa_addr == NULL)
		  continue;

		/*
		 * On Linux getifaddrs returns information related to the raw underlying
		 * devices. We're not interested in this information yet.
		 */
		if (ent->ifa_addr->sa_family == PF_PACKET)
		  continue;

		address->name = strdup(ent->ifa_name);

		if (ent->ifa_addr->sa_family == AF_INET6) {
		  address->address.address6 = *((struct sockaddr_in6*) ent->ifa_addr);
		} else {
		  address->address.address4 = *((struct sockaddr_in*) ent->ifa_addr);
		}

		if (ent->ifa_netmask->sa_family == AF_INET6) {
		  address->netmask.netmask6 = *((struct sockaddr_in6*) ent->ifa_netmask);
		} else {
		  address->netmask.netmask4 = *((struct sockaddr_in*) ent->ifa_netmask);
		}

		address->is_internal = !!(ent->ifa_flags & IFF_LOOPBACK);

		address++;
	}

	/* Fill in physical addresses for each interface */
	for (ent = addrs; ent != NULL; ent = ent->ifa_next) {
		if (!((ent->ifa_flags & IFF_UP) && (ent->ifa_flags & IFF_RUNNING)) ||
		    (ent->ifa_addr == NULL) ||
		    (ent->ifa_addr->sa_family != PF_PACKET)) {
		  continue;
		}

		address = *addresses;

		for (i = 0; i < (*count); i++) {
		  if (strcmp(address->name, ent->ifa_name) == 0) {
		    sll = (struct sockaddr_ll*)ent->ifa_addr;
		    memcpy(address->phys_addr, sll->sll_addr, sizeof(address->phys_addr));
		  }
		  address++;
		}
	}

	freeifaddrs(addrs);

	return 0;
}

void uv_free_interface_addresses(uv_interface_address_t* addresses,
  int count)
{
	int i = 0;
	for (i = 0; i < count; i++) {
		free(addresses[i].name);
	}

	free(addresses);
}

#endif



