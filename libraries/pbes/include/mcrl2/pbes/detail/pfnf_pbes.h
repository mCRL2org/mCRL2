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

#include <iostream>
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

    const pbes_expression& g() const
    {
      return m_g;
    }

    pbes_expression& g()
    {
      return m_g;
    }

    const std::vector<propositional_variable_instantiation>& variables() const
    {
      return m_v;
    }

    std::vector<propositional_variable_instantiation>& variables()
    {
      return m_v;
    }
};

inline
std::ostream& operator<<(std::ostream& out, const pfnf_implication& x)
{
  return out << pbes_system::pp(imp(x.g(), pbes_expr::join_or(x.variables().begin(), x.variables().end())));
}

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

    bool is_forall() const
    {
      return m_is_forall;
    }

    const data::variable_list& variables() const
    {
      return m_variables;
    }

    // applies the quantifier to a pbes expression
    pbes_expression apply(const pbes_expression& phi) const
    {
      if (m_is_forall)
      {
        return forall(m_variables, phi);
      }
      else
      {
        return exists(m_variables, phi);
      }
    }
};

// represents Qq: Q. h /\ implications
class pfnf_equation
{
  protected:
    // left hand side
    fixpoint_symbol m_symbol;
    propositional_variable m_X;
    std::vector<data::variable> m_parameters;

    // right hand side
    std::vector<pfnf_quantifier> m_quantifiers;
    pbes_expression m_h;
    std::vector<pfnf_implication> m_implications;

  public:
    pfnf_equation(const pbes_equation& eqn)
    {
      m_symbol = eqn.symbol();
      propositional_variable X = eqn.variable();
      pbes_expression phi = eqn.formula();

      // left hand side
      m_X = X;
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

    const propositional_variable& variable() const
    {
      return m_X;
    }

    const std::vector<data::variable>& parameters() const
    {
      return m_parameters;
    }

    const std::vector<pfnf_quantifier>& quantifiers() const
    {
      return m_quantifiers;
    }

    const pbes_expression& h() const
    {
      return m_h;
    }

    pbes_expression& h()
    {
      return m_h;
    }

    const std::vector<pfnf_implication>& implications() const
    {
      return m_implications;
    }

    std::vector<pfnf_implication>& implications()
    {
      return m_implications;
    }

    // computes the equation with the implications replaced by new_implication
    pbes_equation apply_implication(const pbes_expression& new_implications) const
    {
      pbes_expression phi = new_implications;

      // apply quantifiers
      for (std::vector<pfnf_quantifier>::const_reverse_iterator i = m_quantifiers.rbegin(); i != m_quantifiers.rend(); ++i)
      {
        phi = i->apply(phi);
      }

      phi = and_(m_h, phi);

      return pbes_equation(m_symbol, m_X, phi);
    }
};

// explicit representation of a pbes in PFNF format
class pfnf_pbes
{
  protected:
    const pbes<>& m_pbes; // store a reference to the original pbes, to indicate that it should not be destroyed
    std::vector<pfnf_equation> m_equations;

  public:
    /// \brief Constructor
    /// \pre The pbes p must be in PFNF format
    pfnf_pbes(const pbes<>& p)
      : m_pbes(p)
    {
      const atermpp::vector<pbes_equation>& equations = p.equations();
      for (atermpp::vector<pbes_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
      {
        m_equations.push_back(pfnf_equation(*i));
      }
    }

    const std::vector<pfnf_equation>& equations() const
    {
      return m_equations;
    }

    std::vector<pfnf_equation>& equations()
    {
      return m_equations;
    }

    const pbes_expression& initial_state() const
    {
      return m_pbes.initial_state();
    }

    const data::data_specification& data() const
    {
      return m_pbes.data();
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PFNF_PBES_H
