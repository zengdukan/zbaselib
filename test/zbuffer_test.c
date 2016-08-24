#include "zbaselib_buffer.h"
#include "zbaselib_thread.h"
#include "zbaselib.h"

zbaselib_buffer* buf = NULL;

zbaselib_thread_result ZBASELIB_THREAD_PROCSPEC thread_read(zbaselib_thread_param arg)
{
	//zbuffer *buf = (zbuffer *)arg;
	int tmp[1000] = {0};
	int index = 0;
	while(1)
	{		
		memset(tmp, 0, sizeof(tmp));
		if(zbaselib_buffer_read(buf,(void *)tmp, sizeof(tmp)) == 0)
		{
			for(index = 0; index < 1000; index++)
			{
				printf("%d ", tmp[index]);

				if((index+1)%100 == 0)
				{
					printf("\n");
				}
			}
		}
		
		zbaselib_sleep_msec(2);
	}
}

zbaselib_thread_result ZBASELIB_THREAD_PROCSPEC thread_write(zbaselib_thread_param arg)
{
	//zbuffer *buf = (zbuffer *)arg;
	
	int tmp[1000] = {0};
	int index = 0;
	while(1)
	{
		for(index = 0; index< 1000; index++)
		{
			tmp[index] = index;
		}
		
		if(zbaselib_buffer_write(buf, (void *)tmp,sizeof(tmp)) == 0)
		{
			//++tmp;
		}
		zbaselib_sleep_msec(1);
	}
}

#if 0
int main(int argc, char *argv[])
{
	zbuffer *buf = zbuffer_create(32, 0);
	assert(zbuffer_isempty(buf));
	assert(!zbuffer_isfull(buf));
	assert(zbuffer_busynum(buf) == 0);

	char tmp[10];
	memset(tmp, 1, sizeof(tmp));
	assert(zbuffer_read(buf,tmp,sizeof(tmp)) < 0);
	assert(zbuffer_write(buf, tmp,sizeof(tmp)) == 0);
	
	zbuffer_destory(buf);
	
	return 0;
}
#endif

#if 1
int main(int argc, char *argv[])
{
	zbaselib_thread_t rd_t=0, wt_t=0;	
	buf = zbaselib_buffer_create(1024*64, 0);

	rd_t = zbaselib_thread_create(thread_read, NULL);
	wt_t = zbaselib_thread_create(thread_write, NULL);

	while(1)
	{
		zbaselib_sleep_msec(10000);
	}

	
	zbaselib_buffer_destory(buf);
	
	return 0;
}
#endif

