// Author(s): Wieger Wesselink, Jan Friso Groote. Based on the aterm library by Paul Klint and others.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/detail/function_symbol.h
/// \brief A foundational class for function symbols. 

#ifndef DETAIL_FUNCTION_SYMBOL_H
#define DETAIL_FUNCTION_SYMBOL_H

#include "mcrl2/utilities/noncopyable.h"
#include "mcrl2/utilities/shared_reference.h"

#include <string>
#include <cstddef>

namespace atermpp
{
namespace detail
{

/// \brief Enables thread safety for the global term and function symbol pools.
/// \details The reason to declare it here is because the function symbol is included by everything.
constexpr static bool GlobalThreadSafe = false;

/// \brief Stores the data for a function symbol (name, arity) pair.
class _function_symbol : public mcrl2::utilities::shared_reference_counted<_function_symbol, GlobalThreadSafe>, private mcrl2::utilities::noncopyable
{
public:
  /// \brief A shared reference for this object.
  using ref = mcrl2::utilities::shared_reference<_function_symbol>;

  _function_symbol(const std::string& name, std::size_t arity) :
     m_arity(arity),
     m_name(name)
  {}

  const std::string& name() const noexcept
  {
    return m_name;
  }

  std::size_t arity() const noexcept
  {
    return m_arity;
  }

  bool operator==(const _function_symbol& f) const noexcept
  {
    return m_arity == f.m_arity && m_name == f.m_name;
  }

private:
  const std::size_t m_arity;
  const std::string m_name;
};

} // namespace detail
} // namespace atermpp

#endif // DETAIL_FUNCTION_SYMBOL_H

