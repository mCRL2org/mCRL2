#ifndef _MSVC_WORKAROUNDS_H_
#define _MSVC_WORKAROUNDS_H_

/*
 * Windows Compatibility Functions -- functions to support portability to the
 * Windows environment.
 */

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cerrno>
#include <process.h>
#include <io.h>
#include <unistd.h>
#include <malloc.h>

#define chdir    _chdir
#define strdup   _strdup
#define dup      _dup
#define dup2     _dup2
#define getpid   _getpid
#define pipe     _pipe
#define snprintf _snprintf
#define fileno   _fileno
#define isatty   _isatty

//Declare a local array NAME of type TYPE and SIZE elements (where SIZE
//is not a constant value)
#define DECL_A(NAME,TYPE,SIZE)  TYPE *NAME = (TYPE *) _alloca((SIZE)*sizeof(TYPE))
#define FREE_A(NAME)            

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

inline double round(double d) {
  return (std::floor(d));
}

#include "../workarounds.h"

#endif
