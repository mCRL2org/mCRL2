#ifndef _MINGW_SUBSTITUTES_H_
#define _MINGW_SUBSTITUTES_H_

/*
 * Windows MinGW Compatibility Functions -- functions to support portability to the
 * Windows environment.
 */

#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <windows.h>

inline pid_t waitpid(pid_t pid, int* status, int options) {

  return _cwait (status, pid, _WAIT_CHILD);
}

inline int kill(int pid, int signal) {
  return (TerminateProcess(reinterpret_cast < void* > (pid), signal));
}

#define SIGKILL 9

#ifndef WIFEXITED
  #define WIFEXITED(S) 1
#endif

#ifndef WEXITSTATUS
  #define WEXITSTATUS(S) (S)
#endif

#endif
