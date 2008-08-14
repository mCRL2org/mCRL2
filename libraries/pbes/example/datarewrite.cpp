#include <iostream>
#include <string>
#include <cassert>
#include "mcrl2/data/parser.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/data_parse.h"

using namespace mcrl2;

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  // Add some stuff to the data specification, to make sure that rewrite
  // rules for standard types are generated.
  data::data_specification data_spec = data::parse_data_specification(
    "map MCRL2_DUMMY_1:Pos;  \n"
    "var MCRL2_DUMMY_2:Bool; \n"
    "    MCRL2_DUMMY_3:Pos;  \n"
    "    MCRL2_DUMMY_4:Nat;  \n"
    "    MCRL2_DUMMY_5:Int;  \n"
    "    MCRL2_DUMMY_6:Real; \n"
    "eqn MCRL2_DUMMY_1 = 1;  \n"
  );
  data::rewriter r(data_spec);
  data::data_expression d1 = pbes_system::parse_data_expression("1+2");
  data::data_expression d2 = pbes_system::parse_data_expression("3");
  assert(r(d1) == r(d2));

  return 0;
}
