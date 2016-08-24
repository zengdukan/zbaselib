#include "zbaselib_array.h"
#include <stdio.h>

// 最小个数
#define ZBASELIB_ARRAY_MIN_NUM	10

#define RETUAN_FAIL_IF_ERROR(thiz, value)	\
	if(thiz == NULL || thiz->data == NULL)	\
	{	\
		return value;	\
	}


struct _zbasebib_array
{
	int elemSize;	// 元素大小
	int size;		// 元素个数
	int capacity;	// 数组容量，可以容纳元素个数
	char* data;		// 内存地址
};

static int zbaselib_array_expand(zbaselib_array* thiz, int need)
{
	RETUAN_FAIL_IF_ERROR(thiz, -1);

	if((thiz->size + need) > thiz->capacity)
	{
		int capacity = thiz->capacity + (thiz->capacity>>1) + ZBASELIB_ARRAY_MIN_NUM;

		char* data = (char*)realloc(thiz->data, thiz->elemSize * capacity);
		if(data != NULL)
		{
			thiz->data = data;
			thiz->capacity = capacity;
		}

		MESSAGE("capacity = %d, size = %d\n", thiz->capacity, thiz->size);
	}

	return ((thiz->size + need) <= thiz->capacity);
}

static int zbaselib_array_reduce(zbaselib_array* thiz)
{
	RETUAN_FAIL_IF_ERROR(thiz, -1);

	if((thiz->size < (thiz->capacity >> 1)) && (thiz->capacity > ZBASELIB_ARRAY_MIN_NUM))
	{
		int capacity = thiz->size + (thiz->size >> 1);

		char* data = (char*)realloc(thiz->data, thiz->elemSize * capacity);
		if(data != NULL)
		{
			thiz->data = data;
			thiz->capacity = capacity;
		}

		MESSAGE("capacity = %d, size = %d\n", thiz->capacity, thiz->size);
	}

	return 0;
}

zbaselib_array* zbaselib_array_create(int elemSize, int elemNum)
{
	zbaselib_array* thiz = (zbaselib_array*) malloc(sizeof(zbaselib_array));
	if(thiz == NULL)
	{
		return NULL;
	}

	thiz->elemSize = elemSize;
	thiz->capacity = (elemNum < ZBASELIB_ARRAY_MIN_NUM ? ZBASELIB_ARRAY_MIN_NUM : elemNum);
	thiz->size = 0;
	thiz->data = (char*)malloc(thiz->elemSize * thiz->capacity);
	if(thiz->data == NULL)
	{
		free(thiz);
		thiz = NULL;
	}
	
	return thiz;
}

void zbaselib_array_destroy(zbaselib_array* thiz)
{
	if(thiz != NULL)
	{
		if(thiz->data != NULL)
		{
			free(thiz->data);
		}

		free(thiz);
	}
}

int zbaselib_array_insert(zbaselib_array* thiz, const void* elem, int index)
{
	RETUAN_FAIL_IF_ERROR(thiz, -1);

	if(index > thiz->size || index < 0 || elem == NULL)
	{
		return -1;
	}

	if(zbaselib_array_expand(thiz, 1))
	{
		int i = 0;
		for(i = thiz->size; i > index; i--)
		{
			memcpy(thiz->data + i * thiz->elemSize, thiz->data + (i-1) * thiz->elemSize, thiz->elemSize);
		}

		memcpy(thiz->data + index * thiz->elemSize, elem, thiz->elemSize);
		thiz->size++;
	}

	return 0;
}

int zbaselib_array_prepend(zbaselib_array* thiz, const void* elem)
{
	return zbaselib_array_insert(thiz, elem, 0);
}

int zbaselib_array_append(zbaselib_array* thiz, const void* elem)
{
	int end = zbaselib_array_size(thiz);
	return zbaselib_array_insert(thiz, elem, end);
}

int zbaselib_array_delete(zbaselib_array* thiz, int index)
{
	int i = 0;
	
	RETUAN_FAIL_IF_ERROR(thiz, -1);

	if(index >= thiz->size || index < 0)
	{
		return -1;
	}


	for(i = index; (i+1) < thiz->size; i++)
	{
		memcpy(thiz->data + i * thiz->elemSize, thiz->data + (i + 1) * thiz->elemSize, thiz->elemSize);
	}
	thiz->size--;

	zbaselib_array_reduce(thiz);

	return 0;
}

void* zbaselib_array_get(zbaselib_array* thiz, int index)
{
	RETUAN_FAIL_IF_ERROR(thiz, NULL);

	if(index >= thiz->size || index < 0)
	{
		return NULL;
	}

	return thiz->data + index * thiz->elemSize;
}

int zbaselib_array_set(zbaselib_array* thiz, const void* elem, int index)
{
	RETUAN_FAIL_IF_ERROR(thiz, -1);

	if(index >= thiz->size || index < 0)
	{
		return -1;
	}

	memcpy(thiz->data + index * thiz->elemSize, elem, thiz->elemSize);
	return 0;
}

int zbaselib_array_capacity(zbaselib_array* thiz)
{
	RETUAN_FAIL_IF_ERROR(thiz, -1);

	return thiz->capacity;
}

int zbaselib_array_size(zbaselib_array* thiz)
{
	RETUAN_FAIL_IF_ERROR(thiz, -1);

	return thiz->size;
}

int zbaselib_array_find(zbaselib_array* thiz, DataCmpFunc cmp, const void* ctx)
{
	int i = 0;

	RETUAN_FAIL_IF_ERROR(thiz, -1);

	for(i = 0; i < thiz->size; i++)
	{
		if(cmp(ctx, thiz->data + i * thiz->elemSize) == 0)
		{
			break;
		}
	}

	return (i == thiz->size ? -1 : i);
}

void zbaselib_array_sort(zbaselib_array* thiz, DataSortFunc sort, DataCmpFunc cmp)
{
	if(thiz == NULL || sort == NULL || cmp == NULL)
		return;

	sort(thiz->data, thiz->size, cmp);
}

