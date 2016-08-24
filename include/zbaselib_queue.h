#ifndef _ZBASELIB_QUEUE_H_
#define _ZBASELIB_QUEUE_H_

#include "zbaselib_typedef.h"
#include "zbaselib_list.h"

ZBASELIB_BEGIN_DECL

typedef struct _zbaselib_queue zbaselib_queue;

// ddfunc=NULLʱ��list ɾ���ڵ�ʱ����free(data);
// ����zbaselib_queue_pop1�᷵��ͷ���ݣ�
// ����queueһ��Ҫ����zbaselib_queue_destroy1
ZLIB_API zbaselib_queue* zbaselib_queue_create(destroy_data_func ddfunc);

ZLIB_API void zbaselib_queue_destroy(zbaselib_queue* thiz);

// ���zbaselib_queue_createʱddfunc=NULL�����ô˺�������queue����������ڴ�й¶
ZLIB_API void zbaselib_queue_destroy1(zbaselib_queue* thiz, destroy_data_func ddfunc);


ZLIB_API int zbaselib_queue_push(zbaselib_queue* thiz, void* data);

ZLIB_API int zbaselib_queue_pop(zbaselib_queue* thiz);

// ��queue��ɾ��ͷ�ڵ㣬�������ݣ�ע���ڴ���queueʱҪ��ddfunc=NULL�����ǵ�
// �Ƿ�dataָ�룬�����ڴ�й¶
ZLIB_API void* zbaselib_queue_pop1(zbaselib_queue* thiz);

ZLIB_API void* zbaselib_queue_peek(zbaselib_queue* thiz);

ZLIB_API int zbaselib_queue_size(zbaselib_queue* thiz);

ZLIB_API int zbaselib_queue_empty(zbaselib_queue* thiz);

ZLIB_API zbaselib_list_iterater* zbaselib_queue_iterater_create(zbaselib_queue* thiz);

ZBASELIB_END_DECL

#endif

