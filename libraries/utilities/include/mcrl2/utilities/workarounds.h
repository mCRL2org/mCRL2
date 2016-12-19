#ifndef WORKAROUNDS_H__
#define WORKAROUNDS_H__

void work_around_qtbug_38598();

// Windows specific workarounds
#if defined(BOOST_MSVC) || defined(BOOST_INTEL_WIN) || defined(_MSC_VER)
/*
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
#endif // STDIN_FILENO

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
#  define M_PI_2 (M_PI/2)
# endif
# ifndef M_PI_4
#  define M_PI_4 (M_PI/4)
# endif
# ifndef GL_BGRA
#  define GL_BGRA 0x80E1
# endif
# ifndef GL_UNSIGNED_INT_8_8_8_8_REV
#  define GL_UNSIGNED_INT_8_8_8_8_REV 0x8367
# endif
# ifndef GL_FOG_COORD_SRC
#  define GL_FOG_COORD_SRC 0x8450
# endif
# ifndef GL_FRAGMENT_DEPTH
#  define GL_FRAGMENT_DEPTH 0x8452
# endif
*/

// CYGWIN and MINGW specific
#elif defined(__CYGWIN__) || defined(__MINGW32__)
/*
# include <cassert>
# include <cerrno>
# include <unistd.h>
# include <process.h>
# if defined(__MINGW32__)
#  define getpid _getpid
# endif
*/

// Apple specific
#elif defined(__APPLE_CPP__) || defined(__APPLE_CC__)
// Workaround for OS X with Apples patched gcc 4.0.1
//#undef nil
#endif

/*
Workaround for compilers that do not support C++11 nullptr. Especially,
GCC 4.4 and older do not yet have this keyword. File is included by passing
the -include flag on the command line.
*/
#ifndef MCRL2_HAVE_NULLPTR
#ifndef __cplusplus
#define nullptr ((void*)0)
#elif defined(__GNUC__)
#define nullptr __null
#elif defined(_WIN64)
#define nullptr 0LL
#else
#define nullptr 0L
#endif
#endif // defined(HAVE_NULLPTR)

/*
Workaround for compilers that do not support C++11 noexcept. This cannot be removed as the 
Visual studio 2013 does not yet know the keyword noexcept. 
*/
#ifdef MCRL2_HAVE_NOEXCEPT
#define NOEXCEPT noexcept
#else
#define NOEXCEPT throw()
#endif // defined(HAVE_NOEXCEPT) 

// Code used for all platforms
//#include <limits.h>

#endif // WORKAROUNDS_H__
