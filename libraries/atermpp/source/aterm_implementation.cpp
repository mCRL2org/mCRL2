// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/detail/global_aterm_pool.h"
#include "mcrl2/utilities/shared_mutex.h"

using namespace atermpp;
using namespace atermpp::detail;

/// \brief Check for reasonably sized aterm (32 bits, 4 bytes)
///        This check might break on perfectly valid architectures
///        that have char == 2 bytes, and sizeof(header_type) == 2
static_assert(sizeof(std::size_t) == sizeof(_aterm*), "The size of an aterm pointer is not equal to the size of type std::size_t. Cannot compile the MCRL2 toolset for this platform.");
static_assert(sizeof(std::size_t) >= 4,"The size of std::size_t should at least be four bytes. Cannot compile the toolset for this platform.");

void atermpp::add_deletion_hook(const function_symbol& function, term_callback callback)
{  
  g_term_pool().add_deletion_hook(function, callback);
}

namespace atermpp::detail
{
/// \brief A reference to the thread local term pool storage
thread_aterm_pool& g_thread_term_pool()
{
#ifdef MCRL2_ENABLE_MULTITHREADING
  static_assert(mcrl2::utilities::detail::GlobalThreadSafe);
  thread_local thread_aterm_pool instance(g_aterm_pool_instance);
#else 
  static_assert(!mcrl2::utilities::detail::GlobalThreadSafe);
  static thread_aterm_pool instance(g_aterm_pool_instance);
#endif
  return instance;
}

} // end namespace atermpp::detail

aterm_stream::~aterm_stream() = default;

aterm_istream::~aterm_istream() = default;

aterm_ostream::~aterm_ostream() = default;

/// Definition of the extern global term pool.
alignas(aterm_pool)
std::byte atermpp::detail::g_aterm_pool_storage[sizeof(aterm_pool)] = {};     
