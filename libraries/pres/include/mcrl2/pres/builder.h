// Author(s): Jan Friso Groote. Based on builder.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/builder.h
/// \brief A builder framework for the class pres. 

#ifndef MCRL2_PRES_BUILDER_H
#define MCRL2_PRES_BUILDER_H

#include "mcrl2/data/builder.h"
#include "mcrl2/pres/pres.h"

namespace mcrl2
{

namespace pres_system
{

/// \brief Builder class
template <typename Derived>
struct pres_expression_builder_base: public core::builder<Derived>
{
  typedef core::builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
  	
  template <class T>
  void apply(T& result, const data::data_expression& x)
  {
    result = x;
  }
};

// Adds sort expression traversal to a builder
//--- start generated add_sort_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_sort_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  void update(pres_system::pres_equation& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    pres_expression result_formula;
    static_cast<Derived&>(*this).apply(result_formula, x.formula());
    x.formula() = result_formula;
    static_cast<Derived&>(*this).leave(x);
  }

  void update(pres_system::pres& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).update(x.global_variables());
    static_cast<Derived&>(*this).update(x.equations());
    propositional_variable_instantiation result_initial_state;
    static_cast<Derived&>(*this).apply(result_initial_state, x.initial_state());
    x.initial_state() = result_initial_state;
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::propositional_variable_instantiation& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_propositional_variable_instantiation(result, x.name(), [&](data::data_expression_list& result){ static_cast<Derived&>(*this).apply(result, x.parameters()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::minus& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_minus(result, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.operand()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::and_& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_and_(result, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::or_& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_or_(result, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::imp& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_imp(result, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::plus& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_plus(result, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::multiply& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_multiply(result, [&](data::data_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::minall& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_minall(result, [&](data::variable_list& result){ static_cast<Derived&>(*this).apply(result, x.variables()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.body()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::maxall& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_maxall(result, [&](data::variable_list& result){ static_cast<Derived&>(*this).apply(result, x.variables()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.body()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::sum& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_sum(result, [&](data::variable_list& result){ static_cast<Derived&>(*this).apply(result, x.variables()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.body()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::pres_expression& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<data::data_expression>(x));
    }
    else if (data::is_variable(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<data::variable>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    else if (pres_system::is_propositional_variable_instantiation(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::propositional_variable_instantiation>(x));
    }
    else if (pres_system::is_minus(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::minus>(x));
    }
    else if (pres_system::is_and(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::and_>(x));
    }
    else if (pres_system::is_or(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::or_>(x));
    }
    else if (pres_system::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::imp>(x));
    }
    else if (pres_system::is_plus(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::plus>(x));
    }
    else if (pres_system::is_multiply(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::multiply>(x));
    }
    else if (pres_system::is_minall(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::minall>(x));
    }
    else if (pres_system::is_maxall(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::maxall>(x));
    }
    else if (pres_system::is_sum(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::sum>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \\brief Builder class
template <typename Derived>
struct sort_expression_builder: public add_sort_expressions<data::sort_expression_builder, Derived>
{
};
//--- end generated add_sort_expressions code ---//

// Adds data expression traversal to a builder
//--- start generated add_data_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_data_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  void update(pres_system::pres_equation& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    pres_expression result_formula;
    static_cast<Derived&>(*this).apply(result_formula, x.formula());
    x.formula() = result_formula;
    static_cast<Derived&>(*this).leave(x);
  }

  void update(pres_system::pres& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).update(x.equations());
    propositional_variable_instantiation result_initial_state;
    static_cast<Derived&>(*this).apply(result_initial_state, x.initial_state());
    x.initial_state() = result_initial_state;
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::propositional_variable_instantiation& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_propositional_variable_instantiation(result, x.name(), [&](data::data_expression_list& result){ static_cast<Derived&>(*this).apply(result, x.parameters()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::minus& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_minus(result, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.operand()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::and_& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_and_(result, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::or_& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_or_(result, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::imp& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_imp(result, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::plus& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_plus(result, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::multiply& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_multiply(result, [&](data::data_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::minall& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_minall(result, x.variables(), [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.body()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::maxall& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_maxall(result, x.variables(), [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.body()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::sum& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_sum(result, x.variables(), [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.body()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::pres_expression& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<data::data_expression>(x));
    }
    else if (data::is_variable(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<data::variable>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    else if (pres_system::is_propositional_variable_instantiation(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::propositional_variable_instantiation>(x));
    }
    else if (pres_system::is_minus(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::minus>(x));
    }
    else if (pres_system::is_and(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::and_>(x));
    }
    else if (pres_system::is_or(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::or_>(x));
    }
    else if (pres_system::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::imp>(x));
    }
    else if (pres_system::is_plus(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::plus>(x));
    }
    else if (pres_system::is_multiply(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::multiply>(x));
    }
    else if (pres_system::is_minall(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::minall>(x));
    }
    else if (pres_system::is_maxall(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::maxall>(x));
    }
    else if (pres_system::is_sum(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::sum>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \\brief Builder class
template <typename Derived>
struct data_expression_builder: public add_data_expressions<data::data_expression_builder, Derived>
{
};
//--- end generated add_data_expressions code ---//

//--- start generated add_variables code ---//
template <template <class> class Builder, class Derived>
struct add_variables: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  void update(pres_system::pres_equation& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    pres_expression result_formula;
    static_cast<Derived&>(*this).apply(result_formula, x.formula());
    x.formula() = result_formula;
    static_cast<Derived&>(*this).leave(x);
  }

  void update(pres_system::pres& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).update(x.global_variables());
    static_cast<Derived&>(*this).update(x.equations());
    propositional_variable_instantiation result_initial_state;
    static_cast<Derived&>(*this).apply(result_initial_state, x.initial_state());
    x.initial_state() = result_initial_state;
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::propositional_variable_instantiation& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_propositional_variable_instantiation(result, x.name(), [&](data::data_expression_list& result){ static_cast<Derived&>(*this).apply(result, x.parameters()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::minus& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_minus(result, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.operand()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::and_& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_and_(result, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::or_& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_or_(result, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::imp& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_imp(result, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::plus& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_plus(result, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::multiply& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_multiply(result, [&](data::data_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::minall& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_minall(result, [&](data::variable_list& result){ static_cast<Derived&>(*this).apply(result, x.variables()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.body()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::maxall& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_maxall(result, [&](data::variable_list& result){ static_cast<Derived&>(*this).apply(result, x.variables()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.body()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::sum& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_sum(result, [&](data::variable_list& result){ static_cast<Derived&>(*this).apply(result, x.variables()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.body()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::pres_expression& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<data::data_expression>(x));
    }
    else if (data::is_variable(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<data::variable>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    else if (pres_system::is_propositional_variable_instantiation(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::propositional_variable_instantiation>(x));
    }
    else if (pres_system::is_minus(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::minus>(x));
    }
    else if (pres_system::is_and(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::and_>(x));
    }
    else if (pres_system::is_or(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::or_>(x));
    }
    else if (pres_system::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::imp>(x));
    }
    else if (pres_system::is_plus(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::plus>(x));
    }
    else if (pres_system::is_multiply(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::multiply>(x));
    }
    else if (pres_system::is_minall(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::minall>(x));
    }
    else if (pres_system::is_maxall(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::maxall>(x));
    }
    else if (pres_system::is_sum(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::sum>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \\brief Builder class
template <typename Derived>
struct variable_builder: public add_variables<data::data_expression_builder, Derived>
{
};
//--- end generated add_variables code ---//

// Adds pres expression traversal to a builder
//--- start generated add_pres_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_pres_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  void update(pres_system::pres_equation& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    pres_expression result_formula;
    static_cast<Derived&>(*this).apply(result_formula, x.formula());
    x.formula() = result_formula;
    static_cast<Derived&>(*this).leave(x);
  }

  void update(pres_system::pres& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).update(x.equations());
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::propositional_variable_instantiation& x)
  { 
    
    result = x;
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    result = x;
  }

  template <class T>
  void apply(T& result, const pres_system::minus& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_minus(result, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.operand()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::and_& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_and_(result, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::or_& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_or_(result, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::imp& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_imp(result, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::plus& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_plus(result, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::multiply& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_multiply(result, x.left(), [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::minall& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_minall(result, x.variables(), [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.body()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::maxall& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_maxall(result, x.variables(), [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.body()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::sum& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    pres_system::make_sum(result, x.variables(), [&](pres_expression& result){ static_cast<Derived&>(*this).apply(result, x.body()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const pres_system::pres_expression& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<data::data_expression>(x));
    }
    else if (data::is_variable(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<data::variable>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    else if (pres_system::is_propositional_variable_instantiation(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::propositional_variable_instantiation>(x));
    }
    else if (pres_system::is_minus(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::minus>(x));
    }
    else if (pres_system::is_and(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::and_>(x));
    }
    else if (pres_system::is_or(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::or_>(x));
    }
    else if (pres_system::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::imp>(x));
    }
    else if (pres_system::is_plus(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::plus>(x));
    }
    else if (pres_system::is_multiply(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::multiply>(x));
    }
    else if (pres_system::is_minall(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::minall>(x));
    }
    else if (pres_system::is_maxall(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::maxall>(x));
    }
    else if (pres_system::is_sum(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<pres_system::sum>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \\brief Builder class
template <typename Derived>
struct pres_expression_builder: public add_pres_expressions<pres_system::pres_expression_builder_base, Derived>
{
};
//--- end generated add_pres_expressions code ---//

} // namespace pres_system

} // namespace mcrl2

#endif // MCRL2_PRES_BUILDER_H
