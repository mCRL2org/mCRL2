// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/aterm_list.h
// date          : 25-10-2005
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

///////////////////////////////////////////////////////////////////////////////
/// \file atermpp/aterm_list.h
/// Contains a read-only singly linked list of terms.

#ifndef ATERM_TERM_LIST_H
#define ATERM_TERM_LIST_H

#include <cassert>
#include <boost/iterator/iterator_facade.hpp>
#include "atermpp/aterm.h"
#include "atermpp/aterm_make_match.h"
#include "atermpp/detail/aterm_conversion.h"
#include "atermpp/aterm_list_iterator.h"

namespace {
  // In the ATerm library the following functions are #define's:
  
  // #define   ATgetNext(l)  ((l)->tail)
  // #define   ATgetFirst(l) ((l)->head)
  
  /// INTERNAL ONLY
  inline ATermList aterm_get_next(ATermList l)
  {
    return ATgetNext(l);
  }
  
  /// INTERNAL ONLY
  inline ATerm aterm_get_first(ATermList l)
  {
    return ATgetFirst(l);
  }

  /// INTERNAL ONLY
  inline
  int aterm_get_length(ATermList l)
  {
    return ATgetLength(l);
  }
}

namespace atermpp {

  ///////////////////////////////////////////////////////////////////////////////
  // term_list
  /// \brief represents a singly linked list of terms (ATermList).
  ///
  /// N.B. This is intended as a replacement for the term_list of the atermpp library.
  ///
  template <typename Term>
  class term_list: public aterm_base
  {
    protected:
      const ATermList list() const
      { return reinterpret_cast<const ATermList>(m_term); }
  
      ATermList list()
      { return reinterpret_cast<ATermList>(m_term); }

    public:
      /// The type of object, T stored in the term_list.
      ///
      typedef Term value_type;

      /// Pointer to T.
      ///
      typedef Term* pointer;

      /// Reference to T.
      ///
      typedef Term& reference;

      /// Const reference to T.
      ///
      typedef const Term const_reference;

      /// An unsigned integral type.                                      
      ///
      typedef size_t size_type;

      /// A signed integral type.                                         
      ///
      typedef ptrdiff_t difference_type;

      /// Iterator used to iterate through an term_list.                      
      ///
      typedef term_list_iterator<Term> iterator;

      /// Const iterator used to iterate through an term_list.                
      ///
      typedef term_list_iterator<Term> const_iterator;

      /// Default constructor.
      ///
      term_list()
        : aterm_base(ATmakeList0())
      {}

      /// Construction from ATermList.
      ///
      term_list(ATermList l)
        : aterm_base(l)
      {
      }

      /// Construction from aterm_list.
      ///
      term_list(const term_list<aterm>& t);

      /// Allow construction from an aterm. The aterm must be of the right type.
      ///
      term_list(aterm t)
        : aterm_base(t)
      {
        assert(type() == AT_LIST);
      }

      /// Creates an term_list with a copy of a range.
      ///

      template <class Iter>
      term_list(Iter first, Iter last)
        : aterm_base(ATmakeList0())
      {
        while (first != last)
          m_term = void2term(list2void(ATinsert(list(), aterm(*(--last)))));
      }

      term_list<Term>& operator=(aterm_base t)
      {
        assert(t.type() == AT_LIST);
        m_term = aterm_traits<aterm_base>::term(t);
        return *this;
      }

      term_list<Term>& operator=(ATermList t)
      {
        m_term = reinterpret_cast<ATerm>(t);
        return *this;
      }

      /// The destructor.
      ///

      /// The assignment operator                                                                                                                                                        
      ///

      /// Returns an iterator pointing to the end of the term_list.     
      ///
      const_iterator begin() const
      { return const_iterator(list()); } 

      /// Returns a const_iterator pointing to the beginning of the term_list.
      ///
      const_iterator end() const
      { return const_iterator(ATmakeList0()); }
  
      ///
      /// Returns the size of the term_list.
      ///
      size_type size() const
      { return aterm_get_length(list()); }     

      ///
      /// Returns the largest possible size of the term_list.
      ///
      size_type max_size() const
      { return GET_LENGTH((std::numeric_limits<unsigned long>::max)()); }

      ///
      /// true if the list's size is 0.
      ///
      bool empty() const
      { return ATisEmpty(list()) == ATtrue; }

      ///
      /// Creates an term_list with n elements, each of which is a copy of T().
      ///

      ///
      /// Creates an term_list with n copies of t.
      ///

      ///
      /// Returns the first element.
      ///
      Term front() const
      { return Term(void2appl(term2void(aterm_get_first(list())))); }

      ///
      /// pos must be a valid iterator in *this. The return value is an iterator prev such that ++prev == pos. Complexity: linear in the number of iterators in the range [begin(), pos).
      ///
      const_iterator previous(const_iterator pos) const
      {
        const_iterator prev = end();
        for (const_iterator i = begin(); i != end(); ++i)
        {
          if (i == pos)
            return prev;
          prev = i;
        }
        return end();
      }

      /// Conversion to ATermList.
      ///
      operator ATermList() const
      { return void2list(m_term); }

      /// Applies a substitution to this list and returns the result.
      /// The Substitution object must supply the method aterm operator()(aterm).
      ///
      template <typename Substitution>
      term_list<Term> substitute(Substitution f) const
      {
        return term_list<Term>(f(*this));
      }     
  };

  ///
  /// A term_list with elements of type aterm.
  ///
  typedef term_list<aterm> aterm_list;

  ///
  /// Returns the list obtained by inserting a new element at the beginning.
  ///
  template <typename Term>
  inline
  Term front(term_list<Term> l)
  {
    return *l.begin();
  }

  ///
  /// Returns the list obtained by inserting a new element at the beginning.
  ///
  template <typename Term>
  inline
  term_list<Term> push_front(term_list<Term> l, Term elem)
  {
    return term_list<Term>(ATinsert(l, aterm(elem)));
  }

  ///
  /// Returns the list obtained by inserting a new element at the end. Note
  /// that the complexity of this function is O(n), with n the number of
  /// elements in the list!!!
  ///
  template <typename Term>
  inline
  term_list<Term> push_back(term_list<Term> l, Term elem)
  {
    return term_list<Term>(ATappend(l, aterm(elem)));
  }

  ///
  /// Returns the list obtained by removing the first element.
  ///
  template <typename Term>
  inline
  term_list<Term> pop_front(term_list<Term> l)
  {
    return term_list<Term>(aterm_get_next(l));
  }

  ///
  /// Returns the list with the elements in reversed order.
  ///
  template <typename Term>
  inline
  term_list<Term> reverse(term_list<Term> l)
  {
    return term_list<Term>(ATreverse(l));
  }

  ///
  /// Applies the function f to all elements of the list and returns the result.
  ///
  template <typename Term, typename Function>
  inline
  aterm_list apply(term_list<Term> l, const Function f)
  {
    aterm_list result;
    for (typename term_list<Term>::iterator i = l.begin(); i != l.end(); ++i)
    {
      result = push_front(result, aterm(f(*i)));
    }
    return reverse(result);
  }

  ///
  /// Return the concatenation of the lists l and m.
  ///
  template <typename Term>
  inline
  term_list<Term> operator+(term_list<Term> l, term_list<Term> m)
  { return term_list<Term>(ATconcat(l, m)); }

  ///
  /// Return the concatenation of the list l and the element t.
  ///
  template <typename Term>
  inline
  term_list<Term> operator+(term_list<Term> l, Term t)
  { return term_list<Term>(ATappend(l, aterm(t))); }

  ///
  /// Return the concatenation of the element t and the list l.
  ///
  template <typename Term>
  inline
  term_list<Term> operator+(Term t, term_list<Term> l)
  { return term_list<Term>(ATappend(l, aterm(t))); }

  template <typename Term>
  struct aterm_traits<term_list<Term> >
  {
    typedef ATermList aterm_type;
    static void protect(term_list<Term> t)   { t.protect(); }
    static void unprotect(term_list<Term> t) { t.unprotect(); }
    static void mark(term_list<Term> t)      { t.mark(); }
    static ATerm term(term_list<Term> t)     { return t.term(); }
    static ATerm* ptr(term_list<Term>& t)    { return &t.term(); }
  };

  template <typename Term>
  bool operator==(const term_list<Term>& x, const term_list<Term>& y)
  {
    return ATisEqual(aterm_traits<term_list<Term> >::term(x), aterm_traits<term_list<Term> >::term(y)) == ATtrue;
  }
  
  template <typename Term>
  bool operator==(const term_list<Term>& x, ATermList y)
  {
    return ATisEqual(aterm_traits<term_list<Term> >::term(x), y) == ATtrue;
  }
  
  template <typename Term>
  bool operator==(ATermList x, const term_list<Term>& y)
  {
    return ATisEqual(x, aterm_traits<term_list<Term> >::term(y)) == ATtrue;
  }

  // implementation
  template <typename Term>
  term_list<Term>::term_list(const term_list<aterm>& t)
    : aterm_base(t)
  {}

} // namespace atermpp

#include "atermpp/make_list.h"

#endif // ATERM_TERM_LIST_H
