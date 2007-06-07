#include <iostream>
#include <string>
#include <cstdlib>
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/lps/detail/tools.h"
#include "mcrl2/lps/detail/read_text.h"

using namespace std;
using namespace lps;
using namespace lps::detail;

int main(int argc, char* argv[])
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);
  gsEnableConstructorFunctions();

  if (argc < 3)
  {
    cout << "Usage: " << argv[0] << " specification_file formula_file [timed=0]" << endl;
    return 1;
  }

  std::string spec_text    = read_text(argv[1]);
  std::string formula_text = read_text(argv[2]);
  bool timed = false;
  if (argc > 3 && (argv[3][0] == '1' || argv[3][0] == 't'))
    timed = false;

  try
  {
    pbes p = lps2pbes(spec_text, formula_text, timed);
    pbes_equation_list eqn(p.equations().begin(), p.equations().end());
    cout << pp(eqn) << endl;
  }
  catch (std::runtime_error e)
  {
    cout << e.what() << endl;
  }     
  
  return 0;
}
