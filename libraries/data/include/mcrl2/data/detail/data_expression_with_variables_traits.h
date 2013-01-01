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

#ifndef MCRL2_DATA_DATA_EXPRESSION_WITH_VARIABLES_TRAITS_H
#define MCRL2_DATA_DATA_EXPRESSION_WITH_VARIABLES_TRAITS_H

#include "mcrl2/data/detail/data_expression_with_variables.h"
#include "mcrl2/data/expression_traits.h"

namespace mcrl2
{

namespace core
{

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
  term_type true_()
  {
    return data::sort_bool::true_();
  }

  /// \brief The value false
  /// \return The value false
  static inline
  term_type false_()
  {
    return data::sort_bool::false_();
  }

  /// \brief Operator not
  /// \param p A term
  /// \return Operator not applied to p
  static inline
  term_type not_(term_type p)
  {
    return term_type(data::sort_bool::not_(p), p.variables());
  }

  /// \brief Make a conjunction
  /// \param p A term
  /// \param q A term
  /// \return The value <tt>p && q</tt>
  static inline
  term_type and_(term_type p, term_type q)
  {
    return term_type(data::sort_bool::and_(p, q), p.variables() + q.variables());
  }

  /// \brief Make a disjunction
  /// \param p A term
  /// \param q A term
  /// \return The value <tt>p || q</tt>
  static inline
  term_type or_(term_type p, term_type q)
  {
    return term_type(data::sort_bool::or_(p, q), p.variables() + q.variables());
  }

  /// \brief Test for value true
  /// \param t A term
  /// \return True if the term has the value true
  static inline
  bool is_true(term_type t)
  {
    return data::sort_bool::is_true_function_symbol(t);
  }

  /// \brief Test for value false
  /// \param t A term
  /// \return True if the term has the value false
  static inline
  bool is_false(term_type t)
  {
    return data::sort_bool::is_false_function_symbol(t);
  }

  /// \brief Test for operator not
  /// \param t A term
  /// \return True if the term is of type not
  static inline
  bool is_not(term_type t)
  {
    return data::sort_bool::is_not_application(t);
  }

  /// \brief Test for operator and
  /// \param t A term
  /// \return True if the term is of type and
  static inline
  bool is_and(term_type t)
  {
    return data::sort_bool::is_and_application(t);
  }

  /// \brief Test for operator or
  /// \param t A term
  /// \return True if the term is of type or
  static inline
  bool is_or(term_type t)
  {
    return data::sort_bool::is_or_application(t);
  }

  /// \brief Test for implication
  /// \param t A term
  /// \return True if the term is an implication
  static inline
  bool is_imp(term_type t)
  {
    return data::sort_bool::is_implies_application(t);
  }

  /// \brief Test for universal quantification
  /// \param t A term
  /// \return True if the term is an universal quantification
  static inline
  bool is_forall(term_type t)
  {
    data::data_expression e(t);
    return data::is_forall(e);
  }

  /// \brief Test for existential quantification
  /// \param t A term
  /// \return True if the term is an existential quantification
  static inline
  bool is_exists(term_type t)
  {
    data::data_expression e(t);
    return data::is_exists(e);
  }

  /// \brief Conversion from variable to term
  /// \param v A variable
  /// \return The converted variable
  static inline
  term_type variable2term(variable_type v)
  {
    return term_type(v, make_list(v));
  }

  /// \brief Test if a term is a variable
  /// \param t A term
  /// \return True if the term is a variable
  static inline
  bool is_variable(term_type t)
  {
    return data::is_variable(t);
  }

  /// \brief Returns the free variables of a term
  /// \param t A term
  /// \return The free variables of a term
  static inline
  variable_sequence_type free_variables(term_type t)
  {
    return t.variables();
  }

  /// \brief Returns the intersection of two unordered sets of variables
  /// \param v A sequence of data variables
  /// \param w A sequence of data variables
  /// \return The intersection of two sets.
  static inline
  variable_sequence_type set_intersection(const variable_sequence_type& v, const variable_sequence_type& w)
  {
    return term_traits<data::data_expression>::set_intersection(v, w);
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
    return term_traits<data::data_expression>::pp(t) + " " + data::pp(t.variables());
  }
};

} // namespace core

} // namespace mcrl2

#endif // MCRL2_DATA_DATA_EXPRESSION_WITH_VARIABLES_H
