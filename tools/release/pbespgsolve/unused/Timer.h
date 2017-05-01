// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

/*! Returns the current time in seconds since the Unix epoch as a floating
    point number.  Although the unit is seconds, the actual resolution is much
    higher (though platform-dependent). */
double time_now();

//! A simple timer class to keep track of elapsed wall clock time.
class Timer
{
public:
    Timer() : last_(time_now()) { };

    //! Returns how many seconds have elapsed since the timer was constructed.
    double elapsed() { return time_now() - last_; }

private:
    //! Tracks the timestamp
    double last_;
};

#endif /* ndef TIMER_H_INCLUDED */
