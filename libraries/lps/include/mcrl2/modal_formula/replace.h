// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/replace.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_REPLACE_H
#define MCRL2_MODAL_FORMULA_REPLACE_H

#include "mcrl2/data/detail/replace.h"
#include "mcrl2/modal_formula/add_binding.h"
#include "mcrl2/modal_formula/builder.h"    

namespace mcrl2 {

namespace action_formulas {

//--- start generated action_formulas replace code ---//
#ifdef MCRL2_NEW_REPLACE_VARIABLES
  template <typename T, typename Substitution>
  void replace_variables(T& x,
                         Substitution sigma,
                         typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                        )
  {
    core::make_update_apply_builder<action_formulas::data_expression_builder>(sigma)(x);
  }

  template <typename T, typename Substitution>
  T replace_variables(const T& x,
                      Substitution sigma,
                      typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                     )
  {   
    return core::make_update_apply_builder<action_formulas::data_expression_builder>(sigma)(x);
  }

  template <typename T, typename Substitution>
  void replace_free_variables(T& x,
                              Substitution sigma,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                             )
  {
    data::detail::make_replace_free_variables_builder<action_formulas::data_expression_builder, action_formulas::add_data_variable_binding>(sigma)(x);
  }

  template <typename T, typename Substitution>
  T replace_free_variables(const T& x,
                           Substitution sigma,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                          )
  {
    return data::detail::make_replace_free_variables_builder<action_formulas::data_expression_builder, action_formulas::add_data_variable_binding>(sigma)(x);
  }

  template <typename T, typename Substitution, typename VariableContainer>
  void replace_free_variables(T& x,
                              Substitution sigma,
                              const VariableContainer& bound_variables,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                             )
  {
    data::detail::make_replace_free_variables_builder<action_formulas::data_expression_builder, action_formulas::add_data_variable_binding>(sigma)(x, bound_variables);
  }

  template <typename T, typename Substitution, typename VariableContainer>
  T replace_free_variables(const T& x,
                           Substitution sigma,
                           const VariableContainer& bound_variables,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                          )
  {
    return data::detail::make_replace_free_variables_builder<action_formulas::data_expression_builder, action_formulas::add_data_variable_binding>(sigma)(x, bound_variables);
  }
#else
  template < typename Container, typename Substitution >
  Container replace_variables(Container const& container, Substitution substitution)
  {
    return data::replace_variables(container, substitution);
  }
  
  template <typename Container, typename Substitution >
  Container replace_free_variables(Container const& container, Substitution substitution)
  {
    return data::replace_free_variables(container, substitution);
  }

  template <typename Container, typename Substitution , typename VariableSequence >
  Container replace_free_variables(Container const& container, Substitution substitution, VariableSequence const& bound)
  {
    return data::replace_free_variables(container, substitution, bound);
  }
#endif // MCRL2_NEW_REPLACE_VARIABLES
//--- end generated action_formulas replace code ---//

} // namespace action_formulas

namespace regular_formulas {

//--- start generated regular_formulas replace code ---//
#ifdef MCRL2_NEW_REPLACE_VARIABLES
  template <typename T, typename Substitution>
  void replace_variables(T& x,
                         Substitution sigma,
                         typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                        )
  {
    core::make_update_apply_builder<regular_formulas::data_expression_builder>(sigma)(x);
  }

  template <typename T, typename Substitution>
  T replace_variables(const T& x,
                      Substitution sigma,
                      typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                     )
  {   
    return core::make_update_apply_builder<regular_formulas::data_expression_builder>(sigma)(x);
  }

  template <typename T, typename Substitution>
  void replace_free_variables(T& x,
                              Substitution sigma,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                             )
  {
    data::detail::make_replace_free_variables_builder<regular_formulas::data_expression_builder, regular_formulas::add_data_variable_binding>(sigma)(x);
  }

  template <typename T, typename Substitution>
  T replace_free_variables(const T& x,
                           Substitution sigma,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                          )
  {
    return data::detail::make_replace_free_variables_builder<regular_formulas::data_expression_builder, regular_formulas::add_data_variable_binding>(sigma)(x);
  }

  template <typename T, typename Substitution, typename VariableContainer>
  void replace_free_variables(T& x,
                              Substitution sigma,
                              const VariableContainer& bound_variables,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                             )
  {
    data::detail::make_replace_free_variables_builder<regular_formulas::data_expression_builder, regular_formulas::add_data_variable_binding>(sigma)(x, bound_variables);
  }

  template <typename T, typename Substitution, typename VariableContainer>
  T replace_free_variables(const T& x,
                           Substitution sigma,
                           const VariableContainer& bound_variables,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                          )
  {
    return data::detail::make_replace_free_variables_builder<regular_formulas::data_expression_builder, regular_formulas::add_data_variable_binding>(sigma)(x, bound_variables);
  }
#else
  template < typename Container, typename Substitution >
  Container replace_variables(Container const& container, Substitution substitution)
  {
    return data::replace_variables(container, substitution);
  }
  
  template <typename Container, typename Substitution >
  Container replace_free_variables(Container const& container, Substitution substitution)
  {
    return data::replace_free_variables(container, substitution);
  }

  template <typename Container, typename Substitution , typename VariableSequence >
  Container replace_free_variables(Container const& container, Substitution substitution, VariableSequence const& bound)
  {
    return data::replace_free_variables(container, substitution, bound);
  }
#endif // MCRL2_NEW_REPLACE_VARIABLES
//--- end generated regular_formulas replace code ---//

} // namespace regular_formulas

namespace state_formulas {

//--- start generated state_formulas replace code ---//
#ifdef MCRL2_NEW_REPLACE_VARIABLES
  template <typename T, typename Substitution>
  void replace_variables(T& x,
                         Substitution sigma,
                         typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                        )
  {
    core::make_update_apply_builder<state_formulas::data_expression_builder>(sigma)(x);
  }

  template <typename T, typename Substitution>
  T replace_variables(const T& x,
                      Substitution sigma,
                      typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                     )
  {   
    return core::make_update_apply_builder<state_formulas::data_expression_builder>(sigma)(x);
  }

  template <typename T, typename Substitution>
  void replace_free_variables(T& x,
                              Substitution sigma,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                             )
  {
    data::detail::make_replace_free_variables_builder<state_formulas::data_expression_builder, state_formulas::add_data_variable_binding>(sigma)(x);
  }

  template <typename T, typename Substitution>
  T replace_free_variables(const T& x,
                           Substitution sigma,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                          )
  {
    return data::detail::make_replace_free_variables_builder<state_formulas::data_expression_builder, state_formulas::add_data_variable_binding>(sigma)(x);
  }

  template <typename T, typename Substitution, typename VariableContainer>
  void replace_free_variables(T& x,
                              Substitution sigma,
                              const VariableContainer& bound_variables,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                             )
  {
    data::detail::make_replace_free_variables_builder<state_formulas::data_expression_builder, state_formulas::add_data_variable_binding>(sigma)(x, bound_variables);
  }

  template <typename T, typename Substitution, typename VariableContainer>
  T replace_free_variables(const T& x,
                           Substitution sigma,
                           const VariableContainer& bound_variables,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                          )
  {
    return data::detail::make_replace_free_variables_builder<state_formulas::data_expression_builder, state_formulas::add_data_variable_binding>(sigma)(x, bound_variables);
  }
#else
  template < typename Container, typename Substitution >
  Container replace_variables(Container const& container, Substitution substitution)
  {
    return data::replace_variables(container, substitution);
  }
  
  template <typename Container, typename Substitution >
  Container replace_free_variables(Container const& container, Substitution substitution)
  {
    return data::replace_free_variables(container, substitution);
  }

  template <typename Container, typename Substitution , typename VariableSequence >
  Container replace_free_variables(Container const& container, Substitution substitution, VariableSequence const& bound)
  {
    return data::replace_free_variables(container, substitution, bound);
  }
#endif // MCRL2_NEW_REPLACE_VARIABLES
//--- end generated state_formulas replace code ---//

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_REPLACE_H
