#ifndef _ZLIST_H_
#define _ZLIST_H_

#include "zbaselib_typedef.h"

ZBASELIB_BEGIN_DECL

/**
 *author:zeng
 *date:2010-7-21
 *desc:双向链表,保存的数据必须动态分配是指针。如:
 *int *p = (int *)malloc(xxx);  zbaselib_list_addhead(p); //对的
 *int p = 10; zbaselib_list_addhead(&p);		//错的,因为删除节点是会free指针
 *
 *缺点:当一个指针多次，删除一个节点导致其他节点
 *也被释放了，当访问这些节点是可能出错。使用时要
 *避免多次插入同一个指针!!!!!!!!
 */

typedef struct _zbaselib_list zbaselib_list;

typedef void (*destroy_data_func)(void* data);

ZLIB_API zbaselib_list *zbaselib_list_create(destroy_data_func ddfunc);

ZLIB_API void zbaselib_list_addhead(zbaselib_list *thiz, void *data);

ZLIB_API void zbaselib_list_addtail(zbaselib_list *thiz, void *data);

ZLIB_API int zbaselib_list_add(zbaselib_list *thiz, void *data, int index);

ZLIB_API void zbaselib_list_delhead(zbaselib_list *thiz);

ZLIB_API void zbaselib_list_deltail(zbaselib_list *thiz);

ZLIB_API int zbaselib_list_del(zbaselib_list *thiz, int index);

// 返回:-1:参数错误，>=0:删除节点个数
ZLIB_API int zbaselib_list_del_use_cond(zbaselib_list *thiz, DataCmpFunc cmp, void *ctx);

ZLIB_API void *zbaselib_list_gethead(zbaselib_list *thiz);

ZLIB_API void *zbaselib_list_gettail(zbaselib_list *thiz);

ZLIB_API void *zbaselib_list_get(zbaselib_list *thiz, int index);

ZLIB_API void zbaselib_list_sethead(zbaselib_list *thiz, void *data);

ZLIB_API void zbaselib_list_settail(zbaselib_list *thiz, void *data);

ZLIB_API int zbaselib_list_set(zbaselib_list *thiz, void *data, int index);

ZLIB_API int zbaselib_list_foreach(zbaselib_list *thiz, DatatVisitFunc visit, void *ctx);

// 注意返回-1的情况，可能是没找到，也有可能是list为空，插入操作时要注意
ZLIB_API int zbaselib_list_find(zbaselib_list *thiz, DataCmpFunc cmp, void *ctx);

ZLIB_API int zbaselib_list_size(zbaselib_list *thiz);

ZLIB_API int zbaselib_list_empty(zbaselib_list *thiz);

ZLIB_API void zbaselib_list_clear(zbaselib_list *thiz);

ZLIB_API void zbaselib_list_destroy(zbaselib_list **thiz);

/*----------------------------------------------------------------------------*/
typedef struct _zbaselib_list_iterater zbaselib_list_iterater;

ZLIB_API zbaselib_list_iterater* zbaselib_list_iterater_create(zbaselib_list* list);

ZLIB_API void* zbaselib_list_iterater_first(zbaselib_list_iterater* thiz);

ZLIB_API void* zbaselib_list_iterater_next(zbaselib_list_iterater* thiz);

ZLIB_API void* zbaselib_list_iterater_last(zbaselib_list_iterater* thiz);

ZLIB_API int zbaselib_list_iterater_isend(zbaselib_list_iterater* thiz);

ZLIB_API void zbaselib_list_iterater_destroy(zbaselib_list_iterater** thiz);

ZBASELIB_END_DECL

#endif
