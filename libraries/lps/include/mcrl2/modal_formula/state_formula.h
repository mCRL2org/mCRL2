// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/state_formula.h
/// \brief Add your file description here.

#ifndef MCRL2_MODAL_STATE_FORMULA_H
#define MCRL2_MODAL_STATE_FORMULA_H

#include <iostream> // for debugging

#include <string>
#include <cassert>
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/modal_formula/regular_formula.h"
#include "mcrl2/modal_formula/action_formula.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/assignment.h"

namespace mcrl2 {

namespace state_formulas {

///////////////////////////////////////////////////////////////////////////////
// state_formula
/// \brief state formula
//<StateFrm>     ::= <DataExpr>
//                 | StateTrue
//                 | StateFalse
//                 | StateNot(<StateFrm>)
//                 | StateAnd(<StateFrm>, <StateFrm>)
//                 | StateOr(<StateFrm>, <StateFrm>)
//                 | StateImp(<StateFrm>, <StateFrm>)
//                 | StateForall(<DataVarId>+, <StateFrm>)
//                 | StateExists(<DataVarId>+, <StateFrm>)
//                 | StateMust(<RegFrm>, <StateFrm>)
//                 | StateMay(<RegFrm>, <StateFrm>)
//                 | StateYaled
//                 | StateYaledTimed(<DataExpr>)
//                 | StateDelay
//                 | StateDelayTimed(<DataExpr>)
//                 | StateVar(<String>, <DataExpr>*)
//                 | StateNu(<String>, <DataVarIdInit>*, <StateFrm>)
//                 | StateMu(<String>, <DataVarIdInit>*, <StateFrm>)
class state_formula: public atermpp::aterm_appl
{
  public:
    /// \brief Constructor
    state_formula()
      : atermpp::aterm_appl(mcrl2::core::detail::constructStateFrm())
    {}

    /// \brief Constructor
    /// \param t A term
    state_formula(ATermAppl t)
      : atermpp::aterm_appl(atermpp::aterm_appl(t))
    {
      assert(mcrl2::core::detail::check_rule_StateFrm(m_term));
    }

    /// \brief Constructor
    /// \param t A term
    state_formula(atermpp::aterm_appl t)
      : atermpp::aterm_appl(t)
    {
      assert(mcrl2::core::detail::check_rule_StateFrm(m_term));
    }

    /// \brief Returns true if the formula is timed.
    /// \return True if the formula is timed.
    bool has_time() const;

    /// \brief Applies a low level substitution function to this term and returns the result.
    /// \param f A
    /// The function <tt>f</tt> must supply the method <tt>aterm operator()(aterm)</tt>.
    /// This function is applied to all <tt>aterm</tt> noded appearing in this term.
    /// \deprecated
    /// \return The substitution result.
    template <typename Substitution>
    state_formula substitute(Substitution f) const
    {
      return state_formula(f(atermpp::aterm(*this)));
    }
};

///////////////////////////////////////////////////////////////////////////////
// state_formula_list
/// \brief Read-only singly linked list of state formulas
typedef atermpp::term_list<state_formula> state_formula_list;

/// \brief Returns true if the term t is a state formula
/// \param t A term
/// \return True if the term is a state formula
// TODO: generate this function
inline
bool is_state_formula(atermpp::aterm_appl t)
{
  return core::detail::gsIsStateFrm(t);
}

//--- start generated classes ---//
/// \brief The value true for state formulas
class true_: public state_formula
{
  public:
    /// \brief Default constructor.
    true_()
      : state_formula(core::detail::constructStateTrue())
    {}

    /// \brief Constructor.
    /// \param term A term
    true_(atermpp::aterm_appl term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateTrue(m_term));
    }
};

/// \brief The value false for state formulas
class false_: public state_formula
{
  public:
    /// \brief Default constructor.
    false_()
      : state_formula(core::detail::constructStateFalse())
    {}

    /// \brief Constructor.
    /// \param term A term
    false_(atermpp::aterm_appl term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateFalse(m_term));
    }
};

/// \brief The not operator for state formulas
class not_: public state_formula
{
  private:
    /// \brief Constructor.
    /// \param term A term
    explicit not_(atermpp::aterm_appl term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateNot(m_term));
    }
 
  public:
    /// \brief Default constructor.
    not_()
      : state_formula(core::detail::constructStateNot())
    {}

    static inline
    not_ construct(atermpp::aterm_appl term)
    {
      return not_(term);
    }

    /// \brief Constructor.
    not_(const state_formula& operand)
      : state_formula(core::detail::gsMakeStateNot(operand))
    {}

    state_formula operand() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief The and operator for state formulas
class and_: public state_formula
{
  public:
    /// \brief Default constructor.
    and_()
      : state_formula(core::detail::constructStateAnd())
    {}

    /// \brief Constructor.
    /// \param term A term
    and_(atermpp::aterm_appl term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateAnd(m_term));
    }

    /// \brief Constructor.
    and_(const state_formula& left, const state_formula& right)
      : state_formula(core::detail::gsMakeStateAnd(left, right))
    {}

    state_formula left() const
    {
      return atermpp::arg1(*this);
    }

    state_formula right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The or operator for state formulas
class or_: public state_formula
{
  public:
    /// \brief Default constructor.
    or_()
      : state_formula(core::detail::constructStateOr())
    {}

    /// \brief Constructor.
    /// \param term A term
    or_(atermpp::aterm_appl term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateOr(m_term));
    }

    /// \brief Constructor.
    or_(const state_formula& left, const state_formula& right)
      : state_formula(core::detail::gsMakeStateOr(left, right))
    {}

    state_formula left() const
    {
      return atermpp::arg1(*this);
    }

    state_formula right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The implication operator for state formulas
class imp: public state_formula
{
  public:
    /// \brief Default constructor.
    imp()
      : state_formula(core::detail::constructStateImp())
    {}

    /// \brief Constructor.
    /// \param term A term
    imp(atermpp::aterm_appl term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateImp(m_term));
    }

    /// \brief Constructor.
    imp(const state_formula& left, const state_formula& right)
      : state_formula(core::detail::gsMakeStateImp(left, right))
    {}

    state_formula left() const
    {
      return atermpp::arg1(*this);
    }

    state_formula right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The universal quantification operator for state formulas
class forall: public state_formula
{
  public:
    /// \brief Default constructor.
    forall()
      : state_formula(core::detail::constructStateForall())
    {}

    /// \brief Constructor.
    /// \param term A term
    forall(atermpp::aterm_appl term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateForall(m_term));
    }

    /// \brief Constructor.
    forall(const data::variable_list& variables, const state_formula& operand)
      : state_formula(core::detail::gsMakeStateForall(variables, operand))
    {}

    data::variable_list variables() const
    {
      return atermpp::list_arg1(*this);
    }

    state_formula operand() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The existential quantification operator for state formulas
class exists: public state_formula
{
  public:
    /// \brief Default constructor.
    exists()
      : state_formula(core::detail::constructStateExists())
    {}

    /// \brief Constructor.
    /// \param term A term
    exists(atermpp::aterm_appl term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateExists(m_term));
    }

    /// \brief Constructor.
    exists(const data::variable_list& variables, const state_formula& operand)
      : state_formula(core::detail::gsMakeStateExists(variables, operand))
    {}

    data::variable_list variables() const
    {
      return atermpp::list_arg1(*this);
    }

    state_formula operand() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The must operator for state formulas
class must: public state_formula
{
  public:
    /// \brief Default constructor.
    must()
      : state_formula(core::detail::constructStateMust())
    {}

    /// \brief Constructor.
    /// \param term A term
    must(atermpp::aterm_appl term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateMust(m_term));
    }

    /// \brief Constructor.
    must(const regular_formulas::regular_formula& formula, const state_formula& operand)
      : state_formula(core::detail::gsMakeStateMust(formula, operand))
    {}

    regular_formulas::regular_formula formula() const
    {
      return atermpp::arg1(*this);
    }

    state_formula operand() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The may operator for state formulas
class may: public state_formula
{
  public:
    /// \brief Default constructor.
    may()
      : state_formula(core::detail::constructStateMay())
    {}

    /// \brief Constructor.
    /// \param term A term
    may(atermpp::aterm_appl term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateMay(m_term));
    }

    /// \brief Constructor.
    may(const regular_formulas::regular_formula& formula, const state_formula& operand)
      : state_formula(core::detail::gsMakeStateMay(formula, operand))
    {}

    regular_formulas::regular_formula formula() const
    {
      return atermpp::arg1(*this);
    }

    state_formula operand() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The yaled operator for state formulas
class yaled: public state_formula
{
  public:
    /// \brief Default constructor.
    yaled()
      : state_formula(core::detail::constructStateYaled())
    {}

    /// \brief Constructor.
    /// \param term A term
    yaled(atermpp::aterm_appl term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateYaled(m_term));
    }
};

/// \brief The timed yaled operator for state formulas
class yaled_timed: public state_formula
{
  public:
    /// \brief Default constructor.
    yaled_timed()
      : state_formula(core::detail::constructStateYaledTimed())
    {}

    /// \brief Constructor.
    /// \param term A term
    yaled_timed(atermpp::aterm_appl term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateYaledTimed(m_term));
    }

    /// \brief Constructor.
    yaled_timed(const data::data_expression& time_stamp)
      : state_formula(core::detail::gsMakeStateYaledTimed(time_stamp))
    {}

    data::data_expression time_stamp() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief The delay operator for state formulas
class delay: public state_formula
{
  public:
    /// \brief Default constructor.
    delay()
      : state_formula(core::detail::constructStateDelay())
    {}

    /// \brief Constructor.
    /// \param term A term
    delay(atermpp::aterm_appl term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateDelay(m_term));
    }
};

/// \brief The timed delay operator for state formulas
class delay_timed: public state_formula
{
  public:
    /// \brief Default constructor.
    delay_timed()
      : state_formula(core::detail::constructStateDelayTimed())
    {}

    /// \brief Constructor.
    /// \param term A term
    delay_timed(atermpp::aterm_appl term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateDelayTimed(m_term));
    }

    /// \brief Constructor.
    delay_timed(const data::data_expression& time_stamp)
      : state_formula(core::detail::gsMakeStateDelayTimed(time_stamp))
    {}

    data::data_expression time_stamp() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief The state formula variable
class variable: public state_formula
{
  public:
    /// \brief Default constructor.
    variable()
      : state_formula(core::detail::constructStateVar())
    {}

    /// \brief Constructor.
    /// \param term A term
    variable(atermpp::aterm_appl term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateVar(m_term));
    }

    /// \brief Constructor.
    variable(const core::identifier_string& name, const data::data_expression_list& arguments)
      : state_formula(core::detail::gsMakeStateVar(name, arguments))
    {}

    /// \brief Constructor.
    variable(const std::string& name, const data::data_expression_list& arguments)
      : state_formula(core::detail::gsMakeStateVar(core::identifier_string(name), arguments))
    {}

    core::identifier_string name() const
    {
      return atermpp::arg1(*this);
    }

    data::data_expression_list arguments() const
    {
      return atermpp::list_arg2(*this);
    }
};

/// \brief The nu operator for state formulas
class nu: public state_formula
{
  public:
    /// \brief Default constructor.
    nu()
      : state_formula(core::detail::constructStateNu())
    {}

    /// \brief Constructor.
    /// \param term A term
    nu(atermpp::aterm_appl term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateNu(m_term));
    }

    /// \brief Constructor.
    nu(const core::identifier_string& name, const data::assignment_list& assignments, const state_formula& operand)
      : state_formula(core::detail::gsMakeStateNu(name, assignments, operand))
    {}

    /// \brief Constructor.
    nu(const std::string& name, const data::assignment_list& assignments, const state_formula& operand)
      : state_formula(core::detail::gsMakeStateNu(core::identifier_string(name), assignments, operand))
    {}

    core::identifier_string name() const
    {
      return atermpp::arg1(*this);
    }

    data::assignment_list assignments() const
    {
      return atermpp::list_arg2(*this);
    }

    state_formula operand() const
    {
      return atermpp::arg3(*this);
    }
};

/// \brief The mu operator for state formulas
class mu: public state_formula
{
  public:
    /// \brief Default constructor.
    mu()
      : state_formula(core::detail::constructStateMu())
    {}

    /// \brief Constructor.
    /// \param term A term
    mu(atermpp::aterm_appl term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateMu(m_term));
    }

    /// \brief Constructor.
    mu(const core::identifier_string& name, const data::assignment_list& assignments, const state_formula& operand)
      : state_formula(core::detail::gsMakeStateMu(name, assignments, operand))
    {}

    /// \brief Constructor.
    mu(const std::string& name, const data::assignment_list& assignments, const state_formula& operand)
      : state_formula(core::detail::gsMakeStateMu(core::identifier_string(name), assignments, operand))
    {}

    core::identifier_string name() const
    {
      return atermpp::arg1(*this);
    }

    data::assignment_list assignments() const
    {
      return atermpp::list_arg2(*this);
    }

    state_formula operand() const
    {
      return atermpp::arg3(*this);
    }
};
//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a true_ expression
    /// \param t A term
    /// \return True if it is a true_ expression
    inline
    bool is_true(const state_formula& t)
    {
      return core::detail::gsIsStateTrue(t);
    }

    /// \brief Test for a false_ expression
    /// \param t A term
    /// \return True if it is a false_ expression
    inline
    bool is_false(const state_formula& t)
    {
      return core::detail::gsIsStateFalse(t);
    }

    /// \brief Test for a not_ expression
    /// \param t A term
    /// \return True if it is a not_ expression
    inline
    bool is_not(const state_formula& t)
    {
      return core::detail::gsIsStateNot(t);
    }

    /// \brief Test for a and_ expression
    /// \param t A term
    /// \return True if it is a and_ expression
    inline
    bool is_and(const state_formula& t)
    {
      return core::detail::gsIsStateAnd(t);
    }

    /// \brief Test for a or_ expression
    /// \param t A term
    /// \return True if it is a or_ expression
    inline
    bool is_or(const state_formula& t)
    {
      return core::detail::gsIsStateOr(t);
    }

    /// \brief Test for a imp expression
    /// \param t A term
    /// \return True if it is a imp expression
    inline
    bool is_imp(const state_formula& t)
    {
      return core::detail::gsIsStateImp(t);
    }

    /// \brief Test for a forall expression
    /// \param t A term
    /// \return True if it is a forall expression
    inline
    bool is_forall(const state_formula& t)
    {
      return core::detail::gsIsStateForall(t);
    }

    /// \brief Test for a exists expression
    /// \param t A term
    /// \return True if it is a exists expression
    inline
    bool is_exists(const state_formula& t)
    {
      return core::detail::gsIsStateExists(t);
    }

    /// \brief Test for a must expression
    /// \param t A term
    /// \return True if it is a must expression
    inline
    bool is_must(const state_formula& t)
    {
      return core::detail::gsIsStateMust(t);
    }

    /// \brief Test for a may expression
    /// \param t A term
    /// \return True if it is a may expression
    inline
    bool is_may(const state_formula& t)
    {
      return core::detail::gsIsStateMay(t);
    }

    /// \brief Test for a yaled expression
    /// \param t A term
    /// \return True if it is a yaled expression
    inline
    bool is_yaled(const state_formula& t)
    {
      return core::detail::gsIsStateYaled(t);
    }

    /// \brief Test for a yaled_timed expression
    /// \param t A term
    /// \return True if it is a yaled_timed expression
    inline
    bool is_yaled_timed(const state_formula& t)
    {
      return core::detail::gsIsStateYaledTimed(t);
    }

    /// \brief Test for a delay expression
    /// \param t A term
    /// \return True if it is a delay expression
    inline
    bool is_delay(const state_formula& t)
    {
      return core::detail::gsIsStateDelay(t);
    }

    /// \brief Test for a delay_timed expression
    /// \param t A term
    /// \return True if it is a delay_timed expression
    inline
    bool is_delay_timed(const state_formula& t)
    {
      return core::detail::gsIsStateDelayTimed(t);
    }

    /// \brief Test for a variable expression
    /// \param t A term
    /// \return True if it is a variable expression
    inline
    bool is_variable(const state_formula& t)
    {
      return core::detail::gsIsStateVar(t);
    }

    /// \brief Test for a nu expression
    /// \param t A term
    /// \return True if it is a nu expression
    inline
    bool is_nu(const state_formula& t)
    {
      return core::detail::gsIsStateNu(t);
    }

    /// \brief Test for a mu expression
    /// \param t A term
    /// \return True if it is a mu expression
    inline
    bool is_mu(const state_formula& t)
    {
      return core::detail::gsIsStateMu(t);
    }
//--- end generated is-functions ---//

  /// \brief Returns true if the term t is a data expression
  /// \param t A term
  /// \return True if the term t is a data expression
  inline bool is_data(atermpp::aterm_appl t)
  {
    return core::detail::gsIsDataExpr(t);
  }

namespace accessors {

  /// \brief Returns the argument of a data expression
  /// \param t A modal formula
  /// \return The argument of a data expression
  inline
  data::data_expression val(state_formula t)
  {
    assert(core::detail::gsIsDataExpr(t));
    return t;
  }

  /// \brief Returns the state formula argument of an expression of type
  /// not, mu, nu, exists, forall, must or may.
  /// \param t A modal formula
  /// \return The state formula argument of an expression of type
  inline
  state_formula arg(state_formula t)
  {
    if (core::detail::gsIsStateNot(t))
    {
      return atermpp::arg1(t);
    }
    if (core::detail::gsIsStateMu(t) || core::detail::gsIsStateNu(t))
    {
      return atermpp::arg3(t);
    }
    assert(core::detail::gsIsStateExists(t) ||
           core::detail::gsIsStateForall(t) ||
           core::detail::gsIsStateMust(t)   ||
           core::detail::gsIsStateMay(t)
          );
    return atermpp::arg2(t);
  }

  /// \brief Returns the left hand side of an expression of type and/or/imp
  /// \param t A modal formula
  /// \return The left hand side of an expression of type and/or/imp
  inline
  state_formula left(state_formula t)
  {
    assert(core::detail::gsIsStateAnd(t) || core::detail::gsIsStateOr(t) || core::detail::gsIsStateImp(t));
    return atermpp::arg1(t);
  }

  /// \brief Returns the right hand side of an expression of type and/or/imp.
  /// \param t A modal formula
  /// \return The right hand side of an expression of type and/or/imp.
  inline
  state_formula right(state_formula t)
  {
    assert(core::detail::gsIsStateAnd(t) || core::detail::gsIsStateOr(t) || core::detail::gsIsStateImp(t));
    return atermpp::arg2(t);
  }

  /// \brief Returns the variables of a quantification expression
  /// \param t A modal formula
  /// \return The variables of a quantification expression
  inline
  data::variable_list var(state_formula t)
  {
    assert(core::detail::gsIsStateExists(t) || core::detail::gsIsStateForall(t));
    return data::variable_list(
      atermpp::term_list_iterator< data::variable >(atermpp::list_arg1(t)),
      atermpp::term_list_iterator< data::variable >());
  }

  /// \brief Returns the time of a delay or yaled expression
  /// \param t A modal formula
  /// \return The time of a delay or yaled expression
  inline
  data::data_expression time(state_formula t)
  {
    assert(core::detail::gsIsStateDelayTimed(t) || core::detail::gsIsStateYaledTimed(t));
    return atermpp::arg1(t);
  }

  /// \brief Returns the name of a variable expression
  /// \param t A modal formula
  /// \return The name of a variable expression
  inline
  core::identifier_string name(state_formula t)
  {
    assert(core::detail::gsIsStateVar(t) ||
           core::detail::gsIsStateMu(t)  ||
           core::detail::gsIsStateNu(t)
          );
    return atermpp::arg1(t);
  }

  /// \brief Returns the parameters of a variable expression
  /// \param t A modal formula
  /// \return The parameters of a variable expression
  inline
  data::data_expression_list param(state_formula t)
  {
    assert(core::detail::gsIsStateVar(t));
    return data::data_expression_list(
      atermpp::term_list_iterator< data::data_expression >(atermpp::list_arg2(t)),
      atermpp::term_list_iterator< data::data_expression >());
  }

  /// \brief Returns the parameters of a mu or nu expression
  /// \param t A modal formula
  /// \return The parameters of a mu or nu expression
  inline
  data::assignment_list ass(state_formula t)
  {
    assert(core::detail::gsIsStateMu(t) || core::detail::gsIsStateNu(t));
    return data::assignment_list(
      atermpp::term_list_iterator< data::assignment >(atermpp::list_arg2(t)),
      atermpp::term_list_iterator< data::assignment >());
  }

  /// \brief Returns the regular formula of a must or may expression
  /// \param t A modal formula
  /// \return The regular formula of a must or may expression
  inline
  regular_formulas::regular_formula act(state_formula t)
  {
    assert(core::detail::gsIsStateMust(t) || core::detail::gsIsStateMay(t));
    return atermpp::arg1(t);
  }

} // namespace accessors

  /// \cond INTERNAL_DOCS
  //
  /// \brief Function that determines if a state formula is time dependent
  struct is_timed_subterm
  {
    /// \brief Function call operator
    /// \param t A term
    /// \return The function result
    bool operator()(atermpp::aterm_appl t) const
    {
      return (is_state_formula(t) && (is_delay_timed(t) || is_yaled_timed(t)))
        || action_formulas::act_frm::is_at(t);
    }
  };
  /// \endcond

  /// \brief Returns true if the formula is timed.
  /// \return True if the formula is timed.
  inline
  bool state_formula::has_time() const
  {
    return atermpp::find_if(*this, is_timed_subterm()) != atermpp::aterm();
  }

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_STATE_FORMULA_H
