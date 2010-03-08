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

#include "mcrl2/data/detail/sequence_algorithm.h"
#include "mcrl2/pbes/fixpoint_symbol.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/propositional_variable.h"
#include "mcrl2/pbes/pbes_expression_visitor.h"
#include "mcrl2/pbes/detail/quantifier_visitor.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  struct propositional_variable_visitor: public pbes_expression_visitor<pbes_expression>
  {
    struct found_propositional_variable
    {};
    
    /// \brief Visit propositional_variable node
    /// \param e A term
    /// \return The result of visiting the node
    bool visit_propositional_variable(const pbes_expression& /* e */, const propositional_variable_instantiation& v)
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

/// \brief pbes equation.
class pbes_equation
{
  friend struct atermpp::aterm_traits<pbes_equation>;
  
  protected:
    /// \brief The fixpoint symbol of the equation
    fixpoint_symbol m_symbol;

    /// \brief The variable on the left hand side of the equation
    propositional_variable m_variable;

    /// \brief The expression on the right hand side of the equation
    pbes_expression m_formula;

    /// \brief Protects the term from being freed during garbage collection.
    void protect()
    {
      m_symbol.protect();
      m_variable.protect();
      m_formula.protect();
    }

    /// \brief Unprotect the term.
    /// Releases protection of the term which has previously been protected through a
    /// call to protect.
    void unprotect()
    {
      m_symbol.unprotect();
      m_variable.unprotect();
      m_formula.unprotect();
    }

    /// \brief Mark the term for not being garbage collected.
    void mark()
    {
      m_symbol.mark();
      m_variable.mark();
      m_formula.mark();
    }

  public:
    /// \brief The expression type of the equation.
    typedef pbes_expression term_type;

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

    /// \brief Returns true if the equation is a BES (boolean equation system).
    /// \return True if the equation is a BES (boolean equation system).
    bool is_bes() const
    {
      return variable().parameters().empty() && formula().is_bes();
    }

    /// \brief Checks if the equation is well typed
    /// \return True if
    /// <ul>
    /// <li>the binding variable parameters have unique names</li>
    /// <li>the names of the quantifier variables in the equation are disjoint with the binding variable parameter names</li>
    /// <li>within the scope of a quantifier variable in the formula, no other quantifier variables with the same name may occur</li>
    /// </ul>
    bool is_well_typed() const
    {
      // check 1)
      if (data::detail::sequence_contains_duplicates(
             boost::make_transform_iterator(variable().parameters().begin(), data::detail::variable_name()),
             boost::make_transform_iterator(variable().parameters().end()  , data::detail::variable_name())
            )
         )
      {
        std::cerr << "pbes_equation::is_well_typed() failed: the names of the binding variable parameters are not unique" << std::endl;
        return false;
      }

      // check 2)
      detail::quantifier_visitor qvisitor;
      qvisitor.visit(formula());
      if (data::detail::sequences_do_overlap(
             boost::make_transform_iterator(variable().parameters().begin(), data::detail::variable_name()),
             boost::make_transform_iterator(variable().parameters().end()  , data::detail::variable_name()),
             boost::make_transform_iterator(qvisitor.variables.begin()     , data::detail::variable_name()),
             boost::make_transform_iterator(qvisitor.variables.end()       , data::detail::variable_name())
           )
         )
      {
        std::cerr << "pbes_equation::is_well_typed() failed: the names of the quantifier variables and the names of the binding variable parameters are not disjoint in expression " << mcrl2::core::pp(pbes_equation_to_aterm(*this)) << std::endl;
        return false;
      }

      // check 3)
      detail::quantifier_name_clash_visitor nvisitor;
      nvisitor.visit(formula());
      if (nvisitor.result)
      {
        std::cerr << "pbes_equation::is_well_typed() failed: the quantifier variable " << mcrl2::core::pp(nvisitor.name_clash) << " occurs within the scope of a quantifier variable with the same name." << std::endl;
        return false;
      }

      return true;
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

/// \brief Conversion to ATermAppl.
/// \return The specification converted to ATerm format.
inline
atermpp::aterm_appl pbes_equation_to_aterm(const pbes_equation& eqn)
{
  return core::detail::gsMakePBEqn(eqn.symbol(), eqn.variable(), eqn.formula());
}

} // namespace pbes_system

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp {

template<>
struct aterm_traits<mcrl2::pbes_system::pbes_equation>
{
  typedef ATermAppl aterm_type;
  static void protect(mcrl2::pbes_system::pbes_equation t)   { t.protect(); }
  static void unprotect(mcrl2::pbes_system::pbes_equation t) { t.unprotect(); }
  static void mark(mcrl2::pbes_system::pbes_equation t)      { t.mark(); }
  //static ATerm term(mcrl2::pbes_system::pbes_equation t)     { return t.term(); }
  //static ATerm* ptr(mcrl2::pbes_system::pbes_equation& t)    { return &t.term(); }
};

} // namespace atermpp
/// \endcond

#endif // MCRL2_PBES_PBES_EQUATION_H
