#ifndef _ZBASELIB_BUFFER_H_
#define _ZBASELIB_BUFFER_H_

#include "zbaselib_typedef.h"

ZBASELIB_BEGIN_DECL

// ����buffer
struct _zbaselib_buffer;
typedef struct _zbaselib_buffer zbaselib_buffer;

/*
 * ����buffer
 * @param buffer_size:buffer������
 * @param islock:buffer�Ƿ����,0-������,���õ��߳�
 * ��0-����,���ö��߳�����ͬ��
 * @����:�ɹ����ش�����bufferָ��,����ʧ�ܷ���NULL
 */
ZLIB_API zbaselib_buffer *zbaselib_buffer_create(unsigned int buffer_size, int islock);

/**
 * ��buffer��д��data_len�ֽ�,��ע��wdata�ĳ���Ҫ��data_len
 * @param thiz:д�����ݵ�bufferָ��
 * @param wdata:д�����ݵ�ָ��
 * @param data_len:д�����ݵĳ���
 * @return:-1-���������Ч,-2-buffer��д�ռ�С��data_len, 0-�ɹ�
 */
ZLIB_API int zbaselib_buffer_write(zbaselib_buffer *thiz, void *wdata, unsigned int data_len);

/**
 * ��buffer�ж���data_len���ȵ����ݵ�rdata
 * @param thiz: bufferָ��
 * @param rdata:������ָ��
 * @param data_len:��ȡ�ĳ���
 * @return:-1-���������Ч,-2-buffer�ɶ�����С��data_len, 0-�ɹ�
 */
ZLIB_API int zbaselib_buffer_read(zbaselib_buffer *thiz, void *rdata, unsigned int data_len);

ZLIB_API int zbaselib_buffer_skip(zbaselib_buffer *thiz, unsigned int data_len);


/**
 * ��buffer�ж���data_len���ȵ����ݵ�rdata,���ı��д��־
 * @param thiz: bufferָ��
 * @param rdata:������ָ��
 * @param data_len:��ȡ�ĳ���
 * @return:-1-���������Ч,-2-buffer�ɶ�����С��data_len, 0-�ɹ�
 */

ZLIB_API int zbaselib_buffer_get(zbaselib_buffer *thiz, void *rdata, unsigned int data_len);
/**
 * ����buffer�еĿɶ�����
 * @param thiz:buffer
 * @return:-1:������Ч,>=0����buffer�еĿɶ�����
 */
ZLIB_API unsigned int zbaselib_buffer_busynum(zbaselib_buffer *thiz);

/**
 * �ж�buffer�Ƿ��
 * @param thiz:bufferָ��
 * @return:0-����,��0-��
 */
ZLIB_API int zbaselib_buffer_isempty(zbaselib_buffer *thiz);

/**
 * �ж�buffer�Ƿ���
 * @param thiz:bufferָ��
 * @return:0-����,��0-��
 */
ZLIB_API int zbaselib_buffer_isfull(zbaselib_buffer *thiz);

/**
 * ���buffer
 * @param thiz:buffeָ��
 * @return
 */
ZLIB_API void zbaselib_buffer_clear(zbaselib_buffer *thiz);

/**
 * ����buffer
 * @param thiz:bufferָ��
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

