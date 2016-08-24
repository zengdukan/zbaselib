#ifndef _ZBASELIB_BUFFER_H_
#define _ZBASELIB_BUFFER_H_

#include "zbaselib_typedef.h"

ZBASELIB_BEGIN_DECL

// 环形buffer
struct _zbaselib_buffer;
typedef struct _zbaselib_buffer zbaselib_buffer;

/*
 * 创建buffer
 * @param buffer_size:buffer的容量
 * @param islock:buffer是否加锁,0-不加锁,适用单线程
 * 非0-加锁,适用多线程数据同步
 * @返回:成功返回创建的buffer指针,创建失败返回NULL
 */
ZLIB_API zbaselib_buffer *zbaselib_buffer_create(unsigned int buffer_size, int islock);

/**
 * 向buffer中写入data_len字节,须注意wdata的长度要≥data_len
 * @param thiz:写入数据的buffer指针
 * @param wdata:写入数据的指针
 * @param data_len:写入数据的长度
 * @return:-1-传入参数无效,-2-buffer可写空间小于data_len, 0-成功
 */
ZLIB_API int zbaselib_buffer_write(zbaselib_buffer *thiz, void *wdata, unsigned int data_len);

/**
 * 从buffer中读出data_len长度的数据到rdata
 * @param thiz: buffer指针
 * @param rdata:读数据指针
 * @param data_len:读取的长度
 * @return:-1-传入参数无效,-2-buffer可读数据小于data_len, 0-成功
 */
ZLIB_API int zbaselib_buffer_read(zbaselib_buffer *thiz, void *rdata, unsigned int data_len);

ZLIB_API int zbaselib_buffer_skip(zbaselib_buffer *thiz, unsigned int data_len);


/**
 * 从buffer中读出data_len长度的数据到rdata,不改变读写标志
 * @param thiz: buffer指针
 * @param rdata:读数据指针
 * @param data_len:读取的长度
 * @return:-1-传入参数无效,-2-buffer可读数据小于data_len, 0-成功
 */

ZLIB_API int zbaselib_buffer_get(zbaselib_buffer *thiz, void *rdata, unsigned int data_len);
/**
 * 返回buffer中的可读数据
 * @param thiz:buffer
 * @return:-1:参数无效,>=0返回buffer中的可读数据
 */
ZLIB_API unsigned int zbaselib_buffer_busynum(zbaselib_buffer *thiz);

/**
 * 判断buffer是否空
 * @param thiz:buffer指针
 * @return:0-不空,非0-空
 */
ZLIB_API int zbaselib_buffer_isempty(zbaselib_buffer *thiz);

/**
 * 判断buffer是否满
 * @param thiz:buffer指针
 * @return:0-不满,非0-满
 */
ZLIB_API int zbaselib_buffer_isfull(zbaselib_buffer *thiz);

/**
 * 清空buffer
 * @param thiz:buffe指针
 * @return
 */
ZLIB_API void zbaselib_buffer_clear(zbaselib_buffer *thiz);

/**
 * 销毁buffer
 * @param thiz:buffer指针
 * @return
 */
ZLIB_API void zbaselib_buffer_destory(zbaselib_buffer *thiz);

ZLIB_API char* zbaselib_buffer_get_readpos(zbaselib_buffer* thiz);

ZLIB_API char* zbaselib_buffer_get_writepos(zbaselib_buffer* thiz);

/***********************************************************************************/
/***********************************************************************************/

typedef struct zbuffer_s
{
	char* buf;
	int data_size;
	int buf_size;
} zbuffer_t;

ZLIB_API zbuffer_t* zbuffer_new(const char* data, int data_size, int buf_size);

ZLIB_API void zbuffer_delete(zbuffer_t* zbuf);

ZLIB_API int zbuffer_set(zbuffer_t* zbuf, const char* data, int data_size);



ZBASELIB_END_DECL

#endif

