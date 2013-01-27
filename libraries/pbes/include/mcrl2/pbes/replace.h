// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/replace.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REPLACE_H
#define MCRL2_PBES_REPLACE_H

#include "mcrl2/pbes/add_binding.h"
#include "mcrl2/pbes/builder.h"
#include "mcrl2/data/replace.h"

namespace mcrl2
{

namespace pbes_system
{

namespace detail {

/// \cond INTERNAL_DOCS
template <template <class> class Builder, class Derived, class Substitution>
struct add_capture_avoiding_replacement: public data::detail::add_capture_avoiding_replacement<Builder, Derived, Substitution>
{
  typedef data::detail::add_capture_avoiding_replacement<Builder, Derived, Substitution> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::sigma;
  using super::update_sigma;

  pbes_expression operator()(const forall& x)
  {
    data::variable_list v = update_sigma.push(x.variables());
    pbes_expression result = forall(v, (*this)(x.body()));
    update_sigma.pop(v);
    return result;
  }

  pbes_expression operator()(const exists& x)
  {
    data::variable_list v = update_sigma.push(x.variables());
    pbes_expression result = exists(v, (*this)(x.body()));
    update_sigma.pop(v);
    return result;
  }

  void operator()(pbes_equation& x)
  {
    data::variable_list v = update_sigma.push(x.variable().parameters());
    x.variable() = propositional_variable(x.variable().name(), v);
    x.formula() = (*this)(x.formula());
    update_sigma.pop(v);
  }

  template <typename Container>
  void operator()(pbes<Container>& x)
  {
    atermpp::set<data::variable> v = update_sigma(x.global_variables());
    x.global_variables() = v;
    (*this)(x.equations());
    update_sigma.pop(v);
  }

  add_capture_avoiding_replacement(Substitution& sigma, std::multiset<data::variable>& V)
    : super(sigma, V)
  { }
};
/// \endcond

} // namespace detail

//--- start generated pbes_system replace code ---//
template <typename T, typename Substitution>
void replace_sort_expressions(T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                             )
{
  data::detail::make_replace_sort_expressions_builder<pbes_system::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_sort_expressions(const T& x,
                           Substitution sigma,
                           bool innermost,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                          )
{
  return data::detail::make_replace_sort_expressions_builder<pbes_system::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
void replace_data_expressions(T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                             )
{
  data::detail::make_replace_data_expressions_builder<pbes_system::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_data_expressions(const T& x,
                           Substitution sigma,
                           bool innermost,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                          )
{
  return data::detail::make_replace_data_expressions_builder<pbes_system::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
void replace_variables(T& x,
                       Substitution sigma,
                       typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                      )
{
  core::make_update_apply_builder<pbes_system::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_variables(const T& x,
                    Substitution sigma,
                    typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                   )
{
  return core::make_update_apply_builder<pbes_system::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
void replace_free_variables(T& x,
                            Substitution sigma,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                           )
{
  data::detail::make_replace_free_variables_builder<pbes_system::data_expression_builder, pbes_system::add_data_variable_binding>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_free_variables(const T& x,
                         Substitution sigma,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                        )
{
  return data::detail::make_replace_free_variables_builder<pbes_system::data_expression_builder, pbes_system::add_data_variable_binding>(sigma)(x);
}

template <typename T, typename Substitution, typename VariableContainer>
void replace_free_variables(T& x,
                            Substitution sigma,
                            const VariableContainer& bound_variables,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                           )
{
  data::detail::make_replace_free_variables_builder<pbes_system::data_expression_builder, pbes_system::add_data_variable_binding>(sigma)(x, bound_variables);
}

template <typename T, typename Substitution, typename VariableContainer>
T replace_free_variables(const T& x,
                         Substitution sigma,
                         const VariableContainer& bound_variables,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                        )
{
  return data::detail::make_replace_free_variables_builder<pbes_system::data_expression_builder, pbes_system::add_data_variable_binding>(sigma)(x, bound_variables);
}

/// \brief Applies sigma as a capture avoiding substitution to x
/// \param sigma_variables contains the free variables appearing in the right hand side of sigma
template <typename T, typename Substitution, typename VariableContainer>
void replace_variables_capture_avoiding(T& x,
                       Substitution& sigma,
                       const VariableContainer& sigma_variables,
                       typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                      )
{
  std::multiset<data::variable> V;
  pbes_system::find_free_variables(x, std::inserter(V, V.end()));
  V.insert(sigma_variables.begin(), sigma_variables.end());
  data::detail::apply_replace_capture_avoiding_variables_builder<pbes_system::data_expression_builder, pbes_system::detail::add_capture_avoiding_replacement>(sigma, V)(x);
}

/// \brief Applies sigma as a capture avoiding substitution to x
/// \param sigma_variables contains the free variables appearing in the right hand side of sigma
template <typename T, typename Substitution, typename VariableContainer>
T replace_variables_capture_avoiding(const T& x,
                    Substitution& sigma,
                    const VariableContainer& sigma_variables,
                    typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                   )
{
  std::multiset<data::variable> V;
  pbes_system::find_free_variables(x, std::inserter(V, V.end()));
  V.insert(sigma_variables.begin(), sigma_variables.end());
  return data::detail::apply_replace_capture_avoiding_variables_builder<pbes_system::data_expression_builder, pbes_system::detail::add_capture_avoiding_replacement>(sigma, V)(x);
}
//--- end generated pbes_system replace code ---//

/// \brief Applies a propositional variable substitution.
template <typename T, typename Substitution>
void replace_propositional_variables(T& x,
                                        Substitution sigma,
                                        typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                                       )
{
  core::make_update_apply_builder<pbes_expression_builder>(sigma)(x);
}

/// \brief Applies a propositional variable substitution.
template <typename T, typename Substitution>
T replace_propositional_variables(const T& x,
                                     Substitution sigma,
                                     typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                                    )
{
  return core::make_update_apply_builder<pbes_expression_builder>(sigma)(x);
}

} // namespace pbes_system

} // namespace mcrl2

#ifndef MCRL2_PBES_SUBSTITUTIONS_H
#include "mcrl2/pbes/substitutions.h"
#endif

#endif // MCRL2_PBES_REPLACE_H
