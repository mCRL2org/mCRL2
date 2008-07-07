// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/bag_comprehension.h
/// \brief The class bag_comprehension.

#ifndef MCRL2_DATA_BAG_COMPREHENSION_H
#define MCRL2_DATA_BAG_COMPREHENSION_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/data/abstraction.h"
#include "mcrl2/data/variable.h"

namespace mcrl2 {
  
  namespace data {

    /// \brief function symbol.
    ///
    class bag_comprehension: public abstraction
    {
      protected:
        variable_list m_variables; ///< The list of variables that is abstracted.

      public:

        /// Constructor.
        ///
        /// \param[in] d A data expression.
        /// \pre d has the internal structure of an abstraction.
        /// \pre d is a bag comprehension.
        bag_comprehension(const data_expression& d)
          : abstraction(d),
            m_variables(atermpp::term_list<data_expression>(atermpp::list_arg2(d)).begin(), atermpp::term_list<data_expression>(atermpp::list_arg2(d)).end())
        {
          assert(d.is_abstraction());
          assert(static_cast<abstraction>(d).is_bag_comprehension());
        }

        /// Constructor.
        ///
        /// \param[in] variables A nonempty list of binding variables.
        /// \param[in] body The body of the bag_comprehension abstraction.
        bag_comprehension(const boost::iterator_range<variable_list::const_iterator>& variables,
                          const data_expression& body)
          : abstraction("bagcomprehension", variables, body),
            m_variables(variables.begin(), variables.end())
        {
          assert(!variables.empty());
        }

        /*  Should be enabled when the implementation in data_expression is
         * removed
        /// \overload
        inline
        sort_expression sort() const
        {
          return function_sort(sorts_of_data_expressions(boost::make_iterator_range(m_variables.begin(), m_variables.end())), body().sort());
        }
        */

    }; // class bag_comprehension

    /// \brief list of bag_comprehension
    ///
    typedef atermpp::vector<bag_comprehension> bag_comprehension_list;

  } // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::data::bag_comprehension)
/// \endcond

#endif // MCRL2_DATA_BAG_COMPREHENSION_H

