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
  using namespace data;
  native_translations nt;

  nt.sorts[sort_bool::bool_()] = "Bool";
  nt.sorts[sort_pos::pos()] = "Int";
  nt.sorts[sort_nat::nat()] = "Int";
  nt.sorts[sort_int::int_()] = "Int";
  nt.sorts[sort_real::real_()] = "Real";

  std::set<sort_expression> sorts = dataspec.sorts();
  std::vector<sort_expression> number_sorts({ sort_pos::pos(), sort_nat::nat(), sort_int::int_(), sort_real::real_() });
  for(const sort_expression& sort: sorts)
  {
    if(is_basic_sort(sort))
    {
      nt.set_native_definition(equal_to(sort), "=");
      nt.set_native_definition(not_equal_to(sort), "distinct");
      if(std::find(number_sorts.begin(), number_sorts.end(), sort) != number_sorts.end())
      {
        nt.set_native_definition(less(sort));
        nt.set_native_definition(less_equal(sort));
        nt.set_native_definition(greater(sort));
        nt.set_native_definition(greater_equal(sort));
        nt.set_alternative_name(sort_real::exp(sort, sort == sort_real::real_() ? sort_int::int_() : sort_nat::nat()), "^");
        nt.set_alternative_name(sort_real::minus(sort, sort), "-");
        nt.set_alternative_name(sort_real::negate(sort), "-");
      }
      else if(sort == basic_sort("@NatPair"))
      {
        // NatPair is not used and its equations upset Z3
        nt.set_native_definition(less(sort));
        nt.set_native_definition(less_equal(sort));
        nt.set_native_definition(greater(sort));
        nt.set_native_definition(greater_equal(sort));
      }
      else
      {
        // Functions <, <=, >, >= are already defined on Int/Real and cannot be overloaded
        nt.set_alternative_name(less(sort), "@less");
        nt.set_alternative_name(less_equal(sort), "@less_equal");
        nt.set_alternative_name(greater(sort), "@greater");
        nt.set_alternative_name(greater_equal(sort), "@greater_equal");
      }
      nt.set_native_definition(if_(sort), "ite");

      nt.set_native_definition(sort_list::empty(sort), "nil");
      nt.set_native_definition(sort_list::cons_(sort), "insert");
      nt.set_native_definition(sort_list::head(sort));
      nt.set_native_definition(sort_list::tail(sort));
      nt.sorts[sort_list::list(sort)] = "(List " + (nt.sorts.find(sort) != nt.sorts.end() ? nt.sorts.find(sort)->second : pp(sort)) + ")";
    }
  }

  std::vector<function_symbol_vector> fs_numbers(
    {
      sort_pos::pos_generate_functions_code(),
      sort_nat::nat_generate_functions_code(),
      sort_int::int_generate_functions_code(),
      sort_real::real_generate_functions_code()
    }
  );
  for(const auto& fsv: fs_numbers)
  {
    for(const auto& fs: fsv)
    {
      nt.set_native_definition(fs);
    }
  }

  auto one = [](const sort_expression& s)
  {
    return s == sort_int::int_() ? sort_int::int_(1) : sort_real::real_(1);
  };
  for(const sort_expression& s: { sort_int::int_(), sort_real::real_() })
  {
    variable v1("v1", s);
    variable v2("v2", s);
    variable_list l1({v1});
    variable_list l2({v1,v2});
    nt.mappings[sort_real::minimum(s,s)] = data_equation(l2, sort_real::minimum(v1, v2), if_(less(v1,v2), v1, v2));
    nt.mappings[sort_real::maximum(s,s)] = data_equation(l2, sort_real::maximum(v1, v2), if_(less(v1,v2), v2, v1));
    nt.mappings[sort_real::succ(s)] = data_equation(l1, sort_real::succ(v1), sort_real::plus(v1, one(s)));
    nt.mappings[sort_real::pred(s)] = data_equation(l1, sort_real::pred(v1), sort_real::minus(v1, one(s)));
  }
  // TODO come up with equations for these
  // nt.mappings[sort_real::floor(s)]
  // nt.mappings[sort_real::ceil(s)]
  // nt.mappings[sort_real::round(s)]

  nt.set_native_definition(sort_bool::not_(), "not");
  nt.set_native_definition(sort_bool::and_(), "and");
  nt.set_native_definition(sort_bool::or_(), "or");
  nt.set_native_definition(sort_bool::implies());

  nt.set_native_definition(sort_pos::c1(), pp(sort_pos::c1()));
  nt.set_native_definition(sort_nat::c0(), pp(sort_nat::c0()));
  nt.expressions[sort_pos::cdub()] = pp_translation;
  nt.expressions[sort_nat::cnat()] = pp_translation;
  nt.expressions[sort_int::cneg()] = pp_translation;
  nt.expressions[sort_int::cint()] = pp_translation;
  nt.expressions[sort_real::creal()] = pp_real_translation;
  nt.set_native_definition(sort_real::creal());

  nt.set_native_definition(sort_nat::pos2nat(), "@id");
  nt.set_native_definition(sort_nat::nat2pos(), "@id");
  nt.set_native_definition(sort_int::pos2int(), "@id");
  nt.set_native_definition(sort_int::int2pos(), "@id");
  nt.set_native_definition(sort_int::nat2int(), "@id");
  nt.set_native_definition(sort_int::int2nat(), "@id");
  nt.set_native_definition(sort_real::pos2real(), "to_real");
  nt.set_native_definition(sort_real::real2pos(), "to_int");
  nt.set_native_definition(sort_real::nat2real(), "to_real");
  nt.set_native_definition(sort_real::real2nat(), "to_int");
  nt.set_native_definition(sort_real::int2real(), "to_real");
  nt.set_native_definition(sort_real::real2int(), "to_int");

  function_symbol id_int("@id", function_sort({sort_int::int_()}, sort_int::int_()));
  function_symbol id_real("@id", function_sort({sort_real::real_()}, sort_real::real_()));
  variable vi("i", sort_int::int_());
  variable vr("r", sort_real::real_());
  nt.mappings[id_int] = data_equation(variable_list({vi}), id_int(vi), vi);
  nt.mappings[id_real] = data_equation(variable_list({vr}), id_real(vr), vr);
  // necessary for translating the two equations above
  nt.set_native_definition(equal_to(sort_int::int_()), "=");
  nt.set_native_definition(equal_to(sort_real::real_()), "=");

  return nt;
}

inline
std::string translate_identifier(const std::string& id)
{
  std::string result = id;
  for(std::size_t i = 0; i < result.size(); i++)
  {
    if(result[i] == '\'')
    {
      result[i] = '!';
    }
  }
  return result;
}

inline
std::string translate_identifier(const core::identifier_string& id)
{
  return translate_identifier(core::pp(id));
}

inline
std::string make_projection_name(const data::function_symbol& cons, std::size_t i)
{
  return "@proj-" + translate_identifier(cons.name()) + "-" + std::to_string(i);
}

inline
data::function_symbol make_projection_func(const data::function_symbol& cons, const data::sort_expression& arg_sort, std::size_t i)
{
  data::function_sort sort(data::sort_expression_list({ cons.sort().target_sort() }), arg_sort);
  return data::function_symbol(make_projection_name(cons, i), sort);
}

inline
std::string make_recogniser_name(const data::function_symbol& cons)
{
  // Z3 automatically generates recognisers "is-constructorname"
  return "is-" + translate_identifier(cons.name());
}

inline
data::function_symbol make_recogniser_func(const data::function_symbol& cons)
{
  data::function_sort sort(data::sort_expression_list({ cons.sort().target_sort() }), data::sort_bool::bool_());
  return data::function_symbol(make_recogniser_name(cons), sort);
}

} // namespace smt
} // namespace mcrl2

#endif
