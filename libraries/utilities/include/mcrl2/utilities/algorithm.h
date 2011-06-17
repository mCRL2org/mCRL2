// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/algorithm.h
/// \brief add your file description here.

#ifndef MCRL2_UTILITIES_ALGORITHM_H
#define MCRL2_UTILITIES_ALGORITHM_H

#include <iostream>
#include <string>

namespace mcrl2
{

namespace utilities
{

/// \brief Base class for algorithms.
class algorithm
{
  protected:
    size_t m_verbose_level;

    /// \brief Returns a reference to the verbosity level.
    /// The higher this number, the more output that will be written to clog.
    /// Verbosity level 0 means no output.
    size_t& verbose_level()
    {
      return m_verbose_level;
    }

    /// \brief Returns the verbosity level.
    size_t verbose_level() const
    {
      return m_verbose_level;
    }

    /// \brief Returns true if a message with the given level should be printed.
    bool check_log_level(size_t level) const
    {
      return m_verbose_level >= level;
    }

    /// \brief Very simplistic log function
    void LOG(size_t level, const std::string& s) const
    {
      if (check_log_level(level))
      {
        std::clog << s << std::flush;
      }
    }

  public:
    /// \brief Constructor
    algorithm(size_t verbose_level = 0)
      : m_verbose_level(verbose_level)
    {
    }

};

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_ALGORITHM_H
