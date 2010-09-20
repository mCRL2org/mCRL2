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
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/algorithm.h" // find_if
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/term_traits.h"
#include "mcrl2/bes/boolean_variable.h"

namespace mcrl2 {

namespace bes {

  /// \brief boolean expression
  class boolean_expression: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      boolean_expression()
        : atermpp::aterm_appl(core::detail::constructBooleanExpression())
      {}

      /// \brief Constructor.
      /// \param term A term
      boolean_expression(ATermAppl term)
        : atermpp::aterm_appl(term)
      {
        assert(core::detail::check_rule_BooleanExpression(m_term));
      }

      /// \brief Constructor.
      /// \param term A term
      boolean_expression(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(core::detail::check_rule_BooleanExpression(m_term));
      }
  };

  /// \brief Read-only singly linked list of boolean expressions
  typedef atermpp::term_list<boolean_expression> boolean_expression_list;

  /// \brief Returns true if the term t is a boolean expression
  /// \param t A boolean expression
  /// \return True if the term t is a boolean expression
  inline
  bool is_boolean_expression(atermpp::aterm_appl t)
  {
    // TODO: this code should be generated
    return
      core::detail::gsIsBooleanTrue    (t) ||
      core::detail::gsIsBooleanFalse   (t) ||
      core::detail::gsIsBooleanVariable(t) ||
      core::detail::gsIsBooleanNot     (t) ||
      core::detail::gsIsBooleanAnd     (t) ||
      core::detail::gsIsBooleanOr      (t) ||
      core::detail::gsIsBooleanImp     (t)
    ;
  }

} // namespace bes

} // namespace mcrl2

namespace mcrl2 {

namespace core {

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
    term_type true_() { return core::detail::gsMakeBooleanTrue(); }

    /// \brief The value false
    /// \return The value false
    static inline
    term_type false_() { return core::detail::gsMakeBooleanFalse(); }

    /// \brief Operator not
    /// \param p A term
    /// \return Operator not applied to p
    static inline
    term_type not_(term_type p) { return core::detail::gsMakeBooleanNot(p); }

    /// \brief Operator and
    /// \param p A term
    /// \param q A term
    /// \return Operator and applied to p and q
    static inline
    term_type and_(term_type p, term_type q) { return core::detail::gsMakeBooleanAnd(p, q); }

    /// \brief Operator or
    /// \param p A term
    /// \param q A term
    /// \return Operator or applied to p and q
    static inline
    term_type or_(term_type p, term_type q) { return core::detail::gsMakeBooleanOr(p, q); }

    /// \brief Implication
    /// \param p A term
    /// \param q A term
    /// \return Implication applied to p and q
    static inline
    term_type imp(term_type p, term_type q) { return core::detail::gsMakeBooleanImp(p, q); }

    /// \brief Test for value true
    /// \param t A term
    /// \return True if the term has the value true
    static inline
    bool is_true(term_type t) { return core::detail::gsIsBooleanTrue(t); }

    /// \brief Test for value false
    /// \param t A term
    /// \return True if the term has the value false
    static inline
    bool is_false(term_type t) { return core::detail::gsIsBooleanFalse(t); }

    /// \brief Test for operator not
    /// \param t A term
    /// \return True if the term is of type and
    static inline
    bool is_not(term_type t) { return core::detail::gsIsBooleanNot(t); }

    /// \brief Test for operator and
    /// \param t A term
    /// \return True if the term is of type and
    static inline
    bool is_and(term_type t) { return core::detail::gsIsBooleanAnd(t); }

    /// \brief Test for operator or
    /// \param t A term
    /// \return True if the term is of type or
    static inline
    bool is_or(term_type t) { return core::detail::gsIsBooleanOr(t); }

    /// \brief Test for implication
    /// \param t A term
    /// \return True if the term is an implication
    static inline
    bool is_imp(term_type t) { return core::detail::gsIsBooleanImp(t); }

    /// \brief Test for boolean variable
    /// \param t A term
    /// \return True if the term is a boolean variable
    static inline
    bool is_variable(term_type t) { return core::detail::gsIsBooleanVariable(t); }

    /// \brief Test for propositional variable
    /// \param t A term
    /// \return True if the term is a propositional variable
    static inline
    bool is_prop_var(term_type t) { return is_variable(t); }

    /// \brief Returns the argument of a term of type not
    /// \param t A term
    /// \return The requested argument
    static inline
    term_type arg(term_type t)
    {
      assert(is_not(t));
      return atermpp::arg1(t);
    }

    /// \brief Returns the left argument of a term of type and, or or imp
    /// \param t A term
    /// \return The left argument of the term
    static inline
    term_type left(term_type t)
    {
      assert(is_and(t) || is_or(t) || is_imp(t));
      return atermpp::arg1(t);
    }

    /// \brief Returns the right argument of a term of type and, or or imp
    /// \param t A term
    /// \return The right argument of the term
    static inline
    term_type right(term_type t)
    {
      assert(is_and(t) || is_or(t) || is_imp(t));
      return atermpp::arg2(t);
    }

    /// \brief Returns the name of a boolean variable
    /// \param t A term
    /// \return The name of the boolean variable
    static inline
    string_type name(term_type t)
    {
      assert(is_variable(t));
      return atermpp::arg1(t);
    }

    /// \brief Conversion from variable to term
    /// \param v A variable
    /// \returns The converted variable
    static inline
    term_type variable2term(variable_type v) { return v; }

    /// \brief Conversion from term to variable
    /// \param t a term
    /// \returns The converted term
    static inline
    variable_type term2variable(term_type t) { return t; }

    /// \brief Test if a term is constant
    /// \param t A term
    /// \return True if the term is constant
    static inline
    bool is_constant(term_type t)
    {
      return atermpp::find_if(t, is_variable) != atermpp::aterm_appl();
    }

    /// \brief Pretty print function
    /// \param t A term
    /// \return Returns a pretty print representation of the term
    static inline
    std::string pp(term_type t)
    {
      return mcrl2::bes::pp(t);
    }
  };

} // namespace core

} // namespace mcrl2

namespace mcrl2 {

namespace bes {

  /// \brief Pretty print function
  /// \param e A boolean expression
  /// \param add_parens If true, parentheses are put around sub-expressions.
  /// \return A pretty printed representation of the boolean expression.
  // TODO: the implementation is not very efficient
  inline
  std::string pp(boolean_expression e, bool add_parens = false)
  {
    typedef core::term_traits<boolean_expression> tr;

    if (tr::is_variable(e))
    {
      return pp(tr::term2variable(e));
    }
    else if (tr::is_true(e))
    {
      return "true";
    }
    else if (tr::is_false(e))
    {
      return "false";
    }
    else if (tr::is_not(e))
    {
      return std::string("!") + (add_parens ? "(" : "") + pp(tr::arg(e), true) + (add_parens ? ")" : "");
    }
    else if (tr::is_and(e))
    {
      return (add_parens ? "(" : "") + pp(tr::left(e), true) + " && " + pp(tr::right(e), true) + (add_parens ? ")" : "");
    }
    else if (tr::is_or(e))
    {
      return (add_parens ? "(" : "") + pp(tr::left(e), true) + " || " + pp(tr::right(e), true) + (add_parens ? ")" : "");
    }
    else if (tr::is_imp(e))
    {
      return (add_parens ? "(" : "") + pp(tr::left(e), true) + " => " + pp(tr::right(e), true) + (add_parens ? ")" : "");
    }
    throw mcrl2::runtime_error("error in mcrl2::bes::pp: encountered unknown boolean expression " + e.to_string());
    return "";
  }

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_BOOLEAN_EXPRESSION_H
