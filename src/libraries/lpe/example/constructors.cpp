#include <iostream>
#include "lpe/specification.h"
#include "lpe/detail/mcrl22lpe.h"
#include "test_specifications.h"

using namespace std;
using namespace atermpp;
using namespace lpe;
using namespace lpe::detail;

int main()
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  gsEnableConstructorFunctions(); 

  specification spec1 = mcrl22lpe(ABP_SPECIFICATION);
  specification spec2(spec1.data(),
                      spec1.action_labels(),
                      spec1.lpe(),
                      spec1.initial_free_variables(),
                      spec1.initial_variables(),
                      spec1.initial_state());

  assert(spec1 == spec2);

  return 0;
}
