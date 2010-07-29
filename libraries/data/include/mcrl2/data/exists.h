// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/exists.h
/// \brief The class exists.

#ifndef MCRL2_DATA_EXISTS_H
#define MCRL2_DATA_EXISTS_H

#include "mcrl2/data/abstraction.h"
#include "mcrl2/data/variable.h"

namespace mcrl2 {

  namespace data {

    /// \brief existential quantification.
    ///
    class exists: public abstraction
    {
      public:

        /// Constructor.
        ///
        /// \param[in] d A data expression
        /// \pre d has the internal structure of an abstraction.
        /// \pre d is an existential quantification.
        exists(const data_expression& d)
          : abstraction(d)
        {
          assert(is_abstraction(d));
          assert(abstraction(d).binding_operator() == exists_binder());
        }

        /// Constructor.
        ///
        /// \param[in] variables A nonempty list of binding variables (objects of type variable).
        /// \param[in] body The body of the exists abstraction.
        /// \pre variables is not empty.
        template < typename Container >
        exists(const Container& variables,
               const data_expression& body,
               typename atermpp::detail::enable_if_container< Container, variable >::type* = 0)
          : abstraction(exists_binder(), variables, body)
        {
          assert(!variables.empty());
        }

    }; // class exists

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_EXISTS_H

