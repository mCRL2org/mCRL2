// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/selection.h
/// \brief Provides selection utility functionality

#ifndef MCRL2_DATA_SELECTION_H
#define MCRL2_DATA_SELECTION_H

#include <algorithm>

#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/detail/dependent_sorts.h"

namespace mcrl2 {

  namespace data {

    class used_data_equation_selector {
      
      private:

        std::set< sort_expression > m_used_sorts;

      public:

        bool operator()(data_equation const& e) const
        {
          if (m_used_sorts.find(e.lhs()) != m_used_sorts.end())
          {
            std::set< sort_expression > used_sorts;

            find_sort_expressions(e, std::inserter(used_sorts, used_sorts.end()));

            std::set< sort_expression > result;

            std::set_intersection(m_used_sorts.begin(), m_used_sorts.end(), used_sorts.begin(), used_sorts.end(),
									 std::inserter(result, result.end()));

            return !result.empty();
          }

          return true;
        }

        template < typename Sequence >
        used_data_equation_selector(data_specification const& specification, Sequence context)
        {
          find_dependent_sorts(specification, context, std::inserter(m_used_sorts, m_used_sorts.end()));
        }
    };

  } // namespace data

} // namespace mcrl2

#endif //MCRL2_DATA_UTILITY_H

