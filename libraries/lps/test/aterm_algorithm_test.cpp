// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file aterm_algorithm_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <iterator>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/core/garbage_collection.h"

using namespace std;
using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace mcrl2::lps::detail;

std::string SPECIFICATION =
"% This file contains the alternating bit protocol, as described in W.J.    \n"
"% Fokkink, J.F. Groote and M.A. Reniers, Modelling Reactive Systems.       \n"
"%                                                                          \n"
"% The only exception is that the domain D consists of two data elements to \n"
"% facilitate simulation.                                                   \n"
"                                                                           \n"
"sort                                                                       \n"
"  D     = struct d1 | d2;                                                  \n"
"  Error = struct e;                                                        \n"
"                                                                           \n"
"act                                                                        \n"
"  r1,s4: D;                                                                \n"
"  s2,r2,c2: D # Bool;                                                      \n"
"  s3,r3,c3: D # Bool;                                                      \n"
"  s3,r3,c3: Error;                                                         \n"
"  s5,r5,c5: Bool;                                                          \n"
"  s6,r6,c6: Bool;                                                          \n"
"  s6,r6,c6: Error;                                                         \n"
"  i;                                                                       \n"
"                                                                           \n"
"proc                                                                       \n"
"  S(b:Bool)     = sum d:D. r1(d).T(d,b);                                   \n"
"  T(d:D,b:Bool) = s2(d,b).(r6(b).S(!b)+(r6(!b)+r6(e)).T(d,b));             \n"
"                                                                           \n"
"  R(b:Bool)     = sum d:D. r3(d,b).s4(d).s5(b).R(!b)+                      \n"
"                  (sum d:D.r3(d,!b)+r3(e)).s5(!b).R(b);                    \n"
"                                                                           \n"
"  K             = sum d:D,b:Bool. r2(d,b).(i.s3(d,b)+i.s3(e)).K;           \n"
"                                                                           \n"
"  L             = sum b:Bool. r5(b).(i.s6(b)+i.s6(e)).L;                   \n"
"                                                                           \n"
"init                                                                       \n"
"  allow({r1,s4,c2,c3,c5,c6,i},                                             \n"
"    comm({r2|s2->c2, r3|s3->c3, r5|s5->c5, r6|s6->c6},                     \n"
"        S(true) || K || L || R(true)                                       \n"
"    )                                                                      \n"
"  );                                                                       \n";

struct compare_variable
{
  aterm d;

  compare_variable(variable d_)
    : d(d_)
  {}

  bool operator()(aterm t) const
  {
    return d == t;
  }
};

bool occurs_in(data_expression d, variable v)
{
  return find_if(aterm_appl(d), compare_variable(v)) != aterm_appl();
}

/// Search for a data variable in the term t. Precondition: t must contain
/// at least one variable.
template <typename Term>
variable find_variable(Term t)
{
  aterm_appl result = atermpp::find_if(t, data::is_variable);
  assert((result)); // check if a variable has been found
  return result;
}

void test_find_variable()
{
  variable d("d", basic_sort("D"));
  variable e("e", basic_sort("E"));
  data_expression d_e = sort_bool::and_(d, e);
  variable v = find_variable(d_e);
  BOOST_CHECK(v == d);
}

// insert elements of a container of type D into a container of type C
template < typename C, typename D >
void insert(C& c, D const& d) {
  c.insert(d.begin(), d.end());
}

int test_main(int argc, char** argv)
{
  struct local {
    static bool is_exists(atermpp::aterm_appl const& p) {
      return data::is_abstraction(p) && data::is_exists(data::abstraction(p));
    }
  };

  MCRL2_ATERMPP_INIT(argc, argv)

  test_find_variable();
  core::garbage_collect();

  specification spec = linearise(SPECIFICATION);
  linear_process lps = spec.process();

  // find all action labels in lps
  std::set<action_label> labels;
  find_all_if(linear_process_to_aterm(lps), is_action_label, inserter(labels, labels.end()));

  core::garbage_collect();

  // find all data variables in lps
  std::set<variable> variables;
  find_all_if(linear_process_to_aterm(lps), data::is_variable, inserter(variables, variables.end()));

  core::garbage_collect();

  // find all functions in spec
  std::set< mcrl2::data::function_symbol > functions;
  insert(functions, spec.data().constructors());
  insert(functions, spec.data().mappings());

  core::garbage_collect();

  // find all existential quantifications in lps
  std::set<data_expression> existential_quantifications;
  find_all_if(linear_process_to_aterm(lps), local::is_exists, inserter(existential_quantifications, existential_quantifications.end()));

  core::garbage_collect();

  return 0;
}
