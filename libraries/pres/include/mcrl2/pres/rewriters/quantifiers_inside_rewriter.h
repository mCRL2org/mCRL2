// Author(s): Jan Friso Groote based on a similar file for PBESs by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/rewriters/quantifiers_inside_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_REWRITERS_QUANTIFIERS_INSIDE_REWRITER_H
#define MCRL2_PRES_REWRITERS_QUANTIFIERS_INSIDE_REWRITER_H

#include "mcrl2/data/rewriters/quantifiers_inside_rewriter.h"
#include "mcrl2/pres/builder.h"
#include "mcrl2/pres/join.h"

namespace mcrl2 {

namespace pres_system {

namespace detail {

pres_expression quantifiers_inside(const pres_expression& x);
pres_expression quantifiers_inside_infimum(const std::set<data::variable>& variables, const pres_expression& x);
pres_expression quantifiers_inside_supremum(const std::set<data::variable>& variables, const pres_expression& x);

template <typename BinaryOperation>
std::tuple<pres_expression, pres_expression> compute_Phi_Psi(const std::vector<pres_expression>& X, const std::set<data::variable>& V, BinaryOperation op, pres_expression empty_sequence_result)
{
  using utilities::detail::set_difference;
  using utilities::detail::set_intersection;

  std::vector<std::set<data::variable>> vars; // free variables
  for (const pres_expression& x_j: X)
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

  std::vector<pres_expression> phi;
  std::vector<pres_expression> psi;
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
  pres_expression Phi = utilities::detail::join(phi.begin(), phi.end(), op, empty_sequence_result);
  pres_expression Psi = utilities::detail::join(psi.begin(), psi.end(), op, empty_sequence_result);
  return { Phi, Psi };
}

struct quantifiers_inside_builder: public pres_expression_builder<quantifiers_inside_builder>
{
  typedef pres_expression_builder<quantifiers_inside_builder> super;
  using super::apply;

  template <class T>
  void apply(T& result, const infimum& x)
  {
    pres_expression const& phi = x.body();
    std::set<data::variable> W = data::detail::make_variable_set(x.variables());
    apply(result, phi);
    result = quantifiers_inside_infimum(W, result);
  }

  template <class T>
  void apply(T& result, const supremum& x)
  {
    pres_expression const& phi = x.body();
    std::set<data::variable> W = data::detail::make_variable_set(x.variables());
    apply(result, phi);
    result = quantifiers_inside_supremum(W, result);
  }
};

struct quantifiers_inside_infimum_builder: public data_expression_builder<quantifiers_inside_infimum_builder>
{
  typedef data_expression_builder<quantifiers_inside_infimum_builder> super;
  using super::apply;

  const std::set<data::variable>& V;

  explicit quantifiers_inside_infimum_builder(const std::set<data::variable>& V_)
      : V(V_)
  {}

  static pres_expression make_infimum_(const data::variable_list& vars, const pres_expression& body)
  {
    return vars.empty() ? body : infimum(vars, body);
  }

  // default case
  template <typename T>
  pres_expression apply_default(const T& x)
  {
    using utilities::detail::set_intersection;
    std::set<data::variable> W = set_intersection(V, find_free_variables(x));
    return make_infimum_(data::variable_list(W.begin(), W.end()), x);
  }

  template <class T>
  void apply(T& result, const infimum& x)
  {
    using utilities::detail::set_union;
    std::set<data::variable> W = data::detail::make_variable_set(x.variables());
    result = quantifiers_inside_infimum(set_union(V, W), x.body());
  }

  template <class T>
  void apply(T& result, const minus& x)
  {
    const auto& phi = x.operand();
    result = minus(quantifiers_inside_supremum(V, phi));
  }

  template <class T>
  void apply(T& result, const and_& x)
  {
    const pres_expression& phi = x.left();
    const pres_expression& psi = x.right();
    make_and_(result, quantifiers_inside_infimum(V, phi), quantifiers_inside_infimum(V, psi));
  }

  template <class T>
  void apply(T& result, const or_& x)
  {
    using utilities::detail::set_difference;
    using utilities::detail::set_intersection;
    typedef core::term_traits<pres_expression> tr;

    std::vector<pres_expression> X;
    utilities::detail::split(x, std::back_inserter(X), tr::is_or, tr::left, tr::right);
    const auto [Phi, Psi] = compute_Phi_Psi(X, V, tr::or_, tr::false_());
    if (is_false(Psi))
    {
      result = make_infimum_(data::detail::make_variable_list(set_intersection(V, find_free_variables(x))), x);
      return;
    }
    std::set<data::variable> vars_Phi = find_free_variables(Phi);
    std::set<data::variable> vars_Psi = find_free_variables(Psi);
    optimized_or(result,
                 quantifiers_inside_infimum(set_difference(set_intersection(V, vars_Phi), vars_Psi), Phi),
                 quantifiers_inside_infimum(set_difference(set_intersection(V, vars_Psi), vars_Phi), Psi));
    result = make_infimum_(data::detail::make_variable_list(set_intersection(V, set_intersection(vars_Phi, vars_Psi))),
                          result);
  }

  template <class T>
  void apply(T& result, const imp& x)
  {
    result = quantifiers_inside_infimum(V, or_(minus(x.left()), x.right()));
  }

  template <class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    result = apply_default(x);
  }

  template <class T>
  void apply(T& result, const supremum& x)
  {
    result = apply_default(x);
  }

  template <class T>
  void apply(T& result, const data::data_expression& x)
  {
    result = x; // TODO: Investigate whether a supremum needs to be pushed inside a data-expression. 
    // result = data::detail::quantifiers_inside_infimum(V, x);
  }
};

struct quantifiers_inside_supremum_builder: public pres_expression_builder<quantifiers_inside_supremum_builder>
{
  typedef pres_expression_builder<quantifiers_inside_supremum_builder> super;
  using super::apply;

  const std::set<data::variable>& V;

  explicit quantifiers_inside_supremum_builder(const std::set<data::variable>& V_)
      : V(V_)
  {}

  static pres_expression make_supremum_(const data::variable_list& vars, const pres_expression& body)
  {
    return vars.empty() ? body : supremum(vars, body);
  }

  // default case
  template <typename T>
  pres_expression apply_default(const T& x)
  {
    using utilities::detail::set_intersection;
    std::set<data::variable> W = set_intersection(V, find_free_variables(x));
    return make_supremum_(data::variable_list(W.begin(), W.end()), x);
  }

  template <class T>
  void apply(T& result, const supremum& x)
  {
    using utilities::detail::set_union;
    std::set<data::variable> W = data::detail::make_variable_set(x.variables());
    result = quantifiers_inside_supremum(set_union(V, W), x.body());
  }

  template <class T>
  void apply(T& result, const minus& x)
  {
    const auto& phi = x.operand();
    result = minus(quantifiers_inside_infimum(V, phi));
  }

  template <class T>
  void apply(T& result, const or_& x)
  {
    const pres_expression& phi = x.left();
    const pres_expression& psi = x.right();
    result = or_(quantifiers_inside_supremum(V, phi), quantifiers_inside_supremum(V, psi));
  }

  template <class T>
  void apply(T& result, const and_& x)
  {
    using utilities::detail::set_difference;
    using utilities::detail::set_intersection;
    typedef core::term_traits<pres_expression> tr;

    std::vector<pres_expression> X;
    utilities::detail::split(x, std::back_inserter(X), tr::is_and, tr::left, tr::right);
    const auto [Phi, Psi] = compute_Phi_Psi(X, V, tr::and_, tr::true_());
    if (is_true(Psi))
    {
      result = make_supremum_(data::detail::make_variable_list(set_intersection(V, find_free_variables(x))), x);
      return;
    }
    std::set<data::variable> vars_Phi = find_free_variables(Phi);
    std::set<data::variable> vars_Psi = find_free_variables(Psi);
    optimized_and(result,
                  quantifiers_inside_supremum(set_difference(set_intersection(V, vars_Phi), vars_Psi), Phi),
                  quantifiers_inside_supremum(set_difference(set_intersection(V, vars_Psi), vars_Phi), Psi));
    result = make_supremum_(data::detail::make_variable_list(set_intersection(V, set_intersection(vars_Phi, vars_Psi))),
                          result);
  }

  template <class T>
  void apply(T& result, const imp& x)
  {
    result = quantifiers_inside_supremum(V, and_(minus(x.left()), x.right()));
  }

  template <class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    result = apply_default(x);
  }

  template <class T>
  void apply(T& result, const infimum& x)
  {
    result = apply_default(x);
  }

  template <class T>
  void apply(T& result, const data::data_expression& x)
  {
    result = x;  // TODO: INvestigate whether supremum needs to be pushed inside a data expression of sort real. 
    // result = data::detail::quantifiers_inside_supremum(V, x);
  }
};

inline
pres_expression quantifiers_inside(const pres_expression& x)
{
  quantifiers_inside_builder f;
  pres_expression result;
  f.apply(result, x);
  return result;
}

inline
pres_expression quantifiers_inside_infimum(const std::set<data::variable>& variables, const pres_expression& x)
{
  quantifiers_inside_infimum_builder f(variables);
  pres_expression result;
  f.apply(result, x);
  return result;
}

inline
pres_expression quantifiers_inside_supremum(const std::set<data::variable>& variables, const pres_expression& x)
{
  quantifiers_inside_supremum_builder f(variables);
  pres_expression result;
  f.apply(result, x);
  return result;
}

} // namespace detail

/// \brief A rewriter that pushes quantifiers inside in a PRES expression.
class quantifiers_inside_rewriter
{
  public:
    /// \brief The term type
    typedef pres_expression term_type;

    /// \brief The variable type
    typedef data::variable variable_type;

    /// \brief Rewrites a pres expression.
    /// \param x A term
    /// \return The rewrite result.
    pres_expression operator()(const pres_expression& x) const
    {
      return detail::quantifiers_inside(x);
    }
};

} // namespace pres_system

} // namespace mcrl2

#endif // MCRL2_PRES_REWRITERS_QUANTIFIERS_INSIDE_REWRITER_H
