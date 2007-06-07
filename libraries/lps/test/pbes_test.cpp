#include <iostream>
#include <iterator>
#include <utility>
#include <boost/test/minimal.hpp>
#include <boost/algorithm/string.hpp>
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_utility.h"
#include "mcrl2/pbes/pbes_translate.h"
#include "mcrl2/lps/detail/tools.h"

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

void test_xyz_generator()
{
  XYZ_identifier_generator generator(propositional_variable("X1(d:D)"));
  identifier_string x;
  x = generator(); BOOST_CHECK(std::string(x) == "X");
  x = generator(); BOOST_CHECK(std::string(x) == "Y");
  x = generator(); BOOST_CHECK(std::string(x) == "Z");
  x = generator(); BOOST_CHECK(std::string(x) == "X0");
  x = generator(); BOOST_CHECK(std::string(x) == "Y0");
  x = generator(); BOOST_CHECK(std::string(x) == "Z0");
  x = generator(); BOOST_CHECK(std::string(x) == "Y1"); // X1 should be skipped
}

void test_state_formula()
{
  specification spec    = mcrl22lps(SPECIFICATION);
  state_formula formula = mcf2statefrm("mu X. mu X. X", spec);
  std::map<identifier_string, identifier_string> replacements;
  fresh_identifier_generator generator(make_list(formula, spec));
  formula = remove_name_clashes_impl(formula, generator, replacements);
  std::cout << "formula: " << pp(formula) << std::endl;
  BOOST_CHECK(pp(formula) == "mu X. mu X00. X00");
}

int test_main(int argc, char* argv[])
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);
  gsEnableConstructorFunctions();

  test_pbes();
  test_normalize();
  test_state_formula();
  test_xyz_generator();

  return 0;
}
