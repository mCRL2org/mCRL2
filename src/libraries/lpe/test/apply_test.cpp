#include <sstream>
#include <boost/test/minimal.hpp>

#include "atermpp/aterm.h"
#include "atermpp/aterm_appl.h"
#include "atermpp/make_list.h"
#include "lpe/data.h"
#include "lpe/data_functional.h"

using namespace std;
using namespace atermpp;
using namespace lpe;

lpe::data_expression test_substitution(lpe::data_expression d)
{
  data_variable a("i:Int");
  data_expression b = data_expression(gsMakeDataExprInt("4"));
  data_assignment f(a, b); // i := 4

  return d.substitute(f);
}

void test_apply()
{
  data_variable a("i:Int");
  data_expression b(gsMakeDataExprInt("4"));
  data_expression c = lpe::equal_to(a, b); // i == 4

  data_expression_list cl; //singleton Container with c
  cl = push_front(cl, c);

  data_expression_list dl = apply(cl, test_substitution); // 4 == 4

  cerr << "cl: " << cl << endl;
  cerr << "dl: " << dl << endl;
}
  
int test_main( int, char*[] )
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  gsEnableConstructorFunctions(); 

  test_apply();
  return 0;
}
