// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/assignment.h
/// \brief The class assignment.

#ifndef MCRL2_DATA_ASSIGNMENT_H
#define MCRL2_DATA_ASSIGNMENT_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/variable.h"

namespace mcrl2 {
  
  namespace data {

    /// \brief data assignment.
    ///
    class assignment: public atermpp::aterm_appl
    {
      public:

        /// \brief Constructor.
        ///
        assignment()
          : atermpp::aterm_appl(core::detail::constructDataVarIdInit())
        {}

        /// \internal
        assignment(const atermpp::aterm_appl& a)
          : atermpp::aterm_appl(a)
        {}

        /// \brief Constructor
        ///
        /// \param[in] lhs The left hand side of the assignment.
        /// \param[in] rhs The right hand side of the assignment.
        assignment(const variable& lhs, const data_expression& rhs)
          : atermpp::aterm_appl(core::detail::gsMakeDataVarIdInit(lhs, rhs))
        {}

        /// \brief Returns the name of the assignment.
        inline
        variable lhs() const
        {
          return variable(atermpp::arg1(*this));
        }
        
        /// \brief Returns the right hand side of the assignment
        inline
        data_expression rhs() const
        {
          return atermpp::arg2(*this);
        }

    }; // class assignment

    /// \brief list of assignments
    ///
    typedef atermpp::vector<assignment> assignment_list;

  } // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::data::assignment)
/// \endcond

#endif // MCRL2_DATA_ASSIGNMENT_H

