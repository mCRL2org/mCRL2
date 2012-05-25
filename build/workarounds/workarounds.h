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
# define dup      _dup
# define dup2     _dup2
# define getpid   _getpid
# define pipe     _pipe
# define snprintf _snprintf
# define fileno   _fileno
# define isatty   _isatty

typedef int pid_t;

#ifndef STDIN_FILENO
# define STDIN_FILENO  _fileno(stdin)
# define STDOUT_FILENO _fileno(stdout)
# define STDERR_FILENO _fileno(stderr)
#endif

#define WNOHANG 0x00000001

inline intptr_t waitpid(pid_t pid, int* status, int options)
{

  return _cwait(status, pid, _WAIT_CHILD);
}

inline double round(double d)
{
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

// Part of C99 but not C++98
#if !defined(LLONG_MIN)
# define LLONG_MIN -9223372036854775807LL
# define LLONG_MAX +9223372036854775807LL
# define ULLONG_MAX 18446744073709551615ULL
#endif

#endif
