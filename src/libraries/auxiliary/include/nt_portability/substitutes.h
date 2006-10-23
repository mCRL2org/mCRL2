#ifndef _NT_SUBSTITUTES_H_
#define _NT_SUBSTITUTES_H_

/*
 * Windows Compatibility Functions -- functions to support portability to the
 * Windows environment.
 */

# define snprintf _snprintf
# define getpid   _getpid

typedef int pid_t;

#include <assert.h>
#include <stdio.h>
#include <process.h>
#include <errno.h>
#include <unistd.h>

#define WNOHANG 0x00000001

inline pid_t waitpid(pid_t pid, int* status, int options) {

  return _cwait (status, pid, _WAIT_CHILD);
}

#endif
