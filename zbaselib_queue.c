#include "zbaselib_queue.h"

#define QUEUE_RETURN_VAL_IF_NULL(queue, val)	\
	if(queue == NULL || queue->list == NULL)	\
	{	\
		return val;	\
	}

struct _zbaselib_queue
{
	zbaselib_list* list;
};

zbaselib_queue* zbaselib_queue_create(destroy_data_func ddfunc)
{
	zbaselib_queue* thiz = (zbaselib_queue*)malloc(sizeof(zbaselib_queue));

	if(thiz != NULL)
	{
		if((thiz->list = zbaselib_list_create(ddfunc)) == NULL)
		{
			free(thiz);
			thiz = NULL;
		}
	}

	return thiz;
}

void zbaselib_queue_destroy(zbaselib_queue* thiz)
{
	if(thiz != NULL)
	{
		if(thiz->list != NULL)
		{
			zbaselib_list_destroy(&thiz->list);
		}

		free(thiz);
	}
}

static void zbaselib_queue_ddfunc(void* data, void* ctx)
{
	destroy_data_func ddfunc = (destroy_data_func)ctx;
	ddfunc(data);
}

// 如果zbaselib_queue_create时ddfunc=NULL，调用此函数销毁queue，否则造成内存泄露
void zbaselib_queue_destroy1(zbaselib_queue* thiz, destroy_data_func ddfunc)
{
	if(thiz != NULL)
	{
		if(thiz->list != NULL)
		{
			zbaselib_list_foreach(thiz->list, zbaselib_queue_ddfunc, ddfunc);
			zbaselib_list_destroy(&thiz->list);
		}

		free(thiz);
	}
}


int zbaselib_queue_push(zbaselib_queue* thiz, void* data)
{
	QUEUE_RETURN_VAL_IF_NULL(thiz, -1);
	
	zbaselib_list_addtail(thiz->list, data);
	return 0;
}

int zbaselib_queue_pop(zbaselib_queue* thiz)
{
	QUEUE_RETURN_VAL_IF_NULL(thiz, -1);

	zbaselib_list_delhead(thiz->list);
	return 0;
}

void* zbaselib_queue_pop1(zbaselib_queue* thiz)
{
	void* data = NULL;
	
	QUEUE_RETURN_VAL_IF_NULL(thiz, NULL);

	data = zbaselib_list_gethead(thiz->list);
	zbaselib_list_delhead(thiz->list);
	
	return data;
}

void* zbaselib_queue_peek(zbaselib_queue* thiz)
{
	QUEUE_RETURN_VAL_IF_NULL(thiz, NULL);

	return zbaselib_list_gethead(thiz->list);
}

int zbaselib_queue_size(zbaselib_queue* thiz)
{
	QUEUE_RETURN_VAL_IF_NULL(thiz, 0);
	
	return zbaselib_list_size(thiz->list);
}

int zbaselib_queue_empty(zbaselib_queue* thiz)
{
	return zbaselib_list_empty(thiz->list);
}

zbaselib_list_iterater* zbaselib_queue_iterater_create(zbaselib_queue* thiz)
{
	QUEUE_RETURN_VAL_IF_NULL(thiz, NULL);
	
	return zbaselib_list_iterater_create(thiz->list);
}
