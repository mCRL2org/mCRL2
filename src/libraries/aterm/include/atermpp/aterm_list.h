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
  class term_list: public aterm
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
        : aterm(ATmakeList0())
      {}

      /// Construction from ATermList.
      ///
      term_list(ATermList l)
        : aterm(l)
      {
        assert(type() == AT_LIST);
      }

      /// Allow construction from an aterm. The aterm must be of the right type.
      ///
      term_list(aterm t)
        : aterm(t)
      {
        assert(type() == AT_LIST);
      }

      /// Creates an term_list with a copy of a range.
      ///

      template <class Iter>
      term_list(Iter first, Iter last)
        : aterm(ATmakeList0())
      {
        while (first != last)
          m_term = void2term(list2void(ATinsert(list(), aterm(*(--last)))));
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
  term_list<Term> push_front(term_list<Term> l, Term elem)
  {
    return term_list<Term>(ATinsert(l, aterm(elem)));
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
  term_list<Term> apply(term_list<Term> l, const Function& f)
  {
    term_list<Term> result;
    for (typename term_list<Term>::iterator i = l.begin(); i != l.end(); ++i)
    {
      result = push_front(result, f(*i));
    }
    return reverse(result);
  }

/*
  ///
  /// Returns the next part (the tail) of list l.
  ///
  template <typename Term>
  inline
  term_list<Term> get_next(term_list<Term> l)
  {
    return term_list<Term>(aterm_get_next(l));
  }

  ///
  /// Return the sublist from start to the end of list l.
  ///
  template <typename Term>
  inline
  term_list<Term> tail(term_list<Term> l, int start)
  { return term_list<Term>(ATgetTail(l, start)); }
  
  ///
  /// Replace the tail of list l from position start with new_tail.
  ///
  template <typename Term>
  inline
  term_list<Term> replace_tail(term_list<Term> l, term_list<Term> new_tail, int start)
  { return term_list<Term>(ATreplaceTail(l, new_tail, start)); }
  
  ///
  /// Return all but the last element of list l.
  ///
  template <typename Term>
  inline
  term_list<Term> prefix(term_list<Term> l)
  { return term_list<Term>(ATgetPrefix(l)); }
  
  ///
  /// Return the last element of list l.
  ///
  template <typename Term>
  inline
  Term get_last(term_list<Term> l)
  { return Term(ATgetLast(l)); }
  
  ///
  /// Get a portion (slice) of list l.
  /// Return the portion of list that lies between start and end.  Thus start is
  /// included, end is not.
  ///
  template <typename Term>
  inline
  term_list<Term> slice(term_list<Term> l, int start, int end)
  { return term_list<Term>(ATgetSlice(l, start, end)); }
  
  ///
  /// Return list l with el inserted.
  /// The behaviour of insert is of constant complexity. That is, the behaviour of
  /// insert does not degrade as the length of list increases.
  ///
  template <typename Term>
  inline
  term_list<Term> insert(term_list<Term> l, Term el)
  { return term_list<Term>(ATinsert(l, el)); }
  
  ///
  /// Return list l with el inserted at position index.
  ///
  template <typename Term>
  inline
  term_list<Term> insert_at(term_list<Term> l, Term el, int index)
  { return term_list<Term>(ATinsertAt(l, el, index)); }
  
  ///
  /// Return list l with el appended to it.
  /// Note that append is implemented in terms of insert by making a new list
  /// with el as the first element and then inserting all elements from list. As such, the complexity
  /// of append is linear in the number of elements in list.
  ///    When append is needed inside a loop that traverses a list behaviour
  /// of the loop will demonstrate quadratic complexity.
  ///
  template <typename Term>
  inline
  term_list<Term> append(term_list<Term> l, Term el)
  { return term_list<Term>(ATappend(l, el)); }
  
  ///
  /// Return the concatenation of the list l and m.
  ///
  template <typename Term>
  inline
  term_list<Term> concat(term_list<Term> l, term_list<Term> m)
  { return term_list<Term>(ATconcat(l, m)); }
  
  ///
  /// Return the index of a Term in a list.
  /// Return the index where el can be found in list. Start looking at position start.
  /// Returns -1 if el is not in list.
  ///
  template <typename Term>
  inline
  int index_of(term_list<Term> l, Term el, int start)
  { return ATindexOf(l, el, start); }
  
  ///
  /// Return the index of an Term in a list (reverse).
  /// Search backwards for el in list. Start searching at start. Return the index of
  /// the first occurrence of l encountered, or -1 when el is not present before start.
  ///
  template <typename Term>
  inline
  int last_index_of(term_list<Term> l, Term el, int start)
  { return ATlastIndexOf(l, el, start); }
  
  ///
  /// Return a specific element of a list.
  /// Return the element at position index in list. Returns `Term()` when index is not in
  /// list.
  ///
  template <typename Term>
  inline
  Term element_at(term_list<Term> l, int index)
  { return Term(ATelementAt(l, index)); }
  
  ///
  /// Return list with one occurrence of el removed.
  ///
  template <typename Term>
  inline
  term_list<Term> remove_element(term_list<Term> l, Term elem)
  { return term_list<Term>(ATremoveElement(l, elem)); }
  
  ///
  /// Return list l with all occurrences of el removed.
  ///
  template <typename Term>
  inline
  term_list<Term> remove_all(term_list<Term> l, Term el)
  {
    return term_list<Term>(ATremoveAll(l, el));
  }

  ///
  /// Return list l with the element at index removed.
  ///
  template <typename Term>
  inline
  term_list<Term> remove_element_at(term_list<Term> l, int index)
  { return term_list<Term>(ATremoveElementAt(l, index)); }
  
  ///
  /// Return list l with the element at index replaced by el.
  ///
  template <typename Term>
  inline
  term_list<Term> replace(term_list<Term> l, Term elem, int index)
  { return term_list<Term>(ATreplace(l, elem, index)); } 
*/

  /// INTERNAL ONLY
  template <typename T>
  ATerm aterm_ptr(atermpp::term_list<T>& t)
  {
    return t;
  }
  
  /// INTERNAL ONLY
  template <typename T>
  ATerm aterm_ptr(const atermpp::term_list<T>& t)
  {
    return t;
  }

   template <>
   class aterm_protect_traits<aterm_list>
   {
     public:
       static void protect(aterm_list t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<aterm_list>::protect() " << t << std::endl;
#endif // ATERM_DEBUG_PROTECTION
         t.protect();
       }

       static void unprotect(aterm_list t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<aterm_list>::unprotect() " << t << std::endl;
#endif // ATERM_DEBUG_PROTECTION
         t.unprotect();
       }

       static void mark(aterm_list t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<aterm_list>::mark() " << t << std::endl;
#endif // ATERM_DEBUG_PROTECTION
         t.mark();
       }
   };

} // namespace atermpp

#endif // ATERM_TERM_LIST_H
