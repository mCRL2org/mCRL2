// Copyright (c) 2007 Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file mcrl2/pbes/pbes_equation.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_PBES_EQUATION_H
#define MCRL2_PBES_PBES_EQUATION_H

#include "atermpp/algorithm.h"
#include "mcrl2/lps/mucalculus.h"
#include "mcrl2/pbes/fixpoint_symbol.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/propositional_variable.h"

namespace lps {

using atermpp::aterm;
using atermpp::aterm_appl;

///////////////////////////////////////////////////////////////////////////////
// pbes_equation
/// \brief pbes equation.
///
class pbes_equation: public aterm_appl
{
  protected:
    fixpoint_symbol        m_symbol;
    propositional_variable m_variable;
    pbes_expression        m_formula;  // the right hand side

  public:
    pbes_equation()
      : aterm_appl(detail::constructPBEqn())
    {}

    pbes_equation(aterm_appl t)
      : aterm_appl(t)
    {
      assert(detail::check_rule_PBEqn(m_term));
      iterator i = t.begin();
      m_symbol   = fixpoint_symbol(*i++);
      m_variable = propositional_variable(*i++);
      m_formula  = pbes_expression(*i);
    }

    pbes_equation(fixpoint_symbol symbol, propositional_variable variable, pbes_expression expr)
      : aterm_appl(gsMakePBEqn(symbol, variable, expr)),
        m_symbol(symbol),
        m_variable(variable),
        m_formula(expr)
    {
    }

    // allow assignment to aterms
    pbes_equation& operator=(aterm t)
    {
      m_term = t;
      return *this;
    }

    /// Returns the fixpoint symbol of the equation.
    ///
    fixpoint_symbol symbol() const
    {
      return m_symbol;
    }

    /// Returns the pbes variable of the equation.
    ///
    propositional_variable variable() const
    {
      return m_variable;
    }

    /// Returns the predicate formula on the right hand side of the equation.
    ///
    pbes_expression formula() const
    {
      return m_formula;
    }
    
    /// Returns true if the predicate formula on the right hand side contains no predicate variables.
    /// (Comment Wieger: is_const would be a better name)
    ///
    bool is_solved() const
    {
      aterm t = atermpp::find_if(ATermAppl(m_formula), state_frm::is_var);
      return t == aterm(); // true if nothing was found
    }

    /// Returns true if the equation is a BES (boolean equation system).
    ///
    bool is_bes() const
    {
      return variable().parameters().empty() && formula().is_bes();
    }
};

///////////////////////////////////////////////////////////////////////////////
// pbes_equation_list
/// \brief singly linked list of data expressions
///
typedef term_list<pbes_equation> pbes_equation_list;

} // namespace lps

/// INTERNAL ONLY
namespace atermpp {

using lps::pbes_equation;

template <>
struct term_list_iterator_traits<pbes_equation>
{
  typedef ATermAppl value_type;
};

} // namespace atermpp

/// INTERNAL ONLY
namespace atermpp
{
using lps::pbes_equation;

template<>
struct aterm_traits<pbes_equation>
{
  typedef ATermAppl aterm_type;
  static void protect(pbes_equation t)   { t.protect(); }
  static void unprotect(pbes_equation t) { t.unprotect(); }
  static void mark(pbes_equation t)      { t.mark(); }
  static ATerm term(pbes_equation t)     { return t.term(); }
  static ATerm* ptr(pbes_equation& t)    { return &t.term(); }
};

} // namespace atermpp

#endif // MCRL2_PBES_PBES_EQUATION_H
