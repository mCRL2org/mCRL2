// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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
#include "mcrl2/data/detail/data_sequence_algorithm.h"

namespace mcrl2::data
{

namespace detail
{
/// \post result is the optimized version of <tt>!arg</tt>
template <typename TermTraits>
inline
void optimized_not(typename TermTraits::term_type& result,
                   const typename TermTraits::term_type& arg,
                   TermTraits)
{
  using tr = TermTraits;

  if (tr::is_true(arg))
  {
    result=tr::false_();
  }
  else if (tr::is_false(arg))
  {
    result=tr::true_();
  }
  else if (tr::is_not(arg))
  {
    result=tr::not_arg(arg);
  }
  else
  {
    tr::make_not_(result, arg);
  }
}

/// \brief Make a conjunction and optimize it if possible.
/// \param left A term
/// \param right A term
/// \post result is the optimized version of <tt>left && right</tt>
template <typename TermTraits>
inline
void optimized_and(typename TermTraits::term_type& result,
                   const typename TermTraits::term_type& left,
                   const typename TermTraits::term_type& right,
                   TermTraits)
{
  using tr = TermTraits;

  if (tr::is_true(left))
  {
    result=right;
  }
  else if (tr::is_false(left))
  {
    result=tr::false_();
  }
  else if (tr::is_true(right))
  {
    result=left;
  }
  else if (tr::is_false(right))
  {
    result=tr::false_();
  }
  else if (left == right)
  {
    result=left;
  }
  else
  {
    tr::make_and_(result, left, right);
  }
}

/// \brief Make a disjunction
/// \param left A term
/// \param right A term
/// \post result is the optimized version of <tt>left || right</tt>
template <typename TermTraits>
inline
void optimized_or(typename TermTraits::term_type& result,
                  const typename TermTraits::term_type& left,
                  const typename TermTraits::term_type& right, TermTraits)
{
  using tr = TermTraits;

  if (tr::is_true(left))
  {
    result=tr::true_();
  }
  else if (tr::is_false(left))
  {
    result=right;
  }
  else if (tr::is_true(right))
  {
    result=tr::true_();
  }
  else if (tr::is_false(right))
  {
    result=left;
  }
  else if (left == right)
  {
    result=left;
  }
  else
  {
    tr::make_or_(result, left, right);
  }
}

/// \brief Make an implication
/// \param left A term
/// \param right A term
/// \post result is the optimized version of <tt>left => right</tt>
template <typename TermTraits>
inline
void optimized_imp(typename TermTraits::term_type& result,
                   const typename TermTraits::term_type& left,
                   const typename TermTraits::term_type& right, TermTraits t)
{
  using tr = TermTraits;

  if (tr::is_true(left))
  {
    result=right;
  }
  else if (tr::is_false(left))
  {
    result=tr::true_();
  }
  else if (tr::is_true(right))
  {
    result=tr::true_();
  }
  else if (tr::is_false(right))
  {
    optimized_not(result,left, t);
  }
  else if (left == right)
  {
    result=tr::true_();
  }
  else
  {
    tr::make_imp(result, left, right);
  }
}

/// \brief Make a universal quantification
/// \param v A sequence of variables
/// \param arg A term
////// \param remove_variables If true, remove bound variables that do not occur in \a arg.
/// \param empty_domain_allowed If true, and there are no variables in \a v, treat
///        as empty domain, hence yielding <tt>true</tt>, otherwise <tt>arg</tt> arg
///        is returned in this case.
/// \post result is the optimized version of <tt>forall v.arg</tt>
template <typename TermTraits>
inline
void optimized_forall(typename TermTraits::term_type& result,
                      const typename TermTraits::variable_sequence_type& v,
                      const typename TermTraits::term_type& arg,
                      bool remove_variables,
                      bool empty_domain_allowed, TermTraits)
{
  using tr = TermTraits;

  if (v.empty())
  {
    if (empty_domain_allowed)
    {
      result = tr::true_();
    }
    else
    {
      result = arg;
    }
  }
  else if (tr::is_true(arg))
  {
    result = tr::true_();
  }
  else if (tr::is_false(arg))
  {
    result = tr::false_();
  }
  else
  {
    if (remove_variables)
    {
      data::variable_list variables = data::detail::set_intersection(v, free_variables(arg));
      if (variables.empty())
      {
        result = arg;
      }
      else
      {
        tr::make_forall(result, variables, arg);
      }
    }
    else
    {
      tr::make_forall(result, v, arg);
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
/// \post result is the optimized version of <tt>exists v.arg</tt>
template <typename TermTraits>
inline
void optimized_exists(typename TermTraits::term_type& result,
                      const typename TermTraits::variable_sequence_type& v,
                      const typename TermTraits::term_type& arg,
                      bool remove_variables,
                      bool empty_domain_allowed,
                      TermTraits)
{
  using tr = TermTraits;

  if (v.empty())
  {
    if (empty_domain_allowed)
    {
      result = tr::false_();
    }
    else
    {
      result = arg;
    }
  }
  else if (tr::is_true(arg))
  {
    result = tr::true_();
  }
  else if (tr::is_false(arg))
  {
    result = tr::false_();
  }
  else
  {
    if (remove_variables)
    {
      data::variable_list variables = data::detail::set_intersection(v, free_variables(arg));
      if (variables.empty())
      {
        result = arg;
      }
      else
      {
        result = tr::exists(variables, arg);
      }
    }
    else
    {
      result = tr::exists(v, arg);
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

/// \brief Make a conjunction
/// \param left A term
/// \param right A term
/// \param and_ The operation and
/// \param true_ The value true
/// \param is_true Function that tests for the value true
/// \param false_ The value false
/// \param is_false Function that tests for the value false
/// \return The value <tt>left && right</tt>

/// \brief Make a disjunction
/// \param left A term
/// \param right A term
/// \param or_ The operation or
/// \param true_ The value true
/// \param is_true Function that tests for the value true
/// \param false_ The value false
/// \param is_false Function that tests for the value false
/// \return The value <tt>left || right</tt>

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

/// \brief Make a universal quantification
/// \param v A sequence of variables
/// \param arg A term
/// \param forall The universal quantification operator
/// \param true_ The value true
/// \param is_true Function that tests for the value true
/// \param false_ The value false
/// \param is_false Function that tests for the value false
/// \return The universal quantification <tt>forall v.arg</tt>

/// \brief Make an existential quantification
/// \param v A sequence of variables
/// \param arg A term
/// \param exists The existential quantification operator
/// \param true_ The value true
/// \param is_true Function that tests for the value true
/// \param false_ The value false
/// \param is_false Function that tests for the value false
/// \return The existential quantification <tt>exists v.arg</tt>

} // namespace detail

/// \brief Make a negation
/// \param arg A term
/// \post result is the optimized version of <tt>!arg</tt>
template <typename Term>
inline
void optimized_not(Term& result, const Term& arg)
{
  detail::optimized_not(result, arg, core::term_traits<Term>());
}

/// \brief Make a conjunction, and optimize if possible.
/// \param result Contains the optimized and.
/// \param p A term
/// \param q A term
/// \post result is the optimized version of <tt>p && q</tt>
template <typename Term>
inline
void optimized_and(Term& result, const Term& p, const Term& q)
{
  return detail::optimized_and(result, p, q, core::term_traits<Term>());
}

/* /// \brief Make a conjunction, and optimize if possible.
/// \param p A term
/// \param q A term
/// \return The application of and to the arguments.
template <typename Term>
inline
Term optimized_and(const Term& p, const Term& q)
{
  return detail::optimized_and(p, q, core::term_traits<Term>());
} */

/// \brief Make a disjunction
/// \param p A term
/// \param q A term
/// \post result is the optimized version of <tt>p || q</tt>
template <typename Term>
inline
void optimized_or(Term& result, const Term& p, const Term& q)
{
  detail::optimized_or(result, p, q, core::term_traits<Term>());
}

/// \brief Make an implication
/// \param p A term
/// \param q A term
/// \post result is the optimized version of <tt>p => q</tt>
template <typename Term>
inline
void optimized_imp(Term& result, const Term& p, const Term& q)
{
  detail::optimized_imp(result, p, q, core::term_traits<Term>());
}

/// \brief Make a universal quantification
/// \param l A sequence of variables
/// \param p A term
/// \param remove_variables If true, unused quantifier variables are removed
/// \post result is the optimized version of <tt>forall l.p</tt>
template <typename Term, typename VariableSequence>
inline
void optimized_forall(Term& result, const VariableSequence& l, const Term& p, bool remove_variables = false)
{
  bool empty_domain_allowed = true;
  detail::optimized_forall(result, l, p, remove_variables, empty_domain_allowed, core::term_traits<Term>());
}

/// \brief Make a universal quantification
/// \param l A sequence of variables
/// \param p A term
/// \param remove_variables If true, unused quantifier variables are removed
/// \post result is the optimized version of <tt>forall l.p</tt>
/// The optimization forall x:empty_set. phi = true is not applied.
template <typename Term, typename VariableSequence>
inline
void optimized_forall_no_empty_domain(Term& result, const VariableSequence& l, const Term& p, bool remove_variables = false)
{
  bool empty_domain_allowed = false;
  detail::optimized_forall(result, l, p, remove_variables, empty_domain_allowed, core::term_traits<Term>());
}

/// \brief Make an existential quantification
/// \param l A sequence of variables
/// \param p A term
/// \param remove_variables If true, unused quantifier variables are removed
/// \post result is the optimized version of <tt>exists l.p</tt>
template <typename Term, typename VariableSequence>
inline
void optimized_exists(Term& result, const VariableSequence& l, const Term& p, bool remove_variables = false)
{
  bool empty_domain_allowed = true;
  detail::optimized_exists(result, l, p, remove_variables, empty_domain_allowed, core::term_traits<Term>());
}

/// \brief Make an existential quantification
/// \param l A sequence of variables
/// \param p A term
/// \param remove_variables If true, unused quantifier variables are removed
/// \post result is the optimized version of <tt>exists l.p</tt>
/// The optimization exists x:empty_set. phi = false is not applied.
template <typename Term, typename VariableSequence>
inline
void optimized_exists_no_empty_domain(Term& result, const VariableSequence& l, const Term& p, bool remove_variables = false)
{
  bool empty_domain_allowed = false;
  detail::optimized_exists(result, l, p, remove_variables, empty_domain_allowed, core::term_traits<Term>());
}

} // namespace mcrl2::data

#endif // MCRL2_DATA_OPTIMIZED_BOOLEAN_OPERATORS_H
