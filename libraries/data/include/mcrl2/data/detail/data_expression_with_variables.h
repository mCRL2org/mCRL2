// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/data_expression_with_variables.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_DATA_EXPRESSION_WITH_VARIABLES_H
#define MCRL2_DATA_DATA_EXPRESSION_WITH_VARIABLES_H

#include "mcrl2/atermpp/set_operations.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/abstraction.h"
#include "mcrl2/data/expression_traits.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/print.h"

namespace mcrl2 {

namespace data {

  /// \brief Data expression with a sequence of variables attached to it
  /// The intended use case is to store the free variables of the expression.
  class data_expression_with_variables: public data_expression
  {
    protected:

      /// \brief The attached variables.
      variable_list m_variables;

    public:

      /// \brief Constructor.
      data_expression_with_variables()
      {}

      /// \brief Constructor. Creates a data expression with an empty sequence of variables.
      /// \param term A term
      data_expression_with_variables(atermpp::aterm_appl term)
        : data_expression(term)
      {}

      /// \brief Constructor. Creates a data expression with an empty sequence of variables.
      /// \param term A term
      data_expression_with_variables(ATermAppl term)
        : data_expression(term)
      {}

      /// \brief Constructor.
      /// \param expression A data expression
      /// \param variables A sequence of data variables
      template < typename Container >
      data_expression_with_variables(data_expression expression, const Container& variables)
        : data_expression(expression), m_variables(convert< variable_list >(variables))
      {}

      /// \brief Return the variables.
      /// \return The variables of the data expression.
      variable_list variables() const
      {
        return m_variables;
      }

      /// \brief Return the variables.
      /// \return The variables of the data expression.
      variable_list& variables()
      {
        return m_variables;
      }

      /// \brief Returns true if the sequence of variables is empty.
      /// \return True if the sequence of variables is empty.
      bool is_constant() const
      {
        return m_variables.empty();
      }
  };

} // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp {
template<>
struct aterm_traits<mcrl2::data::data_expression_with_variables >
{
  typedef ATermAppl aterm_type;
  static void protect(mcrl2::data::data_expression_with_variables t)   { t.protect(); t.variables().protect(); }
  static void unprotect(mcrl2::data::data_expression_with_variables t) { t.unprotect(); t.variables().unprotect(); }
  static void mark(mcrl2::data::data_expression_with_variables t)      { t.mark(); t.variables().mark(); }
  static ATerm term(mcrl2::data::data_expression_with_variables t)     { return t.term(); }
  static ATerm* ptr(mcrl2::data::data_expression_with_variables& t)    { return &t.term(); }
};
}
/// \endcond

namespace mcrl2 {

namespace core {

  /// \brief Contains type information for data expressions with variables.
  template <>
  struct term_traits<data::data_expression_with_variables>
  {
    /// \brief The term type
    typedef data::data_expression_with_variables term_type;

    /// \brief The variable type
    typedef data::variable variable_type;

    /// \brief The variable sequence type
    typedef data::variable_list variable_sequence_type;

    /// \brief The value true
    /// \return The value true
    static inline
    term_type true_() { return data::sort_bool_::true_(); }

    /// \brief The value false
    /// \return The value false
    static inline
    term_type false_() { return data::sort_bool_::false_(); }

    /// \brief Operator not
    /// \param p A term
    /// \return Operator not applied to p
    static inline
    term_type not_(term_type p) { return term_type(data::sort_bool_::not_(p), p.variables()); }

    /// \brief Make a conjunction
    /// \param p A term
    /// \param q A term
    /// \return The value <tt>p && q</tt>
    static inline
    term_type and_(term_type p, term_type q)
    {
      return term_type(data::sort_bool_::and_(p, q), p.variables() + q.variables());
    }

    /// \brief Make a disjunction
    /// \param p A term
    /// \param q A term
    /// \return The value <tt>p || q</tt>
    static inline
    term_type or_(term_type p, term_type q)
    {
      return term_type(data::sort_bool_::or_(p, q), p.variables() + q.variables());
    }

    /// \brief Test for value true
    /// \param t A term
    /// \return True if the term has the value true
    static inline
    bool is_true(term_type t) { return data::sort_bool_::is_true__function_symbol(t); }

    /// \brief Test for value false
    /// \param t A term
    /// \return True if the term has the value false
    static inline
    bool is_false(term_type t) { return data::sort_bool_::is_false__function_symbol(t); }

    /// \brief Test for operator not
    /// \param t A term
    /// \return True if the term is of type not
    static inline
    bool is_not(term_type t) { return data::sort_bool_::is_not__application(t); }

    /// \brief Test for operator and
    /// \param t A term
    /// \return True if the term is of type and
    static inline
    bool is_and(term_type t) { return data::sort_bool_::is_and__application(t); }

    /// \brief Test for operator or
    /// \param t A term
    /// \return True if the term is of type or
    static inline
    bool is_or(term_type t) { return data::sort_bool_::is_or__application(t); }

    /// \brief Test for implication
    /// \param t A term
    /// \return True if the term is an implication
    static inline
    bool is_imp(term_type t) { return data::sort_bool_::is_implies_application(t); }

    /// \brief Test for universal quantification
    /// \param t A term
    /// \return True if the term is an universal quantification
    static inline
    bool is_forall(term_type t)
    {
      data::data_expression e(t);
      return e.is_abstraction() && data::abstraction(e).is_forall();
    }

    /// \brief Test for existential quantification
    /// \param t A term
    /// \return True if the term is an existential quantification
    static inline
    bool is_exists(term_type t)
    {
      data::data_expression e(t);
      return e.is_abstraction() && data::abstraction(e).is_exists();
    }

    /// \brief Conversion from variable to term
    /// \param v A variable
    /// \return The converted variable
    static inline
    term_type variable2term(variable_type v)
    {
      return term_type(v, atermpp::push_front(data::variable_list(), v));
    }

    /// \brief Test if a term is a variable
    /// \param t A term
    /// \return True if the term is a variable
    static inline
    bool is_variable(term_type t)
    {
      return data::data_expression(t).is_variable();
    }

    /// \brief Returns the free variables of a term
    /// \param t A term
    /// \return The free variables of a term
    static inline
    variable_sequence_type free_variables(term_type t)
    {
      return t.variables();
    }

    /// \brief Returns the difference of two unordered sets of variables
    /// \param v A sequence of data variables
    /// \param w A sequence of data variables
    /// \return The difference of two sets.
    static inline
    variable_sequence_type set_difference(const variable_sequence_type& v, const variable_sequence_type& w)
    {
      return term_traits<data::data_expression>::set_difference(v, w);
    }

    /// \brief Test if a term is constant
    /// \param t A term
    /// \return True if the term is constant
    static inline
    bool is_constant(term_type t)
    {
      return t.variables().empty();
    }

    /// \brief Pretty print function
    /// \param t A term
    /// \return Returns a pretty print representation of the term
    static inline
    std::string pp(term_type t)
    {
      return core::pp(t) + " " + data::pp(t.variables());
    }
  };

} // namespace core

} // namespace mcrl2

#endif // MCRL2_DATA_DATA_EXPRESSION_WITH_VARIABLES_H
