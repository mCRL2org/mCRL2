#include <cstdlib>
#include <string>
#include <boost/test/minimal.hpp>
#include "lps/data.h"
#include "libstruct.h"

using namespace std;
using namespace atermpp;
using namespace lps;

int test_main(int, char*[])
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  gsEnableConstructorFunctions(); 

  ATermAppl T = gsMakeDataExprTrue();
  ATermAppl F = gsMakeDataExprFalse();
  data_expression d(T);

  BOOST_CHECK(data_expr::is_true(d));
  BOOST_CHECK(!data_expr::is_false(d));
  
  data_variable v("d:D");
  data_expression e(v);

  return 0;
}
