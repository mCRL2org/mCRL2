// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bes_io_test.cpp
/// \brief Some io tests for boolean equation systems.

#include <sstream>
#include <string>
#include <boost/test/minimal.hpp>
#include "mcrl2/bes/pg_parse.h"

using namespace mcrl2;
using namespace mcrl2::bes;

void test_parse_pg(std::string const& in)
{
  std::stringstream from(in);
  boolean_equation_system<> bes;
  parse_pgsolver(from,bes);
}

void test_case()
{
  std::string game =
    "0 2 0 1 \"undef\";\n"
    "1 2 1 2, 3 \"&\";\n"
    "2 2 1 4, 5 \"&\";\n"
    "4 2 1 5, 7, 8 \"&\";\n"
    "7 2 1 5, 10 \"&\";\n"
    "8 2 1 5, 11 \"&\";\n"
    "10 2 1 5, 13 \"&\";\n"
    "11 2 1 5, 14 \"&\";\n"
    "13 2 1 5, 16 \"&\";\n"
    "14 2 1 5, 17 \"&\";\n"
    "16 2 1 5, 20, 21 \"&\";\n"
    "17 2 1 2, 5 \"&\";\n"
    "20 2 1 1, 5 \"&\";\n"
    "21 2 1 5, 24 \"&\";\n"
    "24 2 1 5, 7 \"&\";\n"
    "3 1 0 6 \"undef\";\n"
    "5 1 0 31 \"undef\";\n"
    "6 1 1 5, 9 \"&\";\n"
    "9 1 0 12 \"undef\";\n"
    "12 1 1 5, 15 \"&\";\n"
    "15 1 1 18, 19 \"&\";\n"
    "18 1 1 5, 22 \"&\";\n"
    "19 1 1 5, 23 \"&\";\n"
    "22 1 0 25 \"undef\";\n"
    "23 1 0 26 \"undef\";\n"
    "25 1 0 5 \"undef\";\n"
    "26 1 1 5, 27 \"&\";\n"
    "27 1 0 28 \"undef\";\n"
    "28 1 1 5, 29 \"&\";\n"
    "29 1 0 30 \"undef\";\n"
    "30 1 1 3, 5 \"&\";\n"
    "31 0 0 31 \"undef\";\n";

  test_parse_pg(game);
}

void test_case_parity()
{
  std::string game =
    "parity 31;\n"
    "0 2 0 1 \"undef\";\n"
    "1 2 1 2, 3 \"&\";\n"
    "2 2 1 4, 5 \"&\";\n"
    "4 2 1 5, 7, 8 \"&\";\n"
    "7 2 1 5, 10 \"&\";\n"
    "8 2 1 5, 11 \"&\";\n"
    "10 2 1 5, 13 \"&\";\n"
    "11 2 1 5, 14 \"&\";\n"
    "13 2 1 5, 16 \"&\";\n"
    "14 2 1 5, 17 \"&\";\n"
    "16 2 1 5, 20, 21 \"&\";\n"
    "17 2 1 2, 5 \"&\";\n"
    "20 2 1 1, 5 \"&\";\n"
    "21 2 1 5, 24 \"&\";\n"
    "24 2 1 5, 7 \"&\";\n"
    "3 1 0 6 \"undef\";\n"
    "5 1 0 31 \"undef\";\n"
    "6 1 1 5, 9 \"&\";\n"
    "9 1 0 12 \"undef\";\n"
    "12 1 1 5, 15 \"&\";\n"
    "15 1 1 18, 19 \"&\";\n"
    "18 1 1 5, 22 \"&\";\n"
    "19 1 1 5, 23 \"&\";\n"
    "22 1 0 25 \"undef\";\n"
    "23 1 0 26 \"undef\";\n"
    "25 1 0 5 \"undef\";\n"
    "26 1 1 5, 27 \"&\";\n"
    "27 1 0 28 \"undef\";\n"
    "28 1 1 5, 29 \"&\";\n"
    "29 1 0 30 \"undef\";\n"
    "30 1 1 3, 5 \"&\";\n"
    "31 0 0 31 \"undef\";\n";

  test_parse_pg(game);
}

void test_case_start()
{
  std::string game =
    "start 0;\n"
    "0 2 0 1 \"undef\";\n"
    "1 2 1 2, 3 \"&\";\n"
    "2 2 1 4, 5 \"&\";\n"
    "4 2 1 5, 7, 8 \"&\";\n"
    "7 2 1 5, 10 \"&\";\n"
    "8 2 1 5, 11 \"&\";\n"
    "10 2 1 5, 13 \"&\";\n"
    "11 2 1 5, 14 \"&\";\n"
    "13 2 1 5, 16 \"&\";\n"
    "14 2 1 5, 17 \"&\";\n"
    "16 2 1 5, 20, 21 \"&\";\n"
    "17 2 1 2, 5 \"&\";\n"
    "20 2 1 1, 5 \"&\";\n"
    "21 2 1 5, 24 \"&\";\n"
    "24 2 1 5, 7 \"&\";\n"
    "3 1 0 6 \"undef\";\n"
    "5 1 0 31 \"undef\";\n"
    "6 1 1 5, 9 \"&\";\n"
    "9 1 0 12 \"undef\";\n"
    "12 1 1 5, 15 \"&\";\n"
    "15 1 1 18, 19 \"&\";\n"
    "18 1 1 5, 22 \"&\";\n"
    "19 1 1 5, 23 \"&\";\n"
    "22 1 0 25 \"undef\";\n"
    "23 1 0 26 \"undef\";\n"
    "25 1 0 5 \"undef\";\n"
    "26 1 1 5, 27 \"&\";\n"
    "27 1 0 28 \"undef\";\n"
    "28 1 1 5, 29 \"&\";\n"
    "29 1 0 30 \"undef\";\n"
    "30 1 1 3, 5 \"&\";\n"
    "31 0 0 31 \"undef\";\n";

  test_parse_pg(game);
}

void test_case_parity_and_start()
{
  std::string game =
    "parity 31;\n"
    "start 0;\n"
    "0 2 0 1 \"undef\";\n"
    "1 2 1 2, 3 \"&\";\n"
    "2 2 1 4, 5 \"&\";\n"
    "4 2 1 5, 7, 8 \"&\";\n"
    "7 2 1 5, 10 \"&\";\n"
    "8 2 1 5, 11 \"&\";\n"
    "10 2 1 5, 13 \"&\";\n"
    "11 2 1 5, 14 \"&\";\n"
    "13 2 1 5, 16 \"&\";\n"
    "14 2 1 5, 17 \"&\";\n"
    "16 2 1 5, 20, 21 \"&\";\n"
    "17 2 1 2, 5 \"&\";\n"
    "20 2 1 1, 5 \"&\";\n"
    "21 2 1 5, 24 \"&\";\n"
    "24 2 1 5, 7 \"&\";\n"
    "3 1 0 6 \"undef\";\n"
    "5 1 0 31 \"undef\";\n"
    "6 1 1 5, 9 \"&\";\n"
    "9 1 0 12 \"undef\";\n"
    "12 1 1 5, 15 \"&\";\n"
    "15 1 1 18, 19 \"&\";\n"
    "18 1 1 5, 22 \"&\";\n"
    "19 1 1 5, 23 \"&\";\n"
    "22 1 0 25 \"undef\";\n"
    "23 1 0 26 \"undef\";\n"
    "25 1 0 5 \"undef\";\n"
    "26 1 1 5, 27 \"&\";\n"
    "27 1 0 28 \"undef\";\n"
    "28 1 1 5, 29 \"&\";\n"
    "29 1 0 30 \"undef\";\n"
    "30 1 1 3, 5 \"&\";\n"
    "31 0 0 31 \"undef\";\n";

  test_parse_pg(game);
}

int test_main(int argc, char* argv[])
{
  test_case();
  test_case_parity();
  test_case_start();
  test_case_parity_and_start();
  return 0;
}
