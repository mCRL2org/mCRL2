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

// N.B. Using sort_traverser instead of traverser fails!
class my_traverser: public mcrl2::data::detail::traverser<my_traverser>
{
  protected:
    unsigned int m_sort_count;

  public:
    typedef mcrl2::data::detail::traverser<my_traverser> super;
      
    // N.B. The following using statements are required, unless they have
    // been redefined for all possible objects.
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

void traverser_test()
{
  data_expression x;
  std::string var_decl =
    "n: Pos;\n"
    ; 
  x = parse_data_expression("n < 10", var_decl); 
  
  // Apparently the traverser class skips all sort expressions:
  my_traverser t;
  t(x);
  BOOST_CHECK(t.sort_count() == 0);
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv);

  traverser_test();
  core::garbage_collect();

  return EXIT_SUCCESS;
}
