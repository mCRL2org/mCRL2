#include <iostream>
#include <set>
#include "mcrl2/data/classic_enumerator.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::data;

void enumerate(const std::set<variable>& variables, const data_expression& condition) 
{
  data_specification data_spec;
  data_spec.add_context_sort(sort_nat::nat());
  data_spec.add_context_sort(sort_bool::bool_());
  rewriter evaluator(data_spec);

  for (classic_enumerator<> i(data_spec, variables, evaluator, condition); i!= classic_enumerator<>(); ++i)
  {
    std::cout << core::pp((*i)(condition)) << std::endl;
  }
}

void test1()
{
  variable b("b", sort_bool::bool_());
  variable c("c", sort_bool::bool_());
  data_expression T = sort_bool::true_();
  data_expression F = sort_bool::false_();

  data_expression condition = sort_bool::and_(b, c);

  std::set<variable> variables;
  variables.insert(b);
  
  enumerate(variables, condition);
}

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv);

  test1();

  return 0;
}
