#include <iostream>
#include <iterator>
#include <utility>
#include <boost/test/minimal.hpp>
#include <boost/algorithm/string.hpp>
#include "lpe/pbes.h"
#include "lpe/pbes_utility.h"
#include "lpe/detail/lpe2pbes.h"

using namespace std;
using namespace lpe;
using namespace lpe::detail;

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

const std::string FORMULA = "nu X(n:Nat = 1). [forall m:Nat. a(m)](val(n < 10)  && X(n+2))";

int test_main(int argc, char* argv[])
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);
  gsEnableConstructorFunctions();

  std::pair<pbes, specification> p = lpe2pbes_and_spec(SPECIFICATION, FORMULA, true);
  pbes_expression e = p.first.equations().front().formula();
  cout << "e                      = " << pp(e) << endl;

  specification spec = p.second; 
  data_expression d = pbes2data(e, spec);
  cout << "pbes2data(e)           = " << pp(d) << endl;

  pbes_expression f = data2pbes(d); 
  cout << "data2pbes(pbes2data(e) = " << pp(e) << endl;
    
  return 0;
}
