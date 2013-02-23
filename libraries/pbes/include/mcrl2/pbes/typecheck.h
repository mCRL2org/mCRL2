// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/typecheck.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_TYPECHECK_H
#define MCRL2_PBES_TYPECHECK_H

#include "mcrl2/core/typecheck.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2
{

namespace pbes_system
{

/** \brief     Type check a parsed mCRL2 pbes specification.
 *  Throws an exception if something went wrong.
 *  \param[in] pbes_spec A process specification  that has not been type checked.
 *  \post      pbes_spec is type checked.
 **/

template <typename Container>
void type_check(pbes<Container>& pbes_spec)
{
  // TODO: replace all this nonsense code by a proper type check implementation
  atermpp::aterm_appl t1 = pbes_to_aterm(pbes_spec);
  atermpp::aterm_appl t2 = core::type_check_pbes_spec(t1);
  if (t2==atermpp::aterm_appl())
  {
    throw mcrl2::runtime_error("could not type check " + pp(pbes_to_aterm(pbes_spec)));
  }
  pbes_spec = pbes<Container>(t2, true);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TYPECHECK_H
