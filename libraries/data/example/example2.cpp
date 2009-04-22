#include "mcrl2/data/data.h"
#include "mcrl2/data/parser.h"
#include <iostream>

using namespace mcrl2::core;
using namespace mcrl2::data;

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  // note that the newline characters are significant
  data_specification data_spec = parse_data_specification(
    "sort                      \n"
    "  Bit   = struct b0 | b1; \n"
    "                          \n"
    "map                       \n"
    "  invert: Bit -> Bit;     \n"
    "                          \n"
    "cons                      \n"
    "  K: Pos;                 \n"
    "                          \n"
    "eqn                       \n"
    "  invert(b1)= b0;         \n"
    "  invert(b0)= b1;         \n"
  );

  // pretty print the data specification
  std::cout << data::pp(data_spec) << "\n";

  return 0;
}
