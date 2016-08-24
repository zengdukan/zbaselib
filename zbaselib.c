#include "zbaselib.h"

#ifdef WIN32

int zbaselib_gettimeofday(struct timeval* tv)
{
	struct _timeb tb;

	if (tv == NULL)
		return -1;

	_ftime(&tb);
	tv->tv_sec = (long) tb.time;
	tv->tv_usec = ((int) tb.millitm) * 1000;
	return 0;
}

/*
 * Change all slashes to backslashes. It is Windows.
 */
static void
fix_directory_separators(char *path)
{
	int	i;

	for (i = 0; path[i] != '\0'; i++) {
		if (path[i] == '/')
			path[i] = '\\';
		/* i > 0 check is to preserve UNC paths, \\server\file.txt */
		if (path[i] == '\\' && i > 0)
			while (path[i + 1] == '\\' || path[i + 1] == '/')
				(void) memmove(path + i + 1,
				    path + i + 2, strlen(path + i + 1));
	}
}

/*
 * Encode 'path' which is assumed UTF-8 string, into UNICODE string.
 * wbuf and wbuf_len is a target buffer and its length.
 */
static void
to_unicode(const char *path, wchar_t *wbuf, size_t wbuf_len)
{
	char	buf[MAX_PATH], *p;

	strncpy(buf, path, sizeof(buf));
	fix_directory_separators(buf);

	/* Point p to the end of the file name */
	p = buf + strlen(buf) - 1;

	/* Trim trailing backslash character */
	while (p > buf && *p == '\\' && p[-1] != ':')
		*p-- = '\0';

	/*
	 * Protect from CGI code disclosure.
	 * This is very nasty hole. Windows happily opens files with
	 * some garbage in the end of file name. So fopen("a.cgi    ", "r")
	 * actually opens "a.cgi", and does not return an error!
	 */
	if (*p == 0x20 || *p == 0x2e || *p == 0x2b || (*p & ~0x7f)) {
		(void) fprintf(stderr, "Rejecting suspicious path: [%s]", buf);
		buf[0] = '\0';
	}

	(void) MultiByteToWideChar(CP_UTF8, 0, buf, -1, wbuf, (int) wbuf_len);
}

/*
 * Implementation of POSIX opendir/closedir/readdir for Windows.
 */
DIR *opendir(const char *name)
{
	DIR	*dir = NULL;
	wchar_t	wpath[MAX_PATH];
	DWORD attrs;

	if (name == NULL) {
		SetLastError(ERROR_BAD_ARGUMENTS);
	} else if ((dir = (DIR *) malloc(sizeof(*dir))) == NULL) {
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
	} else {
		to_unicode(name, wpath, sizeof(wpath)/sizeof(wpath[0]));
		attrs = GetFileAttributesW(wpath);
		if (attrs != 0xFFFFFFFF &&
		    ((attrs & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)) {
			(void) wcscat(wpath, L"\\*");
			dir->handle = FindFirstFileW(wpath, &dir->info);
			dir->result.d_name[0] = '\0';
		} else {
			free(dir);
			dir = NULL;
		}
	}

	return (dir);
}

int closedir(DIR *dir)
{
	int result = 0;

	if (dir != NULL) {
		if (dir->handle != INVALID_HANDLE_VALUE)
			result = FindClose(dir->handle) ? 0 : -1;

		free(dir);
	} else {
		result = -1;
		SetLastError(ERROR_BAD_ARGUMENTS);
	}

	return (result);
}

struct dirent *readdir(DIR *dir)
{
	struct dirent *result = 0;

	if (dir) {
		if (dir->handle != INVALID_HANDLE_VALUE) {
			result = &dir->result;
			(void) WideCharToMultiByte(CP_UTF8, 0,
			    dir->info.cFileName, -1, result->d_name,
			    sizeof(result->d_name), NULL, NULL);

			if (!FindNextFileW(dir->handle, &dir->info)) {
				(void) FindClose(dir->handle);
				dir->handle = INVALID_HANDLE_VALUE;
			}

		} else {
			SetLastError(ERROR_FILE_NOT_FOUND);
		}
	} else {
		SetLastError(ERROR_BAD_ARGUMENTS);
	}

	return (result);
}

int zbaselib_pipe(int pipefd[2], unsigned int psize)
{
	return _pipe(pipefd, psize, _O_BINARY);
}

int zbaselib_pipe_close(int pipefd)
{
	return _close(pipefd);
}


#else

int zbaselib_gettimeofday(struct timeval* tv)
{
	return gettimeofday(tv, NULL);
}


int zbaselib_pipe(int pipefd[2], unsigned int psize)
{
	return pipe(pipefd);
}

int zbaselib_pipe_close(int pipefd)
{
	return close(pipefd);
}

#endif

#ifndef LLONG_MAX
#   define LLONG_MAX    9223372036854775807LL
#endif

#define MAX_SECONDS_IN_MSEC_LLONG \
	(((LLONG_MAX) - 999) / 1000)

llong zbaselib_tv_to_msec(const struct timeval *tv)
{
	if (tv->tv_usec > 1000000 || tv->tv_sec > MAX_SECONDS_IN_MSEC_LLONG)
	{
		return -1;
	}

	return (((long long)tv->tv_sec * 1000) + ((tv->tv_usec + 999) / 1000));
}

unsigned int ngx_hash_key(unsigned char *data, size_t len)
{
    unsigned int  i, key;

    key = 0;

    for (i = 0; i < len; i++) {
        key = ngx_hash(key, data[i]);
    }

    return key;
}

int zbaselib_createdir(const char* dirPath)
{
	char DirName[256] = {0}; 
	int i = 0, len = 0;

	memset(DirName, 0, sizeof(DirName));
	strcpy(DirName, dirPath); 
	len = strlen(DirName); 
	if(DirName[len-1]!='/') 
		DirName[len] = '/'; 

	len = strlen(DirName); 

	for(i=1; i<len; i++) 
	{ 
		if(DirName[i]=='/') 
		{ 
			DirName[i] = 0; 
			if( zbaselib_access(DirName, zbaselib_F_OK) != 0 ) 
			{ 
				if(zbaselib_mkdir(DirName) == -1) 
				{ 
					perror("mkdir error"); 
					return -1; 
				} 
			} 
			DirName[i] = '/'; 
		} 
	} 

	return 0; 
}
