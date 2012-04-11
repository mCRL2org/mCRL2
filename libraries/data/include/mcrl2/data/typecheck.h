// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/typecheck.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_TYPECHECK_H
#define MCRL2_DATA_TYPECHECK_H

#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/detail/pp_deprecated.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/unknown_sort.h"
#include "mcrl2/data/find.h"

namespace mcrl2
{

namespace data
{

/** \brief     Type check a sort expression.
 *  Throws an exception if something went wrong.
 *  \param[in] sort_expr A sort expression that has not been type checked.
 *  \post      sort_expr is type checked.
 **/
inline
void type_check(sort_expression& sort_expr, const data_specification& data_spec)
{
  // TODO: replace all this nonsense code by a proper type check implementation
  ATermAppl t = sort_expr;
  t = core::type_check_sort_expr(t, detail::data_specification_to_aterm_data_spec(data_spec));
  if (!t)
  {
    throw mcrl2::runtime_error("could not type check " + core::pp_deprecated(sort_expr));
  }
  sort_expr = sort_expression(t);
#ifndef MCRL2_DISABLE_TYPECHECK_ASSERTIONS
  assert(!search_sort_expression(sort_expr, unknown_sort()));
#endif
}

/** \brief     Type check a data expression.
 *  Throws an exception if something went wrong.
 *  \param[in] data_expr A data expression that has not been type checked.
 *  \param[in] first The start of a variables that can occur in the data expression.
 *  \param[in] last  The end of the potentially free variables in the expression.
 *  \param[in] data_spec The data specification that is used for type checking.
 *  \post      data_expr is type checked.
 **/
template <typename VariableIterator>
void type_check(data_expression& data_expr,
                const VariableIterator first,
                const VariableIterator last,
                const data_specification& data_spec = data_specification())
{
  // TODO: replace all this nonsense code by a proper type check implementation
  ATermAppl t = data_expr;

  atermpp::table variables;
  for (VariableIterator v = first; v != last; ++v)
  {
    // The application of atermpp::aterm_string is necessary to take care that
    // the name of the variable is quoted, which is what the typechecker expects.
    variables.put(atermpp::aterm_string(v->name()),v->sort());
  }

  // The typechecker replaces untyped identifiers by typed identifiers (when typechecking
  // succeeds) and adds type transformations between terms of sorts Pos, Nat, Int and Real if necessary.
  t = core::type_check_data_expr(t, 0, mcrl2::data::detail::data_specification_to_aterm_data_spec(data_spec), variables);
  if (t == 0)
  {
    data_expr = data_expression();
    throw mcrl2::runtime_error("could not type check " + core::pp_deprecated(data_expr));
  }
  else
  {
    data_expr = data_expression(t);
#ifndef MCRL2_DISABLE_TYPECHECK_ASSERTIONS
    assert(!search_sort_expression(data_expr, unknown_sort()));
#endif
  }
}

/** \brief     Type check a data expression.
 *  Throws an exception if something went wrong.
 *  \param[in] data_expr A data expression that has not been type checked.
 *  \post      data_expr is type checked.
 **/
inline
void type_check(data_expression& data_expr, const data_specification& data_spec = data_specification())
{
  variable_list v;
  return type_check(data_expr, v.begin(), v.end(), data_spec);
}

/** \brief     Type check a parsed mCRL2 data specification.
 *  Throws an exception if something went wrong.
 *  \param[in] data_spec A data specification that has not been type checked.
 *  \post      data_spec is type checked.
 **/
inline
void type_check(data_specification& data_spec)
{
  // TODO: replace all this nonsense code by a proper type check implementation
  ATermAppl t = detail::data_specification_to_aterm_data_spec(data_spec);
  t = core::type_check_data_spec(t);
  if (!t)
  {
    throw mcrl2::runtime_error("could not type check data specification " + core::pp_deprecated(data::detail::data_specification_to_aterm_data_spec(data_spec)));
  }
  data_spec = data_specification(t);
  data_spec.declare_data_specification_to_be_type_checked();
  // assert(!search_sort_expression(data_spec, unknown_sort())); SHOULD BE REACTIVATED. JFG TODO.
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_TYPECHECK_H
