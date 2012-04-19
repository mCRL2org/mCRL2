// Author(s): Gijs Kant
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bqnf_quantifier_rewriter_test.cpp
/// \brief Test for the bqnf_quantifier rewriter.

#include <iostream>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/atermpp/deque.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/rewrite.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;


void rewrite_bqnf_quantifier(std::string source_text, std::string target_text)
{
  pbes<> p = txt2pbes(source_text);
  bqnf_rewriter pbesr;
  pbes_rewrite(p, pbesr);
  normalize(p);
  //std::clog << pp(p);
  pbes<> target = txt2pbes(target_text);
  normalize(target);
  BOOST_CHECK(p==target);
}


void test_bqnf_quantifier_rewriter()
{
  // buffer.always_send_and_receive
  std::string source_text =
      "pbes nu X(n: Pos) =\n"
      "  forall d: Pos . (val(d < 3) => Y(d)) && (val(d > 5 && d < 7) => Z(d));\n"
      "mu Y(d: Pos) = true;\n"
      "mu Z(d: Pos) = true;\n"
      "init X(1);"
  ;
  std::string target_text =
      "pbes nu X(n: Pos) =\n"
      "  (forall d: Pos. val(!(d < 3)) || Y(d)) && (forall d: Pos. val(!(d > 5 && d < 7)) || Z(d));\n"
      "mu Y(d: Pos) = true;\n"
      "mu Z(d: Pos) = true;\n"
      "init X(1);"
  ;
  rewrite_bqnf_quantifier(source_text, target_text);
}


int test_main(int argc, char* argv[])
{
  ATinit();

  //log::log_level_t log_level = log::debug2;
  //log::mcrl2_logger::set_reporting_level(log_level);

  test_bqnf_quantifier_rewriter();

  return 0;
}
