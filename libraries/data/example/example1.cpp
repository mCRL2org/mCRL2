#include <cassert>
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/parser.h"

using namespace mcrl2::data;

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  data_expression two   = parse_variable("2:Nat");
  data_expression three = parse_variable("3:Nat");
  data_expression five  = sort_nat::plus(two, three);

  assert(five.sort() == sort_nat::nat());
  return 0;
}
