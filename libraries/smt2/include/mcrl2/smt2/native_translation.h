// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_SMT_NATIVE_TRANSLATION_H
#define MCRL2_SMT_NATIVE_TRANSLATION_H

#include <functional>

#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/function_symbol.h"

namespace mcrl2
{
namespace smt
{

typedef std::function<std::string(data::data_expression)> native_translation_t;
typedef std::map<data::function_symbol, native_translation_t> native_translation_map_t;
struct native_translations
{
  native_translation_map_t symbols;
  native_translation_map_t expressions;
  std::map<data::function_symbol, data::data_equation> mappings;
  std::map<data::sort_expression, std::string> sorts;

  native_translations(native_translation_map_t s,
                      native_translation_map_t e,
                      std::map<data::function_symbol, data::data_equation> m,
                      std::map<data::sort_expression, std::string> so
                     )
  : symbols(std::move(s))
  , expressions(std::move(e))
  , mappings(std::move(m))
  , sorts(so)
  {}
};

namespace detail {

struct fixed_string_translation: public native_translation_t
{
  std::string translation;

  fixed_string_translation(const std::string& s)
  : translation(s)
  {}

  std::string operator()(const data::data_expression& /*expr*/) const
  {
    return translation;
  }
};

static native_translation_t pp_translation = [](const data::data_expression& e){ return data::pp(e); };
static native_translation_t pp_real_translation = [](const data::data_expression& e)
{
  assert(data::sort_real::is_creal_application(e));
  const data::application& a = atermpp::down_cast<data::application>(e);
  return "(/ " +  data::pp(a[0]) + ".0  " + data::pp(a[1]) + ".0)";
};

} // namespace detail

inline
native_translations initialise_native_translation(const data::data_specification& dataspec)
{
  using namespace detail;
  native_translation_map_t symbols;
  native_translation_map_t expressions;
  std::map<data::function_symbol, data::data_equation> mappings;
  std::map<data::sort_expression, std::string> sorts;

  sorts[data::sort_bool::bool_()] = "Bool";
  sorts[data::sort_pos::pos()] = "Int";
  sorts[data::sort_nat::nat()] = "Int";
  sorts[data::sort_int::int_()] = "Int";
  sorts[data::sort_real::real_()] = "Real";

  for(const data::sort_expression& sort: dataspec.sorts())
  {
    if(data::is_basic_sort(sort))
    {
      symbols[data::equal_to(sort)] = fixed_string_translation("=");
      symbols[data::not_equal_to(sort)] = fixed_string_translation("distinct");
      auto find_result = sorts.find(sort);
      if(find_result != sorts.end() && find_result->second == "Int")
      {
        symbols[data::less(sort)] = pp_translation;
        symbols[data::less_equal(sort)] = pp_translation;
        symbols[data::greater(sort)] = pp_translation;
        symbols[data::greater_equal(sort)] = pp_translation;
      }
      else
      {
        symbols[data::less(sort)] = fixed_string_translation("@less");
        symbols[data::less_equal(sort)] = fixed_string_translation("@less_equal");
        symbols[data::greater(sort)] = fixed_string_translation("@greater");
        symbols[data::greater_equal(sort)] = fixed_string_translation("@greater_equal");
        mappings[data::less(sort)] =
        mappings[data::less_equal(sort)] =
        mappings[data::greater(sort)] =
        mappings[data::greater_equal(sort)] =
      }
      symbols[data::if_(sort)] = fixed_string_translation("ite");

      symbols[data::sort_list::empty(sort)] = fixed_string_translation("nil");
      symbols[data::sort_list::cons_(sort)] = fixed_string_translation("insert");
      symbols[data::sort_list::head(sort)] = pp_translation;
      symbols[data::sort_list::tail(sort)] = pp_translation;
      sorts[data::sort_list::list(sort)] = "(List " + pp(s) + ")";
    }
  }
  symbols[data::sort_bool::not_()] = fixed_string_translation("not");
  symbols[data::sort_bool::and_()] = fixed_string_translation("and");
  symbols[data::sort_bool::or_()] = fixed_string_translation("or");
  symbols[data::sort_bool::implies()] = pp_translation;

  symbols[data::sort_pos::c1()] = pp_translation;
  expressions[data::sort_pos::cdub()] = pp_translation;
  symbols[data::sort_nat::c0()] = pp_translation;
  expressions[data::sort_nat::cnat()] = pp_translation;
  expressions[data::sort_int::cneg()] = pp_translation;
  expressions[data::sort_int::cint()] = pp_translation;
  expressions[data::sort_real::creal()] = pp_real_translation;

  symbols[data::sort_nat::pos2nat()] = fixed_string_translation("@id");
  symbols[data::sort_nat::nat2pos()] = fixed_string_translation("@id");
  symbols[data::sort_int::pos2int()] = fixed_string_translation("@id");
  symbols[data::sort_int::int2pos()] = fixed_string_translation("@id");
  symbols[data::sort_int::nat2int()] = fixed_string_translation("@id");
  symbols[data::sort_int::int2nat()] = fixed_string_translation("@id");
  symbols[data::sort_real::pos2real()] = fixed_string_translation("to_real");
  symbols[data::sort_real::real2pos()] = fixed_string_translation("to_int");
  symbols[data::sort_real::nat2real()] = fixed_string_translation("to_real");
  symbols[data::sort_real::real2nat()] = fixed_string_translation("to_int");
  symbols[data::sort_real::int2real()] = fixed_string_translation("to_real");
  symbols[data::sort_real::real2int()] = fixed_string_translation("to_int");

  data::function_symbol id_int("@id", data::function_sort({data::sort_int::int_()}, data::sort_int::int_()));
  data::function_symbol id_real("@id", data::function_sort({data::sort_real::real_()}, data::sort_real::real_()));
  data::variable vi("i", data::sort_int::int_());
  data::variable vr("r", data::sort_real::real_());
  mappings[id_int] = data::data_equation(data::variable_list({vi}), vi, vi);
  mappings[id_real] = data::data_equation(data::variable_list({vr}), vr, vr);


  return native_translations(symbols, expressions, mappings, sorts);
}

inline
bool has_native_translation(const data::function_symbol& f, const native_translations& nt)
{
  return nt.symbols.find(f) != nt.symbols.end() || nt.expressions.find(f) != nt.expressions.end();
}

inline
bool has_native_translation(const data::data_equation& eq, const native_translations& nt)
{
  const data::data_expression& lhs = eq.lhs();
  if(data::is_function_symbol(lhs))
  {
    return has_native_translation(atermpp::down_cast<data::function_symbol>(lhs), nt);
  }
  else if(data::is_application(lhs) && data::is_function_symbol(atermpp::down_cast<data::application>(lhs).head()))
  {
    return has_native_translation(atermpp::down_cast<data::function_symbol>(atermpp::down_cast<data::application>(lhs).head()), nt);
  }
  else
  {
    return false;
  }
}

} // namespace smt
} // namespace mcrl2

#endif
