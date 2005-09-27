///////////////////////////////////////////////////////////////////////////////
/// \file mcrl2/list_iterator.h
/// Contains a general term_list iterator.

#ifndef MCRL2_TERM_LIST_H
#define MCRL2_TERM_LIST_H

#include <list>
#include <vector>
#include <boost/iterator/iterator_facade.hpp>
#include "atermpp/aterm.h"
#include "mcrl2/term_list_iterator.h"

namespace mcrl2 {

  ///////////////////////////////////////////////////////////////////////////////
  // term_list
  /// \brief represents a singly linked list of terms (ATermList).
  ///
  /// N.B. This is intended as a replacement for the aterm_list of the atermpp library.
  ///
  template <typename Term>
  class term_list
  {
    protected:
        ATermList m_list;

    public:
      /**
        * The type of object, m_list, stored in the term_list.
        **/
      typedef Term value_type;

      /**
        * Pointer to m_list.
        **/
      typedef Term* pointer;

      /**
        * Reference to T.
        **/
      typedef Term& reference;

      /**
        * Const reference to T.
        **/
      typedef const Term const_reference;

      /**
        * An unsigned integral type.                                      
        **/
      typedef size_t size_type;

      /**
        * A signed integral type.                                         
        **/
      typedef ptrdiff_t difference_type;

      /**
        * Iterator used to iterate through an term_list.                      
        **/
      typedef term_list_iterator<Term> iterator;

      /**
        * Const iterator used to iterate through an term_list.                
        **/
      typedef term_list_iterator<Term> const_iterator;

      /**
        * The copy constructor.
        **/
      term_list(const term_list<Term>& l)
        : m_list(l.m_list)
      {}

      term_list(ATermList l)
        : m_list(l)
      {}

      /**
        * Creates an term_list with a copy of a range.
        **/

      template <class Iter>
      term_list(Iter first, Iter last)
      {
        m_list = ATmakeList0();
        while (first != last)
          m_list = ATinsert(m_list, (--last)->term());
          //m_list = ATinsert(m_list, convert_term(*(--last)));
      }

      /**
        * Allow construction from an aterm.
        **/
      term_list(atermpp::aterm t)
        : m_list(void2list(term2void(t.term())))
      {}

      /**
        * The destructor.
        **/

      /**
        * The assignment operator                                                                                                                                                        
        **/

      /**
        * Swaps the contents of two term_lists.
        **/
      void swap(term_list<Term>& l)
      {
        std::swap(m_list, l.m_list);
      }

      /**
        * Returns an iterator pointing to the end of the term_list.     
        **/
      const_iterator begin() const
      { return const_iterator(m_list); } 

      /**
        * Returns a const_iterator pointing to the beginning of the term_list.
        **/
      const_iterator end() const
      { return const_iterator(ATmakeList0()); }
  
      /**
        * Returns the size of the term_list.
        **/
      size_type size()
      { return ATgetLength(m_list); }     

      /**
        * Returns the largest possible size of the term_list.
        **/
      size_type max_size()
      { return GET_LENGTH((std::numeric_limits<unsigned long>::max)()); }

      /**
        * true if the list's size is 0.
        **/
      bool empty() const
      { return ATisEmpty(m_list) == ATtrue; }

      /**
        * Creates an empty term_list.
        **/
      term_list()
        : m_list(ATmakeList0())
      {}

      /**
        * Creates an term_list with n elements, each of which is a copy of T().
        **/

      /**
        * Creates an term_list with n copies of t.
        **/

      /**
        * Returns the first element.
        **/
      Term front() const
      { return Term(void2appl(term2void(ATgetFirst(m_list)))); }

      /**
        * pos must be a valid iterator in *this. The return value is an iterator prev such that ++prev == pos. Complexity: linear in the number of iterators in the range [begin(), pos).
        **/
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

      /**
        * Erases all of the elements.
        **/
      void clear()
      { m_list = ATmakeList0(); }

      /**
        * Conversion to ATermList.
        **/
      operator ATermList() const
      { return m_list; }
  
      /**
        * Returns the ATermList that is contained by the term_list.
        **/
      ATermList list() const
      { return m_list; }

      /**
        * Returns the ATermList that is contained by the term_list.
        **/
      ATermList term() const
      { return m_list; }

      /// Applies a substitution to this term_list and returns the result.
      /// The Substitution object must supply the method aterm_appl operator()(aterm_appl).
      ///
      // TODO: can this be done more efficiently (without a temporary vector)?
      template <typename Substitution>
      term_list<Term> substitute(Substitution f)
      {
        std::vector<aterm_appl> tmp;
        tmp.reserve(size());
        for (iterator i = begin(); i != end(); ++i)
        {
          tmp.push_back(f(*i));
        }
        return term_list<Term>(tmp.begin(), tmp.end());
      }     

      /// Applies a sequence of substitutions to this term_list and returns the result.
      /// The SubstIter objects must supply the method aterm_appl operator()(aterm_appl).
      ///
      template <typename SubstIter>
      term_list<Term> substitute(SubstIter first, SubstIter last)
      {
        std::vector<aterm_appl> tmp;
        tmp.reserve(size());
        for (iterator i = begin(); i != end(); ++i)
        {
          tmp.push_back(aterm_appl_substitute(i->term(), first, last));
        }
        return term_list<Term>(tmp.begin(), tmp.end());
      }     

      /// Returns a string representation of the term_list.
      ///
      std::string to_string() const
      {
        return atermpp::aterm_list(m_list).to_string();
      }
  };

  /**
    * Lexicographical comparison. This is a global function, not a member function.
    **/

  /**
    * Inserts a new element at the beginning.
    **/
  template <typename Term>
  inline
  term_list<Term> push_front(term_list<Term> l, Term elem)
  { return term_list<Term>(ATinsert(l.list(), elem.term())); }

  /**
    * Removes the first element.
    **/
  template <typename Term>
  inline
  term_list<Term> pop_front(term_list<Term> l)
  { return term_list<Term>(ATgetNext(l.list())); }

  /**
    * Returns the next part (the tail) of list l.
    **/
  template <typename Term>
  inline
  term_list<Term> get_next(term_list<Term> l)
  {
    return ATgetNext(l.list());
  }

  /**
    * Return the sublist from start to the end of list l.
    **/
  template <typename Term>
  inline
  term_list<Term> tail(term_list<Term> l, int start)
  { return ATgetTail(l.list(), start); }
  
  /**
    * Replace the tail of list l from position start with new_tail.
    **/
  template <typename Term>
  inline
  term_list<Term> replace_tail(term_list<Term> l, term_list<Term> new_tail, int start)
  { return ATreplaceTail(l.list(), new_tail.list(), start); }
  
  /**
    * Return all but the last element of list l.
    **/
  template <typename Term>
  inline
  term_list<Term> prefix(term_list<Term> l)
  { return ATgetPrefix(l.list()); }
  
  /**
    * Return the last element of list l.
    **/
  template <typename Term>
  inline
  Term get_last(term_list<Term> l)
  { return ATgetLast(l.list()); }
  
  /**
    * Get a portion (slice) of list l.
    * Return the portion of list that lies between start and end.  Thus start is
    * included, end is not.
    **/
  template <typename Term>
  inline
  term_list<Term> slice(term_list<Term> l, int start, int end)
  { return ATgetSlice(l.list(), start, end); }
  
  /**
    * Return list l with el inserted.
    * The behaviour of insert is of constant complexity. That is, the behaviour of
    * insert does not degrade as the length of list increases.
    **/
  template <typename Term>
  inline
  term_list<Term> insert(term_list<Term> l, Term el)
  { return ATinsert(l.list(), el.term()); }
  
  /**
    * Return list l with el inserted at position index.
    **/
  template <typename Term>
  inline
  term_list<Term> insert_at(term_list<Term> l, Term el, int index)
  { return ATinsertAt(l.list(), el.term(), index); }
  
  /**
    * Return list l with el appended to it.
    * Note that append is implemented in terms of insert by making a new list
    * with el as the first element and then inserting all elements from list. As such, the complexity
    * of append is linear in the number of elements in list.
    *    When append is needed inside a loop that traverses a list behaviour
    * of the loop will demonstrate quadratic complexity.
    **/
  template <typename Term>
  inline
  term_list<Term> append(term_list<Term> l, Term el)
  { return ATappend(l.list(), el.term()); }
  
  /**
    * Return the concatenation of the list l and m.
    **/
  template <typename Term>
  inline
  term_list<Term> concat(term_list<Term> l, term_list<Term> m)
  { return ATconcat(l.list(), m.list()); }
  
  /**
    * Return the index of an m_list in a list.
    * Return the index where el can be found in list. Start looking at position start.
    * Returns -1 if el is not in list.
    **/
  template <typename Term>
  inline
  int index_of(term_list<Term> l, Term el, int start)
  { return ATindexOf(l.list(), el.term(), start); }
  
  /**
    * Return the index of an m_list in a list (reverse).
    * Search backwards for el in list. Start searching at start. Return the index of
    * the first occurrence of l encountered, or -1 when el is not present before start.
    **/
  template <typename Term>
  inline
  int last_index_of(term_list<Term> l, Term el, int start)
  { return ATlastIndexOf(l.list(), el.term(), start); }
  
  /**
    * Return a specific element of a list.
    * Return the element at position index in list. Returns `m_list()` when index is not in
    * list.
    **/
  template <typename Term>
  inline
  Term element_at(term_list<Term> l, int index)
  { return ATelementAt(l.list(), index); }
  
  /**
    * Return list with one occurrence of el removed.
    **/
  template <typename Term>
  inline
  term_list<Term> remove_element(term_list<Term> l, Term elem)
  { return ATremoveElement(l.list(), elem.term()); }
  
  /**
    * Return list l with all occurrences of el removed.
    **/
  template <typename Term>
  inline
  term_list<Term> remove_all(term_list<Term> l, Term el)
  {
    return ATremoveAll(l.list(), el.term());
  }

  /**
    * Return list l with the element at index removed.
    **/
  template <typename Term>
  inline
  term_list<Term> remove_element_at(term_list<Term> l, int index)
  { return ATremoveElementAt(l.list(), index); }
  
  /**
    * Return list l with the element at index replaced by el.
    **/
  template <typename Term>
  inline
  term_list<Term> replace(term_list<Term> l, Term elem, int index)
  { return ATreplace(l.list(), elem.term(), index); }
  
  /**
    * Return list l with its elements in reversed order.
    **/
  template <typename Term>
  inline
  term_list<Term> reverse(term_list<Term> l)
  { return ATreverse(l.list()); }

} // namespace mcrl

#endif // MCRL2_TERM_LIST_H
