// Author(s): Unknown
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parelm_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/confluence_checker.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::core;
using namespace mcrl2::lps;
using namespace mcrl2::lps::detail;

// Parameter i should be removed
const std::string case_1(
  "act a, b;\n"
  "init tau.a + tau.b;\n");

const std::string case_2(
  "act a, b;\n"
  "init a.tau.b.delta ;\n");

const std::string case_3(
  "act a, b;\n"
  "init a.b.delta ;\n");


static bool check_for_ctau(lps::specification const& s)  // s1 is an lps.
{
  const action_summand_vector v_summands = s.process().action_summands();

  for(action_summand_vector::const_iterator i=v_summands.begin(); i!=v_summands.end(); ++i) 
  { 
    const action_list al=i->multi_action().actions();
    if (al.size()==1)
    { 
      const action_label lab=al.front().label();
      if (lab.name()=="ctau")
      { 
        return true;
      }
    }
  }
  return false;
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  using namespace mcrl2::lps;

  specification s0;
  specification s1;

  // case 1
  std::cerr << "Confluence check case 1\n";
  s0 = linearise(case_1);
  Confluence_Checker checker1(s0);
  s1=lps::specification(checker1.check_confluence_and_mark(data::sort_bool::true_(),0));  
                                                             // Check confluence for all summands and
                                                             // replace confluents tau's by ctau's.
  BOOST_CHECK(!check_for_ctau(s1));
  core::garbage_collect();

  // case 2
  std::cerr << "Confluence check case 2\n";
  s0 = linearise(case_2);
  Confluence_Checker checker2(s0);
  s1=lps::specification(checker2.check_confluence_and_mark(data::sort_bool::true_(),0));  
                                                             // Check confluence for all summands and
                                                             // replace confluents tau's by ctau's.
  BOOST_CHECK(check_for_ctau(s1));
  core::garbage_collect();

  // case 3
  std::cerr << "Confluence check case 3\n";
  s0 = linearise(case_3);
  Confluence_Checker checker3(s0);
  s1=lps::specification(checker3.check_confluence_and_mark(data::sort_bool::true_(),0));  
                                                             // Check confluence for all summands and
                                                             // replace confluents tau's by ctau's.
  BOOST_CHECK(!check_for_ctau(s1));
  core::garbage_collect();

  return 0;
}
