// Author(s): Maurice Laveaux.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_ATERMPP_DETAIL_FUNCTION_SYMBOL_HASH_H_
#define MCRL2_ATERMPP_DETAIL_FUNCTION_SYMBOL_HASH_H_

#include "mcrl2/atermpp/function_symbol.h"

#include <cstdint>

namespace std
{
 
/// \brief Specialisation of the standard hash function for function_symbol.
template<>
struct hash<atermpp::function_symbol>
{
  std::size_t operator()(const atermpp::function_symbol& f) const
  {
    // Function symbols take 48 bytes in memory, so when they are packed there
    // are at least 32 bits that do not distinguish two function symbols. As
    // such these can be removed.
    return reinterpret_cast<std::uint64_t>(f.m_function_symbol.get()) >> 5;
  }
};

/// \brief Specialisation of the standard hash function for _function_symbol.
template<>
struct hash<atermpp::detail::_function_symbol>
{
  std::size_t operator()(const atermpp::detail::_function_symbol& f) const
  {
    std::hash<std::string> string_hasher;
    std::size_t h = string_hasher(f.name());
    return (h ^ f.arity());
  }
};

} // namespace std

namespace atermpp::detail
{

/// \brief Computes the hash for given function symbol objects and for the
///        function_symbol_key.
struct function_symbol_hasher
{
  using is_transparent = void;

  inline std::size_t operator() (const _function_symbol& symbol) const noexcept;
  inline std::size_t operator() (const std::string& name, std::size_t arity) const noexcept;
};

/// \brief True iff the given function symbols are equal to eachother or to
///        the given key.
struct function_symbol_equals
{
  using is_transparent = void;

  inline bool operator() (const _function_symbol& first, const _function_symbol& second) const noexcept;
  inline bool operator()(const _function_symbol& symbol, const std::string& name, std::size_t arity) const noexcept;
};

std::size_t function_symbol_hasher::operator() (const _function_symbol& symbol) const noexcept
{
  const std::hash<_function_symbol> function_symbol_hasher;
  return function_symbol_hasher(symbol);
}

std::size_t function_symbol_hasher::operator() (const std::string& name, std::size_t arity) const noexcept
{
  std::hash<std::string> string_hasher;
  std::size_t h = string_hasher(name);
  return (h ^ arity);
}

bool function_symbol_equals::operator() (const _function_symbol& first, const _function_symbol& second) const noexcept
{
  return first == second;
}

bool function_symbol_equals::operator()(const _function_symbol& symbol, const std::string& name, std::size_t arity) const noexcept
{
  return (symbol.name() == name) && (symbol.arity() == arity);
}

} // namespace atermpp::detail


#endif // MCRL2_ATERMPP_DETAIL_FUNCTION_SYMBOL_HASH_H_
