// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/timer.h
/// \brief Class to obtain running times of code.

#ifndef MCRL2_UTILITIES_TIMER_H
#define MCRL2_UTILITIES_TIMER_H

#include <ctime>
#include <fstream>
#include <sstream>
#include <string>
#include "mcrl2/exception.h"

namespace mcrl2 {
  
  namespace utilities {

    class timer
    {
      protected:
        std::string m_tool_name;
        std::string m_timing_name;
        std::string m_filename;
        clock_t m_start;
        clock_t m_finish;
        bool m_has_reported;

      public:
        timer(std::string const& tool_name, std::string const& filename) :
          m_tool_name(tool_name),
          m_timing_name(""),
          m_filename(filename),
          m_start(0),
          m_finish(0),
          m_has_reported(false)
        {}

        ~timer()
        {}

        void start(std::string const& timing_name)
        {
          m_timing_name = timing_name;
          m_start = clock();
          m_finish = 0;
        }

        void finish()
        {
          m_finish = clock();
        }

        void report()
        {
          if (m_start == (clock_t)-1 || m_finish == (clock_t)-1)
          {
            throw mcrl2::runtime_error("Cannot report running times.");
          }

          std::stringstream output;
          if(!m_has_reported)
          {
            output << "- tool: " << m_tool_name << std::endl
                   << "  timing:" << std::endl;
          }

          output << "    " << m_timing_name << ": "
                 << (static_cast<float>(m_finish - m_start))/CLOCKS_PER_SEC
                 << std::endl;

          if(m_filename.empty())
          {
            std::cerr << output.str();
          }
          else
          {
            std::ofstream out;
            out.open(m_filename.c_str(), std::ios::app);
            out << output.str();
            out.close();
          }

          m_has_reported = true;
        }

    };

  } // namespace utilities
  
} // namespace mcrl2

#endif //MCRL2_UTILITIES_TIMER_H
