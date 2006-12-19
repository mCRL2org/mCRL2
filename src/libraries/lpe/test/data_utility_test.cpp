#include <iostream>
#include <string>
#include <set>
#include <algorithm>
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

  fresh_variable_generator generator(e, "d", d.sort());
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

  data_variable f("f:F");
  x = generator(f);
  BOOST_CHECK(x == data_variable("f:F"));
  x = generator(f);
  BOOST_CHECK(x == data_variable("f00:F"));

  data_variable_list w = make_list(data_variable("d:D"), data_variable("e:E"), data_variable("f:F"));
  std::set<std::string> context;
  context.insert("e");
  context.insert("f_00");
  data_variable_list w1 = fresh_variables(w, context);
  std::cout << "w1 = " << w1 << std::endl;
  BOOST_CHECK(std::find(w1.begin(), w1.end(), data_variable("d_01:D")) != w1.end());
  BOOST_CHECK(std::find(w1.begin(), w1.end(), data_variable("e_01:E")) != w1.end());
  BOOST_CHECK(std::find(w1.begin(), w1.end(), data_variable("f_01:F")) != w1.end());

  return 0;
}
