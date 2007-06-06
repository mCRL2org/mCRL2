#include <sstream>
#include "libstruct.h"
#include "libparse.h"
#include "typecheck.h"
#include "gc.h"

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

int main()
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  gsEnableConstructorFunctions(); 

  std::stringstream spec_stream;
  spec_stream << SPECIFICATION;
  ATermAppl t = parse_spec(spec_stream);
  AT_collect();
  if (t == NULL)
    return 1;
  t = type_check_spec(t);

  return 0;
}
