// Author(s): Wieger Wesselink
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
#include "mcrl2/modal_formula/mucalculus.h"
#include "mcrl2/lps/detail/sequence_algorithm.h"
#include "mcrl2/pbes/fixpoint_symbol.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/propositional_variable.h"
#include "mcrl2/pbes/detail/quantifier_visitor.h"

namespace lps {

using atermpp::aterm;
using atermpp::aterm_appl;

/// \brief pbes equation.
///
class pbes_equation: public aterm_appl
{
  protected:
    fixpoint_symbol        m_symbol;
    propositional_variable m_variable;
    pbes_expression        m_formula;  // the right hand side

  public:
    /// Constructor.
    ///
    pbes_equation()
      : aterm_appl(detail::constructPBEqn())
    {}

    /// Constructor.
    ///
    pbes_equation(aterm_appl t)
      : aterm_appl(t)
    {
      assert(detail::check_rule_PBEqn(m_term));
      iterator i = t.begin();
      m_symbol   = fixpoint_symbol(*i++);
      m_variable = propositional_variable(*i++);
      m_formula  = pbes_expression(*i);
    }

    /// Constructor.
    ///
    pbes_equation(fixpoint_symbol symbol, propositional_variable variable, pbes_expression expr)
      : aterm_appl(gsMakePBEqn(symbol, variable, expr)),
        m_symbol(symbol),
        m_variable(variable),
        m_formula(expr)
    {
    }

    /// Assignment operator.
    ///
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
    // (Comment Wieger: is_const would be a better name)
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

    /// Returns true if
    /// <ul>
    /// <li>the binding variable parameters have unique names</li>
    /// <li>the names of the quantifier variables in the equation are disjoint with the binding variable parameter names</li>
    /// <li>within the scope of a quantifier variable in the formula, no other quantifier variables with the same name may occur</li>
    /// </ul>
    ///
    bool is_well_typed() const
    {
      // check 1)
      if (detail::sequence_contains_duplicates(
             boost::make_transform_iterator(variable().parameters().begin(), detail::data_variable_name()),
             boost::make_transform_iterator(variable().parameters().end()  , detail::data_variable_name())
            )
         )
      {
        std::cerr << "pbes_equation::is_well_typed() failed: the names of the binding variable parameters are not unique" << std::endl;
        return false;
      }

      // check 2)
      detail::quantifier_visitor qvisitor;
      qvisitor.visit(formula());
      if (detail::sequences_do_overlap(
             boost::make_transform_iterator(variable().parameters().begin(), detail::data_variable_name()),
             boost::make_transform_iterator(variable().parameters().end()  , detail::data_variable_name()),
             boost::make_transform_iterator(qvisitor.variables.begin()     , detail::data_variable_name()),
             boost::make_transform_iterator(qvisitor.variables.end()       , detail::data_variable_name())
           )
         )
      {
        std::cerr << "pbes_equation::is_well_typed() failed: the names of the quantifier variables and the names of the binding variable parameters are not disjoint in expression " << pp(*this) << std::endl;
        return false;
      }

      // check 3)
      detail::quantifier_name_clash_visitor nvisitor;
      nvisitor.visit(formula());
      if (nvisitor.result)
      {
        std::cerr << "pbes_equation::is_well_typed() failed: the quantifier variable " << pp(nvisitor.name_clash) << " occurs within the scope of a quantifier variable with the same name." << std::endl;
        return false;
      }

      return true;
    }
};

/// \brief singly linked list of data expressions
///
typedef term_list<pbes_equation> pbes_equation_list;

} // namespace lps

/// \cond INTERNAL_DOCS
namespace atermpp {

using lps::pbes_equation;

template <>
struct term_list_iterator_traits<pbes_equation>
{
  typedef ATermAppl value_type;
};

} // namespace atermpp
/// \endcond

/// \cond INTERNAL_DOCS
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
/// \endcond

#endif // MCRL2_PBES_PBES_EQUATION_H
