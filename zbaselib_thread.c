/**
*参考libevent的设计
*/
#include "zbaselib_thread.h"
#include "zbaselib.h"
#include "zbaselib_list.h"

#ifdef WIN32

#define SPIN_COUNT 2000

zbaselib_mutex_t zbaselib_mutex_create()
{
	zbaselib_mutex_t lock = (zbaselib_mutex_t)malloc(sizeof(CRITICAL_SECTION));

	if (!lock)
	{
		return NULL;
	}
	
	if (InitializeCriticalSectionAndSpinCount(lock, SPIN_COUNT) == 0)
	{
		free(lock);
		return NULL;
	}
	
	return lock;
}

void zbaselib_mutex_destroy(zbaselib_mutex_t mutex)
{
	if(mutex != NULL)
	{
		DeleteCriticalSection(mutex);
		free(mutex);
	}
}

int zbaselib_mutex_lock(zbaselib_mutex_t mutex)
{
	if(mutex != NULL)
	{
		EnterCriticalSection(mutex);
		return 0;
	}
	else
	{
		return -1;
	}
}


int zbaselib_mutex_unlock(zbaselib_mutex_t mutex)
{
	if(mutex != NULL)
	{
		LeaveCriticalSection(mutex);
		return 0;
	}
	else
	{
		return -1;
	}
}

zbaselib_threadid_t zbaselib_thread_getid()
{
	return GetCurrentThreadId();
}

zbaselib_cond_t zbaselib_cond_create()
{
	struct zbaselib_win32_cond *cond;
	if (!(cond = (malloc(sizeof(struct zbaselib_win32_cond)))))
		return NULL;
	if (InitializeCriticalSectionAndSpinCount(&cond->lock, SPIN_COUNT)==0)
	{
		free(cond);
		return NULL;
	}
	if ((cond->event = CreateEvent(NULL,TRUE,FALSE,NULL)) == NULL) 
	{
		DeleteCriticalSection(&cond->lock);
		free(cond);
		return NULL;
	}
	cond->n_waiting = cond->n_to_wake = cond->generation = 0;
	return cond;
}

void zbaselib_cond_destroy(zbaselib_cond_t cond)
{
	if(cond != NULL)
	{
		DeleteCriticalSection(&cond->lock);
		CloseHandle(cond->event);
		free(cond);
	}
}

int zbaselib_cond_signal(zbaselib_cond_t cond, int broadcast)
{
	if(cond == NULL)
		return -1;

	EnterCriticalSection(&cond->lock);
	if (broadcast)
		cond->n_to_wake = cond->n_waiting;
	else
		++cond->n_to_wake;
	cond->generation++;
	SetEvent(cond->event);
	LeaveCriticalSection(&cond->lock);
	return 0;
}

int zbaselib_cond_wait(zbaselib_cond_t cond, zbaselib_mutex_t lock, const struct timeval *tv)
{
	int generation_at_start;
	int waiting = 1;
	int result = -1;
	DWORD ms = INFINITE, ms_orig = INFINITE, startTime, endTime;
	if (tv)
		ms_orig = ms = zbaselib_tv_to_msec(tv);

	EnterCriticalSection(&cond->lock);
	++cond->n_waiting;
	generation_at_start = cond->generation;
	LeaveCriticalSection(&cond->lock);

	LeaveCriticalSection(lock);

	startTime = GetTickCount();
	do {
		DWORD res;
		res = WaitForSingleObject(cond->event, ms);
		EnterCriticalSection(&cond->lock);
		if (cond->n_to_wake &&
		    cond->generation != generation_at_start) {
			--cond->n_to_wake;
			--cond->n_waiting;
			result = 0;
			waiting = 0;
			goto out;
		} else if (res != WAIT_OBJECT_0) {
			result = (res==WAIT_TIMEOUT) ? 1 : -1;
			--cond->n_waiting;
			waiting = 0;
			goto out;
		} else if (ms != INFINITE) {
			endTime = GetTickCount();
			if (startTime + ms_orig <= endTime) {
				result = 1; /* Timeout */
				--cond->n_waiting;
				waiting = 0;
				goto out;
			} else {
				ms = startTime + ms_orig - endTime;
			}
		}
		/* If we make it here, we are still waiting. */
		if (cond->n_to_wake == 0) {
			/* There is nobody else who should wake up; reset
			 * the event. */
			ResetEvent(cond->event);
		}
	out:
		LeaveCriticalSection(&cond->lock);
	} while (waiting);

	EnterCriticalSection(lock);

	EnterCriticalSection(&cond->lock);
	if (!cond->n_waiting)
		ResetEvent(cond->event);
	LeaveCriticalSection(&cond->lock);

	return result;
}

/// @brief 创建一个线程
/// @param threadProc 线程函数
/// @param param 线程函数的参数
/// @return 失败返回INVALID_THREAD,成功返回线程句柄
/// @see ThreadProc_T
zbaselib_thread_t zbaselib_thread_create(ThreadFunc threadProc, zbaselib_thread_param param)
{
	return (HANDLE) _beginthreadex(NULL, ZBASELIB_THREAD_STACK_SIZE, (unsigned (WINAPI *)(void *)) threadProc, param, 0, NULL);
}

/// @brief 等待一个线程结束，并销毁它。
/// @param thread 要等待结束的线程句柄
void zbaselib_thread_join(zbaselib_thread_t thread)
{
	WaitForSingleObject(thread, INFINITE);
	CloseHandle(thread);
}

#else

zbaselib_mutex_t zbaselib_mutex_create()
{
	zbaselib_mutex_t lock = (zbaselib_mutex_t)malloc(sizeof(pthread_mutex_t));

	if (!lock)
	{
		return NULL;
	}
	
	if (pthread_mutex_init(lock, NULL)) 
	{
		free(lock);
		return NULL;
	}
	
	return lock;
}

void zbaselib_mutex_destroy(zbaselib_mutex_t mutex)
{
	if(mutex != NULL)
	{
		pthread_mutex_destroy(mutex);
		free(mutex);
	}
}

int zbaselib_mutex_lock(zbaselib_mutex_t mutex)
{
	if(mutex == NULL)
	{
		return -1;
	}

	return pthread_mutex_lock(mutex);
}

int zbaselib_mutex_unlock(zbaselib_mutex_t mutex)
{
	if(mutex == NULL)
	{
		return -1;
	}

	return pthread_mutex_unlock(mutex);
}

zbaselib_threadid_t zbaselib_thread_getid()
{
	return pthread_self();
}

zbaselib_cond_t zbaselib_cond_create()
{
	pthread_cond_t* cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
	if (!cond)
		return NULL;
	if (pthread_cond_init(cond, NULL)) {
		free(cond);
		return NULL;
	}
	return cond;
}

void zbaselib_cond_destroy(zbaselib_cond_t cond)
{
	if(cond != NULL)
	{
	//	pthread_cond_destroy(cond);
		free(cond);
	}
}

int zbaselib_cond_signal(zbaselib_cond_t cond, int broadcast)
{
	if(cond == NULL)
		return -1;
	
	int r;
	if (broadcast)
		r = pthread_cond_broadcast(cond);
	else
		r = pthread_cond_signal(cond);
	return r ? -1 : 0;
}

int zbaselib_cond_wait(zbaselib_cond_t cond, zbaselib_mutex_t lock, const struct timeval *tv)
{
	if(cond == NULL)
		return -1;

	int r;

	if (tv) 
	{
		struct timeval now, abstime;
		struct timespec ts;
		zbaselib_gettimeofday(&now);
		zbaselib_timeradd(&now, tv, &abstime);
		ts.tv_sec = abstime.tv_sec;
		ts.tv_nsec = abstime.tv_usec*1000;
		r = pthread_cond_timedwait(cond, lock, &ts);
		if (r == ETIMEDOUT)
			return 1;
		else if (r)
			return -1;
		else
			return 0;
	} 
	else 
	{
		r = pthread_cond_wait(cond, lock);
		return r ? -1 : 0;
	}
}

/// @brief 创建一个线程
/// @param threadProc 线程函数
/// @param param 线程函数的参数
/// @return 失败返回INVALID_THREAD,成功返回线程句柄
/// @see ThreadProc_T
zbaselib_thread_t zbaselib_thread_create(ThreadFunc threadProc, zbaselib_thread_param param)
{
	pthread_attr_t thread_attr;
	zbaselib_thread_t thread;
	int ret;

	pthread_attr_init(&thread_attr);
	pthread_attr_setstacksize(&thread_attr, ZBASELIB_THREAD_STACK_SIZE);
	ret = pthread_create(&thread, &thread_attr, threadProc, param);
	pthread_attr_destroy(&thread_attr);

	return (ret == 0) ? thread : ZBASELIB_INVALID_THREAD;
}

/// @brief 等待一个线程结束，并销毁它。
/// @param thread 要等待结束的线程句柄
void zbaselib_thread_join(zbaselib_thread_t thread)
{
	pthread_join(thread, NULL);
}

#endif

//////////////////////////////////////////////////////////////////////////
//线程池
//////////////////////////////////////////////////////////////////////////
//最大任务并发数
#define ZBASELIB_THREADPOOL_MAX	20

struct _zbaselib_threadpool_t
{
	zbaselib_thread_t* tid;				// 线程id
	int* th_status;						// 线程状态，1-忙， 0-空闲, -1:未就绪
	unsigned int size;					// 线程池大小
	zbaselib_mutex_t mutex;				// 
	zbaselib_cond_t task_empty;			// 任务空
	zbaselib_cond_t task_not_empty;		// 任务非空
	zbaselib_list* task_queue;			// 任务队列
	int stop;							// 停止标志, 1-停止线程池
};

struct _zbaselib_threadpool_task
{
	ThreadFunc job;
	zbaselib_thread_param param;
};
typedef struct _zbaselib_threadpool_task zbaselib_threadpool_task;

typedef struct _zbaselib_threadpool_channel
{
	zbaselib_threadpool_t* pool;
	int channel;
}zbaselib_threadpool_channel;

static void destroy_threadpool_task(void* data)
{
	free(data);
}

/*任务线程*/
static zbaselib_thread_result ZBASELIB_THREAD_PROCSPEC zbaselib_threadpool_func(zbaselib_thread_param arg)
{
	zbaselib_threadpool_channel channel = *(zbaselib_threadpool_channel*)arg;
	zbaselib_threadpool_t* pool = channel.pool;

	pool->th_status[channel.channel] = 0;
	while(1)
	{
		zbaselib_threadpool_task* head = NULL;
		if(0 != zbaselib_mutex_lock(pool->mutex))
		{
			continue;
		}
		
		if(pool->stop)
		{
			PRINT("pool is stop\n");
			zbaselib_mutex_unlock(pool->mutex);
			return (zbaselib_thread_result)NULL;
		}

		PRINT("add lock ok\n");

		//当任务队列为空时等待
		while(!pool->stop && zbaselib_list_empty(pool->task_queue))
		{	
			PRINT("threadpool:%d sleep\n", channel.channel);
			zbaselib_cond_wait(pool->task_empty, pool->mutex, NULL);
		}

		PRINT("threadpool:%d awake, %d\n", channel.channel, zbaselib_list_size(pool->task_queue));

		
		head = (zbaselib_threadpool_task *)zbaselib_list_gethead(pool->task_queue);
		if(head != NULL)
		{
			zbaselib_threadpool_task task = *head;
			
			//从任务队列中删除任务
			zbaselib_list_delhead(pool->task_queue);
				
			if(zbaselib_list_empty(pool->task_queue))
			{
				zbaselib_cond_signal(pool->task_not_empty, 0);
			}
			
			pool->th_status[channel.channel] = 1;
			zbaselib_mutex_unlock(pool->mutex);

			//执行任务
			if(task.job != NULL)
			{
				task.job(task.param);
			}

			PRINT("threadpool:task %d work finish\n", channel.channel);
			
			zbaselib_mutex_lock(pool->mutex);
			pool->th_status[channel.channel] = 0;
			zbaselib_mutex_unlock(pool->mutex);
		}
		else
		{
			zbaselib_list_delhead(pool->task_queue);
			zbaselib_mutex_unlock(pool->mutex);
		}
	}

	return (zbaselib_thread_result)NULL;
}

/*创建线程池*/
zbaselib_threadpool_t* zbaselib_threadpool_create(unsigned int size)
{
	int i = 0;

	zbaselib_threadpool_channel* tp_channel = NULL;
	zbaselib_threadpool_t* thiz = (zbaselib_threadpool_t*)malloc(sizeof(zbaselib_threadpool_t));
	if(thiz == NULL)
	{
		return NULL;
	}

	memset((void*)thiz, 0, sizeof(zbaselib_threadpool_t));
	thiz->size = (size < ZBASELIB_THREADPOOL_MAX ? size : ZBASELIB_THREADPOOL_MAX);
	thiz->tid = (zbaselib_thread_t*)malloc(sizeof(zbaselib_thread_t) * thiz->size);
	thiz->th_status = (int*)calloc(sizeof(int), thiz->size);
	thiz->stop = 0;
	
	thiz->mutex = zbaselib_mutex_create();
	if(thiz->mutex == NULL)
		goto ERROR_EXIT;
	
	thiz->task_empty = zbaselib_cond_create();
	if(thiz->task_empty == NULL)
		goto ERROR_EXIT;
	
	thiz->task_not_empty = zbaselib_cond_create();
	if(thiz->task_not_empty == NULL)
		goto ERROR_EXIT;
	
	thiz->task_queue = zbaselib_list_create(destroy_threadpool_task);
	if(thiz->task_queue == NULL)
		goto ERROR_EXIT;
	
	tp_channel = (zbaselib_threadpool_channel*)calloc(sizeof(zbaselib_threadpool_channel), thiz->size);
	// 创建任务线程
	for(i = 0; i < thiz->size; i++)
	{
		tp_channel[i].pool = thiz;
		tp_channel[i].channel = i;
		thiz->th_status[i] = -1;		//设置为未就绪状态
		thiz->tid[i] = zbaselib_thread_create(zbaselib_threadpool_func, (zbaselib_thread_param)&tp_channel[i]);
		if(thiz->tid[i] == ZBASELIB_INVALID_THREAD)
		{
			goto ERROR_EXIT;
		}

		PRINT("threadpool: thread id = %lx\n", thiz->tid[i]);
	}

	// 等待工作线程全部就绪，释放内存
	while(1)
	{
		int ret = 0;
		for(i = 0; i < thiz->size; i++)
		{
			if(thiz->th_status[i] == -1)
			{
				ret = 1;
				break;
			}
		}

		if(ret == 1)
			zbaselib_sleep_msec(1000);
		else
		{
			free(tp_channel);
			break;
		}
	}
	
	return thiz;
	
ERROR_EXIT:
	if(thiz->tid != NULL)
		free(thiz->tid);

	if(thiz->th_status != NULL)
		free(thiz->th_status);

	if(thiz->mutex != NULL)
		zbaselib_mutex_destroy(thiz->mutex);

	if(thiz->task_empty != NULL)
		zbaselib_cond_destroy(thiz->task_empty);

	if(thiz->task_not_empty != NULL)
		zbaselib_cond_destroy(thiz->task_not_empty);

	if(thiz->task_queue != NULL)
		zbaselib_list_destroy(&thiz->task_queue);

	free(thiz);
	return NULL;
}

/*添加任务*/
int zbaselib_threadpool_addtask(zbaselib_threadpool_t* thiz, 
	ThreadFunc threadFunc, zbaselib_thread_param param)
{
	zbaselib_threadpool_task* task = NULL;

	if(thiz == NULL || threadFunc == NULL)
	{
		return -1;
	}
	
	zbaselib_mutex_lock(thiz->mutex);

	if(thiz->stop)
	{
		zbaselib_mutex_unlock(thiz->mutex);
		return -1;
	}
	
	task = (zbaselib_threadpool_task *)calloc(sizeof(zbaselib_threadpool_task), 1);
	task->job = threadFunc;
	task->param = param;

	zbaselib_list_addtail(thiz->task_queue, (void *)task);
	
	zbaselib_cond_signal(thiz->task_empty, 0);
	zbaselib_mutex_unlock(thiz->mutex);
	
	return 0;
}

/*
*销毁线程池
*finish:完成任务队列中的全部任务再销毁
*/
void zbaselib_threadpool_destroy(zbaselib_threadpool_t* thiz, int finish)
{
	int i = 0;
	if(thiz == NULL)
	{
		return;
	}
	
	zbaselib_mutex_lock(thiz->mutex);

	if(thiz->stop)
	{
		zbaselib_mutex_unlock(thiz->mutex);
		return;
	}

	if(finish == 1)
	{
		while(!zbaselib_list_empty(thiz->task_queue))
		{
			zbaselib_cond_wait(thiz->task_not_empty, thiz->mutex, NULL);
		}
	}
	
	thiz->stop = 1;
	zbaselib_cond_signal(thiz->task_empty, 1);
	zbaselib_sleep_msec(1);	
	zbaselib_mutex_unlock(thiz->mutex);
	
	for(i = 0; i < thiz->size; i++)
	{
		zbaselib_thread_join(thiz->tid[i]);
	}
	
	zbaselib_list_destroy(&thiz->task_queue);

	zbaselib_mutex_destroy(thiz->mutex);
	zbaselib_cond_destroy(thiz->task_empty);
	zbaselib_cond_destroy(thiz->task_not_empty);
	free(thiz->tid);
	free(thiz->th_status);
	
	free(thiz);
}

int zbaselib_threadpool_status(zbaselib_threadpool_t* thiz)
{
	int idle_num = 0;
	int i = 0;

	if(thiz == NULL)
	{
		return -1;
	}

	zbaselib_mutex_lock(thiz->mutex);
	for(i = 0; i < thiz->size; i++)
	{
		if(thiz->th_status[i] == 0)
		{
			idle_num++;
		}
	}
	zbaselib_mutex_unlock(thiz->mutex);

	return idle_num;
}
