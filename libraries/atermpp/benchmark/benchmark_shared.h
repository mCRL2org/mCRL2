// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_ATERMPP_BENCHMARK_BENCHMARK_SHARED_H
#define MCRL2_ATERMPP_BENCHMARK_BENCHMARK_SHARED_H

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/utilities/stopwatch.h"

#include <cassert>
#include <cstddef>
#include <thread>
#include <utility>
#include <vector>

/// \brief Run the given function f on number_of_threads threads (including the main thread) and
///        report the elapsed wall-clock time.
/// \pre number_of_threads >= 1.
template<typename F>
void benchmark_threads(std::size_t number_of_threads, F f)
{
  assert(number_of_threads >= 1);

  stopwatch timer;

  // Initialize a number of worker threads. std::jthread joins on destruction, which keeps this
  // exception-safe: if f(0) throws below, the still-running threads are joined during unwinding
  // instead of triggering std::terminate.
  std::vector<std::jthread> threads(number_of_threads - 1);
  std::size_t id = 1;
  for (auto& thread : threads)
  {
    thread = std::jthread(f, id);
    ++id;
  }

  // Run the benchmark on the main thread as well.
  f(0);

  // Wait for all worker threads to complete before reading the timer.
  for (auto& thread : threads)
  {
    thread.join();
  }

  std::cerr << "time: " << timer.seconds() << std::endl;
}

/// \brief Create a nested function application f_depth. Where f_0 = c and f_i = f(f_i-1,...,f_i-1).
template<bool with_converter = false>
atermpp::aterm create_nested_function(const std::string& function_name,  const std::string& leaf_name, std::size_t number_of_arguments, std::size_t depth)
{
  // Create a suitable function application.
  atermpp::function_symbol f(function_name, number_of_arguments);
  atermpp::function_symbol c(leaf_name, 0);

  atermpp::aterm c_term(c);

  // Initialize a wide function application.
  std::vector<atermpp::aterm> arguments(f.arity());
  for (atermpp::aterm& argument: arguments)
  {
    argument = c_term;
  }
  atermpp::aterm f_term(f, arguments.begin(), arguments.end());

  for (std::size_t j = 0; j < depth; ++j)
  {
    // Create a very wide nested function application
    for (atermpp::aterm& argument: arguments)
    {
      argument = f_term;
    }

    if (with_converter)
    {      
      make_term_appl(f_term, f, arguments.begin(), arguments.end(), atermpp::detail::do_not_convert_term<atermpp::aterm>());
    }
    else
    {
      make_term_appl(f_term, f, arguments.begin(), arguments.end());
    }
  }

  return f_term;
}

/// \brief Create a nested function application f_depth. Where f_0 = c and f_i = f(f_i-1,...,f_i-1).
///        However, this function uses the fixed arity constructor of length N which should be faster.
template<std::size_t N>
atermpp::aterm create_nested_function(const std::string& function_name, const std::string& leaf_name, std::size_t depth)
{
  // Create a suitable function application.
  atermpp::function_symbol f(function_name, N);
  atermpp::function_symbol c(leaf_name, 0);

  atermpp::aterm c_term(c);

  // Construct f(c, ..., c) with exactly N arguments.
  atermpp::aterm f_term = [&]<std::size_t... I>(std::index_sequence<I...>)
    {
      return atermpp::aterm(f, (static_cast<void>(I), c_term)...);
    }(std::make_index_sequence<N>{});

  for (std::size_t j = 0; j < depth; ++j)
  {
    // Construct f(f_term, ..., f_term) with exactly N arguments.
    [&]<std::size_t... I>(std::index_sequence<I...>)
    {
      make_term_appl(f_term, f, (static_cast<void>(I), f_term)...);
    }(std::make_index_sequence<N>{});
  }

  return f_term;
}

#endif // MCRL2_ATERMPP_BENCHMARK_BENCHMARK_SHARED_H