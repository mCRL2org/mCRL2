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
#include "mcrl2/core/detail/construction_utility.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/structured_sort.h"


/// \brief A local class defining a data type for use in lpsrealelm.
/// \details The defined data type is
///          sort Comp = struct smaller?is_smaller | equal?is_equal | larger?is_larger;

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::data::detail;

class comp_struct:public mcrl2::data::structured_sort
{

  private:
    function_symbol f_smaller;
    function_symbol f_equal;
    function_symbol f_larger;
    function_symbol f_is_smaller;
    function_symbol f_is_equal;
    function_symbol f_is_larger;

    static structured_sort_constructor& c_smaller()
    {
      static structured_sort_constructor c_smaller =
                    core::detail::initialise_static_expression(c_smaller,structured_sort_constructor("smaller", "is_smaller"));
      return c_smaller;
    }

    static structured_sort_constructor& c_equal()
    {
      static structured_sort_constructor c_equal =
                    core::detail::initialise_static_expression(c_equal,structured_sort_constructor("equal", "is_equal"));
      return c_equal;
    }

    static structured_sort_constructor& c_larger()
    {
      static structured_sort_constructor c_larger =
                    core::detail::initialise_static_expression(c_larger,structured_sort_constructor("larger", "is_larger"));
      return c_larger;
    }

    static basic_sort& comp_sort() 
    {
      static basic_sort comp_sort = core::detail::initialise_static_expression(comp_sort,basic_sort("Comp"));
      return comp_sort;
    }


  public:
    comp_struct():
      structured_sort(make_vector(c_smaller(),c_equal(),c_larger()))
    {
      f_smaller=c_smaller().constructor_function(sort());
      f_equal=c_equal().constructor_function(sort());
      f_larger=c_larger().constructor_function(sort());
      f_is_smaller=c_smaller().recogniser_function(sort());
      f_is_equal=c_equal().recogniser_function(sort());
      f_is_larger=c_larger().recogniser_function(sort());
      // f_is_larger=c_larger().recogniser_function(comp_sort());
    }

    ~comp_struct()
    { 
      c_smaller().unprotect();
      c_equal().unprotect();
      c_larger().unprotect();
      comp_sort().unprotect();
    }

    sort_expression sort() const
    {
      // return *this;
      return comp_sort();
    }

    basic_sort basic_sort_name() const
    {
      return comp_sort();
    }

    data_expression smaller() const
    {
      return f_smaller;
    }

    data_expression equal() const
    {
      return f_equal;
    }

    data_expression larger() const
    {
      return f_larger;
    }

    data_expression is_smaller(const data_expression& e) const
    {
      return f_is_smaller(e);
    }

    data_expression is_equal(const data_expression& e) const
    {
      return f_is_equal(e);
    }

    data_expression is_larger(const data_expression& e) const
    {
      return f_is_larger(e);
    }
};

#endif //MCRL2_LPSRTA_COMP_H

