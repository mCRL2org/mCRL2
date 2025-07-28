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

namespace mcrl2::lps
{

namespace detail {

template <template <class> class Builder, class Derived, class Substitution>
struct add_capture_avoiding_replacement: public process::detail::add_capture_avoiding_replacement<Builder, Derived, Substitution>
{
  using super = process::detail::add_capture_avoiding_replacement<Builder, Derived, Substitution>;
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
    data::data_expression condition;
    lps::multi_action multi_action;
    data::assignment_list assignments;

    apply(condition, x.condition());
    apply(multi_action, x.multi_action());
    apply(assignments, x.assignments());

    x.condition() = condition;
    x.multi_action() = multi_action;
    x.assignments() = assignments;
  }

  void update(action_summand& x)
  {
    data::variable_list sumvars = x.summation_variables();
    data::variable_list v1 = sigma.add_fresh_variable_assignments(sumvars);
    do_action_summand(x, v1);
    sigma.remove_fresh_variable_assignments(sumvars);
  }

  void update(stochastic_action_summand& x)
  {
    data::variable_list sumvars = x.summation_variables();
    data::variable_list v1 = sigma.add_fresh_variable_assignments(sumvars);
    do_action_summand(x, v1);

    lps::stochastic_distribution dist;
    apply(dist, x.distribution(), x.assignments());
    x.distribution() = dist;
    sigma.remove_fresh_variable_assignments(sumvars);
  }

  void update(deadlock_summand& x)
  {
    data::variable_list sumvars = x.summation_variables();
    data::variable_list v1 = sigma.add_fresh_variable_assignments(sumvars);
    x.summation_variables() = v1;
    
    data::data_expression condition;
    apply(condition, x.condition());
    x.condition() = condition;

    update(x.deadlock());
    sigma.remove_fresh_variable_assignments(sumvars);
  }

  template <typename LinearProcess>
  void do_linear_process(LinearProcess& x)
  {
    data::variable_list process_params = x.process_parameters();
    data::variable_list v1 = sigma.add_fresh_variable_assignments(process_params);
    x.process_parameters() = v1;
    update(x.action_summands());
    update(x.deadlock_summands());
    sigma.remove_fresh_variable_assignments(process_params);
  }

  void update(linear_process& x)
  {
    do_linear_process(x);
  }

  void update(stochastic_linear_process& x)
  {
    do_linear_process(x);
  }

  template <typename Specification>
  void do_specification(Specification& x)
  {
    data::variable_list global_vars = x.global_variables();
    data::variable_list v1 = sigma.add_fresh_variable_assignments(global_vars);
    x.global_variables() = std::set<data::variable>(v1.begin(), v1.end());

    typename Specification::process_type process;
    process::action_label_list action_labels;
    typename Specification::initial_process_type initial_process;

    apply(process, x.process());
    apply(action_labels, x.action_labels());
    apply(initial_process, x.initial_process());
    x.process() = process;
    x.action_labels() = action_labels;
    x.initial_process() = initial_process;

    sigma.remove_fresh_variable_assignments(global_vars);
  }

  void operator()(specification& x)
  {
    do_specification(x);
  }

  void operator()(stochastic_specification& x)
  {
    do_specification(x);
  }

  template<class T>
  void apply(T& /* result */, const stochastic_distribution& /* x */)
  {
    assert(false); // This function should never be called. If a stochastic distribution is 
                   // changed, the associated parameter list should be changed too. 
  }

  /// In the code below, it is essential that the assignments are also updated. They are passed by reference and changed in place. 
  template<class T>
  void apply(T& result, const stochastic_distribution& x, data::assignment_list& assignments)
  {
    data::variable_list v1 = sigma.add_fresh_variable_assignments(x.variables());

    data::data_expression dist;
    apply(dist, x.distribution());
    data::assignment_list aux_assignments;
    apply(aux_assignments, assignments);
    assignments = aux_assignments;
    result = stochastic_distribution(v1, dist);
    sigma.remove_fresh_variable_assignments(x.variables());
  }

  /// In the code below, it is essential that the assignments are also updated. They are passed by reference and changed in place. 
  template<class T>
  void apply(T& result, const stochastic_distribution& x, data::data_expression_list& pars)
  {
    data::variable_list v1 = sigma.add_fresh_variable_assignments(x.variables());

    data::data_expression dist;
    apply(dist, x.distribution());
    data::data_expression_list aux_pars;
    apply(aux_pars, pars);
    pars = aux_pars;
    result = stochastic_distribution(v1, dist);
    sigma.remove_fresh_variable_assignments(x.variables());
  }
};

} // namespace detail

//--- start generated lps replace_capture_avoiding code ---//
/// \\brief Applies sigma as a capture avoiding substitution to x.
/// \\param x The object to which the subsitution is applied.
/// \\param sigma A substitution.
/// \\param id_generator An identifier generator that generates names that do not appear in x and sigma
template <typename T, typename Substitution>
void replace_variables_capture_avoiding(T& x,
                                        Substitution& sigma,
                                        data::set_identifier_generator& id_generator
)
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  data::detail::capture_avoiding_substitution_updater<Substitution> sigma1(sigma, id_generator);
  data::detail::apply_replace_capture_avoiding_variables_builder<lps::data_expression_builder, lps::detail::add_capture_avoiding_replacement>(sigma1).update(x);
}

/// \\brief Applies sigma as a capture avoiding substitution to x.
/// \\param x The object to which the substiution is applied.
/// \\param sigma A substitution.
/// \\param id_generator An identifier generator that generates names that do not appear in x and sigma
template <typename T, typename Substitution>
T replace_variables_capture_avoiding(const T& x,
                                     Substitution& sigma,
                                     data::set_identifier_generator& id_generator
)
  requires std::is_base_of_v<atermpp::aterm, T>
{
  data::detail::capture_avoiding_substitution_updater<Substitution> sigma1(sigma, id_generator);
  T result;
  data::detail::apply_replace_capture_avoiding_variables_builder<lps::data_expression_builder, lps::detail::add_capture_avoiding_replacement>(sigma1).apply(result, x);
  return result;
}

/// \\brief Applies sigma as a capture avoiding substitution to x.
/// \\param x The object to which the subsitution is applied.
/// \\param sigma A substitution.
template <typename T, typename Substitution>
void replace_variables_capture_avoiding(T& x,
                                        Substitution& sigma
)
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  data::set_identifier_generator id_generator;
  id_generator.add_identifiers(lps::find_identifiers(x));
  for (const data::variable& v: substitution_variables(sigma))
  {
    id_generator.add_identifier(v.name());
  }
  lps::replace_variables_capture_avoiding(x, sigma, id_generator);
}

/// \\brief Applies sigma as a capture avoiding substitution to x.
/// \\param x The object to which the substiution is applied.
/// \\param sigma A substitution.
template <typename T, typename Substitution>
T replace_variables_capture_avoiding(const T& x,
                                     Substitution& sigma
)
  requires std::is_base_of_v<atermpp::aterm, T>
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

/// \\brief Applies sigma as a capture avoiding substitution to x with x a distribution..
/// \\details The capture avoiding substitution must also be applied to the expression to which the distribution is applied.
/// \\param x The object to which the substiution is applied.
/// \\param pars The parameter list to which the distribution is applied. 
/// \\param sigma A substitution.
/// \\param id_generator An identifier generator that generates names that do not appear in x and sigma
template <typename Substitution>
stochastic_distribution replace_variables_capture_avoiding(
                                     const stochastic_distribution& x,
                                     data::data_expression_list& pars,
                                     Substitution& sigma,
                                     data::set_identifier_generator& id_generator
)
{
  data::detail::capture_avoiding_substitution_updater<Substitution> sigma1(sigma, id_generator);
  stochastic_distribution result;
  data::detail::apply_replace_capture_avoiding_variables_builder<lps::data_expression_builder, lps::detail::add_capture_avoiding_replacement>(sigma1).apply(result, x, pars);
  return result;
}

/// \\brief Applies sigma as a capture avoiding substitution to a stochastic_distribution and a list of parameters.
/// \\param x The object to which the substiution is applied.
/// \\param pars The parameters of which the variables are bound. This list is changed if necessary.
/// \\param sigma A substitution.
template <typename Substitution>
stochastic_distribution replace_variables_capture_avoiding(
                                     const stochastic_distribution& x,
                                     data::data_expression_list& pars,
                                     Substitution& sigma
) 
{ 
  data::set_identifier_generator id_generator;
  id_generator.add_identifiers(lps::find_identifiers(x));
  for (const data::variable& v: substitution_variables(sigma))
  { 
    id_generator.add_identifier(v.name());
  } 
  return lps::replace_variables_capture_avoiding(x, pars, sigma, id_generator);
}

} // namespace mcrl2::lps

#endif // MCRL2_LPS_REPLACE_CAPTURE_AVOIDING_H
