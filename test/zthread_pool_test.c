#include "zbaselib_thread.h"
#include "zbaselib.h"
#include <assert.h>

zbaselib_thread_result ZBASELIB_THREAD_PROCSPEC func(zbaselib_thread_param arg)
{
	int id = *(int *)arg;
	int i = 0;	
	for(i = 0; i < id; i++)
	{
		printf("id = %d, index = %d\n", id, i);
		zbaselib_sleep_msec(1000);
	}

	return (zbaselib_thread_result)NULL;
}

zbaselib_thread_result ZBASELIB_THREAD_PROCSPEC pool_status(zbaselib_thread_param param)
{
	zbaselib_threadpool_t* pool = (zbaselib_threadpool_t*)param;

	for (; ;)
	{
		printf("pool status = %d\n", zbaselib_threadpool_status(pool));
		zbaselib_sleep_msec(1000);

		if(zbaselib_threadpool_status(pool) != 0)
		{
			int arg = rand()%20 + 1;
			zbaselib_threadpool_addtask(pool, func, (zbaselib_thread_param)&arg);
		}
	}
	
	return (zbaselib_thread_result)NULL;
}

int main(int argc, char *argv[])
{
	int size = 5;
	int i = 0;
	int id[7] = {0};

	zbaselib_threadpool_t* pool = zbaselib_threadpool_create(size);
	assert(pool != NULL);
	
	//zbaselib_threadpool_addtask(pool, pool_status, (zbaselib_thread_param)pool);
#if 1
	for(i = 0; i < 7; i++)
	{
		id[i] = (i+1) * 2;
		zbaselib_threadpool_addtask(pool, func, (zbaselib_thread_param)&id[i]);
		//sleep(1);
	}

	printf("add task ok\n");
#endif

	//sleep(10);
#if 0
	while(1)
	{
		zbaselib_sleep_msec(10000);
	}
#endif
	zbaselib_threadpool_destroy(pool, 1);

	return 0;
}

