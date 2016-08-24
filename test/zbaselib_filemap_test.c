#include "zbaselib_filemap.h"
#include "zbaselib_typedef.h"
#include <assert.h>

#ifdef WIN32
#define STAT _stat
#else
#define STAT stat
#endif

int main(int argc, char **argv)
{
	struct STAT _st = {0};
	zbaselib_filemap* map = NULL;
	FILE* file;

	if(argc != 2)
	{
		printf("input filename\n");
		return -1;
	}

	
	
	map = zbaselib_filemap_create(argv[1], 0, -1);
	
	if(STAT(argv[1], &_st) != 0)
	{
		printf("stat fail\n");
	}
	printf("size = %d\n", zbaselib_filemap_size(map));
	assert(_st.st_size == zbaselib_filemap_size(map));

	file = fopen("filemap", "wb");
	fwrite(zbaselib_filemap_data(map), zbaselib_filemap_size(map), 1, file);
	fclose(file);

	zbaselib_filemap_destroy(map);

	return 0;
}


