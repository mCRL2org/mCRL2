#include <cstdlib>
#include <string>
#include <boost/test/minimal.hpp>
#include "lpe/data.h"
#include "libstruct.h"

using namespace std;
using namespace atermpp;
using namespace lpe;

int test_main(int, char*[])
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  gsEnableConstructorFunctions(); 

  ATermAppl T = gsMakeDataExprTrue();
  ATermAppl F = gsMakeDataExprFalse();
  data_expression d(T);

  BOOST_CHECK(d.is_true());
  BOOST_CHECK(!d.is_false());

  return 0;
}
