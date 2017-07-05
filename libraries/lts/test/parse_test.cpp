// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parse_test.cpp
/// \brief Add your file description here.

#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdio>

#include <boost/test/included/unit_test_framework.hpp>

#include "mcrl2/lts/parse.h"

using namespace mcrl2;

void test_fsm_parser(const std::string& text)
{
  // parse the text
  lts::probabilistic_lts_fsm_t fsm;
  lts::parse_fsm_specification(text, fsm);

  // write the fsm to a file
  std::string temp_filename = "parse_test.tmp";
  fsm.save(temp_filename);

  // read the fsm from the file
  std::string text1 = utilities::read_text(temp_filename);

  // remove the file
  std::remove(temp_filename.c_str());

  if (text != text1)
  {
    std::cerr << "--- Error: difference detected ---\n" << text << "\n-------------------\n" << text1 << "\n-------------------\n";
  }

  // the text should be the same
  BOOST_CHECK(text == text1);
}

BOOST_AUTO_TEST_CASE(fsm_parser_test)
{
  test_fsm_parser(
    "b(2) Bool  \"F\" \"T\"\n"
    "n(2) Nat  \"1\" \"2\"\n"
    "---\n"
    "0 0\n"
    "0 1\n"
    "1 0\n"
    "1 1\n"
    "---\n"
    "1 2 \"increase\"\n"
    "1 3 \"on\"\n"
    "2 4 \"on\"\n"
    "2 1 \"decrease\"\n"
    "3 1 \"off\"\n"
    "3 4 \"increase\"\n"
    "4 2 \"off\"\n"
    "4 3 \"decrease\"\n"
  );

  test_fsm_parser(
    "b(2) Bool # Bool -> Nat  \"F\" \"T\"\n"
    "n(2) Nat -> Nat  \"1\" \"2\"\n"
    "---\n"
    "0 0\n"
    "0 1\n"
    "1 0\n"
    "1 1\n"
    "---\n"
    "1 2 \"increase\"\n"
    "1 3 \"on\"\n"
    "2 4 \"on\"\n"
    "2 1 \"decrease\"\n"
    "3 1 \"off\"\n"
    "3 4 \"increase\"\n"
    "4 2 \"off\"\n"
    "4 3 \"decrease\"\n"
  );

  test_fsm_parser(
    "---\n"
    "---\n"
    "1 1 \"tau\"\n"
  );
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return nullptr;
}
