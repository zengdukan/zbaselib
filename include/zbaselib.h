/**
* 1.socket
* 2.log工具
*/
#ifndef _ZBASELIB_H_
#define _ZBASELIB_H_

#include "zbaselib_typedef.h"

ZBASELIB_BEGIN_DECL

ZLIB_API int zbaselib_gettimeofday(struct timeval* tv);

/*
* 创建无名管道
* @psize:pipe缓存区大小,windows有用,linux无效
* @return:0-ok, -1:fail
*/
ZLIB_API int zbaselib_pipe(int pipefd[2], unsigned int psize);

ZLIB_API int zbaselib_pipe_close(int pipefd);


/*
* 创建多级目录 
* @path:目录路径, 最长256, 以'/'为目录分隔符
* @return: 0-ok, -1:fail
*/
ZLIB_API int zbaselib_createdir(const char* dirPath);

#ifdef WIN32
#define zbaselib_timeradd(tvp, uvp, vvp)					\
	do {								\
	(vvp)->tv_sec = (tvp)->tv_sec + (uvp)->tv_sec;		\
	(vvp)->tv_usec = (tvp)->tv_usec + (uvp)->tv_usec;       \
	if ((vvp)->tv_usec >= 1000000) {			\
	(vvp)->tv_sec++;				\
	(vvp)->tv_usec -= 1000000;			\
	}							\
	} while (0)

#define	zbaselib_timersub(tvp, uvp, vvp)					\
	do {								\
	(vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec;		\
	(vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec;	\
	if ((vvp)->tv_usec < 0) {				\
	(vvp)->tv_sec--;				\
	(vvp)->tv_usec += 1000000;			\
	}							\
	} while (0)

#define zbaselib_sleep_msec(n)		Sleep(n)

#define zbaselib_access _access
#define zbaselib_F_OK	0
#define zbaselib_R_OK	4
#define zbaselib_W_OK	2

#define zbaselib_mkdir(dir) _mkdir(dir)
#define zbaselib_stat	_stat
#define zbaselib_strdup	_strdup


typedef struct dirent {
	char	d_name[MAX_PATH];
} dirent;

typedef struct DIR {
	HANDLE			handle;
	WIN32_FIND_DATAW	info;
	struct dirent		result;
} DIR;

ZLIB_API DIR* opendir(const char *name);
ZLIB_API struct dirent* readdir(DIR *dirp);
ZLIB_API int closedir(DIR *dirp);

#else

#define zbaselib_timeradd(tvp, uvp, vvp) timeradd((tvp), (uvp), (vvp))

#define zbaselib_timersub(tvp, uvp, vvp) timersub((tvp), (uvp), (vvp))

#define zbaselib_sleep_msec(n)		usleep((n)*1000)

#define zbaselib_access access
#define zbaselib_F_OK	F_OK
#define zbaselib_R_OK	R_OK
#define zbaselib_W_OK	W_OK

#define zbaselib_mkdir(dir)	mkdir(dir, 0777)
#define zbaselib_stat	stat
#define zbaselib_strdup	strdup

#endif

/** Return true iff the tvp is related to uvp according to the relational
 * operator cmp.  Recognized values for cmp are ==, <=, <, >=, and >. */
#define	evutil_timercmp(tvp, uvp, cmp)					\
	(((tvp)->tv_sec == (uvp)->tv_sec) ?				\
	 ((tvp)->tv_usec cmp (uvp)->tv_usec) :				\
	 ((tvp)->tv_sec cmp (uvp)->tv_sec))


ZLIB_API llong zbaselib_tv_to_msec(const struct timeval *tv);

// 计算字符串hash值, 提取ngnix
#define ngx_hash(key, c)   ((unsigned int) key * 31 + c)
ZLIB_API unsigned int ngx_hash_key(unsigned char *data, size_t len);


ZBASELIB_END_DECL


#endif

