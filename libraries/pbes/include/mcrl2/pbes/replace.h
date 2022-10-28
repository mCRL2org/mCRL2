// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/replace.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REPLACE_H
#define MCRL2_PBES_REPLACE_H

#include "mcrl2/data/replace.h"
#include "mcrl2/pbes/replace_capture_avoiding.h"

namespace mcrl2
{

namespace pbes_system
{

namespace detail {

/// \cond INTERNAL_DOCS
template <template <class> class Builder, class Substitution>
struct substitute_pbes_expressions_builder: public Builder<substitute_pbes_expressions_builder<Builder, Substitution> >
{
  typedef Builder<substitute_pbes_expressions_builder<Builder, Substitution> > super;
  using super::apply;

  Substitution sigma;
  bool innermost;

  substitute_pbes_expressions_builder(Substitution sigma_, bool innermost_)
    : sigma(sigma_),
      innermost(innermost_)
  {}

  template <class T>
  void apply(T& result, const pbes_expression& x)
  {
    if (innermost)
    {
      pbes_expression y;
      super::apply(y, x);
      result = sigma(y);
      return;
    }
    result = sigma(x);
  }
};

template <template <class> class Builder, class Substitution>
substitute_pbes_expressions_builder<Builder, Substitution>
make_replace_pbes_expressions_builder(Substitution sigma, bool innermost)
{
  return substitute_pbes_expressions_builder<Builder, Substitution>(sigma, innermost);
}

template <template <class> class Builder, class Substitution>
struct replace_propositional_variables_builder: public Builder<replace_propositional_variables_builder<Builder, Substitution> >
{
  typedef Builder<replace_propositional_variables_builder<Builder, Substitution> > super;
  using super::apply;

  const Substitution& sigma;

  explicit replace_propositional_variables_builder(const Substitution& sigma_)
    : sigma(sigma_)
  {}

  template <class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    result = sigma(x);
  }
};

template <template <class> class Builder, class Substitution>
replace_propositional_variables_builder<Builder, Substitution>
make_replace_propositional_variables_builder(const Substitution& sigma)
{
  return replace_propositional_variables_builder<Builder, Substitution>(sigma);
}


/// \endcond

} // namespace detail

//--- start generated pbes_system replace code ---//
template <typename T, typename Substitution>
void replace_sort_expressions(T& x,
                              const Substitution& sigma,
                              bool innermost,
                              typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                             )
{
  data::detail::make_replace_sort_expressions_builder<pbes_system::sort_expression_builder>(sigma, innermost).update(x);
}

template <typename T, typename Substitution>
T replace_sort_expressions(const T& x,
                           const Substitution& sigma,
                           bool innermost,
                           typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                          )
{
  T result;
  data::detail::make_replace_sort_expressions_builder<pbes_system::sort_expression_builder>(sigma, innermost).apply(result, x);
  return result;
}

template <typename T, typename Substitution>
void replace_data_expressions(T& x,
                              const Substitution& sigma,
                              bool innermost,
                              typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                             )
{
  data::detail::make_replace_data_expressions_builder<pbes_system::data_expression_builder>(sigma, innermost).update(x);
}

template <typename T, typename Substitution>
T replace_data_expressions(const T& x,
                           const Substitution& sigma,
                           bool innermost,
                           typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                          )
{
  T result;
  data::detail::make_replace_data_expressions_builder<pbes_system::data_expression_builder>(sigma, innermost).apply(result, x);
  return result;
}


template <typename T, typename Substitution>
void replace_variables(T& x,
                       const Substitution& sigma,
                       typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                      )
{
  core::make_update_apply_builder<pbes_system::data_expression_builder>(sigma).update(x);
}

template <typename T, typename Substitution>
T replace_variables(const T& x,
                    const Substitution& sigma,
                    typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                   )
{
  T result;
  core::make_update_apply_builder<pbes_system::data_expression_builder>(sigma).apply(result, x);
  return result;
}

template <typename T, typename Substitution>
void replace_all_variables(T& x,
                           const Substitution& sigma,
                           typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                          )
{
  core::make_update_apply_builder<pbes_system::variable_builder>(sigma).update(x);
}

template <typename T, typename Substitution>
T replace_all_variables(const T& x,
                        const Substitution& sigma,
                        typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                       )
{
  T result;
  core::make_update_apply_builder<pbes_system::variable_builder>(sigma).apply(result, x);
  return result;
}

/// \\brief Applies the substitution sigma to x.
/// \\pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution>
void replace_free_variables(T& x,
                            const Substitution& sigma,
                            typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                           )
{
  assert(data::is_simple_substitution(sigma));
  data::detail::make_replace_free_variables_builder<pbes_system::data_expression_builder, pbes_system::add_data_variable_builder_binding>(sigma).update(x);
}

/// \\brief Applies the substitution sigma to x.
/// \\pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution>
T replace_free_variables(const T& x,
                         const Substitution& sigma,
                         typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                        )
{
  assert(data::is_simple_substitution(sigma));
  T result;
  data::detail::make_replace_free_variables_builder<pbes_system::data_expression_builder, pbes_system::add_data_variable_builder_binding>(sigma).apply(result, x);
  return result;
}

/// \\brief Applies the substitution sigma to x, where the elements of bound_variables are treated as bound variables.
/// \\pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution, typename VariableContainer>
void replace_free_variables(T& x,
                            const Substitution& sigma,
                            const VariableContainer& bound_variables,
                            typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                           )
{
  assert(data::is_simple_substitution(sigma));
  data::detail::make_replace_free_variables_builder<pbes_system::data_expression_builder, pbes_system::add_data_variable_builder_binding>(sigma).update(x, bound_variables);
}

/// \\brief Applies the substitution sigma to x, where the elements of bound_variables are treated as bound variables.
/// \\pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution, typename VariableContainer>
T replace_free_variables(const T& x,
                         const Substitution& sigma,
                         const VariableContainer& bound_variables,
                         typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                        )
{
  assert(data::is_simple_substitution(sigma));
  T result;
  data::detail::make_replace_free_variables_builder<pbes_system::data_expression_builder, pbes_system::add_data_variable_builder_binding>(sigma).apply(result, x, bound_variables);
  return result;
}
//--- end generated pbes_system replace code ---//

/// \brief Applies a propositional variable substitution.
template <typename T, typename Substitution>
void replace_propositional_variables(T& x,
                                     const Substitution& sigma,
                                     typename std::enable_if< !std::is_base_of< atermpp::aterm, T >::value>::type* = nullptr
                                    )
{
  pbes_system::detail::make_replace_propositional_variables_builder<pbes_system::pbes_expression_builder>(sigma).update(x);
}

/// \brief Applies a propositional variable substitution.
template <typename T, typename Substitution>
T replace_propositional_variables(const T& x,
                                  const Substitution& sigma,
                                  typename std::enable_if<std::is_base_of< atermpp::aterm, T>::value>::type* = nullptr
                                 )
{
  T result;
  pbes_system::detail::make_replace_propositional_variables_builder<pbes_system::pbes_expression_builder>(sigma).apply(result, x);
  return result;
}

/// \brief Applies a propositional variable substitution.
template <typename T, typename Substitution>
void replace_propositional_variables(T& result,
                                     const T& x,
                                     const Substitution& sigma,
                                     typename std::enable_if<std::is_base_of< atermpp::aterm, T>::value>::type* = nullptr
                                    )
{
  pbes_system::detail::make_replace_propositional_variables_builder<pbes_system::pbes_expression_builder>(sigma).apply(result, x);
}

template <typename T, typename Substitution>
void replace_pbes_expressions(T& x,
                              const Substitution& sigma,
                              bool innermost = true,
                              typename std::enable_if< !std::is_base_of< atermpp::aterm, T >::value>::type* = 0
                             )
{
  pbes_system::detail::make_replace_pbes_expressions_builder<pbes_system::pbes_expression_builder>(sigma, innermost).update(x);
}

template <typename T, typename Substitution>
T replace_pbes_expressions(const T& x,
                           const Substitution& sigma,
                           bool innermost = true,
                           typename std::enable_if< std::is_base_of< atermpp::aterm, T >::value>::type* = nullptr
                          )
{
  T result;
  pbes_system::detail::make_replace_pbes_expressions_builder<pbes_system::pbes_expression_builder>(sigma, innermost).apply(result, x);
  return result;
}

} // namespace pbes_system

} // namespace mcrl2

#ifndef MCRL2_PBES_SUBSTITUTIONS_H
#include "mcrl2/pbes/substitutions.h"
#endif

#endif // MCRL2_PBES_REPLACE_H
