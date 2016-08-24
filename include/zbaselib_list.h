#ifndef _ZLIST_H_
#define _ZLIST_H_

#include "zbaselib_typedef.h"

ZBASELIB_BEGIN_DECL

/**
 *author:zeng
 *date:2010-7-21
 *desc:˫������,��������ݱ��붯̬������ָ�롣��:
 *int *p = (int *)malloc(xxx);  zbaselib_list_addhead(p); //�Ե�
 *int p = 10; zbaselib_list_addhead(&p);		//���,��Ϊɾ���ڵ��ǻ�freeָ��
 *
 *ȱ��:��һ��ָ���Σ�ɾ��һ���ڵ㵼�������ڵ�
 *Ҳ���ͷ��ˣ���������Щ�ڵ��ǿ��ܳ���ʹ��ʱҪ
 *�����β���ͬһ��ָ��!!!!!!!!
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

// ����:-1:��������>=0:ɾ���ڵ����
ZLIB_API int zbaselib_list_del_use_cond(zbaselib_list *thiz, DataCmpFunc cmp, void *ctx);

ZLIB_API void *zbaselib_list_gethead(zbaselib_list *thiz);

ZLIB_API void *zbaselib_list_gettail(zbaselib_list *thiz);

ZLIB_API void *zbaselib_list_get(zbaselib_list *thiz, int index);

ZLIB_API void zbaselib_list_sethead(zbaselib_list *thiz, void *data);

ZLIB_API void zbaselib_list_settail(zbaselib_list *thiz, void *data);

ZLIB_API int zbaselib_list_set(zbaselib_list *thiz, void *data, int index);

ZLIB_API int zbaselib_list_foreach(zbaselib_list *thiz, DatatVisitFunc visit, void *ctx);

// ע�ⷵ��-1�������������û�ҵ���Ҳ�п�����listΪ�գ��������ʱҪע��
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
