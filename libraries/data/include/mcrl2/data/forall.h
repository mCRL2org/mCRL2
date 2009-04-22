// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/forall.h
/// \brief The class forall.

#ifndef MCRL2_DATA_FORALL_H
#define MCRL2_DATA_FORALL_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/data/abstraction.h"
#include "mcrl2/data/variable.h"

namespace mcrl2 {

  namespace data {

    /// \brief universal quantification.
    ///
    class forall: public abstraction
    {
      public:

        /// Constructor.
        ///
        /// \param[in] d A data expression.
        /// \pre d has the interal structure of an abstraction.
        /// \pre d is a universal quantification.
        forall(const data_expression& d)
          : abstraction(d)
        {
          assert(d.is_abstraction());
          assert(static_cast<abstraction>(d).binding_operator() == "forall");
        }

        /// Constructor.
        ///
        /// \param[in] variables A nonempty list of binding variables.
        /// \param[in] body The body of the forall abstraction.
        /// \pre variables is not empty.
        forall(const variable_list& variables,
               const data_expression& body)
          : abstraction("forall", variables, body)
        {
          assert(!variables.empty());
        }

        /// Constructor.
        ///
        /// \param[in] variables A nonempty list of binding variables.
        /// \param[in] body The body of the forall abstraction.
        /// \pre variables is not empty.
        forall(const variable_vector& variables,
               const data_expression& body)
          : abstraction("forall", make_variable_list(variables), body)
        {
          assert(!variables.empty());
        }

        /// Constructor.
        ///
        /// \param[in] variables A nonempty list of binding variables.
        /// \param[in] body The body of the forall abstraction.
        /// \pre variables is not empty.
        template < typename ForwardTraversalIterator >
        forall(const boost::iterator_range< ForwardTraversalIterator >& variables,
               const data_expression& body)
          : abstraction("forall", make_variable_list(variables), body)
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

    }; // class forall

    /// \brief list of foralls
    ///
    typedef atermpp::term_list<forall> forall_list;

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_FORALL_H

