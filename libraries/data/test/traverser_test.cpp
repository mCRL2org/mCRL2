// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file traverser_test.cpp
/// \brief Traverser tests.

#include <iostream>
#include <boost/test/minimal.hpp>

#include "mcrl2/data/detail/binding_aware_traverser.h"
#include "mcrl2/data/detail/sort_traverser.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::data;

class identity_traverser: public mcrl2::data::detail::traverser<identity_traverser>
{
  public:
    typedef mcrl2::data::detail::traverser<identity_traverser> super;
      
    using super::operator();
    using super::enter;
    using super::leave;
};

class identity_sort_traverser: public mcrl2::data::detail::sort_traverser<identity_sort_traverser>
{
  public:
    typedef mcrl2::data::detail::traverser<identity_sort_traverser> super;
      
    using super::operator();
    using super::enter;
    using super::leave;
};

class my_traverser: public mcrl2::data::detail::traverser<my_traverser>
{
  protected:
    unsigned int m_sort_count;

  public:
    typedef mcrl2::data::detail::traverser<my_traverser> super;
      
    using super::operator();
    using super::enter;
    using super::leave;

    my_traverser() : m_sort_count(0)
    { }

    void enter(sort_expression const& s)
    {
      m_sort_count++;
    }
    
    unsigned int sort_count() const
    {
      return m_sort_count;
    }
};

class my_sort_traverser: public mcrl2::data::detail::sort_traverser<my_sort_traverser>
{
  protected:
    unsigned int m_sort_count;

  public:
    typedef mcrl2::data::detail::sort_traverser<my_sort_traverser> super;
      
    using super::operator();
    using super::enter;
    using super::leave;

    my_sort_traverser() : m_sort_count(0)
    { }

    void enter(sort_expression const& s)
    {
      m_sort_count++;
    }
    
    unsigned int sort_count() const
    {
      return m_sort_count;
    }
};

void test_traversers()
{
  data_expression x;
  std::string var_decl =
    "n: Pos;\n"
    ; 
  x = parse_data_expression("n < 10", var_decl); 
  
  identity_traverser t1;
  t1(x); 

  identity_sort_traverser t2;
  t2(x); 

  my_traverser t3;
  t3(x);
  BOOST_CHECK(t3.sort_count() == 0);

  my_sort_traverser t4;
  t4(x);
  BOOST_CHECK(t4.sort_count() > 0);
  core::garbage_collect();
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv);

  test_traversers();

  return EXIT_SUCCESS;
}
