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
#include "mcrl2/smt2/translate_expression.h"
#include "mcrl2/smt2/translate_sort.h"

namespace mcrl2
{
namespace smt
{
namespace detail
{

template <typename OutputStream>
inline
void translate_sort_definition(const std::string& sort_name, const data::sort_expression& s, const data::data_specification& dataspec, OutputStream& out, const native_translations& nt, data::set_identifier_generator& id_gen)
{
  auto find_result = nt.sorts.find(s);
  if(find_result != nt.sorts.end())
  {
    // Do not output anything for natively defined sorts
    return;
  }

  out << "(declare-datatypes () ((" << sort_name << " ";
  for(const data::function_symbol& cons: dataspec.constructors(s))
  {
    out << "(" << cons.name() << " ";
    if(data::is_function_sort(cons.sort()))
    {
      const data::function_sort& cs = atermpp::down_cast<data::function_sort>(cons.sort());
      for(const data::sort_expression& arg: cs.domain())
      {
        out << "(" << id_gen("@recog") << " ";
        translate_sort_expression(arg, out, nt);
        out << ") ";
      }
    }
    out << ") ";
  }
  out << ")))\n";
}

template <typename OutputStream>
inline
void translate_sort_definition(const data::basic_sort& s, const data::data_specification& dataspec, OutputStream& out, const native_translations& nt, data::set_identifier_generator& id_gen)
{
  translate_sort_definition(pp(s.name()), s, dataspec, out, nt, id_gen);
}

template <typename OutputStream>
inline
void translate_alias(const data::sort_expression& s, const data::data_specification& dataspec, OutputStream& out, const native_translations& nt)
{

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
  data::set_identifier_generator id_gen;
  for(const data::basic_sort& s: dataspec.user_defined_sorts())
  {
    detail::translate_sort_definition(s, dataspec, o, nt, id_gen);
  }
  for(const data::alias& s: dataspec.user_defined_aliases())
  {
    if(data::is_structured_sort(s.reference()))
    {
      detail::translate_sort_definition(pp(s.name().name()), s.reference(), dataspec, o, nt, id_gen);
    }
    // detail::translate_sort_definition(s, dataspec, o, nt, id_gen);
  }
  for(const data::function_symbol& f: dataspec.user_defined_mappings())
  {
    detail::translate_mapping(f, o, nt);
  }
  for(const data::data_equation& eq: dataspec.user_defined_equations())
  {
    detail::translate_equation(eq, o, nt);
  }
}

} // namespace smt
} // namespace mcrl2

#endif
