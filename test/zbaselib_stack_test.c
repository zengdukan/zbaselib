#include "zbaselib_stack.h"
#include <assert.h>

void stack_print(zbaselib_stack* stack)
{
	int* pi = 0;
	
	zbaselib_list_iterater* iter = zbaselib_stack_iterater_create(stack);
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

#if 0
// 自动是否内存版
int main(void)
{
	int i = 0;
	int n = 1000;
	int* ret_data = 0;
	int* data = 0;
	zbaselib_stack* stack = zbaselib_stack_create(destroy_int);
		
	for(i = 0; i < n; i++)
	{		
		data = (int*)malloc(sizeof(int));
		*data = i;
		assert(zbaselib_stack_push(stack, (void*)data) == 0);
		assert(zbaselib_stack_peek(stack) != NULL);
		assert(zbaselib_stack_size(stack) == (i + 1));
	}

	//stack_print(stack);

	for(i = n; i > 0; i--)
	{ 
		assert((ret_data = (int*)zbaselib_stack_peek(stack)) != NULL);
		assert(*ret_data == (i - 1));
		printf("%d ", *ret_data);
		assert(zbaselib_stack_size(stack) == i);
		assert(zbaselib_stack_pop(stack) == 0);
		
	}
	printf("\n");
	
	zbaselib_stack_destroy(stack);
	
	return 0;
}
#else
// 手动释放内存版
int main(void)
{
	int i = 0;
	int n = 1000;
	int* ret_data = 0;
	int* data = 0;
	zbaselib_stack* stack = zbaselib_stack_create(NULL);
		
	for(i = 0; i < n; i++)
	{		
		data = (int*)malloc(sizeof(int));
		*data = i;
		assert(zbaselib_stack_push(stack, (void*)data) == 0);
		assert(zbaselib_stack_peek(stack) != NULL);
		assert(zbaselib_stack_size(stack) == (i + 1));
	}

	//stack_print(stack);

	for(i = n; i > 0; i--)
	{ 
		assert((ret_data = (int*)zbaselib_stack_peek(stack)) != NULL);
		assert(*ret_data == (i - 1));
		printf("%d ", *ret_data);
		assert(zbaselib_stack_size(stack) == i);
		assert((ret_data = (int*)zbaselib_stack_pop1(stack)) != NULL);
		assert(*ret_data == (i - 1));
		free(ret_data);
		
	}
	printf("\n");
	assert(zbaselib_stack_size(stack) == 0);
	zbaselib_stack_destroy(stack);
	
	return 0;
}

#endif

