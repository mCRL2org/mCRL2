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
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/detail/precedence.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/modal_formula/regular_formula.h"
#include "mcrl2/modal_formula/action_formula.h"

namespace mcrl2
{

namespace state_formulas
{

//--- start generated classes ---//
/// \brief A state formula
class state_formula: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    state_formula()
      : atermpp::aterm_appl(core::detail::constructStateFrm())
    {}

    /// \brief Constructor.
    /// \param term A term
    state_formula(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_StateFrm(*this));
    }
//--- start user section state_formula ---//
    /// \brief Returns true if the formula is timed.
    /// \return True if the formula is timed.
    bool has_time() const;
//--- end user section state_formula ---//
};

/// \brief list of state_formulas
typedef atermpp::term_list<state_formula> state_formula_list;

/// \brief vector of state_formulas
typedef std::vector<state_formula>    state_formula_vector;


/// \brief Test for a state_formula expression
/// \param t A term
/// \return True if it is a state_formula expression
inline
bool is_state_formula(const atermpp::aterm_appl& t)
{
  return core::detail::gsIsStateFrm(t);
}


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
    true_(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateTrue(*this));
    }
};

/// \brief Test for a true expression
/// \param t A term
/// \return True if it is a true expression
inline
bool is_true(const state_formula& t)
{
  return core::detail::gsIsStateTrue(t);
}


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
    false_(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateFalse(*this));
    }
};

/// \brief Test for a false expression
/// \param t A term
/// \return True if it is a false expression
inline
bool is_false(const state_formula& t)
{
  return core::detail::gsIsStateFalse(t);
}


/// \brief The not operator for state formulas
class not_: public state_formula
{
  public:
    /// \brief Default constructor.
    not_()
      : state_formula(core::detail::constructStateNot())
    {}

    /// \brief Constructor.
    /// \param term A term
    not_(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateNot(*this));
    }

    /// \brief Constructor.
    not_(const state_formula& operand)
      : state_formula(core::detail::gsMakeStateNot(operand))
    {}

    const state_formula& operand() const
    {
      return atermpp::aterm_cast<const state_formula>(atermpp::arg1(*this));
    }
};

/// \brief Test for a not expression
/// \param t A term
/// \return True if it is a not expression
inline
bool is_not(const state_formula& t)
{
  return core::detail::gsIsStateNot(t);
}


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
    and_(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateAnd(*this));
    }

    /// \brief Constructor.
    and_(const state_formula& left, const state_formula& right)
      : state_formula(core::detail::gsMakeStateAnd(left, right))
    {}

    const state_formula& left() const
    {
      return atermpp::aterm_cast<const state_formula>(atermpp::arg1(*this));
    }

    const state_formula& right() const
    {
      return atermpp::aterm_cast<const state_formula>(atermpp::arg2(*this));
    }
};

/// \brief Test for a and expression
/// \param t A term
/// \return True if it is a and expression
inline
bool is_and(const state_formula& t)
{
  return core::detail::gsIsStateAnd(t);
}


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
    or_(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateOr(*this));
    }

    /// \brief Constructor.
    or_(const state_formula& left, const state_formula& right)
      : state_formula(core::detail::gsMakeStateOr(left, right))
    {}

    const state_formula& left() const
    {
      return atermpp::aterm_cast<const state_formula>(atermpp::arg1(*this));
    }

    const state_formula& right() const
    {
      return atermpp::aterm_cast<const state_formula>(atermpp::arg2(*this));
    }
};

/// \brief Test for a or expression
/// \param t A term
/// \return True if it is a or expression
inline
bool is_or(const state_formula& t)
{
  return core::detail::gsIsStateOr(t);
}


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
    imp(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateImp(*this));
    }

    /// \brief Constructor.
    imp(const state_formula& left, const state_formula& right)
      : state_formula(core::detail::gsMakeStateImp(left, right))
    {}

    const state_formula& left() const
    {
      return atermpp::aterm_cast<const state_formula>(atermpp::arg1(*this));
    }

    const state_formula& right() const
    {
      return atermpp::aterm_cast<const state_formula>(atermpp::arg2(*this));
    }
};

/// \brief Test for a imp expression
/// \param t A term
/// \return True if it is a imp expression
inline
bool is_imp(const state_formula& t)
{
  return core::detail::gsIsStateImp(t);
}


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
    forall(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateForall(*this));
    }

    /// \brief Constructor.
    forall(const data::variable_list& variables, const state_formula& body)
      : state_formula(core::detail::gsMakeStateForall(variables, body))
    {}

    const data::variable_list& variables() const
    {
      return atermpp::aterm_cast<const data::variable_list>(atermpp::list_arg1(*this));
    }

    const state_formula& body() const
    {
      return atermpp::aterm_cast<const state_formula>(atermpp::arg2(*this));
    }
};

/// \brief Test for a forall expression
/// \param t A term
/// \return True if it is a forall expression
inline
bool is_forall(const state_formula& t)
{
  return core::detail::gsIsStateForall(t);
}


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
    exists(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateExists(*this));
    }

    /// \brief Constructor.
    exists(const data::variable_list& variables, const state_formula& body)
      : state_formula(core::detail::gsMakeStateExists(variables, body))
    {}

    const data::variable_list& variables() const
    {
      return atermpp::aterm_cast<const data::variable_list>(atermpp::list_arg1(*this));
    }

    const state_formula& body() const
    {
      return atermpp::aterm_cast<const state_formula>(atermpp::arg2(*this));
    }
};

/// \brief Test for a exists expression
/// \param t A term
/// \return True if it is a exists expression
inline
bool is_exists(const state_formula& t)
{
  return core::detail::gsIsStateExists(t);
}


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
    must(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateMust(*this));
    }

    /// \brief Constructor.
    must(const regular_formulas::regular_formula& formula, const state_formula& operand)
      : state_formula(core::detail::gsMakeStateMust(formula, operand))
    {}

    const regular_formulas::regular_formula& formula() const
    {
      return atermpp::aterm_cast<const regular_formulas::regular_formula>(atermpp::arg1(*this));
    }

    const state_formula& operand() const
    {
      return atermpp::aterm_cast<const state_formula>(atermpp::arg2(*this));
    }
};

/// \brief Test for a must expression
/// \param t A term
/// \return True if it is a must expression
inline
bool is_must(const state_formula& t)
{
  return core::detail::gsIsStateMust(t);
}


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
    may(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateMay(*this));
    }

    /// \brief Constructor.
    may(const regular_formulas::regular_formula& formula, const state_formula& operand)
      : state_formula(core::detail::gsMakeStateMay(formula, operand))
    {}

    const regular_formulas::regular_formula& formula() const
    {
      return atermpp::aterm_cast<const regular_formulas::regular_formula>(atermpp::arg1(*this));
    }

    const state_formula& operand() const
    {
      return atermpp::aterm_cast<const state_formula>(atermpp::arg2(*this));
    }
};

/// \brief Test for a may expression
/// \param t A term
/// \return True if it is a may expression
inline
bool is_may(const state_formula& t)
{
  return core::detail::gsIsStateMay(t);
}


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
    yaled(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateYaled(*this));
    }
};

/// \brief Test for a yaled expression
/// \param t A term
/// \return True if it is a yaled expression
inline
bool is_yaled(const state_formula& t)
{
  return core::detail::gsIsStateYaled(t);
}


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
    yaled_timed(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateYaledTimed(*this));
    }

    /// \brief Constructor.
    yaled_timed(const data::data_expression& time_stamp)
      : state_formula(core::detail::gsMakeStateYaledTimed(time_stamp))
    {}

    const data::data_expression& time_stamp() const
    {
      return atermpp::aterm_cast<const data::data_expression>(atermpp::arg1(*this));
    }
};

/// \brief Test for a yaled_timed expression
/// \param t A term
/// \return True if it is a yaled_timed expression
inline
bool is_yaled_timed(const state_formula& t)
{
  return core::detail::gsIsStateYaledTimed(t);
}


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
    delay(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateDelay(*this));
    }
};

/// \brief Test for a delay expression
/// \param t A term
/// \return True if it is a delay expression
inline
bool is_delay(const state_formula& t)
{
  return core::detail::gsIsStateDelay(t);
}


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
    delay_timed(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateDelayTimed(*this));
    }

    /// \brief Constructor.
    delay_timed(const data::data_expression& time_stamp)
      : state_formula(core::detail::gsMakeStateDelayTimed(time_stamp))
    {}

    const data::data_expression& time_stamp() const
    {
      return atermpp::aterm_cast<const data::data_expression>(atermpp::arg1(*this));
    }
};

/// \brief Test for a delay_timed expression
/// \param t A term
/// \return True if it is a delay_timed expression
inline
bool is_delay_timed(const state_formula& t)
{
  return core::detail::gsIsStateDelayTimed(t);
}


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
    variable(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateVar(*this));
    }

    /// \brief Constructor.
    variable(const core::identifier_string& name, const data::data_expression_list& arguments)
      : state_formula(core::detail::gsMakeStateVar(name, arguments))
    {}

    /// \brief Constructor.
    variable(const std::string& name, const data::data_expression_list& arguments)
      : state_formula(core::detail::gsMakeStateVar(core::identifier_string(name), arguments))
    {}

    const core::identifier_string& name() const
    {
      return atermpp::aterm_cast<const core::identifier_string>(atermpp::arg1(*this));
    }

    const data::data_expression_list& arguments() const
    {
      return atermpp::aterm_cast<const data::data_expression_list>(atermpp::list_arg2(*this));
    }
};

/// \brief Test for a variable expression
/// \param t A term
/// \return True if it is a variable expression
inline
bool is_variable(const state_formula& t)
{
  return core::detail::gsIsStateVar(t);
}


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
    nu(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateNu(*this));
    }

    /// \brief Constructor.
    nu(const core::identifier_string& name, const data::assignment_list& assignments, const state_formula& operand)
      : state_formula(core::detail::gsMakeStateNu(name, assignments, operand))
    {}

    /// \brief Constructor.
    nu(const std::string& name, const data::assignment_list& assignments, const state_formula& operand)
      : state_formula(core::detail::gsMakeStateNu(core::identifier_string(name), assignments, operand))
    {}

    const core::identifier_string& name() const
    {
      return atermpp::aterm_cast<const core::identifier_string>(atermpp::arg1(*this));
    }

    const data::assignment_list& assignments() const
    {
      return atermpp::aterm_cast<const data::assignment_list>(atermpp::list_arg2(*this));
    }

    const state_formula& operand() const
    {
      return atermpp::aterm_cast<const state_formula>(atermpp::arg3(*this));
    }
};

/// \brief Test for a nu expression
/// \param t A term
/// \return True if it is a nu expression
inline
bool is_nu(const state_formula& t)
{
  return core::detail::gsIsStateNu(t);
}


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
    mu(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateMu(*this));
    }

    /// \brief Constructor.
    mu(const core::identifier_string& name, const data::assignment_list& assignments, const state_formula& operand)
      : state_formula(core::detail::gsMakeStateMu(name, assignments, operand))
    {}

    /// \brief Constructor.
    mu(const std::string& name, const data::assignment_list& assignments, const state_formula& operand)
      : state_formula(core::detail::gsMakeStateMu(core::identifier_string(name), assignments, operand))
    {}

    const core::identifier_string& name() const
    {
      return atermpp::aterm_cast<const core::identifier_string>(atermpp::arg1(*this));
    }

    const data::assignment_list& assignments() const
    {
      return atermpp::aterm_cast<const data::assignment_list>(atermpp::list_arg2(*this));
    }

    const state_formula& operand() const
    {
      return atermpp::aterm_cast<const state_formula>(atermpp::arg3(*this));
    }
};

/// \brief Test for a mu expression
/// \param t A term
/// \return True if it is a mu expression
inline
bool is_mu(const state_formula& t)
{
  return core::detail::gsIsStateMu(t);
}

//--- end generated classes ---//

inline
int precedence(const state_formula& x)
{
  if (is_mu(x) || is_nu(x))
  {
    return 1;
  }
  else if (is_forall(x) || is_exists(x))
  {
    return 2;
  }
  else if (is_imp(x))
  {
    return 3;
  }
  else if (is_or(x))
  {
    return 4;
  }
  else if (is_and(x))
  {
    return 5;
  }
  else if (is_must(x) || is_may(x))
  {
    return 6;
  }
  else if (is_not(x))
  {
    return 7;
  }
  return core::detail::precedences::max_precedence;
}

// TODO: is there a cleaner way to make the precedence function work for derived classes like and_ ?
inline int precedence(const mu& x) { return precedence(static_cast<const state_formula&>(x)); }
inline int precedence(const nu& x) { return precedence(static_cast<const state_formula&>(x)); }
inline int precedence(const forall& x) { return precedence(static_cast<const state_formula&>(x)); }
inline int precedence(const exists& x) { return precedence(static_cast<const state_formula&>(x)); }
inline int precedence(const imp& x) { return precedence(static_cast<const state_formula&>(x)); }
inline int precedence(const and_& x) { return precedence(static_cast<const state_formula&>(x)); }
inline int precedence(const or_& x) { return precedence(static_cast<const state_formula&>(x)); }
inline int precedence(const must& x) { return precedence(static_cast<const state_formula&>(x)); }
inline int precedence(const may& x) { return precedence(static_cast<const state_formula&>(x)); }
inline int precedence(const not_& x) { return precedence(static_cast<const state_formula&>(x)); }

} // namespace state_formulas

} // namespace mcrl2

#ifndef MCRL2_MODAL_FORMULA_TRAVERSER_H
#include "mcrl2/modal_formula/traverser.h"
#endif

namespace mcrl2
{

namespace state_formulas
{

/// \cond INTERNAL_DOCS
//
/// \brief Function that determines if a state formula is time dependent
// \brief Visitor for checking if a state formula is timed.
struct is_timed_traverser: public state_formula_traverser<is_timed_traverser>
{
  typedef state_formula_traverser<is_timed_traverser> super;
  using super::enter;
  using super::leave;
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  bool result;

  is_timed_traverser()
    : result(false)
  {}

  void enter(const delay_timed& /* x */)
  {
    result = true;
  }

  void enter(const yaled_timed& /* x */)
  {
    result = true;
  }

  void enter(const action_formulas::at& /* x */)
  {
    result = true;
  }
};
/// \endcond

/// \brief Checks if a state formula is timed
/// \param x A state formula
/// \return True if a state formula is timed
inline
bool is_timed(const state_formula& x)
{
  is_timed_traverser f;
  f(x);
  return f.result;
}

/// \brief Returns true if the formula is timed.
/// \return True if the formula is timed.
inline
bool state_formula::has_time() const
{
  return is_timed(*this);
}

// template function overloads
std::string pp(const state_formula& x);
state_formula normalize_sorts(const state_formula& x, const data::data_specification& dataspec);
state_formulas::state_formula translate_user_notation(const state_formulas::state_formula& x);
std::set<data::sort_expression> find_sort_expressions(const state_formulas::state_formula& x);
std::set<data::variable> find_variables(const state_formulas::state_formula& x);
std::set<data::variable> find_free_variables(const state_formulas::state_formula& x);
std::set<core::identifier_string> find_identifiers(const state_formulas::state_formula& x);
bool find_nil(const state_formulas::state_formula& x);

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_STATE_FORMULA_H
