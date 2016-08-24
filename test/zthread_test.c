#include "zbaselib_thread.h"
#include "zbaselib.h"
#include <assert.h>

#define THREAD_NUMBER 2

int myglobal = 0;
zbaselib_mutex_t mutex;

zbaselib_thread_result ZBASELIB_THREAD_PROCSPEC fun(zbaselib_thread_param param)
{
	int i;
	for ( i=0; i<20; i++) {
		zbaselib_mutex_lock(mutex);
		myglobal=myglobal+1;
		printf(".");
		fflush(stdout);
		//zbaselib_sleep_msec(600);
		zbaselib_mutex_unlock(mutex);
	}
	
	return (zbaselib_thread_result)NULL;
}

int main(int argc, char* argv[])
{
	zbaselib_thread_t mythread;
	int i;

	mutex = zbaselib_mutex_create();
	assert(mutex != NULL);
	mythread = zbaselib_thread_create(fun, (zbaselib_thread_param)NULL);
	if(mythread == ZBASELIB_INVALID_THREAD)
	{
		printf("create thread fail\n");
		return 1;
	}

//	zbaselib_sleep_msec(4000);

	for ( i=0; i<20; i++) {
		zbaselib_mutex_lock(mutex);
		myglobal=myglobal+1;
		
		printf("o");
		fflush(stdout);
		zbaselib_mutex_unlock(mutex);
		//zbaselib_sleep_msec(500);
	}
	
	zbaselib_thread_join(mythread);
	
	printf("\nmyglobal equals %d\n",myglobal);
	zbaselib_mutex_destroy(mutex);

	getchar();
	return 0;
}