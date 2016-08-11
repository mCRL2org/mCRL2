// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsparunfold_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>
#include <boost/algorithm/string.hpp>
#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/lpsparunfoldlib.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/lps/parse.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;


int test_main(int, char**)
{
  {
    /**
      * Unfold Pos process parameter at index 0
      *
      **/

    std::string case_1 =
      "% Test Case 1 -- Unfold Pos                                                      \n"
      "%                                                                                \n"
      "% The first process parameter i of type Pos is is unfolded                       \n"
      "                                                                                 \n"
      "act action :Pos;                                                                 \n"
      "                                                                                 \n"
      "proc P(i: Pos) = action(i). P(i);                                                \n"
      "                                                                                 \n"
      "init P(1);                                                                       \n"
      ;


    stochastic_specification s0;
    parse_lps(case_1,s0);
    variable_list p0 = s0.process().process_parameters();

    /* Requirements */
    if (p0.size() != 1)
    {
      std::clog << "--- failed test ---" << std::endl;
      std::clog << case_1 << std::endl;
      std::clog << "expected 1 process parameter" << std::endl;
      std::clog << "encountered " << p0.size() << "process parameters" << std::endl;
    }
    BOOST_CHECK(p0.size() == 1);

    std::string t0 = data::pp(p0.front().sort());
    if (t0.compare("Pos") != 0)
    {
      std::clog << "--- failed test ---" << std::endl;
      std::clog << case_1 << std::endl;
      std::clog << "expected process parameter to be of type Pos" << std::endl;
      std::clog << "encountered process parameter of type " << p0.front().sort() << std::endl;
    }
    BOOST_CHECK(t0.compare("Pos") == 0);

    /* Return */

    std::map< mcrl2::data::sort_expression , lspparunfold::unfold_cache_element > unfold_cache;
    lpsparunfold lpsparunfold(s0, &unfold_cache);
    stochastic_specification s1 = lpsparunfold.algorithm(0);
    variable_list p1 = s1.process().process_parameters();
    if (p1.size() != 3)
    {
      std::clog << "--- failed test ---" << std::endl;
      std::clog << case_1 << std::endl;
      std::clog << "expected result to have 3 process parameters" << std::endl;
      std::clog << "computed " << p1.size() << " process parameters" << std::endl;
    }

    for (variable_list::iterator i = p1.begin(); i != p1.end(); ++i)
    {
      if (std::distance(p1.begin(), i) == 1 && data::pp(i->sort()).compare("Bool") != 0)
      {
        std::clog << "--- failed test ---" << std::endl;
        std::clog << lps::pp(s1) << std::endl;
        std::clog << "expected 2nd process parameter to be of type Bool" << std::endl;
        std::clog << "computed process parameter of type "  << data::pp(i->sort()) << std::endl;
      }
      BOOST_CHECK(!(std::distance(p1.begin(), i) == 1 && data::pp(i->sort()).compare("Bool") != 0));

      if (std::distance(p1.begin(), i) == 2 && data::pp(i->sort()).compare("Pos") != 0)
      {
        std::clog << "--- failed test ---" << std::endl;
        std::clog << lps::pp(s1) << std::endl;
        std::clog << "expected 3th process parameter to be of type Pos " << std::endl;
        std::clog << "computed process parameter of type "  << data::pp(i->sort()) << std::endl;
      }
      BOOST_CHECK(!(std::distance(p1.begin(), i) == 2 && data::pp(i->sort()).compare("Pos") != 0));
    }
  }

  return 0;
}

