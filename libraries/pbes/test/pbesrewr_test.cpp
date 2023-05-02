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
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/txt2pbes.h"

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
