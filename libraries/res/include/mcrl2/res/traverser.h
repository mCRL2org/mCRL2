// Author(s): Jan Friso Groote. Based on bes/traverser.h by Jeroen van der Wulp, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/res/traverser.h
/// \brief A traverser framework for the res class.

// To avoid circular inclusion problems
#ifndef MCRL2_RES_RES_EQUATION_SYSTEM_H
#include "mcrl2/res/res_equation_system.h"
#endif

#ifndef MCRL2_RES_TRAVERSER_H
#define MCRL2_RES_TRAVERSER_H

namespace mcrl2
{

namespace res
{

//--- start generated add_traverser_res_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_res_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const res::res_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::res_equation_system& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.equations());
    static_cast<Derived&>(*this).apply(x.initial_state());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::res_variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::minus& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::plus& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::multiply& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::rescondand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arg1());
    static_cast<Derived&>(*this).apply(x.arg2());
    static_cast<Derived&>(*this).apply(x.arg3());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::rescondor& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arg1());
    static_cast<Derived&>(*this).apply(x.arg2());
    static_cast<Derived&>(*this).apply(x.arg3());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::reseqinf& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::reseqninf& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::res_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (res::is_true(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::true_>(x));
    }
    else if (res::is_false(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::false_>(x));
    }
    else if (res::is_res_variable(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::res_variable>(x));
    }
    else if (res::is_minus(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::minus>(x));
    }
    else if (res::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::and_>(x));
    }
    else if (res::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::or_>(x));
    }
    else if (res::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::imp>(x));
    }
    else if (res::is_plus(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::plus>(x));
    }
    else if (res::is_multiply(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::multiply>(x));
    }
    else if (res::is_rescondand(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::rescondand>(x));
    }
    else if (res::is_rescondor(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::rescondor>(x));
    }
    else if (res::is_reseqinf(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::reseqinf>(x));
    }
    else if (res::is_reseqninf(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::reseqninf>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \\brief Traverser class
template <typename Derived>
struct res_expression_traverser: public add_traverser_res_expressions<core::traverser, Derived>
{
};
//--- end generated add_traverser_res_expressions code ---//

//--- start generated add_traverser_res_variables code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_res_variables: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const res::res_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variable());
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::res_equation_system& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.equations());
    static_cast<Derived&>(*this).apply(x.initial_state());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::res_variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::minus& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::plus& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::multiply& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::rescondand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arg1());
    static_cast<Derived&>(*this).apply(x.arg2());
    static_cast<Derived&>(*this).apply(x.arg3());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::rescondor& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arg1());
    static_cast<Derived&>(*this).apply(x.arg2());
    static_cast<Derived&>(*this).apply(x.arg3());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::reseqinf& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::reseqninf& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const res::res_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (res::is_true(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::true_>(x));
    }
    else if (res::is_false(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::false_>(x));
    }
    else if (res::is_res_variable(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::res_variable>(x));
    }
    else if (res::is_minus(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::minus>(x));
    }
    else if (res::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::and_>(x));
    }
    else if (res::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::or_>(x));
    }
    else if (res::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::imp>(x));
    }
    else if (res::is_plus(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::plus>(x));
    }
    else if (res::is_multiply(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::multiply>(x));
    }
    else if (res::is_rescondand(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::rescondand>(x));
    }
    else if (res::is_rescondor(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::rescondor>(x));
    }
    else if (res::is_reseqinf(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::reseqinf>(x));
    }
    else if (res::is_reseqninf(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<res::reseqninf>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \\brief Traverser class
template <typename Derived>
struct res_variable_traverser: public add_traverser_res_variables<core::traverser, Derived>
{
};
//--- end generated add_traverser_res_variables code ---//

//--- start generated add_traverser_identifier_strings code ---//
//--- end generated add_traverser_identifier_strings code ---//

} // namespace res

} // namespace mcrl2

#endif // MCRL2_RES_TRAVERSER_H
