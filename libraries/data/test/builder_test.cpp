// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file builder_test.cpp
/// \brief Builder tests.

#define BOOST_TEST_MODULE builder_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/builder.h"
#include "mcrl2/data/parse.h"

using namespace mcrl2;
using namespace mcrl2::data;

variable bool_(const std::string& name)
{
  return variable(name, sort_bool::bool_());
}

template <typename Derived>
struct my_builder: public add_data_variable_binding<data::data_expression_builder, Derived>
{
  using super = add_data_variable_binding<data::data_expression_builder, Derived>;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
  using super::is_bound;

  std::multiset<variable> unbound;

  template <class T>
  void apply(T& result, const variable& v)
  {
    if (!is_bound(v))
    {
      unbound.insert(v);
    }
    super::apply(result, v);
  }
};

template <template <class> class Builder, template <template <class> class, class> class Binder, class Substitution>
struct replace_free_variables_builder: public Binder<Builder, replace_free_variables_builder<Builder, Binder, Substitution> >
{
  using super = Binder<Builder, replace_free_variables_builder<Builder, Binder, Substitution>>;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
  using super::is_bound;
  using super::increase_bind_count;

  Substitution sigma;

  replace_free_variables_builder(Substitution sigma_)
    : sigma(sigma_)
  {}

  template <typename VariableContainer>
  replace_free_variables_builder(Substitution sigma_, const VariableContainer& bound_variables)
    : sigma(sigma_)
  {
    increase_bind_count(bound_variables);
  }

  template <class T>
  void apply(T& result, const variable& v)
  {
    if (is_bound(v))
    {
      result = v;
      return;
    }
    result = sigma(v);
  }
};

struct subst
{
  using argument_type = variable;
  using result_type = data_expression;

  data_expression operator()(const variable& v)
  {
    if (v == bool_("b"))
    {
      return bool_("a");
    }
    else if (v == bool_("c"))
    {
      return bool_("d");
    }
    return bool_("e");
  }
};

template <template <class> class Builder, template <template <class> class, class> class Binder, class Substitution>
replace_free_variables_builder<Builder, Binder, Substitution>
make_replace_free_variables_builder(Substitution sigma)
{
  return replace_free_variables_builder<Builder, Binder, Substitution>(sigma);
}

template <template <class> class Builder, template <template <class> class, class> class Binder, class Substitution, class VariableContainer>
replace_free_variables_builder<Builder, Binder, Substitution>
make_replace_free_variables_builder(Substitution sigma, const VariableContainer& bound_variables)
{
  return replace_free_variables_builder<Builder, Binder, Substitution>(sigma, bound_variables);
}

void test_binding()
{
  variable_vector v;
  v.push_back(bool_("c"));
  v.push_back(bool_("d"));
  data_expression x = parse_data_expression("exists b: Bool. if(c, c, b)", v);
  core::apply_builder<my_builder> f;
  data_expression result;
  f.apply(result, x);
  BOOST_CHECK(f.unbound.size() == 2);
  std::size_t count = f.unbound.erase(bool_("c"));
  BOOST_CHECK(count == 2);

  std::multiset<variable> bound_variables;
  bound_variables.insert(bool_("c"));
  data::data_expression y; 
  make_replace_free_variables_builder<data::data_expression_builder, data::add_data_variable_binding>(subst()).apply(y, x);
  data::data_expression z;
  make_replace_free_variables_builder<data::data_expression_builder, data::add_data_variable_binding>(subst(), bound_variables).apply(z, x);
  BOOST_CHECK(y == parse_data_expression("exists b: Bool. if(d, d, b)", v));
  BOOST_CHECK(z == x);
}

BOOST_AUTO_TEST_CASE(test_main)
{
  test_binding();
}
