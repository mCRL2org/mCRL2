// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pfnf_pbes.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_PFNF_PBES_H
#define MCRL2_PBES_DETAIL_PFNF_PBES_H

#include "mcrl2/pbes/detail/is_pfnf.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

inline
void split_pfnf_implication(const pbes_expression& x, pbes_expression& g, std::vector<propositional_variable_instantiation>& Xij)
{
  if (is_pfnf_simple_expression(x))
  {
    g = x;
  }
  else
  {
    pbes_expression y = x;
    if (is_imp(y))
    {
      g = imp(y).left();
      y = imp(y).right();
    }
    else
    {
      g = true_();
    }

    std::vector<pbes_expression> v;
    split_or(y, v);
    for (std::vector<pbes_expression>::iterator i = v.begin(); i != v.end(); ++i)
    {
      assert(is_propositional_variable_instantiation(*i));
      Xij.push_back(*i);
    }
  }
}

// represents g => \/_j in J . X_j(e_j)
class pfnf_implication
{
  protected:
    pbes_expression m_g;
    std::vector<propositional_variable_instantiation> m_v;

  public:
    pfnf_implication(const pbes_expression& x)
    {
      split_pfnf_implication(x, m_g, m_v);
    }
};

// represents forall d:D or exists d:D
class pfnf_quantifier
{
  protected:
    bool m_is_forall;
    data::variable_list m_variables;

  public:
    pfnf_quantifier(bool is_forall, const data::variable_list& variables)
      : m_is_forall(is_forall), m_variables(variables)
    {}
};

// represents Qq: Q. h /\ implications
class pfnf_equation
{
  protected:
    // left hand side
    core::identifier_string m_name;
    std::vector<data::variable> m_parameters;

    // right hand side
    std::vector<pfnf_quantifier> m_quantifiers;
    pbes_expression m_h;
    std::vector<pfnf_implication> m_implications;

  public:
    pfnf_equation(const pbes_equation& eqn)
    {
      propositional_variable X = eqn.variable();
      pbes_expression phi = eqn.formula();

      // left hand side
      m_name = X.name();
      data::variable_list d = X.parameters();
      m_parameters = std::vector<data::variable>(d.begin(), d.end());

      // right hand side
      pbes_expression y = phi;
      while (is_forall(y) || is_exists(y))
      {
        if (is_forall(y))
        {
          m_quantifiers.push_back(pfnf_quantifier(true, forall(y).variables()));
          y = forall(y).body();
        }
        else if (is_exists(y))
        {
          m_quantifiers.push_back(pfnf_quantifier(false, exists(y).variables()));
          y = exists(y).body();
        }
      }
      std::vector<pbes_expression> g;
      split_pfnf_expression(y, m_h, g);
      for (std::vector<pbes_expression>::iterator i = g.begin(); i != g.end(); ++i)
      {
        m_implications.push_back(pfnf_implication(*i));
      }
    }
};

// explicit representation of a pbes in PFNF format
class pfnf_pbes
{
  protected:
    const pbes<>& m_pbes; // store a reference to the original pbes, to indicate that it should not be destroyed
    std::vector<pfnf_equation> m_equations;
    pbes_expression m_initial_state;


  public:
    pfnf_pbes(const pbes<>& p)
      : m_pbes(p)
    {
      const atermpp::vector<pbes_equation>& equations = p.equations();
      for (atermpp::vector<pbes_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
      {
        m_equations.push_back(pfnf_equation(*i));
      }

      m_initial_state = p.initial_state();
    }

    const std::vector<pfnf_equation>& equations() const
    {
      return m_equations;
    }

    const pbes_expression& initial_state() const
    {
      return m_initial_state;
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PFNF_PBES_H
