#include <iostream>
#include <string>
#include <iterator>
#include <fstream>

#include "lpe/pbes.h"

using namespace std;
using namespace atermpp;
using namespace lpe;

int main()
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  gsEnableConstructorFunctions(); 
  
  pbes p;
  if (!p.load("data/1.pbes"))
  {
    cerr << "could not load data/1.pbes" << endl;
    return 1;
  }
  p.save("data/1_copy.pbes");

  return 0;
}
