#include <iostream>
#include "lps/specification.h"
#include "lps/detail/tools.h"
#include "test_specifications.h"

using namespace std;
using namespace atermpp;
using namespace lps;
using namespace lps::detail;

int main()
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  gsEnableConstructorFunctions(); 

  specification spec1 = mcrl22lps(ABP_SPECIFICATION);
  specification spec2(spec1.data(),
                      spec1.action_labels(),
                      spec1.lps(),
                      spec1.initial_process()
                     );
  assert(spec1 == spec2);

  return 0;
}
