// Author(s): Maurice Laveaux.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_ATERMPP_DETAIL_GC_STRESS_THREAD_H
#define MCRL2_ATERMPP_DETAIL_GC_STRESS_THREAD_H

namespace atermpp::detail
{

/// \brief Spawns a dedicated background thread that continuously triggers garbage collection.
/// \details Only has effect when EnableGCStressThread is set and MCRL2_ENABLE_MULTITHREADING is
///          defined. Useful for stress-testing the thread safety of the term pool under
///          concurrent GC pressure. The thread is stopped automatically at program exit.
void start_gc_stress_thread();

/// \brief Stop the background GC stress thread if it is running.
void stop_gc_stress_thread();

} // namespace atermpp::detail

#endif // MCRL2_ATERMPP_DETAIL_GC_STRESS_THREAD_H
