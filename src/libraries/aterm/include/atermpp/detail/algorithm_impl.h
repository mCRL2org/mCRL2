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
  aterm replace(aterm t, aterm old_value, aterm new_value, bool recursive);

  template <typename UnaryPredicate>
  aterm replace_if(aterm t, UnaryPredicate op, aterm new_value, bool recursive);

  namespace detail {
  
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
  
      // determine child nodes
      aterm_list children;
      if (t.type() == AT_LIST) {
        children = t;
      }
      else if (t.type() == AT_APPL) {
        children = aterm_appl(t).argument_list();
      }
      else {
        return;
      }
  
      for (aterm_list::iterator i = children.begin(); i != children.end(); ++i)
      {
        find_if_impl(*i, op);
      }
    }
  
    template <typename UnaryPredicate, typename OutputIterator>
    void find_all_if_impl(aterm t, UnaryPredicate op, OutputIterator& destBegin)
    {
      aterm_list arguments;
      if (t.type() == AT_LIST) {
        arguments = t;
      }
      else if (t.type() == AT_APPL) {
        if (op(t))
          destBegin++ = t;
        arguments = aterm_appl(t).argument_list();
      }
      else {
        return;
      }
      for (aterm_list::iterator i = arguments.begin(); i != arguments.end(); ++i)
      {
        find_all_if_impl(*i, op, destBegin);
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
