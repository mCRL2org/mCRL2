// Author(s): Wieger Wesselink, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsconvert_test.cpp
/// \brief This file contains tests to see whether ltsconvert
//         reduces problems well.

#include <boost/test/included/unit_test_framework.hpp>

#include "mcrl2/lts/lts_aut.h"
#include "mcrl2/lts/detail/liblts_pbisim.h"

using namespace mcrl2::lts;

static probabilistic_lts_aut_t parse_aut(const std::string& s)
{
	std::stringstream is(s);
	probabilistic_lts_aut_t l;
	l.load(is);
	return l;
}

// Example below represents a.(1/2 b + 1/2 b) which can be reduced to a.b.
const std::string test1 =
"des(1 1 / 9 2 1 / 9 3 1 / 9 4 1 / 9 5 1 / 9 6 1 / 9 7 1 / 9 8 1 / 9 0, 9, 10)
(0, "player_collects_prize(false)", 9)
(1, "player_collects_prize(true)", 9)
(2, "player_collects_prize(true)", 9)
(3, "player_collects_prize(true)", 9)
(4, "player_collects_prize(false)", 9)
(5, "player_collects_prize(true)", 9)
(6, "player_collects_prize(true)", 9)
(7, "player_collects_prize(true)", 9)
(8, "player_collects_prize(false)", 9)";


BOOST_AUTO_TEST_CASE(test_state_space_reductions)
{
	std::cerr << "Test_running\n";
	probabilistic_lts_aut_t t1 = parse_aut(test1);
	detail::probabilistic_bisimulation_reduce(t1); //probabilistic bisimulation reduction
  std::cerr << "Test_finished\n";
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
	return 0;
}