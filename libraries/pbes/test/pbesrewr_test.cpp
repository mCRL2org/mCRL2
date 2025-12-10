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

#define BOOST_TEST_MODULE pbesrewr_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/modal_formula/detail/test_input.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/tools/pbeschain.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/utilities/logger.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

BOOST_AUTO_TEST_CASE(test_pbesrewr1)
{
  std::string pbes_text =
    "sort Enum = struct e1 | e2;                           \n"
    "pbes mu X(n:Enum)=exists m1,m2:Enum.(X(m1) || X(m2)); \n"
    "init X(e1);                                           \n"
    ;
  pbes p = txt2pbes(pbes_text);
  data::rewriter datar(p.data(), data::jitty);
  bool enumerate_infinite_sorts = true;
  enumerate_quantifiers_rewriter pbesr(datar, p.data(), enumerate_infinite_sorts);
  pbes_rewrite(p, pbesr);
  BOOST_CHECK(p.is_well_typed());
}

BOOST_AUTO_TEST_CASE(test_pbesrewr2)
{
  lps::specification spec = remove_stochastic_operators(lps::linearise(lps::detail::ABP_SPECIFICATION()));
  state_formulas::state_formula formula = state_formulas::parse_state_formula(lps::detail::NO_DEADLOCK(), spec, false);
  bool timed = false;
  pbes p = lps2pbes(spec, formula, timed);
  BOOST_CHECK(p.is_well_typed());

  data::rewriter datar(p.data(), data::jitty);
  bool enumerate_infinite_sorts = true;
  enumerate_quantifiers_rewriter pbesr(datar, p.data(), enumerate_infinite_sorts);
  pbes_rewrite(p, pbesr);
  BOOST_CHECK(p.is_well_typed());
}

BOOST_AUTO_TEST_CASE(test_pbesrewr3)
{
  std::string pbes_text =
    "map config: Nat -> Bool;                           \n"
    "var  n: Nat;                                       \n"
    "eqn config(n) = false;                             \n"
    "pbes mu X(vc_TS: Nat -> Bool)= forall n : Nat. "
    "                               X(vc_TS[n -> true]);\n"
    "init X(config);                                    \n"
    ;
  pbes p = txt2pbes(pbes_text);
  data::rewriter datar(p.data());
  simplify_data_rewriter<data::rewriter> pbesr(datar);
  data::mutable_indexed_substitution sigma;
  data::variable var = p.equations()[0].variable().parameters().front();
  propositional_variable_instantiation fn = *find_propositional_variable_instantiations(p.equations()[0].formula()).begin();
  auto update = fn.parameters().front();
  mCRL2log(log::info) << "The function update: " <<  update << std::endl;
  sigma[var] = update;
  pbes_rewrite(p, pbesr, sigma);
  mCRL2log(log::info) << pp(p) << std::endl;
  
  pbes_rewrite(p, pbesr, sigma);
  pbes_rewrite(p, pbesr, sigma);
  
  mCRL2log(log::info) << pp(p) << std::endl;
  BOOST_CHECK(p.is_well_typed());
}

/* The test below checks whether the simplify_data_rewriter substitutes in a capture avoiding way. */
BOOST_AUTO_TEST_CASE(test_pbeschain2)
{
  std::string pbes_text = "pbes nu X(m:Nat)=forall n:Nat.X(m); \n"
                          "init X(0);                                           \n";
  pbes p = txt2pbes(pbes_text);

  data::rewriter data_rewriter(p.data());
  simplify_data_rewriter<data::rewriter> pbes_rewriter(data_rewriter);

  pbes_equation eq = p.equations()[0];

  data::mutable_indexed_substitution sigma;
  propositional_variable_instantiation x = get_propositional_variable_instantiations(eq.formula())[0];

  data::data_expression_list pars = x.parameters();

  std::cerr << "Original PBES:\n" << p << std::endl;
  std::cerr << "Going to substitute in rhs of equation: " << eq.formula() << std::endl;

  std::cerr << "Free variables in expression to be substituted:\n " ;
  for (const data::variable& var: find_free_variables(eq.formula()))
  {
    std::cerr <<  var << " " << std::endl;
  }
  std::cerr << "--- --- --- \n";

  const data::variable& v = eq.variable().parameters().front();
  data::data_expression par = pars.front();
  core::identifier_string name("n");
  sigma[v] = data::variable(name, par.sort());
  std::cerr << " Substitution: " << v << " := " << name << std::endl;

  pbes_expression p2 = pbes_rewrite(eq.formula(), pbes_rewriter, sigma);

  std::cerr << "Rewritten pbes expression:\n" << p2 << std::endl;

  std::cerr << "Free variables in expression:\n " ;
  const std::set<data::variable> free_variables = find_free_variables(p2);
  for (const data::variable& var: free_variables)
  {
    std::cerr << var << " " << std::endl;
  }
  std::cerr << "--- --- --- \n";

  BOOST_CHECK(free_variables.size()==1);
}

// The test below checks whether the head of a term is rewritten too often.
// If so, the variable vc_TS is substituted infinitely often causing the rewriting
// of the PBES to not terminate. 

BOOST_AUTO_TEST_CASE(test_pbesrewr3)
{
  std::string pbes_text =
  "map VC_config: Nat -> Bool;"
  "var  n: Nat;"
  "eqn  VC_config(n)  =  false;"
  "pbes nu Y(vc_TS: Nat -> Bool) = "
         "val(vc_TS(1)) && (forall v_TS1: Nat. Y(vc_TS[v_TS1 -> true]));"
  "init Y(VC_config);"
  ;
    
  pbes p = txt2pbes(pbes_text);
  data::rewriter datar(p.data(), data::rewrite_strategy::jitty);
  simplify_data_rewriter<data::rewriter> pbesr(datar);
    
  data::mutable_indexed_substitution sigma;
  pbes_equation equation = p.equations()[0];
  propositional_variable_instantiation pvi = *find_propositional_variable_instantiations(equation.formula()).begin();
  data::variable var = equation.variable().parameters().front();
  sigma[var] = *pvi.parameters().begin();
  // With erroneous rewriting this does not terminate.
  pbes_rewrite(p, pbesr, sigma);
  // After this rewrite p is not well typed anymore. 

#ifdef MCRL2_ENABLE_JITTYC
  // Now do ti again with the compiling rewriter. 
  data::rewriter datar_compiling(p.data(), data::rewrite_strategy::jitty_compiling);
  simplify_data_rewriter<data::rewriter> pbesr_compiling(datar_compiling);
  pbes_rewrite(p, pbesr_compiling, sigma);
  // After this rewrite p is not well typed anymore. 

  BOOST_CHECK(true);
#endif // MCRL2_ENABLE_JITTYC
}

// The test below checks whether the head of a term is rewritten too often.
// If so, the variable vc_TS is substituted infinitely often causing the rewriting
// of the PBES to not terminate. 

BOOST_AUTO_TEST_CASE(test_pbesrewr4)
{
  std::string pbes_text =
  "map VC_config: Nat -> Bool;"
  "var  n: Nat;"
  "eqn  VC_config(n)  =  false;"
  "pbes nu Y(vc_TS: Nat -> Bool) = "
         "val(vc_TS(1)) && (forall v_TS1: Nat. Y(vc_TS[v_TS1 -> vc_TS(v_TS1)]));"
  "init Y(VC_config);"
  ;
    
  pbes p = txt2pbes(pbes_text);
  data::rewriter datar(p.data(), data::rewrite_strategy::jitty);
  simplify_data_rewriter<data::rewriter> pbesr(datar);
    
  data::mutable_indexed_substitution sigma;
  pbes_equation equation = p.equations()[0];
  propositional_variable_instantiation pvi = *find_propositional_variable_instantiations(equation.formula()).begin();
  data::variable var = equation.variable().parameters().front();
  sigma[var] = *pvi.parameters().begin();
  // With erroneous rewriting this does not terminate.
  pbes_rewrite(p, pbesr, sigma);
  // After this rewrite p is not well typed anymore. 

#ifdef MCRL2_ENABLE_JITTYC
  // Now do it again with the compiling rewriter. 
  data::rewriter datar_compiling(p.data(), data::rewrite_strategy::jitty_compiling);
  simplify_data_rewriter<data::rewriter> pbesr_compiling(datar_compiling);
  std::cerr << "COMPILING TEST4 " << (*pvi.parameters().begin()) << "\n";
  pbes_rewrite(p, pbesr_compiling, sigma); 
  // After this rewrite p is not well typed anymore. 
  std::cerr << "PBES2" << p << "\n";

  BOOST_CHECK(true);
#endif // MCRL2_ENABLE_JITTYC
}
