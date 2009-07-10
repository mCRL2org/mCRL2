// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/expression_traits.h
/// \brief Contains term traits for data_expression.

#ifndef MCRL2_DATA_TERM_TRAITS_H
#define MCRL2_DATA_TERM_TRAITS_H

#include <functional>
#include "mcrl2/core/term_traits.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/abstraction.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/detail/data_sequence_algorithm.h"

namespace mcrl2 {

namespace data {
  template < typename Container >
  std::set< variable > find_free_variables(Container const& container);

  template < typename Container >
  std::set< variable > find_variables(Container const& container);
}

namespace core {

  /// \brief Contains type information for data expressions.
  template <>
  struct term_traits<data::data_expression>
  {
    /// \brief The term type
    typedef data::data_expression term_type;

    /// \brief The variable type
    typedef data::variable variable_type;

    /// \brief The variable sequence type
    typedef data::variable_list variable_sequence_type;

    /// \brief The value true
    /// \return The value true
    static inline
    term_type true_()
    { return data::sort_bool::true_(); }

    /// \brief The value false
    /// \return The value false
    static inline
    term_type false_()
    { return data::sort_bool::false_(); }

    /// \brief Operator not
    /// \param p A term
    /// \return Operator not applied to p
    static inline
    term_type not_(term_type p)
    { return data::sort_bool::not_(p); }

    /// \brief Operator and
    /// \param p A term
    /// \param q A term
    /// \return Operator and applied to p and q
    static inline
    term_type and_(term_type p, term_type q)
    { return data::sort_bool::and_(p, q); }

    /// \brief Operator or
    /// \param p A term
    /// \param q A term
    /// \return Operator or applied to p and q
    static inline
    term_type or_(term_type p, term_type q)
    { return data::sort_bool::or_(p, q); }

    /// \brief Test for value true
    /// \param t A term
    /// \return True if the term has the value true
    static inline
    bool is_true(term_type t)
    { return t == data::sort_bool::true_(); }

    /// \brief Test for value false
    /// \param t A term
    /// \return True if the term has the value false
    static inline
    bool is_false(term_type t)
    { return t == data::sort_bool::false_(); }

    /// \brief Test for operator not
    /// \param t A term
    /// \return True if the term is of type not
    static inline
    bool is_not(term_type t)
    { return data::sort_bool::is_not_application(t); }

    /// \brief Test for operator and
    /// \param t A term
    /// \return True if the term is of type and
    static inline
    bool is_and(term_type t)
    { return data::sort_bool::is_and_application(t); }

    /// \brief Test for operator or
    /// \param t A term
    /// \return True if the term is of type or
    static inline
    bool is_or(term_type t)
    { return data::sort_bool::is_or_application(t); }

    /// \brief Test for implication
    /// \param t A term
    /// \return True if the term is an implication
    static inline
    bool is_imp(term_type t)
    { return data::sort_bool::is_implies_application(t); }

    /// \brief Test for universal quantification
    /// \param t A term
    /// \return True if the term is an universal quantification
    static inline
    bool is_forall(term_type t)
    { return t.is_abstraction() && data::abstraction(t).is_forall(); }

    /// \brief Test for existential quantification
    /// \param t A term
    /// \return True if the term is an existential quantification
    static inline
    bool is_exists(term_type t)
    { return t.is_abstraction() && data::abstraction(t).is_exists(); }

    /// \brief Test for lambda abstraction
    /// \param t A term
    /// \return True if the term is a lambda expression
    static inline
    bool is_lambda(term_type t)
    { return t.is_abstraction() && data::abstraction(t).is_lambda(); }

    /// \brief Conversion from variable to term
    /// \param v A variable
    /// \return The converted variable
    static inline
    term_type variable2term(variable_type v)
    {
      return v;
    }

    /// \brief Test if a term is a variable
    /// \param t A term
    /// \return True if the term is a variable
    static inline
    bool is_variable(term_type t)
    {
      return t.is_variable();
    }

    /// \brief Returns the free variables of a term
    /// \param t A term
    /// \return The free variables of a term
    static inline
    variable_sequence_type free_variables(term_type t)
    {
      std::set<variable_type> v = data::find_free_variables(data::data_expression(t));
      return variable_sequence_type(v.begin(), v.end());
    }

    /// \brief Returns the difference of two unordered sets of variables
    /// \param v A sequence of data variables
    /// \param w A sequence of data variables
    /// \return The difference of two sets.
    static inline
    variable_sequence_type set_intersection(const variable_sequence_type& v, const variable_sequence_type& w)
    {
      return data::detail::set_intersection(v, w);
    }

    /// \brief Test if a term is constant
    /// \param t A term
    /// \return True if the term is constant. N.B. It is unknown if the current implementation
    /// works for quantifier expressions.
    static inline
    bool is_constant(term_type t)
    {
      return find_variables(t).empty();
    }

    /// \brief Pretty print function
    /// \param t A term
    /// \return A pretty print representation of the term
    static inline
    std::string pp(term_type t)
    {
      return data::pp(t);
    }
  };

} // namespace core

namespace data {
  /// \brief expression traits (currently nothing more than core::term_traits)
  template < typename Expression >
  struct expression_traits : public core::term_traits< Expression >
  {
    // Type of expression that represents variables
    typedef mcrl2::data::variable variable_type;

    // Iterator range for arguments of function applications
    typedef application::arguments_const_range arguments_const_range;
    typedef abstraction::variables_const_range bound_variables_const_range;

    static bool is_true(data_expression const& e)
    {
      return sort_bool::is_true_function_symbol(e);
    }

    static bool is_false(data_expression const& e)
    {
      return sort_bool::is_false_function_symbol(e);
    }

    static bool is_application(data_expression const& e)
    {
      return e.is_application();
    }

    static bool is_abstraction(data_expression const& e)
    {
      return e.is_abstraction();
    }

    static data_expression head(data_expression const& e)
    {
      return application(e).head();
    }

    static arguments_const_range arguments(data_expression const& e)
    {
      return application(e).arguments();
    }

    static bound_variables_const_range variables(data_expression const& a)
    {
      return abstraction(a).variables();
    }

    static data_expression body(data_expression const& a)
    {
      return abstraction(a).body();
    }

    static data_expression replace_body(data_expression const& variable_binder, data_expression const& new_body)
    {
      return abstraction(abstraction(variable_binder).binding_operator(), abstraction(variable_binder).variables(), new_body);
    }

    template < typename ForwardTraversalIterator >
    static application make_application(data_expression const& e, boost::iterator_range< ForwardTraversalIterator > const& arguments)
    {
      return application(e, arguments);
    }

    static data_expression false_()
    {
      return sort_bool::false_();
    }

    static data_expression true_()
    {
      return sort_bool::true_();
    }

    static data_expression and_(data_expression const& e1, data_expression const& e2)
    {
      return sort_bool::and_(e1, e1);
    }

    static data_expression or_(data_expression const& e1, data_expression const& e2)
    {
      return sort_bool::or_(e1, e1);
    }
  };
}

} // namespace mcrl2

#include "mcrl2/data/find.h"

#endif // MCRL2_DATA_TERM_TRAITS_H
