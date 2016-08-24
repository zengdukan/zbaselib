#ifndef _ZBASELIB_QUEUE_H_
#define _ZBASELIB_QUEUE_H_

#include "zbaselib_typedef.h"
#include "zbaselib_list.h"

ZBASELIB_BEGIN_DECL

typedef struct _zbaselib_queue zbaselib_queue;

// ddfunc=NULL时，list 删除节点时不会free(data);
// 调用zbaselib_queue_pop1会返回头数据，
// 销毁queue一定要调用zbaselib_queue_destroy1
ZLIB_API zbaselib_queue* zbaselib_queue_create(destroy_data_func ddfunc);

ZLIB_API void zbaselib_queue_destroy(zbaselib_queue* thiz);

// 如果zbaselib_queue_create时ddfunc=NULL，调用此函数销毁queue，否则造成内存泄露
ZLIB_API void zbaselib_queue_destroy1(zbaselib_queue* thiz, destroy_data_func ddfunc);


ZLIB_API int zbaselib_queue_push(zbaselib_queue* thiz, void* data);

ZLIB_API int zbaselib_queue_pop(zbaselib_queue* thiz);

// 从queue中删除头节点，返回数据，注意在创建queue时要将ddfunc=NULL，并记得
// 是否data指针，避免内存泄露
ZLIB_API void* zbaselib_queue_pop1(zbaselib_queue* thiz);

ZLIB_API void* zbaselib_queue_peek(zbaselib_queue* thiz);

ZLIB_API int zbaselib_queue_size(zbaselib_queue* thiz);

ZLIB_API int zbaselib_queue_empty(zbaselib_queue* thiz);

ZLIB_API zbaselib_list_iterater* zbaselib_queue_iterater_create(zbaselib_queue* thiz);

ZBASELIB_END_DECL

#endif

