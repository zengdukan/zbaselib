
/*
 * Copyright (C) Igor Sysoev
 */

/*
 * The red-black tree code is based on the algorithm described in
 * the "Introduction to Algorithms" by Cormen, Leiserson and Rivest.
 */
#include "zbaselib_rbtree.h"
#include <stdio.h>
#include <stdlib.h>


typedef void*  zbaselib_rbtree_key_t;
typedef int   zbaselib_int_t;
typedef unsigned char u_char;
typedef unsigned int uint_t;

struct _zbaselib_rbtree_node {
	zbaselib_rbtree_key_t       key;
	zbaselib_rbtree_node     *left;
	zbaselib_rbtree_node     *right;
	zbaselib_rbtree_node     *parent;
	u_char                 color;
//	u_char                 data;
};

struct _zbaselib_rbtree {
	zbaselib_rbtree_node     *root;
	zbaselib_rbtree_node     *sentinel;
	int size;	// 节点数量
	compareFunc cmp;
};

#define zbaselib_rbt_red(node)               ((node)->color = 1)
#define zbaselib_rbt_black(node)             ((node)->color = 0)
#define zbaselib_rbt_is_red(node)            ((node)->color)
#define zbaselib_rbt_is_black(node)          (!zbaselib_rbt_is_red(node))
#define zbaselib_rbt_copy_color(n1, n2)      (n1->color = n2->color)


/* a sentinel must be black */

#define zbaselib_rbtree_sentinel_init(node)  zbaselib_rbt_black(node)


static void zbaselib_rbtree_delete(volatile zbaselib_rbtree *tree,
    zbaselib_rbtree_node *node);

static void zbaselib_rbtree_left_rotate(zbaselib_rbtree_node **root,
    zbaselib_rbtree_node *sentinel, zbaselib_rbtree_node *node);
static void zbaselib_rbtree_right_rotate(zbaselib_rbtree_node **root,
    zbaselib_rbtree_node *sentinel, zbaselib_rbtree_node *node);

static void zbaselib_rbtree_insert_value(zbaselib_rbtree* tree, zbaselib_rbtree_node *root, zbaselib_rbtree_node *node,
									zbaselib_rbtree_node *sentinel);

static zbaselib_rbtree_node *
zbaselib_rbtree_min_intenal(zbaselib_rbtree_node* node, zbaselib_rbtree_node* sentinel);


void
zbaselib_rbtree_insert_value(zbaselib_rbtree* tree, zbaselib_rbtree_node *temp, zbaselib_rbtree_node *node,
    zbaselib_rbtree_node *sentinel)
{
    zbaselib_rbtree_node  **p;

    for ( ;; ) {

        p = (tree->cmp(node->key, temp->key) < 0) ? &temp->left : &temp->right;
		
        if (*p == sentinel) {
            break;
        }

        temp = *p;
    }

    *p = node;
    node->parent = temp;
    node->left = sentinel;
    node->right = sentinel;
    zbaselib_rbt_red(node);
}

void
zbaselib_rbtree_delete(volatile zbaselib_rbtree *tree,
    zbaselib_rbtree_node *node)
{
    uint_t           red;
    zbaselib_rbtree_node  **root, *sentinel, *subst, *temp, *w;

    /* a binary tree delete */

    root = (zbaselib_rbtree_node **) &tree->root;
    sentinel = tree->sentinel;

    if (node->left == sentinel) {
        temp = node->right;
        subst = node;

    } else if (node->right == sentinel) {
        temp = node->left;
        subst = node;

    } else {
        subst = zbaselib_rbtree_min_intenal(node->right, sentinel);

        if (subst->left != sentinel) {
            temp = subst->left;
        } else {
            temp = subst->right;
        }
    }

    if (subst == *root) {
        *root = temp;
        zbaselib_rbt_black(temp);

        /* DEBUG stuff */
        node->left = NULL;
        node->right = NULL;
        node->parent = NULL;
        node->key = 0;

		--tree->size;
        return;
    }

    red = zbaselib_rbt_is_red(subst);

    if (subst == subst->parent->left) {
        subst->parent->left = temp;

    } else {
        subst->parent->right = temp;
    }

    if (subst == node) {

        temp->parent = subst->parent;

    } else {

        if (subst->parent == node) {
            temp->parent = subst;

        } else {
            temp->parent = subst->parent;
        }

        subst->left = node->left;
        subst->right = node->right;
        subst->parent = node->parent;
        zbaselib_rbt_copy_color(subst, node);

        if (node == *root) {
            *root = subst;

        } else {
            if (node == node->parent->left) {
                node->parent->left = subst;
            } else {
                node->parent->right = subst;
            }
        }

        if (subst->left != sentinel) {
            subst->left->parent = subst;
        }

        if (subst->right != sentinel) {
            subst->right->parent = subst;
        }
    }

    /* DEBUG stuff */
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->key = 0;

    if (red) {
		--tree->size;
        return;
    }

    /* a delete fixup */

    while (temp != *root && zbaselib_rbt_is_black(temp)) {

        if (temp == temp->parent->left) {
            w = temp->parent->right;

            if (zbaselib_rbt_is_red(w)) {
                zbaselib_rbt_black(w);
                zbaselib_rbt_red(temp->parent);
                zbaselib_rbtree_left_rotate(root, sentinel, temp->parent);
                w = temp->parent->right;
            }

            if (zbaselib_rbt_is_black(w->left) && zbaselib_rbt_is_black(w->right)) {
                zbaselib_rbt_red(w);
                temp = temp->parent;

            } else {
                if (zbaselib_rbt_is_black(w->right)) {
                    zbaselib_rbt_black(w->left);
                    zbaselib_rbt_red(w);
                    zbaselib_rbtree_right_rotate(root, sentinel, w);
                    w = temp->parent->right;
                }

                zbaselib_rbt_copy_color(w, temp->parent);
                zbaselib_rbt_black(temp->parent);
                zbaselib_rbt_black(w->right);
                zbaselib_rbtree_left_rotate(root, sentinel, temp->parent);
                temp = *root;
            }

        } else {
            w = temp->parent->left;

            if (zbaselib_rbt_is_red(w)) {
                zbaselib_rbt_black(w);
                zbaselib_rbt_red(temp->parent);
                zbaselib_rbtree_right_rotate(root, sentinel, temp->parent);
                w = temp->parent->left;
            }

            if (zbaselib_rbt_is_black(w->left) && zbaselib_rbt_is_black(w->right)) {
                zbaselib_rbt_red(w);
                temp = temp->parent;

            } else {
                if (zbaselib_rbt_is_black(w->left)) {
                    zbaselib_rbt_black(w->right);
                    zbaselib_rbt_red(w);
                    zbaselib_rbtree_left_rotate(root, sentinel, w);
                    w = temp->parent->left;
                }

                zbaselib_rbt_copy_color(w, temp->parent);
                zbaselib_rbt_black(temp->parent);
                zbaselib_rbt_black(w->left);
                zbaselib_rbtree_right_rotate(root, sentinel, temp->parent);
                temp = *root;
            }
        }
    }

    zbaselib_rbt_black(temp);
	--tree->size;
}


void
zbaselib_rbtree_left_rotate(zbaselib_rbtree_node **root, zbaselib_rbtree_node *sentinel,
    zbaselib_rbtree_node *node)
{
    zbaselib_rbtree_node  *temp;

    temp = node->right;
    node->right = temp->left;

    if (temp->left != sentinel) {
        temp->left->parent = node;
    }

    temp->parent = node->parent;

    if (node == *root) {
        *root = temp;

    } else if (node == node->parent->left) {
        node->parent->left = temp;

    } else {
        node->parent->right = temp;
    }

    temp->left = node;
    node->parent = temp;
}


void
zbaselib_rbtree_right_rotate(zbaselib_rbtree_node **root, zbaselib_rbtree_node *sentinel,
    zbaselib_rbtree_node *node)
{
    zbaselib_rbtree_node  *temp;

    temp = node->left;
    node->left = temp->right;

    if (temp->right != sentinel) {
        temp->right->parent = node;
    }

    temp->parent = node->parent;

    if (node == *root) {
        *root = temp;

    } else if (node == node->parent->right) {
        node->parent->right = temp;

    } else {
        node->parent->left = temp;
    }

    temp->right = node;
    node->parent = temp;
}



zbaselib_rbtree_node *
zbaselib_rbtree_min_intenal(zbaselib_rbtree_node* node, zbaselib_rbtree_node* sentinel)
{	
	while (node->left != sentinel) {
		node = node->left;
	}

	return node;
}


int zbaselib_rbtree_empty(zbaselib_rbtree* tree)
{
	return (tree->size == 0);
}

int zbaselib_rbtree_size(zbaselib_rbtree* tree)
{
	return tree->size;
}

zbaselib_rbtree* zbaselib_rbtree_init(compareFunc cmp)
{
	zbaselib_rbtree* tree = (zbaselib_rbtree*)calloc(1, sizeof(zbaselib_rbtree));
	if(tree != NULL)
	{
		zbaselib_rbtree_node* s = (zbaselib_rbtree_node*)calloc(1, sizeof(zbaselib_rbtree_node));
		zbaselib_rbtree_sentinel_init(s);                                              
		tree->root = s;                                                         
		tree->sentinel = s; 	
		tree->size = 0;
		tree->cmp = cmp;
	}

	return tree;
}

void zbaselib_rbtree_deinit(zbaselib_rbtree* tree)
{
	while(!zbaselib_rbtree_empty(tree))
	{
		zbaselib_rbtree_popmin(tree);
	}

	free(tree->sentinel);
	free(tree);
}

void zbaselib_rbtree_push(zbaselib_rbtree* tree, void* key)
{
	zbaselib_rbtree_node  **root, *temp, *sentinel;

	zbaselib_rbtree_node *node = (zbaselib_rbtree_node*)calloc(1, sizeof(zbaselib_rbtree_node));
	node->key = key;
	
    /* a binary tree insert */

    root = (zbaselib_rbtree_node **) &tree->root;
    sentinel = tree->sentinel;

    if (*root == sentinel) {
        node->parent = NULL;
        node->left = sentinel;
        node->right = sentinel;
        zbaselib_rbt_black(node);
        *root = node;

		++tree->size;
		return;
    }

    zbaselib_rbtree_insert_value(tree, *root, node, sentinel);


    /* re-balance tree */

    while (node != *root && zbaselib_rbt_is_red(node->parent)) {

        if (node->parent == node->parent->parent->left) {
            temp = node->parent->parent->right;

            if (zbaselib_rbt_is_red(temp)) {
                zbaselib_rbt_black(node->parent);
                zbaselib_rbt_black(temp);
                zbaselib_rbt_red(node->parent->parent);
                node = node->parent->parent;

            } else {
                if (node == node->parent->right) {
                    node = node->parent;
                    zbaselib_rbtree_left_rotate(root, sentinel, node);
                }

                zbaselib_rbt_black(node->parent);
                zbaselib_rbt_red(node->parent->parent);
                zbaselib_rbtree_right_rotate(root, sentinel, node->parent->parent);
            }

        } else {
            temp = node->parent->parent->left;

            if (zbaselib_rbt_is_red(temp)) {
                zbaselib_rbt_black(node->parent);
                zbaselib_rbt_black(temp);
                zbaselib_rbt_red(node->parent->parent);
                node = node->parent->parent;

            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    zbaselib_rbtree_right_rotate(root, sentinel, node);
                }

                zbaselib_rbt_black(node->parent);
                zbaselib_rbt_red(node->parent->parent);
                zbaselib_rbtree_left_rotate(root, sentinel, node->parent->parent);
            }
        }
    }

    zbaselib_rbt_black(*root);
	++tree->size;
}

void* zbaselib_rbtree_popmin(zbaselib_rbtree* tree)
{
	zbaselib_rbtree_node *node = NULL;
	zbaselib_rbtree_node *sentinel = NULL;
	void* key = NULL;
	
	if(tree->size == 0)
		return NULL;

	node = tree->root;
	sentinel = tree->sentinel;
	
	while (node->left != sentinel) {
		node = node->left;
	}

	key = node->key;
	zbaselib_rbtree_delete(tree, node);
	free(node);
	
	return key;
}

ZLIB_API void* zbaselib_rbtree_getmin(zbaselib_rbtree* tree)
{
	zbaselib_rbtree_node *node = NULL;
	zbaselib_rbtree_node *sentinel = NULL;
	void* key = NULL;
	
	if(tree->size == 0)
		return NULL;

	node = tree->root;
	sentinel = tree->sentinel;
	
	while (node->left != sentinel) {
		node = node->left;
	}

	key = node->key;	
	return key;
}

ZLIB_API void* zbaselib_rbtree_getmax(zbaselib_rbtree* tree)
{
	zbaselib_rbtree_node *node = NULL;
	zbaselib_rbtree_node *sentinel = NULL;
	void* key = NULL;
	
	if(tree->size == 0)
		return NULL;

	node = tree->root;
	sentinel = tree->sentinel;
	
	while (node->right != sentinel) {
		node = node->right;
	}

	key = node->key;	
	return key;
}



//查找节点
ZLIB_API zbaselib_rbtree_node* zbaselib_rbtree_find(zbaselib_rbtree* tree, void* key)
{
	int ret = 0;
	zbaselib_rbtree_node *node = NULL;

	if(tree == NULL)
		return NULL;

	node = tree->root;
	while(node)
	{
		ret = tree->cmp(node->key, key);
		if(ret == 0)
			return node;
		else if(ret > 0)
			node = node->left;
		else
			node = node->right;
	}

	return NULL;
}


// 删除节点
ZLIB_API int zbaselib_rbtree_delnode(zbaselib_rbtree* tree, void* key)
{
	zbaselib_rbtree_node *node = zbaselib_rbtree_find(tree, key);

	if(node == NULL || key == NULL)
		return -1;
	else
	{
		zbaselib_rbtree_delete(tree, node);
		free(node);
		return 0;
	}
}



