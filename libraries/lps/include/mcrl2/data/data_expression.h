// Copyright (c) 2007 Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file mcrl2/data/data_expression.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_DATA_EXPRESSION_H
#define MCRL2_LPS_DATA_EXPRESSION_H

#include <string>
#include <cassert>
#include "mcrl2/data/sort.h"
#include "mcrl2/data/pretty_print.h"
#include "mcrl2/lps/detail/soundness_checks.h"
#include "libstruct.h"

namespace lps {

using atermpp::aterm_appl;
using atermpp::term_list;
using atermpp::aterm;

// prototypes
class data_expression;

///////////////////////////////////////////////////////////////////////////////
// data_expression_list
/// \brief singly linked list of data expressions
///
typedef term_list<data_expression> data_expression_list;

///////////////////////////////////////////////////////////////////////////////
// data_expression
/// \brief data expression
/// Represents a data expression or nil.
/// Before using a data expression, the user must make sure that it is not nil.
class data_expression: public aterm_appl
{
  public:
    data_expression()
      : aterm_appl(detail::constructDataExpr())
    {}

    data_expression(aterm_appl term)
      : aterm_appl(term)
    {
      assert(detail::check_rule_DataExprOrNil(m_term));
    }

    data_expression(ATermAppl term)
      : aterm_appl(term)
    {
      assert(detail::check_rule_DataExprOrNil(m_term));
    }

    /// Returns the sort of the data expression.
    ///
    lps::sort sort() const
    {
      ATermAppl result = gsGetSort(*this);
      assert(!gsIsSortUnknown(result));
      return lps::sort(result);
    }     

    /// Returns the head of the data expression.
    ///
    data_expression head() const
    {
      return gsGetDataExprHead(*this);
    }

    /// Returns the arguments of the data expression.
    ///
    data_expression_list arguments() const
    {
      return gsGetDataExprArgs(*this);
    }  
  
    /// Applies a substitution to this data expression and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    data_expression substitute(Substitution f) const
    {
      return data_expression(f(aterm(*this)));
    }     
};

/// \brief Returns true if the term t is a data expression
inline
bool is_data_expression(aterm_appl t)
{
  return gsIsDataExpr(t);
}

namespace data_expr {
  // //data expression
  // <DataExpr>     ::= Id(<String>)                                          (- tc)
  //                  | <DataVarId>                                           (+ tc)
  //                  | <OpId>                                                (+ tc)
  //                  | DataAppl(<DataExpr>, <DataExpr>+)
  //                  | Number(<NumberString>, <SortExprOrUnknown>)           (- di)
  //                  | ListEnum(<DataExpr>+, <SortExprOrUnknown>)            (- di)
  //                  | SetEnum(<DataExpr>+, <SortExprOrUnknown>)             (- di)
  //                  | BagEnum(<BagEnumElt>+, <SortExprOrUnknown>)           (- di)
  //                  | Binder(<BindingOperator>, <DataVarId>+, <DataExpr>)   (- di)
  //                  | Whr(<DataExpr>, <WhrDecl>+)                           (- di)

  /// \brief Returns true if the term t is equal to nil
  inline bool is_nil(aterm_appl t) { return t == gsMakeNil(); }     

  /// \brief Returns true if the term t is equal to true
  inline bool is_true(aterm_appl t) { return gsIsDataExprTrue(t); }

  /// \brief Returns true if the term t is equal to false
  inline bool is_false(aterm_appl t) { return gsIsDataExprFalse(t); }

  /// \brief Returns true if the term t is a not expression
  inline bool is_not(aterm_appl t) { return gsIsDataExprNot(t); }

  /// \brief Returns true if the term t is an and expression
  inline bool is_and(aterm_appl t) { return gsIsDataExprAnd(t); }

  /// \brief Returns true if the term t is an or expression
  inline bool is_or(aterm_appl t) { return gsIsDataExprOr(t); }

  /// \brief Returns true if the term t is a  expression
  inline bool is_implies(aterm_appl t) { return gsIsDataExprImp(t); }

  /// \brief Returns true if the term t is a binder
  inline bool is_binder(aterm_appl t) { return gsIsBinder(t); }

  /// \brief Returns true if the term t is a where expression
  inline bool is_where(aterm_appl t) { return gsIsWhr(t); }                                 

  /// \brief Returns true if the term t has type real
  inline bool is_real(aterm_appl t) { return sort_expr::is_real(data_expression(t).sort()); }

  /// \brief Returns true if the term t has type int
  inline bool is_int(aterm_appl t) { return sort_expr::is_int (data_expression(t).sort()); }

  /// \brief Returns true if the term t has type pos
  inline bool is_pos(aterm_appl t) { return sort_expr::is_pos (data_expression(t).sort()); }

  /// \brief Returns true if the term t has type nat
  inline bool is_nat(aterm_appl t) { return sort_expr::is_nat (data_expression(t).sort()); }

  /// \brief Returns true if the term t has type bool
  inline bool is_bool(aterm_appl t) { return sort_expr::is_bool(data_expression(t).sort()); }

  // TODO: The following three functions do not belong here anymore, we need to find
  // a better place for these. Note that they have been changed to make sure
  // that we know we are working with a Binder before we take the binding operator.

  /// \brief Returns true if the term t is an existial quantification
  inline bool is_exists(aterm_appl t) { return gsIsBinder(t) && gsIsExists(aterm_appl(t(0))); }

  /// \brief Returns true if the term t is a universal quantification
  inline bool is_forall(aterm_appl t) { return gsIsBinder(t) && gsIsForall(aterm_appl(t(0))); }

  /// \brief Returns true if the term t is a lambda expression
  inline bool is_lambda(aterm_appl t) { return gsIsBinder(t) && gsIsLambda(aterm_appl(t(0))); }

  /// \brief Returns a real with value i
  // TODO: a function gsMakaDataExprReal_int will be added by Aad
  // Perhaps even real(double d) will become possible?
  inline
  data_expression real(int i)
  {
    return gsMakeDataExprCReal(gsMakeDataExprInt_int(i));
  }

  /// \brief Returns a pos with value i
  /// Precondition: i > 0
  inline
  data_expression pos(unsigned int i)
  {
    assert(i > 0);
    return gsMakeDataExprPos_int(i);
  }

  /// \brief Returns a nat with value i
  inline
  data_expression nat(unsigned int i)
  {
    return gsMakeDataExprNat_int(i);
  }

  /// \brief Returns an int with value i
  inline
  data_expression int_(int i)
  {
    return gsMakeDataExprInt_int(i);
  }

  /// \brief Returns the expression true
  inline
  data_expression true_()
  {
    return data_expression(gsMakeDataExprTrue());
  }
  
  /// \brief Returns the expression false
  inline
  data_expression false_()
  {
    return data_expression(gsMakeDataExprFalse());
  }
  
  /// \brief Returns not applied to p
  /// This function contains optimizations for true and false arguments.
  inline
  data_expression not_(data_expression p)
  {
    if (is_true(p))
      return false_();
    else if (is_false(p))
      return true_();
    else
      return data_expression(gsMakeDataExprNot(p));
  }
  
  /// \brief Returns and applied to p and q
  /// This function contains optimizations for true and false arguments.
  inline
  data_expression and_(data_expression p, data_expression q)
  {
    if (is_true(p))
      return q;
    else if (is_false(p))
      return false_();
    if (is_true(q))
      return p;
    else if (is_false(q))
      return false_();
    else
      return data_expression(gsMakeDataExprAnd(p,q));
  }
  
  /// \brief Returns or applied to p and q
  /// This function contains optimizations for true and false arguments.
  inline
  data_expression or_(data_expression p, data_expression q)
  {
    if (is_true(p))
      return true_();
    else if (is_false(p))
      return q;
    if (is_true(q))
      return true_();
    else if (is_false(q))
      return p;
    else
      return data_expression(gsMakeDataExprOr(p,q));
  }
  
  /// \brief Returns or applied to the sequence of data expressions [first, last[
  /// This function contains optimizations for true and false arguments.
  template <typename FwdIt>
  data_expression multi_or(FwdIt first, FwdIt last)
  {
    using namespace data_expr;
  
    if (first == last)
      return data_expr::false_();
    data_expression result = *first++;
    while (first != last)
    {
      result = or_(result, *first++);
    }
    return result;
  }
  
  /// \brief Returns and_ applied to the sequence of data expressions [first, last[
  /// This function contains optimizations for true and false arguments.
  template <typename FwdIt>
  data_expression multi_and(FwdIt first, FwdIt last)
  {
    using namespace data_expr;
  
    if (first == last)
      return data_expr::true_();
    data_expression result = *first++;
    while (first != last)
    {
      result = and_(result, *first++);
    }
    return result;
  }

} // namespace data_expr

} // namespace lps

/// INTERNAL ONLY
namespace atermpp
{
using lps::data_expression;

template<>
struct aterm_traits<data_expression>
{
  typedef ATermAppl aterm_type;
  static void protect(lps::data_expression t)   { t.protect(); }
  static void unprotect(lps::data_expression t) { t.unprotect(); }
  static void mark(lps::data_expression t)      { t.mark(); }
  static ATerm term(lps::data_expression t)     { return t.term(); }
  static ATerm* ptr(lps::data_expression& t)    { return &t.term(); }
};

} // namespace atermpp

#endif // MCRL2_LPS_DATA_EXPRESSION_H
