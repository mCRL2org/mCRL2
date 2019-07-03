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
struct native_translations
{
  // If f occurs in symbols, its translation should be symbols[f]
  std::map<data::function_symbol, std::string> symbols;
  // If f occurs in expressions, all applications f(a) should be translated by the function expressions[f]
  std::map<data::function_symbol, native_translation_t> expressions;
  // Function symbols for which the equations should not be defined
  std::set<data::function_symbol> do_not_define;
  // Pairs of mappings and equations that need to be translated as well
  std::map<data::function_symbol, data::data_equation> mappings;
  // Sorts that have a native definition in Z3
  std::map<data::sort_expression, std::string> sorts;

  native_translations() = default;

  // native_translations(native_translation_map_t s,
  //                     native_translation_map_t e,
  //                     std::map<data::function_symbol, data::data_equation> m,
  //                     std::map<data::sort_expression, std::string> so
  //                    )
  // : symbols(std::move(s))
  // , expressions(std::move(e))
  // , mappings(std::move(m))
  // , sorts(std::move(so))
  // {}

  std::map<data::function_symbol, native_translation_t>::const_iterator find_native_translation(const data::application& a) const
  {
    if(data::is_function_symbol(a.head()))
    {
      auto& f = atermpp::down_cast<data::function_symbol>(a.head());
      return expressions.find(f);
    }
    return expressions.end();
  }

  bool has_native_definition(const data::function_symbol& f) const
  {
    return do_not_define.find(f) != do_not_define.end();
  }

  bool has_native_definition(const data::application& a) const
  {
    if(data::is_function_symbol(a.head()))
    {
      auto& f = atermpp::down_cast<data::function_symbol>(a.head());
      return has_native_definition(f);
    }
    return false;
  }

  bool has_native_definition(const data::data_equation& eq) const
  {
    const data::data_expression& lhs = eq.lhs();
    if(data::is_function_symbol(lhs))
    {
      return has_native_definition(atermpp::down_cast<data::function_symbol>(lhs));
    }
    else if(data::is_application(lhs) && data::is_function_symbol(atermpp::down_cast<data::application>(lhs).head()))
    {
      return has_native_definition(atermpp::down_cast<data::function_symbol>(atermpp::down_cast<data::application>(lhs).head()));
    }
    else
    {
      return false;
    }
  }

  /**
   * \brief Record that the mapping and equations for f should not be translated
   * \details This translation is either not desired because there is a native Z3
   * counterpart, or because the function symbol is only used internally.
   */
  void set_native_definition(const data::function_symbol& f)
  {
    do_not_define.insert(f);
  }

  void set_native_definition(const data::function_symbol& f, const std::string& s)
  {
    symbols[f] = s;
    do_not_define.insert(f);
  }

  void set_alternative_name(const data::function_symbol& f, const std::string& s)
  {
    symbols[f] = s;
  }
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
  native_translations nt;

  nt.sorts[data::sort_bool::bool_()] = "Bool";
  nt.sorts[data::sort_pos::pos()] = "Int";
  nt.sorts[data::sort_nat::nat()] = "Int";
  nt.sorts[data::sort_int::int_()] = "Int";
  nt.sorts[data::sort_real::real_()] = "Real";

  for(const data::sort_expression& sort: dataspec.sorts())
  {
    if(data::is_basic_sort(sort))
    {
      nt.set_native_definition(data::equal_to(sort), "=");
      nt.set_native_definition(data::not_equal_to(sort), "distinct");
      auto find_result = nt.sorts.find(sort);
      if(find_result != nt.sorts.end() && find_result->second == "Int")
      {
        // Functions <, <=, >, >= are already defined on Int and cannot be overloaded
        nt.set_native_definition(data::less(sort));
        nt.set_native_definition(data::less_equal(sort));
        nt.set_native_definition(data::greater(sort));
        nt.set_native_definition(data::greater_equal(sort));
      }
      else
      {
        nt.set_alternative_name(data::less(sort), "@less");
        nt.set_alternative_name(data::less_equal(sort), "@less_equal");
        nt.set_alternative_name(data::greater(sort), "@greater");
        nt.set_alternative_name(data::greater_equal(sort), "@greater_equal");
      }
      nt.set_native_definition(data::if_(sort), "ite");

      nt.set_native_definition(data::sort_list::empty(sort), "nil");
      nt.set_native_definition(data::sort_list::cons_(sort), "insert");
      nt.set_native_definition(data::sort_list::head(sort));
      nt.set_native_definition(data::sort_list::tail(sort));
      nt.sorts[data::sort_list::list(sort)] = "(List " + pp(sort) + ")";
    }
  }
  nt.set_native_definition(data::sort_bool::not_(), "not");
  nt.set_native_definition(data::sort_bool::and_(), "and");
  nt.set_native_definition(data::sort_bool::or_(), "or");
  nt.set_native_definition(data::sort_bool::implies());

  nt.set_native_definition(data::sort_pos::c1(), pp(data::sort_pos::c1()));
  nt.set_native_definition(data::sort_nat::c0(), pp(data::sort_nat::c0()));
  nt.expressions[data::sort_pos::cdub()] = pp_translation;
  nt.expressions[data::sort_nat::cnat()] = pp_translation;
  nt.expressions[data::sort_int::cneg()] = pp_translation;
  nt.expressions[data::sort_int::cint()] = pp_translation;
  nt.expressions[data::sort_real::creal()] = pp_real_translation;

  nt.set_native_definition(data::sort_pos::plus());
  nt.set_native_definition(data::sort_pos::times());

  nt.set_native_definition(data::sort_nat::pos2nat(), "@id");
  nt.set_native_definition(data::sort_nat::nat2pos(), "@id");
  nt.set_native_definition(data::sort_int::pos2int(), "@id");
  nt.set_native_definition(data::sort_int::int2pos(), "@id");
  nt.set_native_definition(data::sort_int::nat2int(), "@id");
  nt.set_native_definition(data::sort_int::int2nat(), "@id");
  nt.set_native_definition(data::sort_real::pos2real(), "to_real");
  nt.set_native_definition(data::sort_real::real2pos(), "to_int");
  nt.set_native_definition(data::sort_real::nat2real(), "to_real");
  nt.set_native_definition(data::sort_real::real2nat(), "to_int");
  nt.set_native_definition(data::sort_real::int2real(), "to_real");
  nt.set_native_definition(data::sort_real::real2int(), "to_int");

  data::function_symbol id_int("@id", data::function_sort({data::sort_int::int_()}, data::sort_int::int_()));
  data::function_symbol id_real("@id", data::function_sort({data::sort_real::real_()}, data::sort_real::real_()));
  data::variable vi("i", data::sort_int::int_());
  data::variable vr("r", data::sort_real::real_());
  nt.mappings[id_int] = data::data_equation(data::variable_list({vi}), vi, vi);
  nt.mappings[id_real] = data::data_equation(data::variable_list({vr}), vr, vr);

  return nt;
}


} // namespace smt
} // namespace mcrl2

#endif
