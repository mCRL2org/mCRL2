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
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/detail/dependent_sorts.h"

namespace mcrl2 {

  namespace data {

    class used_data_equation_selector {
      
      private:

        std::set< function_symbol > m_used_symbols;

        template < typename Range >
        void add_symbols(Range const& r)
        {
          m_used_symbols.insert(r.begin(), r.end()); 
        }

      public:

        bool operator()(data_equation const& e) const
        {
          std::set< function_symbol > used_symbols;

          detail::make_find_helper< function_symbol, detail::traverser >(std::inserter(used_symbols, used_symbols.end()))(e.lhs());

          return std::includes(m_used_symbols.begin(), m_used_symbols.end(), used_symbols.begin(), used_symbols.end());
        }

        // temporary measure: use aterm
        used_data_equation_selector(data_specification const& specification, atermpp::aterm_appl const& context, bool add_symbols_for_global_variables = true)
        {
          // Trick, traverse all but the data specification
          for (atermpp::aterm_appl::const_iterator i = ++(++context.begin()); i != context.end(); ++i)
          {
            detail::make_find_helper< function_symbol, detail::traverser >(std::inserter(m_used_symbols, m_used_symbols.end()))(*i);

            if (add_symbols_for_global_variables)
            {
              const std::set< variable > variables = find_free_variables(*i);

              // Compensate for symbols that could be used as part of an instantiation of free variables
              for (std::set< variable >::const_iterator j = variables.begin(); j != variables.end(); ++j)
              {
                add_symbols(specification.constructors(j->sort()));
                add_symbols(specification.mappings(j->sort()));
              }
            }
          }

          std::set< data_equation > equations(boost::copy_range< std::set< data_equation > >(specification.equations()));

          std::map< data_equation, std::set< function_symbol > > symbols_for_equation;

          for (std::set< data_equation >::const_iterator i = equations.begin(); i != equations.end(); ++i)
          {
             std::set< function_symbol > used_symbols;

             detail::make_find_helper< function_symbol, detail::traverser >(std::inserter(used_symbols, used_symbols.end()))(i->lhs());

             symbols_for_equation[*i].swap(used_symbols);
          }

          for (std::set< data_equation >::size_type n = 0, m = equations.size(); n != m; n = m, m = equations.size())
          {
            for (std::set< data_equation >::iterator i = equations.begin(), j = equations.begin(); j++ != equations.end(); i = j)
            {
              if (std::includes(m_used_symbols.begin(), m_used_symbols.end(), symbols_for_equation[*i].begin(), symbols_for_equation[*i].end()))
              {
                detail::make_find_helper< function_symbol, detail::traverser >(std::inserter(m_used_symbols, m_used_symbols.end()))(i->rhs());
                detail::make_find_helper< function_symbol, detail::traverser >(std::inserter(m_used_symbols, m_used_symbols.end()))(i->condition());

                equations.erase(i);
              }
            }
          }
        }
    };

  } // namespace data

} // namespace mcrl2

#endif //MCRL2_DATA_UTILITY_H

