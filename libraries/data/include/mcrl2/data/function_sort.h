// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/function_sort.h
/// \brief The class function_sort.

#ifndef MCRL2_DATA_FUNCTION_SORT_H
#define MCRL2_DATA_FUNCTION_SORT_H

#include <iostream>
#include <boost/range/iterator_range.hpp>

#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/detail/convert.h"

namespace mcrl2 {
  
  namespace data {

    /// \brief function sort..
    ///
    class function_sort: public sort_expression
    {
      protected:

        sort_expression_list m_domain;

      public:    

        /// \brief Constructor
        ///
        function_sort()
          : sort_expression(core::detail::constructSortArrow())
        {}

        /// \brief Constructor
        ///
        /// \param[in] domain The domain of the sort.
        /// \param[in] codomain The codomain of the sort.
        function_sort(boost::iterator_range<sort_expression_list::const_iterator> domain, sort_expression codomain)
          : sort_expression(mcrl2::core::detail::gsMakeSortArrow(atermpp::term_list<sort_expression>(domain.begin(), domain.end()), codomain)),
            m_domain (domain.begin(), domain.end())
        {}

        /// \overload
        ///
        inline
        bool is_function_sort() const
        {
          return true;
        }

        /// \brief Returns the domain of the sort.
        ///
        inline
        boost::iterator_range<sort_expression_list::const_iterator> domain() const
        {
          return boost::make_iterator_range(m_domain.begin(), m_domain.end());
        }

        /// \brief Returns the codomain of the sort.
        ///
        inline
        sort_expression codomain() const
        {
          return atermpp::arg2(*this);
        }

    }; // class function_sort

    /// \brief list of function sorts
    ///
    typedef atermpp::vector<function_sort> function_sort_list;

  } // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::data::function_sort)
/// \endcond

#endif // MCRL2_DATA_FUNCTION_SORT_H

