#include <cassert>
#include "mcrl2/data/parser.h"
#include "mcrl2/data/rewriter.h"

using namespace mcrl2::data;

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  rewriter r = default_data_rewriter();

  // Rewrite two data expressions, and check if they are the same
  data_expression d1 = parse_data_expression("2+7");
  data_expression d2 = parse_data_expression("4+5");
  assert(r(d1) == r(d2));

  return 0;
}
