// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_SMT_TRANSLATE_SPECIFICATION_H
#define MCRL2_SMT_TRANSLATE_SPECIFICATION_H

#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/smt2/native_translation.h"

namespace mcrl2
{
namespace smt
{
namespace detail
{

/**
 * \brief Declare variables to be used in binder such as exists or forall and print the declaration to out
 * \return An expression that constrains the domains of Pos and Nat variables
 */
template <class OutputStream>
static inline
data::data_expression declare_variables_binder(OutputStream& out, const data::variable_list& vars)
{
  data::data_expression result = data::sort_bool::true_();
  out << "(";
  for(const data::variable& var: vars)
  {
    out << "(" << var.name() << " ";
    if(var.sort() == data::sort_pos::pos())
    {
      out << "Int";
      result = data::lazy::and_(result, greater_equal(var, data::sort_pos::c1()));
    }
    else if(var.sort() == data::sort_nat::nat())
    {
      out << "Int";
      result = data::lazy::and_(result, greater_equal(var, data::sort_nat::c0()));
    }
    else
    {
      out << var.sort();
    }
    out << ") ";
  }
  out << ")";
  return result;
}

template <typename OutputStream>
inline
void translate_mapping(const data::function_symbol& f, OutputStream& out, const native_translations& nt)
{
  if(has_native_translation(f, nt))
  {
    return;
  }

  out << "(declare-fun " << f.name() << " ";
  if(data::is_function_sort(f.sort()))
  {
    out << "(";
    data::function_sort fs(atermpp::down_cast<data::function_sort>(f.sort()));
    for(const data::sort_expression& s: fs.domain())
    {
      out << s << " ";
    }
    out << ") " << fs.codomain();
  }
  else
  {
    out << "() " << f.sort();
  }
  out << ")\n";
}

template <typename OutputStream>
inline
void translate_equation(const data::data_equation& eq, OutputStream& out, const native_translations& nt)
{
  if(has_native_translation(eq, nt))
  {
    return;
  }

  const data::variable_list& vars = eq.variables();
  data::data_expression body(data::sort_bool::implies(eq.condition(), data::equal_to(eq.lhs(), eq.rhs())));
  data::data_expression definition(vars.empty() ? body : data::forall(vars, body));

  out << "(assert ";
  translate_data_expression(definition, out, nt);
  out << ")\n";
}

} // namespace detail

template <typename OutputStream>
void translate_data_specification(const data::data_specification& dataspec, OutputStream& o, const native_translations& nt)
{
  for(const data::function_symbol& f: dataspec.mappings())
  {
    detail::translate_mapping(f, o, nt);
  }
  for(const data::data_equation& eq: dataspec.equations())
  {
    detail::translate_equation(eq, o, nt);
  }
}

} // namespace smt
} // namespace mcrl2

#endif
