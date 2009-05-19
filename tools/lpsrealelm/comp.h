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
// #include "mcrl2/data/data_operation.h"
// #include "mcrl2/data/data_application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/structured_sort.h"

// using namespace atermpp;
using namespace mcrl2;
// using namespace mcrl2::core;
// using namespace mcrl2::core::detail;
using namespace mcrl2::data;
// using namespace mcrl2::data::data_expr;
using namespace mcrl2::data::detail;

/* 
/// \brief Name for the sort Comp.
/// Comp is used as sort indicating relative order in lpsrealelm.
inline identifier_string comp_name()
{
  return identifier_string("Comp");
}

/// \brief Name for the operator smaller on sort Comp.
inline
identifier_string smaller_name()
{
  return identifier_string("smaller");
}

/// \brief Name for the operator equal on sort Comp.
inline
identifier_string equal_name()
{
  return identifier_string("equal");
}

/// \brief Name for the operator larger on sort Comp.
inline
identifier_string larger_name()
{
  return identifier_string("larger");
}

/// \brief Name for the operator is_smaller on sort Comp.
inline
identifier_string is_smaller_name()
{
  return identifier_string("is_smaller");
}

/// \brief Name of the operator is_equal on sort Comp.
inline
identifier_string is_equal_name()
{
  return identifier_string("is_equal");
}

/// \brief Name of the operator is_larger on sort Comp.
inline
identifier_string is_larger_name()
{
  return identifier_string("is_larger");
} */

/// \brief Sort expression for sort Comp.
/* inline
basic_sort comp()
{
  return basic_sort("Comp");
}

/// \brief Constructor smaller for sort Comp.
inline
function_symbol smaller()
{
  return function_symbol("smaller",comp());
}

/// \brief Constructor equal for sort Comp.
inline
function_symbol equal()
{
  return function_symbol("equal", comp());
}

/// \brief Constructor larger for sort Comp.
inline
function_symbol larger()
{
  return function_symbol("larger", comp());
}

/// \brief Sort expression Comp -> Bool
inline
sort_expression comp2bool()
{
  return sort_arrow(make_list(comp()), sort_expr::bool_());
}

/// \brief Operation is_smaller for sort Comp.
inline
data_operation is_smaller()
{
  return data_operation(is_smaller_name(), comp2bool());
}

/// \brief Operation is_equal for sort Comp.
inline
data_operation is_equal()
{
  return data_operation(is_equal_name(), comp2bool());
}

/// \brief Operation is_larger for sort Comp.
inline
data_operation is_larger()
{
  return data_operation(is_larger_name(), comp2bool());
}

/// \brief Application of is_smaller on a data expression e.
inline
data_application is_smaller(const data_expression& e)
{
  return data_application(is_smaller(), make_list(e));
}

/// \brief Application of is_equal on a data expression e.
inline
data_application is_equal(const data_expression& e)
{
  return data_application(is_equal(), make_list(e));
}

/// \brief Application of is_larger on a data_expression e.
inline
data_application is_larger(const data_expression& e)
{
  return data_application(is_larger(), make_list(e));
}

/// \brief Add declarations for sort Comp to data specification s
/// \param s A data specification.
/// \ret s to which declarations for sort Comp have been added.
inline
structured_sort add_comp_sort(data_specification& s)
{
  // Constructors
  structured_sort_constructor comp_smaller("smaller","is_smaller");
  structured_sort_constructor comp_equal("equal","is_equal");
  structured_sort_constructor comp_larger("larger","is_larger");
  structured_sort_constructor_vector comp_constructors = 
             make_vector(comp_smaller, comp_equal, comp_larger);

  // Build up structured sort
  structured_sort comp_struct(comp_constructors);

  basic_sort comp("Comp");
  s.add_sort(alias(comp,comp_struct));
  return comp_struct;
} */

class comp_struct:public structured_sort
{
  private:
    function_symbol f_smaller;
    function_symbol f_equal;
    function_symbol f_larger;
    function_symbol f_is_smaller;
    function_symbol f_is_equal;
    function_symbol f_is_larger;

    static structured_sort_constructor &c_smaller()
    { static structured_sort_constructor c_smaller("smaller","is_smaller");
      return c_smaller;
    }

    static structured_sort_constructor &c_equal()
    { static structured_sort_constructor c_equal("equal","is_equal");
      return c_equal;
    }

    static structured_sort_constructor &c_larger()
    { static structured_sort_constructor c_larger("larger","is_larger");
      return c_larger;
    }

    static basic_sort &comp_sort()
    { static basic_sort comp_sort("Comp");
      return comp_sort;
    }

  public:
    comp_struct():
           structured_sort(make_vector(c_smaller(),c_equal(),c_larger()))
    { c_smaller().protect(); 
      c_equal().protect();
      c_larger().protect();
      comp_sort().protect();
      f_smaller=c_smaller().constructor_function(comp_sort());
      f_equal=c_equal().constructor_function(comp_sort());
      f_larger=c_larger().constructor_function(comp_sort());
      f_is_smaller=c_smaller().recogniser_function(comp_sort());
      f_is_equal=c_equal().recogniser_function(comp_sort());
      f_is_larger=c_larger().recogniser_function(comp_sort());
    }

    basic_sort sort() const
    { return comp_sort();
    }

    data_expression smaller() const
    { return f_smaller;
    }

    data_expression equal() const
    { return f_equal;
    }

    data_expression larger() const
    { return f_larger;
    }

    data_expression is_smaller(const data_expression& e) const
    { return f_is_smaller(e);
    }

    data_expression is_equal(const data_expression& e) const
    { return f_is_equal(e);
    }

    data_expression is_larger(const data_expression& e) const
    { return f_is_larger(e);
    }
};

#endif //MCRL2_LPSRTA_COMP_H

