#include "zbaselib_list.h"

typedef struct _zbaselib_list_node zbaselib_list_node;
struct _zbaselib_list_node
{
   zbaselib_list_node *prev;
   zbaselib_list_node *next;

   void *data;
};

struct _zbaselib_list
{
	zbaselib_list_node *head;
	destroy_data_func ddfunc;		// data销毁函数-free/delete
};

static zbaselib_list_node *zlist_create_node(void *data)
{
	zbaselib_list_node *node = (zbaselib_list_node *)calloc(sizeof(zbaselib_list_node), 1);
	node->data = data;

	return node;
}

static void zlist_destroy_node(zbaselib_list_node **node, destroy_data_func ddfunc)
{
	if(node == NULL || (*node) == NULL)
	{
		return;
	}
	
	(*node)->prev = NULL;
	(*node)->next = NULL;
	if((*node)->data != NULL)
	{
		if(ddfunc != NULL)
			ddfunc((*node)->data);

		(*node)->data = NULL;
	}

	free(*node);
	*node = NULL;
}

static void zlist_add_node(zbaselib_list_node *new_node, zbaselib_list_node *prev, zbaselib_list_node *next)
{
	if(new_node == NULL || prev == NULL 
		|| next == NULL)
	{
		return;
	}
	
	prev->next = new_node;
	new_node->prev = prev;
	new_node->next = next;
	next->prev = new_node;
}

static void zlist_del_node(zbaselib_list_node *prev, zbaselib_list_node *next)
{
	if(prev == NULL || next == NULL)
	{
		return;
	}

	prev->next = next;
	next->prev = prev;
}

static zbaselib_list_node *zlist_get_node(zbaselib_list *thiz, int index)
{
	zbaselib_list_node* cursor = NULL;
	
	if(thiz == NULL || index < 0)
	{
		return NULL;
	}

	cursor = thiz->head;
	//空链表直接返回null
	if(cursor->next == thiz->head)
	{
		return NULL;
	}
	
	while((cursor->next != thiz->head) && (index >= 0))
	{
		--index;
		cursor = cursor->next;
	}

	return index < 0 ? cursor : NULL;
}

zbaselib_list *zbaselib_list_create(destroy_data_func ddfunc)
{
	zbaselib_list *thiz = (zbaselib_list *)calloc(sizeof(zbaselib_list), 1);
	thiz->head = zlist_create_node(NULL);
	thiz->head->next = thiz->head;
	thiz->head->prev = thiz->head;
	thiz->ddfunc = ddfunc;

	return thiz;
}

void zbaselib_list_addhead(zbaselib_list * thiz,void * data)
{
	zbaselib_list_node* node = NULL;
	if(thiz == NULL)
	{
		return;
	}
	
	node = zlist_create_node(data);
	zlist_add_node(node, thiz->head, thiz->head->next);
}

void zbaselib_list_addtail(zbaselib_list * thiz,void * data)
{
	zbaselib_list_node* node = NULL;
	if(thiz == NULL)
	{
		return;
	}
	
	node = zlist_create_node(data);
	zlist_add_node(node, thiz->head->prev, thiz->head);
}

int zbaselib_list_add(zbaselib_list * thiz,void * data,int index)
{
	zbaselib_list_node* node = NULL;
	zbaselib_list_node* new_node = NULL;

	if(thiz == NULL || index < 0)
	{
		return -1;
	}
	
	//空链表&&index==0直接插入头
	if(zbaselib_list_empty(thiz) && index == 0)
	{
		zbaselib_list_addhead(thiz, data);
		return 0;
	}

	//插入末尾
	if(index == zbaselib_list_size(thiz))
	{
		zbaselib_list_addtail(thiz, data);
		return 0;
	}

	//获取插入位置的节点
	node = zlist_get_node(thiz, index);
	if(node == NULL)
	{
		return -1;
	}

	new_node = zlist_create_node(data);
	zlist_add_node(new_node, node->prev, node);

	return 0;
}

void zbaselib_list_delhead(zbaselib_list * thiz)
{
	zbaselib_list_node* node = NULL;

	if(thiz == NULL || zbaselib_list_empty(thiz))
	{
		return;
	}

	node = thiz->head->next;
	zlist_del_node(node->prev, node->next);
	zlist_destroy_node(&node, thiz->ddfunc);
}

void zbaselib_list_deltail(zbaselib_list * thiz)
{
	zbaselib_list_node* node = NULL;

	if(thiz == NULL || zbaselib_list_empty(thiz))
	{
		return;
	}

	node = thiz->head->prev;
	zlist_del_node(node->prev, node->next);
	zlist_destroy_node(&node, thiz->ddfunc);
}

int zbaselib_list_del(zbaselib_list * thiz,int index)
{
	zbaselib_list_node* node = NULL;
	if(thiz == NULL || zbaselib_list_empty(thiz)
		|| index < 0)
	{
		return -1;
	}

	node = zlist_get_node(thiz, index);
	if(node == NULL)
	{
		return -1;
	}
	
	zlist_del_node(node->prev, node->next);
	zlist_destroy_node(&node, thiz->ddfunc);

	return 0;
}

int zbaselib_list_del_use_cond(zbaselib_list *thiz, DataCmpFunc cmp, void *ctx)
{
	zbaselib_list_node* cursor = NULL;
	int del_num = 0;	// 是否删除节点
	
    if(thiz == NULL || zbaselib_list_empty(thiz))
    {
            return -1;
    }

	cursor = thiz->head;
    while(cursor->next != thiz->head)
    {
		cursor = cursor->next;

        if(cmp(cursor->data, ctx))
		{
			zbaselib_list_node* del_node = cursor;
			cursor = del_node->prev;
			zlist_del_node(del_node->prev, del_node->next);
			zlist_destroy_node(&del_node, thiz->ddfunc);

			++del_num;
		}
    }

    return del_num;
}

void *zbaselib_list_gethead(zbaselib_list * thiz)
{
    if(thiz == NULL || zbaselib_list_empty(thiz))
    {
            return NULL;
    }

    return thiz->head->next->data;
}

static zbaselib_list_node* zlist_get_headnode(zbaselib_list* thiz)
{
    if(thiz == NULL || zbaselib_list_empty(thiz))
    {
            return NULL;
    }

    return thiz->head->next;
}

void *zbaselib_list_gettail(zbaselib_list * thiz)
{
    if(thiz == NULL || zbaselib_list_empty(thiz))
    {
        return NULL;
    }

    return thiz->head->prev->data;
}

static zbaselib_list_node* zlist_get_tailnode(zbaselib_list* thiz)
{
    if(thiz == NULL || zbaselib_list_empty(thiz))
    {
        return NULL;
    }

    return thiz->head->prev;
}

void *zbaselib_list_get(zbaselib_list * thiz,int index)
{
	 zbaselib_list_node* node = NULL;
    if(thiz == NULL || zbaselib_list_empty(thiz))
    {
            return NULL;
    }

    node = zlist_get_node(thiz, index);
    if(node == NULL)
    {
            return NULL;
    }

    return node->data;
}

void zbaselib_list_sethead(zbaselib_list * thiz,void * data)
{
    if(thiz == NULL || zbaselib_list_empty(thiz))
    {
		return;
    }

    if(thiz->head->next->data != NULL)
    {
//            free(thiz->head->next->data);
		thiz->ddfunc(thiz->head->next->data);
        thiz->head->next->data = NULL;
    }
    thiz->head->next->data = data;
}

void zbaselib_list_settail(zbaselib_list * thiz,void * data)
{
    if(thiz == NULL || zbaselib_list_empty(thiz))
    {
            return;
    }

    if(thiz->head->prev->data != NULL)
    {
        //free(thiz->head->prev->data);
		thiz->ddfunc(thiz->head->prev->data);
        thiz->head->prev->data = NULL;
    }
    thiz->head->prev->data = data;
}

int zbaselib_list_set(zbaselib_list * thiz,void * data,int index)
{
	 zbaselib_list_node* node = NULL;
    if(thiz == NULL || index < 0)
    {
            return -1;
    }

    node = zlist_get_node(thiz, index);
    if(NULL == node)
    {
        return -1;
    }

    if(node->data != NULL)
    {
        //free(node->data);
		thiz->ddfunc(node->data);
        node->data = NULL;
    }
    node->data = data;

    return 0;
}

int zbaselib_list_size(zbaselib_list * thiz)
{
	zbaselib_list_node* cursor = NULL;
	int size = 0;
    
	if(thiz == NULL)
    {
		return -1;
    }

    cursor = thiz->head;
    
    while(cursor->next != thiz->head)
    {
        ++size;
        cursor = cursor->next;
    }

    return size;
}

int zbaselib_list_empty(zbaselib_list * thiz)
{
    if(thiz == NULL)
    {	
            return 1;
    }

    return (thiz->head->next == thiz->head);
}

void zbaselib_list_clear(zbaselib_list * thiz)
{
	zbaselib_list_node* cursor = NULL;
   
	if(thiz == NULL || zbaselib_list_empty(thiz))
    {
            return;
    }

    cursor = thiz->head;
    while(cursor->next != thiz->head)
    {
            zbaselib_list_deltail(thiz);
    }	
}

void zbaselib_list_destroy(zbaselib_list **thiz)
{
    if(thiz == NULL || (*thiz) == NULL)
    {
            return;
    }

    zbaselib_list_clear(*thiz);

    zlist_destroy_node(&((*thiz)->head), (*thiz)->ddfunc);

    free(*thiz);
    *thiz = NULL;
}

int zbaselib_list_foreach(zbaselib_list *thiz, DatatVisitFunc visit, void *ctx)
{
	zbaselib_list_node* cursor = NULL;
    if(thiz == NULL || zbaselib_list_empty(thiz))
    {
            return -1;
    }

	cursor = thiz->head;
    while(cursor->next != thiz->head)
    {
            cursor = cursor->next;
            visit(cursor->data, ctx);
    }

    return 0;
}

static int zlist_isend(zbaselib_list_node* node, zbaselib_list* thiz)
{
    if(node == NULL || thiz == NULL)
    {
        return 1;
    }
    
    return (node == thiz->head);
}

int zbaselib_list_find(zbaselib_list *thiz, DataCmpFunc cmp, void *ctx)
{
	int index = 0;
	zbaselib_list_node* cursor = NULL;
	char flag = 0;

    if(thiz == NULL || zbaselib_list_empty(thiz))
    {
            return -1;
    }

    
    cursor = thiz->head;
    while(cursor->next != thiz->head)
    {
            cursor = cursor->next;
            if(cmp(cursor->data, ctx))
            {
                    flag = 1;
                    break;
            }

            ++index;
    }

    return flag == 1 ? index : -1;
}

struct _zbaselib_list_iterater
{
    zbaselib_list* list;
    zbaselib_list_node* node;
};

zbaselib_list_iterater* zbaselib_list_iterater_create(zbaselib_list* list)
{
    zbaselib_list_iterater* iterater = NULL;
	
	if(list == NULL)
    {
        return NULL;
    }
    
    iterater = (zbaselib_list_iterater*)calloc(sizeof(zbaselib_list_iterater), 1);
    iterater->list = list;
    
    return iterater;
}

void* zbaselib_list_iterater_first(zbaselib_list_iterater* thiz)
{
	zbaselib_list_node* first = NULL;

    if(thiz == NULL || thiz->list == NULL)
    {
        return NULL;
    }
    
    first = zlist_get_headnode(thiz->list);
    if(first != NULL)
    {
        thiz->node = first;
        return thiz->node->data;
    }
    else
    {
        return NULL;
    }
}

void* zbaselib_list_iterater_next(zbaselib_list_iterater* thiz)
{
    if(thiz == NULL || thiz->list == NULL)
    {
        return NULL;
    }
    
    if(!zlist_isend(thiz->node, thiz->list))
    {
        thiz->node = thiz->node->next;
        return thiz->node->data;
    }
    else
    {
        return NULL;
    }
}

void* zbaselib_list_iterater_last(zbaselib_list_iterater* thiz)
{
	zbaselib_list_node* node = NULL;

    if(thiz == NULL || thiz->list == NULL)
    {
        return NULL;
    }
    
    node = zlist_get_tailnode(thiz->list);
    if(node != NULL)
    {
        thiz->node = node;
        return node->data;
    }
    else
    {
        return NULL;
    }
}

int zbaselib_list_iterater_isend(zbaselib_list_iterater* thiz)
{
    return zlist_isend(thiz->node, thiz->list);
}

void zbaselib_list_iterater_destroy(zbaselib_list_iterater** thiz)
{
    if(thiz == NULL || (*thiz) == NULL)
    {
            return;
    }
    
    free(*thiz);
    *thiz = NULL;
}
