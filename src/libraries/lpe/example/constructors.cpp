#include <iostream>
#include "lpe/specification.h"

using namespace std;
using namespace atermpp;
using namespace lpe;

int main()
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  gsEnableConstructorFunctions(); 
  
  specification spec1;
  if (!spec1.load("data/abp_b.lpe"))
  {
    cerr << "could not load data/abp_b.lpe" << endl;
    return 1;
  }

  specification spec2(spec1.sorts(),
                      spec1.constructors(),
                      spec1.mappings(),
                      spec1.equations(),
                      spec1.actions(),
                      spec1.lpe(),
                      spec1.initial_free_variables(),
                      spec1.initial_variables(),
                      spec1.initial_state());

  assert(spec1 == spec2);

  return 0;
}
