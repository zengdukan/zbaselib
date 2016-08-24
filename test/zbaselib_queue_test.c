#include "zbaselib_queue.h"
#include <assert.h>

void queue_print(zbaselib_queue* queue)
{
	int* pi = 0;
	
	zbaselib_list_iterater* iter = zbaselib_queue_iterater_create(queue);
	for(pi = zbaselib_list_iterater_first(iter); !zbaselib_list_iterater_isend(iter); pi = zbaselib_list_iterater_next(iter))
	{
	    printf("%d ", *pi);
	}

	printf("\n");

	zbaselib_list_iterater_destroy(&iter);
}

void destroy_int(void* data)
{
	free(data);
}

#if 1	// zbaselib_queue_create ddfunc!=null，自动释放版本
int main(void)
{
	int i = 0;
	int n = 1000;
	int* ret_data = 0;
	int* data = 0;
	char* str = NULL;
	char txt[128];
	zbaselib_queue* strqueue = NULL;
	zbaselib_queue* queue = zbaselib_queue_create(destroy_int);
	
	for(i = 0; i < n; i++)
	{		
		data = (int*)malloc(sizeof(int));
		*data = i;
		assert(zbaselib_queue_push(queue, (void*)data) == 0);
		assert(zbaselib_queue_peek(queue) != NULL);
		assert(zbaselib_queue_size(queue) == (i + 1));
	}

	queue_print(queue);

	for(i = 0; i < n; i++)
	{ 
		assert((ret_data = (int*)zbaselib_queue_peek(queue)) != NULL);
		assert(*ret_data == i);
		printf("%d ", *ret_data);
		assert(zbaselib_queue_size(queue) == (n - i));
		assert(zbaselib_queue_pop(queue) == 0);
		
	}
	printf("\n");
	zbaselib_queue_destroy(queue);

	strqueue = zbaselib_queue_create(destroy_int);
	n = 20;
	for(i = 0; i < n; i++)
	{
		memset(txt, 0, sizeof(txt));
		sprintf(txt, "good boy, %d", i);
		str = strdup(txt);
		zbaselib_queue_push(strqueue, (void *)str);
	}

	for(i = 0; i < n; i++)
	{ 
		assert((str = (char*)zbaselib_queue_peek(strqueue)) != NULL);
		printf("%s\n", str);
		assert(zbaselib_queue_size(strqueue) == (n - i));
		assert(zbaselib_queue_pop(strqueue) == 0);
		
	}
	printf("\n");
	
	zbaselib_queue_destroy(strqueue);
	
	return 0;
}
#else
// zbaselib_queue_create ddfunc==null，手动释放版本
int main(void)
{
	int i = 0;
	int n = 1000;
	int* ret_data = 0;
	int* data = 0;
	zbaselib_queue* queue = zbaselib_queue_create(NULL);
		
	for(i = 0; i < n; i++)
	{		
		data = (int*)malloc(sizeof(int));
		*data = i;
		assert(zbaselib_queue_push(queue, (void*)data) == 0);
		assert(zbaselib_queue_peek(queue) != NULL);
		assert(zbaselib_queue_size(queue) == (i + 1));
	}

	//queue_print(queue);

	for(i = 0; i < n; i++)
	{ 
		assert((ret_data = (int*)zbaselib_queue_peek(queue)) != NULL);
		assert(*ret_data == i);
		printf("%d ", *ret_data);
		assert(zbaselib_queue_size(queue) == (n - i));
		assert((ret_data = (int*)zbaselib_queue_pop1(queue)) != NULL);
		assert(*ret_data == i);
		free(ret_data);
		
	}
	printf("\n");
	assert(zbaselib_queue_empty(queue));

	
	zbaselib_queue_destroy1(queue, destroy_int);
	
	return 0;
}

#endif


