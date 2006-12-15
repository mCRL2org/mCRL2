#include <iostream>
#include <string>
#include "lpe/pbes.h"
#include "lpe/detail/lpe2pbes.h"

using namespace std;
using namespace lpe;
using namespace lpe::detail;

inline
std::string read_text(const std::string& filename, bool warn=false)
{
  std::ifstream in(filename.c_str());
  if (!in)
  {
    if (warn)
      std::cerr << "Could not open input file: " << filename << std::endl;
    return "";
  }
  in.unsetf(std::ios::skipws); //  Turn of white space skipping on the stream

  std::string s;
  std::copy(
    std::istream_iterator<char>(in),
    std::istream_iterator<char>(),
    std::back_inserter(s)
  );

  return s;
}

int main(int argc, char* argv[])
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);
  gsEnableConstructorFunctions();

  if (argc < 3)
  {
    cout << "Usage: " << argv[0] << " specification_file formula_file" << endl;
    return 1;
  }
  std::string spec    = read_text(argv[1], true);
  std::string formula = read_text(argv[2], true);
  bool untimed = true;

  pbes p = lpe2pbes(spec, formula, untimed);
  pbes_equation_list eqn(p.equations().begin(), p.equations().end());
  cout << pp(eqn) << endl;
  
  return 0;
}
