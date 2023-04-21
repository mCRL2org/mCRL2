// Author(s): Jan Friso Groote. Based on bes/builder.h  by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/res/builder.h
/// \brief A builder framework for the class res.

#ifndef MCRL2_RES_BUILDER_H
#define MCRL2_RES_BUILDER_H

#include "mcrl2/res/res_equation_system.h"
#include "mcrl2/core/builder.h"

namespace mcrl2
{

namespace res
{

//--- start generated add_boolean_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_boolean_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;


};

/// \\brief Builder class
template <typename Derived>
struct boolean_expression_builder: public add_boolean_expressions<core::builder, Derived>
{
};
//--- end generated add_boolean_expressions code ---//

//--- start generated add_boolean_variables code ---//
template <template <class> class Builder, class Derived>
struct add_boolean_variables: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  void update(res::res_equation& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    res_variable result_variable;
    static_cast<Derived&>(*this).apply(result_variable, x.variable());
    x.variable() = result_variable;
    res_expression result_formula;
    static_cast<Derived&>(*this).apply(result_formula, x.formula());
    x.formula() = result_formula;
    static_cast<Derived&>(*this).leave(x);
  }

  void update(res::res_equation_system& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).update(x.equations());
    res_expression result_initial_state;
    static_cast<Derived&>(*this).apply(result_initial_state, x.initial_state());
    x.initial_state() = result_initial_state;
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const res::true_& x)
  { 
    
    result = x;
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    result = x;
  }

  template <class T>
  void apply(T& result, const res::false_& x)
  { 
    
    result = x;
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    result = x;
  }

  template <class T>
  void apply(T& result, const res::res_variable& x)
  { 
    
    result = x;
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    result = x;
  }

  template <class T>
  void apply(T& result, const res::not_& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    res::make_not_(result, [&](res_expression& result){ static_cast<Derived&>(*this).apply(result, x.operand()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const res::and_& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    res::make_and_(result, [&](res_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](res_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const res::or_& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    res::make_or_(result, [&](res_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](res_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const res::imp& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    res::make_imp(result, [&](res_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](res_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const res::plus& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    res::make_plus(result, [&](res_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](res_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const res::multiply& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    res::make_multiply(result, x.left(), [&](res_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const res::rescondand& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    res::make_rescondand(result, [&](res_expression& result){ static_cast<Derived&>(*this).apply(result, x.arg1()); }, [&](res_expression& result){ static_cast<Derived&>(*this).apply(result, x.arg2()); }, [&](res_expression& result){ static_cast<Derived&>(*this).apply(result, x.arg3()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const res::rescondor& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    res::make_rescondor(result, [&](res_expression& result){ static_cast<Derived&>(*this).apply(result, x.arg1()); }, [&](res_expression& result){ static_cast<Derived&>(*this).apply(result, x.arg2()); }, [&](res_expression& result){ static_cast<Derived&>(*this).apply(result, x.arg3()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const res::reseqinf& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    res::make_reseqinf(result, [&](res_expression& result){ static_cast<Derived&>(*this).apply(result, x.operand()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const res::reseqninf& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    res::make_reseqninf(result, [&](res_expression& result){ static_cast<Derived&>(*this).apply(result, x.operand()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const res::res_expression& x)
  { 
    
    static_cast<Derived&>(*this).enter(x);
    if (res::is_true(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<res::true_>(x));
    }
    else if (res::is_false(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<res::false_>(x));
    }
    else if (res::is_res_variable(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<res::res_variable>(x));
    }
    else if (res::is_not(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<res::not_>(x));
    }
    else if (res::is_and(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<res::and_>(x));
    }
    else if (res::is_or(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<res::or_>(x));
    }
    else if (res::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<res::imp>(x));
    }
    else if (res::is_plus(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<res::plus>(x));
    }
    else if (res::is_multiply(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<res::multiply>(x));
    }
    else if (res::is_rescondand(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<res::rescondand>(x));
    }
    else if (res::is_rescondor(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<res::rescondor>(x));
    }
    else if (res::is_reseqinf(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<res::reseqinf>(x));
    }
    else if (res::is_reseqninf(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<res::reseqninf>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \\brief Builder class
template <typename Derived>
struct boolean_variable_builder: public add_boolean_variables<core::builder, Derived>
{
};
//--- end generated add_boolean_variables code ---//

} // namespace res

} // namespace mcrl2

#endif // MCRL2_RES_BUILDER_H
