#ifndef LOGGING_H_INCLUDED
#define LOGGING_H_INCLUDED

#include "timing.h"

#ifdef __cplusplus
extern "C" {
#endif

void info(const char *fmt, ...);
void warn(const char *fmt, ...);
void error(const char *fmt, ...);
__attribute__((noreturn)) void fatal(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* ndef LOGGING_H_INCLUDED */
