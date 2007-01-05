#include <iostream>
#include <string>
#include "lpe/pbes.h"
#include "lpe/detail/lpe2pbes.h"
#include "lpe/detail/read_text.h"

using namespace atermpp;
using namespace lpe;
using namespace lpe::detail;

int main(int argc, char* argv[])
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);
  gsEnableConstructorFunctions();

  if (argc < 4)
  {
    std::cerr << "Usage: " << argv[0] << " specification_file formula_file (timed/untimed)" << endl;
    return 1;
  }

  std::string specification_file = argv[1];
  std::string spec = read_text(specification_file, true);

  std::string formula_file = argv[2];
  std::string formula = read_text(formula_file, true);

  bool timed = (std::string(argv[3]) == "timed");

  std::string result_file;

  try
  {
    if (timed)
    {
      std::cout << formula_file << "[timed] ";
      pbes p = lpe2pbes(spec, formula, false);
      pbes_equation_list eqn(p.equations().begin(), p.equations().end());
      std::cout << pp(eqn) << std::endl;
      result_file = formula_file.substr(0, formula_file.size() - 4) + "expected_timed_result";
      p.save(result_file);
    }
    else
    {
      cout << formula_file << "[untimed] ";
      pbes p = lpe2pbes(spec, formula, true);
      pbes_equation_list eqn(p.equations().begin(), p.equations().end());
      std::cout << pp(eqn) << std::endl;
      result_file = formula_file.substr(0, formula_file.size() - 4) + "expected_untimed_result";
      p.save(result_file);
    }
  }
  catch (std::runtime_error e)
  {
    std::cerr << e.what() << std::endl;
  }     
  
  return 0;
}
