#include "util.h"

#ifdef _WIN32
#else // _WIN32

#include <string.h>

#endif // UNIX

#include <stdio.h>
#include <stdlib.h>

void log_err(int err)
{
	char *errstr = err_sys_to_str(err);
	fprintf(stderr, "error %i: %s\n", err, errstr);
#ifdef _WIN32
	LocalFree(errstr);
#endif
}

char *err_sys_to_str(int err)
{
#ifdef _WIN32
    LPSTR errbuf = NULL;
    DWORD len = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                    FORMAT_MESSAGE_FROM_SYSTEM     |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    err,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPSTR)&errbuf,
                    0,
                    NULL);
    // no way to remove newline on system errors, do it by hand
    errbuf[len - 1] = '\0';
    return errbuf;
#else
    return strerror(err);
#endif
}

int thread_spawn(void *data, TH_ENTRYPOINT start, Thread *thread)
{
#ifdef _WIN32
	*thread = CreateThread(
		NULL,
		0,
		start,
		(LPVOID)data,
		0,
		NULL);
    return *thread ? 0 : GetLastError();
#else
    return pthread_create(thread, NULL, start, data);
#endif
}