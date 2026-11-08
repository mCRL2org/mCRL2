// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/symbolic/gc_statistics.h
/// \brief Counts the garbage collections of Sylvan and the time they take, for the verbose output of the tools.

#ifndef MCRL2_SYMBOLIC_GC_STATISTICS_H
#define MCRL2_SYMBOLIC_GC_STATISTICS_H

#ifdef MCRL2_ENABLE_SYLVAN

#include <sylvan.h>

#include <chrono>
#include <cstddef>
#include <iomanip>
#include <sstream>
#include <string>

namespace mcrl2::symbolic {

/// \brief The number of garbage collections and their total time (from the first to the last hook of each).
struct gc_statistics
{
  std::size_t count = 0;
  double seconds = 0.0;
  std::chrono::steady_clock::time_point start;
};

/// \brief The statistics of this process. Sylvan runs one garbage collection at a time, and calls its hooks on one
/// worker, so no lock is needed.
inline gc_statistics& global_gc_statistics()
{
  static gc_statistics statistics;
  return statistics;
}

namespace detail {

// The Lace task macros define functions that are not inline, so give them
// internal linkage to keep this header usable in more than one translation unit.
namespace {

VOID_TASK_0(symbolic_gc_start)
{
  global_gc_statistics().start = std::chrono::steady_clock::now();
}

VOID_TASK_0(symbolic_gc_end)
{
  gc_statistics& statistics = global_gc_statistics();
  statistics.count++;
  statistics.seconds += std::chrono::duration<double>(std::chrono::steady_clock::now() - statistics.start).count();
}

} // namespace

} // namespace detail

/// \brief Counts the garbage collections from now on; call after sylvan_init_package.
inline void install_gc_statistics()
{
  sylvan::sylvan_gc_hook_pregc(detail::symbolic_gc_start_CALL);
  sylvan::sylvan_gc_hook_postgc(detail::symbolic_gc_end_CALL);
}

/// \brief Prints Sylvan's own statistics to stdout; this only prints something when Sylvan is built with SYLVAN_STATS.
inline void print_sylvan_statistics()
{
  sylvan::sylvan_stats_report(stdout);
}

/// \brief For example "garbage collections: 12 (0.53s)".
inline std::string print_gc_statistics()
{
  const gc_statistics& statistics = global_gc_statistics();
  std::ostringstream out;
  out << "garbage collections: " << statistics.count << " (" << std::setprecision(2) << std::fixed << statistics.seconds << "s)";
  return out.str();
}

} // namespace mcrl2::symbolic

#endif // MCRL2_ENABLE_SYLVAN

#endif // MCRL2_SYMBOLIC_GC_STATISTICS_H
