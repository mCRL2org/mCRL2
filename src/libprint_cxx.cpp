#include "libprint_cxx.h"
#include <sstream>

#define GS_PRINT_CXX 1
#include "libprint_common.h"

using namespace std;

void PrintPart_CXX(std::ostream &OutStream, const ATerm Part)
{
  PrintPart__CXX(OutStream, (ATerm) Part);
}

string PrintPart_CXX(const ATerm Part)
{
  stringstream ss;
  PrintPart_CXX(ss, Part);
  return ss.str();
}
