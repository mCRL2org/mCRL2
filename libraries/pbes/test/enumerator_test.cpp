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

#include <boost/test/minimal.hpp>
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/pbes/enumerator.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/rewriters/simplify_rewriter.h"

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

// TODO: use a lambda expression instead of this struct
struct report
{
  std::vector<pbes_system::pbes_expression>& solutions;

  report(std::vector<pbes_system::pbes_expression>& solutions_)
    : solutions(solutions_)
  {}

  void operator()(const enumerator_list_element<pbes_expression>& e) const
  {
    solutions.push_back(e.phi);
  }
};

void test_enumerator()
{
  typedef pbes_system::simplify_data_rewriter<data::rewriter> pbes_rewriter;
  typedef core::term_traits<pbes_expression> tr;
  typedef core::term_traits<data::data_expression> tt;

  data::data_specification data_spec;
  data_spec.add_context_sort(data::sort_nat::nat());
  data::rewriter datar(data_spec);
  pbes_rewriter R(datar);

  data::variable_list v;
  v.push_front(data::variable("n", data::sort_nat::nat()));
  pbes_expression phi = parse_pbes_expression("val(n < 2)", VARSPEC);
  pbes_expression stop = tt::false_();

  data::mutable_indexed_substitution<> sigma;
  enumerator_algorithm<pbes_rewriter, data::mutable_indexed_substitution<> > E(R, sigma, data_spec);
  std::vector<pbes_system::pbes_expression> solutions;
  std::deque<enumerator_list_element<pbes_expression> > P;
  P.push_back(enumerator_list_element<pbes_expression>(v, phi));
  E.next(P, is_not_true(), report(solutions));
  std::clog << "solutions = " << core::detail::print_list(solutions) << std::endl;
  BOOST_CHECK(solutions.size() >= 1);
}

int test_main(int argc, char** argv)
{
  log::mcrl2_logger::set_reporting_level(log::debug);
  test_enumerator();

  return 0;
}
