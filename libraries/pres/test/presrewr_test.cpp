// Author(s): Jan Friso Groote. Based on pbesrewr_test by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file presrewr_test.cpp
/// \brief Test for the pres rewriters.

#define BOOST_TEST_MODULE presrewr_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/modal_formula/detail/test_input.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/pres/lps2pres.h"
#include "mcrl2/pres/rewrite.h"
#include "mcrl2/pres/rewriter.h"
#include "mcrl2/pres/rewriters/enumerate_quantifiers_rewriter.h"
#include "mcrl2/pres/txt2pres.h"

using namespace mcrl2;
using namespace mcrl2::pres_system;

BOOST_AUTO_TEST_CASE(test_presrewr1)
{
  std::string pres_text =
    "sort Enum = struct e1 | e2;                           \n"
    "pres mu X(n:Enum)=sup m1,m2:Enum.(X(m1) || X(m2)); \n"
    "init X(e1);                                           \n"
    ;
  pres p = txt2pres(pres_text);
  data::rewriter datar(p.data(), data::jitty);
  bool enumerate_infinite_sorts = true;
  enumerate_quantifiers_rewriter presr(datar, p.data(), enumerate_infinite_sorts);
  pres_rewrite(p, presr);
  BOOST_CHECK(p.is_well_typed());
}

BOOST_AUTO_TEST_CASE(test_presrewr2)
{
  lps::stochastic_specification spec = lps::linearise(lps::detail::ABP_SPECIFICATION());
  state_formulas::state_formula formula = state_formulas::parse_state_formula(lps::detail::NO_DEADLOCK(), spec, false);
  bool timed = false;
  pres p = lps2pres(spec, formula, timed);
  BOOST_CHECK(p.is_well_typed());

  data::rewriter datar(p.data(), data::jitty);
  bool enumerate_infinite_sorts = true;
  enumerate_quantifiers_rewriter presr(datar, p.data(), enumerate_infinite_sorts);
  pres_rewrite(p, presr);
  BOOST_CHECK(p.is_well_typed());
}
