// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file builder_test.cpp
/// \brief Builder tests.

#include <set>
#include <iostream>
#include <boost/test/minimal.hpp>

#include "mcrl2/data/builder.h"
#include "mcrl2/data/add_binding.h"
#include "mcrl2/data/parse.h"

using namespace mcrl2;
using namespace mcrl2::data;

variable bool_(std::string name)
{
  return variable(name, sort_bool::bool_());
}

template <typename Derived>
struct my_builder: public add_data_variable_binding<data::data_expression_builder, Derived>
{
  typedef add_data_variable_binding<data::data_expression_builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::is_bound;

  std::multiset<variable> unbound;

  data_expression operator()(const variable& v)
  {
    if (!is_bound(v))
    {
      unbound.insert(v);
    }
    return super::operator()(v);
  }
};

template <template <class> class Builder, template <template <class> class, class> class Binder, class Substitution>
struct replace_free_variables_builder: public Binder<Builder, replace_free_variables_builder<Builder, Binder, Substitution> >
{
  typedef Binder<Builder, replace_free_variables_builder<Builder, Binder, Substitution> > super;
  using super::enter;
  using super::leave;
  using super::operator();
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

  data_expression operator()(const variable& v)
  {
    if (is_bound(v))
    {
      return v;
    }
    return sigma(v);
  }
};

struct subst: public std::unary_function<data::variable, data::data_expression>
{
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
  data_expression x = parse_data_expression("exists b: Bool. if(c, c, b)", v.begin(), v.end());
  core::apply_builder<my_builder> f;
  f(x);
  BOOST_CHECK(f.unbound.size() == 2);
  size_t count = f.unbound.erase(bool_("c"));
  BOOST_CHECK(count == 2);

  std::multiset<variable> bound_variables;
  bound_variables.insert(bool_("c"));
  data::data_expression y = make_replace_free_variables_builder<data::data_expression_builder, data::add_data_variable_binding>(subst())(x);
  data::data_expression z = make_replace_free_variables_builder<data::data_expression_builder, data::add_data_variable_binding>(subst(), bound_variables)(x);
  BOOST_CHECK(y == parse_data_expression("exists b: Bool. if(d, d, b)", v.begin(), v.end()));
  BOOST_CHECK(z == x);
}

int test_main(int argc, char* argv[])
{
  test_binding();

  return EXIT_SUCCESS;
}
