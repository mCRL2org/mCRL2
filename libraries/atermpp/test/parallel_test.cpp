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
static const std::size_t size_of_terms=10;
static const std::size_t number_of_terms_to_construct=100000000;

std::size_t term_size(const aterm_appl& t)
{
  std::size_t size=1;
  for(const aterm& a: t)
  {
    size = size + term_size(atermpp::down_cast<aterm_appl>(a));
  }
  return size;
}

// Function to create and destroy arbitrary terms. 
void create_and_destroy_many_terms(const std::size_t n, const function_symbol& f, const function_symbol& c)
{
  aterm_appl t(c);
  for(std::size_t i=0; i<n; ++i)
  {
    t=aterm_appl(f,t,t);
  }
  // BOOST_CHECK(term_size(t)==2047);
} 

void aterm_test_thread()
{
  const function_symbol f("f",2);
  const function_symbol c("c",0);
  for(std::size_t i=0; i<number_of_terms_to_construct/number_of_threads; ++i)
  {
    create_and_destroy_many_terms(size_of_terms,f,c);
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

  std::cerr << "All " << number_of_threads << " threads are completed.\n";

  return 0;
}

