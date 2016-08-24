#include "zbaselib_rbtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>

const int MAX_N = 100;

int cmp_int(const void* data1, const void* data2)
{
	// < ÉýÐò
	// > ½µÐò
	return (data1 - data2);
}

int main(int argc, char* argv[])
{
	zbaselib_rbtree* timer_tree;
	void* min = NULL;
	void* max; 
	int i = 0;
	struct timeval tpstart;
	int key;
	
	timer_tree = zbaselib_rbtree_init(cmp_int);

	
	min = zbaselib_rbtree_getmin(timer_tree);
	max = zbaselib_rbtree_getmax(timer_tree);
	assert(min == NULL);
	assert(max == NULL);
	min = (void*)2;
	max = (void*)100;
	assert(-1 == zbaselib_rbtree_delnode(timer_tree, min));
	assert(-1 == zbaselib_rbtree_delnode(timer_tree, max));
	assert(zbaselib_rbtree_size(timer_tree) == 0);
	
	gettimeofday(&tpstart,NULL);
	srand(tpstart.tv_usec);
	for(i = 1; i <= MAX_N; i++)
	{
		key = 1+(int) (1000.0*rand()/(RAND_MAX+1.0));
		zbaselib_rbtree_push(timer_tree, (void*)key);
		assert(zbaselib_rbtree_size(timer_tree) == i);
		printf("%d ", key);
	}
	printf("\n\n");

	printf("test find\n");
	for(i = 1; i <= 1000; i++)
	{
		if(zbaselib_rbtree_find(timer_tree, (void *)i) != NULL)
			printf("%d ", i);
		assert(zbaselib_rbtree_size(timer_tree) == MAX_N);
	}
	printf("\n\n");

	printf("zbaselib_rbtree_size(timer_tree) = %d\n", zbaselib_rbtree_size(timer_tree));
	printf("min = %d, max = %d\n", (int)zbaselib_rbtree_getmin(timer_tree), (int)zbaselib_rbtree_getmax(timer_tree));


	printf("test delete node\n");
	min = zbaselib_rbtree_getmin(timer_tree);
	max = zbaselib_rbtree_getmax(timer_tree);
	i = zbaselib_rbtree_size(timer_tree);
	assert(0 == zbaselib_rbtree_delnode(timer_tree, min));
	--i;
	assert(0 == zbaselib_rbtree_delnode(timer_tree, max));
	--i;
	
	while(NULL != zbaselib_rbtree_find(timer_tree, min))
	{
		assert(0 == zbaselib_rbtree_delnode(timer_tree, min));
		--i;
	}
	while(NULL != zbaselib_rbtree_find(timer_tree, max))
	{
		assert(0 == zbaselib_rbtree_delnode(timer_tree, max));
		--i;
	}
	
	assert(-1 == zbaselib_rbtree_delnode(timer_tree, min));
	assert(-1 == zbaselib_rbtree_delnode(timer_tree, max));
	assert(zbaselib_rbtree_size(timer_tree) == (i));


	printf("\nmin = %d, max = %d\n\n", (int)zbaselib_rbtree_getmin(timer_tree), (int)zbaselib_rbtree_getmax(timer_tree));
	i = zbaselib_rbtree_size(timer_tree);
	while((min = zbaselib_rbtree_popmin(timer_tree)) != NULL)
	{
		printf("%d ", (int)min);
		--i;
		assert(zbaselib_rbtree_size(timer_tree) == i);
	}

	printf("\n\n");

	zbaselib_rbtree_deinit(timer_tree);

	return 0;
}
