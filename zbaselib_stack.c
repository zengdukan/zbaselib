#include "zbaselib_stack.h"

#define STACK_RETURN_VAL_IF_NULL(stack, val)	\
	if(stack == NULL || stack->list == NULL)	\
	{	\
		return val;	\
	}

struct _zbaselib_stack
{
	zbaselib_list* list;
};

zbaselib_stack* zbaselib_stack_create(destroy_data_func ddfunc)
{
	zbaselib_stack* thiz = (zbaselib_stack*)malloc(sizeof(zbaselib_stack));
	if(thiz != NULL)
	{
		thiz->list = zbaselib_list_create(ddfunc);
		if(thiz->list == NULL)
		{
			free(thiz);
			thiz = NULL;
		}
	}

	return thiz;
}

void zbaselib_stack_destroy(zbaselib_stack* thiz)
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

static void zbaselib_stack_ddfunc(void* data, void* ctx)
{
	destroy_data_func ddfunc = (destroy_data_func)ctx;
	ddfunc(data);
}


void zbaselib_stack_destroy1(zbaselib_stack* thiz, destroy_data_func ddfunc)
{
	if(thiz != NULL)
	{
		if(thiz->list != NULL)
		{
			zbaselib_list_foreach(thiz->list, zbaselib_stack_ddfunc, ddfunc);
			zbaselib_list_destroy(&thiz->list);
		}

		free(thiz);
	}
}


int zbaselib_stack_push(zbaselib_stack* thiz, void* data)
{
	STACK_RETURN_VAL_IF_NULL(thiz, -1);

	zbaselib_list_addhead(thiz->list, data);
	return 0;
}

int zbaselib_stack_pop(zbaselib_stack* thiz)
{
	STACK_RETURN_VAL_IF_NULL(thiz, -1);

	zbaselib_list_delhead(thiz->list);
	return 0;
}

// 注意释放返回的指针
void* zbaselib_stack_pop1(zbaselib_stack* thiz)
{
	void* data = NULL;
	STACK_RETURN_VAL_IF_NULL(thiz, NULL);

	data = zbaselib_list_gethead(thiz->list);
	zbaselib_list_delhead(thiz->list);

	return data;
}


void* zbaselib_stack_peek(zbaselib_stack* thiz)
{
	STACK_RETURN_VAL_IF_NULL(thiz, NULL);

	return zbaselib_list_gethead(thiz->list);
}

int zbaselib_stack_size(zbaselib_stack* thiz)
{
	STACK_RETURN_VAL_IF_NULL(thiz, 0);
	
	return zbaselib_list_size(thiz->list);
}

zbaselib_list_iterater* zbaselib_stack_iterater_create(zbaselib_stack* thiz)
{
	STACK_RETURN_VAL_IF_NULL(thiz, NULL);
	
	return zbaselib_list_iterater_create(thiz->list);
}
