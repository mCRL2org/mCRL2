// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_ATERMPP_DETAIL_ATERM_APPL_H
#define MCRL2_ATERMPP_DETAIL_ATERM_APPL_H

#include "mcrl2/atermpp/aterm.h"

#include <array>
#include <tuple>

namespace atermpp
{

template <typename Term>
class term_appl_iterator;

namespace detail
{

/// \brief This class stores a term followed by N arguments. Where N should be equal to
///        the arity of the function symbol. These arguments do have room reserved for them
///        during the creation of the _aterm_appl.
template <class Term, std::size_t N = 1>
class _aterm_appl : public _aterm
{
public:

  /// \brief Constructs a term application with the given symbol and arguments.
  template<typename ...Terms,
           typename std::enable_if<are_terms<Terms...>::value>::type* = nullptr,
           typename std::enable_if<sizeof...(Terms) == N>::type* = nullptr>
  _aterm_appl(const function_symbol& sym, const Terms& ...arguments)
    : _aterm(sym),
      m_arguments{arguments...}
  {
    assert(N == sym.arity()); // The arity of the function symbol matches.
  }

  /// \brief constructs a term application with the given symbol and an iterator where the number
  ///        of elements is equal to the template parameter N.
  template<typename Iterator,
           typename std::enable_if<is_iterator<Iterator>::value>::type* = nullptr>
  _aterm_appl(const function_symbol& sym, Iterator it)
    : _aterm(sym)
  {
    for (std::size_t i = 0; i < N; ++i)
    {
      // Do not call the copy constructor.
      m_arguments[i] = *it;
      ++it;
    }
  }

  /// \brief Constructs a term application with the given symbol and arguments.
  _aterm_appl(const function_symbol& sym, std::array<unprotected_aterm, N> arguments)
    : _aterm(sym),
      m_arguments(arguments)
  {}

  /// \brief constructs a term application with the given symbol and its arguments from the iterator.
  template<typename Iterator,
           typename std::enable_if<is_iterator<Iterator>::value>::type* = nullptr>
  _aterm_appl(const function_symbol& symbol, Iterator it, bool)
    : _aterm(symbol)
  {
    for (std::size_t i = 0; i < symbol.arity(); ++i)
    {
      // Do not call the copy constructor.
      m_arguments[i] = *it;
      ++it;
    }
  }

  /// \brief Constructs a term application with the given symbol and arguments.
  _aterm_appl(const function_symbol& symbol, unprotected_aterm arguments[], bool)
    : _aterm(symbol)
  {
    for (std::size_t i = 0; i < symbol.arity(); ++i)
    {
      // Do not call the copy constructor.
      m_arguments[i] = arguments[i];
    }
  }

  /// \returns A reference to the arguments at the ith position.
  Term& arg(std::size_t index)
  {
    return reinterpret_cast<Term&>(m_arguments.data()[index]);
  }

  /// \returns A const reference to the arguments at the ith position.
  const Term& arg(std::size_t index) const
  {
    return reinterpret_cast<const Term&>(m_arguments.data()[index]);
  }

private:
  std::array<unprotected_aterm, N> m_arguments; /// \brief Array of arguments.
};

/// \brief This class allocates _aterm_appl objects where the size is based on the arity of
///        the function symbol.
/// \details The template T is required to be an object that implicitly converts to an _aterm_appl.
template<typename T = _aterm_appl<aterm>>
class _aterm_appl_allocator
{
public:

  template <class U>
  struct rebind
  {
      typedef _aterm_appl_allocator<U> other;
  };

  constexpr static std::uint64_t term_appl_size(std::uint64_t arity)
  {
    return sizeof(T) + (arity - 1) * sizeof(aterm);
  }

  template<typename ForwardIterator>
  T* allocate_and_construct(const function_symbol& symbol, ForwardIterator begin)
  {
    // We assume that object T contains the _aterm_appl<aterm, 1> at the end and reserve extra space for parameters.
    char* newTerm = m_packed_allocator.allocate(term_appl_size(symbol.arity()));
    new (newTerm) T(symbol, begin, true);
    return reinterpret_cast<T*>(newTerm);
  }

  T* allocate_and_construct(const function_symbol& symbol, unprotected_aterm* arguments)
  {
    // We assume that object T contains the _aterm_appl<aterm, 1> at the end and reserve extra space for parameters.
    char* newTerm = m_packed_allocator.allocate(term_appl_size(symbol.arity()));
    new (newTerm) T(symbol, arguments, true);
    return reinterpret_cast<T*>(newTerm);
  }

  void destroy(T* element)
  {
    assert(element != nullptr);

    // Only destroy the function symbol.
    _aterm_appl<aterm>& term = *element;
    term.function().~function_symbol();
  }

  void deallocate(T* element, std::size_t)
  {
    assert(element != nullptr);

    // Deallocate the memory of this aterm appl.
    _aterm_appl<aterm>& term = *element;
    m_packed_allocator.deallocate(reinterpret_cast<char*>(element), term_appl_size(term.function().arity()));
  }

  constexpr bool has_free_slots() const noexcept
  {
    return false;
  }

  constexpr std::size_t capacity() const
  {
    return 0;
  }

private:
  std::allocator<char> m_packed_allocator;
};

static_assert(sizeof(_aterm_appl<aterm>) == sizeof(_aterm) + sizeof(aterm), "Sanity check: aterm_appl size");

template < class Derived, class Base >
term_appl_iterator<Derived> aterm_appl_iterator_cast(term_appl_iterator<Base> a,
                                                                typename std::enable_if<
                                                                     std::is_base_of<aterm, Base>::value &&
                                                                     std::is_base_of<aterm, Derived>::value
>::type* = nullptr);

} // namespace detail
} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ATERM_APPL_H
