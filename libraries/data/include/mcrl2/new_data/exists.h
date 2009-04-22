// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/exists.h
/// \brief The class exists.

#ifndef MCRL2_DATA_EXISTS_H
#define MCRL2_DATA_EXISTS_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/new_data/abstraction.h"
#include "mcrl2/new_data/variable.h"

namespace mcrl2 {

  namespace new_data {

    /// \brief existential quantification.
    ///
    class exists: public abstraction
    {
      public:

        /// Constructor.
        ///
        /// \param[in] d A new_data expression
        /// \pre d has the internal structure of an abstraction.
        /// \pre d is an existential quantification.
        exists(const data_expression& d)
          : abstraction(d)
        {
          assert(d.is_abstraction());
          assert(static_cast<abstraction>(d).binding_operator() == "exists");
        }

        /// Constructor.
        ///
        /// \param[in] variables A nonempty list of binding variables.
        /// \param[in] body The body of the exists abstraction.
        /// \pre variables is not empty.
        exists(const variable_list& variables,
               const data_expression& body)
          : abstraction("exists", variables, body)
        {
          assert(!variables.empty());
        }

        /// Constructor.
        ///
        /// \param[in] variables A nonempty list of binding variables.
        /// \param[in] body The body of the exists abstraction.
        /// \pre variables is not empty.
        exists(const variable_vector& variables,
               const data_expression& body)
          : abstraction("exists", make_variable_list(variables), body)
        {
          assert(!variables.empty());
        }

        /// Constructor.
        ///
        /// \param[in] variables A nonempty list of binding variables.
        /// \param[in] body The body of the exists abstraction.
        /// \pre variables is not empty.
        template < typename ForwardTraversalIterator >
        exists(const boost::iterator_range< ForwardTraversalIterator >& variables,
               const data_expression& body)
          : abstraction("exists", make_variable_list(variables), body)
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

    }; // class exists

    /// \brief list of exists
    typedef atermpp::term_list<exists> exists_list;

  } // namespace new_data

} // namespace mcrl2

#endif // MCRL2_DATA_EXISTS_H

