#include "mcrl2/data/data.h"
#include <cassert>

using namespace mcrl2::data;

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  data_expression two   = data_expr::nat(2);
  data_expression three = data_expr::nat(3);
  data_expression five  = data_expr::plus(two, three);

  assert(five.sort() == sort_expr::nat());
  return 0;
}
