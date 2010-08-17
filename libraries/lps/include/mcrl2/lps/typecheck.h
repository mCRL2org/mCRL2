// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/typecheck.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_TYPECHECK_H
#define MCRL2_LPS_TYPECHECK_H

#include "mcrl2/core/detail/struct_core.h"  // gsMakeMultAct
#include "mcrl2/core/typecheck.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace lps {

    /** \brief     Type check a multi action
     *  Throws an exception if something went wrong.
     *  \param[in] mult_act A multi action that has not been type checked.
     *  \post      mult_action is type checked and sorts have been added when necessary.
     **/
   inline
    void type_check(multi_action& mult_act, const specification& lps)
    {
      // TODO: replace all this nonsense code by a proper type check implementation
      ATermAppl t = core::type_check_mult_act(core::detail::gsMakeMultAct(mult_act.actions()), specification_to_aterm(lps));
      if (!t)
      {
        throw mcrl2::runtime_error("could not type check multi action " + core::pp(core::detail::gsMakeMultAct(mult_act.actions())));
      }
      mult_act = multi_action(t);
    }

  
    /** \brief     Type check a multi action
     *  Throws an exception if something went wrong.
     *  \param[in] mult_act A multi action that has not been type checked.
     *  \post      mult_action is type checked and sorts have been added when necessary.
     **/
   inline
    void type_check(atermpp::vector<multi_action>& mult_actions, const specification& lps)
    {
      // TODO: replace all this nonsense code by a proper type check implementation
      core::type_check_mult_actions(mult_actions, specification_to_aterm(lps));
    }

} // namespace lps

} // naoespace mcrl2

#endif // MCRL2_LPS_TYPECHECK_H
