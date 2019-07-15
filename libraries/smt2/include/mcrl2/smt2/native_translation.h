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
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/sort_expression.h"

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
  // Function symbols that are overloaded
  std::set<data::function_symbol> ambiguous_symbols;

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

  bool is_ambiguous(const data::function_symbol& f) const
  {
    return ambiguous_symbols.find(f) != ambiguous_symbols.end();
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

  void set_native_definition(const data::function_symbol& f, const data::data_equation& eq)
  {
    do_not_define.insert(f);
    mappings[f] = eq;
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

  void set_ambiguous(const data::function_symbol& f)
  {
    ambiguous_symbols.insert(f);
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
native_translations initialise_native_translation(const data::data_specification& dataspec);

} // namespace smt
} // namespace mcrl2

#endif
