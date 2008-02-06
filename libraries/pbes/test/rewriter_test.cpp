// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <iterator>
#include <utility>
#include <boost/test/minimal.hpp>
#include <boost/algorithm/string.hpp>
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_translate.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/pbes_expression_builder.h"
#include "mcrl2/pbes/detail/quantifier_rename_builder.h"
#include "mcrl2/data/rewriter.h"
//#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/rewriter2.h"
#include "mcrl2/pbes/rename.h"
#include "mcrl2/pbes/complement.h"
#include "mcrl2/pbes/normalize.h"

using namespace std;
using namespace atermpp;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace mcrl2::lps::detail;
using namespace mcrl2::modal;
using namespace mcrl2::modal::detail;
using namespace mcrl2::pbes_system;
using namespace mcrl2::pbes_system::detail;

const std::string SPECIFICATION =
"act a:Nat;                               \n"
"                                         \n"
"map smaller: Nat#Nat -> Bool;            \n"
"                                         \n"
"var x,y : Nat;                           \n"
"                                         \n"
"eqn smaller(x,y) = x < y;                \n"
"                                         \n"
"proc P(n:Nat) = sum m: Nat. a(m). P(m);  \n"
"                                         \n"
"init P(0);                               \n";

const std::string FORMULA = "forall m:Nat. [a(m)]false";

data_variable nat(std::string name)
{
  return data_variable(core::identifier_string(name) , sort_expr::nat());
}

data_variable bool_(std::string name)
{
  return data_variable(core::identifier_string(name) , sort_expr::bool_());
}

void test_rewriter()
{
  specification spec    = mcrl22lps(SPECIFICATION);
  //state_formula formula = mcf2statefrm(FORMULA, spec);
  //bool timed = false;
  //pbes<> p = lps2pbes(spec, formula, timed);
  //data::rewriter datar(spec.data());
  //pbes_system::rewriter<data::rewriter> pbesr(datar, spec.data());
  //
  //for (atermpp::vector<pbes_equation>::iterator i = p.equations().begin(); i != p.equations().end(); ++i)
  //{
  //  pbes_expression e = i->formula();
  //  //std::cout << pp(e) << " -> " << pp(pbesr(e)) << std::endl;
  //  pbes_expression f = pbes_expression_substitute_and_rewrite(e, spec.data(), datar.get_rewriter());
  //  std::cout << pp(e) << " -> " << pp(f) << std::endl;
  //}

  data_variable b  = bool_("b");
  data_variable b1 = bool_("b1");
  data_variable b2 = bool_("b2");
  data_variable b3 = bool_("b3");

  data_variable n  = nat("n");
  data_variable n1 = nat("n1");
  data_variable n2 = nat("n2");
  data_variable n3 = nat("n3");

  using namespace pbes_expr_unoptimized;
  pbes_expression p = ;
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  test_rewriter();

  return 0;
}
