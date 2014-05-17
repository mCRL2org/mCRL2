// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/boolean_expression.h
/// \brief add your file description here.

#ifndef MCRL2_BES_BOOLEAN_EXPRESSION_H
#define MCRL2_BES_BOOLEAN_EXPRESSION_H

#include <cassert>
#include <string>

#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_base_of.hpp>

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/utilities/detail/join.h"
#include "mcrl2/core/detail/default_values.h"
#include "mcrl2/core/detail/function_symbols.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/detail/precedence.h"
#include "mcrl2/core/index_traits.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/term_traits.h"
#include "mcrl2/core/print.h"
#include "mcrl2/utilities/text_utility.h"

namespace mcrl2
{

namespace bes
{

typedef core::identifier_string boolean_variable_key_type;

template <typename T> std::string pp(const T& t);

using namespace core::detail::precedences;

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
    not_(const boolean_expression& operand)
      : boolean_expression(atermpp::aterm_appl(core::detail::function_symbol_BooleanNot(), operand))
    {}

    const boolean_expression& operand() const
    {
      return atermpp::aterm_cast<const boolean_expression>((*this)[0]);
    }
};

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

    const boolean_expression& left() const
    {
      return atermpp::aterm_cast<const boolean_expression>((*this)[0]);
    }

    const boolean_expression& right() const
    {
      return atermpp::aterm_cast<const boolean_expression>((*this)[1]);
    }
};

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

    const boolean_expression& left() const
    {
      return atermpp::aterm_cast<const boolean_expression>((*this)[0]);
    }

    const boolean_expression& right() const
    {
      return atermpp::aterm_cast<const boolean_expression>((*this)[1]);
    }
};

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

    const boolean_expression& left() const
    {
      return atermpp::aterm_cast<const boolean_expression>((*this)[0]);
    }

    const boolean_expression& right() const
    {
      return atermpp::aterm_cast<const boolean_expression>((*this)[1]);
    }
};

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


    const core::identifier_string& name() const
    {
      return atermpp::aterm_cast<const core::identifier_string>((*this)[0]);
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
      : boolean_expression(atermpp::aterm_appl(core::detail::function_symbol_BooleanVariable(),
          name,
          atermpp::aterm_int(core::index_traits<boolean_variable, boolean_variable_key_type, 1>::insert(name)
        )))
    {}

    /// \brief Constructor.
    boolean_variable(const std::string& name)
      : boolean_expression(atermpp::aterm_appl(core::detail::function_symbol_BooleanVariable(),
          core::identifier_string(name),
          atermpp::aterm_int(core::index_traits<boolean_variable, boolean_variable_key_type, 1>::insert(name)
        )))
    {}
//--- end user section boolean_variable ---//
};

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

// From the documentation:
// The "!" operator has the highest priority, followed by "&&" and "||", followed by "=>".
// The infix operators "&&", "||" and "=>" associate to the right.
/// \brief Returns the precedence of boolean expressions
// N.B. The is_base_of construction is needed to make sure that the precedence also works on
// classes of type 'and_', 'or_' and 'imp'.
inline int left_precedence(const imp&)    { return 2; }
inline int left_precedence(const or_&)    { return 3; }
inline int left_precedence(const and_&)   { return 4; }
inline int left_precedence(const not_&)   { return 5; }
inline int left_precedence(const boolean_expression& x)
{
       if (is_imp(x)) { return left_precedence(static_cast<const imp&>(x)); }
  else if (is_or(x))  { return left_precedence(static_cast<const or_&>(x)); }
  else if (is_and(x)) { return left_precedence(static_cast<const and_&>(x)); }
  else if (is_not(x)) { return left_precedence(static_cast<const not_&>(x)); }
  return core::detail::precedences::max_precedence;
}

inline int right_precedence(const boolean_expression& x)
{
  return left_precedence(x);
}

inline const boolean_expression& unary_operand(const not_& x) { return x.operand(); }
inline const boolean_expression& binary_left(const and_& x)   { return x.left(); }
inline const boolean_expression& binary_right(const and_& x)  { return x.right(); }
inline const boolean_expression& binary_left(const or_& x)    { return x.left(); }
inline const boolean_expression& binary_right(const or_& x)   { return x.right(); }
inline const boolean_expression& binary_left(const imp& x)    { return x.left(); }
inline const boolean_expression& binary_right(const imp& x)   { return x.right(); }

/// \brief Returns true if the operations have the same precedence, but are different
inline
bool is_same_different_precedence(const and_&, const boolean_expression& x)
{
  return is_or(x);
}

/// \brief Returns true if the operations have the same precedence, but are different
inline
bool is_same_different_precedence(const or_&, const boolean_expression& x)
{
  return is_and(x);
}

namespace accessors
{
inline
const boolean_expression &left(boolean_expression const& e)
{
  assert(is_and(e) || is_or(e) || is_imp(e));
  return atermpp::aterm_cast<const boolean_expression>(e[0]);
}

inline
const boolean_expression &right(boolean_expression const& e)
{
  assert(is_and(e) || is_or(e) || is_imp(e));
  return atermpp::aterm_cast<const boolean_expression>(e[1]);
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
  term_type true_()
  {
    return term_type(atermpp::aterm_appl(core::detail::function_symbol_BooleanTrue()));
  }

  /// \brief The value false
  /// \return The value false
  static inline
  term_type false_()
  {
    return term_type(atermpp::aterm_appl(core::detail::function_symbol_BooleanFalse()));
  }

  /// \brief Operator not
  /// \param p A term
  /// \return Operator not applied to p
  static inline
  term_type not_(const term_type& p)
  {
    return term_type(atermpp::aterm_appl(core::detail::function_symbol_BooleanNot(), p));
  }

  /// \brief Operator and
  /// \param p A term
  /// \param q A term
  /// \return Operator and applied to p and q
  static inline
  term_type and_(term_type p, term_type q)
  {
    return term_type(atermpp::aterm_appl(core::detail::function_symbol_BooleanAnd(), p, q));
  }

  /// \brief Operator or
  /// \param p A term
  /// \param q A term
  /// \return Operator or applied to p and q
  static inline
  term_type or_(term_type p, term_type q)
  {
    return term_type(atermpp::aterm_appl(core::detail::function_symbol_BooleanOr(), p, q));
  }

  /// \brief Implication
  /// \param p A term
  /// \param q A term
  /// \return Implication applied to p and q
  static inline
  term_type imp(term_type p, term_type q)
  {
    return term_type(atermpp::aterm_appl(core::detail::function_symbol_BooleanImp(), p, q));
  }

  /// \brief Test for value true
  /// \param t A term
  /// \return True if the term has the value true
  static inline
  bool is_true(const term_type& t)
  {
    return bes::is_true(t);
  }

  /// \brief Test for value false
  /// \param t A term
  /// \return True if the term has the value false
  static inline
  bool is_false(const term_type& t)
  {
    return bes::is_false(t);
  }

  /// \brief Test for operator not
  /// \param t A term
  /// \return True if the term is of type and
  static inline
  bool is_not(const term_type& t)
  {
    return bes::is_not(t);
  }

  /// \brief Test for operator and
  /// \param t A term
  /// \return True if the term is of type and
  static inline
  bool is_and(const term_type& t)
  {
    return bes::is_and(t);
  }

  /// \brief Test for operator or
  /// \param t A term
  /// \return True if the term is of type or
  static inline
  bool is_or(const term_type& t)
  {
    return bes::is_or(t);
  }

  /// \brief Test for implication
  /// \param t A term
  /// \return True if the term is an implication
  static inline
  bool is_imp(const term_type& t)
  {
    return bes::is_imp(t);
  }

  /// \brief Test for boolean variable
  /// \param t A term
  /// \return True if the term is a boolean variable
  static inline
  bool is_variable(const term_type& t)
  {
    return bes::is_boolean_variable(t);
  }

  /// \brief Test for propositional variable
  /// \param t A term
  /// \return True if the term is a propositional variable
  static inline
  bool is_prop_var(const term_type& t)
  {
    return is_variable(t);
  }

  /// \brief Returns the argument of a term of type not
  /// \param t A term
  /// \return The requested argument
  static inline
  const term_type &arg(const term_type& t)
  {
    assert(is_not(t));
    return atermpp::aterm_cast<const term_type>(t[0]);
  }

  /// \brief Returns the left argument of a term of type and, or or imp
  /// \param t A term
  /// \return The left argument of the term
  static inline
  const term_type &left(const term_type& t)
  {
    assert(is_and(t) || is_or(t) || is_imp(t));
    return atermpp::aterm_cast<const term_type>(t[0]);
  }

  /// \brief Returns the right argument of a term of type and, or or imp
  /// \param t A term
  /// \return The right argument of the term
  static inline
  const term_type &right(const term_type& t)
  {
    assert(is_and(t) || is_or(t) || is_imp(t));
    return atermpp::aterm_cast<const term_type>(t[1]);
  }

  /// \brief Returns the argument of a term of type not
  /// \param t A term
  static inline
  const term_type &not_arg(const term_type& t)
  {
    assert(is_not(t));
    return atermpp::aterm_cast<const term_type>(t[0]);
  }

  /// \brief Returns the name of a boolean variable
  /// \param t A term
  /// \return The name of the boolean variable
  static inline
  const string_type &name(const term_type& t)
  {
    assert(is_variable(t));
    return atermpp::aterm_cast<const string_type>(t[0]);
  }

  /// \brief Conversion from variable to term
  /// \param v A variable
  /// \returns The converted variable
  static inline
  const term_type &variable2term(const variable_type &v)
  {
    return atermpp::aterm_cast<const term_type>(v);
  }

  /// \brief Conversion from term to variable
  /// \param t a term
  /// \returns The converted term
  static inline
  const variable_type &term2variable(const term_type& t)
  {
    return atermpp::aterm_cast<const variable_type>(t);
  }

  /// \brief Pretty print function
  /// \param t A term
  /// \return Returns a pretty print representation of the term
  static inline
  std::string pp(const term_type& t)
  {
    return bes::pp(t);
  }
};

} // namespace core

} // namespace mcrl2

namespace mcrl2
{

namespace bes
{

/// \brief Returns or applied to the sequence of boolean expressions [first, last)
/// \param first Start of a sequence of boolean expressions
/// \param last End of a sequence of of boolean expressions
/// \return Or applied to the sequence of boolean expressions [first, last)
template <typename FwdIt>
inline
boolean_expression join_or(FwdIt first, FwdIt last)
{
  typedef core::term_traits<boolean_expression> tr;
  return utilities::detail::join(first, last, tr::or_, tr::false_());
}

/// \brief Returns and applied to the sequence of boolean expressions [first, last)
/// \param first Start of a sequence of boolean expressions
/// \param last End of a sequence of of boolean expressions
/// \return And applied to the sequence of boolean expressions [first, last)
template <typename FwdIt>
inline
boolean_expression join_and(FwdIt first, FwdIt last)
{
  typedef core::term_traits<boolean_expression> tr;
  return utilities::detail::join(first, last, tr::and_, tr::true_());
}

/* inline bool
operator<(const boolean_expression& x, const boolean_expression& y)
{
  return aterm_appl(x) < aterm_appl(y);
} */

/// \brief Splits a disjunction into a sequence of operands
/// Given a boolean expression of the form p1 || p2 || .... || pn, this will yield a
/// set of the form { p1, p2, ..., pn }, assuming that pi does not have a || as main
/// function symbol.
/// \param expr A boolean expression
/// \return A sequence of operands
inline
std::set<boolean_expression> split_or(const boolean_expression& expr)
{
  using namespace accessors;
  std::set<boolean_expression> result;
  utilities::detail::split(expr, std::insert_iterator<std::set<boolean_expression> >(result, result.begin()), is_or, left, right);
  return result;
}

/// \brief Splits a conjunction into a sequence of operands
/// Given a boolean expression of the form p1 && p2 && .... && pn, this will yield a
/// set of the form { p1, p2, ..., pn }, assuming that pi does not have a && as main
/// function symbol.
/// \param expr A boolean expression
/// \return A sequence of operands
inline
std::set<boolean_expression> split_and(const boolean_expression& expr)
{
  using namespace accessors;
  std::set<boolean_expression> result;
  utilities::detail::split(expr, std::insert_iterator<std::set<boolean_expression> >(result, result.begin()), is_and, left, right);
  return result;
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_BOOLEAN_EXPRESSION_H
