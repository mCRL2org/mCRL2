// Author(s): Jan Friso Groote. Based on pbes/replace_capture_avoiding_with_an_identifier_generator.h
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/replace_capture_avoiding_with_an_identifier_generator.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_REPLACE_CAPTURE_AVOIDING_WITH_AN_IDENTIFIER_GENERATOR_H
#define MCRL2_PRES_REPLACE_CAPTURE_AVOIDING_WITH_AN_IDENTIFIER_GENERATOR_H

#include "mcrl2/data/replace_capture_avoiding_with_an_identifier_generator.h"
#include "mcrl2/pres/add_binding.h"
#include "mcrl2/pres/builder.h"

namespace mcrl2::pres_system {

namespace detail {

// Below the code for the capture avoiding replacement with an identifier generator is provided.

template<template<class> class Builder, class Derived, class Substitution, class IdentifierGenerator>
struct add_capture_avoiding_replacement_with_an_identifier_generator
  : public data::detail::add_capture_avoiding_replacement_with_an_identifier_generator<Builder, Derived, Substitution, IdentifierGenerator>
{
  typedef data::detail::add_capture_avoiding_replacement_with_an_identifier_generator <Builder, Derived, Substitution, IdentifierGenerator> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
  using super::update_sigma;

  template <class T>
  void apply(T& result, const infimum& x)
  {
    data::variable_list v = update_sigma.push(x.variables());
    pres_expression body;
    apply(body, x.body());
    make_infimum(result, v, apply(x.body()));
    update_sigma.pop(v);
  }

  template <class T>
  void apply(T& result, const supremum& x)
  {
    data::variable_list v = update_sigma.push(x.variables());
    pres_expression body;
    apply(body, x.body());
    make_supremum(result, v, apply(x.body()));
    update_sigma.pop(v);
  }

  template <class T>
  void apply(T& result, const sum& x)
  {
    data::variable_list v = update_sigma.push(x.variables());
    pres_expression body;
    apply(body, x.body());
    make_sum(result, v, apply(x.body()));
    update_sigma.pop(v);
  }

  void update(pres_equation& x)
  {
    data::variable_list v = update_sigma.push(x.variable().parameters());
    x.variable() = propositional_variable(x.variable().name(), v);
    pres_expression formula;
    apply(formula, x.formula());
    x.formula() = formula;
    update_sigma.pop(v);
  }

  void update(pres& x)
  {
    std::set<data::variable> v = update_sigma(x.global_variables());
    x.global_variables() = v;
    update(x.equations());
    update_sigma.pop(v);
  }

  add_capture_avoiding_replacement_with_an_identifier_generator(Substitution& sigma, IdentifierGenerator& id_generator)
    : super(sigma, id_generator)
  {
  }
};

} // namespace detail

//--- start generated pres_system replace_capture_avoiding_with_identifier_generator code ---//
/// \\brief Applies sigma as a capture avoiding substitution to x using an identifier generator.
/// \\details This substitution function is much faster than replace_variables_capture_avoiding, but
///          it requires an identifier generator that generates strings for fresh variables. These
///          strings must be unique in the sense that they have not been used for other variables.
/// \\param x The object to which the subsitution is applied.
/// \\param sigma A mutable substitution of which it can efficiently be checked whether a variable occurs in its
///              right hand side. The class maintain_variables_in_rhs is useful for this purpose.
/// \\param id_generator A generator that generates unique strings, not yet used as variable names.

template <typename T, typename Substitution, typename IdentifierGenerator>
void replace_variables_capture_avoiding_with_an_identifier_generator(T& x,
                       Substitution& sigma,
                       IdentifierGenerator& id_generator
                      )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  data::detail::apply_replace_capture_avoiding_variables_builder_with_an_identifier_generator<pres_system::data_expression_builder, pres_system::detail::add_capture_avoiding_replacement_with_an_identifier_generator>(sigma, id_generator).update(x);
}

/// \\brief Applies sigma as a capture avoiding substitution to x using an identifier generator..
/// \\details This substitution function is much faster than replace_variables_capture_avoiding, but
///          it requires an identifier generator that generates strings for fresh variables. These
///          strings must be unique in the sense that they have not been used for other variables.
/// \\param x The object to which the substiution is applied.
/// \\param sigma A mutable substitution of which it can efficiently be checked whether a variable occurs in its
///              right hand side. The class maintain_variables_in_rhs is useful for this purpose.
/// \\param id_generator A generator that generates unique strings, not yet used as variable names.
/// \\return The result is the term x to which sigma has been applied.
template <typename T, typename Substitution, typename IdentifierGenerator>
T replace_variables_capture_avoiding_with_an_identifier_generator(const T& x,
                    Substitution& sigma,
                    IdentifierGenerator& id_generator
                   )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  T result;
  data::detail::apply_replace_capture_avoiding_variables_builder_with_an_identifier_generator<pres_system::data_expression_builder, pres_system::detail::add_capture_avoiding_replacement_with_an_identifier_generator>(sigma, id_generator).apply(result, x);
  return result;
}
//--- end generated pres_system replace_capture_avoiding_with_identifier_generator code ---//

} // namespace mcrl2::pres_system



#endif // MCRL2_PRES_REPLACE_CAPTURE_AVOIDING_WITH_AN_IDENTIFIER_GENERATOR_H
