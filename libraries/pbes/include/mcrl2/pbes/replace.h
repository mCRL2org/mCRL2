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
#include "mcrl2/pbes/find.h"
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

  void operator()(pbes& x)
  {
    std::set<data::variable> v = update_sigma(x.global_variables());
    x.global_variables() = v;
    (*this)(x.equations());
    update_sigma.pop(v);
  }

  add_capture_avoiding_replacement(Substitution& sigma, std::multiset<data::variable>& V)
    : super(sigma, V)
  { }
};

template <template <class> class Builder, class Substitution>
struct substitute_pbes_expressions_builder: public Builder<substitute_pbes_expressions_builder<Builder, Substitution> >
{
  typedef Builder<substitute_pbes_expressions_builder<Builder, Substitution> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  Substitution sigma;
  bool innermost;

  substitute_pbes_expressions_builder(Substitution sigma_, bool innermost_)
    : sigma(sigma_),
      innermost(innermost_)
  {}

  pbes_expression operator()(const pbes_expression& x)
  {
    if (innermost)
    {
      pbes_expression y = super::operator()(x);
      return sigma(y);
    }
    return sigma(x);
  }

#if BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif
};

template <template <class> class Builder, class Substitution>
substitute_pbes_expressions_builder<Builder, Substitution>
make_replace_pbes_expressions_builder(Substitution sigma, bool innermost)
{
  return substitute_pbes_expressions_builder<Builder, Substitution>(sigma, innermost);
}
/// \endcond

} // namespace detail

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
  return core::static_down_cast<const T&>(data::detail::make_replace_sort_expressions_builder<pbes_system::sort_expression_builder>(sigma, innermost)(x));
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
  return core::static_down_cast<const T&>(data::detail::make_replace_data_expressions_builder<pbes_system::data_expression_builder>(sigma, innermost)(x));
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
void replace_all_variables(T& x,
                           Substitution sigma,
                           typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  core::make_update_apply_builder<pbes_system::variable_builder>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_all_variables(const T& x,
                        Substitution sigma,
                        typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                       )
{
  return core::make_update_apply_builder<pbes_system::variable_builder>(sigma)(x);
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
  data::detail::make_replace_free_variables_builder<pbes_system::data_expression_builder, pbes_system::add_data_variable_binding>(sigma)(x);
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
  return core::static_down_cast<const T&>(data::detail::make_replace_free_variables_builder<pbes_system::data_expression_builder, pbes_system::add_data_variable_binding>(sigma)(x));
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
  data::detail::make_replace_free_variables_builder<pbes_system::data_expression_builder, pbes_system::add_data_variable_binding>(sigma)(x, bound_variables);
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
  return core::static_down_cast<const T&>(data::detail::make_replace_free_variables_builder<pbes_system::data_expression_builder, pbes_system::add_data_variable_binding>(sigma)(x, bound_variables));
}
//--- end generated pbes_system replace code ---//

//--- start generated pbes_system replace_capture_avoiding code ---//
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
  pbes_system::find_free_variables(x, std::inserter(V, V.end()));
  V.insert(sigma_variables.begin(), sigma_variables.end());
  data::detail::apply_replace_capture_avoiding_variables_builder<pbes_system::data_expression_builder, pbes_system::detail::add_capture_avoiding_replacement>(sigma, V)(x);
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
  pbes_system::find_free_variables(x, std::inserter(V, V.end()));
  V.insert(sigma_variables.begin(), sigma_variables.end());
  return core::static_down_cast<const T&>(data::detail::apply_replace_capture_avoiding_variables_builder<pbes_system::data_expression_builder, pbes_system::detail::add_capture_avoiding_replacement>(sigma, V)(x));
}
//--- end generated pbes_system replace_capture_avoiding code ---//

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

template <typename T, typename Substitution>
void replace_pbes_expressions(T& x,
                              Substitution sigma,
                              bool innermost = true,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                             )
{
  pbes_system::detail::make_replace_pbes_expressions_builder<pbes_system::pbes_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_pbes_expressions(const T& x,
                           Substitution sigma,
                           bool innermost = true,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  return pbes_system::detail::make_replace_pbes_expressions_builder<pbes_system::pbes_expression_builder>(sigma, innermost)(x);
}

} // namespace pbes_system

} // namespace mcrl2

#ifndef MCRL2_PBES_SUBSTITUTIONS_H
#include "mcrl2/pbes/substitutions.h"
#endif

#endif // MCRL2_PBES_REPLACE_H
