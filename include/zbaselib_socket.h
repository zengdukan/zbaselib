#ifndef _ZBASELIB_SOCKET_H_
#define _ZBASELIB_SOCKET_H_

#include "zbaselib.h"

ZBASELIB_BEGIN_DECL

#ifdef WIN32

#ifdef DLL_EXPORT
#define ZLIB_API __declspec(dllexport)
#else
#define ZLIB_API 
#endif

typedef SOCKET zbaselib_socket_t;

#define ZBASELIB_SOCKET_SHUTDOWN_RD		SD_RECEIVE
#define ZBASELIB_SOCKET_SHUTDOWN_WR		SD_SEND
#define ZBASELIB_SOCKET_SHUTDOWN_BOTH	SD_BOTH

#define socklen_t int

#define zbaselib_socket_geterror()	WSAGetLastError()

#else

#define ZLIB_API 

typedef int zbaselib_socket_t;

#define ZBASELIB_SOCKET_SHUTDOWN_RD		SHUT_RD
#define ZBASELIB_SOCKET_SHUTDOWN_WR		SHUT_WR
#define ZBASELIB_SOCKET_SHUTDOWN_BOTH	SHUT_RDWR

#define zbaselib_socket_geterror()	(errno)
#define INVALID_SOCKET (~0)
#endif

typedef enum  {
   OPT_DONTFRAGMENT,
   OPT_RCVBUF,		// receive buffer size
   OPT_SNDBUF,		// send buffer size
   OPT_NODELAY, 	// whether Nagle algorithm is enabled
   OPT_IPV6_V6ONLY	// Whether the socket is IPv6 only.
} sock_option;


// ��ʼ��,windowsҪ���ؿ�
ZLIB_API int zbaselib_socket_init();

// ע��
ZLIB_API void zbaselib_socket_deinit();

// libevent:evutil_closesocket
ZLIB_API int zbaselib_socket_close(zbaselib_socket_t sock);

ZLIB_API int zbaselib_socket_shutdown(zbaselib_socket_t sock, int how);

ZLIB_API int zbaselib_socket_nonblocking(zbaselib_socket_t sock);

ZLIB_API int zbaselib_socket_reusebale(zbaselib_socket_t sock);

/*
*@idlemsec:����keepalive����ǰ������Ӧʱ��
*@keepmsec:����keepalive������ʱ��
*/
ZLIB_API int zbaselib_socket_keepalive(zbaselib_socket_t sock, uint idlemsec, uint keepmsec);

ZLIB_API zbaselib_socket_t zbaselib_socket_create_tcpclient(const struct sockaddr_in* addr);

ZLIB_API zbaselib_socket_t zbaselib_socket_create_tcpserver(const struct sockaddr_in* addr);

// �ж��Ƿ����Ϸ�����
ZLIB_API int zbaselib_socket_waitforconnect(zbaselib_socket_t sock, int msec);

ZLIB_API int zbaselib_socket_isvalid(zbaselib_socket_t sock);

ZLIB_API void zbaselib_socket_setaddr(struct sockaddr_in* addr, uint ip, ushort port);

ZLIB_API int zbaselib_socket_rw_wouldblock();

ZLIB_API int zbaselib_socket_connect_wouldblock();

ZLIB_API int zbaselib_socket_accept_wouldblock();

// ���ͣ�AF_INET, TCP_STREAM
ZLIB_API int zbaselib_socket_pair(zbaselib_socket_t pair[2]);

// ��ȫ����size�ֽ�����
ZLIB_API int zbaselib_socket_sendn(zbaselib_socket_t sock, const char* buf, uint size);

// >=0:send size, С��size��ʾ��������û���, -1:error
ZLIB_API int zbaselib_socket_sendn_retry(zbaselib_socket_t sock, const char* buf, uint size, int retry);


ZLIB_API int zbaselib_socket_udp_sendn(zbaselib_socket_t sock, const char* buf, uint size, 
	const struct sockaddr_in* svr_addr);

ZLIB_API int zbaselib_socket_recv(zbaselib_socket_t sock, char* buf, uint size);

ZLIB_API int zbaselib_socket_getoption(zbaselib_socket_t sock, sock_option opt, int* value);

ZLIB_API int zbaselib_socket_setoption(zbaselib_socket_t sock, sock_option opt, int value);

ZLIB_API int zbaselib_socket_udp_broadcast(zbaselib_socket_t fd, int value);

/*
* ��ȡ������ip��ַ
* src_server = �����������IP�ַ���
* dst_server_ip=�Ե��ʮ���Ƶ�ip��ַ
* dst_ip = һ����������IP ��ַ
*/
ZLIB_API int zbaselib_socket_getipbyname(const char* src_server, char* dst_server_ip, int* dst_ip);

ZLIB_API int zbaselib_inet_pton(int af, const char *src, void *dst);
ZLIB_API int zbaselib_inet_ntop(int af, const void *src, char *dst, socklen_t size);

#ifndef WIN32
ZLIB_API int zbaselib_inet_nic_up(char *ifname);
ZLIB_API int zbaselib_inet_nic_down(char *ifname);
ZLIB_API int zbaselib_inet_get_flag(char *ifname);
ZLIB_API in_addr_t zbaselib_inet_get_ifaddr(char *ifname);
ZLIB_API struct in_addr zbaselib_inet_get_ip(int skfd, char *ifname);
ZLIB_API int zbaselib_inet_set_ifaddr(char *ifname, in_addr_t addr);
ZLIB_API in_addr_t zbaselib_inet_get_netmask(char *ifname);
ZLIB_API int zbaselib_inet_set_netmask(char *ifname, in_addr_t addr);
ZLIB_API int zbaselib_inet_get_hwaddr(char *ifname, unsigned char *mac);
ZLIB_API int zbaselib_inet_get_gateway(char* ifname, in_addr_t* gw);

#if 0
ZLIB_API int zbaselib_inet_set_gateway(in_addr_t addr);
ZLIB_API int zbaselib_inet_clean_gateway(void);
#endif

/*
* ��ȡdns
* @return:>0:����dns������<=0:fail
*/
ZLIB_API int zbaselib_inet_get_dns(in_addr_t dns[2]);
ZLIB_API int zbaselib_inet_set_dns(char *dnsname1, char* dnsname2);

#endif

/************************************************************************/
/* socket buffer:���ԡ�������������С������zbaselib_buffer�е�����          */
/************************************************************************/

typedef struct _zbaselib_skbuffer zbaselib_skbuffer;

/*
buf_size:buffer��ʼ����С
buf_max_size:���buffer��С������С��buf_size�����ռ�ﵽbuf_max_size�������󣬷�ֹ��������
*/
ZLIB_API zbaselib_skbuffer* zbaselib_skbuffer_create(uint buf_size, uint buf_max_size, int lock, zbaselib_socket_t sock);

ZLIB_API void zbaselib_skbuffer_destroy(zbaselib_skbuffer* thiz);

// -2:��������
// -1:socket recv����
// -3:buffer�ﵽ���
//  0:�ɹ�
ZLIB_API int zbaselib_skbuffer_recv(zbaselib_skbuffer* thiz);
ZLIB_API int zbaselib_skbuffer_udp_recv(zbaselib_skbuffer* thiz, struct sockaddr_in* addr);

// -2:��������
// -1:socket send����
//  0:�ɹ�
ZLIB_API int zbaselib_skbuffer_send(zbaselib_skbuffer* thiz);

/*
0-�ɹ�
-1=ʧ��
-2=ʣ��ռ䲻��
*/
ZLIB_API int zbaselib_skbuffer_write(zbaselib_skbuffer* thiz, const char* data ,uint size);

// -1:ʧ��
// >=0:���ض�ȡ�ĳ���
ZLIB_API int zbaselib_skbuffer_read(zbaselib_skbuffer* thiz, char* data, uint size);

// -1:ʧ��
// >0:���ض�ȡ�ĳ���
// ��read�������ƣ������޸Ķ�ָ��
ZLIB_API int zbaselib_skbuffer_get(zbaselib_skbuffer* thiz, char* data, uint size);

// ����size�ֽڵ�����
// -1:ʧ��
// >=0:���������ĳ���
ZLIB_API int zbaselib_skbuffer_skip(zbaselib_skbuffer* thiz, uint size);

ZLIB_API int zbaselib_skbuffer_datasize(zbaselib_skbuffer* thiz);

ZLIB_API int zbaselib_skbuffer_empty(zbaselib_skbuffer* thiz);

ZLIB_API void zbaselib_skbuffer_clear(zbaselib_skbuffer* thiz);

ZLIB_API const char* zbaselib_skbuffer_read_pos(zbaselib_skbuffer* thiz);

/************************************************************************/
// ��������, ��ֲlibuv
/************************************************************************/

typedef struct uv_interface_address_s {
  char* name;
  char phys_addr[6];
  int is_internal;
  union {
    struct sockaddr_in address4;
    struct sockaddr_in6 address6;
  } address;
  union {
    struct sockaddr_in netmask4;
    struct sockaddr_in6 netmask6;
  } netmask;
} uv_interface_address_t;


/*
 * This allocates addresses array, and sets count. The array is freed
 * using uv_free_interface_addresses().
 */
ZLIB_API int uv_interface_addresses(uv_interface_address_t** addresses,
  int* count);
ZLIB_API void uv_free_interface_addresses(uv_interface_address_t* addresses,
  int count);


ZBASELIB_END_DECL

#endif
