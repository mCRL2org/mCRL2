// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/is_linear.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_IS_LINEAR_H
#define MCRL2_PROCESS_IS_LINEAR_H

#include "mcrl2/process/detail/linear_process_expression_traverser.h"

namespace mcrl2
{

namespace process
{

/// \brief Returns true if the process specification is linear.
/// \param p A process specification
/// \return True if the process specification is linear.
inline
bool is_linear(const process_specification& p, bool verbose = false)
{
  if (p.equations().size() != 1)
  {
    if (verbose)
    {
      std::clog << "warning: the number of equations is not equal to 1" << std::endl;
    }
    return false;
  }
  detail::linear_process_expression_traverser visitor;
  {
    if (!visitor.is_linear(*p.equations().begin(), verbose))
    {
      if (verbose)
      {
        std::clog << "warning: the first equation is not linear" << std::endl;
      }
      return false;
    }
    if (!is_process_instance(p.init()) && !(is_process_instance_assignment(p.init())))
    {
      if (verbose)
      {
        std::clog << "warning: the initial process " << process::pp(p.init()) << " is not a process instance or a process instance assignment" << std::endl;
      }
      return false;
    }
  }
  return true;
}

/// \brief Returns true if the process equation is linear.
/// \param eqn A process equation
inline
bool is_linear(const process_equation& eqn)
{
  detail::linear_process_expression_traverser f;
  return f.is_linear(eqn);
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_IS_LINEAR_H
