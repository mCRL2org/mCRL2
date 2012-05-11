// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_list.h
/// \brief List of terms.

#ifndef MCRL2_ATERMPP_ATERM_LIST_H
#define MCRL2_ATERMPP_ATERM_LIST_H

#include <cassert>
#include <limits>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/detail/aterm_list_iterator.h"

namespace atermpp
{


template <typename Term>
class term_list:public aterm
{
  protected:

    static term_list<Term> empty_list() 
    { 
      static const term_list<Term> m_empty_list=term_list<Term>(aterm(AS_EMPTY_LIST));
      return m_empty_list;
    } 

  public:

    /// The type of object, T stored in the term_list.
    typedef Term value_type;
    
    /// Pointer to T.
    typedef Term* pointer;
    
    /// Reference to T.
    typedef Term& reference;
    
    /// Const reference to T.
    typedef const Term const_reference;
    
    /// An unsigned integral type.
    typedef size_t size_type;
    
    /// A signed integral type.
    typedef ptrdiff_t difference_type;
    
    /// Iterator used to iterate through an term_list.
    typedef term_list_iterator<Term> iterator;
    
    /// Const iterator used to iterate through an term_list.
    typedef term_list_iterator<Term> const_iterator;
    
    /// Default constructor. Creates an empty list.
    term_list ():aterm(reinterpret_cast<_ATerm*>(&*empty_list()))
    {
    }

    /// Construction from ATermList.
    /// \param l A list.
    term_list(const term_list<Term> &t):aterm(reinterpret_cast<_ATerm *>(&*t))
    {
      assert(m_term==NULL || type() == AT_LIST); // term list can be NULL.
    }

    /// \brief Construction from ATermList.
    /// \param l A list.
    term_list(_ATermList *t):aterm(reinterpret_cast<_ATerm *>(t))
    {
      assert(t==NULL || type() == AT_LIST); // term list can be NULL. This is generally used to indicate a faulty
                                            // situation. This used should be discouraged.
    }

    /// Construction from aterm_list.
    /// \param t A term containing a list.
    //  \deprecated This conversion should be removed. Conversions
    //  between lists must be explicit.
    //  \return The same list of a different type.
    template <typename SpecificTerm>
    term_list<Term>(const term_list<SpecificTerm> &t): aterm(t)
    {} 
    

    /// Explicit construction from ATerm. 
    ///  \param t An aterm.
    explicit term_list(const aterm &t):aterm(t)
    {
      assert(m_term==NULL || t.type()==AT_LIST); // Term list can be NULL; Generally, this is used to indicate a faulty situation.
                                                 // This use should be discouraged.
    }

    /// \brief Construction of a list from a list and an element
    /// \detail This is the standard cons operator on lists.
    /// \param l A list
    /// \param t An element
    term_list(const term_list<Term> &l, const Term &t);
    

    /// Creates an term_list with a copy of a range.
    /// \param first The start of a range of elements.
    /// \param last The end of a range of elements.
    template <class Iter>
    term_list(Iter first, Iter last, typename boost::enable_if<
              typename boost::is_convertible< typename boost::iterator_traversal< Iter >::type,
              boost::random_access_traversal_tag >::type >::type* = 0)
    {
      term_list<Term> result;
      while (first != last)
      {
        const Term t=*(--last);
        result = term_list<Term>(result, t);
      }
      m_term=&*result;
      increase_reference_count<false>(m_term);
    }
    
    /// Creates an term_list with a copy of a range.
    /// \param first The start of a range of elements.
    /// \param last The end of a range of elements.
    template <class Iter>
             term_list(Iter first, Iter last, typename boost::disable_if<
             typename boost::is_convertible< typename boost::iterator_traversal< Iter >::type,
             boost::random_access_traversal_tag >::type >::type* = 0)
    {
      std::vector<Term> temporary_store;
      while (first != last)
      {
        const Term t= *first;
        temporary_store.push_back(t);
        first++;
      }
      term_list<Term> result;
      for(typename std::vector<Term>::reverse_iterator i=temporary_store.rbegin(); 
              i!=temporary_store.rend(); ++i)
      { 
        result=term_list(result, *i); 
      }
      m_term=&*result;
      increase_reference_count<false>(m_term);
    }

    /// Assigment operator.
    /// \param l A list.
    term_list<Term> &operator=(const term_list &l)
    {
      copy_term(l.m_term);
      return *this;
    }

    _ATermList & operator *() const
    {
      // Note that this operator can be applied on a NULL pointer, i.e., in the case &*m_term is checked,
      // which is done quite commonly.
      assert(m_term==NULL || m_term->reference_count>0);
      return *reinterpret_cast<_ATermList*>(m_term); 
    }

    _ATermList *operator ->() const
    {
      assert(m_term!=NULL);
      assert(m_term->reference_count>0);
      return reinterpret_cast<_ATermList*>(m_term);
    }

    /// \brief Conversion to ATermList.
    /// \return The wrapped ATermList pointer.
    operator term_list<aterm>() const
    {
      return static_cast<_ATermList*>(m_term);
    }

    /// \brief Returns the tail of the list.
    /// \return The tail of the list.
    const term_list<Term> tail() const
    {
      return (static_cast<_ATermList*>(m_term))->tail;
    }

    /// \brief Returns the head of the list.
    /// \return The term at the head of the list.
    const Term head()
    {
      return Term(static_cast<_ATermList*>(m_term)->head);
    }

    /// \brief Returns the size of the term_list.
    /// \detail The complexity of this function is linear in the size of the list.
    /// \return The size of the list.
    size_type size() const
    {
      size_t size=0;
      for(_ATermList* m_list=static_cast<_ATermList*>(m_term);
                 m_list!=reinterpret_cast<_ATermList*>(&*empty_list()); m_list=m_list->tail)
      {
        size++;
      }
      return size;
    }

    /// \brief Returns true if the list's size is 0.
    /// \return True if the list is empty.
    bool empty() const
    {
      return m_term==reinterpret_cast<_ATerm*>(&*empty_list());
    }

    /// \brief Returns the first element.
    /// \return The first element of the list.
    Term front() const
    {
      return Term(static_cast<aterm>(static_cast<_ATermList*>(m_term)->head));
    }

    /// \brief Returns a const_iterator pointing to the beginning of the term_list.
    /// \return The beginning of the list.
    const_iterator begin() const
    {
      return const_iterator(m_term);
    }

    /// \brief Returns a const_iterator pointing to the end of the term_list.
    /// \return The end of the list.
    const_iterator end() const
    {
      return const_iterator(reinterpret_cast<_ATerm*>(&*empty_list()));
    }

    /// \brief Returns the largest possible size of the term_list.
    /// \return The largest possible size of the list.
    size_type max_size() const
    {
      return (std::numeric_limits<size_t>::max)();
    }
};

/// \brief A term_list with elements of type aterm.
// typedef term_list<aterm> aterm_list;
typedef term_list<aterm> aterm_list;

/// \brief Returns the first element of the list l.
/// \param l A list
/// \return The first element of the list.
template <typename Term>
inline
Term front(term_list<Term> l)
{
  return *l.begin();
}

/// \brief Returns the list obtained by inserting a new element at the beginning.
/// \param l A list.
/// \param elem A term
/// \return The list with an element inserted in front of it.
template <typename Term>
inline
term_list<Term> push_front(term_list<Term> l, Term elem)
{
  return term_list<Term>(l, elem);
}

/// \brief Returns the list obtained by inserting a new element at the end. Note
/// that the complexity of this function is O(n), with n the number of
/// elements in the list!!!
/// \param l A list.
/// \param elem A term
/// \return The list with an element appended to it.
template <typename Term>
inline
term_list<Term> push_back(term_list<Term> list, const Term &elem)
{
  size_t len = list.size();
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,_ATerm*,len);

  /* Collect all elements of list in buffer */
  for (size_t i=0; i<len; i++)
  {
    buffer[i] = &*list.head();
    list = list.tail();
  }

  term_list<Term> result(term_list<Term>(),elem);

  /* Insert elements at the front of the list */
  for (size_t i=len; i>0; i--)
  {
    result = term_list<Term>(result, Term(buffer[i-1]));
  }

  return result;
}

/// \brief Returns the list obtained by removing the first element.
/// \param l A list.
/// \return The list with the first element removed.
template <typename Term>
inline
term_list<Term> pop_front(const term_list<Term> &l)
{
  return l.tail();
}

/// \brief Returns the list with the elements in reversed order.
/// \param l A list.
/// \return The reversed list.
template <typename Term>
inline
term_list<Term> reverse(term_list<Term> l)
{
  term_list<Term> result;
  while (!l.empty())
  {
    result = term_list<Term>(result, l.head());
    l = l.tail(); 
  }
  return result;
}

/// \brief Returns the list l with one occurrence of the element x removed, or l if x is not present.
/// \param l A list.
/// \param x A list element.
template <typename Term>
inline
term_list<Term> remove_one_element(const term_list<Term> &list, const Term &t)
{
  size_t i = 0;
  term_list<Term> l = list;
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,_ATerm*,list.size());

  while (l!=term_list<Term>())
  {
    if (l.head()==t)
    {
      break;
    }
    buffer[i++] = &*l.head();
    l = l.tail();
  }

  if (l.empty())
  {
    return list;
  }

  l = l.tail();
  term_list<Term> result = l; /* Skip element to be removed */

  /* We found the element. Add all elements prior to this
        one to the tail of the list. */
  for ( ; i>0; i--)
  {
    result = term_list<Term>(result, Term(buffer[i-1]));
  }

  return result;
}

/// \brief Applies a function to all elements of the list and returns the result.
/// \param l The list that is transformed.
/// \param f The function that is applied to the elements of the list.
/// \return The transformed list.
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

/// \brief Returns the concatenation of two lists.
/// \param l A list.
/// \param m A list.
/// \return The concatenation of the lists.
template <typename Term>
inline
term_list<Term> operator+(const term_list<Term> &l, const term_list<Term> &m)
{

  if (m.empty())
  {
    return l;
  }

  size_t len = l.size();

  if (len == 0)
  {
    return m;
  }

  term_list<Term> result = m;
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,_ATerm*,len);
  /* Collect the elements of list1 in buffer */
  term_list<Term> list1=l;
  for (size_t i=0; i<len; i++)
  {
    buffer[i] = &*list1.head();
    list1 = list1.tail();
  }

  /* Insert elements at the front of the list */
  for (size_t i=len; i>0; i--)
  {
    result = term_list<Term>(result, Term(buffer[i-1]));
  }

  return result;
}


/// \brief Returns an element at a certain position in a list
/// \param l A list
/// \param i An index. The first element is at position 0.
/// \return The element at position i in the list l.
template <typename Term>
inline
Term element_at(const term_list<Term> &l, size_t m)
{
  typename term_list<Term>::const_iterator i=l.begin();
  for( ; m>0; --m, ++i)
  {
    assert(i!=l.end());
  }
  assert(i!=l.end());
  return *i;
}

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_LIST_H
