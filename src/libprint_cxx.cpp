#include "libprint_cxx.h"
#include <sstream>

#define PRINT_CXX
#include "libprint_impl.h"

using namespace std;

void PrintPart_CXX(std::ostream &OutStream, const ATerm Part,
  t_pp_format pp_format)
{
  PrintPart__CXX(OutStream, (ATerm) Part, pp_format);
}

string PrintPart_CXX(const ATerm Part, t_pp_format pp_format)
{
  stringstream ss;
  PrintPart_CXX(ss, Part, pp_format);
  return ss.str();
}
