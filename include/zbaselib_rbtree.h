// 提取自nginx, 不能存NULL值

#ifndef _ZBASELIB_TBTREE_H_
#define _ZBASELIB_TBTREE_H_

#ifdef WIN32
#ifdef DLL_EXPORT
#define ZLIB_API __declspec(dllexport)
#else
#define ZLIB_API 
#endif
#else
#define ZLIB_API
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/*typedef int  zbaselib_rbtree_key_t;
typedef int   zbaselib_int_t;
typedef unsigned char u_char;
typedef unsigned int uint_t;
*/
typedef struct _zbaselib_rbtree_node  zbaselib_rbtree_node;
/*struct _zbaselib_rbtree_node {
	zbaselib_rbtree_key_t       key;
	zbaselib_rbtree_node     *left;
	zbaselib_rbtree_node     *right;
	zbaselib_rbtree_node     *parent;
	u_char                 color;
	u_char                 data;
};

struct _zbaselib_rbtree {
	zbaselib_rbtree_node     *root;
	zbaselib_rbtree_node     *sentinel;
	int size;	// 节点数量
};*/

typedef struct _zbaselib_rbtree  zbaselib_rbtree;

typedef int(*compareFunc)(const void *, const void *);

ZLIB_API zbaselib_rbtree* zbaselib_rbtree_init(compareFunc cmp);

ZLIB_API void zbaselib_rbtree_deinit(zbaselib_rbtree* tree);

ZLIB_API int zbaselib_rbtree_empty(zbaselib_rbtree* tree);

ZLIB_API int zbaselib_rbtree_size(zbaselib_rbtree* tree);

ZLIB_API void zbaselib_rbtree_push(zbaselib_rbtree* tree, void* key);

ZLIB_API void* zbaselib_rbtree_popmin(zbaselib_rbtree* tree);

ZLIB_API void* zbaselib_rbtree_getmin(zbaselib_rbtree* tree);

ZLIB_API void* zbaselib_rbtree_getmax(zbaselib_rbtree* tree);



//查找节点
ZLIB_API zbaselib_rbtree_node* zbaselib_rbtree_find(zbaselib_rbtree* tree, void* key);


// 删除节点
ZLIB_API int zbaselib_rbtree_delnode(zbaselib_rbtree* tree, void* key);

//ZLIB_API int zbaselib_rbtree_foreach(zbaselib_rbtree* tree, 

#ifdef __cplusplus
}
#endif

#endif /* _ZBASELIB_TBTREE_H_ */
