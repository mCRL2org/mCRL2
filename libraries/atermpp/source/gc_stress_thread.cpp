// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/atermpp/detail/gc_stress_thread.h"

#include "mcrl2/atermpp/detail/aterm_configuration.h"
#include "mcrl2/atermpp/detail/thread_aterm_pool.h"

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <thread>

namespace atermpp::detail
{

namespace
{

std::atomic<bool> g_gc_stress_running{false};
std::thread g_gc_stress_thread;

} // unnamed namespace

void start_gc_stress_thread()
{
  if constexpr (EnableGCStressThread && mcrl2::utilities::detail::GlobalThreadSafe)
  {
    g_gc_stress_running.store(true, std::memory_order_relaxed);
    g_gc_stress_thread = std::thread([]
    {
      // Registering the thread-local pool connects this thread to the global pool.
      while (g_gc_stress_running.load(std::memory_order_relaxed))
      {
        g_thread_term_pool().collect();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
    });
    std::atexit(stop_gc_stress_thread);
  }
}

void stop_gc_stress_thread()
{
  if (g_gc_stress_thread.joinable())
  {
    g_gc_stress_running.store(false, std::memory_order_relaxed);
    g_gc_stress_thread.join();
  }
}

} // namespace atermpp::detail
