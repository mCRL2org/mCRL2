// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/progress_meter.h
/// \brief add your file description here.

#ifndef MCRL2_UTILITIES_PROGRESS_METER_H
#define MCRL2_UTILITIES_PROGRESS_METER_H

#include <cstddef>
#include <iostream>
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace utilities {

/// \brief Displays progress messages for a task that performs a fixed number of steps.
class progress_meter
{
  protected:
    std::size_t n;
    std::size_t N;

  public:
    /// \brief Constructor
    progress_meter(std::size_t N_ = 0)
      : n(0), N(N_)
    {}

    /// \brief Set the number of steps of the task
    void set_size(std::size_t N_)
    {
      N = N_;
    }

    /// \brief Should be called after every step. Regularly a message is printed to log::status.
    void step()
    {
      n++;
      if (N < 1000 || (n % (N / 1000) == 0) || n == N)
      {
        std::size_t percentage = 1000 * n / N;
        mCRL2log(log::status) << (percentage / 10) << "." << (percentage % 10) << " percent completed" << std::endl;
      }
    }
};

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_PROGRESS_METER_H
