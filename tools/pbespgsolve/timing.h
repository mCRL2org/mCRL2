// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef TIMING_H_INCLUDED
#define TIMING_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

void time_initialize();     /* initialize the timer */
void time_pause();          /* pause the timer */
double time_resume();       /* resume the timer; reports time waited */
double time_used();         /* number of seconds used */

#ifdef __cplusplus
}
#endif

#endif /* TIMING_H_INCLUDED */
