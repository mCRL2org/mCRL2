#ifndef UTIL_H
#define UTIL_H

#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif/* __cplusplus */

#ifndef streq
#  define streq(s,t)	(!(strcmp(s,t)))
#endif

#ifndef MIN
#  define MIN(a,b)	((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#  define MAX(a,b)	((a) > (b) ? (a) : (b))
#endif

#define IDX_TOTAL               0
#define IDX_MIN                 1
#define IDX_MAX                 2

#ifdef WITH_STATS
#define STATS(array, value)  \
  array[IDX_TOTAL] += value; \
  if(value < array[IDX_MIN]) \
    array[IDX_MIN] = value;  \
  if(value > array[IDX_MAX]) \
    array[IDX_MAX] = value
#else
#define STATS(array, value)
#endif

#define MYMAXINT 0x7FFFFFFF

#ifdef __cplusplus
}
#endif/* __cplusplus */ 

#endif
