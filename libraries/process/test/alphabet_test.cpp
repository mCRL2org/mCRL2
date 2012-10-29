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

// void test_alphabet_allow()
// {
//   std::string procspec =
//     "act a, b, c, d; \n"
//     "init delta;  \n"
//     ;
//
//   process_specification P = parse_process_specification(procspec);
//   lps::action_label_list action_decls = lps::parse_action_declaration("a, b, c, d;");
//   process_expression p = parse_process_expression("a || b . c", procspec);
//   BOOST_CHECK(process::pp(p) == "a || b . c");
//   multi_action_name_set A = parse_multi_action_name_set("{a, b, d, a|d, a|c}", action_decls);
//   std::cout << "A = " << lps::pp(A) << std::endl;
//   alphabet_result r = push_allow(p, A, false, P);
//   const multi_action_name_set& B = r.second;
//   std::cout << "B = " << lps::pp(B) << std::endl;
//   multi_action_name_set C = parse_multi_action_name_set("{a, b, a | c}", action_decls);
//   std::cout << "C = " << lps::pp(C) << std::endl;
//   BOOST_CHECK(lps::pp(B) == lps::pp(C));
// }

void test_alphabet_reduce()
{
  std::string text =
    "act a;        \n"
    "proc P = a.P; \n"
    "init P;       \n"
    ;
  process_specification procspec = parse_process_specification(text);
  alphabet_reduce(procspec);
}

inline
multi_action_name mname(const std::string& s)
{
  multi_action_name alpha;
  alpha.insert(core::identifier_string(s));
  return alpha;
}

inline
multi_action_name mname(const std::string& s1, const std::string& s2)
{
  multi_action_name alpha;
  alpha.insert(core::identifier_string(s1));
  alpha.insert(core::identifier_string(s2));
  return alpha;
}

void test_alphabet()
{
  std::string text =
    "act a, b;        \n"
    "init a || b;     \n"
    ;
  process_specification procspec = parse_process_specification(text);
  multi_action_name_set A = alphabet(procspec.init(), procspec.equations());
  BOOST_CHECK(A.size() == 3);
  BOOST_CHECK(A.find(mname("a")) != A.end());
  BOOST_CHECK(A.find(mname("b")) != A.end());
  BOOST_CHECK(A.find(mname("a", "b")) != A.end());
}

void test_alphabet2()
{
  std::string text =
    "act a, b;        \n"
    "init allow({ a, a | b }, a || b);     \n"
    ;
  process_specification procspec = parse_process_specification(text);
  multi_action_name_set A = alphabet(procspec.init(), procspec.equations());
  BOOST_CHECK(A.size() == 2);
  BOOST_CHECK(A.find(mname("a")) != A.end());
  BOOST_CHECK(A.find(mname("a", "b")) != A.end());
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv);

  log::mcrl2_logger::set_reporting_level(log::debug);

  test_action_parse();
  // test_alphabet_allow();
  test_alphabet_reduce();
  test_alphabet();
  test_alphabet2();

  return EXIT_SUCCESS;
}
