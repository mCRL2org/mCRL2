// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsmin_test.cpp
/// \brief Test for the pins class that connects mCRL2 with LTSMin (http://fmt.cs.utwente.nl/tools/ltsmin/).

#include <iostream>

#include <boost/test/minimal.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/ltsmin.h"
#include "mcrl2/lps/detail/test_input.h"

using namespace mcrl2;

typedef int* state_vector;

inline
std::string print_state(const state_vector& s, std::size_t size)
{
  int* first = s;
  int* last = s + size;

  std::ostringstream out;
  out << "[";
  for (int* i = first; i != last; ++i)
  {
    if (i != first)
    {
      out << ", ";
    }
    out << *i;
  }
  out << "]";
  return out.str();
}

struct state_callback_function
{
  std::size_t state_size;
  std::size_t state_count;
  
  state_callback_function(std::size_t size)
    : state_size(size),
      state_count(0)
  {}
  
  void operator()(int edge_label, const state_vector& next_state, int group)
  {
    state_count++;
    std::cout << "visit state " << print_state(next_state, state_size) << " edge label = " << edge_label << " group = " << group << std::endl;
  }
};

void test_ltsmin()
{
  // create an input file
  lps::specification spec = lps::linearise(lps::detail::ABP_SPECIFICATION());
  std::string abp_filename = "temporary_abp.lps";
  spec.save(abp_filename);

  lps::pins p(abp_filename, "jitty");

  BOOST_CHECK(p.edge_label_count() == 1);
  BOOST_CHECK(p.edge_label_name(0) == "action");
  BOOST_CHECK(p.edge_label_type(0) == 1);

  BOOST_CHECK(p.process_parameter_count() == 11);
  for (std::size_t i = 0; i < p.process_parameter_count(); i++)
  {
    BOOST_CHECK(p.process_parameter_type(i) == 0);
  }

  BOOST_CHECK(p.datatype_name(0) == "state");
  BOOST_CHECK(p.datatype_name(1) == "action_label");
  BOOST_CHECK(p.datatype_count() == 2);

  BOOST_CHECK(p.group_count() == 10);
  
  std::cout << p.info() << std::endl;

  // get the initial state
  state_vector initial_state = new int[p.process_parameter_count()];
  p.get_initial_state(initial_state);

  // find all successors of the initial state
  state_callback_function f_all(p.process_parameter_count()); 
  p.next_state_all(initial_state, f_all);
  std::cout << "f_all.state_count = " << f_all.state_count << std::endl;

  // find successors of the initial state for individual groups
  std::size_t count = 0;
  for (std::size_t group = 0; group < p.group_count(); group++)
  {
    state_callback_function f_long(p.process_parameter_count());
    p.next_state_long(initial_state, group, f_long);
    std::cout << "group " << group << " count = " << f_long.state_count << std::endl;
    count += f_long.state_count;
  }
  BOOST_CHECK(count == f_all.state_count);

  // check serialize/deserialize
  for (std::size_t d = 0; d < p.datatype_count(); ++d)
  {
    for (std::size_t i = 0; i < p.datatype_size(d); ++i)
    {
      std::string s = p.serialize(d, i);
      std::cout << "serialize(" << d << ", " << i << ") = " << p.serialize(d, i) << std::endl;
      std::size_t j = p.deserialize(d, s);
      BOOST_CHECK(i == j);
    }
  }

  // check print/parse
  for (std::size_t d = 0; d < p.datatype_count(); ++d)
  {
    for (std::size_t i = 0; i < p.datatype_size(d); ++i)
    {
      std::string s = p.print(d, i);
      std::cout << "print(" << d << ", " << i << ") = " << s << std::endl;
      //std::size_t j = p.parse(d, s);
      //std::cout << "parse(" << d << ", " << s << ") = " << j << std::endl;
    }
  }
  BOOST_CHECK(p.print(0, 0) == "1");
  BOOST_CHECK(p.print(0, 1) == "d1");

  // cleanup temporary files
  boost::filesystem::remove(boost::filesystem::path(abp_filename));
    
  delete[] initial_state;
  
  //BOOST_CHECK(false);
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)
  
  test_ltsmin();

  return 0;
}
