#ifndef LPS_PBES_EXPRESSION_H
#define LPS_PBES_EXPRESSION_H

#include "atermpp/aterm.h"
#include "atermpp/aterm_traits.h"
#include "atermpp/aterm_access.h"
#include "lps/detail/soundness_checks.h"
#include "lps/propositional_variable.h"

namespace lps {

using atermpp::aterm_appl;
using atermpp::arg1;
using atermpp::arg2;
using atermpp::list_arg1;
using atermpp::list_arg2;

// prototype
inline
bool is_bes(aterm_appl t);

///////////////////////////////////////////////////////////////////////////////
// pbes_expression
/// \brief pbes expression.
///
// <PBExpr>       ::= <DataExpr>
//                  | PBESTrue
//                  | PBESFalse
//                  | PBESAnd(<PBExpr>, <PBExpr>)
//                  | PBESOr(<PBExpr>, <PBExpr>)
//                  | PBESForall(<DataVarId>+, <PBExpr>)
//                  | PBESExists(<DataVarId>+, <PBExpr>)
//                  | <PropVarInst>
class pbes_expression: public aterm_appl
{
  public:
    pbes_expression()
      : aterm_appl(detail::constructPBExpr())
    {}

    pbes_expression(aterm_appl term)
      : aterm_appl(term)
    {
      assert(detail::check_rule_PBExpr(m_term));
    }

    // allow assignment to aterms
    pbes_expression& operator=(aterm t)
    {
      m_term = t;
      return *this;
    }

    /// Returns true if the pbes expression equals 'true'.
    /// Note that the term will not be rewritten first.
    ///
    /// DEPRECATED
    bool is_true() const
    {
      return *this == gsMakePBESTrue();
    }     

    /// Returns true if the pbes expression equals 'false'.
    /// Note that the term will not be rewritten first.
    ///
    /// DEPRECATED
    bool is_false() const
    {
      return *this == gsMakePBESFalse();
    }

    /// Applies a substitution to this pbes expression and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    pbes_expression substitute(Substitution f)
    {
      return pbes_expression(f(*this));
    }     

    /// Returns true if the expression is a boolean expression.
    bool is_bes() const
    {
      return lps::is_bes(*this);
    }
};

///////////////////////////////////////////////////////////////////////////////
// pbes_expression_list
/// \brief singly linked list of data expressions
///
typedef term_list<pbes_expression> pbes_expression_list;

/// Functions for accessing attributes of pbes expressions.
namespace pbes_expr {

inline bool is_data                                (pbes_expression t) { return gsIsDataExpr      (t); }
inline bool is_true                                (pbes_expression t) { return gsIsPBESTrue      (t); }
inline bool is_false                               (pbes_expression t) { return gsIsPBESFalse     (t); }
inline bool is_and                                 (pbes_expression t) { return gsIsPBESAnd       (t); }
inline bool is_or                                  (pbes_expression t) { return gsIsPBESOr        (t); }
inline bool is_forall                              (pbes_expression t) { return gsIsPBESForall    (t); }
inline bool is_exists                              (pbes_expression t) { return gsIsPBESExists    (t); }
inline bool is_propositional_variable_instantiation(pbes_expression t) { return gsIsPropVarInst   (t); }

/// Conversion of a data expression to a pbes expression.
inline
pbes_expression val(data_expression d)
{
  return pbes_expression(aterm_appl(d));
}

inline
pbes_expression true_()
{
  return pbes_expression(gsMakePBESTrue());
}

inline
pbes_expression false_()
{
  return pbes_expression(gsMakePBESFalse());
}

inline
pbes_expression and_(pbes_expression p, pbes_expression q)
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
    return pbes_expression(gsMakePBESAnd(p,q));
}

inline
pbes_expression or_(pbes_expression p, pbes_expression q)
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
    return pbes_expression(gsMakePBESOr(p,q));
}

inline
pbes_expression forall(data_variable_list l, pbes_expression p)
{
  return pbes_expression(gsMakePBESForall(l, p));
}

inline
pbes_expression exists(data_variable_list l, pbes_expression p)
{
  return pbes_expression(gsMakePBESExists(l, p));
}

/*
inline
pbes_fixpoint_symbol mu()
{
  return pbes_equation::mu();
}

inline
pbes_fixpoint_symbol nu()
{
  return pbes_equation::nu();
}
*/

/// Returns or_ applied to the sequence of pbes expressions [first, last[
template <typename FwdIt>
inline pbes_expression multi_or(FwdIt first, FwdIt last)
{
  using namespace pbes_expr;

  if (first == last)
    return pbes_expr::false_();
  pbes_expression result = *first++;
  while (first != last)
  {
    result = or_(result, *first++);
  }
  return result;
}

/// Returns and_ applied to the sequence of pbes expressions [first, last[
template <typename FwdIt>
inline pbes_expression multi_and(FwdIt first, FwdIt last)
{
  using namespace pbes_expr;

  if (first == last)
    return pbes_expr::true_();
  pbes_expression result = *first++;
  while (first != last)
  {
    result = and_(result, *first++);
  }
  return result;
}

/// Returns the left hand side of a PBES expression of type and/or.
inline
pbes_expression lhs(pbes_expression t)
{
  assert(gsIsPBESAnd(t) || gsIsPBESOr(t));
  return arg1(t);
}

/// Returns the right hand side of a PBES expression of type and/or.
inline
pbes_expression rhs(pbes_expression t)
{
  assert(gsIsPBESAnd(t) || gsIsPBESOr(t));
  return arg2(t);
}

/// Returns the quantifier variables of a PBES expression of type exists/forall.
inline
data_variable_list quant_vars(pbes_expression t)
{
  assert(gsIsPBESExists(t) || gsIsPBESForall(t));
  return list_arg1(t);
}

/// Returns the quantifier expression of a PBES expression of type exists/forall.
inline
pbes_expression quant_expr(pbes_expression t)
{
  assert(gsIsPBESExists(t) || gsIsPBESForall(t));
  return arg2(t);
}

} // namespace pbes_expr

/// Returns true if the pbes expression t is a boolean expression.
inline
bool is_bes(aterm_appl t)
{
  using namespace pbes_expr;
  using namespace pbes_expr;

  if (is_and(t)) {
    return is_bes(lhs(t)) && is_bes(rhs(t));
  }
  else if (is_or(t)) {
    return is_bes(lhs(t)) && is_bes(rhs(t));
  }
  else if (is_forall(t)) {
    return false;
  }
  else if (is_exists(t)) {
    return false;
  }
  else if (is_propositional_variable_instantiation(t)) {
    return propositional_variable_instantiation(t).parameters().empty();
  }
  else if (is_true(t)) {
    return true;
  }
  else if (is_false(t)) {
    return true;
  }

  return false;
}

} // namespace lps

/// INTERNAL ONLY
namespace atermpp
{
using lps::pbes_expression;

template<>
struct aterm_traits<pbes_expression>
{
  typedef ATermAppl aterm_type;
  static void protect(lps::pbes_expression t)   { t.protect(); }
  static void unprotect(lps::pbes_expression t) { t.unprotect(); }
  static void mark(lps::pbes_expression t)      { t.mark(); }
  static ATerm term(lps::pbes_expression t)     { return t.term(); }
  static ATerm* ptr(lps::pbes_expression& t)    { return &t.term(); }
};

} // namespace atermpp

#endif // LPS_PBES_EXPRESSION_H
