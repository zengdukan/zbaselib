// ÒÆ×Ônginx¿â
#ifndef _ZBASELIB_MD5_
#define _ZBASELIB_MD5_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#ifdef WIN32
#include <windows.h>
typedef unsigned __int64 uint64_t;
typedef unsigned int uint32_t;

#ifdef DLL_EXPORT
#define ZLIB_API __declspec(dllexport)
#else
#define ZLIB_API 
#endif

#else
#include <stdint.h>

#define ZLIB_API 

#endif

typedef unsigned char u_char;

typedef struct {
    uint64_t  bytes;
    uint32_t  a, b, c, d;
    u_char    buffer[64];
} zbaselib_md5_t;


ZLIB_API void zbaselib_md5_init(zbaselib_md5_t *ctx);
ZLIB_API void zbaselib_md5_update(zbaselib_md5_t *ctx, const void *data, size_t size);
ZLIB_API void zbaselib_md5_final(u_char result[16], zbaselib_md5_t *ctx);



#ifdef __cplusplus
}
#endif

#endif /* _NGX_MD5_H_INCLUDED_ */
