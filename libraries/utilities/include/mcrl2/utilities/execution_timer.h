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

    class execution_timer
    {
      protected:

        struct timing
        {
          clock_t start;
          clock_t finish;

          timing() :
            start(0),
            finish(0)
          {}
        };

        std::string m_tool_name;
        std::string m_timing_name;
        std::string m_filename;
        std::map<std::string, timing> m_timings;

      public:
        execution_timer()
        {}

        execution_timer(std::string const& tool_name, std::string const& filename) :
          m_tool_name(tool_name),
          m_filename(filename)
        {}

        ~execution_timer()
        {}

        void start(std::string const& timing_name)
        {
          if(m_timings.find(timing_name) != m_timings.end())
          {
            throw mcrl2::runtime_error("Starting already known timing; causes unreliable results");
          }
          timing t;
          t.start = clock();
          m_timings[timing_name] = t;
        }

        void finish(std::string const& timing_name)
        {
          if(m_timings.find(timing_name) == m_timings.end())
          {
            throw mcrl2::runtime_error("Finishing timing that was not started");
          }
          m_timings[timing_name].finish = clock();
        }

        void report()
        {
          std::stringstream output;
          output.setf(std::ios::fixed, std::ios::floatfield); // Print floats in 2 decimals
          output.precision(2);

          output << "- tool: " << m_tool_name << std::endl
                 << "  timing:" << std::endl;

          for(std::map<std::string, timing>::const_iterator i = m_timings.begin(); i != m_timings.end(); ++i)
          {
            output << "    " << i->first << ": "
                   << (static_cast<float>(i->second.finish - i->second.start))/CLOCKS_PER_SEC
                   << std::endl;
          }

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
        }

    };

  } // namespace utilities
  
} // namespace mcrl2

#endif //MCRL2_UTILITIES_TIMER_H
