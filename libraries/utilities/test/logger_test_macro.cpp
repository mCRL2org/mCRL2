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

// Override MCRL2_MAX_LOG_LEVEL
// Must come before the first include of logger.h
#define MCRL2_MAX_LOG_LEVEL debug3

#include "mcrl2/utilities/logger.h"

using namespace mcrl2::log;

void print_all_log_levels()
{
  mCRL2log(error) << "An error message" << std::endl;
  mCRL2log(warning) << "A warning" << std::endl;
  mCRL2log(info) << "Some information" << std::endl;
  mCRL2log(verbose) << "Detailed information" << std::endl;
  mCRL2log(debug) << "Debugging info" << std::endl;
  mCRL2log(debug1) << "Detailed debugging info" << std::endl;
  mCRL2log(debug2) << "Detailed debugging info (2)" << std::endl;
  mCRL2log(debug3) << "Detailed debugging info (3)" << std::endl;
  mCRL2log(debug4) << "Detailed debugging info (4)" << std::endl;
  mCRL2log(debug5) << "Detailed debugging info (5)" << std::endl;
  // The last two won't be printed because of max log level.
}

BOOST_AUTO_TEST_CASE(test_logging_basic)
{
  // Print of debug3 level will not occur
  mcrl2_logger::set_reporting_level(debug2);
  print_all_log_levels();
}

BOOST_AUTO_TEST_CASE(test_logging_use_case)
{
  mCRL2log(info) << "Entering logging use case" << std::endl;
  const int count = 3;
  mCRL2log(debug) << "A loop with " << count << " iterations" << std::endl;
  for (int i = 0; i < count; ++i)
  {
    mCRL2log(debug1) << "the counter i = " << i << std::endl;
    if(i >= 2)
    {
      mCRL2log(debug2) << "the counter is greater then or equal to 2" << std::endl;
    }
  }
}
