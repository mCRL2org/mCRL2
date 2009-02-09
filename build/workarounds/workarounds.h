#ifndef WORKAROUNDS_H__
#define WORKAROUNDS_H__

#include <boost/config.hpp>

#if defined(BOOST_MSVC) || defined(BOOST_INTEL_WIN)
# include <cassert>
# include <cmath>
# include <cstdio>
# include <cerrno>
# include <process.h>
# include <io.h>
# include <unistd.h>
# include <malloc.h>

# define chdir    _chdir
# define strdup   _strdup
# define dup      _dup
# define dup2     _dup2
# define getpid   _getpid
# define pipe     _pipe
# define snprintf _snprintf
# define fileno   _fileno
# define isatty   _isatty

//Declare a local array NAME of type TYPE and SIZE elements (where SIZE
//is not a constant value)
# define DECL_A(NAME,TYPE,SIZE)  TYPE *NAME = (TYPE *) _alloca((SIZE)*sizeof(TYPE))
# define FREE_A(NAME)

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

# ifndef GL_CLAMP_TO_EDGE
#  define GL_CLAMP_TO_EDGE 0x812F
# endif
# ifndef M_PI
#  define M_PI 3.141592653589793238462643383280
# endif
# ifndef M_PI_2
#  define M_PI_2 2*M_PI
# endif
# ifndef M_PI_4
#  define M_PI_4 4*M_PI
# endif
#elif defined(__CYGWIN__) || defined(__MINGW32__)
# include <cassert>
# include <cerrno>
# include <unistd.h>
# include <process.h>
# if defined(__MINGW32__)
#  define getpid _getpid
# endif
#endif

#include <limits.h>

//String manipulation
//-------------------
//
//Re-implementation of strdup (because it is not part of the C99 standard)
#if !(defined __sun__ || defined __USE_SVID || defined __USE_BSD || defined __USE_XOPEN_EXTENDED || defined __APPLE__ || defined _MSC_VER || defined __MINGW32__ || defined __CYGWIN__ || defined __FreeBSD__)
inline char *strdup(const char *s) {
    char *p;

    if((p = (char *)malloc(strlen(s) + 1)) == NULL)
      return NULL;
    return strcpy(p, s);
}
#endif

// Part of C99 but not C++98
#if !defined(LLONG_MIN)
# define LLONG_MIN -9223372036854775807LL
# define LLONG_MAX +9223372036854775807LL
# define ULLONG_MAX 18446744073709551615ULL
#endif

//Declare a local array NAME of type TYPE and SIZE elements (where SIZE
//is not a constant value)
#ifndef DECL_A
#define DECL_A(NAME,TYPE,SIZE)  TYPE NAME[SIZE]
#define FREE_A(NAME)
#endif

//Make sure __func__ works (as well as possible)
#ifndef __func__
#ifdef __FUNCTION__
#define __func__ __FUNCTION__
#else
#define __func__ __FILE__
#endif
#endif

#endif
