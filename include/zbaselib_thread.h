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

typedef HANDLE zbaselib_thread_t;                                    /// < �߳̾������
typedef DWORD  zbaselib_thread_result;                              /// < �̺߳�������ֵ����
typedef LPVOID zbaselib_thread_param;                               /// < �̺߳�����������
#define ZBASELIB_THREAD_PROCSPEC WINAPI                               /// < �̺߳�������Լ������
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
//������
//////////////////////////////////////////////////////////////////////////
ZLIB_API zbaselib_mutex_t zbaselib_mutex_create();

ZLIB_API void zbaselib_mutex_destroy(zbaselib_mutex_t mutex);

ZLIB_API int zbaselib_mutex_lock(zbaselib_mutex_t mutex);

ZLIB_API int zbaselib_mutex_unlock(zbaselib_mutex_t mutex);

ZLIB_API zbaselib_threadid_t zbaselib_thread_getid();

//////////////////////////////////////////////////////////////////////////
//������
//////////////////////////////////////////////////////////////////////////
ZLIB_API zbaselib_cond_t zbaselib_cond_create();

ZLIB_API void zbaselib_cond_destroy(zbaselib_cond_t cond);

ZLIB_API int zbaselib_cond_signal(zbaselib_cond_t cond, int broadcast);

ZLIB_API int zbaselib_cond_wait(zbaselib_cond_t cond, zbaselib_mutex_t lock, const struct timeval *tv);

//////////////////////////////////////////////////////////////////////////
//�߳�
//////////////////////////////////////////////////////////////////////////
#define ZBASELIB_THREAD_STACK_SIZE 256*1024

/// @brief �̺߳���ָ������
typedef zbaselib_thread_result (ZBASELIB_THREAD_PROCSPEC *ThreadFunc)(zbaselib_thread_param param);

/// @brief ����һ���߳�
/// @param threadProc �̺߳���
/// @param param �̺߳����Ĳ���
/// @return ʧ�ܷ���INVALID_THREAD,�ɹ������߳̾��
/// @see ThreadProc_T
ZLIB_API zbaselib_thread_t zbaselib_thread_create(ThreadFunc threadProc, zbaselib_thread_param param);

/// @brief �ȴ�һ���߳̽���������������
/// @param thread Ҫ�ȴ��������߳̾��
ZLIB_API void zbaselib_thread_join(zbaselib_thread_t thread);

//////////////////////////////////////////////////////////////////////////
//�̳߳�
//////////////////////////////////////////////////////////////////////////

typedef struct _zbaselib_threadpool_t zbaselib_threadpool_t;

/*�����̳߳�*/
ZLIB_API zbaselib_threadpool_t* zbaselib_threadpool_create(unsigned int size);

/*�������*/
ZLIB_API int zbaselib_threadpool_addtask(zbaselib_threadpool_t* thiz, 
	ThreadFunc threadFunc, zbaselib_thread_param param);

/*
*�����̳߳�
*finish:�����������е�ȫ������������
*/
ZLIB_API void zbaselib_threadpool_destroy(zbaselib_threadpool_t* thiz, int finish);

// 0 - æ��>0 - �����߳���
ZLIB_API int zbaselib_threadpool_status(zbaselib_threadpool_t* thiz);

ZBASELIB_END_DECL

#endif
