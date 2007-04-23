// This test cannot be combined with other tests, due to limitations of the mcrl22lps code.

#include <iostream>
#include <iterator>
#include <utility>
#include <boost/test/minimal.hpp>
#include <boost/algorithm/string.hpp>
#include "lps/pbes.h"
#include "lps/pbes_translate.h"
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

  test_state_formula();

  return 0;
}
