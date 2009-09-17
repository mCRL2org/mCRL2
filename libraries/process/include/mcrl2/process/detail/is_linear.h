// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/is_linear.h
/// \brief Utility functions for the is_linear check of process specifications.

#ifndef MCRL2_PROCESS_DETAIL_IS_LINEAR_H
#define MCRL2_PROCESS_DETAIL_IS_LINEAR_H

#include <algorithm>
#include "mcrl2/process/process_specification.h"

namespace mcrl2 {

namespace process {

namespace detail {

    /// \brief Returns true if the process instance assignment a matches with the process equation eq.
    /// \param eq A process equation
    /// \param a A process instance assignment
    /// \return True if the process assignment a matches with the process equation eq.
    bool check_process_instance_assignment(const process_equation& eq, const process_instance_assignment& a)
    {
      if (a.identifier() != eq.identifier())
      {
        return false;
      }
      data::assignment_list a1 = a.assignments();
      data::variable_list v = eq.formal_parameters();

      // check if the left hand sides of the assignments exist
      for (data::assignment_list::iterator i = a1.begin(); i != a1.end(); ++i)
      {
        if (std::find(v.begin(), v.end(), i->lhs()) == v.end())
        {
          return false;
        }
      }
      return true;
    }

    /// \brief Returns true if the process instance a matches with the process equation eq.
    /// \param eq A process equation
    /// \param a A process instance
    /// \return True if the process assignment a matches with the process equation eq.
    bool check_process_instance(const process_equation& eq, const process_instance& init)
    {
      if (eq.identifier() != init.identifier())
      {
        return false;
      }
      data::variable_list v = eq.formal_parameters();
      data::data_expression_list e = init.actual_parameters();
      data::variable_list::const_iterator i = v.begin();
      data::data_expression_list::const_iterator j = e.begin();
      for (; i != v.end(); ++i, ++j)
      {
        if (i->sort() != j->sort())
        {
          return false;
        }
      }
      return true;
    }

} // namespace detail

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_IS_LINEAR_H
