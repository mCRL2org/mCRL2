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
          if (e.lhs().is_application())
          {
            return m_used_sorts.find(application(e.lhs()).head().sort()) != m_used_sorts.end();
          }
          else if (e.lhs().is_function_symbol())
          {
            return m_used_sorts.find(function_symbol(e.lhs()).sort()) != m_used_sorts.end();
          }

          return true;
        }

        template < typename Sequence >
        used_data_equation_selector(data_specification const& specification, Sequence context)
        {
          find_dependent_sorts(specification, context, std::inserter(m_used_sorts, m_used_sorts.end()));

          bool changed = true;

          while (changed) {
            changed = false;

            for (data_specification::equations_const_range r(specification.equations()); !r.empty(); r.advance_begin(1))
            {
              if ((*this)(r.front()))
              {
                std::set< sort_expression >::size_type old_size = m_used_sorts.size();

                find_dependent_sorts(specification, r.front(), std::inserter(m_used_sorts, m_used_sorts.end()));

                changed = changed || (m_used_sorts.size() != old_size);
              }
            }
          }
        }
    };

  } // namespace data

} // namespace mcrl2

#endif //MCRL2_DATA_UTILITY_H

