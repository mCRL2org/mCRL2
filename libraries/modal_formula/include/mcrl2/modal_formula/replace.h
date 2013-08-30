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

#include "mcrl2/lps/replace.h"
#include "mcrl2/modal_formula/add_binding.h"
#include "mcrl2/modal_formula/builder.h"
#include "mcrl2/modal_formula/find.h"

namespace mcrl2
{

namespace action_formulas
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

  action_formula operator()(const forall& x)
  {
    data::variable_list v = update_sigma.push(x.variables());
    action_formula result = forall(v, (*this)(x.body()));
    update_sigma.pop(v);
    return result;
  }

  action_formula operator()(const exists& x)
  {
    data::variable_list v = update_sigma.push(x.variables());
    action_formula result = exists(v, (*this)(x.body()));
    update_sigma.pop(v);
    return result;
  }

  add_capture_avoiding_replacement(Substitution& sigma, std::multiset<data::variable>& V)
    : super(sigma, V)
  { }
};
/// \endcond

} // namespace detail

//--- start generated action_formulas replace code ---//
template <typename T, typename Substitution>
void replace_sort_expressions(T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                             )
{
  data::detail::make_replace_sort_expressions_builder<action_formulas::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_sort_expressions(const T& x,
                           Substitution sigma,
                           bool innermost,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  return core::static_down_cast<const T&>(data::detail::make_replace_sort_expressions_builder<action_formulas::sort_expression_builder>(sigma, innermost)(x));
}

template <typename T, typename Substitution>
void replace_data_expressions(T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                             )
{
  data::detail::make_replace_data_expressions_builder<action_formulas::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_data_expressions(const T& x,
                           Substitution sigma,
                           bool innermost,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  return core::static_down_cast<const T&>(data::detail::make_replace_data_expressions_builder<action_formulas::data_expression_builder>(sigma, innermost)(x));
}

template <typename T, typename Substitution>
void replace_variables(T& x,
                       Substitution sigma,
                       typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                      )
{
  core::make_update_apply_builder<action_formulas::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_variables(const T& x,
                    Substitution sigma,
                    typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                   )
{
  return core::make_update_apply_builder<action_formulas::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
void replace_all_variables(T& x,
                           Substitution sigma,
                           typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  core::make_update_apply_builder<action_formulas::variable_builder>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_all_variables(const T& x,
                        Substitution sigma,
                        typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                       )
{
  return core::make_update_apply_builder<action_formulas::variable_builder>(sigma)(x);
}

/// \brief Applies the substitution sigma to x.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution>
void replace_free_variables(T& x,
                            Substitution sigma,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                           )
{
  assert(data::is_simple_substitution(sigma));
  data::detail::make_replace_free_variables_builder<action_formulas::data_expression_builder, action_formulas::add_data_variable_binding>(sigma)(x);
}

/// \brief Applies the substitution sigma to x.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution>
T replace_free_variables(const T& x,
                         Substitution sigma,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                        )
{
  assert(data::is_simple_substitution(sigma));
  return core::static_down_cast<const T&>(data::detail::make_replace_free_variables_builder<action_formulas::data_expression_builder, action_formulas::add_data_variable_binding>(sigma)(x));
}

/// \brief Applies the substitution sigma to x, where the elements of bound_variables are treated as bound variables.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution, typename VariableContainer>
void replace_free_variables(T& x,
                            Substitution sigma,
                            const VariableContainer& bound_variables,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                           )
{
  assert(data::is_simple_substitution(sigma));
  data::detail::make_replace_free_variables_builder<action_formulas::data_expression_builder, action_formulas::add_data_variable_binding>(sigma)(x, bound_variables);
}

/// \brief Applies the substitution sigma to x, where the elements of bound_variables are treated as bound variables.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution, typename VariableContainer>
T replace_free_variables(const T& x,
                         Substitution sigma,
                         const VariableContainer& bound_variables,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                        )
{
  assert(data::is_simple_substitution(sigma));
  return core::static_down_cast<const T&>(data::detail::make_replace_free_variables_builder<action_formulas::data_expression_builder, action_formulas::add_data_variable_binding>(sigma)(x, bound_variables));
}
//--- end generated action_formulas replace code ---//

//--- start generated action_formulas replace_capture_avoiding code ---//
/// \brief Applies sigma as a capture avoiding substitution to x
/// \param sigma A mutable substitution
/// \param sigma_variables a container of variables
/// \pre { sigma_variables must contain the free variables appearing in the right hand side of sigma }
template <typename T, typename Substitution, typename VariableContainer>
void replace_variables_capture_avoiding(T& x,
                       Substitution& sigma,
                       const VariableContainer& sigma_variables,
                       typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                      )
{
  std::multiset<data::variable> V;
  action_formulas::find_free_variables(x, std::inserter(V, V.end()));
  V.insert(sigma_variables.begin(), sigma_variables.end());
  data::detail::apply_replace_capture_avoiding_variables_builder<action_formulas::data_expression_builder, action_formulas::detail::add_capture_avoiding_replacement>(sigma, V)(x);
}

/// \brief Applies sigma as a capture avoiding substitution to x
/// \param sigma A mutable substitution
/// \param sigma_variables a container of variables
/// \pre { sigma_variables must contain the free variables appearing in the right hand side of sigma }
template <typename T, typename Substitution, typename VariableContainer>
T replace_variables_capture_avoiding(const T& x,
                    Substitution& sigma,
                    const VariableContainer& sigma_variables,
                    typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                   )
{
  std::multiset<data::variable> V;
  action_formulas::find_free_variables(x, std::inserter(V, V.end()));
  V.insert(sigma_variables.begin(), sigma_variables.end());
  return core::static_down_cast<const T&>(data::detail::apply_replace_capture_avoiding_variables_builder<action_formulas::data_expression_builder, action_formulas::detail::add_capture_avoiding_replacement>(sigma, V)(x));
}
//--- end generated action_formulas replace_capture_avoiding code ---//

} // namespace action_formulas

namespace regular_formulas
{

namespace detail
{

/// \cond INTERNAL_DOCS
template <template <class> class Builder, class Derived, class Substitution>
struct add_capture_avoiding_replacement: public action_formulas::detail::add_capture_avoiding_replacement<Builder, Derived, Substitution>
{
  typedef action_formulas::detail::add_capture_avoiding_replacement<Builder, Derived, Substitution> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::sigma;
  using super::update_sigma;

  add_capture_avoiding_replacement(Substitution& sigma, std::multiset<data::variable>& V)
    : super(sigma, V)
  { }
};

template <template <class> class Builder, class Substitution>
add_capture_avoiding_replacement<Builder, class Derived, Substitution>
make_add_capture_avoiding_replacement(Substitution& sigma, std::multiset<data::variable>& V)
{
  return add_capture_avoiding_replacement<Builder, Derived, Substitution>(sigma, V);
}
/// \endcond

} // namespace detail

//--- start generated regular_formulas replace code ---//
template <typename T, typename Substitution>
void replace_sort_expressions(T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                             )
{
  data::detail::make_replace_sort_expressions_builder<regular_formulas::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_sort_expressions(const T& x,
                           Substitution sigma,
                           bool innermost,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  return core::static_down_cast<const T&>(data::detail::make_replace_sort_expressions_builder<regular_formulas::sort_expression_builder>(sigma, innermost)(x));
}

template <typename T, typename Substitution>
void replace_data_expressions(T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                             )
{
  data::detail::make_replace_data_expressions_builder<regular_formulas::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_data_expressions(const T& x,
                           Substitution sigma,
                           bool innermost,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  return core::static_down_cast<const T&>(data::detail::make_replace_data_expressions_builder<regular_formulas::data_expression_builder>(sigma, innermost)(x));
}

template <typename T, typename Substitution>
void replace_variables(T& x,
                       Substitution sigma,
                       typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                      )
{
  core::make_update_apply_builder<regular_formulas::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_variables(const T& x,
                    Substitution sigma,
                    typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                   )
{
  return core::make_update_apply_builder<regular_formulas::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
void replace_all_variables(T& x,
                           Substitution sigma,
                           typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  core::make_update_apply_builder<regular_formulas::variable_builder>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_all_variables(const T& x,
                        Substitution sigma,
                        typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                       )
{
  return core::make_update_apply_builder<regular_formulas::variable_builder>(sigma)(x);
}

/// \brief Applies the substitution sigma to x.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution>
void replace_free_variables(T& x,
                            Substitution sigma,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                           )
{
  assert(data::is_simple_substitution(sigma));
  data::detail::make_replace_free_variables_builder<regular_formulas::data_expression_builder, regular_formulas::add_data_variable_binding>(sigma)(x);
}

/// \brief Applies the substitution sigma to x.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution>
T replace_free_variables(const T& x,
                         Substitution sigma,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                        )
{
  assert(data::is_simple_substitution(sigma));
  return core::static_down_cast<const T&>(data::detail::make_replace_free_variables_builder<regular_formulas::data_expression_builder, regular_formulas::add_data_variable_binding>(sigma)(x));
}

/// \brief Applies the substitution sigma to x, where the elements of bound_variables are treated as bound variables.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution, typename VariableContainer>
void replace_free_variables(T& x,
                            Substitution sigma,
                            const VariableContainer& bound_variables,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                           )
{
  assert(data::is_simple_substitution(sigma));
  data::detail::make_replace_free_variables_builder<regular_formulas::data_expression_builder, regular_formulas::add_data_variable_binding>(sigma)(x, bound_variables);
}

/// \brief Applies the substitution sigma to x, where the elements of bound_variables are treated as bound variables.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution, typename VariableContainer>
T replace_free_variables(const T& x,
                         Substitution sigma,
                         const VariableContainer& bound_variables,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                        )
{
  assert(data::is_simple_substitution(sigma));
  return core::static_down_cast<const T&>(data::detail::make_replace_free_variables_builder<regular_formulas::data_expression_builder, regular_formulas::add_data_variable_binding>(sigma)(x, bound_variables));
}
//--- end generated regular_formulas replace code ---//

//--- start generated regular_formulas replace_capture_avoiding code ---//
/// \brief Applies sigma as a capture avoiding substitution to x
/// \param sigma A mutable substitution
/// \param sigma_variables a container of variables
/// \pre { sigma_variables must contain the free variables appearing in the right hand side of sigma }
template <typename T, typename Substitution, typename VariableContainer>
void replace_variables_capture_avoiding(T& x,
                       Substitution& sigma,
                       const VariableContainer& sigma_variables,
                       typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                      )
{
  std::multiset<data::variable> V;
  regular_formulas::find_free_variables(x, std::inserter(V, V.end()));
  V.insert(sigma_variables.begin(), sigma_variables.end());
  data::detail::apply_replace_capture_avoiding_variables_builder<regular_formulas::data_expression_builder, regular_formulas::detail::add_capture_avoiding_replacement>(sigma, V)(x);
}

/// \brief Applies sigma as a capture avoiding substitution to x
/// \param sigma A mutable substitution
/// \param sigma_variables a container of variables
/// \pre { sigma_variables must contain the free variables appearing in the right hand side of sigma }
template <typename T, typename Substitution, typename VariableContainer>
T replace_variables_capture_avoiding(const T& x,
                    Substitution& sigma,
                    const VariableContainer& sigma_variables,
                    typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                   )
{
  std::multiset<data::variable> V;
  regular_formulas::find_free_variables(x, std::inserter(V, V.end()));
  V.insert(sigma_variables.begin(), sigma_variables.end());
  return core::static_down_cast<const T&>(data::detail::apply_replace_capture_avoiding_variables_builder<regular_formulas::data_expression_builder, regular_formulas::detail::add_capture_avoiding_replacement>(sigma, V)(x));
}
//--- end generated regular_formulas replace_capture_avoiding code ---//

} // namespace regular_formulas

namespace state_formulas
{

namespace detail
{

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

  state_formula operator()(const forall& x)
  {
    data::variable_list v = update_sigma.push(x.variables());
    state_formula result = forall(v, (*this)(x.body()));
    update_sigma.pop(v);
    return result;
  }

  state_formula operator()(const exists& x)
  {
    data::variable_list v = update_sigma.push(x.variables());
    state_formula result = exists(v, (*this)(x.body()));
    update_sigma.pop(v);
    return result;
  }

  add_capture_avoiding_replacement(Substitution& sigma, std::multiset<data::variable>& V)
    : super(sigma, V)
  { }
};

template <template <class> class Builder, class Substitution>
add_capture_avoiding_replacement<Builder, class Derived, Substitution>
make_add_capture_avoiding_replacement(Substitution& sigma, std::multiset<data::variable>& V)
{
  return add_capture_avoiding_replacement<Builder, Derived, Substitution>(sigma, V);
}
/// \endcond

} // namespace detail

//--- start generated state_formulas replace code ---//
template <typename T, typename Substitution>
void replace_sort_expressions(T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                             )
{
  data::detail::make_replace_sort_expressions_builder<state_formulas::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_sort_expressions(const T& x,
                           Substitution sigma,
                           bool innermost,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  return core::static_down_cast<const T&>(data::detail::make_replace_sort_expressions_builder<state_formulas::sort_expression_builder>(sigma, innermost)(x));
}

template <typename T, typename Substitution>
void replace_data_expressions(T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                             )
{
  data::detail::make_replace_data_expressions_builder<state_formulas::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_data_expressions(const T& x,
                           Substitution sigma,
                           bool innermost,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  return core::static_down_cast<const T&>(data::detail::make_replace_data_expressions_builder<state_formulas::data_expression_builder>(sigma, innermost)(x));
}

template <typename T, typename Substitution>
void replace_variables(T& x,
                       Substitution sigma,
                       typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                      )
{
  core::make_update_apply_builder<state_formulas::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_variables(const T& x,
                    Substitution sigma,
                    typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                   )
{
  return core::make_update_apply_builder<state_formulas::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
void replace_all_variables(T& x,
                           Substitution sigma,
                           typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  core::make_update_apply_builder<state_formulas::variable_builder>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_all_variables(const T& x,
                        Substitution sigma,
                        typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                       )
{
  return core::make_update_apply_builder<state_formulas::variable_builder>(sigma)(x);
}

/// \brief Applies the substitution sigma to x.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution>
void replace_free_variables(T& x,
                            Substitution sigma,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                           )
{
  assert(data::is_simple_substitution(sigma));
  data::detail::make_replace_free_variables_builder<state_formulas::data_expression_builder, state_formulas::add_data_variable_binding>(sigma)(x);
}

/// \brief Applies the substitution sigma to x.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution>
T replace_free_variables(const T& x,
                         Substitution sigma,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                        )
{
  assert(data::is_simple_substitution(sigma));
  return core::static_down_cast<const T&>(data::detail::make_replace_free_variables_builder<state_formulas::data_expression_builder, state_formulas::add_data_variable_binding>(sigma)(x));
}

/// \brief Applies the substitution sigma to x, where the elements of bound_variables are treated as bound variables.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution, typename VariableContainer>
void replace_free_variables(T& x,
                            Substitution sigma,
                            const VariableContainer& bound_variables,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                           )
{
  assert(data::is_simple_substitution(sigma));
  data::detail::make_replace_free_variables_builder<state_formulas::data_expression_builder, state_formulas::add_data_variable_binding>(sigma)(x, bound_variables);
}

/// \brief Applies the substitution sigma to x, where the elements of bound_variables are treated as bound variables.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution, typename VariableContainer>
T replace_free_variables(const T& x,
                         Substitution sigma,
                         const VariableContainer& bound_variables,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                        )
{
  assert(data::is_simple_substitution(sigma));
  return core::static_down_cast<const T&>(data::detail::make_replace_free_variables_builder<state_formulas::data_expression_builder, state_formulas::add_data_variable_binding>(sigma)(x, bound_variables));
}
//--- end generated state_formulas replace code ---//

//--- start generated state_formulas replace_capture_avoiding code ---//
/// \brief Applies sigma as a capture avoiding substitution to x
/// \param sigma A mutable substitution
/// \param sigma_variables a container of variables
/// \pre { sigma_variables must contain the free variables appearing in the right hand side of sigma }
template <typename T, typename Substitution, typename VariableContainer>
void replace_variables_capture_avoiding(T& x,
                       Substitution& sigma,
                       const VariableContainer& sigma_variables,
                       typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                      )
{
  std::multiset<data::variable> V;
  state_formulas::find_free_variables(x, std::inserter(V, V.end()));
  V.insert(sigma_variables.begin(), sigma_variables.end());
  data::detail::apply_replace_capture_avoiding_variables_builder<state_formulas::data_expression_builder, state_formulas::detail::add_capture_avoiding_replacement>(sigma, V)(x);
}

/// \brief Applies sigma as a capture avoiding substitution to x
/// \param sigma A mutable substitution
/// \param sigma_variables a container of variables
/// \pre { sigma_variables must contain the free variables appearing in the right hand side of sigma }
template <typename T, typename Substitution, typename VariableContainer>
T replace_variables_capture_avoiding(const T& x,
                    Substitution& sigma,
                    const VariableContainer& sigma_variables,
                    typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                   )
{
  std::multiset<data::variable> V;
  state_formulas::find_free_variables(x, std::inserter(V, V.end()));
  V.insert(sigma_variables.begin(), sigma_variables.end());
  return core::static_down_cast<const T&>(data::detail::apply_replace_capture_avoiding_variables_builder<state_formulas::data_expression_builder, state_formulas::detail::add_capture_avoiding_replacement>(sigma, V)(x));
}
//--- end generated state_formulas replace_capture_avoiding code ---//

namespace detail
{

/// \cond INTERNAL_DOCS
template <template <class> class Builder, class Substitution>
struct substitute_state_formulas_builder: public Builder<substitute_state_formulas_builder<Builder, Substitution> >
{
  typedef Builder<substitute_state_formulas_builder<Builder, Substitution> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  Substitution sigma;
  bool innermost;

  substitute_state_formulas_builder(Substitution sigma_, bool innermost_)
    : sigma(sigma_),
      innermost(innermost_)
  {}

  state_formula operator()(const state_formula& x)
  {
    if (innermost)
    {
      state_formula y = super::operator()(x);
      return sigma(y);
    }
    return sigma(x);
  }

#if BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif
};

template <template <class> class Builder, class Substitution>
substitute_state_formulas_builder<Builder, Substitution>
make_replace_state_formulas_builder(Substitution sigma, bool innermost)
{
  return substitute_state_formulas_builder<Builder, Substitution>(sigma, innermost);
}
/// \endcond

} // namespace detail

template <typename T, typename Substitution>
void replace_state_formulas(T& x,
                              Substitution sigma,
                              bool innermost = true,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                             )
{
  state_formulas::detail::make_replace_state_formulas_builder<state_formulas::state_formula_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_state_formulas(const T& x,
                           Substitution sigma,
                           bool innermost = true,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  return state_formulas::detail::make_replace_state_formulas_builder<state_formulas::state_formula_builder>(sigma, innermost)(x);
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_REPLACE_H
