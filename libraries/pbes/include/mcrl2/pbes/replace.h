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

//--- start generated pbes_system replace code ---//
template <typename T, typename Substitution>
void replace_sort_expressions(T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                             )
{
  data::detail::make_replace_sort_expressions_builder<pbes_system::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_sort_expressions(const T& x,
                           Substitution sigma,
                           bool innermost,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  return data::detail::make_replace_sort_expressions_builder<pbes_system::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
void replace_data_expressions(T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                             )
{
  data::detail::make_replace_data_expressions_builder<pbes_system::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_data_expressions(const T& x,
                           Substitution sigma,
                           bool innermost,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  return data::detail::make_replace_data_expressions_builder<pbes_system::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
void replace_variables(T& x,
                       Substitution sigma,
                       typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                      )
{
  core::make_update_apply_builder<pbes_system::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_variables(const T& x,
                    Substitution sigma,
                    typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                   )
{   
  return core::make_update_apply_builder<pbes_system::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
void replace_free_variables(T& x,
                            Substitution sigma,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                           )
{
  data::detail::make_replace_free_variables_builder<pbes_system::data_expression_builder, pbes_system::add_data_variable_binding>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_free_variables(const T& x,
                         Substitution sigma,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                        )
{
  return data::detail::make_replace_free_variables_builder<pbes_system::data_expression_builder, pbes_system::add_data_variable_binding>(sigma)(x);
}

template <typename T, typename Substitution, typename VariableContainer>
void replace_free_variables(T& x,
                            Substitution sigma,
                            const VariableContainer& bound_variables,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                           )
{
  data::detail::make_replace_free_variables_builder<pbes_system::data_expression_builder, pbes_system::add_data_variable_binding>(sigma)(x, bound_variables);
}

template <typename T, typename Substitution, typename VariableContainer>
T replace_free_variables(const T& x,
                         Substitution sigma,
                         const VariableContainer& bound_variables,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                        )
{
  return data::detail::make_replace_free_variables_builder<pbes_system::data_expression_builder, pbes_system::add_data_variable_binding>(sigma)(x, bound_variables);
}
//--- end generated pbes_system replace code ---//

/// \brief Applies a propositional variable substitution.
template <typename T, typename Substitution>
void replace_propositional_variables(T& x,
                                        Substitution sigma,
                                        typename boost::disable_if<typename boost::is_base_of< atermpp::aterm, T>::type>::type* = 0
                                       )
{
  core::make_update_apply_builder<pbes_expression_builder>(sigma)(x);
}

/// \brief Applies a propositional variable substitution.
template <typename T, typename Substitution>
T replace_propositional_variables(const T& x,
                                     Substitution sigma,
                                     typename boost::enable_if<typename boost::is_base_of< atermpp::aterm, T>::type>::type* = 0
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
