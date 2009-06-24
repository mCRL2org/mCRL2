// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/mucalculus.h
/// \brief Add your file description here.

#ifndef MCRL2_MODAL_ACTION_FORMULA_H
#define MCRL2_MODAL_ACTION_FORMULA_H

#include <iostream> // for debugging

#include <string>
#include <cassert>
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/lps/action.h"

namespace mcrl2 {

namespace modal_formula {

///////////////////////////////////////////////////////////////////////////////
// action_formula
/// \brief action formula expression.
//<ActFrm>       ::= <MultAct>
//                 | <DataExpr>
//                 | ActTrue
//                 | ActFalse
//                 | ActNot(<ActFrm>)
//                 | ActAnd(<ActFrm>, <ActFrm>)
//                 | ActOr(<ActFrm>, <ActFrm>)
//                 | ActImp(<ActFrm>, <ActFrm>)
//                 | ActForall(<DataVarId>+, <ActFrm>)
//                 | ActExists(<DataVarId>+, <ActFrm>)
//                 | ActAt(<ActFrm>, <DataExpr>)
class action_formula: public atermpp::aterm_appl
{
  public:

    /// \brief Constructor
    action_formula()
      : atermpp::aterm_appl(mcrl2::core::detail::constructActFrm())
    {}

    /// \brief Constructor
    /// \param t A term
    action_formula(ATermAppl t)
      : atermpp::aterm_appl(atermpp::aterm_appl(t))
    {
      assert(mcrl2::core::detail::check_rule_ActFrm(m_term));
    }

    /// \brief Constructor
    /// \param t A term
    action_formula(atermpp::aterm_appl t)
      : atermpp::aterm_appl(t)
    {
      assert(mcrl2::core::detail::check_rule_ActFrm(m_term));
    }

    /// \brief Applies a low level substitution function to this term and returns the result.
    /// \param f A
    /// The function <tt>f</tt> must supply the method <tt>aterm operator()(aterm)</tt>.
    /// This function is applied to all <tt>aterm</tt> noded appearing in this term.
    /// \deprecated
    /// \return The substitution result.
    template <typename Substitution>
    action_formula substitute(Substitution f) const
    {
      return action_formula(f(atermpp::aterm(*this)));
    }
};

///////////////////////////////////////////////////////////////////////////////
// action_formula_list
/// \brief Read-only singly linked list of data expressions
typedef atermpp::term_list<action_formula> action_formula_list;

/// Accessor functions and predicates for action formulas.
namespace act_frm
{

  /// \brief Make the value true
  /// \return The value \p true
  inline
  action_formula true_()
  {
    return action_formula(core::detail::gsMakeActTrue());
  }

  /// \brief Make the value false
  /// \return The value \p false
  inline
  action_formula false_()
  {
    return action_formula(core::detail::gsMakeActFalse());
  }

  /// \brief Make a negation
  /// \param p An action formula
  /// \return The value <tt>!p</tt>
  inline
  action_formula not_(action_formula p)
  {
    return action_formula(core::detail::gsMakeActNot(p));
  }

  /// \brief Make a conjunction
  /// \param p An action formula
  /// \param q An action formula
  /// \return The value <tt>p && q</tt>
  inline
  action_formula and_(action_formula p, action_formula q)
  {
    return action_formula(core::detail::gsMakeActAnd(p,q));
  }

  /// \brief Make a disjunction
  /// \param p An action formula
  /// \param q An action formula
  /// \return The value <tt>p || q</tt>
  inline
  action_formula or_(action_formula p, action_formula q)
  {
    return action_formula(core::detail::gsMakeActOr(p,q));
  }

  /// \brief Make an implication
  /// \param p An action formula
  /// \param q An action formula
  /// \return The value <tt>p => q</tt>
  inline
  action_formula imp(action_formula p, action_formula q)
  {
    return action_formula(core::detail::gsMakeActImp(p,q));
  }

  /// \brief Make a universal quantification
  /// \pre l may not be empty
  /// \param l A sequence of data variables
  /// \param p An action formula
  /// \return The value <tt>forall l.p</tt>
  inline
  action_formula forall(data::variable_list l, action_formula p)
  {
    assert(!l.empty());
    return action_formula(core::detail::gsMakeActForall(
      atermpp::term_list< data::variable >(l.begin(), l.end()), p));
  }

  /// \brief Make an existential quantification
  /// \pre l may not be empty
  /// \param l A sequence of data variables
  /// \param p An action formula
  /// \return The value <tt>exists l.p</tt>
  inline
  action_formula exists(data::variable_list l, action_formula p)
  {
    assert(!l.empty());
    return action_formula(core::detail::gsMakeActExists(
      atermpp::term_list< data::variable >(l.begin(), l.end()), p));
  }

  /// \brief Returns the operation 'p at d'
  /// \param p An action formula
  /// \param d A data expression
  /// \return The operation 'p at d'
  inline
  action_formula at(action_formula p, data::data_expression d)
  {
    return action_formula(core::detail::gsMakeActAt(p,d));
  }

  /// \brief Returns true if the term t is a multi action
  inline bool is_mult_act(atermpp::aterm_appl t) { return core::detail::gsIsMultAct(t); }

  /// \brief Returns true if the term t is a data expression
  inline bool is_data(atermpp::aterm_appl t) { return core::detail::gsIsDataExpr(t); }

  /// \brief Returns true if the term t is equal to true
  inline bool is_true(atermpp::aterm_appl t) { return core::detail::gsIsActTrue(t); }

  /// \brief Returns true if the term t is equal to false
  inline bool is_false(atermpp::aterm_appl t) { return core::detail::gsIsActFalse(t); }

  /// \brief Returns true if the term t is a not expression
  inline bool is_not(atermpp::aterm_appl t) { return core::detail::gsIsActNot(t); }

  /// \brief Returns true if the term t is an and expression
  inline bool is_and(atermpp::aterm_appl t) { return core::detail::gsIsActAnd(t); }

  /// \brief Returns true if the term t is an or expression
  inline bool is_or(atermpp::aterm_appl t) { return core::detail::gsIsActOr(t); }

  /// \brief Returns true if the term t is an implication expression
  inline bool is_imp(atermpp::aterm_appl t) { return core::detail::gsIsActImp(t); }

  /// \brief Returns true if the term t is a universal quantification
  inline bool is_forall(atermpp::aterm_appl t) { return core::detail::gsIsActForall(t); }

  /// \brief Returns true if the term t is an existential quantification
  inline bool is_exists(atermpp::aterm_appl t) { return core::detail::gsIsActExists(t); }

  /// \brief Returns true if the term t is an at expression
  inline bool is_at(atermpp::aterm_appl t) { return core::detail::gsIsActAt(t); }

} // namespace act_frm

namespace accessors
{
  /// \brief Returns the parameters of an action formula
  /// \param t An action formula
  /// \return The parameters of an action formula
  inline
  lps::action_list mult_params(action_formula t)
  {
    assert(core::detail::gsIsMultAct(t));
    return atermpp::list_arg1(t);
  }

  /// \brief Returns the action formula argument of an expression of type not, at, exists or forall.
  /// \param t An action formula
  /// \return The action formula argument of an expression of type not, at, exists or forall.
  inline
  action_formula arg(action_formula t)
  {
    if (core::detail::gsIsActNot(t) || core::detail::gsIsActAt(t))
    {
      return atermpp::arg1(t);
    }
    assert(core::detail::gsIsActExists(t) || core::detail::gsIsActForall(t));
    return atermpp::arg2(t);
  }

  /// \brief Returns the left hand side of an expression of type and/or/imp
  /// \param t An action formula
  /// \return The left hand side of an expression of type and/or/imp
  inline
  action_formula left(action_formula t)
  {
    assert(core::detail::gsIsActAnd(t) || core::detail::gsIsActOr(t) || core::detail::gsIsActImp(t));
    return atermpp::arg1(t);
  }

  /// \brief Returns the right hand side of an expression of type and/or/imp.
  /// \param t An action formula
  /// \return The right hand side of an expression of type and/or/imp.
  inline
  action_formula right(action_formula t)
  {
    assert(core::detail::gsIsActAnd(t) || core::detail::gsIsActOr(t) || core::detail::gsIsActImp(t));
    return atermpp::arg2(t);
  }

  /// \brief Returns the variables of a quantification expression
  /// \param t An action formula
  /// \return The variables of a quantification expression
  inline
  data::variable_list var(action_formula t)
  {
    assert(core::detail::gsIsActExists(t) || core::detail::gsIsActForall(t));
    return data::variable_list(
             atermpp::term_list_iterator< data::variable >(atermpp::list_arg1(t)),
             atermpp::term_list_iterator< data::variable >());
  }

  /// \brief Returns the time of an at expression
  /// \param t An action formula
  /// \return The time of an at expression
  inline
  data::data_expression time(action_formula t)
  {
    assert(core::detail::gsIsActAt(t));
    return atermpp::arg2(t);
  }

} // namespace accessors

} // namespace modal_formula

} // namespace mcrl2

#endif // MCRL2_MODAL_ACTION_FORMULA_H
