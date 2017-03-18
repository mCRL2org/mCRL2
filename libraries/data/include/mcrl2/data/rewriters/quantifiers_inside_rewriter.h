// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/quantifiers_inside_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_REWRITERS_QUANTIFIERS_INSIDE_REWRITER_H
#define MCRL2_DATA_REWRITERS_QUANTIFIERS_INSIDE_REWRITER_H

#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2 {

namespace data {

namespace detail {

data_expression quantifiers_inside(const data_expression& x);
data_expression quantifiers_inside_forall(const std::set<data::variable>& variables, const data_expression& x);
data_expression quantifiers_inside_exists(const std::set<data::variable>& variables, const data_expression& x);

inline
std::set<data::variable> make_variable_set(const data::variable_list& x)
{
  return std::set<data::variable>(x.begin(), x.end());
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

  const std::set<data::variable>& V;

  quantifiers_inside_forall_builder(const std::set<data::variable>& V_)
    : V(V_)
  {}

  data_expression make_forall(const variable_list& vars, const data_expression& body)
  {
    return vars.empty() ? body : forall(vars,body);
  }

  data_expression apply(const forall& x)
  {
    using utilities::detail::set_difference;
    data_expression const& phi = x.body();
    std::set<variable> W = make_variable_set(x.variables());
    return forall(x.variables(), quantifiers_inside_forall(set_difference(V, W), phi));
  }

  data_expression apply(const exists& x)
  {
    data_expression const& phi = x.body();
    if (x.variables().empty())
    {
      return apply(phi);
    }
    else
    {
      return make_forall(data::variable_list(V.begin(), V.end()), exists(x.variables(), phi));
    }
  }

  // default case
  template <typename T>
  data_expression apply_default(const T& x)
  {
    using utilities::detail::set_intersection;
    std::set<variable> W = set_intersection(V, find_free_variables(x));
    return make_forall(data::variable_list(W.begin(), W.end()), x);
  }

  data_expression apply(const data::data_expression& x)
  {
    if(sort_bool::is_not_application(x))
    {
      data_expression const& phi = sort_bool::arg(x);
      return sort_bool::not_(quantifiers_inside_exists(V, phi));
    }
    else if(sort_bool::is_and_application(x))
    {
      using utilities::detail::set_intersection;
      data_expression const& phi = sort_bool::left(x);
      data_expression const& psi = sort_bool::right(x);
      std::set<variable> vars_phi = find_free_variables(phi);
      std::set<variable> vars_psi = find_free_variables(psi);
      return sort_bool::and_(quantifiers_inside_forall(set_intersection(V, vars_phi), phi),
                  quantifiers_inside_forall(set_intersection(V, vars_psi), psi)
                 );
    }
    else if(sort_bool::is_or_application(x))
    {
      using utilities::detail::set_difference;
      using utilities::detail::set_intersection;
      data_expression const& phi = sort_bool::left(x);
      data_expression const& psi = sort_bool::right(x);
      std::set<variable> vars_phi = find_free_variables(phi);
      std::set<variable> vars_psi = find_free_variables(psi);
      std::set<variable> W = set_intersection(set_intersection(V, vars_phi), vars_psi);
      return make_forall(data::variable_list(W.begin(), W.end()),
                         sort_bool::or_(quantifiers_inside_forall(set_difference(set_intersection(V, vars_phi), vars_psi), phi),
                             quantifiers_inside_forall(set_difference(set_intersection(V, vars_psi), vars_phi), psi)
                            )
                        );
    }
    else if(sort_bool::is_implies_application(x))
    {
      using utilities::detail::set_difference;
      using utilities::detail::set_intersection;
      data_expression const& phi = sort_bool::left(x);
      data_expression const& psi = sort_bool::right(x);
      std::set<variable> vars_phi = find_free_variables(phi);
      std::set<variable> vars_psi = find_free_variables(psi);
      std::set<variable> W = set_intersection(set_intersection(V, vars_phi), vars_psi);
      return make_forall(data::variable_list(W.begin(), W.end()),
                         sort_bool::implies(quantifiers_inside_exists(set_difference(set_intersection(V, vars_phi), vars_psi), phi),
                             quantifiers_inside_forall(set_difference(set_intersection(V, vars_psi), vars_phi), psi)
                            )
                        );
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

  const std::set<data::variable>& V;

  quantifiers_inside_exists_builder(const std::set<data::variable>& V_)
    : V(V_)
  {}

  data_expression make_exists(const variable_list& vars, const data_expression& body)
  {
    return vars.empty() ? body : exists(vars,body);
  }

  data_expression apply(const exists& x)
  {
    using utilities::detail::set_difference;
    data_expression const& phi = x.body();
    std::set<variable> W = make_variable_set(x.variables());
    return make_exists(x.variables(), quantifiers_inside_exists(set_difference(V, W), phi));
  }

  data_expression apply(const forall& x)
  {
    data_expression const& phi = x.body();
    if (x.variables().empty())
    {
      return apply(phi);
    }
    else
    {
      return make_exists(data::variable_list(V.begin(), V.end()), forall(x.variables(), phi));
    }
  }

  // default case
  template <typename T>
  data_expression apply_default(const T& x)
  {
    using utilities::detail::set_intersection;
    std::set<variable> W = set_intersection(V, find_free_variables(x));
    return make_exists(data::variable_list(W.begin(), W.end()), x);
  }

  data_expression apply(const data::data_expression& x)
  {
    if(sort_bool::is_not_application(x))
    {
      data_expression const& phi = sort_bool::arg(x);
      return sort_bool::not_(quantifiers_inside_forall(V, phi));
    }
    else if(sort_bool::is_and_application(x))
    {
      using utilities::detail::set_difference;
      using utilities::detail::set_intersection;
      data_expression const& phi = sort_bool::left(x);
      data_expression const& psi = sort_bool::right(x);
      std::set<data::variable> vars_phi = find_free_variables(phi);
      std::set<data::variable> vars_psi = find_free_variables(psi);
      std::set<variable> W = set_intersection(set_intersection(V, vars_phi), vars_psi);
      return make_exists(data::variable_list(W.begin(), W.end()),
                         sort_bool::and_(quantifiers_inside_exists(set_difference(set_intersection(V, vars_phi), vars_psi), phi),
                              quantifiers_inside_exists(set_difference(set_intersection(V, vars_psi), vars_phi), psi)
                             )
                        );
    }
    else if(sort_bool::is_or_application(x))
    {
      using utilities::detail::set_intersection;
      data_expression const& phi = sort_bool::left(x);
      data_expression const& psi = sort_bool::right(x);
      std::set<data::variable> vars_phi = find_free_variables(phi);
      std::set<data::variable> vars_psi = find_free_variables(psi);
      return sort_bool::or_(quantifiers_inside_exists(set_intersection(V, vars_phi), phi),
                 quantifiers_inside_exists(set_intersection(V, vars_psi), psi)
                );
    }
    else if(sort_bool::is_implies_application(x))
    {
      using utilities::detail::set_intersection;
      data_expression const& phi = sort_bool::left(x);
      data_expression const& psi = sort_bool::right(x);
      std::set<data::variable> vars_phi = find_free_variables(phi);
      std::set<data::variable> vars_psi = find_free_variables(psi);
      return sort_bool::implies(quantifiers_inside_forall(set_intersection(V, vars_phi), phi),
                 quantifiers_inside_exists(set_intersection(V, vars_psi), psi)
                );
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
data_expression quantifiers_inside_forall(const std::set<data::variable>& variables, const data_expression& x)
{
  quantifiers_inside_forall_builder f(variables);
  return f.apply(x);
}

inline
data_expression quantifiers_inside_exists(const std::set<data::variable>& variables, const data_expression& x)
{
  quantifiers_inside_exists_builder f(variables);
  return f.apply(x);
}

} // namespace detail

struct quantifiers_inside_rewriter: public std::unary_function<data_expression, data_expression>
{
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
