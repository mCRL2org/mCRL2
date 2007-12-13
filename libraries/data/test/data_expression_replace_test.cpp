#include <cstdlib>
#include <string>
#include <iostream>
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
  MCRL2_CORE_LIBRARY_INIT() 


  // y:Real
  data_variable y("y",sort_expr::real()); 
  data_expression e(y);
  // 4:Real
  data_expression x(data_expr::real(4));
  // [y]
  data_expression_list el;
  el = push_front(el, e);
  // [4]
  data_expression_list xl;
  xl = push_front(xl, x);

  // y := 4
  std::map<data_expression, data_expression> replacements;
  replacements[e] = x;

  std::cerr << e << std::endl;
  data_expression e_ = data_expression_map_replace(e, replacements);
  std::cerr << e_ << std::endl;
  BOOST_CHECK(e_ == x);

  std::cerr << xl << std::endl;
  data_expression_list xl_ = data_expression_map_replace(el, replacements);
  std::cerr << xl_ << std::endl;
  BOOST_CHECK(xl_ == xl);

  data_expression u = data_expr::plus(data_expr::real(4), data_expr::real(1));
  data_expression v = data_expr::plus(y, data_expr::real(1));
  std::cerr << "u = " << pp(u) << std::endl;
  std::cerr << "v = " << pp(v) << std::endl;
  data_expression v_ = data_expression_map_replace(v, replacements);
  std::cerr << "v_ = " << pp(v_) << std::endl;
  BOOST_CHECK(v_ != u);

  return 0;
}
