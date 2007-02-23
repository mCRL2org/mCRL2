#ifndef LPE_PBES_EXPRESSION_H
#define LPE_PBES_EXPRESSION_H

#include "atermpp/aterm.h"
#include "atermpp/aterm_traits.h"
#include "atermpp/aterm_access.h"
#include "lpe/soundness_checks.h"

namespace lpe {

using atermpp::aterm_appl;
using atermpp::arg1;
using atermpp::arg2;
using atermpp::list_arg1;
using atermpp::list_arg2;

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
    {}

    pbes_expression(aterm_appl term)
      : aterm_appl(term)
    {
      assert(check_rule_PBExpr(m_term));
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
};

///////////////////////////////////////////////////////////////////////////////
// pbes_expression_list
/// \brief singly linked list of data expressions
///
typedef term_list<pbes_expression> pbes_expression_list;

/// Functions for accessing attributes of pbes expressions.
namespace pbes_expr {

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

} // namespace lpe

/// INTERNAL ONLY
namespace atermpp
{
using lpe::pbes_expression;

template<>
struct aterm_traits<pbes_expression>
{
  typedef ATermAppl aterm_type;
  static void protect(lpe::pbes_expression t)   { t.protect(); }
  static void unprotect(lpe::pbes_expression t) { t.unprotect(); }
  static void mark(lpe::pbes_expression t)      { t.mark(); }
  static ATerm term(lpe::pbes_expression t)     { return t.term(); }
  static ATerm* ptr(lpe::pbes_expression& t)    { return &t.term(); }
};

} // namespace atermpp

#endif // LPE_PBES_EXPRESSION_H
