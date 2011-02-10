#ifndef UTIL_H
#define UTIL_H

#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif/* __cplusplus */

#ifndef streq
#  define streq(s,t)  (!(strcmp(s,t)))
#endif

#ifndef MIN
#  define MIN(a,b)  ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#  define MAX(a,b)  ((a) > (b) ? (a) : (b))
#endif

#ifdef __cplusplus
}
#endif/* __cplusplus */

#endif
