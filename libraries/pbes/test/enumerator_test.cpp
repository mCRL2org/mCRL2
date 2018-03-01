// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file enumerator_test.cpp
/// \brief Add your file description here.

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/pbes/enumerator.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/pbes/rewriters/simplify_rewriter.h"
#include <boost/test/minimal.hpp>

using namespace mcrl2;
using namespace mcrl2::pbes_system;

  const std::string VARSPEC =
    "datavar         \n"
    "  m: Nat;       \n"
    "  n: Nat;       \n"
    "  b: Bool;      \n"
    "  c: Bool;      \n"
    "                \n"
    "predvar         \n"
    "  X: Bool, Pos; \n"
    "  Y: Nat;       \n"
    ;

void test_enumerator()
{
  typedef pbes_system::simplify_data_rewriter<data::rewriter> pbes_rewriter;
  typedef data::enumerator_list_element<pbes_expression> enumerator_element;

  data::data_specification data_spec;
  data_spec.add_context_sort(data::sort_nat::nat());
  data::rewriter datar(data_spec);
  pbes_rewriter R(datar);

  data::variable_list v;
  v.push_front(data::variable("n", data::sort_nat::nat()));
  pbes_expression phi = parse_pbes_expression("val(n < 2)", VARSPEC);
  data::mutable_indexed_substitution<> sigma;
  data::enumerator_identifier_generator id_generator("x");
  data::enumerator_algorithm<pbes_rewriter> E(R, data_spec, datar, id_generator);
  std::vector<pbes_system::pbes_expression> solutions;
  std::deque<enumerator_element> P;
  P.push_back(enumerator_element(v, phi));
  E.next(P, sigma, is_not_true());
  while (!P.empty())
  {
    solutions.push_back(P.front().expression());
    P.pop_front();
    E.next(P, sigma, is_not_true());
  }
  std::clog << "solutions = " << core::detail::print_list(solutions) << std::endl;
  BOOST_CHECK(solutions.size() >= 1);
}

void test_enumerator_with_iterator()
{
  typedef pbes_system::simplify_data_rewriter<data::rewriter> pbes_rewriter;
  typedef data::enumerator_list_element<pbes_expression> enumerator_element;

  data::data_specification data_spec;
  data_spec.add_context_sort(data::sort_nat::nat());
  data::rewriter datar(data_spec);
  pbes_rewriter R(datar);

  data::variable_list v;
  v.push_front(data::variable("n", data::sort_nat::nat()));
  pbes_expression phi = parse_pbes_expression("val(n < 2)", VARSPEC);
  data::mutable_indexed_substitution<> sigma;
  data::enumerator_identifier_generator id_generator;
  data::enumerator_algorithm_with_iterator<pbes_rewriter, enumerator_element, pbes_system::is_not_true> E(R, data_spec, datar, id_generator, 20);
  std::vector<pbes_system::pbes_expression> solutions;

  std::deque<enumerator_element> P;
  P.push_back(enumerator_element(v, phi));
  for (auto i = E.begin(sigma, P); i != E.end(); ++i)
  {
    solutions.push_back(i->expression());
  }
  std::clog << "solutions = " << core::detail::print_list(solutions) << std::endl;
  BOOST_CHECK(solutions.size() >= 1);
}

void test_enumerator_with_substitutions()
{
  typedef pbes_system::simplify_data_rewriter<data::rewriter> pbes_rewriter;
  typedef data::enumerator_list_element_with_substitution<pbes_expression> enumerator_element;

  data::data_specification data_spec;
  data_spec.add_context_sort(data::sort_nat::nat());
  data::rewriter datar(data_spec);
  pbes_rewriter R(datar);

  data::variable_list v;
  v.push_front(data::variable("n", data::sort_nat::nat()));
  pbes_expression phi = parse_pbes_expression("val(n < 2)", VARSPEC);
  data::mutable_indexed_substitution<> sigma;
  data::enumerator_identifier_generator id_generator;
  data::enumerator_algorithm_with_iterator<pbes_rewriter, enumerator_element, pbes_system::is_not_false> E(R, data_spec, datar, id_generator);
  std::vector<pbes_system::pbes_expression> solutions;

  std::deque<enumerator_element> P;
  P.push_back(enumerator_element(v, phi));
  for (auto i = E.begin(sigma, P); i != E.end(); ++i)
  {
    solutions.push_back(i->expression());
    data::mutable_map_substitution<> sigma;
    i->add_assignments(v, sigma, datar);
    std::clog << "  solutions " << i->expression() << " substitution = " << sigma << std::endl;
    BOOST_CHECK(R(phi, sigma) == i->expression());
  }
  std::clog << "solutions = " << core::detail::print_list(solutions) << std::endl;
  BOOST_CHECK(solutions.size() >= 1);
}

int test_main(int argc, char** argv)
{
  log::mcrl2_logger::set_reporting_level(log::debug);
  test_enumerator();
  test_enumerator_with_iterator();
  test_enumerator_with_substitutions();

  return 0;
}
