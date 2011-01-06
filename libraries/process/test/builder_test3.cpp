// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file builder_test3.cpp
/// \brief Test for expression builder.

#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/core/builder.h"

using namespace mcrl2;

namespace my_data {

  template <template <class> class Builder, class Derived>
  struct add_data_dependencies: public Builder<add_data_dependencies<Builder, Derived> >
  {
    typedef Builder<add_data_dependencies<Builder, Derived> > super;
    using super::enter;
    using super::leave;
    using super::operator();

    data::data_expression operator()(const data::identifier& x)
    {
      static_cast<Derived&>(*this).enter(x);  
      // skip
      static_cast<Derived&>(*this).leave(x);
      return x;
    }
    
    data::data_expression operator()(const data::variable& x)
    {
      static_cast<Derived&>(*this).enter(x);  
      // skip
      static_cast<Derived&>(*this).leave(x);
      return x;
    }
    
    data::data_expression operator()(const data::function_symbol& x)
    {
      static_cast<Derived&>(*this).enter(x);  
      // skip
      static_cast<Derived&>(*this).leave(x);
      return x;
    }
    
    data::data_expression operator()(const data::application& x)
    {
      static_cast<Derived&>(*this).enter(x);  
      data::data_expression result = data::application(static_cast<Derived&>(*this)(x.head()), static_cast<Derived&>(*this)(x.arguments()));
      static_cast<Derived&>(*this).leave(x);
      return result;
    }
    
    data::data_expression operator()(const data::where_clause& x)
    {
      static_cast<Derived&>(*this).enter(x);  
      data::data_expression result = data::where_clause(static_cast<Derived&>(*this)(x.body()), static_cast<Derived&>(*this)(x.declarations()));
      static_cast<Derived&>(*this).leave(x);
      return result;
    }
    
    data::assignment_expression operator()(const data::assignment& x)
    {
      static_cast<Derived&>(*this).enter(x);  
      data::assignment_expression result = data::assignment(static_cast<Derived&>(*this)(x.lhs()), static_cast<Derived&>(*this)(x.rhs()));
      static_cast<Derived&>(*this).leave(x);
      return result;
    }
    
    data::assignment_expression operator()(const data::identifier_assignment& x)
    {
      static_cast<Derived&>(*this).enter(x);  
      data::assignment_expression result = data::identifier_assignment(static_cast<Derived&>(*this)(x.lhs()), static_cast<Derived&>(*this)(x.rhs()));
      static_cast<Derived&>(*this).leave(x);
      return result;
    }
    
    data::abstraction operator()(const data::forall& x)
    {
      static_cast<Derived&>(*this).enter(x);  
      data::abstraction result = data::forall(static_cast<Derived&>(*this)(x.variables()), static_cast<Derived&>(*this)(x.body()));
      static_cast<Derived&>(*this).leave(x);
      return result;
    }
    
    data::abstraction operator()(const data::exists& x)
    {
      static_cast<Derived&>(*this).enter(x);  
      data::abstraction result = data::exists(static_cast<Derived&>(*this)(x.variables()), static_cast<Derived&>(*this)(x.body()));
      static_cast<Derived&>(*this).leave(x);
      return result;
    }
    
    data::abstraction operator()(const data::lambda& x)
    {
      static_cast<Derived&>(*this).enter(x);  
      data::abstraction result = data::lambda(static_cast<Derived&>(*this)(x.variables()), static_cast<Derived&>(*this)(x.body()));
      static_cast<Derived&>(*this).leave(x);
      return result;
    }
    
    data::data_equation operator()(const data::data_equation& x)
    {
      static_cast<Derived&>(*this).enter(x);  
      data::data_equation result = data::data_equation(static_cast<Derived&>(*this)(x.variables()), static_cast<Derived&>(*this)(x.condition()), static_cast<Derived&>(*this)(x.lhs()), static_cast<Derived&>(*this)(x.rhs()));
      static_cast<Derived&>(*this).leave(x);
      return result;
    }
    
    data::data_expression operator()(const data::data_expression& x)
    {
      static_cast<Derived&>(*this).enter(x);  
      data::data_expression result;
      if (data::is_abstraction(x)) { result = static_cast<Derived&>(*this)(data::abstraction(atermpp::aterm_appl(x))); }
      else if (data::is_identifier(x)) { result = static_cast<Derived&>(*this)(data::identifier(atermpp::aterm_appl(x))); }
      else if (data::is_variable(x)) { result = static_cast<Derived&>(*this)(data::variable(atermpp::aterm_appl(x))); }
      else if (data::is_function_symbol(x)) { result = static_cast<Derived&>(*this)(data::function_symbol(atermpp::aterm_appl(x))); }
      else if (data::is_application(x)) { result = static_cast<Derived&>(*this)(data::application(atermpp::aterm_appl(x))); }
      else if (data::is_where_clause(x)) { result = static_cast<Derived&>(*this)(data::where_clause(atermpp::aterm_appl(x))); }
      static_cast<Derived&>(*this).leave(x);
      return result;
    }
    
    data::assignment_expression operator()(const data::assignment_expression& x)
    {
      static_cast<Derived&>(*this).enter(x);  
      data::assignment_expression result;
      if (data::is_assignment(x)) { result = static_cast<Derived&>(*this)(data::assignment(atermpp::aterm_appl(x))); }
      else if (data::is_identifier_assignment(x)) { result = static_cast<Derived&>(*this)(data::identifier_assignment(atermpp::aterm_appl(x))); }
      static_cast<Derived&>(*this).leave(x);
      return result;
    }
    
    data::abstraction operator()(const data::abstraction& x)
    {
      static_cast<Derived&>(*this).enter(x);  
      data::abstraction result;
      if (data::is_forall(x)) { result = static_cast<Derived&>(*this)(data::forall(atermpp::aterm_appl(x))); }
      else if (data::is_exists(x)) { result = static_cast<Derived&>(*this)(data::exists(atermpp::aterm_appl(x))); }
      else if (data::is_lambda(x)) { result = static_cast<Derived&>(*this)(data::lambda(atermpp::aterm_appl(x))); }
      static_cast<Derived&>(*this).leave(x);
      return result;
    }
  };

  template <typename Derived>
  struct data_expression_builder: public my_data::add_data_dependencies<core::builder, my_data::data_expression_builder<Derived> >
  {
    typedef my_data::add_data_dependencies<core::builder, my_data::data_expression_builder<Derived> > super;
    using super::enter;
    using super::leave;
    using super::operator();
  };

} // namespace my_data

namespace my_lps {

  template <template <class> class Builder, class Derived>
  struct add_data_dependencies: public Builder<add_data_dependencies<Builder, Derived> >
  {
    typedef Builder<add_data_dependencies<Builder, Derived> > super;
    using super::enter;
    using super::leave;
    using super::operator();

    lps::process_initializer operator()(const lps::process_initializer& x)
    {
      static_cast<Derived&>(*this).enter(x);  
      lps::process_initializer result = lps::process_initializer(static_cast<Derived&>(*this)(x.assignments()));
      static_cast<Derived&>(*this).leave(x);
      return result;
    }   
  };

  template <typename Derived>
  struct data_expression_builder: public my_lps::add_data_dependencies<my_data::data_expression_builder, my_lps::data_expression_builder<Derived> >
  {
    typedef my_lps::add_data_dependencies<my_data::data_expression_builder, my_lps::data_expression_builder<Derived> > super;
    using super::enter;
    using super::leave;
    using super::operator();
  };

} // namespace my_lps

void test1()
{
  data::data_expression x;
  core::apply_builder<my_data::data_expression_builder>()(x);
  core::apply_builder<my_lps::data_expression_builder>()(x);

  lps::process_initializer i;
#ifndef BOOST_MSVC    
  core::apply_builder<my_lps::data_expression_builder>()(i);
#endif
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv);

  test1();

  return EXIT_SUCCESS;
}
