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

namespace mcrl2 {

namespace data {

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
  typedef data_expression_builder<quantifiers_inside_builder> super;
  using super::apply;

  data_expression apply(const forall& x)
  {
    data_expression const& phi = x.body();
    std::set<variable> W = make_variable_set(x.variables());
    return quantifiers_inside_forall(W, apply(phi));
  }

  data_expression apply(const exists& x)
  {
    data_expression const& phi = x.body();
    std::set<variable> W = make_variable_set(x.variables());
    return quantifiers_inside_exists(W, apply(phi));
  }
};

struct quantifiers_inside_forall_builder: public data_expression_builder<quantifiers_inside_forall_builder>
{
  typedef data_expression_builder<quantifiers_inside_forall_builder> super;
  using super::apply;

  const std::set<variable>& V;

  explicit quantifiers_inside_forall_builder(const std::set<variable>& V_)
    : V(V_)
  {}

  static data_expression make_forall(const variable_list& vars, const data_expression& body)
  {
    return vars.empty() ? body : forall(vars,body);
  }

  data_expression apply(const forall& x)
  {
    using utilities::detail::set_union;
    std::set<variable> W = make_variable_set(x.variables());
    return quantifiers_inside_forall(set_union(V, W), x.body());
  }

  // default case
  template <typename T>
  data_expression apply_default(const T& x)
  {
    using utilities::detail::set_intersection;
    std::set<variable> W = set_intersection(V, find_free_variables(x));
    return make_forall(variable_list(W.begin(), W.end()), x);
  }

  data_expression apply(const data_expression& x)
  {
    if (sort_bool::is_not_application(x))
    {
      data_expression const& phi = sort_bool::arg(x);
      return sort_bool::not_(quantifiers_inside_exists(V, phi));
    }
    else if (sort_bool::is_and_application(x))
    {
      const data_expression& phi = sort_bool::left(x);
      const data_expression& psi = sort_bool::right(x);
      return sort_bool::and_(quantifiers_inside_forall(V, phi), quantifiers_inside_forall(V, psi));
    }
    else if (sort_bool::is_or_application(x))
    {
      using utilities::detail::set_difference;
      using utilities::detail::set_intersection;
      typedef core::term_traits<data::data_expression> tr;

      std::vector<data_expression> X;
      utilities::detail::split(x, std::back_inserter(X), tr::is_or, tr::left, tr::right);
      const auto [Phi, Psi] = compute_Phi_Psi(X, V, tr::or_, tr::false_());
      if (sort_bool::is_false_function_symbol(Psi))
      {
        return make_forall(make_variable_list(set_intersection(V,find_free_variables(x))), x);
      }
      std::set<variable> vars_Phi = find_free_variables(Phi);
      std::set<variable> vars_Psi = find_free_variables(Psi);
      return make_forall(make_variable_list(set_intersection(V, set_intersection(vars_Phi, vars_Psi))),
                         lazy::or_(
                           quantifiers_inside_forall(set_difference(set_intersection(V, vars_Phi), vars_Psi), Phi),
                           quantifiers_inside_forall(set_difference(set_intersection(V, vars_Psi), vars_Phi), Psi)
                              )
      );
    }
    else if (sort_bool::is_implies_application(x))
    {
      const data_expression& left = sort_bool::left(x);
      const data_expression& right = sort_bool::right(x);
      return quantifiers_inside_forall(V, sort_bool::or_(sort_bool::not_(left), right));
    }
    else
    {
      return apply_default(x);
    }
  }
};

struct quantifiers_inside_exists_builder: public data_expression_builder<quantifiers_inside_exists_builder>
{
  typedef data_expression_builder<quantifiers_inside_exists_builder> super;
  using super::apply;

  const std::set<variable>& V;

  quantifiers_inside_exists_builder(const std::set<variable>& V_)
    : V(V_)
  {}

  data_expression make_exists(const variable_list& vars, const data_expression& body)
  {
    return vars.empty() ? body : exists(vars,body);
  }

  data_expression apply(const exists& x)
  {
    using utilities::detail::set_union;
    std::set<variable> W = make_variable_set(x.variables());
    return quantifiers_inside_exists(set_union(V, W), x.body());
  }

  // default case
  template <typename T>
  data_expression apply_default(const T& x)
  {
    using utilities::detail::set_intersection;
    std::set<variable> W = set_intersection(V, find_free_variables(x));
    return make_exists(variable_list(W.begin(), W.end()), x);
  }

  data_expression apply(const forall& x)
  {
    return apply_default(x);
  }

  data_expression apply(const data_expression& x)
  {
    if (sort_bool::is_not_application(x))
    {
      data_expression const& phi = sort_bool::arg(x);
      return sort_bool::not_(quantifiers_inside_forall(V, phi));
    }
    else if (sort_bool::is_and_application(x))
    {
      using utilities::detail::set_difference;
      using utilities::detail::set_intersection;
      typedef core::term_traits<data::data_expression> tr;

      std::vector<data_expression> X;
      utilities::detail::split(x, std::back_inserter(X), tr::is_and, tr::left, tr::right);
      const auto [Phi, Psi] = compute_Phi_Psi(X, V, tr::and_, tr::true_());
      if (sort_bool::is_true_function_symbol(Psi))
      {
        return make_exists(make_variable_list(set_intersection(V, find_free_variables(x))), x);
      }
      std::set<variable> vars_Phi = find_free_variables(Phi);
      std::set<variable> vars_Psi = find_free_variables(Psi);
      return make_exists(make_variable_list(set_intersection(V, set_intersection(vars_Phi, vars_Psi))),
                         lazy::and_(
                           quantifiers_inside_exists(set_difference(set_intersection(V, vars_Phi), vars_Psi), Phi),
                           quantifiers_inside_exists(set_difference(set_intersection(V, vars_Psi), vars_Phi), Psi)
                         )
      );
    }
    else if (sort_bool::is_or_application(x))
    {
      data_expression const& phi = sort_bool::left(x);
      data_expression const& psi = sort_bool::right(x);
      return sort_bool::or_(quantifiers_inside_exists(V, phi), quantifiers_inside_exists(V, psi)
      );
    }
    else if (sort_bool::is_implies_application(x))
    {
      const data_expression& left = sort_bool::left(x);
      const data_expression& right = sort_bool::right(x);
      return quantifiers_inside_exists(V, sort_bool::or_(sort_bool::not_(left), right));
    }
    return apply_default(x);
  }
};

inline
data_expression quantifiers_inside(const data_expression& x)
{
  quantifiers_inside_builder f;
  return f.apply(x);
}

inline
data_expression quantifiers_inside_forall(const std::set<variable>& variables, const data_expression& x)
{
  quantifiers_inside_forall_builder f(variables);
  return f.apply(x);
}

inline
data_expression quantifiers_inside_exists(const std::set<variable>& variables, const data_expression& x)
{
  quantifiers_inside_exists_builder f(variables);
  return f.apply(x);
}

} // namespace detail

struct quantifiers_inside_rewriter
{
  using argument_type = data_expression;

  data_expression operator()(const data_expression& x) const
  {
    return detail::quantifiers_inside(x);
  }
};

template <typename T>
void quantifiers_inside_rewrite(T& x, typename std::enable_if< !std::is_base_of< atermpp::aterm, T >::value>::type* = 0)
{
  core::make_update_apply_builder<data_expression_builder>(quantifiers_inside_rewriter()).update(x);
}

template <typename T>
T quantifiers_inside_rewrite(const T& x, typename std::enable_if< std::is_base_of< atermpp::aterm, T >::value>::type* = 0)
{
  T result = core::make_update_apply_builder<data_expression_builder>(quantifiers_inside_rewriter()).apply(x);
  return result;
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_REWRITERS_QUANTIFIERS_INSIDE_REWRITER_H
