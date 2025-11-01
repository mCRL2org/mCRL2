// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/replace.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_REPLACE_H
#define MCRL2_PROCESS_REPLACE_H

#include "mcrl2/data/replace.h"
#include "mcrl2/process/replace_capture_avoiding.h"

namespace mcrl2::process
{

//--- start generated process replace code ---//
template <typename T, typename Substitution>
void replace_sort_expressions(T& x,
                              const Substitution& sigma,
                              bool innermost
                             )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  data::detail::make_replace_sort_expressions_builder<process::sort_expression_builder>(sigma, innermost).update(x);
}

template <typename T, typename Substitution>
T replace_sort_expressions(const T& x,
                           const Substitution& sigma,
                           bool innermost
                          )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  T result;
  data::detail::make_replace_sort_expressions_builder<process::sort_expression_builder>(sigma, innermost).apply(result, x);
  return result;
}

template <typename T, typename Substitution>
void replace_data_expressions(T& x,
                              const Substitution& sigma,
                              bool innermost
                             )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  data::detail::make_replace_data_expressions_builder<process::data_expression_builder>(sigma, innermost).update(x);
}

template <typename T, typename Substitution>
T replace_data_expressions(const T& x,
                           const Substitution& sigma,
                           bool innermost
                          )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  T result;
  data::detail::make_replace_data_expressions_builder<process::data_expression_builder>(sigma, innermost).apply(result, x);
  return result;
}


template <typename T, typename Substitution>
void replace_variables(T& x,
                       const Substitution& sigma
                      )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  core::make_update_apply_builder<process::data_expression_builder>(sigma).update(x);
}

template <typename T, typename Substitution>
T replace_variables(const T& x,
                    const Substitution& sigma
                   )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  T result;
  core::make_update_apply_builder<process::data_expression_builder>(sigma).apply(result, x);
  return result;
}

/* Replace all variables, including those in binders and the left hand side of assignments */
template <typename T, typename Substitution>
void replace_all_variables(T& x,
                           const Substitution& sigma
                          )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  core::make_update_apply_builder<process::sort_expression_builder>(sigma).update(x);
}

template <typename T, typename Substitution>
T replace_all_variables(const T& x,
                        const Substitution& sigma
                       )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  T result;
  core::make_update_apply_builder<process::sort_expression_builder>(sigma).apply(result, x);
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
  data::detail::make_replace_free_variables_builder<process::data_expression_builder, process::add_data_variable_builder_binding>(sigma).update(x);
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
  data::detail::make_replace_free_variables_builder<process::data_expression_builder, process::add_data_variable_builder_binding>(sigma).apply(result, x);
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
  data::detail::make_replace_free_variables_builder<process::data_expression_builder, process::add_data_variable_builder_binding>(sigma).update(x, bound_variables);
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
  data::detail::make_replace_free_variables_builder<process::data_expression_builder, process::add_data_variable_builder_binding>(sigma).apply(result, x, bound_variables);
  return result;
}
//--- end generated process replace code ---//

struct process_identifier_assignment
{
  process_identifier lhs;
  process_identifier rhs;

  using result_type = process_identifier;
  using argument_type = process_identifier;

  process_identifier_assignment(const process_identifier& lhs_, const process_identifier& rhs_)
    : lhs(lhs_), rhs(rhs_)
  {}

  process_identifier operator()(const process_identifier& x) const
  {
    if (x == lhs)
    {
      return rhs;
    }
    return x;
  }
};

template <typename T, typename Substitution>
void replace_process_identifiers(T& x,
    const Substitution& sigma,
    std::enable_if_t<!std::is_base_of_v<atermpp::aterm, T>>* = 0)
{
  core::make_update_apply_builder<process::process_identifier_builder>(sigma).update(x);
}

template <typename T, typename Substitution>
T replace_process_identifiers(const T& x,
    const Substitution& sigma,
    std::enable_if_t<std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  T result;
  core::make_update_apply_builder<process::process_identifier_builder>(sigma).apply(result, x);
  return result;
}

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_REPLACE_H
