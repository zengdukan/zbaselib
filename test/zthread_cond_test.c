#include "zbaselib_thread.h"
#include "zbaselib.h"
#include <stdio.h>

typedef struct manager
{
	zbaselib_thread_t thid;
	zbaselib_cond_t cond;
	zbaselib_mutex_t mutex;
	int flag;
	int flag1;
} manager_t;

zbaselib_thread_result ZBASELIB_THREAD_PROCSPEC ThreadWork(zbaselib_thread_param param)
{
	manager_t* manager = (manager_t*)param;
	//struct timeval tv = {10, 0};
	int i = 0;

	while(manager->flag)
	{
		zbaselib_mutex_lock(manager->mutex);
		zbaselib_cond_wait(manager->cond, manager->mutex, NULL);
		zbaselib_mutex_unlock(manager->mutex);

		printf("start...\n");
		while(manager->flag1)
		{
			printf("i = %d\n", i++);
			zbaselib_sleep_msec(100);
		}
		printf("end...\n");

		manager->flag1 = 0;
	}

	return (zbaselib_thread_result)NULL;
}

int main(int argc, char* argv[])
{
	manager_t manager;
	char c;

	manager.mutex = zbaselib_mutex_create();
	manager.cond = zbaselib_cond_create();
	manager.flag = 1;
	manager.flag1 = 0;
	manager.thid = zbaselib_thread_create(ThreadWork, &manager);
	

	while((c = getchar()) != 'q')
	{
		switch(c)
		{
		case 'a':
			manager.flag1 = 1;
			zbaselib_mutex_lock(manager.mutex);
			zbaselib_cond_signal(manager.cond, 0);
			zbaselib_mutex_unlock(manager.mutex);
			break;
		case 'b':
			manager.flag1 = 0;
			break;
		}
		
	}

	manager.flag = 0;
	manager.flag1 = 0;
	zbaselib_cond_signal(manager.cond, 1);
	zbaselib_thread_join(manager.thid);
	zbaselib_cond_destroy(manager.cond);
	zbaselib_mutex_destroy(manager.mutex);
	

	return 0;
}