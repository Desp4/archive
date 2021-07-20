#ifndef KENT_UTIL_H
#define KENT_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32

#include <Windows.h>

#define TH_CALL WINAPI

typedef DWORD TH_RET;
typedef HANDLE Thread;

#else

#include <pthread.h>

#define TH_CALL

typedef void *TH_RET;
typedef pthread_t Thread;

#endif

typedef TH_RET (TH_CALL *TH_ENTRYPOINT) (void *data);

/*
Convenience function,
outputs "error <error code>: <error string>\n"
to stderr.
*/
void log_err(int err);

/*
Get a string describing a system error code.
On Windows the string should be freed with LocalFree(),
otherwise DO NOT free() the string.

Returns a pointer to a system error string,
the returned string is null-terminated.
*/
char *err_sys_to_str(int err);

/*
Spawn a thread.

data   - data passed to the thread
start  - thread entry point
thread - thread handle

Returns 0 on success or a system error code otherwise.
*/
int thread_spawn(void *data, TH_ENTRYPOINT start, Thread *thread);

#ifdef __cplusplus
}
#endif

#endif