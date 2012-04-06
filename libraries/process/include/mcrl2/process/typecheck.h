// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/typecheck.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_TYPECHECK_H
#define MCRL2_PROCESS_TYPECHECK_H

#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/detail/pp_deprecated.h"
#include "mcrl2/process/process_specification.h"

namespace mcrl2
{

namespace process
{

/** \brief     Type check a process expression.
 *  Throws an exception if something went wrong.
 *  \param[in] proc_expr A process expression that has not been type checked.
 *  \post      proc_expr is type checked.
 **/
inline
void type_check(process_expression& proc_expr, const process_specification& proc_spec)
{
  // TODO: replace all this nonsense code by a proper type check implementation
  ATermAppl t = core::type_check_proc_expr(proc_expr, process_specification_to_aterm(proc_spec));
  if (t==ATermAppl())
  {
    throw mcrl2::runtime_error("could not type check " + core::pp_deprecated(proc_expr));
  }
  proc_expr = process_expression(t);
}

/** \brief     Type check a parsed mCRL2 process specification.
 *  Throws an exception if something went wrong.
 *  \param[in] proc_spec A process specification  that has not been type checked.
 *  \post      proc_spec is type checked.
 **/

inline
void type_check(process_specification& proc_spec)
{
  // TODO: replace all this nonsense code by a proper type check implementation
  ATermAppl t = process_specification_to_aterm(proc_spec);
  t = core::type_check_proc_spec(t);
  if (t==ATermAppl())
  {
    throw mcrl2::runtime_error("could not type check process specification");
  }
  process_specification result(t);
  proc_spec = result;
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_TYPECHECK_H
