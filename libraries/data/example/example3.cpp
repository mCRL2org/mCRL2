#include "mcrl2/data/data.h"
#include "mcrl2/data/parse.h"

using namespace mcrl2::data;

int main(int argc, char* argv[])
{
  // two ways to create the expression m + n
  std::string var_decl = "m, n: Pos;\n";
  data_expression d1 = parse_data_expression("m+n", var_decl);

  variable m("m", sort_pos::pos());
  variable n("n", sort_pos::pos());
  data_expression d2 = sort_pos::plus(m, n);

  return 0;
}
