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

  // prototypes
  inline
  aterm replace(aterm t, aterm old_value, aterm new_value, bool recursive = false);

  template <typename UnaryPredicate>
  aterm replace_if(aterm t, UnaryPredicate op, aterm new_value, bool recursive = false);

  namespace detail {

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
//          value_type v(a);
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
  
    struct replace_substitution
    {
      aterm m_old_value;
      aterm m_new_value;
      bool  m_recursive;
    
      replace_substitution(aterm old_value, aterm new_value, bool recursive)
        : m_old_value(old_value),
          m_new_value(new_value),
          m_recursive(recursive)
      {}
    
      aterm operator()(aterm t) const
      {
        return replace(t, m_old_value, m_new_value, m_recursive);
      }
    };
  
    template <typename UnaryPredicate>
    struct replace_if_substitution
    {
      const UnaryPredicate& m_predicate;
      aterm m_new_value;
      bool  m_recursive;
    
      replace_if_substitution(UnaryPredicate predicate, aterm new_value, bool recursive)
        : m_predicate(predicate),
          m_new_value(new_value),
          m_recursive(recursive)
      {}
    
      aterm operator()(aterm t) const
      {
        return replace_if(t, m_predicate, m_new_value, m_recursive);
      }
    };
  
  } // namespace detail

} // namespace atermpp

#endif // ATERM_ALGORITHM_IMPL_H
