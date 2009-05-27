// Author(s): Jeroen Keiren, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/sort_utility.h
/// \brief Utility functions for sorts.

#ifndef MCRL2_DATA_SORT_UTILITY_H
#define MCRL2_DATA_SORT_UTILITY_H

#include <vector>
#include <cassert>
#include "mcrl2/data/data.h"
#include "mcrl2/data/data_specification.h"

#include "mcrl2/core/deprecation.h"

namespace mcrl2 {

  namespace data {
    namespace detail {

      DEPRECATED(data::data_expression_vector enumerate_constructors(data::data_specification const& d, data::sort_expression s));

      /// Undocumented function.
      /// \deprecated
      inline
      data::data_expression_vector enumerate_constructors(data::data_specification const& d, data::sort_expression s)
      {
        // All datasorts which are taken into account must be finite. Normally this is the case, because a check on finiteness is done in create_bes
        assert(d.is_certainly_finite(s));
        // The resulting data::data_expression_list.
        data_expression_vector ces;
        // For each constructor of sort s...
        for (data_specification::constructors_const_range r(d.constructors(s)); !r.empty(); r.advance_begin(1))
        {
          if (r.front().sort().is_function_sort())
          {
            // Result list
            data::data_expression_vector result;
            // At first put function f in result
            result.push_back(data::data_expression(r.front()));

            // For each sort of the constructor...
            for (function_sort::domain_const_range j(function_sort(r.front().sort()).domain()); !j.empty(); j.advance_begin(1))
            {
              // Put all values which the sort can have in a list
              data::data_expression_vector constructor_terms(enumerate_constructors(d, j.front()));

              //*i is a list of constructor expressions that should be applied to the elements of result
              data::data_expression_vector temporary;

              for (data::data_expression_vector::const_iterator k = constructor_terms.begin(); k != constructor_terms.end(); ++k)
              //*k is a constructor expression that should be applied to the elements of result
              {
                for (data::data_expression_vector::iterator l = result.begin(); l != result.end(); ++l)
                {
                  //  apply *l to *k
                  //  One of the constructor values is applied to f
                  temporary.push_back(data::application(*l, *k));
                }
              }
              // Next iteration replace all values which are created in tmp
              result.swap(temporary);
            }
            //concatenate ces and result
            ces.insert(ces.end(), result.begin(), result.end());
          }
          else {
            ces.push_back(r.front());
          }
        }
        // Put ces in the correct order
        return ces;
      }
   }

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SORT_UTILITY_H
