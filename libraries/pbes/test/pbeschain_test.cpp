// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesrewr_test.cpp
/// \brief Test for the pbes rewriters.

#include "mcrl2/data/variable.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/utilities/logger.h"
#define BOOST_TEST_MODULE pbesrewr_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/modal_formula/detail/test_input.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/txt2pbes.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

BOOST_AUTO_TEST_CASE(test_pbeschain1)
{
  std::string pbes_text = "sort Enum = struct e1 | e2;                           \n"
                          "pbes mu X(n:Enum)=exists m1,m2:Enum.(X(m1) || X(m2)); \n"
                          "init X(e1);                                           \n";
  pbes p = txt2pbes(pbes_text);
  data::rewriter datar(p.data(), data::jitty);
  bool enumerate_infinite_sorts = true;
  enumerate_quantifiers_rewriter pbesr(datar, p.data(), enumerate_infinite_sorts);
  pbes_rewrite(p, pbesr);
  BOOST_CHECK(p.is_well_typed());
}

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \return All data variables that occur in the term t
template<typename Container>
std::vector<propositional_variable_instantiation> get_propositional_variable_instantiations(Container const& container)
{
  std::vector<propositional_variable_instantiation> result;
  pbes_system::find_propositional_variable_instantiations(container, std::inserter(result, result.end()));
  return result;
}

BOOST_AUTO_TEST_CASE(test_pbeschain2)
{
  std::string pbes_text = "pbes nu X(m:Nat)=forall n:Nat.(X(m)); \n"
                          "init X(0);                                           \n";
  pbes p = txt2pbes(pbes_text);

  data::rewriter data_rewriter(p.data());
  simplify_data_rewriter<data::rewriter> pbes_rewriter(data_rewriter);

  pbes_equation eq = p.equations()[0];

  data::mutable_indexed_substitution sigma;
  propositional_variable_instantiation x = get_propositional_variable_instantiations(eq.formula())[0];
  data::data_expression_list pars = x.parameters();

  std::cout << "Original PBES:\n" << pp(p) << std::endl;
  std::cout << "Going to substitute on equation:\n" << pp(eq.formula()) << std::endl;

  std::cout << "Free variables in expression: " ;
  for (data::variable const& var: find_free_variables(eq.formula()))
  {
    std::cout <<  pp(var) << " " << std::endl;
  }
  std::cout << "\n --- --- --- \n" << std::endl;

  const data::variable& v = eq.variable().parameters().front();
  data::data_expression par = pars.front();
  core::identifier_string name("n");
  sigma[v] = data::variable(name, par.sort());
  std::cout << " Substitution: " << pp(v) << " to " << pp(name) << std::endl;

  pbes_expression p2 = pbes_rewrite(eq.formula(), pbes_rewriter, sigma);

  std::cout << "Rewritten pbes expression:\n" << pp(p2) << std::endl;

  std::cout << "Free variables in expression: " ;
  for (data::variable const& var: find_free_variables(p2))
  {
    std::cout << pp(var) << " " << std::endl;
  }
  std::cout << "\n --- --- --- \n" << std::endl;

  BOOST_CHECK(p.is_well_typed());
}
