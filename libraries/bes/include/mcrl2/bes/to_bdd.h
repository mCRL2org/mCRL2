// Author(s): XIAO Qi
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/to_bdd.h
/// \brief Convert a BES to BDD.

#ifndef MCRL2_BES_TO_BDD_H
#define MCRL2_BES_TO_BDD_H

#include <map>
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/bes/boolean_expression.h"
#include "mcrl2/bes/bdd_expression.h"
#include "mcrl2/bes/bdd_operations.h"

namespace mcrl2 {

namespace bes {

static mcrl2::bdd::bdd_expression to_bdd(const boolean_expression& b)
{
  if (is_true(b))
  {
    return mcrl2::bdd::true_();
  }
  if (is_false(b))
  {
    return mcrl2::bdd::false_();
  }
  mcrl2::bdd::bdd_expression result;
  if (is_boolean_variable(b))
  {
    const core::identifier_string name = atermpp::down_cast<const core::identifier_string>(b[0]);
    result = mcrl2::bdd::if_(name, mcrl2::bdd::true_(), mcrl2::bdd::false_());
  }
  else if (is_and(b))
  {
    result = mcrl2::bdd::ordered_and(
        to_bdd(accessors::left(b)),
        to_bdd(accessors::right(b)));
  }
  else if (is_or(b))
  {
    result = mcrl2::bdd::ordered_or(
        to_bdd(accessors::left(b)),
        to_bdd(accessors::right(b)));
  }
  else
  {
    mCRL2log(mcrl2::log::error) << "Unexpected boolean expression " << b << std::endl;
    assert(0);
  }
  return result;
}

// Determine equality of two boolean expressions by comparing their ordered
// BDDs.
inline bool bdd_equal(const boolean_expression& x, const boolean_expression& y)
{
  return to_bdd(x) == to_bdd(y);
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_TO_BDD_H
