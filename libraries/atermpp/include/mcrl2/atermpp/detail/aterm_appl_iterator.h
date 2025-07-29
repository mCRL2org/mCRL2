// Author(s): Jan Friso Groote, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_appl_iterator.h
/// \brief Iterator for term_appl.

#ifndef MCRL2_ATERMPP_DETAIL_ATERM_APPL_ITERATOR_H
#define MCRL2_ATERMPP_DETAIL_ATERM_APPL_ITERATOR_H

#include "mcrl2/atermpp/detail/aterm.h"

namespace atermpp
{

/// \brief Iterator for term_appl.
template <typename Term>
class term_appl_iterator
{
    friend class aterm;

    template <class Derived, class Base>
    friend term_appl_iterator<Derived> detail::aterm_appl_iterator_cast(term_appl_iterator<Base> a,
        std::enable_if_t<std::is_base_of_v<aterm, Base> && std::is_base_of_v<aterm, Derived>>*);


  protected:
    const Term* m_term;
    
    /// \brief Constructor.
    /// \param t A pointer of an array of terms over which the iterator will range.
    term_appl_iterator(const Term* t)
      : m_term(t)
    {}

  public:
    using value_type = Term;
    using reference = const Term&;
    using pointer = const Term*;
    using difference_type = ptrdiff_t;
    using iterator_category = std::random_access_iterator_tag;

    /// \brief The copy constructor.
    /// \param other The iterator that is copy constructed.
    term_appl_iterator(const term_appl_iterator& other)
      : m_term(other.m_term)
    {}

    /// \brief The assignment operator.
    /// \param other The term to be assigned.
    /// \return A reference to the assigned iterator.
    term_appl_iterator& operator=(const term_appl_iterator& other) = default;

    /// \brief The dereference operator.
    /// \return The dereferenced term.
    const Term& operator*() const
    {
      return *m_term;
    }

    /// \brief Dereference the current iterator.
    /// \return The dereference term.
    const Term* operator->() const
    {
      return m_term;
    }

    /// \brief The dereference operator.
    /// \param n The index of the element to be dereferenced.
    /// \return The dereferenced term.
    const Term& operator[](difference_type n) const
    {
      return *(m_term+n);
    }

    /// \brief Prefix increment.
    /// \return The iterator after it is incremented.
    term_appl_iterator& operator++()
    {
      ++m_term;
      return *this;
    }

    /// \brief Postfix increment.
    /// \return The iterator before incrementing it.
    term_appl_iterator operator++(int)
    {
      term_appl_iterator temp=*this;
      ++m_term;
      return temp;
    }

    /// \brief Prefix decrement.
    /// \return The iterator after decrementing it.
    term_appl_iterator& operator--()
    {
      --m_term;
      return *this;
    }

    /// \brief Post decrement an iterator.
    /// \return The iterator before decrementing it.
    term_appl_iterator operator--(int)
    {
      term_appl_iterator temp=*this;
      --m_term;
      return *this;
    }

    /// \brief Increase the iterator with n steps.
    /// \param n The difference with which the iterator is increased.
    /// \return The increased iterator.
    term_appl_iterator& operator+=(difference_type n)
    {
      m_term+=n;
      return *this;
    }

    /// \brief Decrease the iterator with n steps.
    /// \param n a difference with which the iterator is decreased.
    /// \return The decreased iterator.
    term_appl_iterator& operator-=(difference_type n)
    {
      m_term-=n;
      return *this;
    }

    /// \brief Increase by a constant value.
    /// \return The iterator incremented by n.
    term_appl_iterator operator+(ptrdiff_t n) const
    {
      term_appl_iterator temp=*this;
      return temp.m_term+n;
    }

    /// \brief Decrease by a constant value.
    /// \return The iterator decremented by n.
    term_appl_iterator operator-(ptrdiff_t n) const
    {
      term_appl_iterator temp=*this;
      return temp.m_term-n;
    }

    /// \brief The negative distance from this to the other iterator.
    /// \param other the other iterator.
    /// \return The negative distance: this-other.
    ptrdiff_t operator-(const term_appl_iterator& other) const
    {
      return m_term-other.m_term;
    }

    /// \brief Provide the distance to the other iterator.
    /// \param other the other iterator.
    /// \return the distance from other to this iterator.
    ptrdiff_t distance_to(const term_appl_iterator& other) const
    {
      return other.m_term-m_term;
    }


    /// \brief Equality of iterators.
    /// \param other The iterator with which this iterator is compared.
    /// \return true if the iterators point to the same term_list.
    bool operator ==(const term_appl_iterator& other) const
    {
      return m_term == other.m_term;
    }

    /// \brief Inequality of iterators.
    /// \param other The iterator with which this iterator is compared.
    /// \return true if the iterators do not point to the same term_appl.
    bool operator !=(const term_appl_iterator& other) const
    {
      return m_term != other.m_term;
    }

    /// \brief Comparison of iterators.
    /// \param other The iterator with which this iterator is compared.
    /// \return true if the pointer to this termterm is smaller than the other pointer.
    bool operator <(const term_appl_iterator& other) const
    {
      return m_term < other.m_term;
    }

    /// \brief Comparison of iterators.
    /// \param other The iterator with which this iterator is compared.
    /// \return true if the iterators point to the same term_appl.
    bool operator <=(const term_appl_iterator& other) const
    {
      return m_term <= other.m_term;
    }

    /// \brief Comparison of iterators.
    /// \param other The iterator with which this iterator is compared.
    /// \return true if the iterators point to the same term_appl.
    bool operator >(const term_appl_iterator& other) const
    {
      return m_term > other.m_term;
    }

    /// \brief Comparison of iterators.
    /// \param other The iterator with which this iterator is compared.
    /// \return true if the iterators point to the same term_appl.
    bool operator >=(const term_appl_iterator& other) const
    {
      return m_term >= other.m_term;
    }
};

namespace detail
{
  /// This function can be used to translate an term_appl_iterator of one sort into another.
template <class Derived, class Base>
term_appl_iterator<Derived> aterm_appl_iterator_cast(term_appl_iterator<Base> a,
    std::enable_if_t<std::is_base_of_v<aterm, Base> && std::is_base_of_v<aterm, Derived>>* /* = nullptr */)
{
  static_assert(sizeof(Derived) == sizeof(_aterm*),
      "term_appl_iterator only works on aterm classes to which no extra fields are added");
  static_assert(sizeof(Base) == sizeof(_aterm*),
      "term_appl_iterator only works on aterm classes to which no extra fields are added");
  return term_appl_iterator<Derived>(reinterpret_cast<const Derived*>(a.m_term));
  }

} // namespace detail

} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ATERM_APPL_ITERATOR_H
