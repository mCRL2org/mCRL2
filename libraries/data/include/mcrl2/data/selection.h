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

          std::set< data_equation > equations(boost::copy_range< std::set< data_equation > >(specification.equations()));

          for (std::set< data_equation >::size_type n = 0, m = equations.size(); n != m; n = m, m = equations.size())
          {
            for (std::set< data_equation >::iterator i = equations.begin(), j = equations.begin(); j++ != equations.end(); i = j)
            {
              if ((*this)(*i))
              {
                find_dependent_sorts(specification, *i, std::inserter(m_used_sorts, m_used_sorts.end()));

                equations.erase(i);
              }
            }
          }
        }
    };

  } // namespace data

} // namespace mcrl2

#endif //MCRL2_DATA_UTILITY_H

