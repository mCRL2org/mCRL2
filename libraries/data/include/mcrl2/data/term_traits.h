// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/term_traits.h
/// \brief Contains term traits for data_expression.

#ifndef MCRL2_DATA_TERM_TRAITS_H
#define MCRL2_DATA_TERM_TRAITS_H

#include "mcrl2/core/term_traits.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/data_variable.h"

namespace mcrl2 {

namespace core {

  /// \brief Contains type information for data expressions.
  template <>
  struct term_traits<data::data_expression>
  {
    /// \brief The term type
    typedef data::data_expression term_type;

    /// \brief The variable type
    typedef data::data_variable variable_type;

    /// \brief The variable sequence type
    typedef data::data_variable_list variable_sequence_type;
    
    /// \brief The value true
    /// \return The value true
    static inline
    term_type true_() { return core::detail::gsMakeDataExprTrue(); }
    
    /// \brief The value false
    /// \return The value false
    static inline
    term_type false_() { return core::detail::gsMakeDataExprFalse(); }
    
    /// \brief Operator not
    /// \param p A term
    /// \return Operator not applied to p
    static inline
    term_type not_(term_type p) { return core::detail::gsMakeDataExprNot(p); }
    
    /// \brief Operator and
    /// \param p A term
    /// \param q A term
    /// \return Operator and applied to p and q
    static inline
    term_type and_(term_type p, term_type q) { return core::detail::gsMakeDataExprAnd(p,q); }
    
    /// \brief Operator or
    /// \param p A term
    /// \param q A term
    /// \return Operator or applied to p and q
    static inline
    term_type or_(term_type p, term_type q) { return core::detail::gsMakeDataExprOr(p,q); }

    /// \brief Test for value true
    /// \param t A term
    /// \return True if the term has the value true
    static inline
    bool is_true(term_type t) { return core::detail::gsIsDataExprTrue(t); }
    
    /// \brief Test for value false
    /// \param t A term
    /// \return True if the term has the value false
    static inline 
    bool is_false(term_type t) { return core::detail::gsIsDataExprFalse(t); }
    
    /// \brief Test for operator not
    /// \param t A term
    /// \return True if the term is of type not
    static inline 
    bool is_not(term_type t) { return core::detail::gsIsDataExprNot(t); }
    
    /// \brief Test for operator and
    /// \param t A term
    /// \return True if the term is of type and
    static inline 
    bool is_and(term_type t) { return core::detail::gsIsDataExprAnd(t); }
    
    /// \brief Test for operator or
    /// \param t A term
    /// \return True if the term is of type or
    static inline 
    bool is_or(term_type t) { return core::detail::gsIsDataExprOr(t); }
    
    /// \brief Test for implication
    /// \param t A term
    /// \return True if the term is an implication
    static inline 
    bool is_imp(term_type t) { return core::detail::gsIsDataExprImp(t);; }
    
    /// \brief Test for universal quantification
    /// \param t A term
    /// \return True if the term is an universal quantification
    static inline 
    bool is_forall(term_type t) { return core::detail::gsIsBinder(t) && core::detail::gsIsForall(atermpp::aterm_appl(t(0))); }
    
    /// \brief Test for existential quantification
    /// \param t A term
    /// \return True if the term is an existential quantification
    static inline 
    bool is_exists(term_type t) { return core::detail::gsIsBinder(t) && core::detail::gsIsLambda(atermpp::aterm_appl(t(0))); }

    /// \brief Conversion from variable to term
    /// \param v A variable
    /// \returns The converted variable
    static inline
    term_type variable2term(variable_type v)
    {
      return v;
    }

    /// \brief Test if a term is constant
    /// \param t A term
    /// \return True if the term is constant. N.B. It is unknown if the current implementation
    /// works for quantifier expressions.
    static inline
    bool is_constant(term_type t)
    {
      return atermpp::find_if(t, data::is_data_variable) == atermpp::aterm();
    }

    /// \brief Pretty print function
    /// \param t A term
    /// \brief Returns a pretty print representation of the term
    static inline
    std::string pp(term_type t)
    {
      return core::pp(t);
    }
  };
  
} // namespace core

} // namespace mcrl2

#endif // MCRL2_DATA_TERM_TRAITS_H
