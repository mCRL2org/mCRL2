// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/boolean_expression.h
/// \brief add your file description here.

#ifndef MCRL2_BES_BOOLEAN_EXPRESSION_H
#define MCRL2_BES_BOOLEAN_EXPRESSION_H

#include "mcrl2/core/detail/default_values.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/term_traits.h"

namespace mcrl2
{

namespace bes
{

typedef core::identifier_string boolean_variable_key_type;

template <typename T> std::string pp(const T& x);

//--- start generated classes ---//
/// \brief A boolean expression
class boolean_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    boolean_expression()
      : atermpp::aterm_appl(core::detail::default_values::BooleanExpression)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit boolean_expression(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_BooleanExpression(*this));
    }

    /// Move semantics
    boolean_expression(const boolean_expression&) noexcept = default;
    boolean_expression(boolean_expression&&) noexcept = default;
    boolean_expression& operator=(const boolean_expression&) noexcept = default;
    boolean_expression& operator=(boolean_expression&&) noexcept = default;
};

/// \brief list of boolean_expressions
typedef atermpp::term_list<boolean_expression> boolean_expression_list;

/// \brief vector of boolean_expressions
typedef std::vector<boolean_expression>    boolean_expression_vector;

// prototypes
inline bool is_true(const atermpp::aterm_appl& x);
inline bool is_false(const atermpp::aterm_appl& x);
inline bool is_not(const atermpp::aterm_appl& x);
inline bool is_and(const atermpp::aterm_appl& x);
inline bool is_or(const atermpp::aterm_appl& x);
inline bool is_imp(const atermpp::aterm_appl& x);
inline bool is_boolean_variable(const atermpp::aterm_appl& x);

/// \brief Test for a boolean_expression expression
/// \param x A term
/// \return True if \a x is a boolean_expression expression
inline
bool is_boolean_expression(const atermpp::aterm_appl& x)
{
  return bes::is_true(x) ||
         bes::is_false(x) ||
         bes::is_not(x) ||
         bes::is_and(x) ||
         bes::is_or(x) ||
         bes::is_imp(x) ||
         bes::is_boolean_variable(x);
}

// prototype declaration
std::string pp(const boolean_expression& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \param x Object x
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const boolean_expression& x)
{
  return out << bes::pp(x);
}

/// \brief swap overload
inline void swap(boolean_expression& t1, boolean_expression& t2)
{
  t1.swap(t2);
}


/// \brief The value true for boolean expressions
class true_: public boolean_expression
{
  public:
    /// \brief Default constructor.
    true_()
      : boolean_expression(core::detail::default_values::BooleanTrue)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit true_(const atermpp::aterm& term)
      : boolean_expression(term)
    {
      assert(core::detail::check_term_BooleanTrue(*this));
    }

    /// Move semantics
    true_(const true_&) noexcept = default;
    true_(true_&&) noexcept = default;
    true_& operator=(const true_&) noexcept = default;
    true_& operator=(true_&&) noexcept = default;
};

/// \brief Test for a true expression
/// \param x A term
/// \return True if \a x is a true expression
inline
bool is_true(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::BooleanTrue;
}

// prototype declaration
std::string pp(const true_& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \param x Object x
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const true_& x)
{
  return out << bes::pp(x);
}

/// \brief swap overload
inline void swap(true_& t1, true_& t2)
{
  t1.swap(t2);
}


/// \brief The value false for boolean expressions
class false_: public boolean_expression
{
  public:
    /// \brief Default constructor.
    false_()
      : boolean_expression(core::detail::default_values::BooleanFalse)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit false_(const atermpp::aterm& term)
      : boolean_expression(term)
    {
      assert(core::detail::check_term_BooleanFalse(*this));
    }

    /// Move semantics
    false_(const false_&) noexcept = default;
    false_(false_&&) noexcept = default;
    false_& operator=(const false_&) noexcept = default;
    false_& operator=(false_&&) noexcept = default;
};

/// \brief Test for a false expression
/// \param x A term
/// \return True if \a x is a false expression
inline
bool is_false(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::BooleanFalse;
}

// prototype declaration
std::string pp(const false_& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \param x Object x
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const false_& x)
{
  return out << bes::pp(x);
}

/// \brief swap overload
inline void swap(false_& t1, false_& t2)
{
  t1.swap(t2);
}


/// \brief The not operator for boolean expressions
class not_: public boolean_expression
{
  public:
    /// \brief Default constructor.
    not_()
      : boolean_expression(core::detail::default_values::BooleanNot)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit not_(const atermpp::aterm& term)
      : boolean_expression(term)
    {
      assert(core::detail::check_term_BooleanNot(*this));
    }

    /// \brief Constructor.
    explicit not_(const boolean_expression& operand)
      : boolean_expression(atermpp::aterm_appl(core::detail::function_symbol_BooleanNot(), operand))
    {}

    /// Move semantics
    not_(const not_&) noexcept = default;
    not_(not_&&) noexcept = default;
    not_& operator=(const not_&) noexcept = default;
    not_& operator=(not_&&) noexcept = default;

    const boolean_expression& operand() const
    {
      return atermpp::down_cast<boolean_expression>((*this)[0]);
    }
};

/// \brief Make_not_ constructs a new term into a given address.
/// \ \param t The reference into which the new not_ is constructed. 
template <class... ARGUMENTS>
inline void make_not_(atermpp::aterm_appl& t, ARGUMENTS... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_BooleanNot(), args...);
}

/// \brief Test for a not expression
/// \param x A term
/// \return True if \a x is a not expression
inline
bool is_not(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::BooleanNot;
}

// prototype declaration
std::string pp(const not_& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \param x Object x
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const not_& x)
{
  return out << bes::pp(x);
}

/// \brief swap overload
inline void swap(not_& t1, not_& t2)
{
  t1.swap(t2);
}


/// \brief The and operator for boolean expressions
class and_: public boolean_expression
{
  public:
    /// \brief Default constructor.
    and_()
      : boolean_expression(core::detail::default_values::BooleanAnd)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit and_(const atermpp::aterm& term)
      : boolean_expression(term)
    {
      assert(core::detail::check_term_BooleanAnd(*this));
    }

    /// \brief Constructor.
    and_(const boolean_expression& left, const boolean_expression& right)
      : boolean_expression(atermpp::aterm_appl(core::detail::function_symbol_BooleanAnd(), left, right))
    {}

    /// Move semantics
    and_(const and_&) noexcept = default;
    and_(and_&&) noexcept = default;
    and_& operator=(const and_&) noexcept = default;
    and_& operator=(and_&&) noexcept = default;

    const boolean_expression& left() const
    {
      return atermpp::down_cast<boolean_expression>((*this)[0]);
    }

    const boolean_expression& right() const
    {
      return atermpp::down_cast<boolean_expression>((*this)[1]);
    }
};

/// \brief Make_and_ constructs a new term into a given address.
/// \ \param t The reference into which the new and_ is constructed. 
template <class... ARGUMENTS>
inline void make_and_(atermpp::aterm_appl& t, ARGUMENTS... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_BooleanAnd(), args...);
}

/// \brief Test for a and expression
/// \param x A term
/// \return True if \a x is a and expression
inline
bool is_and(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::BooleanAnd;
}

// prototype declaration
std::string pp(const and_& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \param x Object x
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const and_& x)
{
  return out << bes::pp(x);
}

/// \brief swap overload
inline void swap(and_& t1, and_& t2)
{
  t1.swap(t2);
}


/// \brief The or operator for boolean expressions
class or_: public boolean_expression
{
  public:
    /// \brief Default constructor.
    or_()
      : boolean_expression(core::detail::default_values::BooleanOr)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit or_(const atermpp::aterm& term)
      : boolean_expression(term)
    {
      assert(core::detail::check_term_BooleanOr(*this));
    }

    /// \brief Constructor.
    or_(const boolean_expression& left, const boolean_expression& right)
      : boolean_expression(atermpp::aterm_appl(core::detail::function_symbol_BooleanOr(), left, right))
    {}

    /// Move semantics
    or_(const or_&) noexcept = default;
    or_(or_&&) noexcept = default;
    or_& operator=(const or_&) noexcept = default;
    or_& operator=(or_&&) noexcept = default;

    const boolean_expression& left() const
    {
      return atermpp::down_cast<boolean_expression>((*this)[0]);
    }

    const boolean_expression& right() const
    {
      return atermpp::down_cast<boolean_expression>((*this)[1]);
    }
};

/// \brief Make_or_ constructs a new term into a given address.
/// \ \param t The reference into which the new or_ is constructed. 
template <class... ARGUMENTS>
inline void make_or_(atermpp::aterm_appl& t, ARGUMENTS... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_BooleanOr(), args...);
}

/// \brief Test for a or expression
/// \param x A term
/// \return True if \a x is a or expression
inline
bool is_or(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::BooleanOr;
}

// prototype declaration
std::string pp(const or_& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \param x Object x
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const or_& x)
{
  return out << bes::pp(x);
}

/// \brief swap overload
inline void swap(or_& t1, or_& t2)
{
  t1.swap(t2);
}


/// \brief The implication operator for boolean expressions
class imp: public boolean_expression
{
  public:
    /// \brief Default constructor.
    imp()
      : boolean_expression(core::detail::default_values::BooleanImp)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit imp(const atermpp::aterm& term)
      : boolean_expression(term)
    {
      assert(core::detail::check_term_BooleanImp(*this));
    }

    /// \brief Constructor.
    imp(const boolean_expression& left, const boolean_expression& right)
      : boolean_expression(atermpp::aterm_appl(core::detail::function_symbol_BooleanImp(), left, right))
    {}

    /// Move semantics
    imp(const imp&) noexcept = default;
    imp(imp&&) noexcept = default;
    imp& operator=(const imp&) noexcept = default;
    imp& operator=(imp&&) noexcept = default;

    const boolean_expression& left() const
    {
      return atermpp::down_cast<boolean_expression>((*this)[0]);
    }

    const boolean_expression& right() const
    {
      return atermpp::down_cast<boolean_expression>((*this)[1]);
    }
};

/// \brief Make_imp constructs a new term into a given address.
/// \ \param t The reference into which the new imp is constructed. 
template <class... ARGUMENTS>
inline void make_imp(atermpp::aterm_appl& t, ARGUMENTS... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_BooleanImp(), args...);
}

/// \brief Test for a imp expression
/// \param x A term
/// \return True if \a x is a imp expression
inline
bool is_imp(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::BooleanImp;
}

// prototype declaration
std::string pp(const imp& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \param x Object x
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const imp& x)
{
  return out << bes::pp(x);
}

/// \brief swap overload
inline void swap(imp& t1, imp& t2)
{
  t1.swap(t2);
}


/// \brief A boolean variable
class boolean_variable: public boolean_expression
{
  public:


    /// Move semantics
    boolean_variable(const boolean_variable&) noexcept = default;
    boolean_variable(boolean_variable&&) noexcept = default;
    boolean_variable& operator=(const boolean_variable&) noexcept = default;
    boolean_variable& operator=(boolean_variable&&) noexcept = default;

    const core::identifier_string& name() const
    {
      return atermpp::down_cast<core::identifier_string>((*this)[0]);
    }
//--- start user section boolean_variable ---//
    /// \brief Default constructor.
    boolean_variable()
      : boolean_expression(core::detail::default_values::BooleanVariable)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit boolean_variable(const atermpp::aterm& term)
      : boolean_expression(term)
    {
      assert(core::detail::check_term_BooleanVariable(*this));
    }

    /// \brief Constructor.
    boolean_variable(const core::identifier_string& name)
    {
      atermpp::make_term_appl_with_index<boolean_variable,core::identifier_string>
                      (*this,core::detail::function_symbol_BooleanVariable(), name);
    }

    /// \brief Constructor.
    boolean_variable(const std::string& name)
    {
      atermpp::make_term_appl_with_index<boolean_variable,core::identifier_string>
                      (*this,core::detail::function_symbol_BooleanVariable(), core::identifier_string(name));
    }
//--- end user section boolean_variable ---//
};

/// \brief Make_boolean_variable constructs a new term into a given address.
/// \ \param t The reference into which the new boolean_variable is constructed. 
template <class... ARGUMENTS>
inline void make_boolean_variable(atermpp::aterm_appl& t, ARGUMENTS... args)
{
  atermpp::make_term_appl_with_index<boolean_variable,core::identifier_string>(t, core::detail::function_symbol_BooleanVariable(), args...);
}

/// \brief Test for a boolean_variable expression
/// \param x A term
/// \return True if \a x is a boolean_variable expression
inline
bool is_boolean_variable(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::BooleanVariable;
}

// prototype declaration
std::string pp(const boolean_variable& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \param x Object x
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const boolean_variable& x)
{
  return out << bes::pp(x);
}

/// \brief swap overload
inline void swap(boolean_variable& t1, boolean_variable& t2)
{
  t1.swap(t2);
}
//--- end generated classes ---//

namespace accessors
{
inline
const boolean_expression& left(boolean_expression const& e)
{
  assert(is_and(e) || is_or(e) || is_imp(e));
  return atermpp::down_cast<boolean_expression>(e[0]);
}

inline
const boolean_expression& right(boolean_expression const& e)
{
  assert(is_and(e) || is_or(e) || is_imp(e));
  return atermpp::down_cast<boolean_expression>(e[1]);
}

} // namespace accessors

} // namespace bes

} // namespace mcrl2

namespace mcrl2
{

namespace core
{

/// \brief Contains type information for boolean expressions
template <>
struct term_traits<bes::boolean_expression>
{
  /// The term type
  typedef bes::boolean_expression term_type;

  /// \brief The variable type
  typedef bes::boolean_variable variable_type;

  /// \brief The string type
  typedef core::identifier_string string_type;

  /// \brief The value true
  /// \return The value true
  static inline
  bes::boolean_expression true_()
  {
    return bes::true_();
  }

  /// \brief The value false
  /// \return The value false
  static inline
  bes::boolean_expression false_()
  {
    return bes::false_();
  }

  /// \brief Operator not
  /// \param x A term
  /// \return Operator not applied to
  static inline
  bes::boolean_expression not_(const bes::boolean_expression& x)
  {
    return bes::not_(x);
  }

  /// \brief Operator and
  /// \param p A term
  /// \param q A term
  /// \return Operator and applied to p and q
  static inline
  bes::boolean_expression and_(const bes::boolean_expression& p, const bes::boolean_expression& q)
  {
    return bes::and_(p, q);
  }

  /// \brief Operator or
  /// \param p A term
  /// \param q A term
  /// \return Operator or applied to p and q
  static inline
  bes::boolean_expression or_(const bes::boolean_expression& p, const bes::boolean_expression& q)
  {
    return bes::or_(p, q);
  }

  /// \brief Implication
  /// \param p A term
  /// \param q A term
  /// \return Implication applied to p and q
  static inline
  bes::boolean_expression imp(const bes::boolean_expression& p, const bes::boolean_expression& q)
  {
    return bes::imp(p, q);
  }

  /// \brief Test for value true
  /// \param t A term
  /// \return True if the term has the value true
  static inline
  bool is_true(const bes::boolean_expression& t)
  {
    return bes::is_true(t);
  }

  /// \brief Test for value false
  /// \param t A term
  /// \return True if the term has the value false
  static inline
  bool is_false(const bes::boolean_expression& t)
  {
    return bes::is_false(t);
  }

  /// \brief Test for operator not
  /// \param t A term
  /// \return True if the term is of type and
  static inline
  bool is_not(const bes::boolean_expression& t)
  {
    return bes::is_not(t);
  }

  /// \brief Test for operator and
  /// \param t A term
  /// \return True if the term is of type and
  static inline
  bool is_and(const bes::boolean_expression& t)
  {
    return bes::is_and(t);
  }

  /// \brief Test for operator or
  /// \param t A term
  /// \return True if the term is of type or
  static inline
  bool is_or(const bes::boolean_expression& t)
  {
    return bes::is_or(t);
  }

  /// \brief Test for implication
  /// \param t A term
  /// \return True if the term is an implication
  static inline
  bool is_imp(const bes::boolean_expression& t)
  {
    return bes::is_imp(t);
  }

  /// \brief Test for propositional variable
  /// \param t A term
  /// \return True if the term is a propositional variable
  static inline
  bool is_prop_var(const bes::boolean_expression& t)
  {
    return bes::is_boolean_variable(t);
  }

  /// \brief Returns the left argument of a term of type and, or or imp
  /// \param t A term
  /// \return The left argument of the term
  static inline
  const bes::boolean_expression& left(const bes::boolean_expression& t)
  {
    assert(is_and(t) || is_or(t) || is_imp(t));
    return atermpp::down_cast<const bes::boolean_expression>(t[0]);
  }

  /// \brief Returns the right argument of a term of type and, or or imp
  /// \param t A term
  /// \return The right argument of the term
  static inline
  const bes::boolean_expression& right(const bes::boolean_expression& t)
  {
    assert(is_and(t) || is_or(t) || is_imp(t));
    return atermpp::down_cast<const bes::boolean_expression>(t[1]);
  }

  /// \brief Returns the argument of a term of type not
  /// \param t A term
  static inline
  const bes::boolean_expression& not_arg(const bes::boolean_expression& t)
  {
    assert(is_not(t));
    return atermpp::down_cast<bes::not_>(t).operand();
  }

  /// \brief Returns the name of a boolean variable
  /// \param t A term
  /// \return The name of the boolean variable
  static inline
  const core::identifier_string& name(const bes::boolean_expression& t)
  {
    assert(bes::is_boolean_variable(t));
    return atermpp::down_cast<bes::boolean_variable>(t).name();
  }

  /// \brief Conversion from variable to term
  /// \param v A variable
  /// \returns The converted variable
  static inline
  const bes::boolean_expression& variable2term(const bes::boolean_variable& v)
  {
    return v;
  }

  /// \brief Conversion from term to variable
  /// \param t a term
  /// \returns The converted term
  static inline
  const bes::boolean_variable& term2variable(const bes::boolean_expression& t)
  {
    return atermpp::down_cast<bes::boolean_variable>(t);
  }

  /// \brief Pretty print function
  /// \param t A term
  /// \return Returns a pretty print representation of the term
  static inline
  std::string pp(const bes::boolean_expression& t)
  {
    return bes::pp(t);
  }
};

} // namespace core

} // namespace mcrl2

#endif // MCRL2_BES_BOOLEAN_EXPRESSION_H
