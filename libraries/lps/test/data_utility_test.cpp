#include <iostream>
#include <string>
#include <set>
#include <algorithm>
#include <boost/test/minimal.hpp>
#include "atermpp/atermpp.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/data_functional.h"
#include "mcrl2/data/data_utility.h"

using namespace atermpp;
using namespace lps;
using namespace lps::data_expr;

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

  std::set<identifier_string> ids = identifiers(e);
  BOOST_CHECK(ids.size() == 8);
  BOOST_CHECK(ids.find(identifier_string("d"))    != ids.end());
  BOOST_CHECK(ids.find(identifier_string("d0"))   != ids.end());
  BOOST_CHECK(ids.find(identifier_string("d00"))  != ids.end());
  BOOST_CHECK(ids.find(identifier_string("D"))    != ids.end());
  BOOST_CHECK(ids.find(identifier_string("Bool")) != ids.end());
  BOOST_CHECK(ids.find(identifier_string("=="))   != ids.end());
  BOOST_CHECK(ids.find(identifier_string("!="))   != ids.end());
  BOOST_CHECK(ids.find(identifier_string("&&"))   != ids.end());

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
