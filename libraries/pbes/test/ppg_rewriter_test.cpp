// Author(s): Gijs Kant
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ppg_rewriter_test.cpp
/// \brief Test for the ppg rewriter.

#include <iostream>
#include <boost/test/minimal.hpp>
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/detail/ppg_visitor.h"
#include "mcrl2/pbes/detail/bqnf2ppg_rewriter.h"
#include "mcrl2/pbes/detail/bqnf_traverser.h"
#include "mcrl2/pbes/detail/ppg_traverser.h"
#include "mcrl2/pbes/detail/ppg_rewriter.h"


using namespace mcrl2;
using namespace mcrl2::pbes_system;


void rewrite_ppg(std::string bqnf_text, std::string ppg_text)
{
  std::clog << "rewrite_ppg" << std::endl;
  std::clog << "Parsing text..." << std::endl;
  //std::clog << bqnf_text << std::endl;
  pbes<> p = txt2pbes(bqnf_text);
  std::clog << "done." << std::endl;

  bool is_bqnf = pbes_system::detail::is_bqnf(p);
  std::clog << "bqnf_traverser says: p is " << (is_bqnf ? "" : "NOT ") << "in BQNF." << std::endl;
  bool is_ppg = pbes_system::detail::is_ppg(p);
  std::clog << "ppg_traverser says: p is " << (is_ppg ? "" : "NOT ") << "a PPG." << std::endl;
  std::clog << "Try the new rewriter:" << std::endl;
  pbes<> q = pbes_system::detail::to_ppg(p);
  std::clog << "The new rewriter is done." << std::endl;
  //std::clog << "result:" << std::endl << pbes_system::pp(q) << std::endl << std::endl;
  is_ppg = pbes_system::detail::is_ppg(q);
  std::clog << "ppg_traverser says: result is " << (is_ppg ? "" : "NOT ") << "a PPG." << std::endl;
  p = q;
  normalize(p);
  std::clog << "Parsing text..." << std::endl;
  pbes<> ppg = txt2pbes(ppg_text);
  normalize(ppg);
  std::clog << "Checking for equality..." << std::endl;
  if (!(p==ppg))
  {
    std::clog << "target:" << std::endl << ppg_text << std::endl << std::endl;
    std::clog << "result:" << std::endl << pbes_system::pp(p) << std::endl;
  }
  BOOST_CHECK(p==ppg);
  std::clog << "done." << std::endl;
}


void test_ppg_rewriter()
{
  // buffer.always_send_and_receive
  std::string bqnf_text =
      "sort D = struct d1 | d2;\n"
      "map  N: Pos;\n"
      "eqn  N  =  2;\n"
      "pbes nu X(q_Buffer: List(D)) =\n"
      "  (exists d: D. (val(#q_Buffer < 2) && X(q_Buffer <| d))) && (exists d: D. val(head(q_Buffer) == d) && val(!(q_Buffer == [])) && X(tail(q_Buffer)));\n"
      "init X([]);"
  ;
  std::string ppg_text =
      "sort D = struct d1 | d2;\n"
      "map  N: Pos;\n"
      "eqn  N  =  2;\n"
      "pbes nu X_1(q_Buffer: List(D)) =\n"
      "  exists d: D. val(#q_Buffer < 2) && X(q_Buffer <| d);\n"
      "nu X_2(q_Buffer: List(D)) =\n"
      "  exists d: D. val(head(q_Buffer) == d) && val(!(q_Buffer == [])) && X(tail(q_Buffer));\n"
      "nu X(q_Buffer: List(D)) =\n"
      "  X_1(q_Buffer) && X_2(q_Buffer);\n"
      "init X([]);"
  ;
  rewrite_ppg(bqnf_text, ppg_text);
}


int test_main(int argc, char* argv[])
{
  test_ppg_rewriter();

  return 0;
}
