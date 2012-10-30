// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parse_test.cpp
/// \brief Add your file description here.

#include <fstream>
#include <iostream>
#include <sstream>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/test/minimal.hpp>

#include "mcrl2/lts/parse.h"

using namespace mcrl2;

void parse_fsm(const std::string& text, lts::lts_fsm_t& result)
{
  std::string temp_filename = "parse_test.tmp";
  std::ofstream to(temp_filename.c_str());
  to << text;
  to.close();
  result.loadnew(temp_filename);
  boost::filesystem::remove(boost::filesystem::path(temp_filename));
}

inline
std::string print_state_label(const lts::detail::state_label_fsm& label)
{
  std::ostringstream out;
  for (lts::detail::state_label_fsm::const_iterator i = label.begin(); i != label.end(); ++i)
  {
    out << *i << " ";
  }
  return out.str();
}

inline
std::string print_fsm(const lts::lts_fsm_t& l)
{
  std::ostringstream out;

  out << "#states: "         << l.num_state_labels() << "\n" <<
         "#action labels: "  << l.num_action_labels() << "\n"<<
         "#transitions: "    << l.num_transitions() << "\n" <<
         "#has state labels" << (l.has_state_info() ? " yes\n" : " no\n");

  out << "Initial state is " << l.initial_state() << "\n";

  for(unsigned int i = 0; i < l.num_state_labels(); ++i)
  {
    out << "State " << i << " has value " << print_state_label(l.state_label(i)) << "\n";
  }

  for (unsigned int i = 0; i < l.num_action_labels(); ++i)
  {
    out << "Action label " << i << " has value " << l.action_label(i) << (l.is_tau(i) ? " (is internal)" : " (is external)") << "\n";
  }

  for(std::vector<lts::transition>::const_iterator i = l.get_transitions().begin(); i != l.get_transitions().end(); ++i)
  {
    out << "Transition [" << i->from() << "," << i->label() << "," << i->to() << "]\n";
  }

  return out.str();
}

void parse_dot(const std::string& text, lts::lts_dot_t& result)
{
  std::string temp_filename = "parse_test.tmp";
  std::ofstream to(temp_filename.c_str());
  to << text;
  to.close();
  result.loadnew(temp_filename);
  boost::filesystem::remove(boost::filesystem::path(temp_filename));
}

std::string print_dot(const lts::lts_dot_t& dot)
{
  std::cout << "<print_dot>" << dot.num_states() << " " << dot.num_state_labels() << std::endl;
  std::string temp_filename = "parse_test.dot";
  dot.save(temp_filename);
  std::string result = utilities::read_text(temp_filename);
  boost::filesystem::remove(boost::filesystem::path(temp_filename));
  return result;
}

void test_fsm()
{
  lts::lts_fsm_t fsm1;
  lts::lts_fsm_t fsm2;
  std::string text1;
  std::string text2;
  std::string FSM;

  FSM =
    "b(2) Bool \"F\" \"T\" \n"
    "n(2) Nat \"1\" \"2\"  \n"
    "---                   \n"
    "0 0                   \n"
    "0 1                   \n"
    "1 0                   \n"
    "1 1                   \n"
    "---                   \n"
    "1 2 \"increase\"      \n"
    "1 3 \"on\"            \n"
    "2 4 \"on\"            \n"
    "2 1 \"decrease\"      \n"
    "3 1 \"off\"           \n"
    "3 4 \"increase\"      \n"
    "4 2 \"off\"           \n"
    "4 3 \"decrease\"      \n"
    ;
  lts::parse_fsm_specification(FSM, fsm1);
  parse_fsm(FSM, fsm2);
  text1 = print_fsm(fsm1);
  text2 = print_fsm(fsm2);
  if (text1 != text2)
  {
    std::cout << "--- text1 ---" << std::endl;
    std::cout << text1 << std::endl;
    std::cout << "--- text2 ---" << std::endl;
    std::cout << text2 << std::endl;
  }
  BOOST_CHECK(text1 == text2);

  FSM =
    "b(2) Bool # Bool -> Nat \"F\" \"T\" \n"
    "n(2) Nat -> Nat \"1\" \"2\"         \n"
    "---                                 \n"
    "0 0                                 \n"
    "0 1                                 \n"
    "1 0                                 \n"
    "1 1                                 \n"
    "---                                 \n"
    "1 2 \"increase\"                    \n"
    "1 3 \"on\"                          \n"
    "2 4 \"on\"                          \n"
    "2 1 \"decrease\"                    \n"
    "3 1 \"off\"                         \n"
    "3 4 \"increase\"                    \n"
    "4 2 \"off\"                         \n"
    "4 3 \"decrease\"                    \n"
    ;
  lts::parse_fsm_specification(FSM, fsm1);
  parse_fsm(FSM, fsm2);
  text1 = print_fsm(fsm1);
  text2 = print_fsm(fsm2);
  BOOST_CHECK(text1 == text2);
}

void test_dot_abp()
{
  lts::lts_dot_t dot1;
  lts::lts_dot_t dot2;
  std::string text1;
  std::string text2;
  std::string DOT;

  DOT = "digraph \"abp.dot\" {                                       \n"
        "center = TRUE;                                              \n"
        "mclimit = 10.0;                                             \n"
        "nodesep = 0.05;                                             \n"
        "node [ width=0.25, height=0.25, label=\"\" ];               \n"
        "s0 [ peripheries=2 ];                                       \n"
        "s0 [ label=\"(1,d1,true,1,d1,true,1,true,1,d1,true)\" ];    \n"
        "s1 [ label=\"(2,d1,true,1,d1,true,1,true,1,d1,true)\" ];    \n"
        "s2 [ label=\"(2,d2,true,1,d1,true,1,true,1,d1,true)\" ];    \n"
        "s3 [ label=\"(3,d1,true,2,d1,true,1,true,1,d1,true)\" ];    \n"
        "s4 [ label=\"(3,d2,true,2,d2,true,1,true,1,d1,true)\" ];    \n"
        "s5 [ label=\"(3,d1,true,4,d1,true,1,true,1,d1,true)\" ];    \n"
        "s6 [ label=\"(3,d1,true,3,d1,true,1,true,1,d1,true)\" ];    \n"
        "s7 [ label=\"(3,d2,true,4,d1,true,1,true,1,d1,true)\" ];    \n"
        "s8 [ label=\"(3,d2,true,3,d2,true,1,true,1,d1,true)\" ];    \n"
        "s9 [ label=\"(3,d1,true,1,d1,true,1,true,4,d1,true)\" ];    \n"
        "s10 [ label=\"(3,d1,true,1,d1,true,1,true,2,d1,true)\" ];   \n"
        "s11 [ label=\"(3,d2,true,1,d1,true,1,true,4,d1,true)\" ];   \n"
        "s12 [ label=\"(3,d2,true,1,d1,true,1,true,2,d2,true)\" ];   \n"
        "s13 [ label=\"(3,d1,true,1,d1,true,2,false,1,d1,true)\" ];  \n"
        "s14 [ label=\"(3,d1,true,1,d1,true,1,true,3,d1,true)\" ];   \n"
        "s15 [ label=\"(3,d2,true,1,d1,true,2,false,1,d1,true)\" ];  \n"
        "s16 [ label=\"(3,d2,true,1,d1,true,1,true,3,d1,true)\" ];   \n"
        "s17 [ label=\"(3,d1,true,1,d1,true,4,true,1,d1,true)\" ];   \n"
        "s18 [ label=\"(3,d1,true,1,d1,true,3,false,1,d1,true)\" ];  \n"
        "s19 [ label=\"(3,d1,true,1,d1,true,2,true,1,d1,false)\" ];  \n"
        "s20 [ label=\"(3,d2,true,1,d1,true,4,true,1,d1,true)\" ];   \n"
        "s21 [ label=\"(3,d2,true,1,d1,true,3,false,1,d1,true)\" ];  \n"
        "s22 [ label=\"(3,d2,true,1,d1,true,2,true,1,d1,false)\" ];  \n"
        "s23 [ label=\"(3,d1,true,1,d1,true,4,true,1,d1,false)\" ];  \n"
        "s24 [ label=\"(3,d1,true,1,d1,true,3,true,1,d1,false)\" ];  \n"
        "s25 [ label=\"(3,d2,true,1,d1,true,4,true,1,d1,false)\" ];  \n"
        "s26 [ label=\"(3,d2,true,1,d1,true,3,true,1,d1,false)\" ];  \n"
        "s27 [ label=\"(2,d1,true,1,d1,true,1,true,1,d1,false)\" ];  \n"
        "s28 [ label=\"(1,d1,false,1,d1,true,1,true,1,d1,false)\" ]; \n"
        "s29 [ label=\"(2,d2,true,1,d1,true,1,true,1,d1,false)\" ];  \n"
        "s30 [ label=\"(3,d1,true,2,d1,true,1,true,1,d1,false)\" ];  \n"
        "s31 [ label=\"(2,d1,false,1,d1,true,1,true,1,d1,false)\" ]; \n"
        "s32 [ label=\"(2,d2,false,1,d1,true,1,true,1,d1,false)\" ]; \n"
        "s33 [ label=\"(3,d2,true,2,d2,true,1,true,1,d1,false)\" ];  \n"
        "s34 [ label=\"(3,d1,true,4,d1,true,1,true,1,d1,false)\" ];  \n"
        "s35 [ label=\"(3,d1,true,3,d1,true,1,true,1,d1,false)\" ];  \n"
        "s36 [ label=\"(3,d1,false,2,d1,false,1,true,1,d1,false)\" ];\n"
        "s37 [ label=\"(3,d2,false,2,d2,false,1,true,1,d1,false)\" ];\n"
        "s38 [ label=\"(3,d2,true,4,d1,true,1,true,1,d1,false)\" ];  \n"
        "s39 [ label=\"(3,d2,true,3,d2,true,1,true,1,d1,false)\" ];  \n"
        "s40 [ label=\"(3,d1,true,1,d1,true,1,true,4,d1,false)\" ];  \n"
        "s41 [ label=\"(3,d1,false,4,d1,true,1,true,1,d1,false)\" ]; \n"
        "s42 [ label=\"(3,d1,false,3,d1,false,1,true,1,d1,false)\" ];\n"
        "s43 [ label=\"(3,d2,false,4,d1,true,1,true,1,d1,false)\" ]; \n"
        "s44 [ label=\"(3,d2,false,3,d2,false,1,true,1,d1,false)\" ];\n"
        "s45 [ label=\"(3,d2,true,1,d1,true,1,true,4,d1,false)\" ];  \n"
        "s46 [ label=\"(3,d1,false,1,d1,true,1,true,4,d1,false)\" ]; \n"
        "s47 [ label=\"(3,d1,false,1,d1,true,1,true,2,d1,false)\" ]; \n"
        "s48 [ label=\"(3,d2,false,1,d1,true,1,true,4,d1,false)\" ]; \n"
        "s49 [ label=\"(3,d2,false,1,d1,true,1,true,2,d2,false)\" ]; \n"
        "s50 [ label=\"(3,d1,false,1,d1,true,2,true,1,d1,false)\" ]; \n"
        "s51 [ label=\"(3,d1,false,1,d1,true,1,true,3,d1,false)\" ]; \n"
        "s52 [ label=\"(3,d2,false,1,d1,true,2,true,1,d1,false)\" ]; \n"
        "s53 [ label=\"(3,d2,false,1,d1,true,1,true,3,d1,false)\" ]; \n"
        "s54 [ label=\"(3,d1,false,1,d1,true,4,true,1,d1,false)\" ]; \n"
        "s55 [ label=\"(3,d1,false,1,d1,true,3,true,1,d1,false)\" ]; \n"
        "s56 [ label=\"(3,d1,false,1,d1,true,2,false,1,d1,true)\" ]; \n"
        "s57 [ label=\"(3,d2,false,1,d1,true,4,true,1,d1,false)\" ]; \n"
        "s58 [ label=\"(3,d2,false,1,d1,true,3,true,1,d1,false)\" ]; \n"
        "s59 [ label=\"(3,d2,false,1,d1,true,2,false,1,d1,true)\" ]; \n"
        "s60 [ label=\"(3,d1,false,1,d1,true,4,true,1,d1,true)\" ];  \n"
        "s61 [ label=\"(3,d1,false,1,d1,true,3,false,1,d1,true)\" ]; \n"
        "s62 [ label=\"(3,d2,false,1,d1,true,4,true,1,d1,true)\" ];  \n"
        "s63 [ label=\"(3,d2,false,1,d1,true,3,false,1,d1,true)\" ]; \n"
        "s64 [ label=\"(2,d1,false,1,d1,true,1,true,1,d1,true)\" ];  \n"
        "s65 [ label=\"(2,d2,false,1,d1,true,1,true,1,d1,true)\" ];  \n"
        "s66 [ label=\"(3,d1,false,2,d1,false,1,true,1,d1,true)\" ]; \n"
        "s67 [ label=\"(3,d2,false,2,d2,false,1,true,1,d1,true)\" ]; \n"
        "s68 [ label=\"(3,d1,false,4,d1,true,1,true,1,d1,true)\" ];  \n"
        "s69 [ label=\"(3,d1,false,3,d1,false,1,true,1,d1,true)\" ]; \n"
        "s70 [ label=\"(3,d2,false,4,d1,true,1,true,1,d1,true)\" ];  \n"
        "s71 [ label=\"(3,d2,false,3,d2,false,1,true,1,d1,true)\" ]; \n"
        "s72 [ label=\"(3,d1,false,1,d1,true,1,true,4,d1,true)\" ];  \n"
        "s73 [ label=\"(3,d2,false,1,d1,true,1,true,4,d1,true)\" ];  \n"
        "s0->s1[label=\"r1(d1)\"];                                   \n"
        "s0->s2[label=\"r1(d2)\"];                                   \n"
        "s1->s3[label=\"c2(d1, true)\"];                             \n"
        "s2->s4[label=\"c2(d2, true)\"];                             \n"
        "s3->s5[label=\"i\"];                                        \n"
        "s3->s6[label=\"i\"];                                        \n"
        "s4->s7[label=\"i\"];                                        \n"
        "s4->s8[label=\"i\"];                                        \n"
        "s5->s9[label=\"c3(e)\"];                                    \n"
        "s6->s10[label=\"c3(d1, true)\"];                            \n"
        "s7->s11[label=\"c3(e)\"];                                   \n"
        "s8->s12[label=\"c3(d2, true)\"];                            \n"
        "s9->s13[label=\"c5(false)\"];                               \n"
        "s10->s14[label=\"s4(d1)\"];                                 \n"
        "s11->s15[label=\"c5(false)\"];                              \n"
        "s12->s16[label=\"s4(d2)\"];                                 \n"
        "s13->s17[label=\"i\"];                                      \n"
        "s13->s18[label=\"i\"];                                      \n"
        "s14->s19[label=\"c5(true)\"];                               \n"
        "s15->s20[label=\"i\"];                                      \n"
        "s15->s21[label=\"i\"];                                      \n"
        "s16->s22[label=\"c5(true)\"];                               \n"
        "s17->s1[label=\"c6(e)\"];                                   \n"
        "s18->s1[label=\"c6(false)\"];                               \n"
        "s19->s23[label=\"i\"];                                      \n"
        "s19->s24[label=\"i\"];                                      \n"
        "s20->s2[label=\"c6(e)\"];                                   \n"
        "s21->s2[label=\"c6(false)\"];                               \n"
        "s22->s25[label=\"i\"];                                      \n"
        "s22->s26[label=\"i\"];                                      \n"
        "s23->s27[label=\"c6(e)\"];                                  \n"
        "s24->s28[label=\"c6(true)\"];                               \n"
        "s25->s29[label=\"c6(e)\"];                                  \n"
        "s26->s28[label=\"c6(true)\"];                               \n"
        "s27->s30[label=\"c2(d1, true)\"];                           \n"
        "s28->s31[label=\"r1(d1)\"];                                 \n"
        "s28->s32[label=\"r1(d2)\"];                                 \n"
        "s29->s33[label=\"c2(d2, true)\"];                           \n"
        "s30->s34[label=\"i\"];                                      \n"
        "s30->s35[label=\"i\"];                                      \n"
        "s31->s36[label=\"c2(d1, false)\"];                          \n"
        "s32->s37[label=\"c2(d2, false)\"];                          \n"
        "s33->s38[label=\"i\"];                                      \n"
        "s33->s39[label=\"i\"];                                      \n"
        "s34->s40[label=\"c3(e)\"];                                  \n"
        "s35->s40[label=\"c3(d1, true)\"];                           \n"
        "s36->s41[label=\"i\"];                                      \n"
        "s36->s42[label=\"i\"];                                      \n"
        "s37->s43[label=\"i\"];                                      \n"
        "s37->s44[label=\"i\"];                                      \n"
        "s38->s45[label=\"c3(e)\"];                                  \n"
        "s39->s45[label=\"c3(d2, true)\"];                           \n"
        "s40->s19[label=\"c5(true)\"];                               \n"
        "s41->s46[label=\"c3(e)\"];                                  \n"
        "s42->s47[label=\"c3(d1, false)\"];                          \n"
        "s43->s48[label=\"c3(e)\"];                                  \n"
        "s44->s49[label=\"c3(d2, false)\"];                          \n"
        "s45->s22[label=\"c5(true)\"];                               \n"
        "s46->s50[label=\"c5(true)\"];                               \n"
        "s47->s51[label=\"s4(d1)\"];                                 \n"
        "s48->s52[label=\"c5(true)\"];                               \n"
        "s49->s53[label=\"s4(d2)\"];                                 \n"
        "s50->s54[label=\"i\"];                                      \n"
        "s50->s55[label=\"i\"];                                      \n"
        "s51->s56[label=\"c5(false)\"];                              \n"
        "s52->s57[label=\"i\"];                                      \n"
        "s52->s58[label=\"i\"];                                      \n"
        "s53->s59[label=\"c5(false)\"];                              \n"
        "s54->s31[label=\"c6(e)\"];                                  \n"
        "s55->s31[label=\"c6(true)\"];                               \n"
        "s56->s60[label=\"i\"];                                      \n"
        "s56->s61[label=\"i\"];                                      \n"
        "s57->s32[label=\"c6(e)\"];                                  \n"
        "s58->s32[label=\"c6(true)\"];                               \n"
        "s59->s62[label=\"i\"];                                      \n"
        "s59->s63[label=\"i\"];                                      \n"
        "s60->s64[label=\"c6(e)\"];                                  \n"
        "s61->s0[label=\"c6(false)\"];                               \n"
        "s62->s65[label=\"c6(e)\"];                                  \n"
        "s63->s0[label=\"c6(false)\"];                               \n"
        "s64->s66[label=\"c2(d1, false)\"];                          \n"
        "s65->s67[label=\"c2(d2, false)\"];                          \n"
        "s66->s68[label=\"i\"];                                      \n"
        "s66->s69[label=\"i\"];                                      \n"
        "s67->s70[label=\"i\"];                                      \n"
        "s67->s71[label=\"i\"];                                      \n"
        "s68->s72[label=\"c3(e)\"];                                  \n"
        "s69->s72[label=\"c3(d1, false)\"];                          \n"
        "s70->s73[label=\"c3(e)\"];                                  \n"
        "s71->s73[label=\"c3(d2, false)\"];                          \n"
        "s72->s56[label=\"c5(false)\"];                              \n"
        "s73->s59[label=\"c5(false)\"];                              \n"
        "}                                                           \n"
        ;
  lts::parse_dot_specification(DOT, dot1);
  parse_dot(DOT, dot2);
  text1 = print_dot(dot1);
  text2 = print_dot(dot2);
  if (text1 != text2)
  {
    std::cout << "--- text1 ---" << std::endl;
    std::cout << text1 << std::endl;
    std::cout << "--- text2 ---" << std::endl;
    std::cout << text2 << std::endl;
  }
  BOOST_CHECK(text1 == text2);
}

int test_main(int argc, char** argv)
{
  test_fsm();
  test_dot_abp();

  return 0;
}
