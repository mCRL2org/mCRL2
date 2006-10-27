#ifndef _NT_SUBSTITUTES_H_
#define _NT_SUBSTITUTES_H_

/*
 * Windows Compatibility Functions -- functions to support portability to the
 * Windows environment.
 */

#include <assert.h>
#include <stdio.h>
#include <process.h>
#include <errno.h>
#include <io.h>
#include <unistd.h>

#define chdir    _chdir
#define dup      _dup
#define dup2     _dup2
#define getpid   _getpid
#define pipe     _pipe
#define snprintf _snprintf

typedef int pid_t;

#ifndef STDIN_FILENO
# define STDIN_FILENO  _fileno(stdin)
# define STDOUT_FILENO _fileno(stdout)
# define STDERR_FILENO _fileno(stderr)
#endif

#define WNOHANG 0x00000001

inline pid_t waitpid(pid_t pid, int* status, int options) {

  return _cwait (status, pid, _WAIT_CHILD);
}

#endif
