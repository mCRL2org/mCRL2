// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/anonimyze.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_ANONIMYZE_H
#define MCRL2_DATA_ANONIMYZE_H

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/builder.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2 {

namespace data {

namespace detail {

template <typename Derived>
struct anonimyze_builder: public data::sort_expression_builder<Derived>
{
  typedef data::sort_expression_builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;

  data::set_identifier_generator id_generator;
  std::map<core::identifier_string, core::identifier_string> sort_name_substitution;
  std::map<core::identifier_string, core::identifier_string> variable_name_substitution;
  std::map<core::identifier_string, core::identifier_string> function_symbol_name_substitution;

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  void add_name(const core::identifier_string& name, std::map<core::identifier_string, core::identifier_string>& substitution, const std::string& hint)
  {
    using utilities::detail::has_key;
    if (!has_key(substitution, name))
    {
      substitution[name] = id_generator(hint);
    }
  }

  data_specification make_data_specification(
    const basic_sort_vector& user_defined_sorts,
    const function_symbol_vector& user_defined_constructors,
    const function_symbol_vector& user_defined_mappings,
    const alias_vector& user_defined_aliases,
    const data_equation_vector& user_defined_equations
  )
  {
    data_specification result;
    for (const auto& x: user_defined_sorts) { result.add_sort(x); }
    for (const auto& x: user_defined_constructors) { result.add_constructor(x); }
    for (const auto& x: user_defined_mappings) { result.add_mapping(x); }
    for (const auto& x: user_defined_aliases) { result.add_alias(x); }
    for (const auto& x: user_defined_equations) { result.add_equation(x); }
    return result;
  }

  void update(data_specification& x)
  {
    std::set<sort_expression> system_defined_sorts;
    std::set<function_symbol> system_defined_constructors;
    std::set<function_symbol> system_defined_mappings;
    x.get_system_defined_sorts_constructors_and_mappings(system_defined_sorts, system_defined_constructors, system_defined_mappings);

    // prevent system defined sorts from being renamed
    for (const auto& s: system_defined_sorts)
    {
      if (is_basic_sort(s))
      {
        const auto& name = atermpp::down_cast<basic_sort>(s).name();
        sort_name_substitution[name] = name;
      }
    }

    // prevent system defined functions from being renamed
    for (const auto& f: system_defined_constructors)
    {
      function_symbol_name_substitution[f.name()] = f.name();
    }
    for (const auto& f: system_defined_mappings)
    {
      function_symbol_name_substitution[f.name()] = f.name();
    }

    auto user_defined_sorts        = x.user_defined_sorts();
    auto user_defined_constructors = x.user_defined_constructors();
    auto user_defined_mappings     = x.user_defined_mappings();
    auto user_defined_aliases      = x.user_defined_aliases();
    auto user_defined_equations    = x.user_defined_equations();

    derived().update(user_defined_sorts       );
    derived().update(user_defined_constructors);
    derived().update(user_defined_mappings    );
    derived().update(user_defined_aliases     );
    derived().update(user_defined_equations   );
    x = make_data_specification(
            user_defined_sorts       ,
            user_defined_constructors,
            user_defined_mappings    ,
            user_defined_aliases     ,
            user_defined_equations
         );
  }

  void enter(const variable& x)
  {
    add_name(x.name(), variable_name_substitution, "v");
  }

  variable apply(const variable& x)
  {
    derived().enter(x);
    variable result(variable_name_substitution[x.name()], derived().apply(x.sort()));
    derived().leave(x);
    return result;
  }

  void enter(const function_symbol& x)
  {
    add_name(x.name(), function_symbol_name_substitution, "f");
  }

  function_symbol apply(const function_symbol& x)
  {
    derived().enter(x);
    function_symbol result(function_symbol_name_substitution[x.name()], derived().apply(x.sort()));
    derived().leave(x);
    return result;
  }

  void enter(const basic_sort& x)
  {
    add_name(x.name(), sort_name_substitution, "s");
  }

  basic_sort apply(const basic_sort& x)
  {
    derived().enter(x);
    basic_sort result(sort_name_substitution[x.name()]);
    derived().leave(x);
    return result;
  }

  void enter(const structured_sort_constructor& x)
  {
    add_name(x.name(), function_symbol_name_substitution, "f");
    if (x.recogniser() != atermpp::empty_string())
    {
      add_name(x.recogniser(), function_symbol_name_substitution, "f");
    }
  }

  structured_sort_constructor apply(const structured_sort_constructor& x)
  {
    derived().enter(x);
    structured_sort_constructor result(
          function_symbol_name_substitution[x.name()],
          derived().apply(x.arguments()),
          x.recogniser() == atermpp::empty_string() ? x.recogniser() : function_symbol_name_substitution[x.recogniser()]
        );
    derived().leave(x);
    return result;
  }

  void enter(const structured_sort_constructor_argument& x)
  {
    add_name(x.name(), function_symbol_name_substitution, "f");
  }

  structured_sort_constructor_argument apply(const structured_sort_constructor_argument& x)
  {
    derived().enter(x);
    structured_sort_constructor_argument result(function_symbol_name_substitution[x.name()], derived().apply(x.sort()));
    derived().leave(x);
    return result;
  }

  alias apply(const alias& x)
  {
    derived().enter(x);
    alias result(derived().apply(x.name()), derived().apply(x.reference()));
    derived().leave(x);
    return result;
  }
};

struct anonimyze_builder_instance: public anonimyze_builder<anonimyze_builder_instance>
{
};

} // namespace detail

inline
void anonimyze(data_specification& dataspec)
{
  detail::anonimyze_builder_instance f;
  f.update(dataspec);
  std::cout << dataspec << std::endl;
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_ANONIMYZE_H
