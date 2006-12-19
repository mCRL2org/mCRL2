#include <iostream>
#include <iterator>
#include <boost/test/minimal.hpp>
#include "lpe/pbes.h"
#include "lpe/detail/lpe2pbes.h"
#include "test_specifications.h"
#include "test_formulas.h"

using namespace std;
using namespace lpe;
using namespace lpe::detail;

int test_main(int, char*[])
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);
  gsEnableConstructorFunctions();

  pbes p = lpe2pbes(SPEC1, MAY_FORM, true);
  
  return 0;
}
