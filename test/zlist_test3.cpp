#include "zbaselib_list.h"
#include <stdio.h>
#include <assert.h>

class CTest
{
public:
	CTest(int a, int b)
	{
		this->a = a;
		this->b = b;
	}

	~CTest()
	{

	}

	void print()
	{
		printf("a = %d, b = %d\n", a, b);
	}

	int a;
	int b;
};

void destroy_ctest(void* data)
{
	delete (CTest*)data;
}

int main(int argc, char* argv[])
{
	zbaselib_list* list = zbaselib_list_create(destroy_ctest);
	zbaselib_list_iterater* zi = NULL;
	CTest* pi;

	for(int i = 0; i < 100; i++)
	{
		CTest* t = new CTest(i, i+1);
		zbaselib_list_addtail(list, (void*)t);
	}

	assert(100 == zbaselib_list_size(list));

	zi = zbaselib_list_iterater_create(list);
	for(pi = (CTest*)zbaselib_list_iterater_first(zi); !zbaselib_list_iterater_isend(zi); pi = (CTest*)zbaselib_list_iterater_next(zi))
	{
		pi->print();
	}

	zbaselib_list_destroy(&list);

	getchar();

	return 0;
}