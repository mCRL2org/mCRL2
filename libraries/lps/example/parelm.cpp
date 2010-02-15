#include <string>
#include "mcrl2/lps/parelm.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  std::string infile(argv[1]);
  lps::specification spec1;
  spec1.load(argv[1]);
  lps::specification spec2 = lps::parelm(spec1);
  spec2.save(argv[2]);

  return 0;
}
