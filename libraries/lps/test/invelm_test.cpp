// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parelm_test.cpp
/// \brief Add your file description here.

//#define MCRL2_LPS_PARELM_DEBUG

#include <iostream>
#include <string>
#include <boost/test/included/unit_test_framework.hpp>

#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/utilities/test_utilities.h"
#include "mcrl2/atermpp/aterm_init.h"

#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/specification_property_map.h"
#include "mcrl2/lps/invariant_checker.h"
#include "mcrl2/lps/invariant_eliminator.h"
#include "test_specifications.h"

using mcrl2::utilities::collect_after_test_case;
BOOST_GLOBAL_FIXTURE(collect_after_test_case)

using namespace mcrl2;

static inline
lps::specification invelm(const lps::specification& specification,
                          const data::data_expression& invariant)
{
  lps::detail::Invariant_Checker v_invariant_checker(specification);

  if (v_invariant_checker.check_invariant(invariant))
  {
    lps::detail::Invariant_Eliminator invariant_eliminator(specification);

    return invariant_eliminator.simplify(invariant, false, 0);
  }
  return specification;
}

BOOST_AUTO_TEST_CASE(test_abp)
{
  lps::specification specification = lps::parse_linear_process_specification(LINEAR_ABP);
  data::data_expression invariant = data::parse_data_expression("true");

  lps::specification result = invelm(specification, invariant);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return 0;
}
