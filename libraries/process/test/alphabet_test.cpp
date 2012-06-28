// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file alphabet_test.cpp
/// \brief Test for alphabet reduction.

#include <algorithm>
#include <iterator>
#include <set>
#include <sstream>
#include <vector>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/process/alphabet.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/utilities/text_utility.h"

using namespace mcrl2;
using namespace mcrl2::process;

void test_action_parse()
{
  lps::action_label_list act_decl = lps::parse_action_declaration("a: Nat;");
  lps::action a = lps::parse_action("a(2)", act_decl);
  multi_action_name A;
  A.insert(core::identifier_string("a"));

  BOOST_CHECK(name(a) == A);
}

multi_action_name_set parse_multi_action_name_set(const std::string& text, const lps::action_label_list& action_decls)
{
  multi_action_name_set result;
  std::string s = text.substr(1, text.size() - 2);
  std::vector<std::string> v = utilities::split(s, ",");
  for (std::vector<std::string>::iterator i = v.begin(); i != v.end(); ++i)
  {
    result.insert(name(parse_multi_action(*i, action_decls)));
  }
  return result;
}

void test_alphabet_allow()
{
  std::string procspec =
    "act a, b, c, d; \n"
    "init delta;  \n"
    ;

  process_specification P = parse_process_specification(procspec);
  lps::action_label_list action_decls = lps::parse_action_declaration("a, b, c, d;");
  process_expression p = parse_process_expression("a || b . c", procspec);
  BOOST_CHECK(process::pp(p) == "a || b . c");
  multi_action_name_set A = parse_multi_action_name_set("{a, b, d, a|d, a|c}", action_decls);
  std::cout << "A = " << lps::pp(A) << std::endl;
  alphabet_result r = push_allow(p, A, false, P);
  const multi_action_name_set& B = r.second;
  std::cout << "B = " << lps::pp(B) << std::endl;
  multi_action_name_set C = parse_multi_action_name_set("{a, b, a | c}", action_decls);
  std::cout << "C = " << lps::pp(C) << std::endl;
  BOOST_CHECK(lps::pp(B) == lps::pp(C));
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv);

  log::mcrl2_logger::set_reporting_level(log::debug);

  test_action_parse();
  test_alphabet_allow();

  return EXIT_SUCCESS;
}
