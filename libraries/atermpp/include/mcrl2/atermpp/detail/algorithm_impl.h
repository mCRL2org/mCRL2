// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/detail/algorithm_impl.h
/// \brief Implementations of algorithms.

#ifndef MCRL2_ATERMPP_DETAIL_ALGORITHM_IMPL_H
#define MCRL2_ATERMPP_DETAIL_ALGORITHM_IMPL_H

#include <iterator>
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"

namespace atermpp
{

namespace detail
{
/// \brief Applies the function f to all children of a.
/// \param a A term
/// \param f A function on terms
/// \return The transformed term
template <typename Term, typename Function>
aterm_appl appl_apply(const term_appl<Term>& a, const Function f)
{
  return term_appl<Term>(a.function(), a.begin(), a.end(), f);
}

//--- find ----------------------------------------------------------------//

// we need to use our own traits classes to extract the value type from insert iterators
template <class Iterator>
struct iterator_value
{
  typedef typename std::iterator_traits<Iterator>::value_type type;
};

template <class Container>
struct iterator_value<std::insert_iterator<Container> >
{
  typedef typename Container::value_type type;
};

template <class Container>
struct iterator_value<std::back_insert_iterator<Container> >
{
  typedef typename Container::value_type type;
};

template <class Container>
struct iterator_value<std::front_insert_iterator<Container> >
{
  typedef typename Container::value_type type;
};

/// \internal
// used to abort the recursive find
struct found_term_exception
{
  aterm_appl t;

  found_term_exception(const aterm_appl& t_)
    : t(t_)
  {}
};

/// \brief Implements the for_each algorithm
/// \param t A term
/// \param op A unary function on terms
/// \return The result of the algorithm
template <typename UnaryFunction>
UnaryFunction for_each_impl(aterm t, UnaryFunction op)
{
  if (t.type_is_list())
  {
    const aterm_list& l = down_cast<aterm_list>(t);
    for (const aterm& x: l)
    {
      for_each_impl(x, op);
    }
  }
  else if (t.type_is_appl())
  {
    const aterm_appl& a = down_cast<aterm_appl>(t);
    if (op(t))
    {
      for (const aterm& x: a)
      {
        for_each_impl(x, op);
      }
    }
  }
  return op;
}

/// \brief Implements the find_if algorithm
/// If the term t is found, it is stored in output and true is returned
/// \param t A term
/// \param match A predicate function on terms
/// \param output The variable to store the match in
/// \return true if a match was found, false otherwise
template <typename MatchPredicate>
bool find_if_impl(const aterm& t, MatchPredicate match, aterm_appl& output)
{
  if (t.type_is_appl())
  {
    const aterm_appl& a = down_cast<aterm_appl>(t);
    if (match(a))
    {
      output = a;
      return true;
    }
    for (const aterm& x: a)
    {
      if (find_if_impl(x, match, output))
        return true;
    }
  }
  else if (t.type_is_list())
  {
    const aterm_list& l = down_cast<aterm_list>(t);
    for (const aterm& x: l)
    {
      if (find_if_impl(x, match, output))
      {
        return true;
      }
    }
  }
  return false;
}

/// \brief Implements the find_all_if algorithm
/// \param t A term
/// \param op A predicate function on terms
/// \param destBegin The beginning of a range to where the results are written
template <typename MatchPredicate, typename OutputIterator>
void find_all_if_impl(const aterm& t, MatchPredicate op, OutputIterator& destBegin)
{
  typedef typename iterator_value<OutputIterator>::type value_type;

  if (t.type_is_list())
  {
    const aterm_list& l = down_cast<aterm_list>(t);
    for (const aterm& x: l)
    {
      find_all_if_impl<MatchPredicate>(x, op, destBegin);
    }
  }
  else if (t.type_is_appl())
  {
    const aterm_appl& a = down_cast<aterm_appl>(t);
    if (op(a))
    {
      *destBegin++ = vertical_cast<value_type>(a);
    }
    for (const aterm& x: a)
    {
      find_all_if_impl<MatchPredicate>(x, op, destBegin);
    }
  }
  else
  {
    return;
  }
}

//--- partial find --------------------------------------------------------//

/// \brief Implements the partial_find_if_impl algorithm
/// \param t A term
/// \param match A predicate function on terms
/// \param stop A predicate function on terms
template <typename MatchPredicate, typename StopPredicate>
aterm_appl partial_find_if_impl(const aterm& t, MatchPredicate match, StopPredicate stop)
{
  if (t.type_is_appl())
  {
    const aterm_appl& a = down_cast<aterm_appl>(t);
    if (match(a))
    {
      return a; // report the match
    }
    if (stop(a))
    {
      return aterm_appl(); // nothing was found
    }
    for (const aterm& x: a)
    {
      aterm_appl result = partial_find_if_impl<MatchPredicate, StopPredicate>(x, match, stop);
      if (result != aterm_appl())
      {
        return result;
      }
    }
  }

  if (t.type_is_list())
  {
    const aterm_list& l = down_cast<aterm_list>(t);
    for (const aterm& x: l)
    {
      aterm_appl result = partial_find_if_impl<MatchPredicate, StopPredicate>(x, match, stop);
      if (result != aterm_appl())
      {
        return result;
      }
    }
  }
  return aterm_appl();
}

/// \brief Implements the partial_find_all_if algorithm
/// \param t A term
/// \param match A predicate function on terms
/// \param stop A predicate function on terms
/// \param destBegin The beginning of a range to where the results are written
template <typename MatchPredicate, typename StopPredicate, typename OutputIterator>
void partial_find_all_if_impl(const aterm& t, MatchPredicate match, StopPredicate stop, OutputIterator& destBegin)
{
  if (t.type_is_appl())
  {
    const aterm_appl& a = down_cast<aterm_appl>(t);
    if (match(a))
    {
      *destBegin++ = aterm_appl(t);
    }
    if (stop(a))
    {
      return;
    }
    for (const aterm& x: a)
    {
      partial_find_all_if_impl<MatchPredicate, StopPredicate>(x, match, stop, destBegin);
    }
  }

  if (t.type_is_list())
  {
    const aterm_list& l = down_cast<aterm_list>(t);
    for (const aterm& x: l)
    {
      partial_find_all_if_impl<MatchPredicate, StopPredicate>(x, match, stop, destBegin);
    }
  }
}

} // namespace detail

} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ALGORITHM_IMPL_H
