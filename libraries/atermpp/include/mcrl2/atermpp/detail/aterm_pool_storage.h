// Author(s): Maurice Laveaux.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ATERMPP_DETAIL_ATERM_POOL_STORAGE_H
#define ATERMPP_DETAIL_ATERM_POOL_STORAGE_H

#include "mcrl2/atermpp/detail/aterm.h"
#include "mcrl2/atermpp/detail/aterm_appl.h"
#include "mcrl2/atermpp/detail/aterm_int.h"
#include "mcrl2/atermpp/detail/aterm_hash.h"
#include "mcrl2/utilities/block_allocator.h"
#include "mcrl2/utilities/unordered_set.h"

#include <limits>
#include <stack>
#include <utility>
#include <vector>

namespace atermpp
{
namespace detail
{

/// The callback that can be used for term creations and deletions.
typedef void(*term_callback)(const aterm&);
typedef std::pair<function_symbol, term_callback> callback_pair;

// Forward declaration
class aterm_pool;

/// \brief This class provides for all types of term storage. It also
///       provides garbage collection via its mark and sweep functions.
/// \details Internally a hash set is used to ensure that the created terms are unique.
template<typename Element,
         typename Hash = aterm_hasher<>,
         typename Equals = aterm_equals<>,
         std::size_t N = DynamicNumberOfArguments,
         bool ThreadSafe = false>
class aterm_pool_storage : private mcrl2::utilities::noncopyable
{
public:
  using unordered_set = mcrl2::utilities::unordered_set<
    Element,
    Hash,
    Equals,
    typename std::conditional<N == DynamicNumberOfArguments,
      atermpp::detail::_aterm_appl_allocator<>,
      mcrl2::utilities::block_allocator<Element, 1024, ThreadSafe>>::type,
    ThreadSafe>;
  using iterator = typename unordered_set::iterator;
  using const_iterator = typename unordered_set::const_iterator;

  /// \brief The local pool is a friend class so it can mark terms.
  friend class aterm_pool;

  aterm_pool_storage(aterm_pool& pool);

  /// \brief Add a callback that is triggered whenever a term with the given function symbol is created.
  void add_creation_hook(function_symbol sym, term_callback callback);

  /// \brief Add a callback that is triggered whenever a term with the given function symbol is destroyed.
  void add_deletion_hook(function_symbol sym, term_callback callback);

  /// \returns The total number of terms that can be stored without resizing.
  std::size_t capacity() const noexcept { return m_term_set.capacity(); }

  /// \brief Creates a integral term with the given value.
  aterm create_int(std::size_t value);

  /// \brief Creates a term with the given function symbol.
  aterm create_term(const atermpp::function_symbol& sym);

  /// \brief Creates a function application with the given function symbol and arguments.
  template<class ...Terms>
  aterm create_appl(const function_symbol& sym, const Terms&... arguments);

  /// \brief Creates a function application with the given function symbol and the arguments
  ///       as provided by the given iterator. This function assumes that the arity of the
  ///       function symbol is equal to N and that the iterator has exactly N elements.
  template<typename ForwardIterator>
  aterm create_appl_iterator(const function_symbol& sym,
                              ForwardIterator begin,
                              ForwardIterator end);

  /// \brief Creates a function application with the given function symbol and the arguments
  ///       as provided by the given iterator. This function assumes that the arity of the
  ///       function symbol is equal to N and that the iterator has exactly N elements.
  template<typename InputIterator,
           typename TermConverter>
  aterm create_appl_iterator(const function_symbol& sym,
                              TermConverter converter,
                              InputIterator begin,
                              InputIterator end);

  /// \brief Creates a function application with the given function symbol and the arguments
  ///        as provided by the given iterator.
  template<typename ForwardIterator>
  aterm create_appl_dynamic(const function_symbol& sym,
                              ForwardIterator begin,
                              ForwardIterator end);

  /// \brief Creates a function application with the given function symbol and the arguments
  ///        as provided by the given iterator, but the converter is applied to each argument.
  template<typename InputIterator,
           typename TermConverter>
  aterm create_appl_dynamic(const function_symbol& sym,
                              TermConverter converter,
                              InputIterator begin,
                              InputIterator end);

  /// \brief Prints various performance statistics for this storage.
  /// \param identifier A string to identify the printed message for this storage.
  void print_performance_stats(const char* identifier) const;

  /// \brief Marks all terms that are reachable and should not be destroyed.
  void mark();

  /// \brief sweep Destroys all terms that are not reachable. Requires that
  ///        mark() was called first.
  void sweep();

  /// \returns The number of terms stored in this storage.
  std::size_t size() const { return m_term_set.size(); }

  /// \brief A fake copy constructor to fix the issues with GCC 4 and 5.
  aterm_pool_storage(const aterm_pool_storage& other) :
    m_pool(other.m_pool),
    m_term_set(std::move(other.m_term_set))
  {}

  /// \brief Check that all arguments of a term application are marked properly.
  bool verify_mark();

  /// \brief Check that all arguments of a term application are marked properly.
  bool verify_sweep();

private:
  /// \brief Calls the creation hook attached to the function symbol of this term.
  void call_creation_hook(aterm term);

  /// \brief Calls the deletion hook attached to the function symbol of this term.
  void call_deletion_hook(_aterm* term);

  /// \brief Removes an element from the unordered set and deallocates it.
  iterator destroy(iterator it);

  /// \brief Inserts a term constructed by the given arguments, checks for existing term.
  template<typename ...Args>
  aterm insert(Args&&... args);

  /// \brief Insert without checking for an existing term.
  template<typename ...Args>
  aterm emplace(Args&&... args);

  /// \returns True if and only if this term storage can store term applications with a dynamic
  ///          number of arguments.
  constexpr bool is_dynamic_storage() const;

  /// \brief Marks a term and recursively all arguments that are not reachable.
  void mark_term(_aterm& root);

  /// \brief Verify that the given term was constructed properly.
  template<std::size_t Arity = N>
  bool verify_term(_aterm& term);

  /// The pool that this storage belongs to.
  aterm_pool& m_pool;

  /// This is the set of term pointers to keep the terms unique.
  unordered_set m_term_set;

  /// This array stores creation, resp deletion, hooks for function symbols.
  std::vector<callback_pair> m_creation_hooks;
  std::vector<callback_pair> m_deletion_hooks;

  /// Various performance statistics.
  std::size_t m_term_hits = 0; // The number of terms that a term was found in the pool.
  std::size_t m_term_creates = 0; // The number of calls to create a term.
  std::size_t m_erasedBlocks = 0; // The number of blocks that have been erased in the block allocator.

  /// A reusable todo stack.
  std::stack<_aterm*> todo;
};

} // namespace detail
} // namespace atermpp


#endif // ATERMPP_DETAIL_ATERM_POOL_STORAGE_H
