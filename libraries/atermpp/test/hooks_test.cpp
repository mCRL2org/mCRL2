// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file hooks_test.cpp
/// \brief Add your file description here.

#define BOOST_TEST_MODULE hooks_test
#include <iostream>
#include <string>
#include <boost/test/included/unit_test_framework.hpp>

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_string.h"

using namespace atermpp;

static std::size_t variable_count = 0;

inline
const function_symbol& function_symbol_DataVarId()
{
  static function_symbol function_symbol_DataVarId = function_symbol("DataVarId", 1);
  return function_symbol_DataVarId;
}

class variable: public aterm_appl
{
  public:
    variable(const std::string& name)
      : aterm_appl(function_symbol_DataVarId(), aterm_string(name))
    {}

    variable(const aterm_string& name)
      : aterm_appl(function_symbol_DataVarId(), name)
    {}
};

void on_create_variable(const aterm&)
{
  variable_count++;
}

void on_delete_variable(const aterm&)
{
  variable_count--;
}

void f()
{
  BOOST_CHECK(variable_count == 1);
  variable v("v");
  BOOST_CHECK(variable_count == 1);
  variable w("w");
  BOOST_CHECK(variable_count == 2);
}

aterm_appl g()
{
  variable w("w");
  return aterm_appl(function_symbol("f", 1), w);
}

void test_hooks()
{
  add_creation_hook(function_symbol_DataVarId(), on_create_variable);
  add_deletion_hook(function_symbol_DataVarId(), on_delete_variable);
  BOOST_CHECK(variable_count == 0);
  variable v("v");
  BOOST_CHECK(variable_count == 1);
  f();
  BOOST_CHECK(variable_count == 2);
  detail::g_term_pool().collect();
  BOOST_CHECK(variable_count == 1);
  aterm_appl a = g();
  BOOST_CHECK(variable_count == 2);
  detail::g_term_pool().collect();
  BOOST_CHECK(variable_count == 2);
}

BOOST_AUTO_TEST_CASE(test_main)
{
  test_hooks();
}
