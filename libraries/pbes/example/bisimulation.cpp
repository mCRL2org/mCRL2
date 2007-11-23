#include "mcrl2/pbes/bisimulation.h"

using namespace std;
using namespace atermpp;
using namespace lps;

int main(int argc, char* argv[])
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);
  gsEnableConstructorFunctions();

  if (argc < 4)
  {
    cout << "Usage: " << argv[0] << " specfile1 specfile2 outputfile" << endl;
    return 1;
  }

  specification M; 
  specification S; 
  try
  {
    M.load(argv[1]);
    S.load(argv[2]);
    pbes<> result = strong_bisimulation(M, S);
    result.save(argv[3]);
  }
  catch (std::runtime_error e)
  {
    cout << e.what() << endl;
  }     
  
  return 0;
}
