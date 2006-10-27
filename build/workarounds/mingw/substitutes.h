#ifndef _MINGW_SUBSTITUTES_H_
#define _MINGW_SUBSTITUTES_H_

/*
 * Windows MinGW Compatibility Functions -- functions to support portability to the
 * Windows environment.
 */

#include <assert.h>
#include <errno.h>
#include <unistd.h>

#define SIGKILL 9

#ifndef WIFEXITED
  #define WIFEXITED(S) 1
#endif

#ifndef WEXITSTATUS
  #define WEXITSTATUS(S) (S)
#endif

#endif
