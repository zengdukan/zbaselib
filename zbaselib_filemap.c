#include "zbaselib_filemap.h"

struct _zbaselib_filemap
{
	char* data;
	size_t size;
};

#ifdef WIN32
zbaselib_filemap* zbaselib_filemap_create(const char* filename, 
										  size_t offset, size_t size)
{
	zbaselib_filemap* thiz = NULL;
	struct _stat st = {0};
	FILE* fp;

	if(filename == NULL)
		return NULL;

	if(_stat(filename, &st) != 0)
		return NULL;
	if(offset >= st.st_size)
		return NULL;

	size = (offset + size) < st.st_size ? size : st.st_size - offset;

	thiz = (zbaselib_filemap*) calloc(1, sizeof(zbaselib_filemap));
	if(thiz == NULL)
		return NULL;

	fp = fopen(filename, "rb");
	if(fp != NULL)
	{
		thiz->size = size;
		thiz->data = calloc(1, size);
		fseek(fp, offset, SEEK_SET);
		fread(thiz->data, thiz->size, 1, fp);
		fclose(fp);
	}

	if(thiz->data == NULL || thiz->data == NULL)
	{
		free(thiz);
		MESSAGE("map %s fail\n", filename);
	}

	return thiz;
}

char* zbaselib_filemap_data(zbaselib_filemap* thiz)
{
	return (thiz == NULL) ? NULL : thiz->data;
}

size_t zbaselib_filemap_size(zbaselib_filemap* thiz)
{
	return (thiz == NULL) ? 0 : thiz->size;
}

void zbaselib_filemap_destroy(zbaselib_filemap* thiz)
{
	if(thiz != NULL)
	{
		free(thiz->data);
		free(thiz);
	}
}

#else
zbaselib_filemap* zbaselib_filemap_create(const char* filename, 
										  size_t offset, size_t size)
{
	zbaselib_filemap* thiz = NULL;
	struct stat st = {0};
	
	if(filename == NULL)
		return NULL;

	if(stat(filename, &st) != 0)
	{
		return NULL;
	}

	if(offset >= st.st_size)
		return NULL;

	size = (offset + size) < st.st_size ? size : st.st_size - offset;

	thiz = (zbaselib_filemap*) calloc(1, sizeof(zbaselib_filemap));
	if(thiz == NULL) 
		return NULL;

	int fd = open(filename, O_RDONLY);
	if(fd > 0)
	{
		thiz->size = size;
		thiz->data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, offset);
		close(fd);
	}

	if(thiz->data == NULL || thiz->data == MAP_FAILED)
	{
		free(thiz);
		MESSAGE("map %s fail\n", filename);
	}

	return thiz;
}

char* zbaselib_filemap_data(zbaselib_filemap* thiz)
{
	return (thiz == NULL) ? 0 : thiz->data;
}

size_t zbaselib_filemap_size(zbaselib_filemap* thiz)
{
	return (thiz == NULL) ? 0 : thiz->size;
}

void zbaselib_filemap_destroy(zbaselib_filemap* thiz)
{
	if(thiz != NULL)
	{
		munmap(thiz->data, thiz->size);
		free(thiz);
	}
}

#endif
