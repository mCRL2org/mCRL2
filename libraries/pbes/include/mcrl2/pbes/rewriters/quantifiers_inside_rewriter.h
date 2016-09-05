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

#ifndef MCRL2_PBES_REWRITERS_QUANTIFIERS_INSIDE_REWRITER_H
#define MCRL2_PBES_REWRITERS_QUANTIFIERS_INSIDE_REWRITER_H

#include "mcrl2/pbes/builder.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

pbes_expression quantifiers_inside(const pbes_expression& x);
pbes_expression quantifiers_inside_forall(const std::set<data::variable>& variables, const pbes_expression& x);
pbes_expression quantifiers_inside_exists(const std::set<data::variable>& variables, const pbes_expression& x);

inline
std::set<data::variable> make_variable_set(const data::variable_list& x)
{
  return std::set<data::variable>(x.begin(), x.end());
}

struct quantifiers_inside_builder: public pbes_expression_builder<quantifiers_inside_builder>
{
  typedef pbes_expression_builder<quantifiers_inside_builder> super;
  using super::apply;

  pbes_expression apply(const forall& x)
  {
    auto const& phi = x.body();
    auto W = make_variable_set(x.variables());
    return quantifiers_inside_forall(W, apply(phi));
  }

  pbes_expression apply(const exists& x)
  {
    auto const& phi = x.body();
    auto W = make_variable_set(x.variables());
    return quantifiers_inside_exists(W, apply(phi));
  }
};

struct quantifiers_inside_forall_builder: public pbes_expression_builder<quantifiers_inside_forall_builder>
{
  typedef pbes_expression_builder<quantifiers_inside_forall_builder> super;
  using super::apply;

  const std::set<data::variable>& V;

  quantifiers_inside_forall_builder(const std::set<data::variable>& V_)
    : V(V_)
  {}

  std::set<data::variable> vars(const pbes_expression& x)
  {
    return pbes_system::find_free_variables(x);
  }

  pbes_expression apply(const not_& x)
  {
    auto const& phi = x.operand();
    return not_(quantifiers_inside_exists(V, phi));
  }

  pbes_expression apply(const and_& x)
  {
    using utilities::detail::set_intersection;
    auto const& phi = x.left();
    auto const& psi = x.right();
    auto vars_phi = vars(phi);
    auto vars_psi = vars(psi);
    return and_(quantifiers_inside_forall(set_intersection(V, vars_phi), phi),
                quantifiers_inside_forall(set_intersection(V, vars_psi), psi)
               );
  }

  pbes_expression apply(const or_& x)
  {
    using utilities::detail::set_difference;
    using utilities::detail::set_intersection;
    auto const& phi = x.left();
    auto const& psi = x.right();
    auto vars_phi = vars(phi);
    auto vars_psi = vars(psi);
    auto W = set_intersection(set_intersection(V, vars_phi), vars_psi);
    return make_forall(data::variable_list(W.begin(), W.end()),
                       or_(quantifiers_inside_forall(set_difference(set_intersection(V, vars_phi), vars_psi), phi),
                           quantifiers_inside_forall(set_difference(set_intersection(V, vars_psi), vars_phi), psi)
                          )
                      );
  }

  pbes_expression apply(const imp& x)
  {
    using utilities::detail::set_difference;
    using utilities::detail::set_intersection;
    auto const& phi = x.left();
    auto const& psi = x.right();
    auto vars_phi = vars(phi);
    auto vars_psi = vars(psi);
    auto W = set_intersection(set_intersection(V, vars_phi), vars_psi);
    return make_forall(data::variable_list(W.begin(), W.end()),
                       imp(quantifiers_inside_exists(set_difference(set_intersection(V, vars_phi), vars_psi), phi),
                           quantifiers_inside_forall(set_difference(set_intersection(V, vars_psi), vars_phi), psi)
                          )
                      );
  }

  pbes_expression apply(const forall& x)
  {
    using utilities::detail::set_difference;
    auto const& phi = x.body();
    auto W = make_variable_set(x.variables());
    return forall(x.variables(), quantifiers_inside_forall(set_difference(V, W), phi));
  }

  pbes_expression apply(const exists& x)
  {
    auto const& phi = x.body();
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
  pbes_expression apply_default(const T& x)
  {
    using utilities::detail::set_intersection;
    auto W = set_intersection(V, vars(x));
    return make_forall(data::variable_list(W.begin(), W.end()), x);
  }

  pbes_expression apply(const data::data_expression& x)
  {
    return apply_default(x);
  }

  pbes_expression apply(const propositional_variable_instantiation& x)
  {
    return apply_default(x);
  }
};

struct quantifiers_inside_exists_builder: public pbes_expression_builder<quantifiers_inside_exists_builder>
{
  typedef pbes_expression_builder<quantifiers_inside_exists_builder> super;
  using super::apply;

  const std::set<data::variable>& V;

  quantifiers_inside_exists_builder(const std::set<data::variable>& V_)
    : V(V_)
  {}

  std::set<data::variable> vars(const pbes_expression& x)
  {
    return pbes_system::find_free_variables(x);
  }

  pbes_expression apply(const not_& x)
  {
    auto const& phi = x.operand();
    return not_(quantifiers_inside_forall(V, phi));
  }

  pbes_expression apply(const and_& x)
  {
    using utilities::detail::set_difference;
    using utilities::detail::set_intersection;
    auto const& phi = x.left();
    auto const& psi = x.right();
    auto vars_phi = vars(phi);
    auto vars_psi = vars(psi);
    auto W = set_intersection(set_intersection(V, vars_phi), vars_psi);
    return make_exists(data::variable_list(W.begin(), W.end()),
                       and_(quantifiers_inside_exists(set_difference(set_intersection(V, vars_phi), vars_psi), phi),
                            quantifiers_inside_exists(set_difference(set_intersection(V, vars_psi), vars_phi), psi)
                           )
                      );
  }

  pbes_expression apply(const or_& x)
  {
    using utilities::detail::set_intersection;
    auto const& phi = x.left();
    auto const& psi = x.right();
    auto vars_phi = vars(phi);
    auto vars_psi = vars(psi);
    return or_(quantifiers_inside_exists(set_intersection(V, vars_phi), phi),
               quantifiers_inside_exists(set_intersection(V, vars_psi), psi)
              );
  }

  pbes_expression apply(const imp& x)
  {
    using utilities::detail::set_intersection;
    auto const& phi = x.left();
    auto const& psi = x.right();
    auto vars_phi = vars(phi);
    auto vars_psi = vars(psi);
    return imp(quantifiers_inside_forall(set_intersection(V, vars_phi), phi),
               quantifiers_inside_exists(set_intersection(V, vars_psi), psi)
              );
  }

  pbes_expression apply(const exists& x)
  {
    using utilities::detail::set_difference;
    auto const& phi = x.body();
    auto W = make_variable_set(x.variables());
    return exists(x.variables(), quantifiers_inside_exists(set_difference(V, W), phi));
  }

  pbes_expression apply(const forall& x)
  {
    auto const& phi = x.body();
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
  pbes_expression apply_default(const T& x)
  {
    using utilities::detail::set_intersection;
    auto W = set_intersection(V, vars(x));
    return make_exists(data::variable_list(W.begin(), W.end()), x);
  }

  pbes_expression apply(const data::data_expression& x)
  {
    return apply_default(x);
  }

  pbes_expression apply(const propositional_variable_instantiation& x)
  {
    return apply_default(x);
  }
};

inline
pbes_expression quantifiers_inside(const pbes_expression& x)
{
  quantifiers_inside_builder f;
  return f.apply(x);
}

inline
pbes_expression quantifiers_inside_forall(const std::set<data::variable>& variables, const pbes_expression& x)
{
  quantifiers_inside_forall_builder f(variables);
  return f.apply(x);
}

inline
pbes_expression quantifiers_inside_exists(const std::set<data::variable>& variables, const pbes_expression& x)
{
  quantifiers_inside_exists_builder f(variables);
  return f.apply(x);
}

} // namespace detail

/// \brief A rewriter that applies one point rule quantifier elimination to a PBES.
class quantifiers_inside_rewriter
{
  public:
    /// \brief The term type
    typedef pbes_expression term_type;

    /// \brief The variable type
    typedef data::variable variable_type;

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \return The rewrite result.
    pbes_expression operator()(const pbes_expression& x) const
    {
      return detail::quantifiers_inside(x);
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITERS_QUANTIFIERS_INSIDE_REWRITER_H
