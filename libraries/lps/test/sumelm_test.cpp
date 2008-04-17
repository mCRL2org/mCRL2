// Author(s): Jeroen Keiren
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sumelm_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>
#include <mcrl2/lps/specification.h>
#include <mcrl2/lps/sumelm.h>
#include <mcrl2/lps/mcrl22lps.h>

using namespace atermpp;
using namespace mcrl2::data;
using namespace mcrl2::lps;

///Test case which tries to test all possibilities for substitutions. This is a
///test for issue #367
void test_case_1()
{
  const std::string text(
    "sort S = struct s1 | s2;\n"
    "map f : S -> Bool;\n"
    "act a : S # Bool;\n"
    "proc P = sum c : S, b : Bool . (b == f(c) && c == s2) -> a(c, b) . P;\n"
    "init P;\n"
  );

  specification s0 = mcrl22lps(text);
  specification s1 = sumelm(s0);
  summand_list summands1 = s1.process().summands();
  for(summand_list::iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    BOOST_CHECK(i->summation_variables().empty());
    BOOST_CHECK(find_all_data_variables(i->condition()).empty());
    BOOST_CHECK(find_all_data_variables(i->actions()).empty());
  }
}

int test_main(int ac, char** av)
{
  MCRL2_ATERM_INIT(ac, av)

  test_case_1();

  return 0;
}

