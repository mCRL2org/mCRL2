#include <string>
#include <iostream>
#include <boost/test/minimal.hpp>

#include "atermpp/vector.h"
#include "atermpp/aterm_access.h"
#include "lpe/data_functional.h"
#include "lpe/data_operators.h"
#include "lpe/data_init.h"

using namespace lpe;
using namespace atermpp;
using namespace lpe::data_init;

int test_main(int, char*[])
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  gsEnableConstructorFunctions(); 

  data_expression d;
  data_expression d1 = pos(2);
  data_expression d2 = pos(4);
  d = negate       (d1   ); BOOST_CHECK(is_negate       (d));
  d = plus         (d1,d2); BOOST_CHECK(is_plus         (d));
  d = minus        (d1,d2); BOOST_CHECK(is_minus        (d));
  d = multiplies   (d1,d2); BOOST_CHECK(is_multiplies   (d));
  d = divides      (d1,d2); BOOST_CHECK(is_divides      (d));
  d = modulus      (d1,d2); BOOST_CHECK(is_modulus      (d));
  d = equal_to     (d1,d2); BOOST_CHECK(is_equal_to     (d));
  d = not_equal_to (d1,d2); BOOST_CHECK(is_not_equal_to (d));
  d = less         (d1,d2); BOOST_CHECK(is_less         (d));
  d = greater      (d1,d2); BOOST_CHECK(is_greater      (d));
  d = less_equal   (d1,d2); BOOST_CHECK(is_less_equal   (d));
  d = greater_equal(d1,d2); BOOST_CHECK(is_greater_equal(d));
  d = min_         (d1,d2); BOOST_CHECK(is_min          (d));
  d = max_         (d1,d2); BOOST_CHECK(is_max          (d));
  d = abs          (d1   ); BOOST_CHECK(is_abs          (d));

  return 0;
}
