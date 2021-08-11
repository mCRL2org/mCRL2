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

#include <stack>
#include <type_traits>
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/detail/aterm.h"
#include "mcrl2/atermpp/detail/aterm_pool_storage_implementation.h"


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
  aterm_container& operator=(const aterm_container& )
  {
    return *this;
  }

  /// \brief Move assignment
  aterm_container& operator=(aterm_container&& )
  {
    return *this;
  } 

};

template<class T, typename Type = void >  
class reference_aterm;


template<class T>
struct is_pair_helper : public std::false_type
{};

template<class T, class U>
struct is_pair_helper<std::pair<T,U> > : public std::true_type
{};

template<class T>
struct is_pair : public is_pair_helper<typename std::decay<T>::type >
{};

template<class T>
struct is_reference_aterm_helper : public std::false_type
{};

template<class T>
struct is_reference_aterm_helper<reference_aterm<T> > : public std::true_type
{};

template<class T>
struct is_reference_aterm : public is_reference_aterm_helper<typename std::decay<T>::type >
{};

/// \brief Base class that should not be used. 
template<class T, typename Type >
class reference_aterm
{
  reference_aterm(const T& other) = delete;
  reference_aterm(T&& other) = delete;
  T& operator=(const T& other) = delete;
  T& operator=(T&& other) = delete;

};

/// \brief An unprotected term that is stored inside an aterm_container.
template<class T>
class reference_aterm < T, typename std::enable_if<std::is_fundamental<typename std::decay<T>::type>::value>::type >
{
protected:
  typedef typename std::decay<T>::type T_type;

public:

  T_type m_t;

  /// \brief Default constructor.
  reference_aterm() noexcept = default;
  
  reference_aterm(const T& other) noexcept
   : m_t(other)
  { }
  
  reference_aterm(T_type&& other) noexcept  
   : m_t(std::move(other))
  {} 
  
  const T& operator=(const T& other) noexcept
  {
    static_assert(std::is_base_of<aterm, T>::value);
    m_t=other;
    return m_t;
  } 

  /// Converts implicitly to a protected term of type T.
  operator T&()
  {
    return m_t;
  } 

  operator const T&() const
  {
    return m_t;
  }

  void mark(std::stack<std::reference_wrapper<detail::_aterm>>& /* todo */) const
  {
    /* Do nothing */
  } 

  bool operator==(const reference_aterm& other) const
  {
    return m_t==other.m_t;
  }

};

/// \brief An unprotected term that is stored inside an aterm_container.
template<typename T>
class reference_aterm<T, typename std::enable_if<std::is_base_of<aterm, T>::value>::type> : public unprotected_aterm
{
public:
  /// \brief Default constructor.
  reference_aterm() noexcept = default;

  explicit reference_aterm(const unprotected_aterm& other) noexcept
  {
    m_term = detail::address(other);
  }

  reference_aterm(const T& other) noexcept
   : unprotected_aterm(detail::address(other))
  { }

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

  void mark(std::stack<std::reference_wrapper<detail::_aterm>>& todo) const
  {
    if (defined())
    {
      mark_term(*m_term,todo);
    }
  }
};

template<typename T>
typename std::pair<typename std::conditional<is_reference_aterm<typename T::first_type>::value,
                                                      typename T::first_type,
                                                      reference_aterm< typename T::first_type > >::type,
                                          typename std::conditional<is_reference_aterm<typename T::second_type>::value,
                                                      typename T::second_type,
                                                      reference_aterm< typename T::second_type > >::type >
reference_aterm_pair_constructor_helper(const T& other)
{
  if constexpr (is_reference_aterm<typename T::first_type>::value && is_reference_aterm<typename T::second_type>::value)
  {
    return other; 
  }
  else if constexpr (is_reference_aterm<typename T::first_type>::value && !is_reference_aterm<typename T::second_type>::value)
  {
    return std::pair(other.first, reference_aterm<typename T::second_type>(other.second));
  }
  else if constexpr (!is_reference_aterm<typename T::first_type>::value && is_reference_aterm<typename T::second_type>::value)
  {
    return std::pair(reference_aterm<typename T::first_type>(other.first),other.second);
  }
  else 
  { 
    static_assert(!is_reference_aterm<typename T::first_type>::value && 
                  !is_reference_aterm<typename T::second_type>::value,"Logic error");
  
    return std::pair(reference_aterm<typename T::first_type>(other.first), reference_aterm<typename T::second_type>(other.second));
  }
}




/// \brief An unprotected term that is stored inside an aterm_container.
template<typename T>
class reference_aterm<T, typename std::enable_if<is_pair<T>::value>::type > : 
                         public std::pair<typename std::conditional<is_reference_aterm<typename T::first_type>::value,
                                                      typename T::first_type,
                                                      reference_aterm< typename T::first_type > >::type, 
                                          typename std::conditional<is_reference_aterm<typename T::second_type>::value,
                                                      typename T::second_type,
                                                      reference_aterm< typename T::second_type > >::type >
{
protected:
  typedef std::pair<typename std::conditional<is_reference_aterm<typename T::first_type>::value,
                                          typename T::first_type,
                                          reference_aterm< typename T::first_type > >::type,
                    typename std::conditional<is_reference_aterm<typename T::second_type>::value,
                                          typename T::second_type,
                                          reference_aterm< typename T::second_type > >::type >  super;
  typedef T std_pair;

public:
  /// \brief Default constructor.
  reference_aterm() noexcept = default;

  reference_aterm(const reference_aterm& other) noexcept
  {
    *this = other;
  }


  reference_aterm(const std_pair& other) noexcept
    : super(reference_aterm_pair_constructor_helper(other))
  {}

  reference_aterm(std_pair&& other) noexcept
   : super(reference_aterm<typename T::first_type >(std::move(other.first)),
           reference_aterm<typename T::second_type>(std::move(other.second)))
  {} 

  reference_aterm& operator=(const reference_aterm& other) noexcept
  {
    super::first=other.first;
    super::second=other.second;
    return *this;
  }

  const reference_aterm& operator=(const std_pair& other) noexcept
  {
    super::first=other.first;
    super::second=other.second;
    return *this;
  }

  /// Converts implicitly to a protected term of type std::pair<T,U>..
  operator std_pair&()
  {
    return reinterpret_cast<std_pair>(*this);
  }

  operator const std_pair&() const
  {
    return *reinterpret_cast<std_pair const*>(this);
  }

  void mark(std::stack<std::reference_wrapper<detail::_aterm>>& todo) const
  {
    if constexpr (is_reference_aterm<typename T::first_type>::value)
    {
      super::first.mark(todo);
    }
    else 
    {
      reference_aterm<typename T::first_type>(super::first).mark(todo);
    }

    if constexpr (is_reference_aterm<typename T::second_type>::value)
    {
      super::second.mark(todo);
    }
    else 
    {
      reference_aterm<typename T::second_type>(super::second).mark(todo);
    }
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
    for (const typename Container::value_type& element: m_container) 
    {
      static_assert(is_reference_aterm<reference_aterm<typename Container::value_type> >::value,"TEST1");
      if constexpr (is_reference_aterm<typename Container::value_type>::value)
      {
        static_assert(is_reference_aterm<typename Container::value_type >::value,"TEST2");
        element.mark(todo);
      }
      else
      {
        static_assert(!is_reference_aterm<typename Container::value_type >::value,"TEST3");
        reference_aterm<typename Container::value_type>(element).mark(todo);
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
    return std::hash<T>()(t);
  }
};

} // namespace std

#endif // MCRL2_ATERMPP_DETAIL_ATERM_CONTAINER_H
