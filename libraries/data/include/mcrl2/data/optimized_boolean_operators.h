// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/optimized_boolean_operators.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_OPTIMIZED_BOOLEAN_OPERATORS_H
#define MCRL2_DATA_OPTIMIZED_BOOLEAN_OPERATORS_H

#include "mcrl2/core/term_traits.h"

namespace mcrl2
{

namespace data
{

namespace detail
{
/// \return The value <tt>!arg</tt>
template <typename TermTraits>
inline
typename TermTraits::term_type optimized_not(const typename TermTraits::term_type& arg, TermTraits)
{
  typedef TermTraits tr;

  if (tr::is_true(arg))
  {
    return tr::false_();
  }
  else if (tr::is_false(arg))
  {
    return tr::true_();
  }
  else if (tr::is_not(arg))
  {
    return tr::not_arg(arg);
  }
  else
  {
    return tr::not_(arg);
  }
}

/// \brief Make a conjunction
/// \param left A term
/// \param right A term
/// \return The value <tt>left && right</tt>
template <typename TermTraits>
inline
typename TermTraits::term_type optimized_and(const typename TermTraits::term_type& left, const typename TermTraits::term_type& right, TermTraits)
{
  typedef TermTraits tr;

  if (tr::is_true(left))
  {
    return right;
  }
  else if (tr::is_false(left))
  {
    return tr::false_();
  }
  else if (tr::is_true(right))
  {
    return left;
  }
  else if (tr::is_false(right))
  {
    return tr::false_();
  }
  else if (left == right)
  {
    return left;
  }
  else
  {
    return tr::and_(left, right);
  }
}

/// \brief Make a disjunction
/// \param left A term
/// \param right A term
/// \return The value <tt>left || right</tt>
template <typename TermTraits>
inline
typename TermTraits::term_type optimized_or(const typename TermTraits::term_type& left, const typename TermTraits::term_type& right, TermTraits)
{
  typedef TermTraits tr;

  if (tr::is_true(left))
  {
    return tr::true_();
  }
  else if (tr::is_false(left))
  {
    return right;
  }
  else if (tr::is_true(right))
  {
    return tr::true_();
  }
  else if (tr::is_false(right))
  {
    return left;
  }
  else if (left == right)
  {
    return left;
  }
  else
  {
    return tr::or_(left, right);
  }
}

/// \brief Make an implication
/// \param left A term
/// \param right A term
/// \return The value <tt>left => right</tt>
template <typename TermTraits>
inline
typename TermTraits::term_type optimized_imp(const typename TermTraits::term_type& left, const typename TermTraits::term_type& right, TermTraits)
{
  typedef TermTraits tr;

  if (tr::is_true(left))
  {
    return right;
  }
  else if (tr::is_false(left))
  {
    return tr::true_();
  }
  else if (tr::is_true(right))
  {
    return tr::true_();
  }
  else if (tr::is_false(right))
  {
    return tr::not_(left);
  }
  else if (left == right)
  {
    return tr::true_();
  }
  else
  {
    return tr::imp(left, right);
  }
}

/// \brief Make a universal quantification
/// \param v A sequence of variables
/// \param arg A term
////// \param remove_variables If true, remove bound variables that do not occur in \a arg.
/// \param empty_domain_allowed If true, and there are no variables in \a v, treat
///        as empty domain, hence yielding <tt>true</tt>, otherwise <tt>arg</tt> arg
///        is returned in this case.
/// \return The universal quantification <tt>forall v.arg</tt>
template <typename TermTraits>
inline
typename TermTraits::term_type optimized_forall(const typename TermTraits::variable_sequence_type& v, const typename TermTraits::term_type& arg, bool remove_variables, bool empty_domain_allowed, TermTraits)
{
  typedef TermTraits tr;

  if (v.empty())
  {
    if (empty_domain_allowed)
    {
      return tr::true_();
    }
    else
    {
      return arg;
    }
  }
  else if (tr::is_true(arg))
  {
    return tr::true_();
  }
  else if (tr::is_false(arg))
  {
    return tr::false_();
  }
  else
  {
    if (remove_variables)
    {
      typename tr::variable_sequence_type variables = tr::set_intersection(v, tr::free_variables(arg));
      if (variables.empty())
      {
        return arg;
      }
      else
      {
        return tr::forall(variables, arg);
      }
    }
    else
    {
      return tr::forall(v, arg);
    }
  }
}

/// \brief Make an existential quantification
/// \param v A sequence of variables
/// \param arg A term
/// \param remove_variables If true, remove bound variables that do not occur in \a arg.
/// \param empty_domain_allowed If true, and there are no variables in \a v, treat
///        as empty domain, hence yielding <tt>false</tt>, otherwise <tt>arg</tt> arg
///        is returned in this case.
/// \return The existential quantification <tt>exists v.arg</tt>
template <typename TermTraits>
inline
typename TermTraits::term_type optimized_exists(const typename TermTraits::variable_sequence_type& v, const typename TermTraits::term_type& arg, bool remove_variables, bool empty_domain_allowed, TermTraits)
{
  typedef TermTraits tr;

  if (v.empty())
  {
    if (empty_domain_allowed)
    {
      return tr::false_();
    }
    else
    {
      return arg;
    }
  }
  else if (tr::is_true(arg))
  {
    return tr::true_();
  }
  else if (tr::is_false(arg))
  {
    return tr::false_();
  }
  else
  {
    if (remove_variables)
    {
      typename tr::variable_sequence_type variables = tr::set_intersection(v, tr::free_variables(arg));
      if (variables.empty())
      {
        return arg;
      }
      else
      {
        return tr::exists(variables, arg);
      }
    }
    else
    {
      return tr::exists(v, arg);
    }
  }
}

/// \brief Make a negation
/// \param arg A term
/// \param not_ The operation not
/// \param true_ The value true
/// \param is_true Function that tests for the value true
/// \param false_ The value false
/// \param is_false Function that tests for the value false
/// \return The value <tt>!arg</tt>
template <typename T1, typename T2, typename UnaryFunction, typename UnaryPredicate>
inline
T1 optimized_not(T1 arg, UnaryFunction not_, T2 true_, UnaryPredicate is_true, T2 false_, UnaryPredicate is_false)
{
  if (is_true(arg))
  {
    return false_;
  }
  else if (is_false(arg))
  {
    return true_;
  }
  else
  {
    return not_(arg);
  }
}

/// \brief Make a conjunction
/// \param left A term
/// \param right A term
/// \param and_ The operation and
/// \param true_ The value true
/// \param is_true Function that tests for the value true
/// \param false_ The value false
/// \param is_false Function that tests for the value false
/// \return The value <tt>left && right</tt>
template <typename T1, typename T2, typename UnaryPredicate, typename BinaryFunction>
inline
T1 optimized_and(T1 left, T1 right, BinaryFunction and_, T2 /* true_ */, UnaryPredicate is_true, T2 false_, UnaryPredicate is_false)
{
  if (is_true(left))
  {
    return right;
  }
  else if (is_false(left))
  {
    return false_;
  }
  else if (is_true(right))
  {
    return left;
  }
  else if (is_false(right))
  {
    return false_;
  }
  else if (left == right)
  {
    return left;
  }
  else
  {
    return and_(left, right);
  }
}

/// \brief Make a disjunction
/// \param left A term
/// \param right A term
/// \param or_ The operation or
/// \param true_ The value true
/// \param is_true Function that tests for the value true
/// \param false_ The value false
/// \param is_false Function that tests for the value false
/// \return The value <tt>left || right</tt>
template <typename T1, typename T2, typename UnaryPredicate, typename BinaryFunction>
inline
T1 optimized_or(T1 left, T1 right, BinaryFunction or_, T2 true_, UnaryPredicate is_true, T2 /* false_ */, UnaryPredicate is_false)
{
  if (is_true(left))
  {
    return true_;
  }
  else if (is_false(left))
  {
    return right;
  }
  else if (is_true(right))
  {
    return true_;
  }
  else if (is_false(right))
  {
    return left;
  }
  else if (left == right)
  {
    return left;
  }
  else
  {
    return or_(left, right);
  }
}

/// \brief Make an implication
/// \param left A term
/// \param right A term
/// \param imp The implication operator
/// \param not_ The operation not
/// \param true_ The value true
/// \param is_true Function that tests for the value true
/// \param false_ The value false
/// \param is_false Function that tests for the value false
/// \return The value <tt>left => right</tt>
template <typename T1, typename T2, typename UnaryPredicate, typename UnaryFunction, typename BinaryFunction>
inline
T1 optimized_imp(T1 left, T1 right, BinaryFunction imp, UnaryFunction not_, T2 true_, UnaryPredicate is_true, T2 /* false_ */, UnaryPredicate is_false)
{
  if (is_true(left))
  {
    return right;
  }
  else if (is_false(left))
  {
    return true_;
  }
  else if (is_true(right))
  {
    return true_;
  }
  else if (is_false(right))
  {
    return not_(left);
  }
  else if (left == right)
  {
    return true_;
  }
  else
  {
    return imp(left, right);
  }
}

/// \brief Make a universal quantification
/// \param v A sequence of variables
/// \param arg A term
/// \param forall The universal quantification operator
/// \param true_ The value true
/// \param is_true Function that tests for the value true
/// \param false_ The value false
/// \param is_false Function that tests for the value false
/// \return The universal quantification <tt>forall v.arg</tt>
template <typename T1, typename T2, typename VariableSequence, typename UnaryPredicate, typename Forall>
inline
T1 optimized_forall(VariableSequence v, T1 arg, Forall forall, T2 true_, UnaryPredicate is_true, T2 false_, UnaryPredicate is_false)
{
  if (is_true(arg))
  {
    return true_;
  }
  else if (is_false(arg))
  {
    return false_;
  }
  else
  {
    return forall(v, arg);
  }
}

/// \brief Make an existential quantification
/// \param v A sequence of variables
/// \param arg A term
/// \param exists The existential quantification operator
/// \param true_ The value true
/// \param is_true Function that tests for the value true
/// \param false_ The value false
/// \param is_false Function that tests for the value false
/// \return The existential quantification <tt>exists v.arg</tt>
template <typename T1, typename T2, typename VariableSequence, typename UnaryPredicate, typename Exists>
inline
T1 optimized_exists(VariableSequence v, T1 arg, Exists exists, T2 true_, UnaryPredicate is_true, T2 false_, UnaryPredicate is_false)
{
  if (is_true(arg))
  {
    return true_;
  }
  else if (is_false(arg))
  {
    return false_;
  }
  else
  {
    return exists(v, arg);
  }
}

} // namespace detail

/// \brief Make a negation
/// \param arg A term
/// \return The application of not to the argument.
template <typename Term>
inline
Term optimized_not(const Term& arg)
{
  return detail::optimized_not(arg, core::term_traits<Term>());
}

/// \brief Make a conjunction
/// \param p A term
/// \param q A term
/// \return The application of and to the arguments.
template <typename Term>
inline
Term optimized_and(const Term& p, const Term& q)
{
  return detail::optimized_and(p, q, core::term_traits<Term>());
}

/// \brief Make a disjunction
/// \param p A term
/// \param q A term
/// \return The application of or to the arguments.
template <typename Term>
inline
Term optimized_or(const Term& p, const Term& q)
{
  return detail::optimized_or(p, q, core::term_traits<Term>());
}

/// \brief Make an implication
/// \param p A term
/// \param q A term
/// \return The application of implication to the arguments.
template <typename Term>
inline
Term optimized_imp(const Term& p, const Term& q)
{
  return detail::optimized_imp(p, q, core::term_traits<Term>());
}

/// \brief Make a universal quantification
/// \param l A sequence of variables
/// \param p A term
/// \param remove_variables If true, unused quantifier variables are removed
/// \return The application of universal quantification to the arguments.
template <typename Term, typename VariableSequence>
inline
Term optimized_forall(const VariableSequence& l, const Term& p, bool remove_variables = false)
{
  bool empty_domain_allowed = true;
  return detail::optimized_forall(l, p, remove_variables, empty_domain_allowed, core::term_traits<Term>());
}

/// \brief Make a universal quantification
/// \param l A sequence of variables
/// \param p A term
/// \param remove_variables If true, unused quantifier variables are removed
/// \return The application of universal quantification to the arguments.
/// The optimization forall x:empty_set. phi = true is not applied.
template <typename Term, typename VariableSequence>
inline
Term optimized_forall_no_empty_domain(const VariableSequence& l, const Term& p, bool remove_variables = false)
{
  bool empty_domain_allowed = false;
  return detail::optimized_forall(l, p, remove_variables, empty_domain_allowed, core::term_traits<Term>());
}

/// \brief Make an existential quantification
/// \param l A sequence of variables
/// \param p A term
/// \param remove_variables If true, unused quantifier variables are removed
/// \return The application of existential quantification to the arguments.
template <typename Term, typename VariableSequence>
inline
Term optimized_exists(const VariableSequence& l, const Term& p, bool remove_variables = false)
{
  bool empty_domain_allowed = true;
  return detail::optimized_exists(l, p, remove_variables, empty_domain_allowed, core::term_traits<Term>());
}

/// \brief Make an existential quantification
/// \param l A sequence of variables
/// \param p A term
/// \param remove_variables If true, unused quantifier variables are removed
/// \return The application of existential quantification to the arguments.
/// The optimization exists x:empty_set. phi = false is not applied.
template <typename Term, typename VariableSequence>
inline
Term optimized_exists_no_empty_domain(const VariableSequence& l, const Term& p, bool remove_variables = false)
{
  bool empty_domain_allowed = false;
  return detail::optimized_exists(l, p, remove_variables, empty_domain_allowed, core::term_traits<Term>());
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_OPTIMIZED_BOOLEAN_OPERATORS_H
