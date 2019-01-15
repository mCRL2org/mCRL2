// Author(s): Maurice Laveaux.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef DETAIL_FUNCTION_SYMBOL_POOL_H
#define DETAIL_FUNCTION_SYMBOL_POOL_H

#include "mcrl2/atermpp/function_symbol.h"
#include "mcrl2/atermpp/detail/function_symbol_hash.h"
#include "mcrl2/utilities/block_allocator.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/noncopyable.h"
#include "mcrl2/utilities/unordered_set.h"

#include <map>
#include <memory>
#include <string>

namespace atermpp
{
namespace detail
{

/// \brief Enable to print hashtable collision, size and number of buckets.
constexpr static bool EnableFunctionSymbolHashtableMetrics = false;

/// \brief Enable to obtain the percentage of function symbols found compared to created.
constexpr static bool EnableFunctionSymbolMetrics = false;

/// \brief This class stores a set of function symbols.
class function_symbol_pool : private mcrl2::utilities::noncopyable
{
public:
  function_symbol_pool();
  ~function_symbol_pool();

  /// \brief Creates a function symbol pair (name, arity), returns a pointer to an existing element
  ///        if this pair is already defined.
  /// \param check_for_registered_functions Check whether there is a registered prefix p such that
  ///           name equal pn where n is a number. In that case prevent that pn will be generated
  ///           as a fresh function name.
  function_symbol create(const std::string& name, const std::size_t arity, const bool check_for_registered_functions);

  /// \brief Frees the memory used by the passed element and remove it from the set.
  void destroy(_function_symbol* f);

  /// \brief Restore the index back to index before registering this prefix.
  void deregister(const std::string& prefix);

  /// \returns An index that is always a safe index for the given prefix.
  /// \todo These functions are all used by the function_symbol_generator and should probably not
  ///       be public.
  std::shared_ptr<std::size_t> register_prefix(const std::string& prefix);

  /// \brief Get an index such that no function symbol with name prefix + returned value
  ///        and any value above it already exists.
  std::size_t get_sufficiently_large_postfix_index(const std::string& prefix) const;

  /// \brief Print various performance statistics of this function symbol pool.
  void print_performance_stats() const noexcept;

  /// \returns The function symbol used by integral terms.
  static const function_symbol& as_int() noexcept { return g_as_int; }

  /// \returns The function symbol used by the list constructor.
  static const function_symbol& as_list() noexcept { return g_as_list; }

  /// \returns The function symbol used by the term indicating the empty list.
  static const function_symbol& as_empty_list() noexcept { return g_as_empty_list; }

  /// \returns The number of function symbols stored in this pool.
  std::size_t size() const noexcept { return m_symbol_set.size(); }

private:
  using unordered_set = mcrl2::utilities::unordered_set<
    _function_symbol,
    function_symbol_hasher,
    function_symbol_equals,
    mcrl2::utilities::block_allocator<_function_symbol, 1024, GlobalThreadSafe>,
    GlobalThreadSafe>;

  /// \brief Stores the underlying function symbols.
  unordered_set m_symbol_set;

  /// \brief A map that records a function for each prefix that must be called to set the
  ///        postfix number to a sufficiently high number if a function symbol with the same
  ///        prefix string is registered.
  std::map<std::string, std::shared_ptr<std::size_t>> m_prefix_to_register_function_map;

  /// Various performance metrics.
  std::uint64_t m_function_symbols_hits = 0; // The number of function symbols found in the pool.
  std::uint64_t m_function_symbols_creates = 0; // The number of function symbols created.
};

} // namespace detail
} // namespace atermpp

#endif // DETAIL_FUNCTION_SYMBOL_POOL_H

