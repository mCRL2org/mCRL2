// Author(s): Maurice Laveaux, Jeroen Keiren.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE parallel_thread_pool_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/detail/global_aterm_pool.h"
#include "mcrl2/utilities/configuration.h"

#include <atomic>
#include <thread>

using namespace atermpp;

// Verify that an aterm_core created on a worker thread remains protected after
// that thread exits.
//
// Root cause of issue #1934: the thread_local thread_aterm_pool destructor for
// non-main threads deletes m_variables without transferring the contained
// aterm_core* entries to another pool.  Any aterm_core that was registered on
// the worker (e.g. a static-local variable first initialised on that thread)
// therefore becomes invisible to the garbage collector.  The first subsequent
// collection reclaims the underlying _aterm node; later accesses or the
// static destructor at program exit then exhibit undefined behaviour or print
// "Element not found in protection set."
//
// The test encodes the failure mode directly:
//   1. A heap-allocated aterm is constructed on a worker thread so that its
//      aterm_core is registered in the worker's per-thread pool.
//   2. The worker is joined; without the fix the pool is torn down and the
//      entry is simply dropped.
//   3. A full garbage collection is forced.
//   4. The deletion hook must NOT have fired: the underlying _aterm node must
//      still be live because the aterm_core is reachable (either via the main
//      pool after the fix, or via its own still-valid pointer after the fix).
//
// Without the fix step 3 collects the node and step 4 fires the hook, failing
// the BOOST_CHECK.  With the fix the entries are moved to the main-thread pool
// before the worker pool is destroyed, so the node survives GC.
BOOST_AUTO_TEST_CASE(test_worker_thread_aterm_survives_gc)
{
  // This test only makes sense in a multi-threaded build where each thread
  // has its own protection set.
  if constexpr (!mcrl2::utilities::detail::GlobalThreadSafe)
  {
    return;
  }

  // Use a unique function symbol so the deletion hook cannot be confused with
  // terms created elsewhere in the test suite.
  const atermpp::function_symbol sym("__test_thread_orphan__", 0);

  // term_callback is a raw function pointer, so use a file-scope flag.
  static std::atomic<bool> term_was_collected{ false };
  term_was_collected = false;
  atermpp::add_deletion_hook(sym,
    [](const atermpp::aterm&) { term_was_collected = true; });

  // Heap-allocate so the aterm_core outlives the worker thread's stack.
  atermpp::aterm* orphan = nullptr;

  {
    std::thread worker([&]()
    {
      // Constructor runs on the worker thread: registers the aterm_core in the
      // worker's per-thread pool, NOT the main thread's pool.
      orphan = new atermpp::aterm(sym);
    });
    worker.join();
    // The worker's thread_local thread_aterm_pool destructor has now run.
    // Without the fix: *orphan is no longer in any root set.
    // With the fix:    *orphan was transferred to the main-thread pool.
  }

  BOOST_REQUIRE(orphan != nullptr);

  // Force a full GC cycle.  Without the fix the orphaned term is not reachable
  // from any root set, so the underlying _aterm node is swept and the deletion
  // hook fires, setting term_was_collected = true.
  atermpp::detail::g_thread_term_pool().collect();

  BOOST_CHECK_MESSAGE(!term_was_collected,
    "The aterm constructed on a worker thread was garbage-collected after the "
    "worker exited, indicating it was orphaned from all protection sets."
    "The code should transfer pool entries to the main-thread pool on worker "
    "teardown.");

  // Clean up only if the term survived GC; if it was collected the pointer is
  // dangling and must not be dereferenced.
  if (!term_was_collected)
  {
    delete orphan;
  }
}
