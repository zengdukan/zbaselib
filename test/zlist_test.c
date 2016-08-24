#include "zbaselib_list.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#ifndef WIN32
#include <pthread.h>
#include <unistd.h>
#endif

void print_int(void *data, void *ctx)
{
	if(data == NULL)
	{
		printf("NULL\n");
		return;
	}
	
	printf("%d\n", *(int *)data);
}

void sum_int(void *data, void *ctx)
{
	int *sum = (int *)ctx;
	*sum += *(int *)data;
}


#if 0
typedef struct _TT
{
	zbaselib_list	*zlist;
	pthread_mutex_t mutex;
}TT;

int g_read = 0;
int g_write = 0;

void *zlist_read(void *arg)
{
	TT *tt = (TT *)arg;

	while(1)
	{
		pthread_mutex_lock(&(tt->mutex));

		if(!zbaselib_list_empty(tt->zlist))
		{
			int *tmp = (int *)zbaselib_list_gethead(tt->zlist);	
			//fprintf(stderr, "%d\n", g_read);
			assert(*tmp == g_read);
			g_read++;

			zbaselib_list_delhead(tt->zlist);
		}
		
		pthread_mutex_unlock(&(tt->mutex));
		//usleep(100);
	}

	return NULL;
}

void *zlist_write(void *arg)
{
	TT *tt = (TT *)arg;

	while(1)
	{
		pthread_mutex_lock(&(tt->mutex));

		int *tmp = (int *)malloc(sizeof(int));
		*tmp = g_write;
		g_write++;

		zbaselib_list_addtail(tt->zlist, (void *)tmp);
		
		pthread_mutex_unlock(&(tt->mutex));
		//usleep(100);
	}

	return NULL;
}

void destroy_tt(void* data)
{
	free(data);
}

int main(int argc, char *argv[])
{
	g_read = 0;
	g_write = 0;
	
	TT *tt = (TT *)calloc(sizeof(TT), 1);
	tt->zlist = zbaselib_list_create(destroy_tt);
	pthread_mutex_init(&(tt->mutex), NULL);

	pthread_t pid1 = 0, pid2 = 0;

	if(pthread_create(&pid1, NULL, zlist_write, (void *)tt) < 0)
	{
		perror("zlist_write:");
		return -1;
	}

	if(pthread_create(&pid2, NULL, zlist_read, (void *)tt) < 0)
	{
		perror("zlist_read:");
		return -1;
	}

	pthread_join(pid1, NULL);
	pthread_join(pid2, NULL);

	zbaselib_list_destroy(&(tt->zlist));
	pthread_mutex_destroy(&(tt->mutex));
	free(tt);
	tt = NULL;

	return 0;
}
#endif
#if 1

void destroy_int(void* data)
{
	free(data);
}

int cmp_fun(void *data, void *ctx)
{
	return (*(int*)data == *(int*)ctx);
}

int main(int argc, char *argv[])
{
	int i = 0;
	int sum = 0;
	zbaselib_list *zlist = zbaselib_list_create(destroy_int);
	char sz[10] = {1, 3, 3, 3, 5, 5, 5, 7, 9, 7};
	assert(zbaselib_list_empty(zlist));
	
	for(i = 0; i < 10; i++)
	{
		int *tmp = (int *)malloc(sizeof(int));
		*tmp = sz[i];
		zbaselib_list_addtail(zlist, (void *)tmp);
	}

	assert(zbaselib_list_size(zlist) == 10);
	zbaselib_list_foreach(zlist, print_int, NULL);
	zbaselib_list_foreach(zlist, sum_int, &sum);
	printf("sum = %d\n", sum);
	
	i = 9;
	zbaselib_list_del_use_cond(zlist, (DataCmpFunc)cmp_fun, &i);

	zbaselib_list_foreach(zlist, print_int, NULL);

	zbaselib_list_destroy(&zlist);

	getchar();
	return 0;
}
#endif

