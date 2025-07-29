// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsconvert_test.cpp
/// \brief This file contains tests to see whether ltsconvert
//         reduces problems well.

#define BOOST_TEST_MODULE lts_pbisim_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/utilities/probabilistic_arbitrary_precision_fraction.h"
#include "mcrl2/lts/detail/liblts_pbisim_bem.h"
#include "mcrl2/lts/detail/liblts_pbisim_grv.h"

using namespace mcrl2::lts;

static probabilistic_lts_aut_t parse_aut(const std::string& s)
{
	std::stringstream is(s);
	probabilistic_lts_aut_t l;
	l.load(is);
	return l;
}

void execute_test(const std::string test_name,
                  const std::string input_lts,
                  const std::size_t expected_number_of_states,
                  const std::size_t expected_number_of_transitions,
                  const std::size_t expected_number_of_probabilistic_states)
{
  mcrl2::utilities::execution_timer timer;
  probabilistic_lts_aut_t t1 = parse_aut(input_lts);
  probabilistic_lts_aut_t t2=t1;
  BOOST_CHECK(t1==t2);
  detail::probabilistic_bisimulation_reduce_grv(t1,timer);
  if (t1.num_states()!=expected_number_of_states || t1.num_transitions() != expected_number_of_transitions || t1.num_probabilistic_states()!=expected_number_of_probabilistic_states)
  {
    std::cerr << "The test " << test_name << " failed using Groote, Rivera Verduzco, de Vink reduction.\n";
    std::cerr << "Expected number of states: " << expected_number_of_states << ". Actual number of states: " << t1.num_states() << "\n";
    std::cerr << "Expected number of transitions: " << expected_number_of_transitions << ". Actual number of transitions: " << t1.num_transitions() << "\n";
    std::cerr << "Expected number of probabilistic states: " << expected_number_of_probabilistic_states << ". Actual number of probabilistic states: " << t1.num_probabilistic_states() << "\n";
    BOOST_CHECK(false);
  }

  detail::probabilistic_bisimulation_reduce_bem(t2,timer);
  if (t2.num_states()!=expected_number_of_states || t2.num_transitions() != expected_number_of_transitions || t2.num_probabilistic_states()!=expected_number_of_probabilistic_states)
  {
    std::cerr << "The test " << test_name << " failed using Baier, Engelen, Majster-Cederbaum reduction.\n";
    std::cerr << "Expected number of states: " << expected_number_of_states << ". Actual number of states: " << t2.num_states() << "\n";
    std::cerr << "Expected number of transitions: " << expected_number_of_transitions << ". Actual number of transitions: " << t2.num_transitions() << "\n";
    std::cerr << "Expected number of probabilistic states: " << expected_number_of_probabilistic_states << ". Actual number of probabilistic states: " << t2.num_probabilistic_states() << "\n";
    BOOST_CHECK(false);
  }

}

// Example below represents an example using probabilistic lts.
const std::string test1 =
"des(1 1 / 9 2 1 / 9 3 1 / 9 4 1 / 9 5 1 / 9 6 1 / 9 7 1 / 9 8 1 / 9 0, 9, 10)\n"
"(0, \"player_collects_prize(false)\", 9)\n"
"(1, \"player_collects_prize(true)\", 9)\n"
"(2, \"player_collects_prize(true)\", 9)\n"
"(3, \"player_collects_prize(true)\", 9)\n"
"(4, \"player_collects_prize(false)\", 9)\n"
"(5, \"player_collects_prize(true)\", 9)\n"
"(6, \"player_collects_prize(true)\", 9)\n"
"(7, \"player_collects_prize(true)\", 9)\n"
"(8, \"player_collects_prize(false)\", 9)\n";

// Example below represents a.(1/2 b + 1/2 b) which can be reduced to a.b.
const std::string test2 =
"des (0,3,5)\n"
"(0,\"a\",1 1/2 2)\n"
"(1,\"b\",3)\n"
"(2,\"b\",4)\n";

// Example below represents a.(1/2 b + 1/2 b)+ a.(1/4 b + 3/4 b) which can be reduced to a.b.
const std::string test3 =
"des (0,4,5)\n"
"(0,\"a\",1 1/2 2)\n"
"(0,\"a\",1 1/4 2)\n"
"(1,\"b\",3)\n"
"(2,\"b\",4)\n";

//Example in paper of Christel Bier. Deciding Bisimilarity and Similarity for Probabilistic Processes.
const std::string test4 =
"des (0 1/4 5 1/4 11 1/4 14,11,19)\n"
"(0,\"a\",1)\n"
"(0,\"a\",2 1/2 3)\n"
"(2,\"b\",4)\n"
"(5,\"a\",6)\n"
"(5,\"a\",7 1/2 8 1/8 9)\n"
"(7,\"b\",10)\n"
"(11,\"a\",12)\n"
"(12,\"b\",13)\n"
"(14,\"a\",15 1/3 16)\n"
"(15,\"c\",17)\n"
"(16,\"b\",18)\n";

// Airplane ticket example test
const std::string test5 =
"des (1 1/3 0,14,15)\n"
"(0,\"enter\",3 1/4 4 1/4 5 1/4 2)\n"
"(1,\"enter\",7 1/2 6)\n"
"(2,\"enter_plane\",8)\n"
"(3,\"enter_plane\",9)\n"
"(4,\"enter_plane\",8)\n"
"(5,\"enter_plane\",8)\n"
"(6,\"enter_plane\",9)\n"
"(7,\"enter_plane\",9)\n"
"(8,\"enter\",10)\n"
"(9,\"enter\",11)\n"
"(10,\"enter_plane\",12)\n"
"(11,\"enter_plane\",13)\n"
"(12,\"last_passenger_has_his_own_seat(false)\",14)\n"
"(13,\"last_passenger_has_his_own_seat(true)\",14)\n";

// Ant grid test
const std::string test6 =
"des (1 1/4 2 1/4 3 1/4 0,240,240)\n"
"(0,\"step\",5 1/4 6 1/4 7 1/4 4)\n"
"(1,\"step\",9 1/4 10 1/4 11 1/4 8)\n"
"(2,\"step\",13 1/4 14 1/4 15 1/4 12)\n"
"(3,\"step\",17 1/4 18 1/4 19 1/4 16)\n"
"(4,\"step\",21 1/4 22 1/4 23 1/4 20)\n"
"(5,\"step\",1 1/4 2 1/4 3 1/4 0)\n"
"(6,\"step\",25 1/4 26 1/4 27 1/4 24)\n"
"(7,\"step\",29 1/4 30 1/4 31 1/4 28)\n"
"(8,\"step\",1 1/4 2 1/4 3 1/4 0)\n"
"(9,\"step\",33 1/4 34 1/4 35 1/4 32)\n"
"(10,\"step\",37 1/4 38 1/4 39 1/4 36)\n"
"(11,\"step\",41 1/4 42 1/4 43 1/4 40)\n"
"(12,\"step\",25 1/4 26 1/4 27 1/4 24)\n"
"(13,\"step\",37 1/4 38 1/4 39 1/4 36)\n"
"(14,\"step\",45 1/4 46 1/4 47 1/4 44)\n"
"(15,\"step\",1 1/4 2 1/4 3 1/4 0)\n"
"(16,\"step\",29 1/4 30 1/4 31 1/4 28)\n"
"(17,\"step\",41 1/4 42 1/4 43 1/4 40)\n"
"(18,\"step\",1 1/4 2 1/4 3 1/4 0)\n"
"(19,\"step\",49 1/4 50 1/4 51 1/4 48)\n"
"(20,\"step\",53 1/4 54 1/4 55 1/4 52)\n"
"(21,\"step\",5 1/4 6 1/4 7 1/4 4)\n"
"(22,\"step\",57 1/4 58 1/4 59 1/4 56)\n"
"(23,\"step\",61 1/4 62 1/4 63 1/4 60)\n"
"(24,\"step\",57 1/4 58 1/4 59 1/4 56)\n"
"(25,\"step\",13 1/4 14 1/4 15 1/4 12)\n"
"(26,\"step\",65 1/4 66 1/4 67 1/4 64)\n"
"(27,\"step\",5 1/4 6 1/4 7 1/4 4)\n"
"(28,\"step\",61 1/4 62 1/4 63 1/4 60)\n"
"(29,\"step\",17 1/4 18 1/4 19 1/4 16)\n"
"(30,\"step\",5 1/4 6 1/4 7 1/4 4)\n"
"(31,\"step\",69 1/4 70 1/4 71 1/4 68)\n"
"(32,\"step\",9 1/4 10 1/4 11 1/4 8)\n"
"(33,\"step\",73 1/4 74 1/4 75 1/4 72)\n"
"(34,\"step\",77 1/4 78 1/4 79 1/4 76)\n"
"(35,\"step\",81 1/4 82 1/4 83 1/4 80)\n"
"(36,\"step\",13 1/4 14 1/4 15 1/4 12)\n"
"(37,\"step\",77 1/4 78 1/4 79 1/4 76)\n"
"(38,\"step\",85 1/4 86 1/4 87 1/4 84)\n"
"(39,\"step\",9 1/4 10 1/4 11 1/4 8)\n"
"(40,\"step\",17 1/4 18 1/4 19 1/4 16)\n"
"(41,\"step\",81 1/4 82 1/4 83 1/4 80)\n"
"(42,\"step\",9 1/4 10 1/4 11 1/4 8)\n"
"(43,\"step\",89 1/4 90 1/4 91 1/4 88)\n"
"(44,\"dead\",45 1/4 46 1/4 47 1/4 44)\n"
"(45,\"dead\",45 1/4 46 1/4 47 1/4 44)\n"
"(46,\"dead\",45 1/4 46 1/4 47 1/4 44)\n"
"(47,\"dead\",45 1/4 46 1/4 47 1/4 44)\n"
"(48,\"step\",69 1/4 70 1/4 71 1/4 68)\n"
"(49,\"step\",89 1/4 90 1/4 91 1/4 88)\n"
"(50,\"step\",17 1/4 18 1/4 19 1/4 16)\n"
"(51,\"step\",93 1/4 94 1/4 95 1/4 92)\n"
"(52,\"step\",97 1/4 98 1/4 99 1/4 96)\n"
"(53,\"step\",21 1/4 22 1/4 23 1/4 20)\n"
"(54,\"step\",101 1/4 102 1/4 103 1/4 100)\n"
"(55,\"step\",105 1/4 106 1/4 107 1/4 104)\n"
"(56,\"step\",101 1/4 102 1/4 103 1/4 100)\n"
"(57,\"step\",25 1/4 26 1/4 27 1/4 24)\n"
"(58,\"step\",109 1/4 110 1/4 111 1/4 108)\n"
"(59,\"step\",21 1/4 22 1/4 23 1/4 20)\n"
"(60,\"step\",105 1/4 106 1/4 107 1/4 104)\n"
"(61,\"step\",29 1/4 30 1/4 31 1/4 28)\n"
"(62,\"step\",21 1/4 22 1/4 23 1/4 20)\n"
"(63,\"step\",113 1/4 114 1/4 115 1/4 112)\n"
"(64,\"dead\",65 1/4 66 1/4 67 1/4 64)\n"
"(65,\"dead\",65 1/4 66 1/4 67 1/4 64)\n"
"(66,\"dead\",65 1/4 66 1/4 67 1/4 64)\n"
"(67,\"dead\",65 1/4 66 1/4 67 1/4 64)\n"
"(68,\"step\",113 1/4 114 1/4 115 1/4 112)\n"
"(69,\"step\",49 1/4 50 1/4 51 1/4 48)\n"
"(70,\"step\",29 1/4 30 1/4 31 1/4 28)\n"
"(71,\"step\",117 1/4 118 1/4 119 1/4 116)\n"
"(72,\"live\",73 1/4 74 1/4 75 1/4 72)\n"
"(73,\"live\",73 1/4 74 1/4 75 1/4 72)\n"
"(74,\"live\",73 1/4 74 1/4 75 1/4 72)\n"
"(75,\"live\",73 1/4 74 1/4 75 1/4 72)\n"
"(76,\"step\",37 1/4 38 1/4 39 1/4 36)\n"
"(77,\"step\",121 1/4 122 1/4 123 1/4 120)\n"
"(78,\"step\",125 1/4 126 1/4 127 1/4 124)\n"
"(79,\"step\",33 1/4 34 1/4 35 1/4 32)\n"
"(80,\"step\",41 1/4 42 1/4 43 1/4 40)\n"
"(81,\"step\",129 1/4 130 1/4 131 1/4 128)\n"
"(82,\"step\",33 1/4 34 1/4 35 1/4 32)\n"
"(83,\"step\",133 1/4 134 1/4 135 1/4 132)\n"
"(84,\"dead\",85 1/4 86 1/4 87 1/4 84)\n"
"(85,\"dead\",85 1/4 86 1/4 87 1/4 84)\n"
"(86,\"dead\",85 1/4 86 1/4 87 1/4 84)\n"
"(87,\"dead\",85 1/4 86 1/4 87 1/4 84)\n"
"(88,\"step\",49 1/4 50 1/4 51 1/4 48)\n"
"(89,\"step\",133 1/4 134 1/4 135 1/4 132)\n"
"(90,\"step\",41 1/4 42 1/4 43 1/4 40)\n"
"(91,\"step\",137 1/4 138 1/4 139 1/4 136)\n"
"(92,\"step\",117 1/4 118 1/4 119 1/4 116)\n"
"(93,\"step\",137 1/4 138 1/4 139 1/4 136)\n"
"(94,\"step\",49 1/4 50 1/4 51 1/4 48)\n"
"(95,\"step\",141 1/4 142 1/4 143 1/4 140)\n"
"(96,\"live\",97 1/4 98 1/4 99 1/4 96)\n"
"(97,\"live\",97 1/4 98 1/4 99 1/4 96)\n"
"(98,\"live\",97 1/4 98 1/4 99 1/4 96)\n"
"(99,\"live\",97 1/4 98 1/4 99 1/4 96)\n"
"(100,\"step\",145 1/4 146 1/4 147 1/4 144)\n"
"(101,\"step\",57 1/4 58 1/4 59 1/4 56)\n"
"(102,\"step\",149 1/4 150 1/4 151 1/4 148)\n"
"(103,\"step\",53 1/4 54 1/4 55 1/4 52)\n"
"(104,\"step\",153 1/4 154 1/4 155 1/4 152)\n"
"(105,\"step\",61 1/4 62 1/4 63 1/4 60)\n"
"(106,\"step\",53 1/4 54 1/4 55 1/4 52)\n"
"(107,\"step\",157 1/4 158 1/4 159 1/4 156)\n"
"(108,\"dead\",109 1/4 110 1/4 111 1/4 108)\n"
"(109,\"dead\",109 1/4 110 1/4 111 1/4 108)\n"
"(110,\"dead\",109 1/4 110 1/4 111 1/4 108)\n"
"(111,\"dead\",109 1/4 110 1/4 111 1/4 108)\n"
"(112,\"step\",157 1/4 158 1/4 159 1/4 156)\n"
"(113,\"step\",69 1/4 70 1/4 71 1/4 68)\n"
"(114,\"step\",61 1/4 62 1/4 63 1/4 60)\n"
"(115,\"step\",161 1/4 162 1/4 163 1/4 160)\n"
"(116,\"step\",161 1/4 162 1/4 163 1/4 160)\n"
"(117,\"step\",93 1/4 94 1/4 95 1/4 92)\n"
"(118,\"step\",69 1/4 70 1/4 71 1/4 68)\n"
"(119,\"step\",165 1/4 166 1/4 167 1/4 164)\n"
"(120,\"live\",121 1/4 122 1/4 123 1/4 120)\n"
"(121,\"live\",121 1/4 122 1/4 123 1/4 120)\n"
"(122,\"live\",121 1/4 122 1/4 123 1/4 120)\n"
"(123,\"live\",121 1/4 122 1/4 123 1/4 120)\n"
"(124,\"dead\",125 1/4 126 1/4 127 1/4 124)\n"
"(125,\"dead\",125 1/4 126 1/4 127 1/4 124)\n"
"(126,\"dead\",125 1/4 126 1/4 127 1/4 124)\n"
"(127,\"dead\",125 1/4 126 1/4 127 1/4 124)\n"
"(128,\"live\",129 1/4 130 1/4 131 1/4 128)\n"
"(129,\"live\",129 1/4 130 1/4 131 1/4 128)\n"
"(130,\"live\",129 1/4 130 1/4 131 1/4 128)\n"
"(131,\"live\",129 1/4 130 1/4 131 1/4 128)\n"
"(132,\"step\",89 1/4 90 1/4 91 1/4 88)\n"
"(133,\"step\",169 1/4 170 1/4 171 1/4 168)\n"
"(134,\"step\",81 1/4 82 1/4 83 1/4 80)\n"
"(135,\"step\",173 1/4 174 1/4 175 1/4 172)\n"
"(136,\"step\",93 1/4 94 1/4 95 1/4 92)\n"
"(137,\"step\",173 1/4 174 1/4 175 1/4 172)\n"
"(138,\"step\",89 1/4 90 1/4 91 1/4 88)\n"
"(139,\"step\",177 1/4 178 1/4 179 1/4 176)\n"
"(140,\"step\",165 1/4 166 1/4 167 1/4 164)\n"
"(141,\"step\",177 1/4 178 1/4 179 1/4 176)\n"
"(142,\"step\",93 1/4 94 1/4 95 1/4 92)\n"
"(143,\"step\",181 1/4 182 1/4 183 1/4 180)\n"
"(144,\"live\",145 1/4 146 1/4 147 1/4 144)\n"
"(145,\"live\",145 1/4 146 1/4 147 1/4 144)\n"
"(146,\"live\",145 1/4 146 1/4 147 1/4 144)\n"
"(147,\"live\",145 1/4 146 1/4 147 1/4 144)\n"
"(148,\"dead\",149 1/4 150 1/4 151 1/4 148)\n"
"(149,\"dead\",149 1/4 150 1/4 151 1/4 148)\n"
"(150,\"dead\",149 1/4 150 1/4 151 1/4 148)\n"
"(151,\"dead\",149 1/4 150 1/4 151 1/4 148)\n"
"(152,\"live\",153 1/4 154 1/4 155 1/4 152)\n"
"(153,\"live\",153 1/4 154 1/4 155 1/4 152)\n"
"(154,\"live\",153 1/4 154 1/4 155 1/4 152)\n"
"(155,\"live\",153 1/4 154 1/4 155 1/4 152)\n"
"(156,\"step\",185 1/4 186 1/4 187 1/4 184)\n"
"(157,\"step\",113 1/4 114 1/4 115 1/4 112)\n"
"(158,\"step\",105 1/4 106 1/4 107 1/4 104)\n"
"(159,\"step\",189 1/4 190 1/4 191 1/4 188)\n"
"(160,\"step\",189 1/4 190 1/4 191 1/4 188)\n"
"(161,\"step\",117 1/4 118 1/4 119 1/4 116)\n"
"(162,\"step\",113 1/4 114 1/4 115 1/4 112)\n"
"(163,\"step\",193 1/4 194 1/4 195 1/4 192)\n"
"(164,\"step\",193 1/4 194 1/4 195 1/4 192)\n"
"(165,\"step\",141 1/4 142 1/4 143 1/4 140)\n"
"(166,\"step\",117 1/4 118 1/4 119 1/4 116)\n"
"(167,\"step\",197 1/4 198 1/4 199 1/4 196)\n"
"(168,\"live\",169 1/4 170 1/4 171 1/4 168)\n"
"(169,\"live\",169 1/4 170 1/4 171 1/4 168)\n"
"(170,\"live\",169 1/4 170 1/4 171 1/4 168)\n"
"(171,\"live\",169 1/4 170 1/4 171 1/4 168)\n"
"(172,\"step\",137 1/4 138 1/4 139 1/4 136)\n"
"(173,\"step\",201 1/4 202 1/4 203 1/4 200)\n"
"(174,\"step\",133 1/4 134 1/4 135 1/4 132)\n"
"(175,\"step\",205 1/4 206 1/4 207 1/4 204)\n"
"(176,\"step\",141 1/4 142 1/4 143 1/4 140)\n"
"(177,\"step\",205 1/4 206 1/4 207 1/4 204)\n"
"(178,\"step\",137 1/4 138 1/4 139 1/4 136)\n"
"(179,\"step\",209 1/4 210 1/4 211 1/4 208)\n"
"(180,\"dead\",181 1/4 182 1/4 183 1/4 180)\n"
"(181,\"dead\",181 1/4 182 1/4 183 1/4 180)\n"
"(182,\"dead\",181 1/4 182 1/4 183 1/4 180)\n"
"(183,\"dead\",181 1/4 182 1/4 183 1/4 180)\n"
"(184,\"live\",185 1/4 186 1/4 187 1/4 184)\n"
"(185,\"live\",185 1/4 186 1/4 187 1/4 184)\n"
"(186,\"live\",185 1/4 186 1/4 187 1/4 184)\n"
"(187,\"live\",185 1/4 186 1/4 187 1/4 184)\n"
"(188,\"step\",213 1/4 214 1/4 215 1/4 212)\n"
"(189,\"step\",161 1/4 162 1/4 163 1/4 160)\n"
"(190,\"step\",157 1/4 158 1/4 159 1/4 156)\n"
"(191,\"step\",217 1/4 218 1/4 219 1/4 216)\n"
"(192,\"step\",217 1/4 218 1/4 219 1/4 216)\n"
"(193,\"step\",165 1/4 166 1/4 167 1/4 164)\n"
"(194,\"step\",161 1/4 162 1/4 163 1/4 160)\n"
"(195,\"step\",221 1/4 222 1/4 223 1/4 220)\n"
"(196,\"dead\",197 1/4 198 1/4 199 1/4 196)\n"
"(197,\"dead\",197 1/4 198 1/4 199 1/4 196)\n"
"(198,\"dead\",197 1/4 198 1/4 199 1/4 196)\n"
"(199,\"dead\",197 1/4 198 1/4 199 1/4 196)\n"
"(200,\"live\",201 1/4 202 1/4 203 1/4 200)\n"
"(201,\"live\",201 1/4 202 1/4 203 1/4 200)\n"
"(202,\"live\",201 1/4 202 1/4 203 1/4 200)\n"
"(203,\"live\",201 1/4 202 1/4 203 1/4 200)\n"
"(204,\"step\",177 1/4 178 1/4 179 1/4 176)\n"
"(205,\"step\",225 1/4 226 1/4 227 1/4 224)\n"
"(206,\"step\",173 1/4 174 1/4 175 1/4 172)\n"
"(207,\"step\",229 1/4 230 1/4 231 1/4 228)\n"
"(208,\"dead\",209 1/4 210 1/4 211 1/4 208)\n"
"(209,\"dead\",209 1/4 210 1/4 211 1/4 208)\n"
"(210,\"dead\",209 1/4 210 1/4 211 1/4 208)\n"
"(211,\"dead\",209 1/4 210 1/4 211 1/4 208)\n"
"(212,\"live\",213 1/4 214 1/4 215 1/4 212)\n"
"(213,\"live\",213 1/4 214 1/4 215 1/4 212)\n"
"(214,\"live\",213 1/4 214 1/4 215 1/4 212)\n"
"(215,\"live\",213 1/4 214 1/4 215 1/4 212)\n"
"(216,\"step\",233 1/4 234 1/4 235 1/4 232)\n"
"(217,\"step\",193 1/4 194 1/4 195 1/4 192)\n"
"(218,\"step\",189 1/4 190 1/4 191 1/4 188)\n"
"(219,\"step\",237 1/4 238 1/4 239 1/4 236)\n"
"(220,\"dead\",221 1/4 222 1/4 223 1/4 220)\n"
"(221,\"dead\",221 1/4 222 1/4 223 1/4 220)\n"
"(222,\"dead\",221 1/4 222 1/4 223 1/4 220)\n"
"(223,\"dead\",221 1/4 222 1/4 223 1/4 220)\n"
"(224,\"live\",225 1/4 226 1/4 227 1/4 224)\n"
"(225,\"live\",225 1/4 226 1/4 227 1/4 224)\n"
"(226,\"live\",225 1/4 226 1/4 227 1/4 224)\n"
"(227,\"live\",225 1/4 226 1/4 227 1/4 224)\n"
"(228,\"dead\",229 1/4 230 1/4 231 1/4 228)\n"
"(229,\"dead\",229 1/4 230 1/4 231 1/4 228)\n"
"(230,\"dead\",229 1/4 230 1/4 231 1/4 228)\n"
"(231,\"dead\",229 1/4 230 1/4 231 1/4 228)\n"
"(232,\"live\",233 1/4 234 1/4 235 1/4 232)\n"
"(233,\"live\",233 1/4 234 1/4 235 1/4 232)\n"
"(234,\"live\",233 1/4 234 1/4 235 1/4 232)\n"
"(235,\"live\",233 1/4 234 1/4 235 1/4 232)\n"
"(236,\"dead\",237 1/4 238 1/4 239 1/4 236)\n"
"(237,\"dead\",237 1/4 238 1/4 239 1/4 236)\n"
"(238,\"dead\",237 1/4 238 1/4 239 1/4 236)\n"
"(239,\"dead\",237 1/4 238 1/4 239 1/4 236)\n"
;

/* The example below turned out to be a tricky example on which
 * an intermediate version of the Groote/Verduzco/Vink algorithm failed. */
std::string test7 =
"des(0,5,3)\n"
"(0,\"a\", 2)\n"
"(1,\"a\", 2 2/4 2)\n"
"(0,\"tau\",0 1/4 1 2/4 1)\n"
"(1,\"tau\",1 3/4 1)\n"
"(0,\"tau\",2 3/4 2)\n"
;


BOOST_AUTO_TEST_CASE(test_state_space_reductions)
{
  execute_test("Test1",test1,3,2,1);
  execute_test("Test2",test1,3,2,1);
  execute_test("Test3",test3,3,2,2);
  execute_test("Test4",test4,6,6,4); 
  execute_test("Test5",test5,11,10,9);
  execute_test("Test6",test6,13,13,11); 
  execute_test("Test7",test7,3,5,3);
}

// Test whether a probabilistic state of length 1,
// stored as a single number, and stored as a vector of length 1
// are considered the same object for equality and the hash function.
BOOST_AUTO_TEST_CASE(hash_and_equality_test)
{
  using prob_state = probabilistic_state<std::size_t, mcrl2::lps::probabilistic_data_expression>;
  prob_state s1(1);
  prob_state s2;
  s2.add(1,mcrl2::lps::probabilistic_data_expression::one());
  BOOST_CHECK(s1==s2);
  std::hash<prob_state> hasher;
  BOOST_CHECK(hasher(s1)==hasher(s2));

  using prob_state_alt = probabilistic_state<std::size_t, mcrl2::utilities::probabilistic_arbitrary_precision_fraction>;
  prob_state_alt s3(1);
  prob_state_alt s4;
  s4.add(1,mcrl2::utilities::probabilistic_arbitrary_precision_fraction::one());
  BOOST_CHECK(s3==s4);
  std::hash<prob_state_alt> hasher_alt;
  BOOST_CHECK(hasher_alt(s3)==hasher_alt(s4));
}
