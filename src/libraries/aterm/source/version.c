#include "version.h"

#ifdef VERSION
char *at_version = VERSION;
#else
char *at_version = "unavailable";
#endif

#ifdef CURDATE
char *at_date = CURDATE;
#else
char *at_date = "unavailable";
#endif
