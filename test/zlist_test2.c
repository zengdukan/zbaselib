#include "zbaselib_list.h"
#include <assert.h>

void destroy_int(void* data)
{
	free(data);
}

static int zbaselib_list_addbymintime(zbaselib_list* thiz, int* ev)
{
	int* pevent = NULL;
	int i = 0;
	zbaselib_list_iterater* zi = NULL;

	if(thiz == NULL || ev == pevent)
		return -1;

	zi = zbaselib_list_iterater_create(thiz);

	for(pevent = zbaselib_list_iterater_first(zi); pevent != NULL && !zbaselib_list_iterater_isend(zi); pevent = zbaselib_list_iterater_next(zi), i++)
	{
		if(*pevent >= *ev)
			break;
	}

	zbaselib_list_add(thiz, (void*)ev, i);
	zbaselib_list_iterater_destroy(&zi);

	return 0;
}

void test_min()
{
	zbaselib_list_iterater* zi = NULL;
	int size = 200;
	int i = 0;
	int* pi = NULL;

	zbaselib_list *zlist = zbaselib_list_create(destroy_int);
	for(i = 0; i < size; i++)
	{
		int* data = (int*)malloc(sizeof(int));
		*data = rand()%100;
		printf("%d ", *data);
		zbaselib_list_addbymintime(zlist, data);
	}

	printf("\n--------------------------------\n");

	zi = zbaselib_list_iterater_create(zlist);
	
	for(pi = zbaselib_list_iterater_first(zi); !zbaselib_list_iterater_isend(zi); pi = zbaselib_list_iterater_next(zi))
	{
		printf("%d ", *pi);
	}
	printf("\n--------------------------------\n");

	zbaselib_list_iterater_destroy(&zi);
	zbaselib_list_destroy(&zlist);

	getchar();
}

#if 1
int main(int argc, char *argv[])
{
	zbaselib_list *zlist = zbaselib_list_create(destroy_int);
	
	int size = 10;
	int i = 0;
	int* pi = NULL;
	zbaselib_list_iterater* zi;

	for(i = 0; i < size; i++)
	{
		int *tmp;
		int *tail;
		int *head;

		printf("%d\n", i);
		
		tmp = (int *)malloc(sizeof(int));
		*tmp = i;
		zbaselib_list_addtail(zlist, (void *)tmp);
		
		assert(zbaselib_list_size(zlist) == (i+1));

		tail = (int *)zbaselib_list_gettail(zlist);
		assert(*tail == i);

		head = (int *)zbaselib_list_gethead(zlist);
		assert(*head == 0);
	}
	
        zi = zbaselib_list_iterater_create(zlist);
	    
        for(pi = zbaselib_list_iterater_first(zi); !zbaselib_list_iterater_isend(zi); pi = zbaselib_list_iterater_next(zi))
        {
            printf("pi = %d\n", *pi);
        }
        
        printf("------------------------\n");
        for(pi = zbaselib_list_iterater_first(zi); !zbaselib_list_iterater_isend(zi); pi = zbaselib_list_iterater_next(zi))
        {
            printf("pi = %d\n", *pi);
        }
        
        printf("------------------------\n");
        for(pi = zbaselib_list_iterater_last(zi); !zbaselib_list_iterater_isend(zi); pi = zbaselib_list_iterater_next(zi))
        {
            printf("pi = %d\n", *pi);
        }
/*
		printf("------------------------\n");
        for(pi = zbaselib_list_iterater_first(zi); !zbaselib_list_iterater_isend(zi); pi = zbaselib_list_iterater_next(zi))
        {
			if(*pi == 5)
				zbaselib_list_del(zlist, *pi);
            else
				printf("pi = %d\n", *pi);
        }
		*/
        zbaselib_list_iterater_destroy(&zi);
	zbaselib_list_destroy(&zlist);
	
	return 0;
}
#else
int main(int argc, char *argv[])
{
	test_min();

	return 0;
}
#endif
