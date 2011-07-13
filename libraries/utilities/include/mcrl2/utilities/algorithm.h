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

#include "mcrl2/utilities/logger.h"

namespace mcrl2
{

namespace utilities
{

/// \brief Base class for algorithms.
class algorithm
{
  protected:
    /// \brief Logs the string s
    void LOG_VERBOSE(const std::string& s) const
    {
      mCRL2log(verbose) << s;
    }

    /// \brief Logs the string s
    void LOG_DEBUG(const std::string& s) const
    {
      mCRL2log(debug) << s;
    }

    /// \brief Logs the string s
    void LOG_DEBUG1(const std::string& s) const
    {
      mCRL2log(debug1) << s;
    }
};

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_ALGORITHM_H
