// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/where_clause.h
/// \brief The class where_clause.

#ifndef MCRL2_DATA_WHERE_CLAUSE_H
#define MCRL2_DATA_WHERE_CLAUSE_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/new_data/data_expression.h"
#include "mcrl2/new_data/assignment.h"

namespace mcrl2 {

  namespace new_data {

    /// \brief function symbol.
    ///
    class where_clause: public data_expression
    {
      public:

        /// \brief Iterator range over list of declarations
        typedef boost::iterator_range< atermpp::term_list< assignment >::iterator >       declarations_range;

        /// \brief Iterator range over constant list of declarations
        typedef boost::iterator_range< atermpp::term_list< assignment >::const_iterator > declarations_const_range;

      public:

        /// Constructor.
        ///
        where_clause()
          : data_expression(core::detail::constructWhr())
        {}

        /// Constructor.
        ///
        /// \param[in] d A new_data expression
        /// \pre d has the internal structure of a where clause.
        where_clause(const data_expression& d)
          : data_expression(d)
        {
          assert(core::detail::gsIsWhr(d));
        }

        /// Constructor.
        ///
        /// \param[in] body The body of the where_clause.
        /// \param[in] declarations The variable declarations of the where
        ///            clause.
        template < typename ForwardTraversalIterator >
        where_clause(const data_expression& body,
                     const boost::iterator_range< ForwardTraversalIterator >& declarations)
          : data_expression(core::detail::gsMakeWhr(body, atermpp::term_list<assignment>(declarations.begin(), declarations.end())))
        {
          assert(!declarations.empty());
        }

        /// Constructor.
        ///
        /// \param[in] body The body of the where_clause.
        /// \param[in] declarations The variable declarations of the where
        ///            clause.
        where_clause(const data_expression& body,
                     const assignment_vector& declarations)
          : data_expression(core::detail::gsMakeWhr(body, atermpp::term_list<assignment>(declarations.begin(), declarations.end())))
        {
          assert(!declarations.empty());
        }

        /// Constructor.
        ///
        /// \param[in] body The body of the where_clause.
        /// \param[in] declarations The variable declarations of the where
        ///            clause.
        where_clause(const data_expression& body,
                     const assignment_list& declarations)
          : data_expression(core::detail::gsMakeWhr(body, declarations))
        {
          assert(!declarations.empty());
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

        /// \brief Returns the declarations of the where_clause
        inline
        declarations_const_range declarations() const
        {
          return boost::make_iterator_range(atermpp::term_list<assignment>(atermpp::list_arg2(*this)));
        }

        /// \brief Returns the body of the where_clause
        inline
        data_expression body() const
        {
          return atermpp::arg1(*this);
        }

    }; // class where_clause

    /// \brief list of where_clauses
    ///
    typedef atermpp::vector<where_clause> where_clause_vector;

  } // namespace new_data

} // namespace mcrl2

#endif // MCRL2_DATA_WHERE_CLAUSE_H

