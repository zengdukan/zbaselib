#ifndef _ZBASELIB_THREAD_H_
#define _ZBASELIB_THREAD_H_

#include "zbaselib_typedef.h"

ZBASELIB_BEGIN_DECL

#ifdef WIN32
typedef CRITICAL_SECTION* zbaselib_mutex_t;
typedef DWORD zbaselib_threadid_t;

struct zbaselib_win32_cond {
	HANDLE event;

	CRITICAL_SECTION lock;
	int n_waiting;
	int n_to_wake;
	int generation;
};

typedef struct zbaselib_win32_cond* zbaselib_cond_t;

typedef HANDLE zbaselib_thread_t;                                    /// < 线程句柄类型
typedef DWORD  zbaselib_thread_result;                              /// < 线程函数返回值类型
typedef LPVOID zbaselib_thread_param;                               /// < 线程函数参数类型
#define ZBASELIB_THREAD_PROCSPEC WINAPI                               /// < 线程函数调用约定类型
#define ZBASELIB_INVALID_THREAD INVALID_HANDLE_VALUE
#else
typedef pthread_mutex_t* zbaselib_mutex_t;
typedef pthread_t zbaselib_threadid_t;
typedef pthread_cond_t* zbaselib_cond_t;
typedef pthread_t zbaselib_thread_t;
typedef void *    zbaselib_thread_result;
typedef void *    zbaselib_thread_param;
#define ZBASELIB_THREAD_PROCSPEC
#define ZBASELIB_INVALID_THREAD (zbaselib_thread_t)(~0)
#endif

//////////////////////////////////////////////////////////////////////////
//互斥锁
//////////////////////////////////////////////////////////////////////////
ZLIB_API zbaselib_mutex_t zbaselib_mutex_create();

ZLIB_API void zbaselib_mutex_destroy(zbaselib_mutex_t mutex);

ZLIB_API int zbaselib_mutex_lock(zbaselib_mutex_t mutex);

ZLIB_API int zbaselib_mutex_unlock(zbaselib_mutex_t mutex);

ZLIB_API zbaselib_threadid_t zbaselib_thread_getid();

//////////////////////////////////////////////////////////////////////////
//条件锁
//////////////////////////////////////////////////////////////////////////
ZLIB_API zbaselib_cond_t zbaselib_cond_create();

ZLIB_API void zbaselib_cond_destroy(zbaselib_cond_t cond);

ZLIB_API int zbaselib_cond_signal(zbaselib_cond_t cond, int broadcast);

ZLIB_API int zbaselib_cond_wait(zbaselib_cond_t cond, zbaselib_mutex_t lock, const struct timeval *tv);

//////////////////////////////////////////////////////////////////////////
//线程
//////////////////////////////////////////////////////////////////////////
#define ZBASELIB_THREAD_STACK_SIZE 256*1024

/// @brief 线程函数指针类型
typedef zbaselib_thread_result (ZBASELIB_THREAD_PROCSPEC *ThreadFunc)(zbaselib_thread_param param);

/// @brief 创建一个线程
/// @param threadProc 线程函数
/// @param param 线程函数的参数
/// @return 失败返回INVALID_THREAD,成功返回线程句柄
/// @see ThreadProc_T
ZLIB_API zbaselib_thread_t zbaselib_thread_create(ThreadFunc threadProc, zbaselib_thread_param param);

/// @brief 等待一个线程结束，并销毁它。
/// @param thread 要等待结束的线程句柄
ZLIB_API void zbaselib_thread_join(zbaselib_thread_t thread);

//////////////////////////////////////////////////////////////////////////
//线程池
//////////////////////////////////////////////////////////////////////////

typedef struct _zbaselib_threadpool_t zbaselib_threadpool_t;

/*创建线程池*/
ZLIB_API zbaselib_threadpool_t* zbaselib_threadpool_create(unsigned int size);

/*添加任务*/
ZLIB_API int zbaselib_threadpool_addtask(zbaselib_threadpool_t* thiz, 
	ThreadFunc threadFunc, zbaselib_thread_param param);

/*
*销毁线程池
*finish:完成任务队列中的全部任务再销毁
*/
ZLIB_API void zbaselib_threadpool_destroy(zbaselib_threadpool_t* thiz, int finish);

// 0 - 忙，>0 - 空闲线程数
ZLIB_API int zbaselib_threadpool_status(zbaselib_threadpool_t* thiz);

ZBASELIB_END_DECL

#endif
