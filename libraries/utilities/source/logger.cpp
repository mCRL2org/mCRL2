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

  std::string format_time(const time_t* t)
  {
    struct tm* r;
    r = localtime(t);

    char buffer[11];
    size_t res = strftime(buffer, sizeof(buffer), "%H:%M:%S", r);
    if(res == 0)
    {
      std::clog << "Could not write time to buffer" << std::endl;
      return std::string();
    }
    return buffer;
  }

  std::string formatter::format(const log_level_t level, const std::string& hint, const time_t timestamp, const std::string& msg)
  {
    // Construct the message header
    std::stringstream start_of_line;
    start_of_line << "[" << format_time(&timestamp) << " " << hint
                  << (hint == std::string()?"":"::")
                  << log_level_to_string(level) << "]"
                  << std::string(8 - log_level_to_string(level).size(), ' ');

    // Check if message is finished. We
    bool msg_ends_with_newline = false;
    if (msg.size() > 0)
    {
      msg_ends_with_newline = (msg[msg.size()-1] == '\n');
    }

    std::stringstream result;

    // Determine whether the last line should be overwritten (by inserting a
    // carriage return).
    bool overwrite = false;
    if (last_message_was_status())
    {
      if (level == status && hint == last_hint())
      {
        if (last_message_ended_with_newline())
        {
          result << "\r" << start_of_line.str();
          overwrite = true;
        }
      }
      else
      {
        result << "\n" << start_of_line.str();
      }
    }
    else
    if (last_message_ended_with_newline())
    {
      result << start_of_line.str();
    }

    // Copy the message to result, keeping track of the caret position and
    // inserting message headers at newlines. A trailing newline is ignored.
    for (std::string::const_iterator it = msg.begin(); it != msg.end(); )
    {
      if (*it != '\n')
      {
        result << *it++;
        ++caret_pos();
      }
      else
      {
        if (++it != msg.end())
        {
          result << "\n" << start_of_line.str();
          caret_pos() = 0;
          last_caret_pos() = 0;
        }
      }
    }

    // Pad message with spaces when overwriting a previous line
    if (msg_ends_with_newline && overwrite && caret_pos() < last_caret_pos())
    {
      for (size_t i = 0; i < last_caret_pos() - caret_pos(); ++i)
      {
        result << ' ';
      }
    }

    // If this is a finished status message, remember the caret position
    // so the next line can overwrite properly. If this is a finished message
    // of another type, restore the trailing newline.
    if(msg_ends_with_newline)
    {
      if (level == status)
      {
        last_caret_pos() = caret_pos();
      }
      else
      {
        result << "\n";
      }
      caret_pos() = 0;
    }

    // Store information about this message so next time we can determine
    // whether we should overwrite the last line or not.
    last_message_ended_with_newline() = msg_ends_with_newline;
    if (level == status)
    {
      last_message_was_status() = true;
      last_hint() = hint;
    }
    else
    {
      last_message_was_status() = false;
    }

    return result.str();
  }

#if 0
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
#endif

  }
}
