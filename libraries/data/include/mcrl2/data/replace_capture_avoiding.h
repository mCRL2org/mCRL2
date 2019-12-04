// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/replace_capture_avoiding.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_REPLACE_CAPTURE_AVOIDING_H
#define MCRL2_DATA_REPLACE_CAPTURE_AVOIDING_H

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/builder.h"
#include "mcrl2/data/find.h"

namespace mcrl2 {

namespace data {

namespace detail {

// Wraps a substitution, such that assignments to variables can be added and removed.
template <typename Substitution>
struct capture_avoiding_substitution_updater
{
  Substitution& sigma;
  data::set_identifier_generator& id_generator;
  std::map<variable, std::list<variable>> updates;

  capture_avoiding_substitution_updater(Substitution& sigma_, data::set_identifier_generator& id_generator_)
    : sigma(sigma_), id_generator(id_generator_)
  {}

  // adds the assignment [v := v'] to sigma, and returns v'
  variable add_fresh_variable_assignment(const variable& v)
  {
    variable v1(id_generator(v.name()), v.sort());
    updates[v].push_back(v1);
    return v1;
  }

  // removes the assignment [v := v'] from sigma
  void remove_fresh_variable_assignment(const variable& v)
  {
    auto i = updates.find(v);
    id_generator.remove_identifier(i->second.back().name());
    i->second.pop_back();
    if (i->second.empty())
    {
      updates.erase(i);
    }
  }

  // adds the assignments [variables := variables'] to sigma, and returns variables'
  template <typename VariableContainer>
  variable_list add_fresh_variable_assignments(const VariableContainer& variables)
  {
    return variable_list(
      variables.begin(),
      variables.end(),
      [&](const variable& v)
      {
        return add_fresh_variable_assignment(v);
      }
    );
  }

  // removes the assignments [variables := variables'] from sigma
  template <typename VariableContainer>
  void remove_fresh_variable_assignments(const VariableContainer& variables)
  {
    for (const variable& v: variables)
    {
      remove_fresh_variable_assignment(v);
    }
  }

  data_expression operator()(const variable& x)
  {
    auto i = updates.find(x);
    if (i != updates.end())
    {
      return i->second.back();
    }
    else
    {
      return sigma(x);
    }
  }
};

template <typename Substitution>
std::ostream& operator<<(std::ostream& out, const capture_avoiding_substitution_updater<Substitution>& sigma)
{
  std::vector<std::string> updates;
  for (const auto& p: sigma.updates)
  {
    updates.push_back(data::pp(p.first) + " := " + core::detail::print_list(p.second));
  }
  return out << sigma.sigma << " with updates " << core::detail::print_list(updates);
}

template <template <class> class Builder, template <template <class> class, class, class> class Binder, class Substitution>
struct replace_capture_avoiding_variables_builder: public Binder<Builder, replace_capture_avoiding_variables_builder<Builder, Binder, Substitution>, Substitution>
{
  typedef Binder<Builder, replace_capture_avoiding_variables_builder<Builder, Binder, Substitution>, Substitution> super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;

  explicit replace_capture_avoiding_variables_builder(capture_avoiding_substitution_updater<Substitution>& sigma)
    : super(sigma)
  { }
};

template <template <class> class Builder, template <template <class> class, class, class> class Binder, class Substitution>
replace_capture_avoiding_variables_builder<Builder, Binder, Substitution>
apply_replace_capture_avoiding_variables_builder(capture_avoiding_substitution_updater<Substitution>& sigma)
{
  return replace_capture_avoiding_variables_builder<Builder, Binder, Substitution>(sigma);
}

template <template <class> class Builder, class Derived, class Substitution>
struct add_capture_avoiding_replacement: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;

  capture_avoiding_substitution_updater<Substitution>& sigma;

  explicit add_capture_avoiding_replacement(capture_avoiding_substitution_updater<Substitution>& sigma_)
    : sigma(sigma_)
  { }

  // applies the substitution to the right hand sides of the assignments
  assignment_list apply(const assignment_list& x)
  {
    return assignment_list(
      x.begin(),
      x.end(),
      [&](const data::assignment& a)
      {
        return data::assignment(a.lhs(), apply(a.rhs()));
      }
    );
  }

  data_expression apply(const variable& v)
  {
    return sigma(v);
  }

  data_expression apply(const data::where_clause& x)
  {
    const auto& declarations = atermpp::container_cast<data::assignment_list>(x.declarations());

    auto declarations1 = data::assignment_list(
      declarations.begin(),
      declarations.end(),
      [&](const assignment& a)
      {
        const data::variable& v = a.lhs();
        const data_expression& x1 = a.rhs();
        // add the assignment [v := v'] to sigma
        data::variable v1 = sigma.add_fresh_variable_assignment(v);
        return assignment(v1, apply(x1));
      }
    );
    data_expression result = where_clause(apply(x.body()), declarations1);

    // remove the assignments [v := v'] from sigma
    for (const assignment& a: declarations)
    {
      const variable& v = a.lhs();
      sigma.remove_fresh_variable_assignment(v);
    }

    return result;
  }

  data_expression apply(const data::forall& x)
  {
    variable_list v1 = sigma.add_fresh_variable_assignments(x.variables());
    data_expression result = data::forall(v1, apply(x.body()));
    sigma.remove_fresh_variable_assignments(x.variables());
    return result;
  }

  data_expression apply(const data::exists& x)
  {
    variable_list v1 = sigma.add_fresh_variable_assignments(x.variables());
    data_expression result = data::exists(v1, apply(x.body()));
    sigma.remove_fresh_variable_assignments(x.variables());
    return result;
  }

  data_expression apply(const data::lambda& x)
  {
    variable_list v1 = sigma.add_fresh_variable_assignments(x.variables());
    data_expression result = data::lambda(v1, apply(x.body()));
    sigma.remove_fresh_variable_assignments(x.variables());
    return result;
  }

  data_equation apply(data_equation& /* x */)
  {
    throw mcrl2::runtime_error("not implemented yet");
  }
};

} // namespace detail

//--- start generated data replace_capture_avoiding code ---//
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
  data::detail::apply_replace_capture_avoiding_variables_builder<data::data_expression_builder, data::detail::add_capture_avoiding_replacement>(sigma1).update(x);
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
  return data::detail::apply_replace_capture_avoiding_variables_builder<data::data_expression_builder, data::detail::add_capture_avoiding_replacement>(sigma1).apply(x);
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
  id_generator.add_identifiers(data::find_identifiers(x));
  for (const data::variable& v: substitution_variables(sigma))
  {
    id_generator.add_identifier(v.name());
  }
  data::replace_variables_capture_avoiding(x, sigma, id_generator);
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
  id_generator.add_identifiers(data::find_identifiers(x));
  for (const data::variable& v: substitution_variables(sigma))
  {
    id_generator.add_identifier(v.name());
  }
  return data::replace_variables_capture_avoiding(x, sigma, id_generator);
}
//--- end generated data replace_capture_avoiding code ---//

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_REPLACE_CAPTURE_AVOIDING_H
