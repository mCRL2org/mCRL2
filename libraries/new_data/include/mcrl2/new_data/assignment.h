// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/assignment.h
/// \brief The class assignment.

#ifndef MCRL2_NEW_DATA_ASSIGNMENT_H
#define MCRL2_NEW_DATA_ASSIGNMENT_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/print.h"
#include "mcrl2/new_data/data_expression.h"
#include "mcrl2/new_data/variable.h"

namespace mcrl2 {

  namespace new_data {

    /// \brief new_data assignment.
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

        /// \brief Returns true if the sorts of the left and right hand side are equal.
        /// \return True if the assignement is well typed.
        bool is_well_typed() const
        {
          bool result = lhs().sort() == rhs().sort();
          if (!result)
          {
            std::clog << "data_assignment::is_well_typed() failed: the left and right hand sides "
               << mcrl2::core::pp(lhs()) << " and " << mcrl2::core::pp(rhs()) << " have different sorts." << std::endl;
            return false;
          }
          return true;
        }

    }; // class assignment

    /// \brief list of assignments
    ///
    typedef atermpp::vector<assignment> assignment_list;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Constructs an assignment_list by pairwise combining a variable and expression
    /// \param lhs A sequence of data variables
    /// \param rhs A sequence of data expressions
    /// \return The corresponding assignment list.
    inline assignment_list make_assignment_list(variable_list const& lhs, data_expression_list const& rhs)
    {
      assert(lhs.size() == rhs.size());
      assignment_list result;
      variable_list::const_iterator i = lhs.begin();
      data_expression_list::const_iterator j = rhs.begin();
      for ( ; i != lhs.end(); ++i, ++j)
      {
        result.push_back(assignment(*i, *j));
      }
      return result;
    }

  } // namespace new_data

} // namespace mcrl2

#endif // MCRL2_NEW_DATA_ASSIGNMENT_H

