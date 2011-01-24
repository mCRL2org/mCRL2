// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/substitute.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_SUBSTITUTE_H
#define MCRL2_PBES_SUBSTITUTE_H

#include "mcrl2/pbes/detail/pbes_substituter.h"
#include "mcrl2/pbes/detail/propositional_variable_substituter.h"
#include "mcrl2/pbes/add_binding.h"
#include "mcrl2/pbes/builder.h"    
#include "mcrl2/data/substitute.h"

namespace mcrl2 {

namespace pbes_system {

  // TODO: with g++ both 'const Substitution& sigma' nor 'Substitution sigma' leads
  // to problems at different locations; why???
  /// \brief Applies a substitution to a PBES data type.
  template <typename Object, typename Substitution>
  void substitute(Object& o, const Substitution& sigma, bool replace_parameters)
  {
    pbes_system::detail::pbes_substituter<Substitution> r(sigma, replace_parameters);
    r(o);
  }

  // TODO: with g++ both 'const Substitution& sigma' nor 'Substitution sigma' leads
  // to problems at different locations; why???
  /// \brief Applies a substitution to a PBES data type.
  template <typename Object, typename Substitution>
  void substitute(Object& o, const Substitution& sigma)
  {
    substitute_gcc(o, sigma, false);
  }

  // TODO: with g++ both 'const Substitution& sigma' nor 'Substitution sigma' leads
  // to problems at different locations; why???
  /// \brief Applies a substitution to a PBES data type.
  template <typename Object, typename Substitution>
  void substitute_gcc_workaround(Object& o, const Substitution& sigma, bool replace_parameters)
  {
    pbes_system::detail::pbes_substituter<Substitution> r(sigma, replace_parameters);
    r(o);
  }

  // TODO: with g++ both 'const Substitution& sigma' nor 'Substitution sigma' leads
  // to problems at different locations; why???
  /// \brief Applies a substitution to a PBES data type.
  template <typename Object, typename Substitution>
  void substitute_gcc_workaround(Object& o, const Substitution& sigma)
  {
    substitute_gcc_workaround(o, sigma, false);
  }

  /// \brief Applies a propositional variable substitution to a PBES data type.
  template <typename Object, typename PropositionalVariableSubstitution>
  void propositional_variable_substitute(Object& o, PropositionalVariableSubstitution sigma, bool replace_parameters = false)
  {
    pbes_system::detail::propositional_variable_substituter<PropositionalVariableSubstitution> r(sigma, replace_parameters);
    r(o);
  }

//--- start generated pbes_system replace code ---//
template <typename T, typename Substitution>
  void substitute_variables(T& x,
                            Substitution sigma,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                           )
  {
    core::make_update_apply_builder<pbes_system::data_expression_builder>(sigma)(x);
  }

  template <typename T, typename Substitution>
  T substitute_variables(const T& x,
                         Substitution sigma,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                        )
  {   
    return core::make_update_apply_builder<pbes_system::data_expression_builder>(sigma)(x);
  }

  template <typename T, typename Substitution>
  void substitute_free_variables(T& x,
                                 Substitution sigma,
                                 typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                                )
  {
    data::detail::make_substitute_free_variables_builder<pbes_system::data_expression_builder, pbes_system::add_data_variable_binding>(sigma)(x);
  }

  template <typename T, typename Substitution>
  T substitute_free_variables(const T& x,
                              Substitution sigma,
                              typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                             )
  {
    return data::detail::make_substitute_free_variables_builder<pbes_system::data_expression_builder, pbes_system::add_data_variable_binding>(sigma)(x);
  }

  template <typename T, typename Substitution, typename VariableContainer>
  void substitute_free_variables(T& x,
                                 Substitution sigma,
                                 const VariableContainer& bound_variables,
                                 typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                                )
  {
    data::detail::make_substitute_free_variables_builder<pbes_system::data_expression_builder, pbes_system::add_data_variable_binding>(sigma)(x, bound_variables);
  }

  template <typename T, typename Substitution, typename VariableContainer>
  T substitute_free_variables(const T& x,
                              Substitution sigma,
                              const VariableContainer& bound_variables,
                              typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                             )
  {
    return data::detail::make_substitute_free_variables_builder<pbes_system::data_expression_builder, pbes_system::add_data_variable_binding>(sigma)(x, bound_variables);
  }
//--- end generated pbes_system replace code ---//

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_SUBSTITUTE_H
