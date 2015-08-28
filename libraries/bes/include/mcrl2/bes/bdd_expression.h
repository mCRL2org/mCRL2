// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/bdd_expression.h
/// \brief add your file description here.

#ifndef MCRL2_BES_BDD_EXPRESSION_H
#define MCRL2_BES_BDD_EXPRESSION_H

#include <sstream>
#include "mcrl2/core/detail/default_values.h"
#include "mcrl2/core/detail/function_symbols.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/utilities/exception.h"

namespace mcrl2 {

namespace bdd {

//--- start generated classes ---//
/// \brief A bdd expression
class bdd_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    bdd_expression()
      : atermpp::aterm_appl(core::detail::default_values::BddExpression)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit bdd_expression(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_BddExpression(*this));
    }
};

/// \brief list of bdd_expressions
typedef atermpp::term_list<bdd_expression> bdd_expression_list;

/// \brief vector of bdd_expressions
typedef std::vector<bdd_expression>    bdd_expression_vector;

// prototypes
inline bool is_true(const atermpp::aterm_appl& x);
inline bool is_false(const atermpp::aterm_appl& x);
inline bool is_if(const atermpp::aterm_appl& x);

/// \brief Test for a bdd_expression expression
/// \param x A term
/// \return True if \a x is a bdd_expression expression
inline
bool is_bdd_expression(const atermpp::aterm_appl& x)
{
  return bdd::is_true(x) ||
         bdd::is_false(x) ||
         bdd::is_if(x);
}

// prototype declaration
std::string pp(const bdd_expression& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const bdd_expression& x)
{
  return out << bdd::pp(x);
}

/// \brief swap overload
inline void swap(bdd_expression& t1, bdd_expression& t2)
{
  t1.swap(t2);
}


/// \brief The value true for bdd expressions
class true_: public bdd_expression
{
  public:
    /// \brief Default constructor.
    true_()
      : bdd_expression(core::detail::default_values::BddTrue)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit true_(const atermpp::aterm& term)
      : bdd_expression(term)
    {
      assert(core::detail::check_term_BddTrue(*this));
    }
};

/// \brief Test for a true expression
/// \param x A term
/// \return True if \a x is a true expression
inline
bool is_true(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::BddTrue;
}

// prototype declaration
std::string pp(const true_& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const true_& x)
{
  return out << bdd::pp(x);
}

/// \brief swap overload
inline void swap(true_& t1, true_& t2)
{
  t1.swap(t2);
}


/// \brief The value false for bdd expressions
class false_: public bdd_expression
{
  public:
    /// \brief Default constructor.
    false_()
      : bdd_expression(core::detail::default_values::BddFalse)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit false_(const atermpp::aterm& term)
      : bdd_expression(term)
    {
      assert(core::detail::check_term_BddFalse(*this));
    }
};

/// \brief Test for a false expression
/// \param x A term
/// \return True if \a x is a false expression
inline
bool is_false(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::BddFalse;
}

// prototype declaration
std::string pp(const false_& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const false_& x)
{
  return out << bdd::pp(x);
}

/// \brief swap overload
inline void swap(false_& t1, false_& t2)
{
  t1.swap(t2);
}


/// \brief The if operator for bdd expressions
class if_: public bdd_expression
{
  public:
    /// \brief Default constructor.
    if_()
      : bdd_expression(core::detail::default_values::BddIf)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit if_(const atermpp::aterm& term)
      : bdd_expression(term)
    {
      assert(core::detail::check_term_BddIf(*this));
    }

    /// \brief Constructor.
    if_(const core::identifier_string& name, const bdd_expression& left, const bdd_expression& right)
      : bdd_expression(atermpp::aterm_appl(core::detail::function_symbol_BddIf(), name, left, right))
    {}

    /// \brief Constructor.
    if_(const std::string& name, const bdd_expression& left, const bdd_expression& right)
      : bdd_expression(atermpp::aterm_appl(core::detail::function_symbol_BddIf(), core::identifier_string(name), left, right))
    {}

    const core::identifier_string& name() const
    {
      return atermpp::down_cast<core::identifier_string>((*this)[0]);
    }

    const bdd_expression& left() const
    {
      return atermpp::down_cast<bdd_expression>((*this)[1]);
    }

    const bdd_expression& right() const
    {
      return atermpp::down_cast<bdd_expression>((*this)[2]);
    }
};

/// \brief Test for a if expression
/// \param x A term
/// \return True if \a x is a if expression
inline
bool is_if(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::BddIf;
}

// prototype declaration
std::string pp(const if_& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const if_& x)
{
  return out << bdd::pp(x);
}

/// \brief swap overload
inline void swap(if_& t1, if_& t2)
{
  t1.swap(t2);
}
//--- end generated classes ---//

inline
std::string pp(const bdd::false_&)
{
  return "false";
}

inline
std::string pp(const bdd::true_&)
{
  return "true";
}

inline
std::string pp(const bdd::if_& x)
{
  return "if(" + pp(x.left()) + ", " + pp(x.right()) + ")";
}

inline
std::string pp(const bdd::bdd_expression& x)
{
  if (is_false(x))
  {
    return pp(atermpp::down_cast<false_>(x));
  }
  else if (is_true(x))
  {
    return pp(atermpp::down_cast<true_>(x));
  }
  else if (is_if(x))
  {
    return pp(atermpp::down_cast<if_>(x));
  }
  std::ostringstream out;
  out << x;
  throw mcrl2::runtime_error("unknown bdd expression " + out.str());
}

namespace accessors {

bdd_expression left(const bdd_expression &e)
{
  assert(is_if(e));
  return atermpp::down_cast<if_>(e).left();
}

bdd_expression right(const bdd_expression &e)
{
  assert(is_if(e));
  return atermpp::down_cast<if_>(e).right();
}

} // namespace accessors

} // namespace bdd

} // namespace mcrl2

#endif // MCRL2_BES_BDD_EXPRESSION_H
