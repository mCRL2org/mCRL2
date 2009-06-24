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

#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/data/detail/sequence_algorithm.h"
#include "mcrl2/pbes/fixpoint_symbol.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/propositional_variable.h"
#include "mcrl2/pbes/detail/quantifier_visitor.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief pbes equation.
class pbes_equation: public atermpp::aterm_appl
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

    /// \brief Constructor.
    pbes_equation()
      : atermpp::aterm_appl(core::detail::constructPBEqn())
    {}

    /// \brief Constructor.
    /// \param t A term
    pbes_equation(atermpp::aterm_appl t)
      : atermpp::aterm_appl(t)
    {
      assert(core::detail::check_rule_PBEqn(m_term));
      iterator i = t.begin();
      m_symbol   = fixpoint_symbol(*i++);
      m_variable = propositional_variable(*i++);
      m_formula  = pbes_expression(*i);
    }

    /// \brief Constructor.
    /// \param symbol A fixpoint symbol
    /// \param variable A propositional variable declaration
    /// \param expr A PBES expression
    pbes_equation(fixpoint_symbol symbol, propositional_variable variable, pbes_expression expr)
      : atermpp::aterm_appl(core::detail::gsMakePBEqn(symbol, variable, expr)),
        m_symbol(symbol),
        m_variable(variable),
        m_formula(expr)
    {
    }

    /// \brief Assignment operator.
    pbes_equation& operator=(atermpp::aterm t)
    {
      m_term = t;
      return *this;
    }

    /// \brief Returns the fixpoint symbol of the equation.
    /// \return The fixpoint symbol of the equation.
    fixpoint_symbol symbol() const
    {
      return m_symbol;
    }

    /// \brief Returns the pbes variable of the equation.
    /// \return The pbes variable of the equation.
    propositional_variable variable() const
    {
      return m_variable;
    }

    /// \brief Returns the predicate formula on the right hand side of the equation.
    /// \return The predicate formula on the right hand side of the equation.
    pbes_expression formula() const
    {
      return m_formula;
    }

    /// \brief Returns true if the predicate formula on the right hand side contains no predicate variables.
    // (Comment Wieger: is_const would be a better name)
    /// \return True if the predicate formula on the right hand side contains no predicate variables.
    bool is_solved() const
    {
      atermpp::aterm t = atermpp::find_if(ATermAppl(m_formula), is_propositional_variable_instantiation);
      return t == atermpp::aterm(); // true if nothing was found
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
        std::cerr << "pbes_equation::is_well_typed() failed: the names of the quantifier variables and the names of the binding variable parameters are not disjoint in expression " << mcrl2::core::pp(*this) << std::endl;
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

/// \brief Read-only singly linked list of data expressions
typedef atermpp::term_list<pbes_equation> pbes_equation_list;

/// \brief Traverses the summand, and writes all sort expressions
/// that are encountered to the output range [dest, ...).
template <typename OutIter>
void traverse_sort_expressions(const pbes_equation& e, OutIter dest)
{
  traverse_sort_expressions(e.variable(), dest);
  traverse_sort_expressions(e.formula(), dest);
}

} // namespace pbes_system

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp {

using mcrl2::pbes_system::pbes_equation;

template <>
struct term_list_iterator_traits<pbes_equation>
{
  typedef ATermAppl value_type;
};

} // namespace atermpp
/// \endcond

#endif // MCRL2_PBES_PBES_EQUATION_H
