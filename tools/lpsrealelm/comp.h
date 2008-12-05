// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file comp.h
/// \brief Provides an implementation of sort Comp.

#ifndef MCRL2_LPSRTA_COMP_H
#define MCRL2_LPSRTA_COMP_H

#include <iostream>
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/data/data_operation.h"
#include "mcrl2/data/data_application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/core/detail/data_implementation_concrete.h"

using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;
using namespace mcrl2::data::data_expr;

inline
sort_expression comp()
{
  return sort_identifier("Comp");
}

inline
data_operation smaller()
{
  return data_operation(identifier_string("smaller"), comp());
}

inline
data_operation equal()
{
  return data_operation(identifier_string("equal"), comp());
}

inline
data_operation larger()
{
  return data_operation(identifier_string("larger"), comp());
}

inline
sort_expression comp2bool()
{
  return sort_arrow(make_list(comp()), sort_expr::bool_());
}

inline
data_operation is_smaller()
{
  return data_operation(identifier_string("is_smaller"), comp2bool());
}

inline
data_application is_smaller(const data_expression& e)
{
  return data_application(is_smaller(), make_list(e));
}

inline
data_operation is_equal()
{
  return data_operation(identifier_string("is_equal"), comp2bool());
}

inline
data_application is_equal(const data_expression& e)
{
  return data_application(is_equal(), make_list(e));
}

inline
data_operation is_larger()
{
  return data_operation(identifier_string("is_larger"), comp2bool());
}

inline
data_application is_larger(const data_expression& e)
{
  return data_application(is_larger(), make_list(e));
}

inline
data_specification add_comp_sort(const data_specification& s)
{
  sort_expression_list sorts = s.sorts();
  sorts = push_front(sorts, comp());

  data_operation_list constructors = s.constructors();
  data_operation_list new_constructors;
  new_constructors = push_front(new_constructors, smaller());
  new_constructors = push_front(new_constructors, equal());
  new_constructors = push_front(new_constructors, larger());
  constructors = new_constructors + constructors;

  data_operation_list mappings = s.mappings();
  mappings = push_front(mappings, data_operation(gsMakeOpIdEq(comp())));
  mappings = push_front(mappings, data_operation(gsMakeOpIdNeq(comp())));
  mappings = push_front(mappings, data_operation(gsMakeOpIdIf(comp())));
  data_operation_list new_mappings;
  new_mappings = push_front(new_mappings, is_smaller());
  new_mappings = push_front(new_mappings, is_equal());
  new_mappings = push_front(new_mappings, is_larger());
  mappings = new_mappings + mappings;

  data_equation_list equations = s.equations();
  data_variable x("x", comp());
  data_variable y("y", comp());
  data_variable b("b", sort_expr::bool_());
  data_expression nil(gsMakeNil());
  equations = push_front(equations, data_equation(make_list(x), nil, equal_to(x,x), true_()));
  equations = push_front(equations, data_equation(make_list(x,y), nil, not_equal_to(x,y), not_(equal_to(x,y))));
  equations = push_front(equations, data_equation(make_list(x,y), nil, if_(true_(),x,y),x));
  equations = push_front(equations, data_equation(make_list(x,y), nil, if_(false_(),x,y),y));
  equations = push_front(equations, data_equation(make_list(b,x), nil, if_(b,x,x),x));
  for(data_operation_list::iterator i = new_constructors.begin(); i != new_constructors.end(); ++i)
  {
    for(data_operation_list::iterator j = new_constructors.begin(); j != new_constructors.end(); ++j)
    {
      data_expression right = (*i == *j)?true_():false_();
      equations = push_front(equations, data_equation(data_variable_list(), nil, equal_to(aterm_appl(*i),aterm_appl(*j)), right));
    }
  }

  for(data_operation_list::iterator i = new_mappings.begin(); i != new_mappings.end(); ++i)
  {
    for(data_operation_list::iterator j = new_constructors.begin(); j != new_constructors.end(); ++j)
    {
      data_expression right = (i->name() == j->name())?true_():false_();
      equations = push_front(equations, data_equation(data_variable_list(), nil, data_application(aterm_appl(*i),make_list(aterm_appl(*j))), right));
    }
  }

  return data_specification(sorts, constructors, mappings, equations);
}

#endif //MCRL2_LPSRTA_COMP_H

