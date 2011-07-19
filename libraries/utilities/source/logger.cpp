// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file logger.cpp

#include <cstdio>
#include <ctime>
#include <string>
#include <cassert>
#include <iostream>
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {
  namespace log {

// Implementation of now_time; platform specific.
// used to print timestamps in front of debug messages.
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

#include <windows.h>

std::string now_time()
{
    const int MAX_LEN = 200;
    char buffer[MAX_LEN];
    if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0,
            "HH':'mm':'ss", buffer, MAX_LEN) == 0)
        return "Error in NowTime()";

    char result[100] = {0};
    static DWORD first = GetTickCount();
    std::sprintf(result, "%s.%03ld", buffer, (long)(GetTickCount() - first) % 1000);
    return result;
}

#else

#include <sys/time.h>

std::string now_time()
{
    char buffer[11];
    time_t t;

    time(&t);
    struct tm* r;
    r = localtime(&t);

    size_t res = strftime(buffer, sizeof(buffer), "%H:%M:%S", r);
    if(res == 0)
    {
      std::clog << "Could not write time to buffer" << std::endl;
      return std::string();
    }
    struct timeval tv;
    gettimeofday(&tv, 0);
    char result[100] = {0};
    std::sprintf(result, "%s.%03ld", buffer, (long)tv.tv_usec / 1000);
    return result;
}
#endif //WIN32

custom_message_handler_t& mcrl2_custom_message_handler_func()
{
  static custom_message_handler_t mcrl2_custom_message_handler = 0; //< Do not access directly
  return mcrl2_custom_message_handler;
}


  }
}
