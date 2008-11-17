#include <cassert>
#include "mcrl2/core/print.h"
#include "mcrl2/data/parser.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/identifier_generator.h"

using namespace mcrl2::core;
using namespace mcrl2::data;

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  using namespace data_expr;

  data_specification data_spec = parse_data_specification(
    "map dummy1:Pos;  \n"
    "var dummy2:Bool; \n"
    "    dummy3:Pos;  \n"
    "    dummy4:Nat;  \n"
    "    dummy5:Int;  \n"
    "    dummy6:Real; \n"
    "eqn dummy1 = 1;  \n"
  );
  rewriter r(data_spec);
  number_postfix_generator generator("x_");
  data_enumerator<number_postfix_generator> e(data_spec, r, generator);

  data_variable   n = parse_data_expression("n", "n: Pos;\n");
  data_expression c = parse_data_expression("n < 10", "n: Pos;\n");

  enumerator_expression x(c, atermpp::make_list(n));
  atermpp::vector<enumerator_expression> values = e.enumerate(x);
  for (atermpp::vector<enumerator_expression>::iterator i = values.begin(); i != values.end(); ++i)
  {
    std::cout << pp(i->expression()) << " " << pp(i->variables()) << std::endl;
  }

  return 0;
}
