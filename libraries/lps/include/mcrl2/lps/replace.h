// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/replace.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_REPLACE_H
#define MCRL2_LPS_REPLACE_H

#include "mcrl2/data/replace.h"
#include "mcrl2/lps/add_binding.h"
#include "mcrl2/lps/builder.h"

namespace mcrl2
{

namespace lps
{

//--- start generated lps replace code ---//
template <typename T, typename Substitution>
void replace_sort_expressions(T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                             )
{
  data::detail::make_replace_sort_expressions_builder<lps::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_sort_expressions(const T& x,
                           Substitution sigma,
                           bool innermost,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                          )
{
  return data::detail::make_replace_sort_expressions_builder<lps::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
void replace_data_expressions(T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                             )
{
  data::detail::make_replace_data_expressions_builder<lps::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_data_expressions(const T& x,
                           Substitution sigma,
                           bool innermost,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                          )
{
  return data::detail::make_replace_data_expressions_builder<lps::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
void replace_variables(T& x,
                       Substitution sigma,
                       typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                      )
{
  core::make_update_apply_builder<lps::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_variables(const T& x,
                    Substitution sigma,
                    typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                   )
{
  return core::make_update_apply_builder<lps::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
void replace_free_variables(T& x,
                            Substitution sigma,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                           )
{
  data::detail::make_replace_free_variables_builder<lps::data_expression_builder, lps::add_data_variable_binding>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_free_variables(const T& x,
                         Substitution sigma,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                        )
{
  return data::detail::make_replace_free_variables_builder<lps::data_expression_builder, lps::add_data_variable_binding>(sigma)(x);
}

template <typename T, typename Substitution, typename VariableContainer>
void replace_free_variables(T& x,
                            Substitution sigma,
                            const VariableContainer& bound_variables,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                           )
{
  data::detail::make_replace_free_variables_builder<lps::data_expression_builder, lps::add_data_variable_binding>(sigma)(x, bound_variables);
}

template <typename T, typename Substitution, typename VariableContainer>
T replace_free_variables(const T& x,
                         Substitution sigma,
                         const VariableContainer& bound_variables,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                        )
{
  return data::detail::make_replace_free_variables_builder<lps::data_expression_builder, lps::add_data_variable_binding>(sigma)(x, bound_variables);
}
//--- end generated lps replace code ---//

namespace detail {

/// \cond INTERNAL_DOCS
template <template <class> class Builder, template <template <class> class, class> class Binder, class Substitution>
struct replace_process_parameter_builder: public data::detail::substitute_free_variables_builder<Builder, Binder, Substitution>
{
  typedef data::detail::substitute_free_variables_builder<Builder, Binder, Substitution> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::bind_count;

  replace_process_parameter_builder(Substitution sigma)
    : super(sigma)
  {}

  // replace the assignments (in where clauses, summands and process initializers)
  data::assignment_expression operator()(const data::assignment& x)
  {
    if (bind_count(x.lhs()) == 1)
    {
      data::assignment_expression result(super::operator()(x.lhs()), super::operator()(x.rhs()));
      return result;
    }
    else
    {
      return super::operator()(x);
    }
  }

  // replace the process parameters
  void operator()(lps::linear_process& x)
  {
    super::operator()(x);
    x.process_parameters() = super::operator()(x.process_parameters());
  }
};

template <template <class> class Builder, template <template <class> class, class> class Binder, class Substitution>
replace_process_parameter_builder<Builder, Binder, Substitution>
make_replace_process_parameters_builder(Substitution sigma)
{
  return replace_process_parameter_builder<Builder, Binder, Substitution>(sigma);
}
/// \endcond

} // namespace detail

/// \brief Applies a substitution to the process parameters of the specification spec.
template <typename Substitution>
void replace_process_parameters(specification& spec, Substitution sigma)
{
  if (sigma.empty())
  {
    return;
  }
  lps::detail::make_replace_process_parameters_builder<lps::data_expression_builder, lps::add_data_variable_binding>(data::make_map_substitution(sigma))(spec);
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_REPLACE_H
