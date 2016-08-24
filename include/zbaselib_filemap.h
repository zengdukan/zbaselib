#ifndef _ZBASELIB_FILE_MAP_H_
#define _ZBASELIB_FILE_MAP_H_

#include "zbaselib_typedef.h"

ZBASELIB_BEGIN_DECL

typedef struct _zbaselib_filemap zbaselib_filemap;

ZLIB_API zbaselib_filemap* zbaselib_filemap_create(const char* filename, size_t offset, size_t size);

ZLIB_API char* zbaselib_filemap_data(zbaselib_filemap* thiz);

ZLIB_API size_t zbaselib_filemap_size(zbaselib_filemap* thiz);

ZLIB_API void zbaselib_filemap_destroy(zbaselib_filemap* thiz);

ZBASELIB_END_DECL

#endif/*_ZBASELIB_FILE_MAP_H_*/