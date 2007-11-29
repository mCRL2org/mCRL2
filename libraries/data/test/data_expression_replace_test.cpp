#include <cstdlib>
#include <string>
#include <boost/test/minimal.hpp>
#include "mcrl2/data/data.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/data_expression_replace.h"
#include "atermpp/make_list.h"

using namespace std;
using namespace atermpp;
using namespace lps;

int test_main(int, char*[])
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  gsEnableConstructorFunctions(); 

  data_variable y("y",sort_expr::int_()); 
  data_expression e(y);
  data_expression x(data_expr::int_(4));
  data_expression_list el;
  el = push_front(el, e);
  data_expression_list l;
  l = push_front(l, x);

  std::map<data_expression, data_expression> replacements;
  replacements[e] = x;

  data_expression e_ = data_expression_map_replace(e, replacements);
  BOOST_CHECK(e_ == x);
  data_expression_list l_ = data_expression_map_replace(el, replacements);
  BOOST_CHECK(l_ == l);

  return 0;
}
