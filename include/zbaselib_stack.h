#ifndef _ZBASELIB_STACK_H_
#define _ZBASELIB_STACK_H_

#include "zbaselib_typedef.h"
#include "zbaselib_list.h"

ZBASELIB_BEGIN_DECL

typedef struct _zbaselib_stack zbaselib_stack;

// 手动释放内存版:当ddfunc==NULL时，对应函数zbaselib_stack_destroy1，zbaselib_stack_pop1,
// 自动释放内存版:当ddfunc!=NULL时，对应函数zbaselib_stack_destroy，zbaselib_stack_pop
// 否则出错
ZLIB_API zbaselib_stack* zbaselib_stack_create(destroy_data_func ddfunc);

ZLIB_API void zbaselib_stack_destroy(zbaselib_stack* thiz);

ZLIB_API void zbaselib_stack_destroy1(zbaselib_stack* thiz, destroy_data_func ddfunc);


ZLIB_API int zbaselib_stack_push(zbaselib_stack* thiz, void* data);

ZLIB_API int zbaselib_stack_pop(zbaselib_stack* thiz);

// 注意释放返回的指针
ZLIB_API void* zbaselib_stack_pop1(zbaselib_stack* thiz);

ZLIB_API void* zbaselib_stack_peek(zbaselib_stack* thiz);

ZLIB_API int zbaselib_stack_size(zbaselib_stack* thiz);

ZLIB_API zbaselib_list_iterater* zbaselib_stack_iterater_create(zbaselib_stack* thiz);


ZBASELIB_END_DECL

#endif

