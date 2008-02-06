// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/data_expression.h
/// \brief The class data_expression.

#ifndef MCRL2_DATA_DATA_EXPRESSION_H
#define MCRL2_DATA_DATA_EXPRESSION_H

#include <string>
#include <cassert>
#include "mcrl2/core/struct.h"
#include "mcrl2/core/detail/join.h"
#include "mcrl2/core/detail/optimized_logic_operators.h"
#include "mcrl2/data/sort_expression.h"

namespace mcrl2 {

namespace data {

using atermpp::aterm_appl;
using atermpp::term_list;
using atermpp::aterm;

// prototypes
class data_expression;

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
    /// Constructor.
    ///             
    data_expression()
      : aterm_appl(core::detail::constructDataExpr())
    {}

    /// Constructor.
    ///             
    data_expression(aterm_appl term)
      : aterm_appl(term)
    {
      assert(core::detail::check_rule_DataExprOrNil(m_term));
    }

    /// Constructor.
    ///             
    data_expression(ATermAppl term)
      : aterm_appl(term)
    {
      assert(core::detail::check_rule_DataExprOrNil(m_term));
    }

    /// Returns the sort of the data expression.
    ///
    sort_expression sort() const
    {
      ATermAppl result = core::gsGetSort(*this);
      assert(!core::detail::gsIsSortUnknown(result));
      return sort_expression(result);
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
  return core::gsIsDataExpr(t);
}

/// Returns the head of the data expression t.
/// \deprecated
inline
data_expression DEPRECATED_FUNCTION_HEAD(data_expression t)
{
  return core::gsGetDataExprHead(t);
}

/// Returns the arguments of the data expression t.
/// \deprecated
inline
data_expression_list DEPRECATED_FUNCTION_ARGUMENTS(data_expression t)
{
  return core::gsGetDataExprArgs(t);
}

/// Accessor functions and predicates for data expressions.
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
  inline bool is_nil(aterm_appl t) { return t == core::detail::gsMakeNil(); }

  /// \brief Returns true if the term t is equal to true
  inline bool is_true(aterm_appl t) { return core::gsIsDataExprTrue(t); }

  /// \brief Returns true if the term t is equal to false
  inline bool is_false(aterm_appl t) { return core::gsIsDataExprFalse(t); }

  /// \brief Returns true if the term t is a not expression
  inline bool is_not(aterm_appl t) { return core::gsIsDataExprNot(t); }

  /// \brief Returns true if the term t is an and expression
  inline bool is_and(aterm_appl t) { return core::gsIsDataExprAnd(t); }

  /// \brief Returns true if the term t is an or expression
  inline bool is_or(aterm_appl t) { return core::gsIsDataExprOr(t); }

  /// \brief Returns true if the term t is a  expression
  inline bool is_implies(aterm_appl t) { return core::gsIsDataExprImp(t); }

  /// \brief Returns true if the term t is a binder
  inline bool is_binder(aterm_appl t) { return core::detail::gsIsBinder(t); }

  /// \brief Returns true if the term t is a where expression
  inline bool is_where(aterm_appl t) { return core::detail::gsIsWhr(t); }

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
  inline bool is_exists(aterm_appl t) { return core::detail::gsIsBinder(t) && core::detail::gsIsExists(aterm_appl(t(0))); }

  /// \brief Returns true if the term t is a universal quantification
  inline bool is_forall(aterm_appl t) { return core::detail::gsIsBinder(t) && core::detail::gsIsForall(aterm_appl(t(0))); }

  /// \brief Returns true if the term t is a lambda expression
  inline bool is_lambda(aterm_appl t) { return core::detail::gsIsBinder(t) && core::detail::gsIsLambda(aterm_appl(t(0))); }

  /// \brief Returns a real with value i
  // TODO: a function gsMakaDataExprReal_int will be added by Aad
  // Perhaps even real(double d) will become possible?
  inline
  data_expression real(int i)
  {
    return core::gsMakeDataExprCReal(core::gsMakeDataExprInt_int(i));
  }

  /// \brief Returns a pos with value i
  /// Precondition: i > 0
  inline
  data_expression pos(unsigned int i)
  {
    assert(i > 0);
    return core::gsMakeDataExprPos_int(i);
  }

  /// \brief Returns a nat with value i
  inline
  data_expression nat(unsigned int i)
  {
    return core::gsMakeDataExprNat_int(i);
  }

  /// \brief Returns an int with value i
  inline
  data_expression int_(int i)
  {
    return core::gsMakeDataExprInt_int(i);
  }

  /// \brief Returns the expression true
  inline
  data_expression true_()
  {
    return data_expression(core::gsMakeDataExprTrue());
  }

  /// \brief Returns the expression false
  inline
  data_expression false_()
  {
    return data_expression(core::gsMakeDataExprFalse());
  }

  /// \brief Returns not applied to p
  /// This function contains optimizations for true and false arguments.
  inline
  data_expression not_(data_expression p)
  {
    return data_expression(core::gsMakeDataExprNot(p));
  }

  /// \brief Returns and applied to p and q
  /// This function contains optimizations for true and false arguments.
  inline
  data_expression and_(data_expression p, data_expression q)
  {
    return data_expression(core::gsMakeDataExprAnd(p,q));
  }

  /// \brief Returns or applied to p and q
  /// This function contains optimizations for true and false arguments.
  inline
  data_expression or_(data_expression p, data_expression q)
  {
    return data_expression(core::gsMakeDataExprOr(p,q));
  }

  /// \brief Returns or applied to the sequence of data expressions [first, last[
  template <typename FwdIt>
  data_expression join_or(FwdIt first, FwdIt last)
  {
    return core::detail::join(first, last, or_, false_());
  }
  
  /// \brief Returns and applied to the sequence of data expressions [first, last[
  template <typename FwdIt>
  data_expression join_and(FwdIt first, FwdIt last)
  {
    return core::detail::join(first, last, and_, true_());
  }

  /// \brief Returns true if t is a negate expression
  inline bool is_negate(data_expression t) { return core::gsIsDataExprNeg(t); }

  /// \brief Returns true if t is a plus expression
  inline bool is_plus(data_expression t) { return core::gsIsDataExprAdd(t); }

  /// \brief Returns true if t is a minus expression
  inline bool is_minus(data_expression t) { return core::gsIsDataExprSubt(t); }

  /// \brief Returns true if t is a multiply expression
  inline bool is_multiplies(data_expression t) { return core::gsIsDataExprMult(t); }

  /// \brief Returns true if t is a division expression
  inline bool is_divides(data_expression t) { return core::gsIsDataExprDiv(t); }

  /// \brief Returns true if t is a modulus expression
  inline bool is_modulus(data_expression t) { return core::gsIsDataExprMod(t); }

  /// \brief Returns true if t is an equality expression
  inline bool is_equal_to(data_expression t) { return core::gsIsDataExprEq(t); }

  /// \brief Returns true if t is an inequality expression
  inline bool is_not_equal_to(data_expression t) { return core::gsIsDataExprNeq(t); }

  /// \brief Returns true if t is a less expression
  inline bool is_less(data_expression t) { return core::gsIsDataExprLT(t); }

  /// \brief Returns true if t is a greater expression
  inline bool is_greater(data_expression t) { return core::gsIsDataExprGT(t); }

  /// \brief Returns true if t is a less-equal expression
  inline bool is_less_equal(data_expression t) { return core::gsIsDataExprLTE(t); }

  /// \brief Returns true if t is a greater-equal expression
  inline bool is_greater_equal(data_expression t) { return core::gsIsDataExprGTE(t); }

  /// \brief Returns true if t is a min expression
  inline bool is_min(data_expression t) { return core::gsIsDataExprMin(t); }

  /// \brief Returns true if t is a max expression
  inline bool is_max(data_expression t) { return core::gsIsDataExprMax(t); }

  /// \brief Returns true if t is an abs expression
  inline bool is_abs(data_expression t) { return core::gsIsDataExprAbs(t); }

  /// \brief Returns the expression d - e
  inline
  data_expression negate(data_expression d)
  {
    return core::gsMakeDataExprNeg(d);
  }

  /// \brief Returns the expression d + e
  inline
  data_expression plus(data_expression d, data_expression e)
  {
    return core::gsMakeDataExprAdd(d, e);
  }

  /// \brief Returns the expression d - e
  inline
  data_expression minus(data_expression d, data_expression e)
  {
    return core::gsMakeDataExprSubt(d, e);
  }

  /// \brief Returns the expression d * e
  inline
  data_expression multiplies(data_expression d, data_expression e)
  {
    return core::gsMakeDataExprMult(d, e);
  }

  /// \brief Returns the expression d / e
  inline
  data_expression divides(data_expression d, data_expression e)
  {
    return core::gsMakeDataExprDiv(d, e);
  }

  /// \brief Returns the expression d % e
  inline
  data_expression modulus(data_expression d, data_expression e)
  {
    return core::gsMakeDataExprMod(d, e);
  }

  /// \brief Returns the expression d = e
  inline
  data_expression equal_to(data_expression d, data_expression e)
  {
    return core::gsMakeDataExprEq(d, e);
  }

  /// \brief Returns the expression d != e
  inline
  data_expression not_equal_to(data_expression d, data_expression e)
  {
    return core::gsMakeDataExprNeq(d, e);
  }

  /// \brief Returns the expression d < e
  inline
  data_expression less(data_expression d, data_expression e)
  {
    return core::gsMakeDataExprLT(d, e);
  }

  /// \brief Returns the expression d > e
  inline
  data_expression greater(data_expression d, data_expression e)
  {
    return core::gsMakeDataExprGT(d, e);
  }

  /// \brief Returns the expression d <= e
  inline
  data_expression less_equal(data_expression d, data_expression e)
  {
    return core::gsMakeDataExprLTE(d, e);
  }

  /// \brief Returns the expression d >= e
  inline
  data_expression greater_equal(data_expression d, data_expression e)
  {
    return core::gsMakeDataExprGTE(d, e);
  }

  /// \brief Returns an expression for the minimum of d and e
  inline
  data_expression min_(data_expression d, data_expression e)
  {
    return core::gsMakeDataExprMin(d, e);
  }

  /// \brief Returns an expression for the maximum of d and e
  inline
  data_expression max_(data_expression d, data_expression e)
  {
    return core::gsMakeDataExprMax(d, e);
  }

  /// \brief Returns an expression for the absolute value of d
  inline
  data_expression abs(data_expression d)
  {
    return core::gsMakeDataExprAbs(d);
  }

  /// \brief Returns the expression 'if i then t else e'
  inline
  data_expression if_(data_expression i, data_expression t, data_expression e)
  {
    return core::gsMakeDataExprIf(i, t, e);
  }

  namespace optimized {
    /// \brief Returns not applied to p, and simplifies the result.
    inline
    data_expression not_(data_expression p)
    {
      using namespace data_expr;
      return core::detail::optimized_not(p, not_, true_(), is_true, false_(), is_false);
    }
    
    /// \brief Returns and applied to p and q, and simplifies the result.
    inline
    data_expression and_(data_expression p, data_expression q)
    {
      using namespace data_expr;
      return core::detail::optimized_and(p, q, and_, true_(), is_true, false_(), is_false);
    }
    
    /// \brief Returns or applied to p and q, and simplifies the result.
    inline
    data_expression or_(data_expression p, data_expression q)
    {
      using namespace data_expr;
      return core::detail::optimized_or(p, q, or_, true_(), is_true, false_(), is_false);
    }
    
    /// \brief Returns or applied to the sequence of data expressions [first, last[
    template <typename FwdIt>
    inline data_expression join_or(FwdIt first, FwdIt last)
    {
      using namespace data_expr;
      return core::detail::join(first, last, optimized::or_, false_());
    }
    
    /// \brief Returns and applied to the sequence of data expressions [first, last[
    template <typename FwdIt>
    inline data_expression join_and(FwdIt first, FwdIt last)
    {
      using namespace data_expr;
      return core::detail::join(first, last, optimized::and_, true_());
    }
  } // namespace optimized

} // namespace data_expr

} // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp
{
using mcrl2::data::data_expression;

template<>
struct aterm_traits<data_expression>
{
  typedef ATermAppl aterm_type;
  static void protect(data_expression t)   { t.protect(); }
  static void unprotect(data_expression t) { t.unprotect(); }
  static void mark(data_expression t)      { t.mark(); }
  static ATerm term(data_expression t)     { return t.term(); }
  static ATerm* ptr(data_expression& t)    { return &t.term(); }
};

} // namespace atermpp
/// \endcond

#endif // MCRL2_DATA_DATA_EXPRESSION_H
