// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/data_expression.h
/// \brief The class data_expression.

//#define MCRL2_DATA_EXPRESSION_DEBUG

#ifndef MCRL2_DATA_DATA_EXPRESSION_H
#define MCRL2_DATA_DATA_EXPRESSION_H

#include <iostream>
#include <string>
#include <cassert>
#include <stdexcept>
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/detail/join.h"
#include "mcrl2/core/term_traits.h"
#include "mcrl2/core/detail/optimized_logic_operators.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/sort_arrow.h"

namespace mcrl2 {

/// \brief The main namespace for the Data library.
namespace data {

// prototypes
class data_expression;

/// \cond INTERNAL_DOCS
namespace detail {
  bool check_data_application_sorts(atermpp::aterm_appl t);
}
/// \endcond

/// \brief singly linked list of data expressions
///
typedef atermpp::term_list<data_expression> data_expression_list;

///////////////////////////////////////////////////////////////////////////////
// data_expression
/// \brief Data expression
///
/// Represents a data expression or nil. The user must make sure that the
/// expression is not nil before using it.
class data_expression: public atermpp::aterm_appl
{
  protected:
    /// \brief This function checks if the data applications contained in this term
    /// are well defined, using the function data_application::check_sorts.
    void check_data_applications() const
    {
#ifdef MCRL2_DATA_EXPRESSION_DEBUG
      try
      {
        atermpp::for_each(*this, detail::check_data_application_sorts);
      }
      catch(std::runtime_error e)
      {
        std::cerr << e.what() << std::endl;
        assert(false);
      }
#endif
    }

  public:

    /// \brief Constructor.
    data_expression()
      : atermpp::aterm_appl(core::detail::constructDataExpr())
    {}

    /// \brief Constructor.
    /// \param term A term.
    data_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_DataExprOrNil(m_term));
      check_data_applications();
    }

    /// \brief Constructor.
    /// \param term A term.
    data_expression(ATermAppl term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_DataExprOrNil(m_term));
      check_data_applications();
    }

    /// \brief Returns the sort of the data expression.
    /// \return The sort of the data expression.
    sort_expression sort() const
    {
      ATermAppl result = core::detail::gsGetSort(*this);
      assert(!core::detail::gsIsSortUnknown(result));
      return sort_expression(result);
    }

    /// \brief Applies a substitution to this data expression and returns the result.
    /// The Substitution object must supply the method atermpp::aterm operator()(atermpp::aterm).
    /// \param f A substitution function.
    /// \return The application of the substitution to the expression.
    template <typename Substitution>
    data_expression substitute(Substitution f) const
    {
      return data_expression(f(atermpp::aterm(*this)));
    }
};

/// \brief Returns true if the term t is a data expression
/// \param t A term.
/// \return True if the term is a data expression.
inline
bool is_data_expression(atermpp::aterm_appl t)
{
  return core::detail::gsIsDataExpr(t);
}

/// \brief Accessor functions and predicates for data expressions.
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
  inline bool is_nil(atermpp::aterm_appl t) { return t == core::detail::gsMakeNil(); }

  /// \brief Returns true if the term t is equal to true
  inline bool is_true(atermpp::aterm_appl t) { return core::detail::gsIsDataExprTrue(t); }

  /// \brief Returns true if the term t is equal to false
  inline bool is_false(atermpp::aterm_appl t) { return core::detail::gsIsDataExprFalse(t); }

  /// \brief Returns true if the term t is a not expression
  inline bool is_not(atermpp::aterm_appl t) { return core::detail::gsIsDataExprNot(t); }

  /// \brief Returns true if the term t is an and expression
  inline bool is_and(atermpp::aterm_appl t) { return core::detail::gsIsDataExprAnd(t); }

  /// \brief Returns true if the term t is an or expression
  inline bool is_or(atermpp::aterm_appl t) { return core::detail::gsIsDataExprOr(t); }

  /// \brief Returns true if the term t is an implication
  inline bool is_imp(atermpp::aterm_appl t) { return core::detail::gsIsDataExprImp(t); }

  /// \brief Returns true if the term t is a 'binder'
  inline bool is_binder(atermpp::aterm_appl t) { return core::detail::gsIsBinder(t); }

  /// \brief Returns true if the term t is a where expression
  inline bool is_where(atermpp::aterm_appl t) { return core::detail::gsIsWhr(t); }

  /// \brief Returns true if the term t has type real
  inline bool is_real(atermpp::aterm_appl t) { return sort_expr::is_real(data_expression(t).sort()); }

  /// \brief Returns true if the term t has type int
  inline bool is_int(atermpp::aterm_appl t) { return sort_expr::is_int (data_expression(t).sort()); }

  /// \brief Returns true if the term t has type pos
  inline bool is_pos(atermpp::aterm_appl t) { return sort_expr::is_pos (data_expression(t).sort()); }

  /// \brief Returns true if the term t has type nat
  inline bool is_nat(atermpp::aterm_appl t) { return sort_expr::is_nat (data_expression(t).sort()); }

  /// \brief Returns true if the term t has type bool
  inline bool is_bool(atermpp::aterm_appl t) { return sort_expr::is_bool(data_expression(t).sort()); }

  // TODO: The following three functions do not belong here anymore, we need to find
  // a better place for these. Note that they have been changed to make sure
  // that we know we are working with a Binder before we take the binding operator.

  /// \brief Returns true if the term t is an existial quantification
  inline bool is_exists(atermpp::aterm_appl t) { return core::detail::gsIsBinder(t) && core::detail::gsIsExists(atermpp::aterm_appl(t(0))); }

  /// \brief Returns true if the term t is a universal quantification
  inline bool is_forall(atermpp::aterm_appl t) { return core::detail::gsIsBinder(t) && core::detail::gsIsForall(atermpp::aterm_appl(t(0))); }

  /// \brief Returns true if the term t is a lambda expression
  inline bool is_lambda(atermpp::aterm_appl t) { return core::detail::gsIsBinder(t) && core::detail::gsIsLambda(atermpp::aterm_appl(t(0))); }

  /// \brief Returns a real with value i
  // TODO: a function gsMakaDataExprReal_int will be added by Aad
  // Perhaps even real(double d) will become possible?
  inline
  data_expression real(int i)
  {
    return core::detail::gsMakeDataExprCReal(core::detail::gsMakeDataExprInt_int(i));
  }

  /// \brief Returns a pos with value i
  /// Precondition: i > 0
  inline
  data_expression pos(unsigned int i)
  {
    assert(i > 0);
    return core::detail::gsMakeDataExprPos_int(i);
  }

  /// \brief Returns a nat with value i
  inline
  data_expression nat(unsigned int i)
  {
    return core::detail::gsMakeDataExprNat_int(i);
  }

  /// \brief Returns an int with value i
  inline
  data_expression int_(int i)
  {
    return core::detail::gsMakeDataExprInt_int(i);
  }

  /// \brief Returns the expression true
  inline
  data_expression true_()
  {
    return data_expression(core::detail::gsMakeDataExprTrue());
  }

  /// \brief Returns the expression false
  inline
  data_expression false_()
  {
    return data_expression(core::detail::gsMakeDataExprFalse());
  }

  /// \brief Returns not applied to p
  /// This function contains optimizations for true and false arguments.
  inline
  data_expression not_(data_expression p)
  {
    return data_expression(core::detail::gsMakeDataExprNot(p));
  }

  /// \brief Returns and applied to p and q
  /// This function contains optimizations for true and false arguments.
  inline
  data_expression and_(data_expression p, data_expression q)
  {
    return data_expression(core::detail::gsMakeDataExprAnd(p,q));
  }

  /// \brief Returns or applied to p and q
  /// This function contains optimizations for true and false arguments.
  inline
  data_expression or_(data_expression p, data_expression q)
  {
    return data_expression(core::detail::gsMakeDataExprOr(p,q));
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
  inline bool is_negate(data_expression t) { return core::detail::gsIsDataExprNeg(t); }

  /// \brief Returns true if t is a plus expression
  inline bool is_plus(data_expression t) { return core::detail::gsIsDataExprAdd(t); }

  /// \brief Returns true if t is a minus expression
  inline bool is_minus(data_expression t) { return core::detail::gsIsDataExprSubt(t); }

  /// \brief Returns true if t is a multiply expression
  inline bool is_multiplies(data_expression t) { return core::detail::gsIsDataExprMult(t); }

  /// \brief Returns true if t is a division expression
  inline bool is_divides(data_expression t) { return core::detail::gsIsDataExprDiv(t); }

  /// \brief Returns true if t is a modulus expression
  inline bool is_modulus(data_expression t) { return core::detail::gsIsDataExprMod(t); }

  /// \brief Returns true if t is an equality expression
  inline bool is_equal_to(data_expression t) { return core::detail::gsIsDataExprEq(t); }

  /// \brief Returns true if t is an inequality expression
  inline bool is_not_equal_to(data_expression t) { return core::detail::gsIsDataExprNeq(t); }

  /// \brief Returns true if t is a less expression
  inline bool is_less(data_expression t) { return core::detail::gsIsDataExprLT(t); }

  /// \brief Returns true if t is a greater expression
  inline bool is_greater(data_expression t) { return core::detail::gsIsDataExprGT(t); }

  /// \brief Returns true if t is a less-equal expression
  inline bool is_less_equal(data_expression t) { return core::detail::gsIsDataExprLTE(t); }

  /// \brief Returns true if t is a greater-equal expression
  inline bool is_greater_equal(data_expression t) { return core::detail::gsIsDataExprGTE(t); }

  /// \brief Returns true if t is a min expression
  inline bool is_min(data_expression t) { return core::detail::gsIsDataExprMin(t); }

  /// \brief Returns true if t is a max expression
  inline bool is_max(data_expression t) { return core::detail::gsIsDataExprMax(t); }

  /// \brief Returns true if t is an abs expression
  inline bool is_abs(data_expression t) { return core::detail::gsIsDataExprAbs(t); }

  /// \brief Returns the expression d - e
  inline
  data_expression negate(data_expression d)
  {
    return core::detail::gsMakeDataExprNeg(d);
  }

  /// \brief Returns the expression d + e
  inline
  data_expression plus(data_expression d, data_expression e)
  {
    return core::detail::gsMakeDataExprAdd(d, e);
  }

  /// \brief Returns the expression d - e
  inline
  data_expression minus(data_expression d, data_expression e)
  {
    return core::detail::gsMakeDataExprSubt(d, e);
  }

  /// \brief Returns the expression d * e
  inline
  data_expression multiplies(data_expression d, data_expression e)
  {
    return core::detail::gsMakeDataExprMult(d, e);
  }

  /// \brief Returns the expression d / e
  inline
  data_expression divides(data_expression d, data_expression e)
  {
    return core::detail::gsMakeDataExprDiv(d, e);
  }

  /// \brief Returns the expression d % e
  inline
  data_expression modulus(data_expression d, data_expression e)
  {
    return core::detail::gsMakeDataExprMod(d, e);
  }

  /// \brief Returns the expression d = e
  inline
  data_expression equal_to(data_expression d, data_expression e)
  {
    return core::detail::gsMakeDataExprEq(d, e);
  }

  /// \brief Returns the expression d != e
  inline
  data_expression not_equal_to(data_expression d, data_expression e)
  {
    return core::detail::gsMakeDataExprNeq(d, e);
  }

  /// \brief Returns the expression d < e
  inline
  data_expression less(data_expression d, data_expression e)
  {
    return core::detail::gsMakeDataExprLT(d, e);
  }

  /// \brief Returns the expression d > e
  inline
  data_expression greater(data_expression d, data_expression e)
  {
    return core::detail::gsMakeDataExprGT(d, e);
  }

  /// \brief Returns the expression d <= e
  inline
  data_expression less_equal(data_expression d, data_expression e)
  {
    return core::detail::gsMakeDataExprLTE(d, e);
  }

  /// \brief Returns the expression d >= e
  inline
  data_expression greater_equal(data_expression d, data_expression e)
  {
    return core::detail::gsMakeDataExprGTE(d, e);
  }

  /// \brief Returns an expression for the minimum of d and e
  inline
  data_expression min_(data_expression d, data_expression e)
  {
    return core::detail::gsMakeDataExprMin(d, e);
  }

  /// \brief Returns an expression for the maximum of d and e
  inline
  data_expression max_(data_expression d, data_expression e)
  {
    return core::detail::gsMakeDataExprMax(d, e);
  }

  /// \brief Returns an expression for the absolute value of d
  inline
  data_expression abs(data_expression d)
  {
    return core::detail::gsMakeDataExprAbs(d);
  }

  /// \brief Returns the expression 'if i then t else e'
  inline
  data_expression if_(data_expression i, data_expression t, data_expression e)
  {
    return core::detail::gsMakeDataExprIf(i, t, e);
  }

  namespace optimized {
    /// \brief Returns not applied to p, and simplifies the result.
    inline
    data_expression not_(data_expression p)
    {
      namespace d = data_expr;
      return core::detail::optimized_not(p, d::not_, d::true_(), d::is_true, d::false_(), d::is_false);
    }

    /// \brief Returns and applied to p and q, and simplifies the result.
    inline
    data_expression and_(data_expression p, data_expression q)
    {
      namespace d = data_expr;
      return core::detail::optimized_and(p, q, d::and_, d::true_(), d::is_true, d::false_(), d::is_false);
    }

    /// \brief Returns or applied to p and q, and simplifies the result.
    inline
    data_expression or_(data_expression p, data_expression q)
    {
      namespace d = data_expr;
      return core::detail::optimized_or(p, q, d::or_, d::true_(), d::is_true, d::false_(), d::is_false);
    }

    /// \brief Returns the expression d = e
    inline
    data_expression equal_to(data_expression d, data_expression e)
    {
      if (d == e)
      {
        return data_expr::true_();
      }
      return core::detail::gsMakeDataExprEq(d, e);
    }

    /// \brief Returns the expression d != e
    inline
    data_expression not_equal_to(data_expression d, data_expression e)
    {
      if (d == e)
      {
        return data_expr::false_();
      }
      return core::detail::gsMakeDataExprNeq(d, e);
    }

    /// \brief Returns or applied to the sequence of data expressions [first, last)
    template <typename FwdIt>
    data_expression join_or(FwdIt first, FwdIt last)
    {
      namespace d = data_expr;
      return core::detail::join(first, last, optimized::or_, d::false_());
    }

    /// \brief Returns and applied to the sequence of data expressions [first, last)
    template <typename FwdIt>
    data_expression join_and(FwdIt first, FwdIt last)
    {
      namespace d = data_expr;
      return core::detail::join(first, last, optimized::and_, d::true_());
    }
  } // namespace optimized

} // namespace data_expr

/// \cond INTERNAL_DOCS
namespace detail {
  /// \brief Checks if the sorts of the arguments of a data application match
  /// with the head.
  /// \return True if the sorts match.
  //
  // Comments by Aad:
  // Bij het checken van een data expressie van de vorm
  //
  //   DataAppl(e, [e_0,...,e_n])
  //
  // dient te worden nagegaan of de sort van e matcht met die van
  // e_0,...,e_n. Dat wil zeggen dat als e_0,...,e_n sorts s_0,...,s_n
  // hebben, dan dient data expressie e sort
  //
  //   SortArrow([s_0,...,s_n], s)
  //
  inline
  bool check_data_application_sorts(atermpp::aterm_appl t)
  {
    if (core::detail::gsIsDataAppl(t))
    {
      data_expression f = atermpp::arg1(t);
      data_expression_list x = atermpp::list_arg2(t);
      if (!data::is_sort_arrow(f.sort()))
      {
        std::cerr << "f.sort() = " << core::pp(f.sort()) << std::endl;
        throw std::runtime_error("Error: ill-formed data_application detected (1): " + core::pp(t) + " " + core::pp(f.sort()));
      }
      sort_arrow fsort = f.sort();
      sort_expression_list s = fsort.argument_sorts();
      if (s.size() != x.size())
      {
        std::cerr << "s = " << core::pp(s) << " x = " << core::pp(x) << std::endl;
        throw std::runtime_error("Error: ill-formed data_application detected (2): " + core::pp(t) + " " + core::pp(fsort));
      }
      sort_expression_list::iterator i = s.begin();
      data_expression_list::iterator j = x.begin();
      for (; i != s.end(); ++i, ++j)
      {
        if (*i != j->sort())
        {
          std::cerr << "*i = " << core::pp(*i) << " j->sort() = " << core::pp(j->sort()) << std::endl;
          throw std::runtime_error("Error: ill-formed data_application detected (3): " + core::pp(t) + " " + core::pp(fsort));
        }
      }
    }
    return true;
  }
} // namespace detail
/// \endcond

} // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::data::data_expression)
/// \endcond

#endif // MCRL2_DATA_DATA_EXPRESSION_H
