// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/replace.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_REPLACE_H
#define MCRL2_PROCESS_REPLACE_H

#include "mcrl2/lps/replace.h"
#include "mcrl2/process/add_binding.h"
#include "mcrl2/process/builder.h"
#include "mcrl2/process/find.h"

namespace mcrl2
{

namespace process
{

namespace detail {

/// \cond INTERNAL_DOCS
template <template <class> class Builder, class Derived, class Substitution>
struct add_capture_avoiding_replacement: public lps::detail::add_capture_avoiding_replacement<Builder, Derived, Substitution>
{
  typedef lps::detail::add_capture_avoiding_replacement<Builder, Derived, Substitution> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::sigma;
  using super::update_sigma;

  add_capture_avoiding_replacement(Substitution& sigma, std::multiset<data::variable>& V)
    : super(sigma, V)
  { }

  // N.B. We cannot use the default implementation, since the left hand sides of the assignments
  // may not be renamed. This is because they are defined outside the scope of the expression x.
  process::process_expression operator()(const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::assignment_list a = x.assignments();
    std::vector<data::assignment> v(a.begin(), a.end());
    for (std::vector<data::assignment>::iterator i = v.begin(); i != v.end(); ++i)
    {
      *i = data::assignment(i->lhs(), (*this)(i->rhs()));
    }
    process::process_expression result = process::process_instance_assignment(x.identifier(), data::assignment_list(v.begin(), v.end()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process_expression operator()(const sum& x)
  {
    data::variable_list v = update_sigma.push(x.bound_variables());
    process_expression result = sum(v, (*this)(x.operand()));
    update_sigma.pop(v);
    return result;
  }
};
/// \endcond

} // namespace detail

//--- start generated process replace code ---//
template <typename T, typename Substitution>
void replace_sort_expressions(T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                             )
{
  data::detail::make_replace_sort_expressions_builder<process::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_sort_expressions(const T& x,
                           Substitution sigma,
                           bool innermost,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  return data::detail::make_replace_sort_expressions_builder<process::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
void replace_data_expressions(T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                             )
{
  data::detail::make_replace_data_expressions_builder<process::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_data_expressions(const T& x,
                           Substitution sigma,
                           bool innermost,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  return data::detail::make_replace_data_expressions_builder<process::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
void replace_variables(T& x,
                       Substitution sigma,
                       typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                      )
{
  core::make_update_apply_builder<process::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_variables(const T& x,
                    Substitution sigma,
                    typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                   )
{
  return core::make_update_apply_builder<process::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
void replace_free_variables(T& x,
                            Substitution sigma,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                           )
{
  data::detail::make_replace_free_variables_builder<process::data_expression_builder, process::add_data_variable_binding>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_free_variables(const T& x,
                         Substitution sigma,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                        )
{
  return data::detail::make_replace_free_variables_builder<process::data_expression_builder, process::add_data_variable_binding>(sigma)(x);
}

template <typename T, typename Substitution, typename VariableContainer>
void replace_free_variables(T& x,
                            Substitution sigma,
                            const VariableContainer& bound_variables,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                           )
{
  data::detail::make_replace_free_variables_builder<process::data_expression_builder, process::add_data_variable_binding>(sigma)(x, bound_variables);
}

template <typename T, typename Substitution, typename VariableContainer>
T replace_free_variables(const T& x,
                         Substitution sigma,
                         const VariableContainer& bound_variables,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                        )
{
  return data::detail::make_replace_free_variables_builder<process::data_expression_builder, process::add_data_variable_binding>(sigma)(x, bound_variables);
}

/// \brief Applies sigma as a capture avoiding substitution to x
/// \param sigma_variables contains the free variables appearing in the right hand side of sigma
template <typename T, typename Substitution, typename VariableContainer>
void replace_variables_capture_avoiding(T& x,
                       Substitution& sigma,
                       const VariableContainer& sigma_variables,
                       typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                      )
{
  std::multiset<data::variable> V;
  process::find_free_variables(x, std::inserter(V, V.end()));
  V.insert(sigma_variables.begin(), sigma_variables.end());
  data::detail::apply_replace_capture_avoiding_variables_builder<process::data_expression_builder, process::detail::add_capture_avoiding_replacement>(sigma, V)(x);
}

/// \brief Applies sigma as a capture avoiding substitution to x
/// \param sigma_variables contains the free variables appearing in the right hand side of sigma
template <typename T, typename Substitution, typename VariableContainer>
T replace_variables_capture_avoiding(const T& x,
                    Substitution& sigma,
                    const VariableContainer& sigma_variables,
                    typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                   )
{
  std::multiset<data::variable> V;
  process::find_free_variables(x, std::inserter(V, V.end()));
  V.insert(sigma_variables.begin(), sigma_variables.end());
  return data::detail::apply_replace_capture_avoiding_variables_builder<process::data_expression_builder, process::detail::add_capture_avoiding_replacement>(sigma, V)(x);
}
//--- end generated process replace code ---//

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_REPLACE_H
