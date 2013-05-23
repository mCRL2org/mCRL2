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
#include "mcrl2/atermpp/detail/aterm_list_iterator.h"
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include "mcrl2/atermpp/detail/aterm_list.h"

namespace atermpp
{


template <typename Term>
class term_list:public aterm
{
  public:

    /// The type of object, T stored in the term_list.
    typedef Term value_type;

    /// Pointer to T.
    typedef Term* pointer;

    /// Reference to T.
    typedef Term& reference;

    /// Const reference to T.
    typedef const Term &const_reference;

    /// An unsigned integral type.
    typedef size_t size_type;

    /// A signed integral type.
    typedef ptrdiff_t difference_type;

    /// Iterator used to iterate through an term_list.
    typedef term_list_iterator<Term> iterator;

    /// Const iterator used to iterate through an term_list.
    typedef term_list_iterator<Term> const_iterator;

    /// Default constructor. Creates an empty list.
    term_list ():aterm(aterm::empty_aterm_list())
    {
    }

    /// \brief Copy constructor.
    /// \param t A list.
    term_list(const term_list<Term> &t):aterm(t)
    {
      assert(!defined() || type_is_list());
    }

    /// \brief Explicit construction from an aterm.
    /// \param t An aterm.
    explicit term_list(const aterm &t):aterm(t)
    {
      BOOST_STATIC_ASSERT((boost::is_base_of<aterm, Term>::value));
      BOOST_STATIC_ASSERT(sizeof(Term)==sizeof(aterm));
      // Term list can be undefined; Generally, this is used to indicate an error situation.
      // This use should be discouraged. For this purpose exceptions ought to be used.
      assert(!defined() || type_is_list());
    }

    /// \brief Creates a term_list with the elements from first to last.
    /// \details It is assumed that the range can be traversed from last to first.
    /// \param first The start of a range of elements.
    /// \param last The end of a range of elements.
    template <class Iter>
    term_list(Iter first, Iter last, typename boost::enable_if<
              typename boost::is_convertible< typename boost::iterator_traversal< Iter >::type,
              boost::random_access_traversal_tag >::type >::type* = 0):
        aterm(detail::make_list_backward<Term,Iter,
                  detail::do_not_convert_term<Term> >(first, last,detail::do_not_convert_term<Term>()))
    {
      assert(!defined() || type_is_list());
    }

    /// \brief Creates a term_list with the elements from first to last.
    /// \details It is assumed that the range can be traversed from last to first.
    /// \param first The start of a range of elements.
    /// \param last The end of a range of elements.
    /// \param convert_to_aterm A class with a () operation, which is applied to each element
    ///                   before it is put into the list.
    template <class Iter, class ATermConverter>
    term_list(Iter first, Iter last, const ATermConverter &convert_to_aterm, typename boost::enable_if<
              typename boost::is_convertible< typename boost::iterator_traversal< Iter >::type,
              boost::random_access_traversal_tag >::type >::type* = 0):
         aterm(detail::make_list_backward<Term,Iter,ATermConverter>(first, last, convert_to_aterm))
    {
      assert(!defined() || type_is_list());
    }

    /// \brief Creates a term_list from the elements from first to last.
    /// \details The range is traversed from first to last. This requires
    ///           to copy the elements internally, which is less efficient
    ///           than this function with random access iterators as arguments.
    /// \param first The start of a range of elements.
    /// \param last The end of a range of elements.
    template <class Iter>
             term_list(Iter first, Iter last, typename boost::disable_if<
             typename boost::is_convertible< typename boost::iterator_traversal< Iter >::type,
             boost::random_access_traversal_tag >::type >::type* = 0):
         aterm(detail::make_list_forward<Term,Iter,detail::do_not_convert_term<Term> >
                                 (first, last, detail::do_not_convert_term<Term>()))
    {
      assert(!defined() || type_is_list());
    }

    /// \brief Creates a term_list from the elements from first to last.
    /// \details The range is traversed from first to last. This requires
    ///           to copy the elements internally, which is less efficient
    ///           than this function with random access iterators as arguments.
    /// \param first The start of a range of elements.
    /// \param last The end of a range of elements.
    ///  \param convert_to_aterm A class with a () operation, whic is applied to each element
    ///                      before it is put into the list.
    template <class Iter, class  ATermConverter>
             term_list(Iter first, Iter last, const ATermConverter &convert_to_aterm, typename boost::disable_if<
             typename boost::is_convertible< typename boost::iterator_traversal< Iter >::type,
             boost::random_access_traversal_tag >::type >::type* = 0):
         aterm(detail::make_list_forward<Term,Iter,ATermConverter>
                                 (first, last, convert_to_aterm))
    {
      assert(!defined() || type_is_list());
    }

    /// Assigment operator.
    /// \param l A list.
    term_list<Term> &operator=(const term_list &l)
    {
      copy_term(l);
      return *this;
    }

    /// \brief Conversion to aterm_list.
    /// \deprecated
    /// \return This list as an aterm_list.
    operator term_list<aterm>() const
    {
      return atermpp::aterm_cast<term_list<aterm> >(*this);
    }

    /// \brief Returns the tail of the list.
    /// \return The tail of the list.
    const term_list<Term> &tail() const
    {
      assert(!empty());
      return (reinterpret_cast<const detail::_aterm_list<Term>*>(m_term))->tail;
    }

    /// \brief Removes the first element of the list.
    void pop_front()
    {
      *this=tail();
    }

    /// \brief Returns the first element of the list.
    /// \return The term at the head of the list.
    const Term &front() const
    {
      return reinterpret_cast<const detail::_aterm_list<Term>*>(m_term)->head;
    }

    /// \brief Inserts a new element at the beginning of the current list.
    /// \param el The term that is added.
    void push_front(const Term &el);

    /// \brief Returns the size of the term_list.
    /// \details The complexity of this function is linear in the size of the list.
    /// \return The size of the list.
    size_type size() const
    {
      size_t size=0;
      for(const_iterator i=begin(); i!=end(); ++i)
      {
        ++size;
      }
      return size;
    }

    /// \brief Returns true if the list's size is 0.
    /// \return True iff the list is empty.
    bool empty() const
    {
      return m_term->function()==detail::function_adm.AS_EMPTY_LIST;
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
      return const_iterator(detail::static_empty_aterm_list);
    }

    /// \brief Returns the largest possible size of the term_list.
    /// \return The largest possible size of the list.
    size_type max_size() const
    {
      return (std::numeric_limits<size_t>::max)();
    }
};

/// \cond INTERNAL_DOCS
namespace detail
{

template <class Term>
class _aterm_list:public _aterm
{
  public:
    Term head;
    term_list<Term> tail;
};

static const size_t TERM_SIZE_LIST = sizeof(detail::_aterm_list<aterm>)/sizeof(size_t);
} // namespace detail
/// \endcond


/// \brief A term_list with elements of type aterm.
typedef term_list<aterm> aterm_list;


/// \brief Returns the list with the elements in reversed order.
/// \param l A list.
/// \details This operator is linear in the size of the list.
/// \return The reversed list.
template <typename Term>
inline
term_list<Term> reverse(const term_list<Term> &l);


/// \brief Returns the list l with one occurrence of the element x removed, or l if x is not present.
/// \param l A list.
/// \param t A list element.
/// \details This operator is linear in the length of the list.
/// \return The original list where the first occurrence of t has been removed, assuming it is in t.
template <typename Term>
inline
term_list<Term> remove_one_element(const term_list<Term> &l, const Term &t);


/// \brief Returns the concatenation of two lists.
/// \param l A list.
/// \param m A list.
/// \details The complexity of this operator is linear in the length of l.
/// \return The concatenation of the lists l followed by m..
template <typename Term>
inline
term_list<Term> operator+(const term_list<Term> &l, const term_list<Term> &m);


/// \brief Returns an element at a certain position in a list
/// \param l A list
/// \param m An index. The first element is at position 0.
/// \details This operator is linear in the number m. If m>=length of the list
///          the result is undefined.
/// \return The element at position i in the list l.
template <typename Term>
inline
const Term &element_at(const term_list<Term> &l, size_t m)
{
  assert(l.size()>m);
  typename term_list<Term>::const_iterator i=l.begin();
  for( ; m>0; --m, ++i) {}
  return *i;
}

/// \brief Appends a new element at the end of the list. Note
/// that the complexity of this function is O(n), with n the number of
/// elements in the list!!!
///  \param l The list to which the term is appended.
/// \param el A term.
/// \return The list l with elem appended at the end.
template <typename Term>
inline
term_list<Term> push_back(const term_list<Term> &l, const Term &el);

} // namespace atermpp


namespace std
{

/// \brief Swaps two term_lists.
/// \details This operation is more efficient than exchanging terms by an assignment,
///          as swapping does not require to change the protection of terms.
/// \param t1 The first term
/// \param t2 The second term

template <class T>
inline void swap(atermpp::term_list<T> &t1, atermpp::term_list<T> &t2)
{
  t1.swap(t2);
}
} // namespace std

#include "mcrl2/atermpp/detail/aterm_list_implementation.h"

#endif // MCRL2_ATERMPP_ATERM_LIST_H
