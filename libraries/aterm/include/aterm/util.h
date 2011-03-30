#ifndef UTIL_H
#define UTIL_H

#include <string.h>

namespace aterm
{

#ifndef streq
#  define streq(s,t)  (!(strcmp(s,t)))
#endif

#ifndef MIN
#  define MIN(a,b)  ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#  define MAX(a,b)  ((a) > (b) ? (a) : (b))
#endif

} // namespace aterm

#endif
