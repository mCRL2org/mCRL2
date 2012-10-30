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

multi_action_name_set parse_multi_action_name_set(const std::string& text, const lps::action_label_list& action_decls = lps::parse_action_declaration("a, b, c, d;"))
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

template <typename Container>
std::string print_container(const Container& c, const std::string& start = "", const std::string& end = "", const std::string& sep = "")
{
  std::ostringstream out;
  out << start;
  for (typename Container::const_iterator i = c.begin(); i != c.end(); ++i)
  {
    if (i != c.begin())
    {
      out << sep;
    }
    out << *i;
  }
  out << end;
  return out.str();
}

template <typename Container>
std::string print_set(const Container& c)
{
  return print_container(c, "{", "}", ", ");
}

std::string print(const multi_action_name& alpha)
{
  std::set<std::string> A;
  for (multi_action_name::const_iterator i = alpha.begin(); i != alpha.end(); ++i)
  {
    A.insert(std::string(*i));
  }
  return print_container(A);
}

std::string print(const multi_action_name_set& A)
{
  std::set<std::string> V;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    V.insert(print(*i));
  }
  return print_set(V);
}

void test_parse()
{
  lps::action_label_list act_decl = lps::parse_action_declaration("a: Nat;");
  lps::action a = lps::parse_action("a(2)", act_decl);
  multi_action_name A;
  A.insert(core::identifier_string("a"));
  BOOST_CHECK(name(a) == A);

  multi_action_name_set B = parse_multi_action_name_set("{a, a|b}");
  BOOST_CHECK(print(B) == "{a, ab}");
}

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

void test_alphabet(const std::string& expression, const std::string& expected_result, const std::string& equations = "")
{
  std::string text = "act a, b, c, d;\n" + equations + "\ninit " + expression + ";\n";
  process_specification procspec = parse_process_specification(text);
  multi_action_name_set A = alphabet(procspec.init(), procspec.equations());
  if (print(A) != expected_result)
  {
    std::cout << "--- test_alphabet failure ---" << std::endl;
    std::cout << "expression      = " << expression << std::endl;
    std::cout << "result          = " << print(A) << std::endl;
    std::cout << "expected result = " << expected_result << std::endl;
    BOOST_CHECK(print(A) == expected_result);
  }
}

void test_alphabet()
{
  test_alphabet("a || b", "{a, ab, b}");
  test_alphabet("allow({ a, a | b }, a || b)", "{a, ab}");
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv);
  log::mcrl2_logger::set_reporting_level(log::debug);

  test_parse();
  test_alphabet_reduce();
  test_alphabet();

  return EXIT_SUCCESS;
}
