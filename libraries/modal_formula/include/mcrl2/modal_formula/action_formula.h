// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/action_formula.h
/// \brief Add your file description here.

#ifndef MCRL2_MODAL_ACTION_FORMULA_H
#define MCRL2_MODAL_ACTION_FORMULA_H

#include <iostream> // for debugging
#include <stdexcept>
#include <string>
#include <cassert>
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/down_cast.h"
#include "mcrl2/core/detail/precedence.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/lps/action.h"
#include "mcrl2/lps/untyped_action.h"

namespace mcrl2
{

namespace action_formulas
{

//--- start generated classes ---//
/// \brief An action formula
class action_formula: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    action_formula()
      : atermpp::aterm_appl(core::detail::default_values::ActFrm)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit action_formula(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_ActFrm(*this));
    }

    /// \brief Constructor.
    action_formula(const data::data_expression& x)
      : atermpp::aterm_appl(x)
    {}
};

/// \brief list of action_formulas
typedef atermpp::term_list<action_formula> action_formula_list;

/// \brief vector of action_formulas
typedef std::vector<action_formula>    action_formula_vector;

// prototypes
inline bool is_true(const atermpp::aterm_appl& x);
inline bool is_false(const atermpp::aterm_appl& x);
inline bool is_not(const atermpp::aterm_appl& x);
inline bool is_and(const atermpp::aterm_appl& x);
inline bool is_or(const atermpp::aterm_appl& x);
inline bool is_imp(const atermpp::aterm_appl& x);
inline bool is_forall(const atermpp::aterm_appl& x);
inline bool is_exists(const atermpp::aterm_appl& x);
inline bool is_at(const atermpp::aterm_appl& x);
inline bool is_multi_action(const atermpp::aterm_appl& x);
inline bool is_untyped_multi_action(const atermpp::aterm_appl& x);

/// \brief Test for a action_formula expression
/// \param x A term
/// \return True if \a x is a action_formula expression
inline
bool is_action_formula(const atermpp::aterm_appl& x)
{
  return data::is_data_expression(x) ||
         action_formulas::is_true(x) ||
         action_formulas::is_false(x) ||
         action_formulas::is_not(x) ||
         action_formulas::is_and(x) ||
         action_formulas::is_or(x) ||
         action_formulas::is_imp(x) ||
         action_formulas::is_forall(x) ||
         action_formulas::is_exists(x) ||
         action_formulas::is_at(x) ||
         action_formulas::is_multi_action(x) ||
         action_formulas::is_untyped_multi_action(x);
}

// prototype declaration
std::string pp(const action_formula& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const action_formula& x)
{
  return out << action_formulas::pp(x);
}

/// \brief swap overload
inline void swap(action_formula& t1, action_formula& t2)
{
  t1.swap(t2);
}


/// \brief The value true for action formulas
class true_: public action_formula
{
  public:
    /// \brief Default constructor.
    true_()
      : action_formula(core::detail::default_values::ActTrue)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit true_(const atermpp::aterm& term)
      : action_formula(term)
    {
      assert(core::detail::check_term_ActTrue(*this));
    }
};

/// \brief Test for a true expression
/// \param x A term
/// \return True if \a x is a true expression
inline
bool is_true(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::ActTrue;
}

// prototype declaration
std::string pp(const true_& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const true_& x)
{
  return out << action_formulas::pp(x);
}

/// \brief swap overload
inline void swap(true_& t1, true_& t2)
{
  t1.swap(t2);
}


/// \brief The value false for action formulas
class false_: public action_formula
{
  public:
    /// \brief Default constructor.
    false_()
      : action_formula(core::detail::default_values::ActFalse)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit false_(const atermpp::aterm& term)
      : action_formula(term)
    {
      assert(core::detail::check_term_ActFalse(*this));
    }
};

/// \brief Test for a false expression
/// \param x A term
/// \return True if \a x is a false expression
inline
bool is_false(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::ActFalse;
}

// prototype declaration
std::string pp(const false_& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const false_& x)
{
  return out << action_formulas::pp(x);
}

/// \brief swap overload
inline void swap(false_& t1, false_& t2)
{
  t1.swap(t2);
}


/// \brief The not operator for action formulas
class not_: public action_formula
{
  public:
    /// \brief Default constructor.
    not_()
      : action_formula(core::detail::default_values::ActNot)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit not_(const atermpp::aterm& term)
      : action_formula(term)
    {
      assert(core::detail::check_term_ActNot(*this));
    }

    /// \brief Constructor.
    not_(const action_formula& operand)
      : action_formula(atermpp::aterm_appl(core::detail::function_symbol_ActNot(), operand))
    {}

    const action_formula& operand() const
    {
      return atermpp::aterm_cast<const action_formula>((*this)[0]);
    }
};

/// \brief Test for a not expression
/// \param x A term
/// \return True if \a x is a not expression
inline
bool is_not(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::ActNot;
}

// prototype declaration
std::string pp(const not_& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const not_& x)
{
  return out << action_formulas::pp(x);
}

/// \brief swap overload
inline void swap(not_& t1, not_& t2)
{
  t1.swap(t2);
}


/// \brief The and operator for action formulas
class and_: public action_formula
{
  public:
    /// \brief Default constructor.
    and_()
      : action_formula(core::detail::default_values::ActAnd)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit and_(const atermpp::aterm& term)
      : action_formula(term)
    {
      assert(core::detail::check_term_ActAnd(*this));
    }

    /// \brief Constructor.
    and_(const action_formula& left, const action_formula& right)
      : action_formula(atermpp::aterm_appl(core::detail::function_symbol_ActAnd(), left, right))
    {}

    const action_formula& left() const
    {
      return atermpp::aterm_cast<const action_formula>((*this)[0]);
    }

    const action_formula& right() const
    {
      return atermpp::aterm_cast<const action_formula>((*this)[1]);
    }
};

/// \brief Test for a and expression
/// \param x A term
/// \return True if \a x is a and expression
inline
bool is_and(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::ActAnd;
}

// prototype declaration
std::string pp(const and_& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const and_& x)
{
  return out << action_formulas::pp(x);
}

/// \brief swap overload
inline void swap(and_& t1, and_& t2)
{
  t1.swap(t2);
}


/// \brief The or operator for action formulas
class or_: public action_formula
{
  public:
    /// \brief Default constructor.
    or_()
      : action_formula(core::detail::default_values::ActOr)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit or_(const atermpp::aterm& term)
      : action_formula(term)
    {
      assert(core::detail::check_term_ActOr(*this));
    }

    /// \brief Constructor.
    or_(const action_formula& left, const action_formula& right)
      : action_formula(atermpp::aterm_appl(core::detail::function_symbol_ActOr(), left, right))
    {}

    const action_formula& left() const
    {
      return atermpp::aterm_cast<const action_formula>((*this)[0]);
    }

    const action_formula& right() const
    {
      return atermpp::aterm_cast<const action_formula>((*this)[1]);
    }
};

/// \brief Test for a or expression
/// \param x A term
/// \return True if \a x is a or expression
inline
bool is_or(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::ActOr;
}

// prototype declaration
std::string pp(const or_& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const or_& x)
{
  return out << action_formulas::pp(x);
}

/// \brief swap overload
inline void swap(or_& t1, or_& t2)
{
  t1.swap(t2);
}


/// \brief The implication operator for action formulas
class imp: public action_formula
{
  public:
    /// \brief Default constructor.
    imp()
      : action_formula(core::detail::default_values::ActImp)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit imp(const atermpp::aterm& term)
      : action_formula(term)
    {
      assert(core::detail::check_term_ActImp(*this));
    }

    /// \brief Constructor.
    imp(const action_formula& left, const action_formula& right)
      : action_formula(atermpp::aterm_appl(core::detail::function_symbol_ActImp(), left, right))
    {}

    const action_formula& left() const
    {
      return atermpp::aterm_cast<const action_formula>((*this)[0]);
    }

    const action_formula& right() const
    {
      return atermpp::aterm_cast<const action_formula>((*this)[1]);
    }
};

/// \brief Test for a imp expression
/// \param x A term
/// \return True if \a x is a imp expression
inline
bool is_imp(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::ActImp;
}

// prototype declaration
std::string pp(const imp& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const imp& x)
{
  return out << action_formulas::pp(x);
}

/// \brief swap overload
inline void swap(imp& t1, imp& t2)
{
  t1.swap(t2);
}


/// \brief The universal quantification operator for action formulas
class forall: public action_formula
{
  public:
    /// \brief Default constructor.
    forall()
      : action_formula(core::detail::default_values::ActForall)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit forall(const atermpp::aterm& term)
      : action_formula(term)
    {
      assert(core::detail::check_term_ActForall(*this));
    }

    /// \brief Constructor.
    forall(const data::variable_list& variables, const action_formula& body)
      : action_formula(atermpp::aterm_appl(core::detail::function_symbol_ActForall(), variables, body))
    {}

    const data::variable_list& variables() const
    {
      return atermpp::aterm_cast<const data::variable_list>((*this)[0]);
    }

    const action_formula& body() const
    {
      return atermpp::aterm_cast<const action_formula>((*this)[1]);
    }
};

/// \brief Test for a forall expression
/// \param x A term
/// \return True if \a x is a forall expression
inline
bool is_forall(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::ActForall;
}

// prototype declaration
std::string pp(const forall& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const forall& x)
{
  return out << action_formulas::pp(x);
}

/// \brief swap overload
inline void swap(forall& t1, forall& t2)
{
  t1.swap(t2);
}


/// \brief The existential quantification operator for action formulas
class exists: public action_formula
{
  public:
    /// \brief Default constructor.
    exists()
      : action_formula(core::detail::default_values::ActExists)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit exists(const atermpp::aterm& term)
      : action_formula(term)
    {
      assert(core::detail::check_term_ActExists(*this));
    }

    /// \brief Constructor.
    exists(const data::variable_list& variables, const action_formula& body)
      : action_formula(atermpp::aterm_appl(core::detail::function_symbol_ActExists(), variables, body))
    {}

    const data::variable_list& variables() const
    {
      return atermpp::aterm_cast<const data::variable_list>((*this)[0]);
    }

    const action_formula& body() const
    {
      return atermpp::aterm_cast<const action_formula>((*this)[1]);
    }
};

/// \brief Test for a exists expression
/// \param x A term
/// \return True if \a x is a exists expression
inline
bool is_exists(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::ActExists;
}

// prototype declaration
std::string pp(const exists& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const exists& x)
{
  return out << action_formulas::pp(x);
}

/// \brief swap overload
inline void swap(exists& t1, exists& t2)
{
  t1.swap(t2);
}


/// \brief The at operator for action formulas
class at: public action_formula
{
  public:
    /// \brief Default constructor.
    at()
      : action_formula(core::detail::default_values::ActAt)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit at(const atermpp::aterm& term)
      : action_formula(term)
    {
      assert(core::detail::check_term_ActAt(*this));
    }

    /// \brief Constructor.
    at(const action_formula& operand, const data::data_expression& time_stamp)
      : action_formula(atermpp::aterm_appl(core::detail::function_symbol_ActAt(), operand, time_stamp))
    {}

    const action_formula& operand() const
    {
      return atermpp::aterm_cast<const action_formula>((*this)[0]);
    }

    const data::data_expression& time_stamp() const
    {
      return atermpp::aterm_cast<const data::data_expression>((*this)[1]);
    }
};

/// \brief Test for a at expression
/// \param x A term
/// \return True if \a x is a at expression
inline
bool is_at(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::ActAt;
}

// prototype declaration
std::string pp(const at& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const at& x)
{
  return out << action_formulas::pp(x);
}

/// \brief swap overload
inline void swap(at& t1, at& t2)
{
  t1.swap(t2);
}


/// \brief The multi action for action formulas
class multi_action: public action_formula
{
  public:
    /// \brief Default constructor.
    multi_action()
      : action_formula(core::detail::default_values::ActMultAct)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit multi_action(const atermpp::aterm& term)
      : action_formula(term)
    {
      assert(core::detail::check_term_ActMultAct(*this));
    }

    /// \brief Constructor.
    multi_action(const lps::action_list& actions)
      : action_formula(atermpp::aterm_appl(core::detail::function_symbol_ActMultAct(), actions))
    {}

    const lps::action_list& actions() const
    {
      return atermpp::aterm_cast<const lps::action_list>((*this)[0]);
    }
};

/// \brief Test for a multi_action expression
/// \param x A term
/// \return True if \a x is a multi_action expression
inline
bool is_multi_action(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::ActMultAct;
}

// prototype declaration
std::string pp(const multi_action& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const multi_action& x)
{
  return out << action_formulas::pp(x);
}

/// \brief swap overload
inline void swap(multi_action& t1, multi_action& t2)
{
  t1.swap(t2);
}


/// \brief The multi action for action formulas (untyped)
class untyped_multi_action: public action_formula
{
  public:
    /// \brief Default constructor.
    untyped_multi_action()
      : action_formula(core::detail::default_values::UntypedActMultAct)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit untyped_multi_action(const atermpp::aterm& term)
      : action_formula(term)
    {
      assert(core::detail::check_term_UntypedActMultAct(*this));
    }

    /// \brief Constructor.
    untyped_multi_action(const lps::untyped_action_list& arguments)
      : action_formula(atermpp::aterm_appl(core::detail::function_symbol_UntypedActMultAct(), arguments))
    {}

    const lps::untyped_action_list& arguments() const
    {
      return atermpp::aterm_cast<const lps::untyped_action_list>((*this)[0]);
    }
};

/// \brief Test for a untyped_multi_action expression
/// \param x A term
/// \return True if \a x is a untyped_multi_action expression
inline
bool is_untyped_multi_action(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::UntypedActMultAct;
}

// prototype declaration
std::string pp(const untyped_multi_action& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const untyped_multi_action& x)
{
  return out << action_formulas::pp(x);
}

/// \brief swap overload
inline void swap(untyped_multi_action& t1, untyped_multi_action& t2)
{
  t1.swap(t2);
}
//--- end generated classes ---//

//template <typename T>
//int left_precedence(const T&)
//{
//  return core::detail::precedences::max_precedence;
//}
inline int left_precedence(const forall& x) { return 0; }
inline int left_precedence(const exists& x) { return 0; }
inline int left_precedence(const imp& x)    { return 2; }
inline int left_precedence(const or_& x)    { return 3; }
inline int left_precedence(const and_& x)   { return 4; }
inline int left_precedence(const at& x)     { return 5; }
inline int left_precedence(const not_& x)   { return 6; }
inline int left_precedence(const action_formula& x)
{
  if      (is_forall(x)) { return left_precedence(static_cast<const forall&>(x)); }
  else if (is_exists(x)) { return left_precedence(static_cast<const exists&>(x)); }
  else if (is_imp(x))    { return left_precedence(static_cast<const imp&>(x)); }
  else if (is_or(x))     { return left_precedence(static_cast<const or_&>(x)); }
  else if (is_and(x))    { return left_precedence(static_cast<const and_&>(x)); }
  else if (is_at(x))     { return left_precedence(static_cast<const at&>(x)); }
  else if (is_not(x))    { return left_precedence(static_cast<const not_&>(x)); }
  return core::detail::precedences::max_precedence;
}

inline int right_precedence(const forall& x) { return (std::max)(left_precedence(x), left_precedence(static_cast<const forall&>(x).body())); }
inline int right_precedence(const exists& x) { return (std::max)(left_precedence(x), left_precedence(static_cast<const exists&>(x).body())); }
inline int right_precedence(const action_formula& x)
{
  if      (is_forall(x)) { return right_precedence(static_cast<const forall&>(x)); }
  else if (is_exists(x)) { return right_precedence(static_cast<const exists&>(x)); }
  return left_precedence(x);
}

inline const action_formula& unary_operand(const not_& x) { return x.operand(); }
inline const action_formula& unary_operand(const at& x)   { return x.operand(); }
inline const action_formula& binary_left(const and_& x)   { return x.left(); }
inline const action_formula& binary_right(const and_& x)  { return x.right(); }
inline const action_formula& binary_left(const or_& x)    { return x.left(); }
inline const action_formula& binary_right(const or_& x)   { return x.right(); }
inline const action_formula& binary_left(const imp& x)    { return x.left(); }
inline const action_formula& binary_right(const imp& x)   { return x.right(); }

// template function overloads
std::set<data::variable> find_all_variables(const action_formulas::action_formula& x);

} // namespace action_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_ACTION_FORMULA_H
