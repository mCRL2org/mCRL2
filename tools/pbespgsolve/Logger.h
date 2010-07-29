// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#ifdef USE_TIMER
#include "Timer.h"
#endif
#include <stdarg.h>
#include <stdlib.h>

class Logger
{
public:
    //! Severity of reported messages.
    enum Severity {
        LOG_DEBUG = 0, LOG_INFO = 1, LOG_WARN = 2,
        LOG_ERROR = 3, LOG_FATAL = 4, LOG_NONE = 5
    };

    //! Return the minimum severity of displayed messages
    static Severity severity() { return severity_; }

    //! Set the minimum severity of displayed messages
    static void severity(Severity s) { severity_ = s; }

    //! Return whether messages with the given severity are displayed
    static bool enabled(Severity s) { return s >= severity_; }

    inline static void debug(const char *fmt, ...);
    inline static void info(const char *fmt, ...);
    inline static void warn(const char *fmt, ...);
    inline static void error(const char *fmt, ...);
#if __GNUC__ >= 3
    __attribute__((noreturn))
#endif
    inline static void fatal(const char *fmt, ...);

private:
    /*! Prints a formatted message with the given severity to stderr,
        regardless of the current minimum severity level. */
    static void print_message(Severity severity, const char *fmt, va_list ap);

private:
#ifdef USE_TIMER
    static Timer timer_;        //! global timer shown in messages
#endif
    static Severity severity_;  //! minimum severity for displayed messages
};


void Logger::debug(const char *fmt, ...)
{
    if (enabled(LOG_DEBUG))
    {
        va_list ap;
        va_start(ap, fmt);
        print_message(LOG_DEBUG, fmt, ap);
        va_end(ap);
    }
}

void Logger::info(const char *fmt, ...)
{
    if (enabled(LOG_INFO))
    {
        va_list ap;
        va_start(ap, fmt);
        print_message(LOG_INFO, fmt, ap);
        va_end(ap);
    }
}

void Logger::warn(const char *fmt, ...)
{
    if (enabled(LOG_WARN))
    {
        va_list ap;
        va_start(ap, fmt);
        print_message(LOG_WARN, fmt, ap);
        va_end(ap);
    }
}

void Logger::error(const char *fmt, ...)
{
    if (enabled(LOG_ERROR))
    {
        va_list ap;
        va_start(ap, fmt);
        print_message(LOG_ERROR, fmt, ap);
        va_end(ap);
    }
}

void Logger::fatal(const char *fmt, ...)
{
    if (enabled(LOG_FATAL))
    {
        va_list ap;
        va_start(ap, fmt);
        print_message(LOG_FATAL, fmt, ap);
        va_end(ap);
    }
    abort();
}

#endif /* ndef LOGGER_H_INCLUDED */
