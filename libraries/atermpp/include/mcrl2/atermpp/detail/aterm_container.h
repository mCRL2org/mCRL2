// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_ATERMPP_DETAIL_ATERM_CONTAINER_H
#define MCRL2_ATERMPP_DETAIL_ATERM_CONTAINER_H

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/detail/aterm.h"
#include "mcrl2/atermpp/detail/aterm_pool_storage_implementation.h"

#include <stack>

namespace atermpp::detail
{

/// \brief Provides safe storage of unprotected_aterm instances in a container by marking
///        them during garbage collection.
class aterm_container
{
public:
  inline aterm_container();
  virtual inline ~aterm_container();

  /// \brief Ensure that all the terms in the containers.
  virtual void mark(std::stack<std::reference_wrapper<detail::_aterm>>& todo) const = 0;

  /// \brief Copy constructor
  inline aterm_container(const aterm_container& c);

  /// \brief Move constructor
  inline aterm_container(aterm_container&& c);

  /// \brief Assignment This may have to be redefined in due time. 
  /* aterm_container& operator=(const aterm_container& c)
  {
    return *this;
  }

  /// \brief Move assignment
  aterm_container& operator=(aterm_container&& c)
  {
    return *this;
  } */

};

/// \brief An unprotected term that is stored inside an aterm_container.
template<typename T>
class reference_aterm : public unprotected_aterm
{
public:
  /// \brief Default constructor.
  reference_aterm() noexcept = default;

  explicit reference_aterm(const unprotected_aterm& other) noexcept
  {
    m_term = detail::address(other);
  }

  reference_aterm(unprotected_aterm&& other) noexcept
  {
    m_term = detail::address(other);
  }

  const reference_aterm& operator=(const unprotected_aterm& other) noexcept
  {
    m_term = detail::address(other);
    return reinterpret_cast<const reference_aterm&>(m_term);
  }

  /// Converts implicitly to a protected term of type T.
  operator T&()
  {
    static_assert(std::is_base_of<aterm, T>::value,"Term must be derived from an aterm");
    static_assert(sizeof(T)==sizeof(std::size_t),"Term derived from an aterm must not have extra fields");
    return reinterpret_cast<T&>(*this);
  }

  operator const T&() const
  {
    static_assert(std::is_base_of<aterm, T>::value,"Term must be derived from an aterm");
    static_assert(sizeof(T)==sizeof(std::size_t),"Term derived from an aterm must not have extra fields");
    return reinterpret_cast<const T&>(*this);
  }
};

template<typename Container>
class generic_aterm_container : public aterm_container
{
public:
  /// \construct
  generic_aterm_container(const Container& container, bool)
   : m_container(container)
  {}

  /// \brief Provides access to the underlying container.
  // Container& container() { return m_container; }
  // const Container& container() const { return m_container; }

  void mark(std::stack<std::reference_wrapper<detail::_aterm>>& todo) const override
  {
    for (const aterm& element: m_container) 
    {
      // Mark all terms (and their subterms) that are reachable, i.e the root set.
      detail::_aterm* term = detail::address(element);
      if (element.defined() && !term->is_marked())
      {
        // Mark the term itself as reachable.
        term->mark();

        // This variable is not a default term and that term has not been marked.
        mark_term(*term, todo);
      }
    }
  }

protected:
  const Container& m_container;
};

} // namespace atermpp::detail

namespace std
{

/// \brief specialization of the standard std::hash function.
template<class T>
struct hash<atermpp::detail::reference_aterm<T>>
{
  std::size_t operator()(const atermpp::detail::reference_aterm<T>& t) const
  {
    return std::hash<atermpp::aterm>()(t);
  }
};

} // namespace std

#endif // MCRL2_ATERMPP_DETAIL_ATERM_CONTAINER_H
