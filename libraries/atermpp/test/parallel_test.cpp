// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parallel_test.cpp
/// \brief This test tests the creation and destruction of aterms 
///        in parallel. 

#include <iostream>
#include <thread>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_string.h"

using namespace atermpp;
static const std::size_t number_of_threads=1;  // In the sequential toolset this value should be 1
                                               // as otherwise totally unpredictable behaviour will occur.
                                               // For the parallel atermpp library this ought to work
                                               // properly with larger and large values for number_of_threads. 

// Function to create and destroy arbitrary terms. 
void create_and_destroy_many_terms(const std::size_t n)
{
  const function_symbol f("f",2);
  const function_symbol c("c",0);
  aterm_appl t(c);
  for(std::size_t i=0; i<n; ++i)
  {
    t=aterm_appl(f,t,t);
  }
} 

void aterm_test_thread()
{
  for(std::size_t i=0; i<10; ++i)
  {
    create_and_destroy_many_terms(1000/number_of_threads);
  }
}

int test_main(int , char*[])
{
  std::vector<std::thread> threads;

  std::cerr << "Starting " << number_of_threads << " threads.\n";

  for(std::size_t i=0; i<number_of_threads; ++i)
  {
    threads.emplace_back(aterm_test_thread);
  }
  
  std::cerr << "Threads are running.\n";

  for(std::size_t i=0; i<number_of_threads; ++i)
  {
    threads.at(i).join(); 
  }

  aterm_test_thread();
  
  std::cerr << "All " << number_of_threads << " threads are completed.\n";

  return 0;
}

