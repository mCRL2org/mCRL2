// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_expression.h
/// \brief The class pbes_expression.

#ifndef MCRL2_PBES_PBES_EXPRESSION_H
#define MCRL2_PBES_PBES_EXPRESSION_H

#include <iterator>
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/data/data_variable.h"
#include "mcrl2/core/detail/join.h"
#include "mcrl2/core/detail/optimized_logic_operators.h"
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

  /// \brief Returns true if the term t is equal to true
  inline bool is_pbes_true(pbes_expression t) { return core::detail::gsIsPBESTrue(t); }

  /// \brief Returns true if the term t is equal to false
  inline bool is_pbes_false(pbes_expression t) { return core::detail::gsIsPBESFalse(t); }

  /// \brief Returns true if the term t is a not expression
  inline bool is_pbes_not(pbes_expression t) { return core::detail::gsIsPBESNot(t); }

  /// \brief Returns true if the term t is an and expression
  inline bool is_pbes_and(pbes_expression t) { return core::detail::gsIsPBESAnd(t); }

  /// \brief Returns true if the term t is an or expression
  inline bool is_pbes_or(pbes_expression t) { return core::detail::gsIsPBESOr(t); }

  /// \brief Returns true if the term t is an imp expression
  inline bool is_pbes_imp(pbes_expression t) { return core::detail::gsIsPBESImp(t); }

  /// \brief Returns true if the term t is a universal quantification
  inline bool is_pbes_forall(pbes_expression t) { return core::detail::gsIsPBESForall(t); }

  /// \brief Returns true if the term t is an existential quantification
  inline bool is_pbes_exists(pbes_expression t) { return core::detail::gsIsPBESExists(t); }

  /// \brief Returns true if the term t is equal to true
  inline bool is_true(pbes_expression t) { return is_pbes_true(t) || data::data_expr::is_true(t); }

  /// \brief Returns true if the term t is equal to false
  inline bool is_false(pbes_expression t) { return is_pbes_false(t) || data::data_expr::is_false(t); }

  /// \brief Returns true if the term t is a not expression
  inline bool is_not(pbes_expression t) { return is_pbes_not(t) || data::data_expr::is_not(t); }

  /// \brief Returns true if the term t is an and expression
  inline bool is_and(pbes_expression t) { return is_pbes_and(t) || data::data_expr::is_and(t); }

  /// \brief Returns true if the term t is an or expression
  inline bool is_or(pbes_expression t) { return is_pbes_or(t) || data::data_expr::is_or(t); }

  /// \brief Returns true if the term t is an imp expression
  inline bool is_imp(pbes_expression t) { return is_pbes_imp(t) || data::data_expr::is_imp(t); }

  /// \brief Returns true if the term t is a universal quantification
  inline bool is_forall(pbes_expression t) { return is_pbes_forall(t); }

  /// \brief Returns true if the term t is an existential quantification
  inline bool is_exists(pbes_expression t) { return is_pbes_exists(t); }

  /// \brief Returns true if the term t is a data expression
  inline bool is_data(pbes_expression t) { return core::gsIsDataExpr(t); }

  /// \brief Returns true if the term t is a propositional variable expression
  inline bool is_propositional_variable_instantiation(pbes_expression t) { return core::detail::gsIsPropVarInst(t); }

} // namespace pbes_expr

namespace accessors {

  /// Conversion of a pbes expression to a data expression.
  /// \pre The pbes expression must be of the form val(d) for
  /// some data variable d.
  inline
  data::data_expression val(pbes_expression t)
  {
    assert(core::gsIsDataExpr(t));
    return aterm_appl(t);
  }

  /// \brief Returns the pbes expression argument of expressions of type not,
  /// exists and forall.
  inline
  pbes_expression arg(pbes_expression t)
  {
    if (pbes_expr::is_pbes_not(t))
    {
      return arg1(t);
    }
    assert(data::data_expr::is_not(t) ||
           pbes_expr::is_forall(t)    ||
           pbes_expr::is_exists(t)
          );
    return arg2(t); 
  }

  /// \brief Returns the left hand side of an expression of type and, or or imp.
  inline
  pbes_expression left(pbes_expression t)
  {
    assert(pbes_expr::is_and(t) || pbes_expr::is_or(t) || pbes_expr::is_imp(t));
    return arg1(t);
  }
  
  /// \brief Returns the right hand side of an expression of type and, or or imp.
  inline
  pbes_expression right(pbes_expression t)
  {
    assert(pbes_expr::is_and(t) || pbes_expr::is_or(t) || pbes_expr::is_imp(t));
    return arg2(t);
  }
  
  /// \brief Returns the variables of a quantification expression
  inline
  data::data_variable_list var(pbes_expression t)
  {
    assert(pbes_expr::is_forall(t) || pbes_expr::is_exists(t));
    return list_arg1(t);
  }
  
  /// \brief Returns the name of a propositional variable expression
  inline
  core::identifier_string name(pbes_expression t)
  {
    assert(pbes_expr::is_propositional_variable_instantiation(t));
    return arg1(t);
  }
  
  /// \brief Returns the parameters of a propositional variable instantiation.
  inline
  data::data_expression_list param(pbes_expression t)
  {
    assert(pbes_expr::is_propositional_variable_instantiation(t));
    return list_arg2(t);
  }
} // accessors

/// Accessor functions and predicates for pbes expressions.
namespace pbes_expr {

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
  
  /// \brief Returns the universal quantification of the expression p over the variables in l.
  inline
  pbes_expression forall(data::data_variable_list l, pbes_expression p)
  {
    // TODO: Due to a bug in the gs library this check has to be added.
    if (l.empty())
    {
      return p;
    }
    return pbes_expression(core::detail::gsMakePBESForall(l, p));
  }
  
  /// \brief Returns the existential quantification of the expression p over the variables in l.
  inline
  pbes_expression exists(data::data_variable_list l, pbes_expression p)
  {
    // TODO: Due to a bug in the gs library this check has to be added.
    if (l.empty())
    {
      return p;
    }
    return pbes_expression(core::detail::gsMakePBESExists(l, p));
  }

  /// \brief Returns or applied to the sequence of pbes expressions [first, last[
  template <typename FwdIt>
  pbes_expression join_or(FwdIt first, FwdIt last)
  {
    return core::detail::join(first, last, or_, false_());
  }
  
  /// \brief Returns and applied to the sequence of pbes expressions [first, last[
  template <typename FwdIt>
  pbes_expression join_and(FwdIt first, FwdIt last)
  {
    return core::detail::join(first, last, and_, true_());
  }

  /// Given a pbes expression of the form p1 || p2 || .... || pn, this will yield a 
  /// set of the form { p1, p2, ..., pn }, assuming that pi does not have a || as main 
  /// function symbol.
  inline
  atermpp::set<pbes_expression> split_or(const pbes_expression& expr)
  {
    using namespace accessors;
    atermpp::set<pbes_expression> result;
    core::detail::split(expr, std::insert_iterator<atermpp::set<pbes_expression> >(result, result.begin()), is_or, left, right);
    return result;
  }
  
  /// Given a pbes expression of the form p1 && p2 && .... && pn, this will yield a 
  /// set of the form { p1, p2, ..., pn }, assuming that pi does not have a && as main 
  /// function symbol.
  inline
  atermpp::set<pbes_expression> split_and(const pbes_expression& expr)
  {
    using namespace accessors;
    atermpp::set<pbes_expression> result;
    core::detail::split(expr, std::insert_iterator<atermpp::set<pbes_expression> >(result, result.begin()), is_and, left, right);
    return result;
  }
} // namespace pbes_expr

namespace pbes_expr_optimized {
  using pbes_expr::is_pbes_true;
  using pbes_expr::is_pbes_false;
  using pbes_expr::is_pbes_not;
  using pbes_expr::is_pbes_and;
  using pbes_expr::is_pbes_or;
  using pbes_expr::is_pbes_imp;
  using pbes_expr::is_pbes_forall;
  using pbes_expr::is_pbes_exists;
  using pbes_expr::is_true;
  using pbes_expr::is_false;
  using pbes_expr::is_not;
  using pbes_expr::is_and;
  using pbes_expr::is_or;
  using pbes_expr::is_imp;
  using pbes_expr::is_forall;
  using pbes_expr::is_exists;
  using pbes_expr::is_data;
  using pbes_expr::is_propositional_variable_instantiation;
  using pbes_expr::true_;
  using pbes_expr::false_;
  using pbes_expr::split_and;
  using pbes_expr::split_or;

  /// \brief Returns not applied to p, and simplifies the result.
  inline
  pbes_expression not_(pbes_expression p)
  {
    return core::detail::optimized_not(p, pbes_expr::not_, true_(), is_true, false_(), is_false);
  }
  
  /// \brief Returns and applied to p and q, and simplifies the result.
  inline
  pbes_expression and_(pbes_expression p, pbes_expression q)
  {
    return core::detail::optimized_and(p, q, pbes_expr::and_, true_(), is_true, false_(), is_false);
  }
  
  /// \brief Returns or applied to p and q, and simplifies the result.
  inline
  pbes_expression or_(pbes_expression p, pbes_expression q)
  {
    return core::detail::optimized_or(p, q, pbes_expr::or_, true_(), is_true, false_(), is_false);
  }
  
  /// \brief Returns imp applied to p and q, and simplifies the result.
  inline
  pbes_expression imp(pbes_expression p, pbes_expression q)
  {
    return core::detail::optimized_imp(p, q, pbes_expr::imp, not_, true_(), is_true, false_(), is_false);
  }

  /// \brief Returns or applied to the sequence of pbes expressions [first, last[
  template <typename FwdIt>
  inline pbes_expression join_or(FwdIt first, FwdIt last)
  {
    return core::detail::join(first, last, or_, false_());
  }
  
  /// \brief Returns and applied to the sequence of pbes expressions [first, last[
  template <typename FwdIt>
  inline pbes_expression join_and(FwdIt first, FwdIt last)
  {
    return core::detail::join(first, last, and_, true_());
  }

  /// \brief Returns the universal quantification of the expression p over the variables in l.
  /// If l is empty, p is returned.
  inline
  pbes_expression forall(data::data_variable_list l, pbes_expression p)
  {
    if (l.empty())
    {
      return p;
    }
    return pbes_expr::forall(l, p);
  }
  
  /// \brief Returns the existential quantification of the expression p over the variables in l.
  /// If l is empty, p is returned.
  inline
  pbes_expression exists(data::data_variable_list l, pbes_expression p)
  {
    if (l.empty())
    {
      return p;
    }
    return pbes_expr::exists(l, p);
  }

} // namespace pbes_expr_optimized

/// \brief Returns true if the pbes expression t is a boolean expression
inline
bool is_bes(aterm_appl t)
{
  using namespace pbes_expr;
  using namespace accessors;

  if(is_pbes_and(t)) {
    return is_bes(left(t)) && is_bes(right(t));
  }
  else if(is_pbes_or(t)) {
    return is_bes(left(t)) && is_bes(right(t));
  }
  else if(is_pbes_forall(t)) {
    return false;
  }
  else if(is_pbes_exists(t)) {
    return false;
  }
  else if(is_propositional_variable_instantiation(t)) {
    return propositional_variable_instantiation(t).parameters().empty();
  }
  else if(is_pbes_true(t)) {
    return true;
  }
  else if(is_pbes_false(t)) {
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
