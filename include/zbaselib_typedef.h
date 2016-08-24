/* 
 * File:   zbaselib_typedef.h
 * Author: IBM
 *
 * Created on 2011年5月8日, 上午11:14
 */

#ifndef _ZBASELIB_TYPEDEF_H_
#define	_ZBASELIB_TYPEDEF_H_

#ifdef WIN32
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <io.h> 
#include <stdint.h>

#ifndef _WIN32_WINNT
/* Minimum required for InitializeCriticalSectionAndSpinCount */
#define _WIN32_WINNT _WIN32_WINNT_VISTA   
#endif
#include <winsock2.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#include <sys/locking.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <process.h>
#include <Mstcpip.h>
#include <Ws2ipdef.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#include <iprtrmib.h>
#include <ipmib.h>
#include <Iphlpapi.h>
#include <direct.h>
#include <fcntl.h>

#pragma comment (lib, "Ws2_32")

#ifdef DLL_EXPORT
#define ZLIB_API __declspec(dllexport)
#define EXPORT_API __declspec(dllexport)
#else
#define ZLIB_API 
#define EXPORT_API
#endif

#define CALLBACK	__stdcall

#else
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <dirent.h>
#include <stdint.h>


#define ZLIB_API 
#define EXPORT_API
#define CALLBACK	
#define __stdcall
#endif

typedef unsigned int    uint;
typedef unsigned long	ulong;
typedef unsigned short	ushort;
typedef unsigned long long ullong;
typedef long long llong;

#ifdef	__cplusplus
#define ZBASELIB_BEGIN_DECL extern "C" {
#define ZBASELIB_END_DECL   }
#else
#define ZBASELIB_BEGIN_DECL 
#define ZBASELIB_END_DECL   
#endif

#define ZBASE_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define DEBUG
#ifdef DEBUG
#define PRINT(...)	printf(__VA_ARGS__)
#define MESSAGE(x, ...) printf("[%s:%d] "x, __FUNCTION__, __LINE__, __VA_ARGS__);
#else
#define PRINT(...)
#define MESSAGE(x, ...)
#endif

#define ZBASE_DEBUG(fmt, ...)	\
	do {\
		time_t timep = time(NULL);	struct tm *ptm = localtime(&timep);\
		printf("[%04d-%02d-%02d %02d:%02d:%02d]: "fmt, (1900 + ptm->tm_year), (1 + ptm->tm_mon), \
		ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, ##__VA_ARGS__); \
		} while(0)


//访问函数指针，遍历list时使用
typedef void (*DatatVisitFunc)(void *data, void *ctx);
//比较函数指针,查找时使用
typedef int (*DataCmpFunc)(void *data, void *ctx);



#endif	/* _ZBASELIB_TYPEDEF_H_ */

