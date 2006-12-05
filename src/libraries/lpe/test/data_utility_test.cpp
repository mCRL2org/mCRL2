#include <iostream>
#include <string>
#include <set>
#include <boost/test/minimal.hpp>
#include "atermpp/atermpp.h"
#include "lpe/data.h"
#include "lpe/data_init.h"
#include "lpe/data_functional.h"
#include "lpe/data_utility.h"

using namespace atermpp;
using namespace lpe;
using namespace lpe::data_init;

int test_main(int, char*[])
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  gsEnableConstructorFunctions(); 

  data_variable d("d:D");
  data_variable d0("d0:D");
  data_variable d00("d00:D");
  data_expression e = and_(equal_to(d, d0), not_equal_to(d0, d00));

  fresh_variable_generator generator(e, "d", d.type());
  data_variable x = generator();
  BOOST_CHECK(x == data_variable("d01:D"));
  x = generator();
  BOOST_CHECK(x == data_variable("d02:D"));

  std::set<aterm_string> ids = identifiers(e);
  BOOST_CHECK(ids.size() == 8);
  BOOST_CHECK(ids.find(aterm_string("d"))    != ids.end());
  BOOST_CHECK(ids.find(aterm_string("d0"))   != ids.end());
  BOOST_CHECK(ids.find(aterm_string("d00"))  != ids.end());
  BOOST_CHECK(ids.find(aterm_string("D"))    != ids.end());
  BOOST_CHECK(ids.find(aterm_string("Bool")) != ids.end());
  BOOST_CHECK(ids.find(aterm_string("=="))   != ids.end());
  BOOST_CHECK(ids.find(aterm_string("!="))   != ids.end());
  BOOST_CHECK(ids.find(aterm_string("&&"))   != ids.end());

  data_variable_list l = fresh_variable_list(3, e, "e", lpe::sort("E"));
  data_variable_list::iterator i = l.begin();
  BOOST_CHECK(*i++ == data_variable("e:E"));
  BOOST_CHECK(*i++ == data_variable("e00:E"));
  BOOST_CHECK(*i   == data_variable("e01:E"));

  return 0;
}
