// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_expression.h
/// \brief The class pbes_expression.

#ifndef MCRL2_PBES_PBES_EXPRESSION_H
#define MCRL2_PBES_PBES_EXPRESSION_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/data/data_variable.h"
#include "mcrl2/pbes/propositional_variable.h"

namespace mcrl2 {

namespace pbes_system {

using atermpp::aterm_appl;
using atermpp::term_list;
using atermpp::arg1;
using atermpp::arg2;
using atermpp::list_arg1;
using atermpp::list_arg2;

// prototype
inline
bool is_bes(aterm_appl t);

/// \brief pbes expression
///
// <PBExpr>       ::= <DataExpr>
//                  | PBESTrue
//                  | PBESFalse
//                  | PBESNot(<PBExpr>)
//                  | PBESAnd(<PBExpr>, <PBExpr>)
//                  | PBESOr(<PBExpr>, <PBExpr>)
//                  | PBESImp(<PBExpr>, <PBExpr>)
//                  | PBESForall(<DataVarId>+, <PBExpr>)
//                  | PBESExists(<DataVarId>+, <PBExpr>)
//                  | <PropVarInst>
class pbes_expression: public aterm_appl
{
  public:
    /// Constructor.
    ///
    pbes_expression()
      : aterm_appl(core::detail::constructPBExpr())
    {}

    /// Constructor.
    ///
    pbes_expression(aterm_appl term)
      : aterm_appl(term)
    {
      assert(core::detail::check_rule_PBExpr(m_term));
    }

    /// Applies a substitution to this pbes expression and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    pbes_expression substitute(Substitution f) const
    {
      return pbes_expression(f(*this));
    }     

    /// Returns true if the expression is a boolean expression.
    ///
    bool is_bes() const
    {
      return mcrl2::pbes_system::is_bes(*this);
    }
};

/// \brief singly linked list of data expressions
///
typedef term_list<pbes_expression> pbes_expression_list;

/// Accessor functions and predicates for pbes expressions.
namespace pbes_expr {

  /// \brief Returns true if the term t is a data expression
  inline bool is_data(pbes_expression t) { return core::gsIsDataExpr(t); }

  /// \brief Returns true if the term t is equal to true
  inline bool is_true(pbes_expression t) { return core::detail::gsIsPBESTrue(t); }

  /// \brief Returns true if the term t is equal to false
  inline bool is_false(pbes_expression t) { return core::detail::gsIsPBESFalse(t); }

  /// \brief Returns true if the term t is a not expression
  inline bool is_not(pbes_expression t) { return core::detail::gsIsPBESNot(t); }

  /// \brief Returns true if the term t is an and expression
  inline bool is_and(pbes_expression t) { return core::detail::gsIsPBESAnd(t); }

  /// \brief Returns true if the term t is an or expression
  inline bool is_or(pbes_expression t) { return core::detail::gsIsPBESOr(t); }

  /// \brief Returns true if the term t is an imp expression
  inline bool is_imp(pbes_expression t) { return core::detail::gsIsPBESImp(t); }

  /// \brief Returns true if the term t is a universal quantification
  inline bool is_forall(pbes_expression t) { return core::detail::gsIsPBESForall(t); }

  /// \brief Returns true if the term t is an existential quantification
  inline bool is_exists(pbes_expression t) { return core::detail::gsIsPBESExists(t); }

  /// \brief Returns true if the term t is a propositional variable expression
  inline bool is_propositional_variable_instantiation(pbes_expression t) { return core::detail::gsIsPropVarInst(t); }

  /// Conversion of a pbes expression to a data expression.
  /// \pre The pbes expression must be of the form val(d) for
  /// some data variable d.
  inline
  data::data_expression val_arg(pbes_expression t)
  {
    assert(is_data(t));
    return aterm_appl(t);
  }

  /// Conversion of a data expression to a pbes expression.
  inline
  pbes_expression val(data::data_expression d)
  {
    return pbes_expression(aterm_appl(d));
  }

  /// \brief Returns the expression true
  inline
  pbes_expression true_()
  {
    return pbes_expression(core::detail::gsMakePBESTrue());
  }

  /// \brief Returns the expression false
  inline
  pbes_expression false_()
  {
    return pbes_expression(core::detail::gsMakePBESFalse());
  }
  
  /// \brief Returns not applied to p
  inline
  pbes_expression not_(pbes_expression p)
  {
    if(is_true(p))
      return false_();
    else if(is_false(p))
      return true_();
    else
      return pbes_expression(core::detail::gsMakePBESNot(p));
  }
  
  /// \brief Returns and applied to p and q
  inline
  pbes_expression and_(pbes_expression p, pbes_expression q)
  {
    if(is_true(p))
      return q;
    else if(is_false(p))
      return false_();
    if(is_true(q))
      return p;
    else if(is_false(q))
      return false_();
    else
      return pbes_expression(core::detail::gsMakePBESAnd(p,q));
  }
  
  /// \brief Returns or applied to p and q
  inline
  pbes_expression or_(pbes_expression p, pbes_expression q)
  {
    if(is_true(p))
      return true_();
    else if(is_false(p))
      return q;
    if(is_true(q))
      return true_();
    else if(is_false(q))
      return p;
    else
      return pbes_expression(core::detail::gsMakePBESOr(p,q));
  }
  
  /// \brief Returns imp applied to p and q
  inline
  pbes_expression imp(pbes_expression p, pbes_expression q)
  {
    if(is_true(p))
      return q;
    else if(is_true(q))
      return true_();
    else
      return pbes_expression(core::detail::gsMakePBESImp(p,q));
  }
  
  /// \brief Returns the universal quantification of the expression p over the variables in l.
  /// If l is empty, p is returned.
  inline
  pbes_expression forall(data::data_variable_list l, pbes_expression p)
  {
    if (l.empty())
      return p;
    return pbes_expression(core::detail::gsMakePBESForall(l, p));
  }
  
  /// \brief Returns the existential quantification of the expression p over the variables in l.
  /// If l is empty, p is returned.
  inline
  pbes_expression exists(data::data_variable_list l, pbes_expression p)
  {
    if (l.empty())
      return p;
    return pbes_expression(core::detail::gsMakePBESExists(l, p));
  }

  /// \brief Returns or applied to the sequence of pbes expressions [first, last[
  template <typename FwdIt>
  inline pbes_expression join_or(FwdIt first, FwdIt last)
  {
    using namespace pbes_expr;
  
    if(first == last)
      return false_();
    pbes_expression result = *first++;
    while(first != last)
    {
      result = or_(result, *first++);
    }
    return result;
  }
  
  /// \brief Returns and applied to the sequence of pbes expressions [first, last[
  template <typename FwdIt>
  inline pbes_expression join_and(FwdIt first, FwdIt last)
  {
    using namespace pbes_expr;
  
    if(first == last)
      return true_();
    pbes_expression result = *first++;
    while(first != last)
    {
      result = and_(result, *first++);
    }
    return result;
  }

  /// \brief Returns the argument of an expression of type not
  inline
  pbes_expression not_arg(pbes_expression t)
  {
    assert(core::detail::gsIsPBESNot(t));
    return arg1(t);
  }
  
  /// \brief Returns the left hand side of an expression of type and/or
  inline
  pbes_expression lhs(pbes_expression t)
  {
    assert(core::detail::gsIsPBESAnd(t) || core::detail::gsIsPBESOr(t) || core::detail::gsIsPBESImp(t));
    return arg1(t);
  }
  
  /// \brief Returns the right hand side of an expression of type and/or
  inline
  pbes_expression rhs(pbes_expression t)
  {
    assert(core::detail::gsIsPBESAnd(t) || core::detail::gsIsPBESOr(t) || core::detail::gsIsPBESImp(t));
    return arg2(t);
  }
  
  /// \brief Returns the variables of a quantification expression
  inline
  data::data_variable_list quant_vars(pbes_expression t)
  {
    assert(core::detail::gsIsPBESExists(t) || core::detail::gsIsPBESForall(t));
    return list_arg1(t);
  }
  
  /// \brief Returns the formula of a quantification expression
  inline
  pbes_expression quant_expr(pbes_expression t)
  {
    assert(core::detail::gsIsPBESExists(t) || core::detail::gsIsPBESForall(t));
    return arg2(t);
  }
  
  /// \brief Returns the name of a propositional variable expression
  inline
  core::identifier_string var_name(pbes_expression t)
  {
    assert(core::detail::gsIsPropVarInst(t));
    return arg1(t);
  }
  
  /// \brief Returns the value of a propositional variable expression
  inline
  data::data_expression_list var_val(pbes_expression t)
  {
    assert(core::detail::gsIsPropVarInst(t));
    return list_arg2(t);
  }

  /// \cond INTERNAL_DOCS
  namespace detail { 
    inline
    void split_and_impl(const pbes_expression& expr, atermpp::set<pbes_expression>& result)
    {
      if (pbes_expr::is_and(expr))
      {
        split_and_impl(lhs(expr), result);
        split_and_impl(rhs(expr), result);
      }
      else
      {
        result.insert(expr);
      }
    }
    
    inline
    void split_or_impl(const pbes_expression& expr, atermpp::set<pbes_expression>& result)
    {
      if (pbes_expr::is_and(expr))
      {
        split_or_impl(lhs(expr), result);
        split_or_impl(rhs(expr), result);
      }
      else
      {
        result.insert(expr);
      }
    }  
  } // namespace detail
  /// \endcond

  /// Given a pbes expression of the form p1 || p2 || .... || pn, this will yield a 
  /// set of the form { p1, p2, ..., pn }, assuming that pi does not have a || as main 
  /// function symbol.
  inline
  atermpp::set<pbes_expression> split_or(const pbes_expression& expr)
  {
    atermpp::set<pbes_expression> result;
    detail::split_or_impl(expr, result);
    return result;
  }
  
  /// Given a pbes expression of the form p1 && p2 && .... && pn, this will yield a 
  /// set of the form { p1, p2, ..., pn }, assuming that pi does not have a && as main 
  /// function symbol.
  inline
  atermpp::set<pbes_expression> split_and(const pbes_expression& expr)
  {
    atermpp::set<pbes_expression> result;
    detail::split_and_impl(expr, result);
    return result;
  }

} // namespace pbes_expr

/// Unoptimized versions of logical operators for pbes expressions.
namespace pbes_expr_unoptimized {

  /// \brief Returns not applied to p
  inline
  pbes_expression not_(pbes_expression p)
  {
    return pbes_expression(core::detail::gsMakePBESNot(p));
  }
  
  /// \brief Returns and applied to p and q
  inline
  pbes_expression and_(pbes_expression p, pbes_expression q)
  {
    return pbes_expression(core::detail::gsMakePBESAnd(p,q));
  }
  
  /// \brief Returns or applied to p and q
  inline
  pbes_expression or_(pbes_expression p, pbes_expression q)
  {
    return pbes_expression(core::detail::gsMakePBESOr(p,q));
  }
  
  /// \brief Returns imp applied to p and q
  inline
  pbes_expression imp(pbes_expression p, pbes_expression q)
  {
    return pbes_expression(core::detail::gsMakePBESImp(p,q));
  } 
} // namespace pbes_expr_unoptimized

/// \brief Returns true if the pbes expression t is a boolean expression
inline
bool is_bes(aterm_appl t)
{
  using namespace pbes_expr;

  if(is_and(t)) {
    return is_bes(lhs(t)) && is_bes(rhs(t));
  }
  else if(is_or(t)) {
    return is_bes(lhs(t)) && is_bes(rhs(t));
  }
  else if(is_forall(t)) {
    return false;
  }
  else if(is_exists(t)) {
    return false;
  }
  else if(is_propositional_variable_instantiation(t)) {
    return propositional_variable_instantiation(t).parameters().empty();
  }
  else if(is_true(t)) {
    return true;
  }
  else if(is_false(t)) {
    return true;
  }

  return false;
}

} // namespace pbes_system

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp
{
using mcrl2::pbes_system::pbes_expression;

template<>
struct aterm_traits<pbes_expression>
{
  typedef ATermAppl aterm_type;
  static void protect(pbes_expression t)   { t.protect(); }
  static void unprotect(pbes_expression t) { t.unprotect(); }
  static void mark(pbes_expression t)      { t.mark(); }
  static ATerm term(pbes_expression t)     { return t.term(); }
  static ATerm* ptr(pbes_expression& t)    { return &t.term(); }
};

} // namespace atermpp
/// \endcond

#endif // MCRL2_PBES_PBES_EXPRESSION_H
