// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file logger_test.cpp

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/included/unit_test_framework.hpp>

#include "mcrl2/core/detail/memory_utility.h"

size_t args()
{
  return 10;
}

BOOST_AUTO_TEST_CASE(test_alloca)
{
  size_t nr_args = args();
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(array,size_t,nr_args);
  for (size_t i = 0; i < nr_args; ++i)
  {
    array[i] = 10*i;
  }

  for (size_t i = 0; i < nr_args; ++i)
  {
    BOOST_CHECK(array[i] == 10*i);
  }
}

/* The following case is disabled. It was added to show that
 * this kind of array declaration does not work on MSVC
BOOST_AUTO_TEST_CASE(test_varsize_array)
{
  size_t nr_args = args();
  size_t array[nr_args];
  for (size_t i = 0; i < nr_args; ++i)
  {
    array[i] = 10*i;
  }

  for (size_t i = 0; i < nr_args; ++i)
  {
    BOOST_CHECK(array[i] == 10*i);
  }
}
*/
