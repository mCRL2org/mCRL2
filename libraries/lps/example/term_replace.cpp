// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file term_replace.cpp
/// \brief Add your file description here.

#include <cassert>
#include "mcrl2/atermpp/algorithm.h"     // replace
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/atermpp/substitute.h"
#include "mcrl2/new_data/data.h"

using namespace atermpp;
using namespace mcrl2::data;
using namespace mcrl2::new_data::data_expr;

// replace d with d0
aterm_appl replace_d_d0(aterm_appl t)
{
  variable d("d:D");
  variable d0("d0:D");
  return atermpp::replace(t, d, d0);
}

// function object for replacing src with dest
struct replace_variables
{
  const variable& src;
  const variable& dest;

  replace_variables(const variable& src_, const variable& dest_)
    : src(src_), dest(dest_)
  {}

  aterm_appl operator()(aterm_appl t)
  {
    return atermpp::replace(t, src, dest);
  }
};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  variable d("d:D");
  variable d0("d0:D");
  variable e("e:E");
  variable e0("e0:E");

  data_expression d_e   = and_(d, e);
  data_expression d0_e  = and_(d0, e);
  data_expression d0_e0 = and_(d0, e0);

  data_expression t;

  // replace using a function
  t = d_e.substitute(replace_d_d0);
  assert(t == d0_e);

  // replace using a function object
  t = d_e.substitute(replace_variables(d, d0));
  assert(t == d0_e);

  // replace using the predefined function object
  t = d_e.substitute(make_substitution(d, d0));
  assert(t == d0_e);

  // replace using the predefined function object for lists
  variable_list src  = make_list(d, e);
  variable_list dest = make_list(d0, e0);
  t = d_e.substitute(make_list_substitution(src, dest));
  assert(t == d0_e0);

  // use atermpp::replace directly
  t = atermpp::replace(d_e, d, d0);
  assert(t == d0_e);

  // replace using a data assignment
  assignment a(d, d0);
  t = d_e.substitute(a);
  assert(t == d0_e);

  // replace using a data assignment list
  assignment b(e, e0);
  assignment_list l = make_list(a, b);
  t = d_e.substitute(assignment_list_substitution(l));
  assert(t == d0_e0);

  return 0;
}
