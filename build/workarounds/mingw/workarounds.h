#ifndef _MINGW_SUBSTITUTES_H_
#define _MINGW_SUBSTITUTES_H_

/*
 * Windows MinGW Compatibility Functions -- functions to support portability to the
 * Windows environment.
 */

#include <cassert>
#include <cerrno>
#include <unistd.h>
#include <process.h>

#define getpid _getpid

#ifndef WIFEXITED
  #define WIFEXITED(S) 1
#endif

#ifndef WEXITSTATUS
  #define WEXITSTATUS(S) (S)
#endif

#include "../workarounds.h"

#endif
