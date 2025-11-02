// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/utilities/stopwatch.h"

#include <thread>

using namespace atermpp;

template<typename F>
void benchmark_threads(std::size_t number_of_threads, F f)
{
  stopwatch timer;

  // Initialize a number of threads.
  std::vector<std::thread> threads(number_of_threads - 1);
  int id = 1;
  for (auto& thread : threads)
  {
    thread = std::thread(f, id);
    ++id;
  }

  // Run the benchmark on the main thread as well.
  f(0);

  // Wait for all threads to complete.
  for (auto& thread : threads)
  {
    thread.join();
  }

  std::cerr << "time: " << timer.seconds() << std::endl;
}

/// \brief Create a nested function application f_depth. Where f_0 = c and f_i = f(f_i-1,...,f_i-1).
template<bool with_converter = false>
aterm create_nested_function(const std::string& function_name,  const std::string& leaf_name, std::size_t number_of_arguments, std::size_t depth)
{
  // Create a suitable function application.
  function_symbol f(function_name, number_of_arguments);
  function_symbol c(leaf_name, 0);

  aterm c_term(c);

  // Initialize a wide function application.
  std::vector<aterm> arguments(f.arity());
  for (aterm& argument: arguments)
  {
    argument = c_term;
  }
  aterm f_term(f, arguments.begin(), arguments.end());

  for (std::size_t j = 0; j < depth; ++j)
  {
    // Create a very wide nested function application
    for (aterm& argument: arguments)
    {
      argument = f_term;
    }

    if (with_converter)
    {      
      make_term_appl(f_term, f, arguments.begin(), arguments.end(), detail::do_not_convert_term<aterm>());
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
aterm create_nested_function(const std::string& function_name, const std::string& leaf_name, std::size_t depth)
{
  // Create a suitable function application.
  function_symbol f(function_name, N);
  function_symbol c(leaf_name, 0);

  aterm c_term(c);
  aterm f_term(f, c_term, c_term);

  for (std::size_t j = 0; j < depth; ++j)
  {
    make_term_appl(f_term, f, f_term, f_term);
  }

  return f_term;
}
