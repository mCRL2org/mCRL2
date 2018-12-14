// Author(s): Maurice Laveaux.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_ATERMPP_DETAIL_GLOBAL_ATERM_POOL_H_
#define MCRL2_ATERMPP_DETAIL_GLOBAL_ATERM_POOL_H_

#include "mcrl2/atermpp/detail/aterm_pool.h"

namespace atermpp
{
namespace detail
{

/// \brief This a global variant of the aterm_pool that can be used to instantiate a global
///        instance.
/// \details The default aterm_pool properly cleans up all the memory in its destructor. However,
///          for the global terms this is not possible due to undefined destruction order. As such
///          the global pool could be destroyed before certain global terms or function symbols.
class global_aterm_pool : public aterm_pool
{
public:
  global_aterm_pool() {}
  ~global_aterm_pool()
  {
    print_performance_statistics();

    // Default initialize the pool without destroying the pool; this will leak memory.
    new (this) global_aterm_pool();
  }
};

/// \brief The pointer to the global aterm pool instance..
extern global_aterm_pool* g_aterm_pool_instance;

/// \brief Obtain a reference to the global aterm pool.
/// \param lazy Enable lazy initialization which should be used for instantiating
///        global terms and function symbols.
template<bool lazy = false>
inline global_aterm_pool& g_term_pool()
{
  if (lazy)
  {
    // Initialize when needed.
    if (g_aterm_pool_instance == nullptr)
    {
      g_aterm_pool_instance = new global_aterm_pool();
    }
  }

  assert(g_aterm_pool_instance != nullptr);
  return *g_aterm_pool_instance;
}

} // namespace detail
} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_GLOBAL_ATERM_POOL_H_
