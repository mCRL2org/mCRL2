#include <string>
#include "mcrl2/lps/constelm.h"
#include "mcrl2/old_data/rewriter.h"

using namespace mcrl2;

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  std::string infile(argv[1]);
  lps::specification spec1;

  // This is a workaround for undefined behavior of the ATerm Library.
  // Sometimes the bottom_of_stack variable isn't really the bottom of stack,
  // which causes the ATerm Library to garbage collect the wrong terms.
  spec1.protect();

  spec1.load(argv[1]);
  old_data::rewriter r(spec1.data());
  lps::specification spec2;
  spec2 = lps::constelm(spec1, r, true);
  spec2.save(argv[2]);

  return 0;
}
