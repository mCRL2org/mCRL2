// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef LOGGING_H_INCLUDED
#define LOGGING_H_INCLUDED

#include "timing.h"

#ifdef __cplusplus
extern "C" {
#endif

void info(const char *fmt, ...);
void warn(const char *fmt, ...);
void error(const char *fmt, ...);
#if __GNUC__ >= 3
__attribute__((noreturn))
#endif
void fatal(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* ndef LOGGING_H_INCLUDED */
