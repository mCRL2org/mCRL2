// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file rewriter_test.cpp
/// \brief Test for the pbes rewriters.

#define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG
#define MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
#define MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG

#include <iostream>
#include <boost/test/minimal.hpp>
#include "mcrl2/data/enumerator.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/modal_formula/detail/test_input.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

void test_pbesrewr1()
{
  std::string pbes_text =
    "sort Enum = struct e1 | e2;                           \n"
    "pbes mu X(n:Enum)=exists m1,m2:Enum.(X(m1) || X(m2)); \n"
    "init X(e1);                                           \n"
    ;
  pbes<> p = txt2pbes(pbes_text);
  data::rewriter datar(p.data(), data::rewriter::jitty);
  utilities::number_postfix_generator generator("UNIQUE_PREFIX");
  data::data_enumerator<> datae(p.data(), datar, generator);
  data::rewriter_with_variables datarv(datar);
  bool enumerate_infinite_sorts = true;
  enumerate_quantifiers_rewriter<pbes_expression, data::rewriter_with_variables, data::data_enumerator<> > pbesr(datarv, datae, enumerate_infinite_sorts);
  pbes_rewrite(p, pbesr);
  // p.save("pbesrewr.pbes");
}

void test_pbesrewr2()
{
  lps::specification spec = lps::linearise(lps::detail::ABP_SPECIFICATION());
  state_formulas::state_formula formula = state_formulas::parse_state_formula(lps::detail::NO_DEADLOCK(), spec);
  bool timed = false;
  pbes<> p = lps2pbes(spec, formula, timed);
  BOOST_CHECK(p.is_well_typed());

  data::rewriter datar(p.data(), data::rewriter::jitty);
  utilities::number_postfix_generator generator("UNIQUE_PREFIX");
  data::data_enumerator<> datae(p.data(), datar, generator);
  data::rewriter_with_variables datarv(datar);
  bool enumerate_infinite_sorts = true;
  enumerate_quantifiers_rewriter<pbes_expression, data::rewriter_with_variables, data::data_enumerator<> > pbesr(datarv, datae, enumerate_infinite_sorts);
  pbes_rewrite(p, pbesr);
  BOOST_CHECK(p.is_well_typed());
}

// Check that existential quantification over empty domain is not automatically
// rewritten to false.
void test_pbesrewr3()
{
  std::string pbes_text =
    "sort D;\n"
    "map f:D -> Bool;\n"
    "pbes nu X = exists d:D . val(f(d));\n"
    "init X;\n"
    ;

  pbes<> p = txt2pbes(pbes_text);
  data::rewriter datar(p.data(), data::rewriter::jitty);
  utilities::number_postfix_generator generator("UNIQUE_PREFIX");
  data::data_enumerator<> datae(p.data(), datar, generator);
  data::rewriter_with_variables datarv(datar);
  bool enumerate_infinite_sorts = true;
  enumerate_quantifiers_rewriter<pbes_expression, data::rewriter_with_variables, data::data_enumerator<> > pbesr(datarv, datae, enumerate_infinite_sorts);
  try
  {
    pbes_rewrite(p, pbesr); // we expect that an exception is raised because of the type D that cannot be enumerated
  }
  catch (mcrl2::runtime_error)
  {
    // this is OK
    return;
  }
  BOOST_CHECK(false); // this point should not be reached
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_pbesrewr1();
  test_pbesrewr2();
  test_pbesrewr3();

  return 0;
}
