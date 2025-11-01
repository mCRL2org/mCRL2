// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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
#include "mcrl2/modal_formula/replace_capture_avoiding.h"

namespace mcrl2
{

namespace action_formulas
{

//--- start generated action_formulas replace code ---//
template <typename T, typename Substitution>
void replace_sort_expressions(T& x,
                              const Substitution& sigma,
                              bool innermost
                             )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  data::detail::make_replace_sort_expressions_builder<action_formulas::sort_expression_builder>(sigma, innermost).update(x);
}

template <typename T, typename Substitution>
T replace_sort_expressions(const T& x,
                           const Substitution& sigma,
                           bool innermost
                          )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  T result;
  data::detail::make_replace_sort_expressions_builder<action_formulas::sort_expression_builder>(sigma, innermost).apply(result, x);
  return result;
}

template <typename T, typename Substitution>
void replace_data_expressions(T& x,
                              const Substitution& sigma,
                              bool innermost
                             )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  data::detail::make_replace_data_expressions_builder<action_formulas::data_expression_builder>(sigma, innermost).update(x);
}

template <typename T, typename Substitution>
T replace_data_expressions(const T& x,
                           const Substitution& sigma,
                           bool innermost
                          )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  T result;
  data::detail::make_replace_data_expressions_builder<action_formulas::data_expression_builder>(sigma, innermost).apply(result, x);
  return result;
}


template <typename T, typename Substitution>
void replace_variables(T& x,
                       const Substitution& sigma
                      )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  core::make_update_apply_builder<action_formulas::data_expression_builder>(sigma).update(x);
}

template <typename T, typename Substitution>
T replace_variables(const T& x,
                    const Substitution& sigma
                   )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  T result;
  core::make_update_apply_builder<action_formulas::data_expression_builder>(sigma).apply(result, x);
  return result;
}

/* Replace all variables, including those in binders and the left hand side of assignments */
template <typename T, typename Substitution>
void replace_all_variables(T& x,
                           const Substitution& sigma
                          )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  core::make_update_apply_builder<action_formulas::sort_expression_builder>(sigma).update(x);
}

template <typename T, typename Substitution>
T replace_all_variables(const T& x,
                        const Substitution& sigma
                       )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  T result;
  core::make_update_apply_builder<action_formulas::sort_expression_builder>(sigma).apply(result, x);
  return result;
}

/// \\brief Applies the substitution sigma to x.
/// \\pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution>
void replace_free_variables(T& x,
                            const Substitution& sigma
                           )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  assert(data::is_simple_substitution(sigma));
  data::detail::make_replace_free_variables_builder<action_formulas::data_expression_builder, action_formulas::add_data_variable_builder_binding>(sigma).update(x);
}

/// \\brief Applies the substitution sigma to x.
/// \\pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution>
T replace_free_variables(const T& x,
                         const Substitution& sigma
                        )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  assert(data::is_simple_substitution(sigma));
  T result;
  data::detail::make_replace_free_variables_builder<action_formulas::data_expression_builder, action_formulas::add_data_variable_builder_binding>(sigma).apply(result, x);
  return result;
}

/// \\brief Applies the substitution sigma to x, where the elements of bound_variables are treated as bound variables.
/// \\pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution, typename VariableContainer>
void replace_free_variables(T& x,
                            const Substitution& sigma,
                            const VariableContainer& bound_variables
                           )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  assert(data::is_simple_substitution(sigma));
  data::detail::make_replace_free_variables_builder<action_formulas::data_expression_builder, action_formulas::add_data_variable_builder_binding>(sigma).update(x, bound_variables);
}

/// \\brief Applies the substitution sigma to x, where the elements of bound_variables are treated as bound variables.
/// \\pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution, typename VariableContainer>
T replace_free_variables(const T& x,
                         const Substitution& sigma,
                         const VariableContainer& bound_variables
                        )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  assert(data::is_simple_substitution(sigma));
  T result;
  data::detail::make_replace_free_variables_builder<action_formulas::data_expression_builder, action_formulas::add_data_variable_builder_binding>(sigma).apply(result, x, bound_variables);
  return result;
}
//--- end generated action_formulas replace code ---//

} // namespace action_formulas

namespace regular_formulas
{

//--- start generated regular_formulas replace code ---//
template <typename T, typename Substitution>
void replace_sort_expressions(T& x,
                              const Substitution& sigma,
                              bool innermost
                             )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  data::detail::make_replace_sort_expressions_builder<regular_formulas::sort_expression_builder>(sigma, innermost).update(x);
}

template <typename T, typename Substitution>
T replace_sort_expressions(const T& x,
                           const Substitution& sigma,
                           bool innermost
                          )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  T result;
  data::detail::make_replace_sort_expressions_builder<regular_formulas::sort_expression_builder>(sigma, innermost).apply(result, x);
  return result;
}

template <typename T, typename Substitution>
void replace_data_expressions(T& x,
                              const Substitution& sigma,
                              bool innermost
                             )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  data::detail::make_replace_data_expressions_builder<regular_formulas::data_expression_builder>(sigma, innermost).update(x);
}

template <typename T, typename Substitution>
T replace_data_expressions(const T& x,
                           const Substitution& sigma,
                           bool innermost
                          )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  T result;
  data::detail::make_replace_data_expressions_builder<regular_formulas::data_expression_builder>(sigma, innermost).apply(result, x);
  return result;
}


template <typename T, typename Substitution>
void replace_variables(T& x,
                       const Substitution& sigma
                      )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  core::make_update_apply_builder<regular_formulas::data_expression_builder>(sigma).update(x);
}

template <typename T, typename Substitution>
T replace_variables(const T& x,
                    const Substitution& sigma
                   )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  T result;
  core::make_update_apply_builder<regular_formulas::data_expression_builder>(sigma).apply(result, x);
  return result;
}

/* Replace all variables, including those in binders and the left hand side of assignments */
template <typename T, typename Substitution>
void replace_all_variables(T& x,
                           const Substitution& sigma
                          )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  core::make_update_apply_builder<regular_formulas::sort_expression_builder>(sigma).update(x);
}

template <typename T, typename Substitution>
T replace_all_variables(const T& x,
                        const Substitution& sigma
                       )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  T result;
  core::make_update_apply_builder<regular_formulas::sort_expression_builder>(sigma).apply(result, x);
  return result;
}

/// \\brief Applies the substitution sigma to x.
/// \\pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution>
void replace_free_variables(T& x,
                            const Substitution& sigma
                           )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  assert(data::is_simple_substitution(sigma));
  data::detail::make_replace_free_variables_builder<regular_formulas::data_expression_builder, regular_formulas::add_data_variable_builder_binding>(sigma).update(x);
}

/// \\brief Applies the substitution sigma to x.
/// \\pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution>
T replace_free_variables(const T& x,
                         const Substitution& sigma
                        )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  assert(data::is_simple_substitution(sigma));
  T result;
  data::detail::make_replace_free_variables_builder<regular_formulas::data_expression_builder, regular_formulas::add_data_variable_builder_binding>(sigma).apply(result, x);
  return result;
}

/// \\brief Applies the substitution sigma to x, where the elements of bound_variables are treated as bound variables.
/// \\pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution, typename VariableContainer>
void replace_free_variables(T& x,
                            const Substitution& sigma,
                            const VariableContainer& bound_variables
                           )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  assert(data::is_simple_substitution(sigma));
  data::detail::make_replace_free_variables_builder<regular_formulas::data_expression_builder, regular_formulas::add_data_variable_builder_binding>(sigma).update(x, bound_variables);
}

/// \\brief Applies the substitution sigma to x, where the elements of bound_variables are treated as bound variables.
/// \\pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution, typename VariableContainer>
T replace_free_variables(const T& x,
                         const Substitution& sigma,
                         const VariableContainer& bound_variables
                        )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  assert(data::is_simple_substitution(sigma));
  T result;
  data::detail::make_replace_free_variables_builder<regular_formulas::data_expression_builder, regular_formulas::add_data_variable_builder_binding>(sigma).apply(result, x, bound_variables);
  return result;
}
//--- end generated regular_formulas replace code ---//

} // namespace regular_formulas

namespace state_formulas
{

//--- start generated state_formulas replace code ---//
template <typename T, typename Substitution>
void replace_sort_expressions(T& x,
                              const Substitution& sigma,
                              bool innermost
                             )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  data::detail::make_replace_sort_expressions_builder<state_formulas::sort_expression_builder>(sigma, innermost).update(x);
}

template <typename T, typename Substitution>
T replace_sort_expressions(const T& x,
                           const Substitution& sigma,
                           bool innermost
                          )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  T result;
  data::detail::make_replace_sort_expressions_builder<state_formulas::sort_expression_builder>(sigma, innermost).apply(result, x);
  return result;
}

template <typename T, typename Substitution>
void replace_data_expressions(T& x,
                              const Substitution& sigma,
                              bool innermost
                             )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  data::detail::make_replace_data_expressions_builder<state_formulas::data_expression_builder>(sigma, innermost).update(x);
}

template <typename T, typename Substitution>
T replace_data_expressions(const T& x,
                           const Substitution& sigma,
                           bool innermost
                          )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  T result;
  data::detail::make_replace_data_expressions_builder<state_formulas::data_expression_builder>(sigma, innermost).apply(result, x);
  return result;
}


template <typename T, typename Substitution>
void replace_variables(T& x,
                       const Substitution& sigma
                      )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  core::make_update_apply_builder<state_formulas::data_expression_builder>(sigma).update(x);
}

template <typename T, typename Substitution>
T replace_variables(const T& x,
                    const Substitution& sigma
                   )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  T result;
  core::make_update_apply_builder<state_formulas::data_expression_builder>(sigma).apply(result, x);
  return result;
}

/* Replace all variables, including those in binders and the left hand side of assignments */
template <typename T, typename Substitution>
void replace_all_variables(T& x,
                           const Substitution& sigma
                          )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  core::make_update_apply_builder<state_formulas::sort_expression_builder>(sigma).update(x);
}

template <typename T, typename Substitution>
T replace_all_variables(const T& x,
                        const Substitution& sigma
                       )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  T result;
  core::make_update_apply_builder<state_formulas::sort_expression_builder>(sigma).apply(result, x);
  return result;
}

/// \\brief Applies the substitution sigma to x.
/// \\pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution>
void replace_free_variables(T& x,
                            const Substitution& sigma
                           )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  assert(data::is_simple_substitution(sigma));
  data::detail::make_replace_free_variables_builder<state_formulas::data_expression_builder, state_formulas::add_data_variable_builder_binding>(sigma).update(x);
}

/// \\brief Applies the substitution sigma to x.
/// \\pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution>
T replace_free_variables(const T& x,
                         const Substitution& sigma
                        )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  assert(data::is_simple_substitution(sigma));
  T result;
  data::detail::make_replace_free_variables_builder<state_formulas::data_expression_builder, state_formulas::add_data_variable_builder_binding>(sigma).apply(result, x);
  return result;
}

/// \\brief Applies the substitution sigma to x, where the elements of bound_variables are treated as bound variables.
/// \\pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution, typename VariableContainer>
void replace_free_variables(T& x,
                            const Substitution& sigma,
                            const VariableContainer& bound_variables
                           )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  assert(data::is_simple_substitution(sigma));
  data::detail::make_replace_free_variables_builder<state_formulas::data_expression_builder, state_formulas::add_data_variable_builder_binding>(sigma).update(x, bound_variables);
}

/// \\brief Applies the substitution sigma to x, where the elements of bound_variables are treated as bound variables.
/// \\pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution, typename VariableContainer>
T replace_free_variables(const T& x,
                         const Substitution& sigma,
                         const VariableContainer& bound_variables
                        )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  assert(data::is_simple_substitution(sigma));
  T result;
  data::detail::make_replace_free_variables_builder<state_formulas::data_expression_builder, state_formulas::add_data_variable_builder_binding>(sigma).apply(result, x, bound_variables);
  return result;
}
//--- end generated state_formulas replace code ---//

namespace detail
{

/// \cond INTERNAL_DOCS
template <template <class> class Builder, class Substitution>
struct substitute_state_formulas_builder: public Builder<substitute_state_formulas_builder<Builder, Substitution> >
{
  using super = Builder<substitute_state_formulas_builder<Builder, Substitution>>;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  Substitution sigma;
  bool innermost;

  substitute_state_formulas_builder(Substitution sigma_, bool innermost_)
    : sigma(sigma_),
      innermost(innermost_)
  {}

  template <class T>
  void apply(T& result, const state_formula& x)
  {
    if (innermost)
    {
      state_formula y;
      super::apply(y, x);
      result = sigma(y);
      return;
    }
    result = sigma(x);
  }
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
    std::enable_if_t<!std::is_base_of_v<atermpp::aterm, T>>* = 0)
{
  state_formulas::detail::make_replace_state_formulas_builder<state_formulas::state_formula_builder>(sigma, innermost).update(x);
}

template <typename T, typename Substitution>
T replace_state_formulas(const T& x,
    Substitution sigma,
    bool innermost = true,
    std::enable_if_t<std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  T result;
  state_formulas::detail::make_replace_state_formulas_builder<state_formulas::state_formula_builder>(sigma, innermost).apply(result, x);
  return result;
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_REPLACE_H
