// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/regfrmtrans.h
/// \brief Translate regular formulas in terms of state and action formulas.

#ifndef MCRL2_REGFRMTRANS_H
#define MCRL2_REGFRMTRANS_H

#include "mcrl2/modal_formula/state_formula.h"

namespace mcrl2
{
namespace regular_formulas
{
namespace detail
{

/** \brief     Translate regular formulas in terms of state and action formulas.
 *  \param[in] state_frm An aterm representation of a state formula according
 *             to the internal aterm structure after the data implementation
 *             phase.
 *  \return    state_frm in which all regular formulas are translated in
 *             terms of state and action formulas.
 **/
mcrl2::state_formulas::state_formula translate_reg_frms
          (const mcrl2::state_formulas::state_formula &state_frm);

}   // namespace detail
}   // namespace regular_formulas
}     // namespace mcrl2

#endif // MCRL2_REGFRMTRANS_H
