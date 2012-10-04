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

#include <cassert>
#include <iostream>
#include "mcrl2/pbes/detail/is_pfnf.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

inline
void split_pfnf_implication(const pbes_expression& x, pbes_expression& g, atermpp::vector<propositional_variable_instantiation>& Xij)
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
    atermpp::vector<propositional_variable_instantiation> m_v;

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

    const atermpp::vector<propositional_variable_instantiation>& variables() const
    {
      return m_v;
    }

    atermpp::vector<propositional_variable_instantiation>& variables()
    {
      return m_v;
    }

    // convert to pbes_expression
    pbes_expression convert() const
    {
      return imp(m_g, pbes_expr::join_or(m_v.begin(), m_v.end()));
    }

    void protect() const
    {
      m_g.protect();
    }

    void unprotect() const
    {
      m_g.unprotect();
    }

    void mark() const
    {
      m_g.mark();
    }
};

inline
std::ostream& operator<<(std::ostream& out, const pfnf_implication& x)
{
  return out << pbes_system::pp(imp(x.g(), pbes_expr::join_or(x.variables().begin(), x.variables().end())));
}

} // namespace detail
} // namespace pbes_system
} // namespace mcrl2
/// \cond INTERNAL_DOCS
namespace atermpp
{
template<>
struct aterm_traits<mcrl2::pbes_system::detail::pfnf_implication>
{
  static void protect(const mcrl2::pbes_system::detail::pfnf_implication& t)
  {
    t.protect();
  }
  static void unprotect(const mcrl2::pbes_system::detail::pfnf_implication& t)
  {
    t.unprotect();
  }
  static void mark(const mcrl2::pbes_system::detail::pfnf_implication& t)
  {
    t.mark();
  }
};
} // namespace atermpp
/// \endcond
namespace mcrl2 {
namespace pbes_system {
namespace detail {

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

    void protect() const
    {
      m_variables.protect();
    }

    void unprotect() const
    {
      m_variables.unprotect();
    }

    void mark() const
    {
      m_variables.mark();
    }
};

} // namespace detail
} // namespace pbes_system
} // namespace mcrl2
/// \cond INTERNAL_DOCS
namespace atermpp
{
template<>
struct aterm_traits<mcrl2::pbes_system::detail::pfnf_quantifier>
{
  static void protect(const mcrl2::pbes_system::detail::pfnf_quantifier& t)
  {
    t.protect();
  }
  static void unprotect(const mcrl2::pbes_system::detail::pfnf_quantifier& t)
  {
    t.unprotect();
  }
  static void mark(const mcrl2::pbes_system::detail::pfnf_quantifier& t)
  {
    t.mark();
  }
};
} // namespace atermpp
/// \endcond
namespace mcrl2 {
namespace pbes_system {
namespace detail {

// represents Qq: Q. h /\ implications
class pfnf_equation
{
  protected:
    // left hand side
    fixpoint_symbol m_symbol;
    propositional_variable m_X;
    atermpp::vector<data::variable> m_parameters;

    // right hand side
    atermpp::vector<pfnf_quantifier> m_quantifiers;
    pbes_expression m_h;
    atermpp::vector<pfnf_implication> m_implications;

  public:
    pfnf_equation(const pbes_equation& eqn)
    {
      m_symbol = eqn.symbol();
      propositional_variable X = eqn.variable();
      pbes_expression phi = eqn.formula();

      // left hand side
      m_X = X;
      data::variable_list d = X.parameters();
      m_parameters = atermpp::vector<data::variable>(d.begin(), d.end());

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
      atermpp::vector<pbes_expression> g;
      split_pfnf_expression(y, m_h, g);
      for (atermpp::vector<pbes_expression>::iterator i = g.begin(); i != g.end(); ++i)
      {
        m_implications.push_back(pfnf_implication(*i));
      }
    }

    const propositional_variable& variable() const
    {
      return m_X;
    }

    const atermpp::vector<data::variable>& parameters() const
    {
      return m_parameters;
    }

    const atermpp::vector<pfnf_quantifier>& quantifiers() const
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

    const atermpp::vector<pfnf_implication>& implications() const
    {
      return m_implications;
    }

    atermpp::vector<pfnf_implication>& implications()
    {
      return m_implications;
    }

    // convert to pbes_equation
    pbes_equation convert() const
    {
      atermpp::vector<pbes_expression> v;
      for (atermpp::vector<pfnf_implication>::const_iterator i = m_implications.begin(); i != m_implications.end(); ++i)
      {
        v.push_back(i->convert());
      }
      pbes_expression phi = pbes_expr::join_and(v.begin(), v.end());

      phi = and_(m_h, phi);

      // apply quantifiers
      for (atermpp::vector<pfnf_quantifier>::const_reverse_iterator i = m_quantifiers.rbegin(); i != m_quantifiers.rend(); ++i)
      {
        phi = i->apply(phi);
      }
      return pbes_equation(m_symbol, m_X, phi);
    }

    // computes the equation with the implications replaced by new_implication
    pbes_equation apply_implication(const pbes_expression& new_implications) const
    {
      pbes_expression phi = new_implications;
      phi = and_(m_h, phi);

      // apply quantifiers
      for (atermpp::vector<pfnf_quantifier>::const_reverse_iterator i = m_quantifiers.rbegin(); i != m_quantifiers.rend(); ++i)
      {
        phi = i->apply(phi);
      }
      return pbes_equation(m_symbol, m_X, phi);
    }

    void protect() const
    {
      m_symbol.protect();
      m_X.protect();
      m_h.protect();
    }

    void unprotect() const
    {
      m_symbol.unprotect();
      m_X.unprotect();
      m_h.unprotect();
    }

    void mark() const
    {
      m_symbol.mark();
      m_X.mark();
      m_h.mark();
    }
};

} // namespace detail
} // namespace pbes_system
} // namespace mcrl2
/// \cond INTERNAL_DOCS
namespace atermpp
{
template<>
struct aterm_traits<mcrl2::pbes_system::detail::pfnf_equation>
{
  static void protect(const mcrl2::pbes_system::detail::pfnf_equation& t)
  {
    t.protect();
  }
  static void unprotect(const mcrl2::pbes_system::detail::pfnf_equation& t)
  {
    t.unprotect();
  }
  static void mark(const mcrl2::pbes_system::detail::pfnf_equation& t)
  {
    t.mark();
  }
};
} // namespace atermpp
/// \endcond
namespace mcrl2 {
namespace pbes_system {
namespace detail {

// explicit representation of a pbes in PFNF format
class pfnf_pbes
{
  protected:
    data::data_specification m_data;
    atermpp::vector<pfnf_equation> m_equations;
    atermpp::set<data::variable> m_global_variables;
    pbes_expression m_initial_state;

  public:
    pfnf_pbes()
    {}

    /// \brief Constructor
    /// \pre The pbes p must be in PFNF format
    pfnf_pbes(const pbes<>& p)
      : m_data(p.data()), m_global_variables(p.global_variables()), m_initial_state(p.initial_state())
    {
      pbes<> q = p;
      if (!pbes_system::detail::is_pfnf(p))
      {
        mCRL2log(log::verbose) << "converting PBES into PFNF format... " << std::endl;
        pfnf_rewriter R;
        pbes_system::pbes_rewrite(q, R);
        assert (is_pfnf(q));
      }
      const atermpp::vector<pbes_equation>& equations = q.equations();
      for (atermpp::vector<pbes_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
      {
        m_equations.push_back(pfnf_equation(*i));
      }
    }

    const atermpp::vector<pfnf_equation>& equations() const
    {
      return m_equations;
    }

    atermpp::vector<pfnf_equation>& equations()
    {
      return m_equations;
    }

    const atermpp::set<data::variable>& global_variables() const
    {
      return m_global_variables;
    }

    atermpp::set<data::variable>& global_variables()
    {
      return m_global_variables;
    }

    const pbes_expression& initial_state() const
    {
      return m_initial_state;
    }

    const data::data_specification& data() const
    {
      return m_data;
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PFNF_PBES_H
