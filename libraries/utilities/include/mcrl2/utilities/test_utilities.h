// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/test_utilities.h
/// \brief Utility functions for unit testing

#ifndef MCRL2_UTILITIES_TEST_UTILITIES_H
#define MCRL2_UTILITIES_TEST_UTILITIES_H

#include "mcrl2/core/garbage_collection.h"

namespace mcrl2
{
namespace utilities
{
/// \brief Garbage collect after each case.
/// Use with BOOST_GLOBAL_FIXTURE(collect_after_test_case)
struct collect_after_test_case
{
  ~collect_after_test_case()
  {
    core::garbage_collect();
  }
};
}
}

#endif //MCRL2_UTILITIES_TEST_UTILITIES_H
