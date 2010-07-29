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

    /** \brief Component for selecting a subset of equations that are actually used in an encompassing specification
     *
     * This component can be used with the constructor of data::basic_rewriter
     * derived classes to select a smaller set of equations that are used as
     * rewrite rules. This limited set of rewrite rules should be enough for the
     * purpose of rewriting objects that occur in the encompassing
     * specification (the context).
     *
     * \note Use of this component can have a dramatic effect of rewriter
     * initialisation time and overall performance.
     **/
    class used_data_equation_selector 
    {
      
      private:

        std::set< function_symbol > m_used_symbols;

        template < typename Range >
        void add_symbols(Range const& r)
        {
          m_used_symbols.insert(r.begin(), r.end()); 
        }

      protected:
        void add_data_specification_symbols(const data_specification& specification)
        {
          // Add all constructors of all sorts as they may be used when enumerating over these sorts
          atermpp::set< sort_expression > sorts(boost::copy_range< atermpp::set< sort_expression > >(specification.sorts()));
          for (atermpp::set< sort_expression>::const_iterator j = sorts.begin(); j != sorts.end(); ++j)
          { add_symbols(specification.constructors(*j));
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

      public:

        bool operator()(data_equation const& e) const
        {
          // return true;
          // Disabled because of too agressive removal of equations,
          // causing the statespace of 1394-fin to exists of a single state
          // and no transitions. (JK & JFG, 7/10/2009)
          // Potentially repaired by also adding the constructors of all sorts into m_used_symbols (JFG 7/10/2009)
          // It turns out that this does not work properly, due to the fact that
          // linear specifications are not properly timed normalized, causing
          // identical functions to have different types. Incomplete type normalisation
          // can cause many other problems also.
          
          std::set< function_symbol > used_symbols;

          detail::make_find_helper< function_symbol, detail::traverser >(std::inserter(used_symbols, used_symbols.end()))(e.lhs());

          /* for(std::set< function_symbol >::const_iterator i=used_symbols.begin();
                 i!=used_symbols.end(); ++i)
          { std::cerr  << "Used symbol in equation " << *i << "\n";
          }

          for(std::set< function_symbol >::const_iterator i=m_used_symbols.begin();
                 i!=m_used_symbols.end(); ++i)
          { std::cerr  << "Used symbol in context " << *i << "\n";
          } */

          return std::includes(m_used_symbols.begin(), m_used_symbols.end(), used_symbols.begin(), used_symbols.end());
          /* bool result=std::includes(m_used_symbols.begin(), m_used_symbols.end(), used_symbols.begin(), used_symbols.end());
          if (result) 
          { std::cerr << "True +++++++++++++++++++++++++++++++++++++++\n";
          }
          else
          { std::cerr << "False +++++++++++++++++++++++++++++++++++++++\n";
          }
          return result;  */
        }

        /// \brief context is a range of function symbols
        template <typename Range>
        used_data_equation_selector(data_specification const& data_spec, Range const& context)
        {
          add_symbols(context);
          add_data_specification_symbols(data_spec);
        }

        // temporary measure: use aterm
        /// \deprecated
        used_data_equation_selector(data_specification const& specification, atermpp::aterm_appl const& context, bool add_symbols_for_global_variables = true)
        {
          atermpp::aterm_appl::const_iterator start = ++context.begin();

          if (core::detail::gsIsLinProcSpec(context))
          {
            ++start;
          }

          if (add_symbols_for_global_variables)
          {
            variable_list variables(atermpp::aterm_appl(*start)(0));

            // Compensate for symbols that could be used as part of an instantiation of free variables
            for (variable_list::const_iterator j = variables.begin(); j != variables.end(); ++j)
            {
              add_symbols(specification.constructors(j->sort()));
              add_symbols(specification.mappings(j->sort()));
            }
          }

          // Trick, traverse all but the data specification
          for (atermpp::aterm_appl::const_iterator i = ++start; i != context.end(); ++i)
          {
            detail::make_find_helper< function_symbol, detail::traverser >
                   (std::inserter(m_used_symbols, m_used_symbols.end()))(*i);
          }

          add_data_specification_symbols(specification);

          /* for(std::set < function_symbol > :: const_iterator i=m_used_symbols.begin();
                   i!=m_used_symbols.end(); ++i)
          { std::cerr << "SYMBOL USED: " << *i << "\n";
          } */
        }
    };

  } // namespace data

} // namespace mcrl2

#endif //MCRL2_DATA_UTILITY_H

