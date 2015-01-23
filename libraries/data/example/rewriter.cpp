#include <iostream>
#include <string>
#include <cassert>
#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/substitutions/map_substitution.h"

using namespace mcrl2;
using namespace mcrl2::data;

void rewrite1()
{
  data_specification data_spec;
  data_spec.add_context_sort(sort_nat::nat());
  rewriter r(data_spec);

  // Rewrite two data expressions, and check if they are the same
  data_expression d1 = parse_data_expression("2+7");
  data_expression d2 = parse_data_expression("4+5");
  assert(d1 != d2);
  assert(r(d1) == r(d2));
}

void rewrite2()
{
  data_specification data_spec;
  data_spec.add_context_sort(sort_nat::nat());
  rewriter r(data_spec);

  // Create a substitution sequence sigma with two substitutions: [m:=3, n:=4]
  std::string var_decl = "m, n: Pos;\n";
  std::map<variable, data_expression> substitutions;
  substitutions[atermpp::down_cast<variable>(parse_data_expression("m", var_decl))] = r(parse_data_expression("3"));
  substitutions[atermpp::down_cast<variable>(parse_data_expression("n", var_decl))] = r(parse_data_expression("4"));
  map_substitution<std::map<variable, data_expression> > sigma(substitutions);

  // Rewrite two data expressions, and check if they are the same
  data::data_expression d1 = parse_data_expression("m+n", var_decl);
  data::data_expression d2 = parse_data_expression("7");
  assert(r(d1, sigma) == r(d2));
}

int main(int argc, char* argv[])
{
  rewrite1();
  rewrite2();

  return 0;
}
