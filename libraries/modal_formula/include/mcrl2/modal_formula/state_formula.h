// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/state_formula.h
/// \brief Add your file description here.

#ifndef MCRL2_MODAL_FORMULA_STATE_FORMULA_H
#define MCRL2_MODAL_FORMULA_STATE_FORMULA_H

#include "mcrl2/modal_formula/regular_formula.h"

namespace mcrl2
{

namespace state_formulas
{

//--- start generated classes ---//
/// \\brief A state formula
class state_formula: public atermpp::aterm_appl
{
  public:
    /// \\brief Default constructor.
    state_formula()
      : atermpp::aterm_appl(core::detail::default_values::StateFrm)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit state_formula(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_StateFrm(*this));
    }

    /// \\brief Constructor.
    state_formula(const data::data_expression& x)
      : atermpp::aterm_appl(x)
    {}

    /// \\brief Constructor.
    state_formula(const data::untyped_data_parameter& x)
      : atermpp::aterm_appl(x)
    {}

    /// Move semantics
    state_formula(const state_formula&) noexcept = default;
    state_formula(state_formula&&) noexcept = default;
    state_formula& operator=(const state_formula&) noexcept = default;
    state_formula& operator=(state_formula&&) noexcept = default;
//--- start user section state_formula ---//
    /// \brief Returns true if the formula is timed.
    /// \return True if the formula is timed.
    bool has_time() const;
//--- end user section state_formula ---//
};

/// \\brief list of state_formulas
typedef atermpp::term_list<state_formula> state_formula_list;

/// \\brief vector of state_formulas
typedef std::vector<state_formula>    state_formula_vector;

// prototypes
inline bool is_true(const atermpp::aterm_appl& x);
inline bool is_false(const atermpp::aterm_appl& x);
inline bool is_not(const atermpp::aterm_appl& x);
inline bool is_and(const atermpp::aterm_appl& x);
inline bool is_or(const atermpp::aterm_appl& x);
inline bool is_imp(const atermpp::aterm_appl& x);
inline bool is_forall(const atermpp::aterm_appl& x);
inline bool is_exists(const atermpp::aterm_appl& x);
inline bool is_must(const atermpp::aterm_appl& x);
inline bool is_may(const atermpp::aterm_appl& x);
inline bool is_yaled(const atermpp::aterm_appl& x);
inline bool is_yaled_timed(const atermpp::aterm_appl& x);
inline bool is_delay(const atermpp::aterm_appl& x);
inline bool is_delay_timed(const atermpp::aterm_appl& x);
inline bool is_variable(const atermpp::aterm_appl& x);
inline bool is_nu(const atermpp::aterm_appl& x);
inline bool is_mu(const atermpp::aterm_appl& x);

/// \\brief Test for a state_formula expression
/// \\param x A term
/// \\return True if \\a x is a state_formula expression
inline
bool is_state_formula(const atermpp::aterm_appl& x)
{
  return data::is_data_expression(x) ||
         data::is_untyped_data_parameter(x) ||
         state_formulas::is_true(x) ||
         state_formulas::is_false(x) ||
         state_formulas::is_not(x) ||
         state_formulas::is_and(x) ||
         state_formulas::is_or(x) ||
         state_formulas::is_imp(x) ||
         state_formulas::is_forall(x) ||
         state_formulas::is_exists(x) ||
         state_formulas::is_must(x) ||
         state_formulas::is_may(x) ||
         state_formulas::is_yaled(x) ||
         state_formulas::is_yaled_timed(x) ||
         state_formulas::is_delay(x) ||
         state_formulas::is_delay_timed(x) ||
         state_formulas::is_variable(x) ||
         state_formulas::is_nu(x) ||
         state_formulas::is_mu(x);
}

// prototype declaration
std::string pp(const state_formula& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const state_formula& x)
{
  return out << state_formulas::pp(x);
}

/// \\brief swap overload
inline void swap(state_formula& t1, state_formula& t2)
{
  t1.swap(t2);
}


/// \\brief The value true for state formulas
class true_: public state_formula
{
  public:
    /// \\brief Default constructor.
    true_()
      : state_formula(core::detail::default_values::StateTrue)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit true_(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateTrue(*this));
    }

    /// Move semantics
    true_(const true_&) noexcept = default;
    true_(true_&&) noexcept = default;
    true_& operator=(const true_&) noexcept = default;
    true_& operator=(true_&&) noexcept = default;
};

/// \\brief Test for a true expression
/// \\param x A term
/// \\return True if \\a x is a true expression
inline
bool is_true(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::StateTrue;
}

// prototype declaration
std::string pp(const true_& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const true_& x)
{
  return out << state_formulas::pp(x);
}

/// \\brief swap overload
inline void swap(true_& t1, true_& t2)
{
  t1.swap(t2);
}


/// \\brief The value false for state formulas
class false_: public state_formula
{
  public:
    /// \\brief Default constructor.
    false_()
      : state_formula(core::detail::default_values::StateFalse)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit false_(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateFalse(*this));
    }

    /// Move semantics
    false_(const false_&) noexcept = default;
    false_(false_&&) noexcept = default;
    false_& operator=(const false_&) noexcept = default;
    false_& operator=(false_&&) noexcept = default;
};

/// \\brief Test for a false expression
/// \\param x A term
/// \\return True if \\a x is a false expression
inline
bool is_false(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::StateFalse;
}

// prototype declaration
std::string pp(const false_& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const false_& x)
{
  return out << state_formulas::pp(x);
}

/// \\brief swap overload
inline void swap(false_& t1, false_& t2)
{
  t1.swap(t2);
}


/// \\brief The not operator for state formulas
class not_: public state_formula
{
  public:
    /// \\brief Default constructor.
    not_()
      : state_formula(core::detail::default_values::StateNot)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit not_(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateNot(*this));
    }

    /// \\brief Constructor.
    explicit not_(const state_formula& operand)
      : state_formula(atermpp::aterm_appl(core::detail::function_symbol_StateNot(), operand))
    {}

    /// Move semantics
    not_(const not_&) noexcept = default;
    not_(not_&&) noexcept = default;
    not_& operator=(const not_&) noexcept = default;
    not_& operator=(not_&&) noexcept = default;

    const state_formula& operand() const
    {
      return atermpp::down_cast<state_formula>((*this)[0]);
    }
};

/// \\brief Make_not_ constructs a new term into a given address.
/// \\ \param t The reference into which the new not_ is constructed. 
template <class... ARGUMENTS>
inline void make_not_(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_StateNot(), args...);
}

/// \\brief Test for a not expression
/// \\param x A term
/// \\return True if \\a x is a not expression
inline
bool is_not(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::StateNot;
}

// prototype declaration
std::string pp(const not_& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const not_& x)
{
  return out << state_formulas::pp(x);
}

/// \\brief swap overload
inline void swap(not_& t1, not_& t2)
{
  t1.swap(t2);
}


/// \\brief The and operator for state formulas
class and_: public state_formula
{
  public:
    /// \\brief Default constructor.
    and_()
      : state_formula(core::detail::default_values::StateAnd)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit and_(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateAnd(*this));
    }

    /// \\brief Constructor.
    and_(const state_formula& left, const state_formula& right)
      : state_formula(atermpp::aterm_appl(core::detail::function_symbol_StateAnd(), left, right))
    {}

    /// Move semantics
    and_(const and_&) noexcept = default;
    and_(and_&&) noexcept = default;
    and_& operator=(const and_&) noexcept = default;
    and_& operator=(and_&&) noexcept = default;

    const state_formula& left() const
    {
      return atermpp::down_cast<state_formula>((*this)[0]);
    }

    const state_formula& right() const
    {
      return atermpp::down_cast<state_formula>((*this)[1]);
    }
};

/// \\brief Make_and_ constructs a new term into a given address.
/// \\ \param t The reference into which the new and_ is constructed. 
template <class... ARGUMENTS>
inline void make_and_(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_StateAnd(), args...);
}

/// \\brief Test for a and expression
/// \\param x A term
/// \\return True if \\a x is a and expression
inline
bool is_and(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::StateAnd;
}

// prototype declaration
std::string pp(const and_& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const and_& x)
{
  return out << state_formulas::pp(x);
}

/// \\brief swap overload
inline void swap(and_& t1, and_& t2)
{
  t1.swap(t2);
}


/// \\brief The or operator for state formulas
class or_: public state_formula
{
  public:
    /// \\brief Default constructor.
    or_()
      : state_formula(core::detail::default_values::StateOr)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit or_(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateOr(*this));
    }

    /// \\brief Constructor.
    or_(const state_formula& left, const state_formula& right)
      : state_formula(atermpp::aterm_appl(core::detail::function_symbol_StateOr(), left, right))
    {}

    /// Move semantics
    or_(const or_&) noexcept = default;
    or_(or_&&) noexcept = default;
    or_& operator=(const or_&) noexcept = default;
    or_& operator=(or_&&) noexcept = default;

    const state_formula& left() const
    {
      return atermpp::down_cast<state_formula>((*this)[0]);
    }

    const state_formula& right() const
    {
      return atermpp::down_cast<state_formula>((*this)[1]);
    }
};

/// \\brief Make_or_ constructs a new term into a given address.
/// \\ \param t The reference into which the new or_ is constructed. 
template <class... ARGUMENTS>
inline void make_or_(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_StateOr(), args...);
}

/// \\brief Test for a or expression
/// \\param x A term
/// \\return True if \\a x is a or expression
inline
bool is_or(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::StateOr;
}

// prototype declaration
std::string pp(const or_& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const or_& x)
{
  return out << state_formulas::pp(x);
}

/// \\brief swap overload
inline void swap(or_& t1, or_& t2)
{
  t1.swap(t2);
}


/// \\brief The implication operator for state formulas
class imp: public state_formula
{
  public:
    /// \\brief Default constructor.
    imp()
      : state_formula(core::detail::default_values::StateImp)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit imp(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateImp(*this));
    }

    /// \\brief Constructor.
    imp(const state_formula& left, const state_formula& right)
      : state_formula(atermpp::aterm_appl(core::detail::function_symbol_StateImp(), left, right))
    {}

    /// Move semantics
    imp(const imp&) noexcept = default;
    imp(imp&&) noexcept = default;
    imp& operator=(const imp&) noexcept = default;
    imp& operator=(imp&&) noexcept = default;

    const state_formula& left() const
    {
      return atermpp::down_cast<state_formula>((*this)[0]);
    }

    const state_formula& right() const
    {
      return atermpp::down_cast<state_formula>((*this)[1]);
    }
};

/// \\brief Make_imp constructs a new term into a given address.
/// \\ \param t The reference into which the new imp is constructed. 
template <class... ARGUMENTS>
inline void make_imp(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_StateImp(), args...);
}

/// \\brief Test for a imp expression
/// \\param x A term
/// \\return True if \\a x is a imp expression
inline
bool is_imp(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::StateImp;
}

// prototype declaration
std::string pp(const imp& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const imp& x)
{
  return out << state_formulas::pp(x);
}

/// \\brief swap overload
inline void swap(imp& t1, imp& t2)
{
  t1.swap(t2);
}


/// \\brief The universal quantification operator for state formulas
class forall: public state_formula
{
  public:
    /// \\brief Default constructor.
    forall()
      : state_formula(core::detail::default_values::StateForall)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit forall(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateForall(*this));
    }

    /// \\brief Constructor.
    forall(const data::variable_list& variables, const state_formula& body)
      : state_formula(atermpp::aterm_appl(core::detail::function_symbol_StateForall(), variables, body))
    {}

    /// Move semantics
    forall(const forall&) noexcept = default;
    forall(forall&&) noexcept = default;
    forall& operator=(const forall&) noexcept = default;
    forall& operator=(forall&&) noexcept = default;

    const data::variable_list& variables() const
    {
      return atermpp::down_cast<data::variable_list>((*this)[0]);
    }

    const state_formula& body() const
    {
      return atermpp::down_cast<state_formula>((*this)[1]);
    }
};

/// \\brief Make_forall constructs a new term into a given address.
/// \\ \param t The reference into which the new forall is constructed. 
template <class... ARGUMENTS>
inline void make_forall(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_StateForall(), args...);
}

/// \\brief Test for a forall expression
/// \\param x A term
/// \\return True if \\a x is a forall expression
inline
bool is_forall(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::StateForall;
}

// prototype declaration
std::string pp(const forall& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const forall& x)
{
  return out << state_formulas::pp(x);
}

/// \\brief swap overload
inline void swap(forall& t1, forall& t2)
{
  t1.swap(t2);
}


/// \\brief The existential quantification operator for state formulas
class exists: public state_formula
{
  public:
    /// \\brief Default constructor.
    exists()
      : state_formula(core::detail::default_values::StateExists)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit exists(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateExists(*this));
    }

    /// \\brief Constructor.
    exists(const data::variable_list& variables, const state_formula& body)
      : state_formula(atermpp::aterm_appl(core::detail::function_symbol_StateExists(), variables, body))
    {}

    /// Move semantics
    exists(const exists&) noexcept = default;
    exists(exists&&) noexcept = default;
    exists& operator=(const exists&) noexcept = default;
    exists& operator=(exists&&) noexcept = default;

    const data::variable_list& variables() const
    {
      return atermpp::down_cast<data::variable_list>((*this)[0]);
    }

    const state_formula& body() const
    {
      return atermpp::down_cast<state_formula>((*this)[1]);
    }
};

/// \\brief Make_exists constructs a new term into a given address.
/// \\ \param t The reference into which the new exists is constructed. 
template <class... ARGUMENTS>
inline void make_exists(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_StateExists(), args...);
}

/// \\brief Test for a exists expression
/// \\param x A term
/// \\return True if \\a x is a exists expression
inline
bool is_exists(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::StateExists;
}

// prototype declaration
std::string pp(const exists& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const exists& x)
{
  return out << state_formulas::pp(x);
}

/// \\brief swap overload
inline void swap(exists& t1, exists& t2)
{
  t1.swap(t2);
}


/// \\brief The must operator for state formulas
class must: public state_formula
{
  public:
    /// \\brief Default constructor.
    must()
      : state_formula(core::detail::default_values::StateMust)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit must(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateMust(*this));
    }

    /// \\brief Constructor.
    must(const regular_formulas::regular_formula& formula, const state_formula& operand)
      : state_formula(atermpp::aterm_appl(core::detail::function_symbol_StateMust(), formula, operand))
    {}

    /// Move semantics
    must(const must&) noexcept = default;
    must(must&&) noexcept = default;
    must& operator=(const must&) noexcept = default;
    must& operator=(must&&) noexcept = default;

    const regular_formulas::regular_formula& formula() const
    {
      return atermpp::down_cast<regular_formulas::regular_formula>((*this)[0]);
    }

    const state_formula& operand() const
    {
      return atermpp::down_cast<state_formula>((*this)[1]);
    }
};

/// \\brief Make_must constructs a new term into a given address.
/// \\ \param t The reference into which the new must is constructed. 
template <class... ARGUMENTS>
inline void make_must(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_StateMust(), args...);
}

/// \\brief Test for a must expression
/// \\param x A term
/// \\return True if \\a x is a must expression
inline
bool is_must(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::StateMust;
}

// prototype declaration
std::string pp(const must& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const must& x)
{
  return out << state_formulas::pp(x);
}

/// \\brief swap overload
inline void swap(must& t1, must& t2)
{
  t1.swap(t2);
}


/// \\brief The may operator for state formulas
class may: public state_formula
{
  public:
    /// \\brief Default constructor.
    may()
      : state_formula(core::detail::default_values::StateMay)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit may(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateMay(*this));
    }

    /// \\brief Constructor.
    may(const regular_formulas::regular_formula& formula, const state_formula& operand)
      : state_formula(atermpp::aterm_appl(core::detail::function_symbol_StateMay(), formula, operand))
    {}

    /// Move semantics
    may(const may&) noexcept = default;
    may(may&&) noexcept = default;
    may& operator=(const may&) noexcept = default;
    may& operator=(may&&) noexcept = default;

    const regular_formulas::regular_formula& formula() const
    {
      return atermpp::down_cast<regular_formulas::regular_formula>((*this)[0]);
    }

    const state_formula& operand() const
    {
      return atermpp::down_cast<state_formula>((*this)[1]);
    }
};

/// \\brief Make_may constructs a new term into a given address.
/// \\ \param t The reference into which the new may is constructed. 
template <class... ARGUMENTS>
inline void make_may(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_StateMay(), args...);
}

/// \\brief Test for a may expression
/// \\param x A term
/// \\return True if \\a x is a may expression
inline
bool is_may(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::StateMay;
}

// prototype declaration
std::string pp(const may& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const may& x)
{
  return out << state_formulas::pp(x);
}

/// \\brief swap overload
inline void swap(may& t1, may& t2)
{
  t1.swap(t2);
}


/// \\brief The yaled operator for state formulas
class yaled: public state_formula
{
  public:
    /// \\brief Default constructor.
    yaled()
      : state_formula(core::detail::default_values::StateYaled)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit yaled(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateYaled(*this));
    }

    /// Move semantics
    yaled(const yaled&) noexcept = default;
    yaled(yaled&&) noexcept = default;
    yaled& operator=(const yaled&) noexcept = default;
    yaled& operator=(yaled&&) noexcept = default;
};

/// \\brief Test for a yaled expression
/// \\param x A term
/// \\return True if \\a x is a yaled expression
inline
bool is_yaled(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::StateYaled;
}

// prototype declaration
std::string pp(const yaled& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const yaled& x)
{
  return out << state_formulas::pp(x);
}

/// \\brief swap overload
inline void swap(yaled& t1, yaled& t2)
{
  t1.swap(t2);
}


/// \\brief The timed yaled operator for state formulas
class yaled_timed: public state_formula
{
  public:
    /// \\brief Default constructor.
    yaled_timed()
      : state_formula(core::detail::default_values::StateYaledTimed)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit yaled_timed(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateYaledTimed(*this));
    }

    /// \\brief Constructor.
    explicit yaled_timed(const data::data_expression& time_stamp)
      : state_formula(atermpp::aterm_appl(core::detail::function_symbol_StateYaledTimed(), time_stamp))
    {}

    /// Move semantics
    yaled_timed(const yaled_timed&) noexcept = default;
    yaled_timed(yaled_timed&&) noexcept = default;
    yaled_timed& operator=(const yaled_timed&) noexcept = default;
    yaled_timed& operator=(yaled_timed&&) noexcept = default;

    const data::data_expression& time_stamp() const
    {
      return atermpp::down_cast<data::data_expression>((*this)[0]);
    }
};

/// \\brief Make_yaled_timed constructs a new term into a given address.
/// \\ \param t The reference into which the new yaled_timed is constructed. 
template <class... ARGUMENTS>
inline void make_yaled_timed(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_StateYaledTimed(), args...);
}

/// \\brief Test for a yaled_timed expression
/// \\param x A term
/// \\return True if \\a x is a yaled_timed expression
inline
bool is_yaled_timed(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::StateYaledTimed;
}

// prototype declaration
std::string pp(const yaled_timed& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const yaled_timed& x)
{
  return out << state_formulas::pp(x);
}

/// \\brief swap overload
inline void swap(yaled_timed& t1, yaled_timed& t2)
{
  t1.swap(t2);
}


/// \\brief The delay operator for state formulas
class delay: public state_formula
{
  public:
    /// \\brief Default constructor.
    delay()
      : state_formula(core::detail::default_values::StateDelay)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit delay(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateDelay(*this));
    }

    /// Move semantics
    delay(const delay&) noexcept = default;
    delay(delay&&) noexcept = default;
    delay& operator=(const delay&) noexcept = default;
    delay& operator=(delay&&) noexcept = default;
};

/// \\brief Test for a delay expression
/// \\param x A term
/// \\return True if \\a x is a delay expression
inline
bool is_delay(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::StateDelay;
}

// prototype declaration
std::string pp(const delay& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const delay& x)
{
  return out << state_formulas::pp(x);
}

/// \\brief swap overload
inline void swap(delay& t1, delay& t2)
{
  t1.swap(t2);
}


/// \\brief The timed delay operator for state formulas
class delay_timed: public state_formula
{
  public:
    /// \\brief Default constructor.
    delay_timed()
      : state_formula(core::detail::default_values::StateDelayTimed)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit delay_timed(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateDelayTimed(*this));
    }

    /// \\brief Constructor.
    explicit delay_timed(const data::data_expression& time_stamp)
      : state_formula(atermpp::aterm_appl(core::detail::function_symbol_StateDelayTimed(), time_stamp))
    {}

    /// Move semantics
    delay_timed(const delay_timed&) noexcept = default;
    delay_timed(delay_timed&&) noexcept = default;
    delay_timed& operator=(const delay_timed&) noexcept = default;
    delay_timed& operator=(delay_timed&&) noexcept = default;

    const data::data_expression& time_stamp() const
    {
      return atermpp::down_cast<data::data_expression>((*this)[0]);
    }
};

/// \\brief Make_delay_timed constructs a new term into a given address.
/// \\ \param t The reference into which the new delay_timed is constructed. 
template <class... ARGUMENTS>
inline void make_delay_timed(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_StateDelayTimed(), args...);
}

/// \\brief Test for a delay_timed expression
/// \\param x A term
/// \\return True if \\a x is a delay_timed expression
inline
bool is_delay_timed(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::StateDelayTimed;
}

// prototype declaration
std::string pp(const delay_timed& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const delay_timed& x)
{
  return out << state_formulas::pp(x);
}

/// \\brief swap overload
inline void swap(delay_timed& t1, delay_timed& t2)
{
  t1.swap(t2);
}


/// \\brief The state formula variable
class variable: public state_formula
{
  public:
    /// \\brief Default constructor.
    variable()
      : state_formula(core::detail::default_values::StateVar)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit variable(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateVar(*this));
    }

    /// \\brief Constructor.
    variable(const core::identifier_string& name, const data::data_expression_list& arguments)
      : state_formula(atermpp::aterm_appl(core::detail::function_symbol_StateVar(), name, arguments))
    {}

    /// \\brief Constructor.
    variable(const std::string& name, const data::data_expression_list& arguments)
      : state_formula(atermpp::aterm_appl(core::detail::function_symbol_StateVar(), core::identifier_string(name), arguments))
    {}

    /// Move semantics
    variable(const variable&) noexcept = default;
    variable(variable&&) noexcept = default;
    variable& operator=(const variable&) noexcept = default;
    variable& operator=(variable&&) noexcept = default;

    const core::identifier_string& name() const
    {
      return atermpp::down_cast<core::identifier_string>((*this)[0]);
    }

    const data::data_expression_list& arguments() const
    {
      return atermpp::down_cast<data::data_expression_list>((*this)[1]);
    }
};

/// \\brief Make_variable constructs a new term into a given address.
/// \\ \param t The reference into which the new variable is constructed. 
template <class... ARGUMENTS>
inline void make_variable(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_StateVar(), args...);
}

/// \\brief Test for a variable expression
/// \\param x A term
/// \\return True if \\a x is a variable expression
inline
bool is_variable(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::StateVar;
}

// prototype declaration
std::string pp(const variable& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const variable& x)
{
  return out << state_formulas::pp(x);
}

/// \\brief swap overload
inline void swap(variable& t1, variable& t2)
{
  t1.swap(t2);
}


/// \\brief The nu operator for state formulas
class nu: public state_formula
{
  public:
    /// \\brief Default constructor.
    nu()
      : state_formula(core::detail::default_values::StateNu)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit nu(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateNu(*this));
    }

    /// \\brief Constructor.
    nu(const core::identifier_string& name, const data::assignment_list& assignments, const state_formula& operand)
      : state_formula(atermpp::aterm_appl(core::detail::function_symbol_StateNu(), name, assignments, operand))
    {}

    /// \\brief Constructor.
    nu(const std::string& name, const data::assignment_list& assignments, const state_formula& operand)
      : state_formula(atermpp::aterm_appl(core::detail::function_symbol_StateNu(), core::identifier_string(name), assignments, operand))
    {}

    /// Move semantics
    nu(const nu&) noexcept = default;
    nu(nu&&) noexcept = default;
    nu& operator=(const nu&) noexcept = default;
    nu& operator=(nu&&) noexcept = default;

    const core::identifier_string& name() const
    {
      return atermpp::down_cast<core::identifier_string>((*this)[0]);
    }

    const data::assignment_list& assignments() const
    {
      return atermpp::down_cast<data::assignment_list>((*this)[1]);
    }

    const state_formula& operand() const
    {
      return atermpp::down_cast<state_formula>((*this)[2]);
    }
};

/// \\brief Make_nu constructs a new term into a given address.
/// \\ \param t The reference into which the new nu is constructed. 
template <class... ARGUMENTS>
inline void make_nu(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_StateNu(), args...);
}

/// \\brief Test for a nu expression
/// \\param x A term
/// \\return True if \\a x is a nu expression
inline
bool is_nu(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::StateNu;
}

// prototype declaration
std::string pp(const nu& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const nu& x)
{
  return out << state_formulas::pp(x);
}

/// \\brief swap overload
inline void swap(nu& t1, nu& t2)
{
  t1.swap(t2);
}


/// \\brief The mu operator for state formulas
class mu: public state_formula
{
  public:
    /// \\brief Default constructor.
    mu()
      : state_formula(core::detail::default_values::StateMu)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit mu(const atermpp::aterm& term)
      : state_formula(term)
    {
      assert(core::detail::check_term_StateMu(*this));
    }

    /// \\brief Constructor.
    mu(const core::identifier_string& name, const data::assignment_list& assignments, const state_formula& operand)
      : state_formula(atermpp::aterm_appl(core::detail::function_symbol_StateMu(), name, assignments, operand))
    {}

    /// \\brief Constructor.
    mu(const std::string& name, const data::assignment_list& assignments, const state_formula& operand)
      : state_formula(atermpp::aterm_appl(core::detail::function_symbol_StateMu(), core::identifier_string(name), assignments, operand))
    {}

    /// Move semantics
    mu(const mu&) noexcept = default;
    mu(mu&&) noexcept = default;
    mu& operator=(const mu&) noexcept = default;
    mu& operator=(mu&&) noexcept = default;

    const core::identifier_string& name() const
    {
      return atermpp::down_cast<core::identifier_string>((*this)[0]);
    }

    const data::assignment_list& assignments() const
    {
      return atermpp::down_cast<data::assignment_list>((*this)[1]);
    }

    const state_formula& operand() const
    {
      return atermpp::down_cast<state_formula>((*this)[2]);
    }
};

/// \\brief Make_mu constructs a new term into a given address.
/// \\ \param t The reference into which the new mu is constructed. 
template <class... ARGUMENTS>
inline void make_mu(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_StateMu(), args...);
}

/// \\brief Test for a mu expression
/// \\param x A term
/// \\return True if \\a x is a mu expression
inline
bool is_mu(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::StateMu;
}

// prototype declaration
std::string pp(const mu& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const mu& x)
{
  return out << state_formulas::pp(x);
}

/// \\brief swap overload
inline void swap(mu& t1, mu& t2)
{
  t1.swap(t2);
}
//--- end generated classes ---//

namespace algorithms {
    bool is_timed(const state_formula& x);
} // namespace algorithms

/// \brief Returns true if the formula is timed.
/// \return True if the formula is timed.
inline
bool state_formula::has_time() const
{
  return algorithms::is_timed(*this);
}

// template function overloads
state_formula normalize_sorts(const state_formula& x, const data::sort_specification& sortspec);
state_formulas::state_formula translate_user_notation(const state_formulas::state_formula& x);
std::set<data::sort_expression> find_sort_expressions(const state_formulas::state_formula& x);
std::set<data::variable> find_all_variables(const state_formulas::state_formula& x);
std::set<data::variable> find_free_variables(const state_formulas::state_formula& x);
std::set<core::identifier_string> find_identifiers(const state_formulas::state_formula& x);
std::set<process::action_label> find_action_labels(const state_formulas::state_formula& x);
bool find_nil(const state_formulas::state_formula& x);

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_STATE_FORMULA_H
