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
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/detail/aterm_conversion.h"
#include "mcrl2/atermpp/aterm_list_iterator.h"

namespace atermpp
{

/// \cond INTERNAL_DOCS
namespace detail
{
// In the ATerm library the following functions are #define's:
//
// #define   ATgetNext(l)  ((l)->tail)
// #define   ATgetFirst(l) ((l)->head)
//
// So we need to turn them into proper functions.

/// \brief Function that calls the macro ATgetNext
/// \param l A sequence of terms
/// \return The result of calling ATgetNext
inline ATermList aterm_get_next(ATermList l)
{
  return ATgetNext(l);
}

/// \brief Function that calls the macro ATgetFirst
/// \param l A sequence of terms
/// \return The result of calling ATgetFirst
inline ATerm aterm_get_first(ATermList l)
{
  return ATgetFirst(l);
}

/// \brief Function that calls the macro ATgetLength
/// \param l A sequence of terms
/// \return The result of calling ATgetLength
inline
size_t aterm_get_length(ATermList l)
{
  return ATgetLength(l);
}

} // namespace detail
/// \endcond

/// \brief Read-only singly linked list of terms.
template <typename Term>
class term_list: public aterm_base
{
  protected:
    /// \brief Returns the wrapped ATermList.
    /// \return The wrapped ATermList.
    ATermList list() const
    {
      return (ATermList)m_term;
    }

    /// \brief Returns the wrapped ATermList.
    /// \return The wrapped ATermList.
    ATermList list()
    {
      return (ATermList)m_term;
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
    term_list()
      : aterm_base(ATmakeList0())
    {}

    /// Construction from ATermList.
    /// \param l A list.
    term_list(ATermList l)
      : aterm_base(l)
    {
      assert(type() == AT_LIST);
    }

    /// Construction from aterm_list.
    /// \param t A term containing a list.
    template <typename SpecificTerm>
    term_list(const term_list<SpecificTerm>& t);

    /// Allow construction from an aterm. The aterm must be of the right type.
    /// \param t A term containing a list.
    term_list(aterm t)
      : aterm_base(t)
    {
      assert(type() == AT_LIST);
    } 

    /// Creates an term_list with a copy of a range.
    /// \param first The start of a range of elements.
    /// \param last The end of a range of elements.
    template <class Iter>
    term_list(Iter first, Iter last, typename boost::enable_if<
              typename boost::is_convertible< typename boost::iterator_traversal< Iter >::type,
              boost::random_access_traversal_tag >::type >::type* = 0)
      : aterm_base(ATmakeList0())
    {
      while (first != last)
      {
        m_term = detail::void2term(detail::list2void(ATinsert(list(), aterm(*(--last)))));
      }
    }

    /// Creates an term_list with a copy of a range.
    /// \param first The start of a range of elements.
    /// \param last The end of a range of elements.
    template <class Iter>
    term_list(Iter first, Iter last, typename boost::disable_if<
              typename boost::is_convertible< typename boost::iterator_traversal< Iter >::type,
              boost::random_access_traversal_tag >::type >::type* = 0)
      : aterm_base(ATmakeList0())
    {
      while (first != last)
      {
        m_term = detail::void2term(detail::list2void(ATinsert(list(), aterm(*(first++)))));
      }
      m_term = detail::void2term(detail::list2void(ATreverse(list())));
    }


    /// Assignment operator.
    /// \param t A term containing a list.
    term_list<Term>& operator=(aterm_base t)
    {
      assert(t.type() == AT_LIST);
      m_term = aterm_traits<aterm_base>::term(t);
      return *this;
    }

    /// Assignment operator.
    /// \param t A term containing a list.
    term_list<Term>& operator=(ATermList t)
    {
      assert(ATgetType(t) == AT_LIST);
      m_term = t;
      return *this;
    }

    /// \brief Returns a const_iterator pointing to the beginning of the term_list.
    /// \return The beginning of the list.
    const_iterator begin() const
    {
      return const_iterator(list());
    }

    /// \brief Returns a const_iterator pointing to the end of the term_list.
    /// \return The end of the list.
    const_iterator end() const
    {
      return const_iterator(ATmakeList0());
    }

    /// \brief Returns the size of the term_list.
    /// \return The size of the list.
    size_type size() const
    {
      return detail::aterm_get_length(list());
    }

    /// \brief Returns the largest possible size of the term_list.
    /// \return The largest possible size of the list.
    size_type max_size() const
    {
      return GET_LENGTH((std::numeric_limits<unsigned long>::max)());
    }

    /// \brief Returns true if the list's size is 0.
    /// \return True if the list is empty.
    bool empty() const
    {
      return ATisEmpty(list()) == true;
    }

    /// \brief Returns the first element.
    /// \return The first element of the list.
    Term front() const
    {
      return Term(detail::void2appl(detail::term2void(detail::aterm_get_first(list()))));
    }

    /// \brief Returns an iterator prev such that ++prev == pos. Complexity: linear in the number of iterators in the range [begin(), pos).
    /// \param pos An iterator that points to an element in the list.
    /// \return An iterator that points to the previous element in the list.
    const_iterator previous(const_iterator pos) const
    {
      const_iterator prev = end();
      for (const_iterator i = begin(); i != end(); ++i)
      {
        if (i == pos)
        {
          return prev;
        }
        prev = i;
      }
      return end();
    }

    /// \brief Conversion to ATermList.
    /// \return The wrapped ATermList pointer.
    operator ATermList() const
    {
      // return detail::void2list(m_term);
      return (ATermList)m_term;
    }

    /// \brief Applies a low level substitution function to this term and returns the result.
    /// \param f A
    /// The function <tt>f</tt> must supply the method <tt>aterm operator()(aterm)</tt>.
    /// This function is applied to all <tt>aterm</tt> nodes appearing in this term.
    /// \deprecated
    /// \return The substitution result.
    template <typename Substitution>
    term_list<Term> substitute(Substitution f) const
    {
      return term_list<Term>(f(*this));
    }
};

/// \brief A term_list with elements of type aterm.
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
  return term_list<Term>(ATinsert(l, aterm_traits<Term>::term(elem)));
}

/// \brief Returns the list obtained by inserting a new element at the end. Note
/// that the complexity of this function is O(n), with n the number of
/// elements in the list!!!
/// \param l A list.
/// \param elem A term
/// \return The list with an element appended to it.
template <typename Term>
inline
term_list<Term> push_back(term_list<Term> l, Term elem)
{
  return term_list<Term>(ATappend(l, aterm_traits<Term>::term(elem)));
}

/// \brief Returns the list obtained by removing the first element.
/// \param l A list.
/// \return The list with the first element removed.
template <typename Term>
inline
term_list<Term> pop_front(term_list<Term> l)
{
  return term_list<Term>(detail::aterm_get_next(l));
}

/// \brief Returns the list with the elements in reversed order.
/// \param l A list.
/// \return The reversed list.
template <typename Term>
inline
term_list<Term> reverse(term_list<Term> l)
{
  return term_list<Term>(ATreverse(l));
}

/// \brief Returns the list l with one occurrence of the element x removed, or l if x is not present.
/// \param l A list.
/// \param x A list element.
template <typename Term>
inline
term_list<Term> remove_one_element(const term_list<Term>& l, const Term& x)
{
  return term_list<Term>(ATremoveElement(l, aterm_traits<Term>::term(x)));
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
term_list<Term> operator+(term_list<Term> l, term_list<Term> m)
{
  return term_list<Term>(ATconcat(l, m));
}

/// \brief Appends an element to a list.
/// \param l A list.
/// \param t A term
/// \return The list with an element appended to it.
template <typename Term>
inline
term_list<Term> operator+(term_list<Term> l, Term t)
{
  assert(t.type() != AT_FREE);
  return term_list<Term>(ATappend(l, aterm_traits<Term>::term(t)));
}

/// \brief Appends an element to a list.
/// \param t A term
/// \param l A list.
/// \return The list with one element appended to it.
template <typename Term>
inline
term_list<Term> operator+(Term t, term_list<Term> l)
{
  return push_front(l, t);
}

/// \cond INTERNAL_DOCS
template <typename Term>
struct aterm_traits<term_list<Term> >
{
  static void protect(const term_list<Term>& t)
  {
    t.protect();
  }
  static void unprotect(const term_list<Term>& t)
  {
    t.unprotect();
  }
  static void mark(const term_list<Term>& t)
  {
    t.mark();
  }
  static ATerm term(const term_list<Term>& t)
  {
    return t.term();
  }
};
/// \endcond

/// \brief Equality operator.
/// \param x A list.
/// \param y A list.
/// \return True if the arguments are equal.
template <typename Term>
bool operator==(const term_list<Term>& x, const term_list<Term>& y)
{
  return ATisEqual(aterm_traits<term_list<Term> >::term(x), aterm_traits<term_list<Term> >::term(y)) == true;
}

/// \brief Equality operator.
/// \param x A list.
/// \param y A list.
/// \return True if the arguments are equal.
template <typename Term>
bool operator==(const term_list<Term>& x, ATermList y)
{
  return ATisEqual(aterm_traits<term_list<Term> >::term(x), y) == true;
}

/// \brief Equality operator.
/// \param x A list.
/// \param y A list.
/// \return True if the arguments are equal.
template <typename Term>
bool operator==(ATermList x, const term_list<Term>& y)
{
  return ATisEqual(x, aterm_traits<term_list<Term> >::term(y)) == true;
}

/// \brief Inequality operator.
/// \param x A list.
/// \param y A list.
/// \return True if the arguments are not equal.
template <typename Term>
bool operator!=(const term_list<Term>& x, const term_list<Term>& y)
{
  return ATisEqual(aterm_traits<term_list<Term> >::term(x), aterm_traits<term_list<Term> >::term(y)) == false;
}

/// \brief Inequality operator.
/// \param x A list.
/// \param y A list.
/// \return True if the arguments are not equal.
template <typename Term>
bool operator!=(const term_list<Term>& x, ATermList y)
{
  return ATisEqual(aterm_traits<term_list<Term> >::term(x), y) == false;
}

/// \brief Inequality operator.
/// \param x A list.
/// \param y A list.
/// \return True if the arguments are not equal.
template <typename Term>
bool operator!=(ATermList x, const term_list<Term>& y)
{
  return ATisEqual(x, aterm_traits<term_list<Term> >::term(y)) == false;
}

// implementation
template <typename Term>
template <typename SpecificTerm>
term_list<Term>::term_list(const term_list<SpecificTerm>& t)
  : aterm_base(t)
{}

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_LIST_H
