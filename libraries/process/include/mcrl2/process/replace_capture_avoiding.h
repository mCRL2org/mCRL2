// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/replace_capture_avoiding.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_REPLACE_CAPTURE_AVOIDING_H
#define MCRL2_PROCESS_REPLACE_CAPTURE_AVOIDING_H

#include "mcrl2/data/replace_capture_avoiding.h"
#include "mcrl2/process/builder.h"
#include "mcrl2/process/find.h"

namespace mcrl2::process
{

namespace detail {

template<template<class> class Builder, class Derived, class Substitution>
struct add_capture_avoiding_replacement
  : public data::detail::add_capture_avoiding_replacement<Builder, Derived, Substitution>
{
  using super = data::detail::add_capture_avoiding_replacement<Builder, Derived, Substitution>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;
  using super::sigma;

  data::assignment_list::const_iterator find_variable(const data::assignment_list& a, const data::variable& v) const
  {
    for (auto i = a.begin(); i != a.end(); ++i)
    {
      if (i->lhs() == v)
      {
        return i;
      }
    }
    return a.end();
  }

  explicit add_capture_avoiding_replacement(data::detail::capture_avoiding_substitution_updater<Substitution>& sigma)
    : super(sigma)
  { }

  template <class T>
  void apply(T& result, const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    const data::assignment_list& a = x.assignments();
    std::vector <data::assignment> v;

    for (const data::variable& variable: x.identifier().variables())
    {
      auto k = find_variable(a, variable);
      if (k == a.end())
      {
        data::data_expression e;
        apply(e, variable);
        if (e != variable)
        {
          v.emplace_back(variable, e);
        }
      }
      else
      {
        data::data_expression rhs;
        apply(rhs, k->rhs());
        v.emplace_back(k->lhs(), rhs);
      }
    }
    make_process_instance_assignment(result, x.identifier(),data::assignment_list(v.begin(), v.end()));
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const sum& x)
  {
    data::variable_list v1 = sigma.add_fresh_variable_assignments(x.variables());
    process_expression body;
    apply(body, x.operand());
    make_sum(result, v1, body);
    sigma.remove_fresh_variable_assignments(x.variables());
  }

  template <class T>
  void apply(T& result, const stochastic_operator& x)
  {
    data::variable_list v1 = sigma.add_fresh_variable_assignments(x.variables());
    process_expression body;
    apply(body, x.operand());
    data::data_expression dist;
    apply(dist, x.distribution());
    make_stochastic_operator(result, v1, dist, body);
    sigma.remove_fresh_variable_assignments(x.variables());
  }
};

} // namespace detail

//--- start generated process replace_capture_avoiding code ---//
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
  data::detail::apply_replace_capture_avoiding_variables_builder<process::data_expression_builder, process::detail::add_capture_avoiding_replacement>(sigma1).update(x);
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
  data::detail::apply_replace_capture_avoiding_variables_builder<process::data_expression_builder, process::detail::add_capture_avoiding_replacement>(sigma1).apply(result, x);
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
  id_generator.add_identifiers(process::find_identifiers(x));
  for (const data::variable& v: substitution_variables(sigma))
  {
    id_generator.add_identifier(v.name());
  }
  process::replace_variables_capture_avoiding(x, sigma, id_generator);
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
  id_generator.add_identifiers(process::find_identifiers(x));
  for (const data::variable& v: substitution_variables(sigma))
  {
    id_generator.add_identifier(v.name());
  }
  return process::replace_variables_capture_avoiding(x, sigma, id_generator);
}
//--- end generated process replace_capture_avoiding code ---//

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_REPLACE_CAPTURE_AVOIDING_H
