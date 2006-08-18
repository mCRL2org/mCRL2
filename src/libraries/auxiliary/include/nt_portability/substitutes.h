#ifndef _NT_SUBSTITUTES_H_
#define _NT_SUBSTITUTES_H_

/*
 * Windows Compatibility Functions -- functions to support portability to the
 * Windows environment.
 */

# define snprintf _snprintf
# define getpid   _getpid

typedef int pid_t;

#endif
