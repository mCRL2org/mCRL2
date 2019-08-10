// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/replace_capture_avoiding.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_REPLACE_CAPTURE_AVOIDING_H
#define MCRL2_LPS_REPLACE_CAPTURE_AVOIDING_H

#include "mcrl2/lps/builder.h"
#include "mcrl2/lps/find.h"
#include "mcrl2/process/replace_capture_avoiding.h"

namespace mcrl2 {

namespace lps {

namespace detail {

template <template <class> class Builder, class Derived, class Substitution>
struct add_capture_avoiding_replacement: public process::detail::add_capture_avoiding_replacement<Builder, Derived, Substitution>
{
  typedef process::detail::add_capture_avoiding_replacement<Builder, Derived, Substitution> super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;
  using super::sigma;

  explicit add_capture_avoiding_replacement(data::detail::capture_avoiding_substitution_updater<Substitution>& sigma)
    : super(sigma)
  { }

  template <typename ActionSummand>
  void do_action_summand(ActionSummand& x, const data::variable_list& v)
  {
    x.summation_variables() = v;
    x.condition() = apply(x.condition());
    apply(x.multi_action());
    x.assignments() = apply(x.assignments());
  }

  void apply(action_summand& x)
  {
    data::variable_list v1 = sigma.add_fresh_variable_assignments(x.summation_variables());
    do_action_summand(x, v1);
    sigma.remove_fresh_variable_assignments(x.summation_variables());
  }

  void apply(stochastic_action_summand& x)
  {
    data::variable_list v1 = sigma.add_fresh_variable_assignments(x.summation_variables());
    do_action_summand(x, v1);
    x.distribution() = apply(x.distribution());
    sigma.remove_fresh_variable_assignments(x.summation_variables());
  }

  void apply(deadlock_summand& x)
  {
    data::variable_list v1 = sigma.add_fresh_variable_assignments(x.summation_variables());
    x.summation_variables() = v1;
    x.condition() = apply(x.condition());
    apply(x.deadlock());
    sigma.remove_fresh_variable_assignments(x.summation_variables());
  }

  template <typename LinearProcess>
  void do_linear_process(const LinearProcess& x)
  {
    data::variable_list v1 = sigma.add_fresh_variable_assignments(x.summation_variables());
    x.process_parameters() = v1;
    apply(x.action_summands());
    apply(x.deadlock_summands());
    sigma.remove_fresh_variable_assignments(x.summation_variables());
  }

  void apply(linear_process& x)
  {
    do_linear_process(x);
  }

  void apply(stochastic_linear_process& x)
  {
    do_linear_process(x);
  }

  template <typename Specification>
  void do_specification(Specification& x)
  {
    data::variable_list v1 = sigma.add_fresh_variable_assignments(x.global_variables());
    x.global_variables() = std::set<data::variable>(v1.begin(), v1.end());
    apply(x.process());
    x.action_labels() = apply(x.action_labels());
    x.initial_process() = apply(x.initial_process());
    sigma.remove_fresh_variable_assignments(x.global_variables());
  }

  void operator()(specification& x)
  {
    do_specification(x);
  }

  void operator()(stochastic_specification& x)
  {
    do_specification(x);
  }

  stochastic_distribution operator()(stochastic_distribution& x)
  {
    data::variable_list v1 = sigma.add_fresh_variable_assignments(x.variables());
    stochastic_distribution result(v1, apply(x.distribution()));
    sigma.remove_fresh_variable_assignments(x.variables());
    return result;
  }
};

} // namespace detail

//--- start generated lps replace_capture_avoiding code ---//
/// \brief Applies sigma as a capture avoiding substitution to x.
/// \param x The object to which the subsitution is applied.
/// \param sigma A substitution.
/// \param id_generator An identifier generator that generates names that do not appear in x and sigma
template <typename T, typename Substitution>
void replace_variables_capture_avoiding(T& x,
                                        Substitution& sigma,
                                        data::set_identifier_generator& id_generator,
                                        typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
)
{
  data::detail::capture_avoiding_substitution_updater<Substitution> sigma1(sigma, id_generator);
  data::detail::apply_replace_capture_avoiding_variables_builder<lps::data_expression_builder, lps::detail::add_capture_avoiding_replacement>(sigma1).update(x);
}

/// \brief Applies sigma as a capture avoiding substitution to x.
/// \param x The object to which the substiution is applied.
/// \param sigma A substitution.
/// \param id_generator An identifier generator that generates names that do not appear in x and sigma
template <typename T, typename Substitution>
T replace_variables_capture_avoiding(const T& x,
                                     Substitution& sigma,
                                     data::set_identifier_generator& id_generator,
                                     typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
)
{
  data::detail::capture_avoiding_substitution_updater<Substitution> sigma1(sigma, id_generator);
  return data::detail::apply_replace_capture_avoiding_variables_builder<lps::data_expression_builder, lps::detail::add_capture_avoiding_replacement>(sigma1).apply(x);
}

/// \brief Applies sigma as a capture avoiding substitution to x.
/// \param x The object to which the subsitution is applied.
/// \param sigma A substitution.
template <typename T, typename Substitution>
void replace_variables_capture_avoiding(T& x,
                                        Substitution& sigma,
                                        typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
)
{
  data::set_identifier_generator id_generator;
  id_generator.add_identifiers(lps::find_identifiers(x));
  for (const data::variable& v: substitution_variables(sigma))
  {
    id_generator.add_identifier(v.name());
  }
  lps::replace_variables_capture_avoiding(x, sigma, id_generator);
}

/// \brief Applies sigma as a capture avoiding substitution to x.
/// \param x The object to which the substiution is applied.
/// \param sigma A substitution.
template <typename T, typename Substitution>
T replace_variables_capture_avoiding(const T& x,
                                     Substitution& sigma,
                                     typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
)
{
  data::set_identifier_generator id_generator;
  id_generator.add_identifiers(lps::find_identifiers(x));
  for (const data::variable& v: substitution_variables(sigma))
  {
    id_generator.add_identifier(v.name());
  }
  return lps::replace_variables_capture_avoiding(x, sigma, id_generator);
}
//--- end generated lps replace_capture_avoiding code ---//

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_REPLACE_CAPTURE_AVOIDING_H
