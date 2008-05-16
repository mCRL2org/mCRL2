// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file regfrmtrans.h

#ifndef MCRL2_REGFRMTRANS_H
#define MCRL2_REGFRMTRANS_H

#include <aterm2.h>

namespace mcrl2 {
  namespace core {
    namespace detail {

//Global preconditions:
//- the ATerm library has been initialised

ATermAppl translate_reg_frms(ATermAppl state_frm);
/*Pre: state_frm represents a state formula that adheres to the internal ATerm
 *     structure after the data implementation phase.
 *Ret: state_frm in which all regular formulas are translated in
 *     terms of state and action formulas
 */
   
    } // namespace detail
  }   // namespace core
}     // namespace mcrl2

#endif // MCRL2_REGFRMTRANS_H
