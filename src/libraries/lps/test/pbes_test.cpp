#include <iostream>
#include <iterator>
#include <utility>
#include <boost/test/minimal.hpp>
#include <boost/algorithm/string.hpp>
#include "lps/pbes.h"
#include "lps/pbes_utility.h"
#include "lps/detail/tools.h"

using namespace std;
using namespace lps;
using namespace lps::detail;

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

const std::string FORMULA  = "nu X(n:Nat = 1). [forall m:Nat. a(m)](val(n < 10)  && X(n+2))";
const std::string FORMULA2 = "forall m:Nat. [a(m)]false";

void test_pbes()
{
  specification spec    = mcrl22lps(SPECIFICATION);
  state_formula formula = mcf2statefrm(FORMULA2, spec);
  bool timed = false;
  pbes p = lps2pbes(spec, formula, timed);
  pbes_expression e = p.equations().front().formula();

  BOOST_CHECK(!p.equations().is_bes());
  BOOST_CHECK(!e.is_bes());
  
  data_expression d  = pbes2data(e, spec);
  // pbes_expression e1 = data2pbes(d);
  // BOOST_CHECK(e == e1);
}

void test_normalize()
{
  using namespace state_frm;

  state_formula x = var(identifier_string("X"), data_expression_list());
  state_formula y = var(identifier_string("Y"), data_expression_list());
  state_formula f = imp(not_(x), y);
  state_formula f1 = normalize(f);
  state_formula f2 = or_(x, y);
  std::cout << "f  = " << pp(f) << std::endl;
  std::cout << "f1 = " << pp(f1) << std::endl;
  BOOST_CHECK(f1 == f2);
}

int test_main(int argc, char* argv[])
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);
  gsEnableConstructorFunctions();

  test_pbes();
  test_normalize();

  return 0;
}
