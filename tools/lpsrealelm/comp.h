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

/// \brief A local class defining a data type for use in lpsrealelm.
/// \details The defined data type is 
///          sort Comp = struct smaller?is_smaller | equal?is_equal | larger?is_larger;

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
      f_smaller=c_smaller().constructor_function(*this);
      f_equal=c_equal().constructor_function(*this);
      f_larger=c_larger().constructor_function(*this);
      f_is_smaller=c_smaller().recogniser_function(*this);
      f_is_equal=c_equal().recogniser_function(*this);
      f_is_larger=c_larger().recogniser_function(*this);
      // f_is_larger=c_larger().recogniser_function(comp_sort());
    }

    sort_expression sort() const
    { return *this;
    }

    basic_sort basic_sort_name() const
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

