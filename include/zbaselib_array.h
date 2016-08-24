#ifndef _ZBASELIB_ARRAY_H_
#define _ZBASELIB_ARRAY_H_

#include "zbaselib_typedef.h"

ZBASELIB_BEGIN_DECL

typedef struct _zbasebib_array zbaselib_array;
typedef void (*DataSortFunc)(void* array, int size, DataCmpFunc cmp);

/*
*@elemSize:元素大小
*@elemNum:元素个数, 最小10个
*/
ZLIB_API zbaselib_array* zbaselib_array_create(int elemSize, int elemNum);

ZLIB_API void zbaselib_array_destroy(zbaselib_array* thiz);

ZLIB_API int zbaselib_array_insert(zbaselib_array* thiz, const void* elem, int index);

ZLIB_API int zbaselib_array_prepend(zbaselib_array* thiz, const void* elem);

ZLIB_API int zbaselib_array_append(zbaselib_array* thiz, const void* elem);

ZLIB_API int zbaselib_array_delete(zbaselib_array* thiz, int index);

ZLIB_API void* zbaselib_array_get(zbaselib_array* thiz, int index);

ZLIB_API int zbaselib_array_set(zbaselib_array* thiz, const void* elem, int index);

ZLIB_API int zbaselib_array_capacity(zbaselib_array* thiz);

ZLIB_API int zbaselib_array_size(zbaselib_array* thiz);

ZLIB_API int zbaselib_array_find(zbaselib_array* thiz, DataCmpFunc cmp, const void* ctx);

ZLIB_API void zbaselib_array_sort(zbaselib_array* thiz, DataSortFunc sort, DataCmpFunc cmp);

ZBASELIB_END_DECL

#endif	// _ZBASELIB_ARRAY_H_

