// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file enumerator_test.cpp
/// \brief Add your file description here.

#define BOOST_TEST_MODULE enumerator_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/data/enumerator_with_iterator.h"
#include "mcrl2/pbes/enumerator.h"
#include "mcrl2/pbes/detail/parse.h"
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

BOOST_AUTO_TEST_CASE(test_enumerator)
{
  using pbes_rewriter = pbes_system::simplify_data_rewriter<data::rewriter>;
  using enumerator_element = data::enumerator_list_element<pbes_expression>;

  data::data_specification data_spec;
  data_spec.add_context_sort(data::sort_nat::nat());
  data::rewriter datar(data_spec);
  pbes_rewriter R(datar);

  data::variable_list v;
  v.push_front(data::variable("n", data::sort_nat::nat()));
  pbes_expression phi = parse_pbes_expression("val(n < 2)", VARSPEC);
  data::mutable_indexed_substitution<> sigma;
  data::enumerator_identifier_generator id_generator("x");
  bool accept_solutions_with_variables = true;
  data::enumerator_algorithm<pbes_rewriter> E(R, data_spec, datar, id_generator, accept_solutions_with_variables);
  std::set<pbes_system::pbes_expression> solutions;
  E.enumerate(enumerator_element(v, phi),
              sigma,
              [&](const enumerator_element& p)
              {
                solutions.insert(p.expression());
                return false; // do not interrupt
              },
              is_false
  );
  std::clog << "solutions = " << core::detail::print_list(solutions) << std::endl;
  BOOST_CHECK(solutions.size() == 1);
}

BOOST_AUTO_TEST_CASE(test_enumerator_with_iterator)
{
  using pbes_rewriter = pbes_system::simplify_data_rewriter<data::rewriter>;
  using enumerator_element = data::enumerator_list_element<pbes_expression>;

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

  data::enumerator_queue<enumerator_element> P;
  P.push_back(enumerator_element(v, phi));
  for (auto i = E.begin(sigma, P); i != E.end(); ++i)
  {
    solutions.push_back(i->expression());
  }
  std::clog << "solutions = " << core::detail::print_list(solutions) << std::endl;
  BOOST_CHECK(solutions.size() >= 1);
}

BOOST_AUTO_TEST_CASE(test_enumerator_with_substitutions)
{
  using pbes_rewriter = pbes_system::simplify_data_rewriter<data::rewriter>;
  using enumerator_element = data::enumerator_list_element_with_substitution<pbes_expression>;

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

  data::enumerator_queue<enumerator_element> P;
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

BOOST_AUTO_TEST_CASE(enumerate_callback)
{
  using pbes_rewriter = pbes_system::simplify_data_rewriter<data::rewriter>;
  using enumerator_element = data::enumerator_list_element<pbes_expression>;
  data::enumerator_identifier_generator id_generator;
  data::data_specification dataspec;
  dataspec.add_context_sort(data::sort_int::int_());
  std::size_t max_count = 10;
  data::rewriter r(dataspec);
  pbes_rewriter R(r);
  data::enumerator_algorithm<pbes_rewriter> E(R, dataspec, r, id_generator, max_count);

  auto enumerate = [&](const pbes_expression& x)
  {
    data::rewriter::substitution_type sigma;
    pbes_expression result;
    id_generator.clear();
    if (is_forall(x))
    {
      const pbes_system::forall& x_ = atermpp::down_cast<pbes_system::forall>(x);
      result = pbes_system::true_();
      E.enumerate(enumerator_element(x_.variables(), R(x_.body())),
                  sigma,
                  [&](const enumerator_element& p)
                  {
                    data::optimized_and(result, result, p.expression());
                    return is_false(result);
                  },
                  pbes_system::is_true,
                  pbes_system::is_false
      );
    }
    else if (is_exists(x))
    {
      const pbes_system::exists& x_ = atermpp::down_cast<pbes_system::exists>(x);
      result = pbes_system::false_();
      E.enumerate(enumerator_element(x_.variables(), R(x_.body())),
                  sigma,
                  [&](const enumerator_element& p)
                  {
                    data::optimized_or(result, result, p.expression());
                    return is_true(result);
                  },
                  pbes_system::is_false,
                  pbes_system::is_true
      );
    }
    return result;
  };

  BOOST_CHECK_EQUAL(enumerate(parse_pbes_expression("forall n: Nat. val(n < 2)")), false_());
  BOOST_CHECK_EQUAL(enumerate(parse_pbes_expression("exists n: Nat. val(n < 2)")), true_());
}
