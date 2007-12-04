#include <string>
#include "mcrl2/lps/constelm.h"
#include "mcrl2/data/rewriter.h"

using namespace lps;
using namespace std;

int main(int argc, char* argv[])
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);
  gsEnableConstructorFunctions();

  string infile(argv[1]);
  specification spec1;

  // This is a workaround for undefined behavior of the ATerm Library.
  // Sometimes the bottom_of_stack variable isn't really the bottom of stack,
  // which causes the ATerm Library to garbage collect the wrong terms.
  spec1.protect();

  spec1.load(argv[1]);
  rewriter r(spec1.data());
  specification spec2;
  spec2 = constelm(spec1, r);
  spec2.save(argv[2]);

  return 0;
}
