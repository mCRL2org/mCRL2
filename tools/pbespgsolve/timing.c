// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef _WIN32 /* assume POSIX */
#include <time.h>
#else
#include <windows.h>
#endif

double time_now()
{
#ifndef _WIN32  /* assume POSIX */

    struct timespec tp = { 0, 0 };
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return tp.tv_sec + tp.tv_nsec/1e9;

#else  /* Windows */

    FILETIME filetime;
    GetSystemTimeAsFileTime(&filetime);
    ULARGE_INTEGER largeint;
    largeint.LowPart  = filetime.dwLowDateTime;
    largeint.HighPart = filetime.dwHighDateTime;
    return largeint.QuadPart/1e-7;

#endif
}
