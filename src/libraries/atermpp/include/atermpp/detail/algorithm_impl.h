// ======================================================================
//
// Copyright (c) 2006 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/detail/algorithm_impl.h
// date          : 29-09-2006
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

///////////////////////////////////////////////////////////////////////////////
/// \file algorithm_impl.h
/// Contains algorithms on aterm_lists and aterm_appls.

#ifndef ATERM_ALGORITHM_IMPL_H
#define ATERM_ALGORITHM_IMPL_H

#include "atermpp/aterm.h"
#include "atermpp/aterm_appl.h"
#include "atermpp/aterm_list.h"

namespace atermpp {

namespace detail {

  ///
  /// Applies the function f to all elements of the list and returns the result.
  ///
  template <typename Term, typename Function>
  aterm_list list_apply(term_list<Term> l, const Function f)
  {
    if (l.size() == 0)
      return l;
    aterm_list result;
    for (typename term_list<Term>::iterator i = l.begin(); i != l.end(); ++i)
    {
      result = push_front(result, aterm(f(*i)));
    }
    return reverse(result);
  }

  ///
  /// Applies the function f to all children of a and returns the result.
  ///
  template <typename Term, typename Function>
  aterm_appl appl_apply(term_appl<Term> a, const Function f)
  {
    aterm_appl t = a;
    unsigned int n = t.size();
    if (n > 0)
    {
      for (unsigned int i = 0; i < n; i++)
      {
        aterm ti = t(i);
        aterm fi = f(ti);
        if (fi != ti)
          t = t.set_argument(fi, i);
      }
    }
    return t;
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

  /// INTERNAL ONLY
  // used to abort the recursive find
  struct found_term_exception
  {
    aterm t;

    found_term_exception(aterm t_)
      : t(t_)
    {}
  };
  
  template <typename UnaryPredicate>
  void find_if_impl(aterm t, UnaryPredicate op)
  {
    if (op(t))
      throw found_term_exception(t);

    if (t.type() == AT_LIST) {
      for (aterm_list::iterator i = aterm_list(t).begin(); i != aterm_list(t).end(); ++i)
      {
        find_if_impl(*i, op);
      }
    }
    else if (t.type() == AT_APPL) {
      for (aterm_appl::iterator i = aterm_appl(t).begin(); i != aterm_appl(t).end(); ++i)
      {
        find_if_impl(*i, op);
      }
    }
    else {
      return;
    }
  }

  template <typename UnaryPredicate, typename OutputIterator>
  void find_all_if_impl(aterm t, UnaryPredicate op, OutputIterator& destBegin)
  {
    typedef typename iterator_value<OutputIterator>::type value_type;
    
    if (t.type() == AT_LIST) {
      for (aterm_list::iterator i = aterm_list(t).begin(); i != aterm_list(t).end(); ++i)
      {
        find_all_if_impl(*i, op, destBegin);
      }
    }
    else if (t.type() == AT_APPL) {
      aterm_appl a(t);
      if (op(a)) {
          value_type v(a);
        *destBegin++ = a;
      }
      for (aterm_appl::iterator i = a.begin(); i != a.end(); ++i)
      {
        find_all_if_impl(*i, op, destBegin);
      }
    }
    else {
      return;
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
  
    aterm operator()(aterm t) const
    {
      return replace_impl(t, m_replace);
    }
  };

  /// Replaces each subterm in the aterm_appl src for which the unary predicate
  /// op holds with new_value.
  template <typename ReplaceFunction>
  aterm replace_impl(aterm t, ReplaceFunction f)
  {
    aterm result = t;
    if (t.type() == AT_APPL)
    {
      aterm_appl a(t);
      result = appl_apply(f(a), replace_helper<ReplaceFunction>(f));
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
  
    aterm_appl operator()(aterm_appl t) const
    {
      return (t == m_src) ? m_dest : t;
    }
  };

//--- partial replace -----------------------------------------------------//

  template <typename ReplaceFunction>
  aterm partial_replace_impl(aterm t, ReplaceFunction replace);

  template <typename ReplaceFunction>
  struct partial_replace_helper
  {
    ReplaceFunction m_replace;
  
    partial_replace_helper(ReplaceFunction replace)
      : m_replace(replace)
    {}
  
    aterm operator()(aterm t) const
    {
      return partial_replace_impl(t, m_replace);
    }
  };

  /// Replaces each subterm in the aterm_appl src for which the unary predicate
  /// op holds with new_value.
  template <typename ReplaceFunction>
  aterm partial_replace_impl(aterm t, ReplaceFunction f)
  {
    aterm result = t;
    if (t.type() == AT_APPL)
    {
      aterm_appl a(t);
      std::pair<aterm_appl, bool> fa = f(a);
      if (fa.second) // continue recursion
        result = appl_apply(fa.first, partial_replace_helper<ReplaceFunction>(f));
      else
        result = fa.first;
    }
    else if (t.type() == AT_LIST)
    {
      aterm_list l(t);
      result = list_apply(l, partial_replace_helper<ReplaceFunction>(f));
    }
    return result;
  }

//--- bottom-up replace ---------------------------------------------------//

  template <typename ReplaceFunction>
  aterm bottom_up_replace_impl(aterm t, ReplaceFunction bottom_up_replace);
 
  template <typename ReplaceFunction>
  struct bottom_up_replace_helper
  {
    ReplaceFunction m_bottom_up_replace;
  
    bottom_up_replace_helper(ReplaceFunction bottom_up_replace)
      : m_bottom_up_replace(bottom_up_replace)
    {}
  
    aterm operator()(aterm t) const
    {
      return bottom_up_replace_impl(t, m_bottom_up_replace);
    }
  };

  /// Replaces each subterm in the aterm_appl src for which the unary predicate
  /// op holds with new_value.
  template <typename ReplaceFunction>
  aterm bottom_up_replace_impl(aterm t, ReplaceFunction f)
  {
    aterm result = t;
    if (t.type() == AT_APPL)
    {
      aterm_appl a(t);
      result = appl_apply(a, bottom_up_replace_helper<ReplaceFunction>(f));
    }
    else if (t.type() == AT_LIST)
    {
      aterm_list l(t);
      result = list_apply(l, bottom_up_replace_helper<ReplaceFunction>(f));
    }
    return f(result);
  }

  struct default_bottom_up_replace
  {
    aterm_appl m_src;
    aterm_appl m_dest;
    
    default_bottom_up_replace(aterm_appl src, aterm_appl dest)
      : m_src(src),
        m_dest(dest)
    { }
  
    aterm_appl operator()(aterm_appl t) const
    {
      return (t == m_src) ? m_dest : t;
    }
  };

} // namespace detail

} // namespace atermpp

#endif // ATERM_ALGORITHM_IMPL_H
