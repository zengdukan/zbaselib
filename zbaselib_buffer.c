#include "zbaselib_buffer.h"
#include "zbaselib_thread.h"

struct _zbaselib_buffer
{
	void *			buffer;				//buffer地址
	unsigned int 	buffer_size;	//buffer大小
	unsigned int 	wpos;			//写位置
	unsigned int 	rpos;			//读位置
	unsigned int 	busy_num;		//未处理数据的大小
	zbaselib_mutex_t	mutex;		// 锁
	int 			islock;		//是否加锁
};

#define ZBUFFER_LOCK		if(thiz->islock != 0)	{zbaselib_mutex_lock(thiz->mutex);}
#define ZBUFFER_UNLOCK		if(thiz->islock != 0)	{zbaselib_mutex_unlock(thiz->mutex);}

zbaselib_buffer *zbaselib_buffer_create(unsigned int buffer_size, int islock)
{
	zbaselib_buffer *thiz = (zbaselib_buffer *)calloc(sizeof(zbaselib_buffer), 1);
	if(thiz != NULL)
	{
		thiz->buffer_size = buffer_size;
		thiz->buffer = calloc(buffer_size, 1);
		if(thiz->buffer == NULL)
		{
			return NULL;
		}

		thiz->islock = islock;
		if(islock != 0)
		{
			thiz->mutex = zbaselib_mutex_create();
		}
	}

	return thiz;
}

int zbaselib_buffer_write(zbaselib_buffer *thiz, void *wdata, unsigned int data_len)
{
	unsigned int left = 0;

	if(thiz == NULL || thiz->buffer == NULL
		|| wdata == NULL || data_len <= 0)
	{
		return -1;
	}
	
	//加锁
	ZBUFFER_LOCK;//pthread_mutex_lock(&thiz->mutex);
	//计算剩余容量
	left = thiz->buffer_size - thiz->busy_num;
	if(data_len > left)
	{
		ZBUFFER_UNLOCK;//pthread_mutex_unlock(&thiz->mutex);
		return -2;
	}

	if(thiz->wpos + data_len <= thiz->buffer_size)
	{
		memcpy((char*)thiz->buffer + thiz->wpos, wdata, data_len);
		thiz->wpos += data_len;

		if(thiz->wpos >= thiz->buffer_size)
		{
			thiz->wpos = 0;
		}
	}
	else
	{
		memcpy((char*)thiz->buffer + thiz->wpos, wdata, thiz->buffer_size - thiz->wpos);
		memcpy(thiz->buffer, (char*)wdata + thiz->buffer_size - thiz->wpos,
			data_len + thiz->wpos - thiz->buffer_size);

		thiz->wpos = data_len + thiz->wpos - thiz->buffer_size;
	}

	thiz->busy_num += data_len;

	ZBUFFER_UNLOCK;//pthread_mutex_unlock(&thiz->mutex);
	
	return 0;
}

int zbaselib_buffer_read(zbaselib_buffer *thiz, void *rdata, unsigned int data_len)
{
	if(thiz == NULL || thiz->buffer == NULL
		||rdata == NULL || data_len <=0)
	{
		return -1;
	}

	ZBUFFER_LOCK;//pthread_mutex_lock(&thiz->mutex);
	
	if(thiz->busy_num < data_len)
	{
		ZBUFFER_UNLOCK;//pthread_mutex_unlock(&thiz->mutex);
		return -2;
	}

	if(thiz->rpos+ data_len <= thiz->buffer_size)
	{
		memcpy(rdata, (char*)thiz->buffer + thiz->rpos, data_len);
		thiz->rpos += data_len;

		if(thiz->rpos == thiz->buffer_size)
		{
			thiz->rpos = 0;
		}
	}
	else
	{
		memcpy(rdata, (char*)thiz->buffer + thiz->rpos, thiz->buffer_size - thiz->rpos);
		memcpy((char*)rdata + thiz->buffer_size - thiz->rpos, thiz->buffer,
			data_len + thiz->rpos - thiz->buffer_size);

		thiz->rpos = data_len + thiz->rpos - thiz->buffer_size;
	}

	thiz->busy_num -= data_len;

	ZBUFFER_UNLOCK;//pthread_mutex_unlock(&thiz->mutex);
	return 0;
}

int zbaselib_buffer_skip(zbaselib_buffer *thiz, unsigned int data_len)
{
	if(thiz == NULL || thiz->buffer == NULL || data_len <=0)
	{
		return -1;
	}

	ZBUFFER_LOCK;//pthread_mutex_lock(&thiz->mutex);
	
	if(thiz->busy_num < data_len)
	{
		ZBUFFER_UNLOCK;//pthread_mutex_unlock(&thiz->mutex);
		return -2;
	}

	if(thiz->rpos+ data_len <= thiz->buffer_size)
	{
		thiz->rpos += data_len;

		if(thiz->rpos == thiz->buffer_size)
		{
			thiz->rpos = 0;
		}
	}
	else
	{
		thiz->rpos = data_len + thiz->rpos - thiz->buffer_size;
	}

	thiz->busy_num -= data_len;

	ZBUFFER_UNLOCK;//pthread_mutex_unlock(&thiz->mutex);
	return 0;
}


int zbaselib_buffer_get(zbaselib_buffer *thiz, void *rdata, unsigned int data_len)
{
	if(thiz == NULL || thiz->buffer == NULL
		||rdata == NULL || data_len <=0)
	{
		return -1;
	}

	ZBUFFER_LOCK;//pthread_mutex_lock(&thiz->mutex);
	
	if(thiz->busy_num < data_len)
	{
		ZBUFFER_UNLOCK;//pthread_mutex_unlock(&thiz->mutex);
		return -2;
	}

	if(thiz->rpos+ data_len <= thiz->buffer_size)
	{
		memcpy(rdata, (char*)thiz->buffer + thiz->rpos, data_len);
#if 0
		thiz->rpos += data_len;

		if(thiz->rpos == thiz->buffer_size)
		{
			thiz->rpos = 0;
		}
#endif
	}
	else
	{
		memcpy(rdata, (char*)thiz->buffer + thiz->rpos, thiz->buffer_size - thiz->rpos);
		memcpy((char*)rdata + thiz->buffer_size - thiz->rpos, thiz->buffer,
			data_len + thiz->rpos - thiz->buffer_size);
#if 0
		thiz->rpos = data_len + thiz->rpos - thiz->buffer_size;
#endif
	}
#if 0
	thiz->busy_num -= data_len;
#endif
	ZBUFFER_UNLOCK;
	return 0;
}


unsigned int zbaselib_buffer_busynum(zbaselib_buffer *thiz)
{
	unsigned int ret = 0;

	if(thiz == NULL || thiz->buffer == NULL)
		return -1;

	ZBUFFER_LOCK;//pthread_mutex_lock(&thiz->mutex);
	ret = thiz->busy_num;
	ZBUFFER_UNLOCK;//pthread_mutex_unlock(&thiz->mutex);
	
	return ret;
}

int zbaselib_buffer_isempty(zbaselib_buffer *thiz)
{
	int ret = 0;

	ZBUFFER_LOCK;//pthread_mutex_lock(&thiz->mutex);
	ret = (thiz != NULL && thiz->buffer != NULL \
		&& thiz->busy_num == 0);
	ZBUFFER_UNLOCK;//pthread_mutex_unlock(&thiz->mutex);

	return ret;
}	

int zbaselib_buffer_isfull(zbaselib_buffer *thiz)
{
	int ret = 0;

	ZBUFFER_LOCK;//pthread_mutex_lock(&thiz->mutex);
	ret = (thiz != NULL && thiz->buffer != NULL\
		&&thiz->busy_num == thiz->buffer_size);
	ZBUFFER_UNLOCK;//pthread_mutex_unlock(&thiz->mutex);

	return ret;
}

void zbaselib_buffer_clear(zbaselib_buffer *thiz)
{
	ZBUFFER_LOCK;//pthread_mutex_lock(&thiz->mutex);

	if(thiz == NULL || thiz->buffer == NULL)
	{	
		ZBUFFER_UNLOCK;//pthread_mutex_unlock(&thiz->mutex);
		return;
	}
	
	thiz->wpos = 0;
	thiz->rpos = 0;
	thiz->busy_num = 0;

	ZBUFFER_UNLOCK;//pthread_mutex_unlock(&thiz->mutex);
}

void zbaselib_buffer_destory(zbaselib_buffer *thiz)
{
	if(thiz == NULL || thiz->buffer == NULL)
	{
		return;
	}

	free(thiz->buffer);
	thiz->buffer = NULL;
	if(thiz->islock != 0)
	{
		zbaselib_mutex_destroy(thiz->mutex);
	}
	free(thiz);
	//thiz = NULL;
}

char* zbaselib_buffer_get_readpos(zbaselib_buffer* thiz)
{
	if(thiz == NULL || thiz->buffer == NULL)
	{
		return NULL;
	}

	return (char*)thiz->buffer + thiz->rpos;
}

char* zbaselib_buffer_get_writepos(zbaselib_buffer* thiz)
{
	if(thiz == NULL || thiz->buffer == NULL)
	{
		return NULL;
	}

	return (char*)thiz->buffer + thiz->wpos;
}

zbuffer_t* zbuffer_new(const char* data, int data_size, int buf_size)
{
	zbuffer_t* buf = (zbuffer_t*) calloc(1, sizeof(zbuffer_t));

	if(buf != NULL)
	{
		if(buf_size < 1)
		buf_size = 1;

		if(buf_size < data_size)
			buf_size = data_size;

		if(data == NULL)
			data_size = 0;

		buf->buf_size = buf_size;
		buf->data_size = data_size;
		buf->buf = (char*)calloc(1, buf->buf_size);

		if(data != NULL)
			memcpy(buf->buf, data, data_size);
	}

	return buf;
}

void zbuffer_delete(zbuffer_t* zbuf)
{
	if(zbuf != NULL)
	{
		if(zbuf->buf != NULL)
			free(zbuf->buf);

		free(zbuf);
	}
}

int zbuffer_set(zbuffer_t* zbuf, const char* data, int data_size)
{
	if(zbuf == NULL)
		return -1;
	
	if(zbuf->buf_size < data_size)
	{
		zbuf->buf_size = data_size;
		zbuf->buf = (char*)realloc(zbuf->buf, zbuf->buf_size);
	}

	if(data != NULL)
		memcpy(zbuf->buf, data, data_size);

	zbuf->data_size = data_size;

	return 0;
}

