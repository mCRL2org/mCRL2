// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/replace_capture_avoiding_with_an_identifier_generator.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_REPLACE_CAPTURE_AVOIDING_WITH_AN_IDENTIFIER_GENERATOR_H
#define MCRL2_LPS_REPLACE_CAPTURE_AVOIDING_WITH_AN_IDENTIFIER_GENERATOR_H

#include "mcrl2/lps/add_binding.h"
#include "mcrl2/lps/builder.h"
#include "mcrl2/process/replace_capture_avoiding_with_an_identifier_generator.h"

namespace mcrl2::lps
{

namespace detail {

// Below code for capture avoiding subsitutions with an identifier generator are provided.

template<template<class> class Builder, class Derived, class Substitution, class IdentifierGenerator>
struct add_capture_avoiding_replacement_with_an_identifier_generator
  : public process::detail::add_capture_avoiding_replacement_with_an_identifier_generator<Builder, Derived, Substitution, IdentifierGenerator>
{
  using super = process::detail::add_capture_avoiding_replacement_with_an_identifier_generator<Builder,
      Derived,
      Substitution,
      IdentifierGenerator>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;
  using super::update_sigma;

  add_capture_avoiding_replacement_with_an_identifier_generator(Substitution& sigma, IdentifierGenerator& id_generator)
    : super(sigma, id_generator)
  {
  }

  template<typename ActionSummand>
  void do_action_summand(ActionSummand& x, const data::variable_list& v)
  {
    x.summation_variables() = v;
    x.condition() = apply(x.condition());
    x.multi_action() = apply(x.multi_action());
    x.assignments() = apply(x.assignments());
  }

  void update(action_summand& x)
  {
    data::variable_list v = update_sigma.push(x.summation_variables());
    do_action_summand(x, v);
    update_sigma.pop(v);
  }

  void update(stochastic_action_summand& x)
  {
    data::variable_list v = update_sigma.push(x.summation_variables());
    do_action_summand(x, v);
    x.distribution() = apply(x.distribution());
    update_sigma.pop(v);
  }

  void update(deadlock_summand& x)
  {
    data::variable_list v = update_sigma.push(x.summation_variables());
    x.summation_variables() = v;
    x.condition() = apply(x.condition());
    update(x.deadlock());
    update_sigma.pop(v);
  }

  template<typename LinearProcess>
  void do_linear_process(LinearProcess& x)
  {
    data::variable_list v = update_sigma.push(x.process_parameters());
    x.process_parameters() = v;
    update(x.action_summands());
    update(x.deadlock_summands());
    update_sigma.pop(v);
  }

  void update(linear_process& x)
  {
    do_linear_process(x);
  }

  void update(stochastic_linear_process& x)
  {
    do_linear_process(x);
  }

  template<typename Specification>
  void do_specification(Specification& x)
  {
    std::set<data::variable> v = update_sigma(x.global_variables());
    x.global_variables() = v;
    update(x.process());
    x.action_labels() = apply(x.action_labels());
    x.initial_process() = apply(x.initial_process());
    update_sigma.pop(v);
  }

  void operator()(specification& x)
  {
    do_specification(x);
  }

  void operator()(stochastic_specification& x)
  {
    do_specification(x);
  }

  stochastic_distribution apply(stochastic_distribution& x)
  {
    data::variable_list v = update_sigma.push(x.variables());
    stochastic_distribution result(x.variables(), apply(x.distribution()));
    update_sigma.pop(v);
    return result;
  }
};

} // namespace detail

//--- start generated lps replace_capture_avoiding_with_identifier_generator code ---//
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
  data::detail::apply_replace_capture_avoiding_variables_builder_with_an_identifier_generator<lps::data_expression_builder, lps::detail::add_capture_avoiding_replacement_with_an_identifier_generator>(sigma, id_generator).update(x);
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
  data::detail::apply_replace_capture_avoiding_variables_builder_with_an_identifier_generator<lps::data_expression_builder, lps::detail::add_capture_avoiding_replacement_with_an_identifier_generator>(sigma, id_generator).apply(result, x);
  return result;
}
//--- end generated lps replace_capture_avoiding_with_identifier_generator code ---//

} // namespace mcrl2::lps

#endif // MCRL2_LPS_REPLACE_CAPTURE_AVOIDING_WITH_AN_IDENTIFIER_GENERATOR_H
