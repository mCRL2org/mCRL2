// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/optimized_boolean_operators.h
/// \brief add your file description here.

#ifndef MCRL2_UTILITIES_OPTIMIZED_BOOLEAN_OPERATORS_H
#define MCRL2_UTILITIES_OPTIMIZED_BOOLEAN_OPERATORS_H

#include "mcrl2/core/term_traits.h"

namespace mcrl2
{

namespace utilities
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
/// \return The application of universal quantification to the arguments.
template <typename Term, typename VariableSequence>
inline
Term optimized_forall(const VariableSequence& l, const Term& p, bool remove_variables = false, bool empty_domain_allowed = true)
{
  return detail::optimized_forall(l, p, remove_variables, empty_domain_allowed, core::term_traits<Term>());
}

/// \brief Make an existential quantification
/// \param l A sequence of variables
/// \param p A term
/// \return The application of existential quantification to the arguments.
template <typename Term, typename VariableSequence>
inline
Term optimized_exists(const VariableSequence& l, const Term& p, bool remove_variables = false, bool empty_domain_allowed = true)
{
  return detail::optimized_exists(l, p, remove_variables, empty_domain_allowed, core::term_traits<Term>());
}

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_OPTIMIZED_BOOLEAN_OPERATORS_H
