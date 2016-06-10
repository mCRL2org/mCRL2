// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file aterm_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <sstream>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_string.h"

using namespace std;
using namespace atermpp;

void test_aterm()
{
  atermpp::aterm a = read_term_from_string("f(x)");
  atermpp::aterm label = read_term_from_string("label");

  atermpp::aterm d = aterm_int(10);
  BOOST_CHECK(aterm_int(d).value() == 10);

  atermpp::aterm e = atermpp::aterm();
  BOOST_CHECK(e==atermpp::aterm());
}

void test_aterm_string()
{
  const aterm_string& empty = empty_string();
  std::ostringstream out;
  out << empty;
  BOOST_CHECK(out.str() == "");
}

void test_aterm_io(const std::string& input_string)
{
  const aterm input=read_term_from_string(input_string);
  std::ostringstream out;
  write_term_to_text_stream(input, out);
  std::istringstream in(out.str());
  const aterm text_output = read_term_from_text_stream(in);
  if (input!=text_output)
  {
    std::cerr << "Expected the following terms to be the same (aterm textual save and load):\n";
    std::cerr << "Input " << input << "\n";
    std::cerr << "Output " << text_output << "\n";
    std::cerr << "----------------------------------------------------------------- (textual save/load)\n";
  }
  BOOST_CHECK(input == text_output);

  std::stringbuf buf;
  std::iostream binary_stream(&buf);
  write_term_to_binary_stream(input, binary_stream);
  const aterm binary_output = read_term_from_binary_stream(binary_stream);
  if (input!=binary_output)
  {
    std::cerr << "Expected the following terms to be the same (aterm binary save and load):\n";
    std::cerr << "Input " << input << "\n";
    std::cerr << "Output " << binary_output << "\n";
    std::cerr << "----------------------------------------------------------------- (binary save/load)\n";
  }
  BOOST_CHECK(input == binary_output);
}

void test_aterm_io()
{
  test_aterm_io("17");
  test_aterm_io("a_somewhat_longer_constant_name_with_some_blah_blah_at_the_end_to_make_it_longer");
  test_aterm_io("f(g,h)");
  test_aterm_io("[]");
  test_aterm_io("[a]");
  test_aterm_io("[a,b,[]]");
  test_aterm_io("f([a,f(x),[]],2,[g,g(34566)])"); 
}

int test_main(int argc, char* argv[])
{
  test_aterm();
  test_aterm_string(); 
  test_aterm_io();

  return 0;
}
