// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_equation.h
/// \brief The class pbes_equation.

#ifndef MCRL2_PBES_PBES_EQUATION_H
#define MCRL2_PBES_PBES_EQUATION_H

#include <string>
#include <sstream>
#include "mcrl2/data/detail/sequence_algorithm.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/pbes/fixpoint_symbol.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/propositional_variable.h"
#include "mcrl2/pbes/pbes_expression_visitor.h"

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

struct propositional_variable_visitor: public pbes_expression_visitor<pbes_expression>
{
  struct found_propositional_variable
    {};

  /// \brief Visit propositional_variable node
  /// \return The result of visiting the node
  bool visit_propositional_variable(const pbes_expression& /* e */, const propositional_variable_instantiation& /* v */)
  {
    throw found_propositional_variable();
    return true;
  }
};

inline
bool has_propositional_variables(const pbes_expression& t)
{
  propositional_variable_visitor visitor;
  try
  {
    visitor.visit(t);
  }
  catch (propositional_variable_visitor::found_propositional_variable&)
  {
    return true;
  }
  return false;
}

} // namespace detail

class pbes_equation;
atermpp::aterm_appl pbes_equation_to_aterm(const pbes_equation& eqn);
bool is_well_typed(const pbes_equation& eqn);

/// \brief pbes equation.
class pbes_equation
{
  protected:
    /// \brief The fixpoint symbol of the equation
    fixpoint_symbol m_symbol;

    /// \brief The variable on the left hand side of the equation
    propositional_variable m_variable;

    /// \brief The expression on the right hand side of the equation
    pbes_expression m_formula;

  public:
    /// \brief The expression type of the equation.
    typedef pbes_expression term_type;

    /// \brief The variable type of the equation.
    typedef propositional_variable variable_type;

    /// \brief The symbol type of the equation.
    typedef fixpoint_symbol symbol_type;

    /// \brief Constructor.
    pbes_equation()
    {}

    /// \brief Constructor.
    /// \param t A term
    pbes_equation(atermpp::aterm_appl t)
    {
      assert(core::detail::check_rule_PBEqn(t));
      atermpp::aterm_appl::iterator i = t.begin();
      m_symbol   = fixpoint_symbol(*i++);
      m_variable = propositional_variable(*i++);
      m_formula  = pbes_expression(*i);
    }

    /// \brief Constructor.
    /// \param t1 A term
    explicit pbes_equation(const atermpp::aterm & t1)
    {
      atermpp::aterm_appl t(t1);
      assert(core::detail::check_rule_PBEqn(t));
      atermpp::aterm_appl::iterator i = t.begin();
      m_symbol   = fixpoint_symbol(*i++);
      m_variable = propositional_variable(*i++);
      m_formula  = pbes_expression(*i);
    }

    /// \brief Constructor.
    /// \param symbol A fixpoint symbol
    /// \param variable A propositional variable declaration
    /// \param expr A PBES expression
    pbes_equation(const fixpoint_symbol& symbol, const propositional_variable& variable, const pbes_expression& expr)
      :
      m_symbol(symbol),
      m_variable(variable),
      m_formula(expr)
    {
    }

    /// \brief Returns the fixpoint symbol of the equation.
    /// \return The fixpoint symbol of the equation.
    const fixpoint_symbol& symbol() const
    {
      return m_symbol;
    }

    /// \brief Returns the fixpoint symbol of the equation.
    /// \return The fixpoint symbol of the equation.
    fixpoint_symbol& symbol()
    {
      return m_symbol;
    }

    /// \brief Returns the pbes variable of the equation.
    /// \return The pbes variable of the equation.
    const propositional_variable& variable() const
    {
      return m_variable;
    }

    /// \brief Returns the pbes variable of the equation.
    /// \return The pbes variable of the equation.
    propositional_variable& variable()
    {
      return m_variable;
    }

    /// \brief Returns the predicate formula on the right hand side of the equation.
    /// \return The predicate formula on the right hand side of the equation.
    const pbes_expression& formula() const
    {
      return m_formula;
    }

    /// \brief Returns the predicate formula on the right hand side of the equation.
    /// \return The predicate formula on the right hand side of the equation.
    pbes_expression& formula()
    {
      return m_formula;
    }

    /// \brief Returns true if the predicate formula on the right hand side contains no predicate variables.
    // (Comment Wieger: is_const would be a better name)
    /// \return True if the predicate formula on the right hand side contains no predicate variables.
    bool is_solved() const
    {
      return !detail::has_propositional_variables(formula());
    }
};

inline bool
operator==(const pbes_equation& x, const pbes_equation& y)
{
  return x.symbol() == y.symbol() &&
         x.variable() == y.variable() &&
         x.formula() == y.formula();
}

inline bool
operator!=(const pbes_equation& x, const pbes_equation& y)
{
  return !(x == y);
}

/// \brief Conversion to atermaPpl.
/// \return The specification converted to aterm format.
inline
atermpp::aterm_appl pbes_equation_to_aterm(const pbes_equation& eqn)
{
  return core::detail::gsMakePBEqn(eqn.symbol(), eqn.variable(), eqn.formula());
}

/// \brief vector of process equations
typedef std::vector<pbes_equation> pbes_equation_vector;

// template function overloads
std::string pp(const pbes_equation& x);
std::string pp(const pbes_equation_vector& x);
void normalize_sorts(pbes_equation_vector& x, const data::data_specification& dataspec);
std::set<data::variable> find_free_variables(const pbes_system::pbes_equation& x);

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBES_EQUATION_H
