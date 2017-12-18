// Author(s): Jan Friso Groote, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/detail/aterm_list_iterator.h
/// \brief Iterator for term_list.

#ifndef MCRL2_ATERMPP_ATERM_LIST_ITERATOR_H
#define MCRL2_ATERMPP_ATERM_LIST_ITERATOR_H

#include "mcrl2/atermpp/aterm.h"

namespace atermpp
{

/// \cond INTERNAL_DOCS
namespace detail
{
  template <class Term>
  class _aterm_list;
}

/// \endcond

/// \brief Iterator for term_list.
template <typename Term>
class term_list_iterator 
{
    template<class T>
    friend class term_list;

  protected:
    detail::_aterm_list<Term>* m_list;

    /// \brief Constructor from an aterm which must be a list.
    /// \param l A sequence of terms
    term_list_iterator(detail::_aterm* l)
      : m_list(reinterpret_cast<detail::_aterm_list<Term>*>(l))
    { 
      assert(l->function()==detail::function_adm.AS_LIST || l->function()==detail::function_adm.AS_EMPTY_LIST);
    } 

  public:
    typedef Term value_type;
    typedef Term& reference;
    typedef Term* pointer;
    typedef ptrdiff_t difference_type;
    typedef std::forward_iterator_tag iterator_category;

    /// \brief Default constructor.
    term_list_iterator()
      : m_list(nullptr)
    {}

    /// \brief Copy constructor.
    /// \param l A sequence of terms
    term_list_iterator(const term_list_iterator& other)
      : m_list(other.m_list)
    { 
    } 

    /// \brief Assignment
    /// \param l A sequence of terms
    term_list_iterator& operator=(const term_list_iterator& other)
    { 
      m_list=other.m_list;
      return *this;
    } 

    /// \brief Dereference operator on an iterator
    const Term& operator*() const
    {
      assert(m_list->function()==detail::function_adm.AS_LIST);
      return m_list->head;
    }

    /// Arrow operator on an iterator
    const Term* operator->() const
    {
      assert(m_list->function()==detail::function_adm.AS_LIST);
      return &m_list->head;
    }
    
    /// \brief Prefix increment operator on iterator.
    term_list_iterator& operator++()
    {
      assert(m_list->function()==detail::function_adm.AS_LIST);
      m_list = reinterpret_cast<detail::_aterm_list<Term>*>(detail::address(m_list->tail));
      return *this;
    }

    /// \brief Postfix increment operator on iterator.
    term_list_iterator operator++(int)
    {
      assert(m_list->function()==detail::function_adm.AS_LIST);
      const term_list_iterator temp = *this;
      m_list = reinterpret_cast<detail::_aterm_list<Term>*>(detail::address(m_list->tail));
      return temp;
    }

    /// \brief Equality of iterators.
    /// \param other The iterator with which this iterator is compared.
    /// \return true if the iterators point to the same term_list.
    bool operator ==(const term_list_iterator& other) const
    {
      return m_list == other.m_list;
    }

    /// \brief Inequality of iterators.
    /// \param other The iterator with which this iterator is compared.
    /// \return true if the iterators do not point to the same term_list.
    bool operator !=(const term_list_iterator& other) const
    {
      return m_list != other.m_list;
    }

    /// \brief Comparison of iterators.
    /// \param other The iterator with which this iterator is compared.
    /// \return true if the pointer to this termlist is smaller than the other pointer.
    bool operator <(const term_list_iterator& other) const
    {
      return m_list < other.m_list;
    }

    /// \brief Comparison of iterators.
    /// \param other The iterator with which this iterator is compared.
    /// \return true if the iterators point to the same term_list.
    bool operator <=(const term_list_iterator& other) const
    {
      return m_list <= other.m_list;
    }

    /// \brief Comparison of iterators.
    /// \param other The iterator with which this iterator is compared.
    /// \return true if the iterators point to the same term_list.
    bool operator >(const term_list_iterator& other) const
    {
      return m_list > other.m_list;
    }

    /// \brief Comparison of iterators.
    /// \param other The iterator with which this iterator is compared.
    /// \return true if the iterators point to the same term_list.
    bool operator >=(const term_list_iterator& other) const
    {
      return m_list >= other.m_list;
    }

};

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_LIST_ITERATOR_H
