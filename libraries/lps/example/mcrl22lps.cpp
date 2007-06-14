#include <iostream>
#include <string>
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/tools.h"
#include "mcrl2/basic/detail/read_text.h"

using namespace std;
using namespace lps;
using namespace lps::detail;

int main(int argc, char* argv[])
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);
  gsEnableConstructorFunctions();

  string infile(argv[1]);
  specification spec = mcrl22lps(read_text(infile));
  cout << "spec = " << pp(spec) << endl;

  return 0;
}
