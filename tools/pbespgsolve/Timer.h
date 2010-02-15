// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

extern "C" double time_now();

class Timer
{
public:
    Timer() : last_(time_now()) { };
    double elapsed() { return time_now() - last_; }

private:
    double last_;
};

#endif /* nde fTIMER_H_INCLUDED */
