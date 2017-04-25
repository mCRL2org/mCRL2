#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"
#include <cassert>

using namespace mcrl2::data;

int main(int argc, char* argv[])
{
 data_specification data_spec;
  data_spec.add_context_sort(sort_nat::nat());
  rewriter r(data_spec);

  // Rewrite two data expressions, and check if they are the same
  data_expression d1 = parse_data_expression("2+7");
  data_expression d2 = parse_data_expression("4+5");
  assert(r(d1) == r(d2));

  return 0;
}
