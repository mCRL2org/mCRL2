// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/algorithm.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_ALGORITHM_H
#define MCRL2_CORE_ALGORITHM_H

namespace mcrl2 {

  namespace core {

    /// \brief Base class for algorithms.
    class algorithm
    {
    protected:
      unsigned int m_verbose_level;

    public:

      /// \brief Constructor
      algorithm(unsigned int verbose_level = 0)
      : m_verbose_level(verbose_level)
      {
      }

      /// \brief Returns a reference to the verbosity level.
      /// The higher this number, the more output that will be written to clog.
      /// Verbosity level 0 means no output.
      unsigned int& verbose_level()
      {
        return m_verbose_level;
      }

      /// \brief Returns the verbosity level.
      unsigned int verbose_level() const
      {
        return m_verbose_level;
      }
      
      /// \brief Returns true if a message with the given level should be printed.
      bool check_log_level(unsigned int level) const
      {
        return m_verbose_level >= level;
      }
    };

  } // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_ALGORITHM_H
