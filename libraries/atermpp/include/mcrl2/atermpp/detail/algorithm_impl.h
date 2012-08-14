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

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/utilities/detail/memory_utility.h"

namespace atermpp
{

namespace detail
{
/// \brief Applies the function f to all elements of the list and returns the result.
/// \param l A sequence of terms
/// \param f A function on terms
/// \return The transformed sequence
template <typename Term, typename Function>
aterm_list list_apply(term_list<Term> l, const Function f)
{
  if (l.size() == 0)
  {
    return l;
  }
  aterm_list result;
  for (typename term_list<Term>::iterator i = l.begin(); i != l.end(); ++i)
  {
    result = push_front(result, aterm(f(*i)));
  }
  return reverse(result);
}

// template <typename Term, typename Function>
// aterm_appl appl_apply(term_appl<Term> a, const Function f)
// {
//   aterm_appl t = a;
//   unsigned int n = t.size();
//   if (n > 0)
//   {
//     for (unsigned int i = 0; i < n; i++)
//     {
//       aterm ti = t(i);
//       aterm fi = f(ti);
//       if (fi != ti)
//         t = t.set_argument(fi, i);
//     }
//   }
//   return t;
// }

/// \brief Applies the function f to all children of a.
/// \param a A term
/// \param f A function on terms
/// \return The transformed term
template <typename Term, typename Function>
aterm_appl appl_apply(term_appl<Term> a, const Function f)
{
  size_t n = a.size();
  if (n > 0)
  {
    bool term_changed = false;

    MCRL2_SYSTEM_SPECIFIC_ALLOCA(t,ATerm,n);

    for (unsigned int i = 0; i < n; i++)
    {
      t[i] = f(a(i));

      if (t[i] != a(i))
      {
        term_changed = true;
      }
    }
    if (term_changed)
    {
      a = ATmakeApplArray(a.function(), t);
    }
  }
  return a;
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

  found_term_exception(aterm_appl t_)
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
  if (t.type() == AT_LIST)
  {
    for (aterm_list::iterator i = aterm_list(t).begin(); i != aterm_list(t).end(); ++i)
    {
      for_each_impl(*i, op);
    }
  }
  else if (t.type() == AT_APPL)
  {
    if (op(t))
    {
      for (aterm_appl::iterator i = aterm_appl(t).begin(); i != aterm_appl(t).end(); ++i)
      {
        for_each_impl(*i, op);
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
bool find_if_impl(const aterm& t, const MatchPredicate& match, aterm_appl& output)
{
  switch (t.type())
  {
  case AT_APPL:
    {
      aterm_appl appl(t);
      if (match(appl))
      {
        output = appl;
        return true;
      }
      for (aterm_appl::iterator i = appl.begin(); i != appl.end(); ++i)
      {
        if (find_if_impl(*i, match, output))
          return true;
      }
    }
    break;
  case AT_LIST:
    for (aterm_list::iterator i = aterm_list(t).begin(); i != aterm_list(t).end(); ++i)
    {
      if (find_if_impl(*i, match, output))
        return true;
    }
    break;
  }
  return false;
}

/// \brief Implements the find_all_if algorithm
/// \param t A term
/// \param op A predicate function on terms
/// \param destBegin The beginning of a range to where the results are written
template <typename MatchPredicate, typename OutputIterator>
void find_all_if_impl(aterm t, MatchPredicate op, OutputIterator& destBegin)
{
  typedef typename iterator_value<OutputIterator>::type value_type;

  if (t.type() == AT_LIST)
  {
    aterm_list l(t);
    for (aterm_list::iterator i = l.begin(); i != l.end(); ++i)
    {
      find_all_if_impl< MatchPredicate >(*i, op, destBegin);
    }
  }
  else if (t.type() == AT_APPL)
  {
    aterm_appl a(t);
    if (op(a))
    {
      value_type v(a);
      *destBegin++ = a;
    }
    for (aterm_appl::iterator i = a.begin(); i != a.end(); ++i)
    {
      find_all_if_impl< MatchPredicate >(*i, op, destBegin);
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
void partial_find_if_impl(aterm t, MatchPredicate match, StopPredicate stop)
{
  if (t.type() == AT_APPL)
  {
    if (match(aterm_appl(t)))
    {
      throw found_term_exception(aterm_appl(t)); // report the match
    }
    if (stop(aterm_appl(t)))
    {
      return; // nothing was found
    }
    for (aterm_appl::iterator i = aterm_appl(t).begin(); i != aterm_appl(t).end(); ++i)
    {
      partial_find_if_impl< MatchPredicate, StopPredicate >(*i, match, stop);
    }
  }

  if (t.type() == AT_LIST)
  {
    for (aterm_list::iterator i = aterm_list(t).begin(); i != aterm_list(t).end(); ++i)
    {
      partial_find_if_impl< MatchPredicate, StopPredicate >(*i, match, stop);
    }
  }
}

/// \brief Implements the partial_find_all_if algorithm
/// \param t A term
/// \param match A predicate function on terms
/// \param stop A predicate function on terms
/// \param destBegin The beginning of a range to where the results are written
template <typename MatchPredicate, typename StopPredicate, typename OutputIterator>
void partial_find_all_if_impl(aterm t, MatchPredicate match, StopPredicate stop, OutputIterator& destBegin)
{
  if (t.type() == AT_APPL)
  {
    if (match(aterm_appl(t)))
    {
      *destBegin++ = aterm_appl(t);
    }
    if (stop(aterm_appl(t)))
    {
      return;
    }
    for (aterm_appl::iterator i = aterm_appl(t).begin(); i != aterm_appl(t).end(); ++i)
    {
      partial_find_all_if_impl< MatchPredicate, StopPredicate >(*i, match, stop, destBegin);
    }
  }

  if (t.type() == AT_LIST)
  {
    for (aterm_list::iterator i = aterm_list(t).begin(); i != aterm_list(t).end(); ++i)
    {
      partial_find_all_if_impl< MatchPredicate, StopPredicate >(*i, match, stop, destBegin);
    }
  }
}

//--- replace -------------------------------------------------------------//

template <typename ReplaceFunction>
aterm replace_impl(aterm t, ReplaceFunction replace);

template <typename ReplaceFunction>
struct replace_helper
{
  ReplaceFunction m_replace;

  replace_helper(ReplaceFunction replace)
    : m_replace(replace)
  {}

  /// \brief Function call operator.
  /// \param t A term
  /// \return The function result
  aterm operator()(aterm t) const
  {
    return replace_impl(t, m_replace);
  }
};

/// \brief Implements the replace algorithm
/// \param t A term
/// \param f A replace function on terms
/// \return The result of the algorithm
template <typename ReplaceFunction>
aterm replace_impl(aterm t, ReplaceFunction f)
{
  aterm result = t;
  if (t.type() == AT_APPL)
  {
    aterm_appl a(t);
    aterm_appl fa = f(a);
    result = (a == fa) ? appl_apply(f(a), replace_helper<ReplaceFunction>(f)) : fa;
  }
  else if (t.type() == AT_LIST)
  {
    aterm_list l(t);
    result = list_apply(l, replace_helper<ReplaceFunction>(f));
  }
  return result;
}

struct default_replace
{
  aterm_appl m_src;
  aterm_appl m_dest;

  default_replace(aterm_appl src, aterm_appl dest)
    : m_src(src),
      m_dest(dest)
  { }

  /// \brief Function call operator
  /// \param t A term
  /// \return The function result
  aterm_appl operator()(aterm_appl t) const
  {
    return (t == m_src) ? m_dest : t;
  }
};

//--- partial replace -----------------------------------------------------//

template <typename ReplaceFunction>
aterm partial_replace_impl(aterm t, ReplaceFunction replace);

template <typename ReplaceFunction>
struct partial_replace_helpsr
{
  ReplaceFunction m_replace;

  partial_replace_helpsr(ReplaceFunction replace)
    : m_replace(replace)
  {}

  /// \brief Function call operator
  /// \param t A term
  /// \return The function result
  aterm operator()(aterm t) const
  {
    return partial_replace_impl(t, m_replace);
  }
};

/// \brief Implements the partial_replace algorithm
/// \param t A term
/// \param f A replace function on terms
/// \return The result of the algorithm
template <typename ReplaceFunction>
aterm partial_replace_impl(aterm t, ReplaceFunction f)
{
  aterm result = t;
  if (t.type() == AT_APPL)
  {
    aterm_appl a(t);
    std::pair<aterm_appl, bool> fa = f(a);
    if (fa.second) // continue recursion
    {
      result = appl_apply(fa.first, partial_replace_helpsr<ReplaceFunction>(f));
    }
    else
    {
      result = fa.first;
    }
  }
  else if (t.type() == AT_LIST)
  {
    aterm_list l(t);
    result = list_apply(l, partial_replace_helpsr<ReplaceFunction>(f));
  }
  return result;
}

//--- bottom-up replace ---------------------------------------------------//

template <typename ReplaceFunction>
aterm bottom_up_replace_impl(aterm t, ReplaceFunction bottom_up_replace);

template <typename ReplaceFunction>
struct bottom_up_replace_helpsr
{
  ReplaceFunction m_bottom_up_replace;

  bottom_up_replace_helpsr(ReplaceFunction bottom_up_replace)
    : m_bottom_up_replace(bottom_up_replace)
  {}

  /// \brief Function call operator
  /// \param t A term
  /// \return The function result
  aterm operator()(aterm t) const
  {
    return bottom_up_replace_impl(t, m_bottom_up_replace);
  }
};

/// \brief Implements the bottom_up_replace algorithm
/// \param t A term
/// \param f A replace function on terms
/// \return The result of the algorithm
template <typename ReplaceFunction>
aterm bottom_up_replace_impl(aterm t, ReplaceFunction f)
{
  aterm result = t;
  if (t.type() == AT_APPL)
  {
    aterm_appl a(t);
    result = f(appl_apply(a, bottom_up_replace_helpsr<ReplaceFunction>(f)));
  }
  else if (t.type() == AT_LIST)
  {
    aterm_list l(t);
    result = list_apply(l, bottom_up_replace_helpsr<ReplaceFunction>(f));
  }
  return result;
}

struct default_bottom_up_replace
{
  aterm_appl m_src;
  aterm_appl m_dest;

  default_bottom_up_replace(aterm_appl src, aterm_appl dest)
    : m_src(src),
      m_dest(dest)
  { }

  /// \brief Function call operator
  /// \param t A term
  /// \return The function result
  aterm_appl operator()(aterm_appl t) const
  {
    return (t == m_src) ? m_dest : t;
  }
};

} // namespace detail

} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ALGORITHM_IMPL_H
