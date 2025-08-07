// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/quantifiers_inside_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_REWRITERS_QUANTIFIERS_INSIDE_REWRITER_H
#define MCRL2_DATA_REWRITERS_QUANTIFIERS_INSIDE_REWRITER_H

#include <tuple>
#include "mcrl2/data/builder.h"
#include "mcrl2/data/join.h"
#include "mcrl2/data/optimized_boolean_operators.h"

namespace mcrl2::data
{

namespace detail {

data_expression quantifiers_inside(const data_expression& x);
data_expression quantifiers_inside_forall(const std::set<variable>& variables, const data_expression& x);
data_expression quantifiers_inside_exists(const std::set<variable>& variables, const data_expression& x);

inline
std::set<variable> make_variable_set(const variable_list& x)
{
  return std::set<variable>(x.begin(), x.end());
}

inline
variable_list make_variable_list(const std::set<variable>& x)
{
  return variable_list(x.begin(), x.end());
}

template <typename BinaryOperation>
std::tuple<data_expression, data_expression> compute_Phi_Psi(const std::vector<data_expression>& X, const std::set<variable>& V, BinaryOperation op, data_expression empty_sequence_result)
{
  using utilities::detail::set_difference;
  using utilities::detail::set_intersection;

  std::vector<std::set<data::variable>> vars; // free variables
  for (const data_expression& x_j: X)
  {
    vars.push_back(set_intersection(find_free_variables(x_j), V));
  }
  auto j = std::min_element(vars.begin(), vars.end(),
                            [&](const std::set<data::variable>& x, const std::set<data::variable>& y)
                            {
                              return x.size() < y.size();
                            }
  );
  const std::set<data::variable>& Z = *j;

  std::vector<data_expression> phi;
  std::vector<data_expression> psi;
  for (std::size_t i = 0; i < X.size(); i++)
  {
    if (std::includes(Z.begin(), Z.end(), vars[i].begin(), vars[i].end()))
    {
      phi.push_back(X[i]);
    }
    else
    {
      psi.push_back(X[i]);
    }
  }
  data_expression Phi = utilities::detail::join(phi.begin(), phi.end(), op, empty_sequence_result);
  data_expression Psi = utilities::detail::join(psi.begin(), psi.end(), op, empty_sequence_result);
  return { Phi, Psi };
}

struct quantifiers_inside_builder: public data_expression_builder<quantifiers_inside_builder>
{
  using super = data_expression_builder<quantifiers_inside_builder>;
  using super::apply;

  template <class T>
  void apply(T& result, const forall& x)
  {
    const data_expression& phi = x.body();
    std::set<variable> W = make_variable_set(x.variables());
    apply(result, phi);
    result = quantifiers_inside_forall(W, result);
  }

  template <class T>
  void apply(T& result, const exists& x)
  {
    const data_expression& phi = x.body();
    std::set<variable> W = make_variable_set(x.variables());
    apply(result, phi);
    result = quantifiers_inside_exists(W, result);
  }
};

struct quantifiers_inside_forall_builder: public data_expression_builder<quantifiers_inside_forall_builder>
{
  using super = data_expression_builder<quantifiers_inside_forall_builder>;
  using super::apply;

  const std::set<variable>& V;

  explicit quantifiers_inside_forall_builder(const std::set<variable>& V_)
    : V(V_)
  {}

  static data_expression make_forall_(const variable_list& vars, const data_expression& body)
  {
    return vars.empty() ? body : forall(vars,body);
  }

  template <class T>
  void apply(T& result, const forall& x)
  {
    using utilities::detail::set_union;
    std::set<variable> W = make_variable_set(x.variables());
    result = quantifiers_inside_forall(set_union(V, W), x.body());
  }

  // default case
  template <typename T>
  data_expression apply_default(const T& x)
  {
    using utilities::detail::set_intersection;
    std::set<variable> W = set_intersection(V, find_free_variables(x));
    return make_forall_(variable_list(W.begin(), W.end()), x);
  }

  template <class T>
  void apply(T& result, const data_expression& x)
  {
    if (sort_bool::is_not_application(x))
    {
      data_expression const& phi = sort_bool::arg(x);
      result = sort_bool::not_(quantifiers_inside_exists(V, phi));
      return;
    }
    else if (sort_bool::is_and_application(x))
    {
      const data_expression& phi = sort_bool::left(x);
      const data_expression& psi = sort_bool::right(x);
      result = sort_bool::and_(quantifiers_inside_forall(V, phi), quantifiers_inside_forall(V, psi));
      return;
    }
    else if (sort_bool::is_or_application(x))
    {
      using utilities::detail::set_difference;
      using utilities::detail::set_intersection;
      using tr = core::term_traits<data::data_expression>;

      std::vector<data_expression> X;
      utilities::detail::split(x, std::back_inserter(X), tr::is_or, tr::left, tr::right);
      const auto [Phi, Psi] = compute_Phi_Psi(X, V, tr::or_, tr::false_());
      if (sort_bool::is_false_function_symbol(Psi))
      {
        result = make_forall_(make_variable_list(set_intersection(V,find_free_variables(x))), x);
        return;
      }
      std::set<variable> vars_Phi = find_free_variables(Phi);
      std::set<variable> vars_Psi = find_free_variables(Psi);
      result = make_forall_(make_variable_list(set_intersection(V, set_intersection(vars_Phi, vars_Psi))),
                         lazy::or_(
                           quantifiers_inside_forall(set_difference(set_intersection(V, vars_Phi), vars_Psi), Phi),
                           quantifiers_inside_forall(set_difference(set_intersection(V, vars_Psi), vars_Phi), Psi)
                              )
      );
      return;
    }
    else if (sort_bool::is_implies_application(x))
    {
      const data_expression& left = sort_bool::left(x);
      const data_expression& right = sort_bool::right(x);
      result = quantifiers_inside_forall(V, sort_bool::or_(sort_bool::not_(left), right));
      return;
    }
    else
    {
      result = apply_default(x);
      return;
    }
  }
};

struct quantifiers_inside_exists_builder: public data_expression_builder<quantifiers_inside_exists_builder>
{
  using super = data_expression_builder<quantifiers_inside_exists_builder>;
  using super::apply;

  const std::set<variable>& V;

  quantifiers_inside_exists_builder(const std::set<variable>& V_)
    : V(V_)
  {}

  data_expression make_exists_(const variable_list& vars, const data_expression& body)
  {
    return vars.empty() ? body : exists(vars,body);
  }

  template <class T>
  void apply(T& result, const exists& x)
  {
    using utilities::detail::set_union;
    std::set<variable> W = make_variable_set(x.variables());
    result = quantifiers_inside_exists(set_union(V, W), x.body());
  }

  // default case
  template <typename T>
  data_expression apply_default(const T& x)
  {
    using utilities::detail::set_intersection;
    std::set<variable> W = set_intersection(V, find_free_variables(x));
    return make_exists_(variable_list(W.begin(), W.end()), x);
  }

  template <class T>
  void apply(T& result, const forall& x)
  {
    result = apply_default(x);
  }

  template <class T>
  void apply(T& result, const data_expression& x)
  {
    if (sort_bool::is_not_application(x))
    {
      data_expression const& phi = sort_bool::arg(x);
      result = sort_bool::not_(quantifiers_inside_forall(V, phi));
      return;
    }
    else if (sort_bool::is_and_application(x))
    {
      using utilities::detail::set_difference;
      using utilities::detail::set_intersection;
      using tr = core::term_traits<data::data_expression>;

      std::vector<data_expression> X;
      utilities::detail::split(x, std::back_inserter(X), tr::is_and, tr::left, tr::right);
      const auto [Phi, Psi] = compute_Phi_Psi(X, V, tr::and_, tr::true_());
      if (sort_bool::is_true_function_symbol(Psi))
      {
        result = make_exists_(make_variable_list(set_intersection(V, find_free_variables(x))), x);
        return;
      }
      std::set<variable> vars_Phi = find_free_variables(Phi);
      std::set<variable> vars_Psi = find_free_variables(Psi);
      result = make_exists_(make_variable_list(set_intersection(V, set_intersection(vars_Phi, vars_Psi))),
                         lazy::and_(
                           quantifiers_inside_exists(set_difference(set_intersection(V, vars_Phi), vars_Psi), Phi),
                           quantifiers_inside_exists(set_difference(set_intersection(V, vars_Psi), vars_Phi), Psi)
                         )
      );
      return;
    }
    else if (sort_bool::is_or_application(x))
    {
      data_expression const& phi = sort_bool::left(x);
      data_expression const& psi = sort_bool::right(x);
      result = sort_bool::or_(quantifiers_inside_exists(V, phi), quantifiers_inside_exists(V, psi));
      return;
    }
    else if (sort_bool::is_implies_application(x))
    {
      const data_expression& left = sort_bool::left(x);
      const data_expression& right = sort_bool::right(x);
      result = quantifiers_inside_exists(V, sort_bool::or_(sort_bool::not_(left), right));
      return;
    }
    result = apply_default(x);
    return;
  }
};

inline
data_expression quantifiers_inside(const data_expression& x)
{
  quantifiers_inside_builder f;
  data_expression result;
  f.apply(result, x);
  return result;
}

inline
data_expression quantifiers_inside_forall(const std::set<variable>& variables, const data_expression& x)
{
  quantifiers_inside_forall_builder f(variables);
  data_expression result;
  f.apply(result, x);
  return result;
}

inline
data_expression quantifiers_inside_exists(const std::set<variable>& variables, const data_expression& x)
{
  quantifiers_inside_exists_builder f(variables);
  data_expression result;
  f.apply(result, x);
  return result;
}

} // namespace detail

struct quantifiers_inside_rewriter
{
  using argument_type = data_expression;
  using result_type = data_expression;

  data_expression operator()(const data_expression& x) const
  {
    return detail::quantifiers_inside(x);
  }
};

template <typename T>
void quantifiers_inside_rewrite(T& x, std::enable_if_t<!std::is_base_of_v<atermpp::aterm, T>>* = 0)
{
  core::make_update_apply_builder<data_expression_builder>(quantifiers_inside_rewriter()).update(x);
}

template <typename T>
T quantifiers_inside_rewrite(const T& x, std::enable_if_t<std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  T result;
  core::make_update_apply_builder<data_expression_builder>(quantifiers_inside_rewriter()).apply(result, x);
  return result;
}

} // namespace mcrl2::data

#endif // MCRL2_DATA_REWRITERS_QUANTIFIERS_INSIDE_REWRITER_H
