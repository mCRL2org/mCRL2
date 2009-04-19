#include "logging.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define INFO  0
#define WARN  1
#define ERROR 2
#define FATAL 3

extern int logging_enabled;

/* Prints an error message followed by a newline character,
   prefixed by the time in seconds spend by the program
   and displaying the type of error. */
void message(int severity, const char *fmt, va_list ap)
{
    fprintf(stderr, "[%7.3f] ", time_used());
    switch (severity)
    {
    case WARN:  fprintf(stderr, "WARNING: "); break;
    case ERROR: fprintf(stderr, "ERROR: "); break;
    case FATAL: fprintf(stderr, "FATAL ERROR: "); break;
    }
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
    fflush(stderr);
}

void info(const char *fmt, ...)
{
  if (logging_enabled)
  {
    va_list ap;
    va_start(ap, fmt);
    message(INFO, fmt, ap);
    va_end(ap);
  }
}

void warn(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    message(WARN, fmt, ap);
    va_end(ap);
}

void error(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    message(ERROR, fmt, ap);
    va_end(ap);
}

void fatal(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    message(FATAL, fmt, ap);
    va_end(ap);
    abort();
}
