#include "zbaselib_array.h"
#include <assert.h>

int int_cmp(const void* ctx, const void* data)
{
	return *(int*)data - *(int*)ctx;
}

void quick_sort_impl(int* array, int left, int right, DataCmpFunc cmp)
{
	int save_left  = left;
	int save_right = right;
	int x = array[left];

	while(left < right)
	{
		while(cmp((void*)&array[right], (void*)&x) >= 0 && left < right) right--;
		if(left != right)
		{
			array[left] = array[right];
			left++;
		}

		while(cmp((void*)&array[left], (void*)&x) <= 0 && left < right)	left++;
		if(left != right)
		{
			array[right] = array[left];
			right--;
		}
	}
	array[left] = x;

	if(save_left < left)
	{
		quick_sort_impl(array, save_left, left-1, cmp);
	}

	if(save_right > left)
	{
		quick_sort_impl(array, left+1, save_right, cmp);
	}

	return;
}

void quick_sort(void* array, int size, DataCmpFunc cmp)
{
	if(array == NULL || cmp == NULL)
	{
		return;
	}
		
	if(size > 1)
	{
		quick_sort_impl((int*)array, 0, size - 1, cmp);
	}
}

#if 0

static int data_cmp(const void* ctx, const void* data)
{
	if((int*)ctx == *(int**)data)
		return 0;
	else
		return 1;
}

int main(void)
{
	int i = 0;
	int n = 100;
	int *data = NULL;
	int* value = 0;
	zbaselib_array* array = zbaselib_array_create(sizeof(int*), 10);

	for(i = 0; i < n; i++)
	{
		data = (int*)malloc(sizeof(int));
		*data = i;
		zbaselib_array_append(array, (void*)&data);
	}

	value = *(int**)zbaselib_array_get(array, 50);
	printf("%d ", *value);
	printf("index = %d\n", zbaselib_array_find(array, data_cmp, value));

	for(i = 0; i < zbaselib_array_size(array); i++)
	{
		assert((value = *(int**)zbaselib_array_get(array, i)) != NULL);
		printf("%d ", *value);
		free(value);
	}
	printf("\n");



	zbaselib_array_destroy(array);

	getchar();
	return 0;
}
#else
int main(void)
{
	int i = 0;
	int n = 10000000;
	int *data = NULL;
	int value = 0;
	zbaselib_array* array = zbaselib_array_create(sizeof(int), 10);

	for(i = 0; i < n; i++)
	{
		assert(zbaselib_array_append(array, (void*)&i) == 0);
		assert(zbaselib_array_size(array) == (i + 1));
		assert((data  = (int*)zbaselib_array_get(array, i)) != NULL);
		assert(*data == i);
		value = 2 * i;
		assert(zbaselib_array_set(array, (void*)&value, i) == 0);
		assert((data  = (int*)zbaselib_array_get(array, i)) != NULL);
		assert(*data == value);
		assert(zbaselib_array_set(array, (void*)&i, i) == 0);
		assert(zbaselib_array_find(array, int_cmp, (void*)&i) == i);
	}
#if 0

	for(i = 0; i < n; i++)
	{
		assert((data = zbaselib_array_get(array, 0)) != NULL);
		assert(*data == i);
		assert(zbaselib_array_size(array) == (n - i));
		assert(zbaselib_array_delete(array, 0) == 0);
		assert(zbaselib_array_size(array) == (n - i - 1));
		if((i + 1) < n)
		{
			assert((data = zbaselib_array_get(array, 0)) != NULL);
			assert(*data == (i + 1));
		}
	}
	
	assert(zbaselib_array_size(array) == 0);

	for(i = 0; i < n; i++)
	{
		assert(zbaselib_array_prepend(array, (void*)&i) == 0);
		assert(zbaselib_array_size(array) == (i + 1));
		assert((data = zbaselib_array_get(array, 0)) != NULL);
		assert(*data == i);
		value = 2 * i;
		assert(zbaselib_array_set(array, (void*)&value, 0) == 0);
		assert((data = zbaselib_array_get(array, 0)) != NULL);
		assert(*data == value);
		assert(zbaselib_array_set(array, (void*)&i, 0) == 0);
	}
/*
	for(i = 0; i < zbaselib_array_size(array); i++)
	{
		assert((data = zbaselib_array_get(array, i)) != NULL);
		printf("%d ", *data);
	}
	printf("\n");
*/
#endif
	zbaselib_array_destroy(array);

#if 0
	// test sort
	n = 10;
	array = zbaselib_array_create(sizeof(int), 10);
	for(i = 0; i < n; i++)
	{
		value = rand()%100;
		zbaselib_array_append(array, (void*)&value);
	}

	for(i = 0; i < zbaselib_array_size(array); i++)
	{
		assert((data = zbaselib_array_get(array, i)) != NULL);
		printf("%d ", *data);
	}
	printf("\n");

	zbaselib_array_sort(array, quick_sort, int_cmp);

	for(i = 0; i < zbaselib_array_size(array); i++)
	{
		assert((data = zbaselib_array_get(array, i)) != NULL);
		printf("%d ", *data);
	}
	printf("\n");

	zbaselib_array_destroy(array);
#endif
	return 0;
}
#endif
