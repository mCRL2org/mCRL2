// Author(s): Aad Mathijssen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file libprint.h

#ifndef MCRL2_LIBPRINT_H
#define MCRL2_LIBPRINT_H

#include <ostream>
#include <string>
#include <aterm2.h>
#include "libprint_types.h"

//Global preconditions:
//- the ATerm library has been initialised
//- gsEnableConstructorFunctions() has been called

void PrintPart_CXX(std::ostream &OutStream, const ATerm Part,
  t_pp_format pp_format);
/*Pre: OutStream points to a stream to which can be written
       Part is an ATerm containing a part of a mCRL2 specification or state
       formula
  Post:A textual representation of Part is written to OutStream using method
       pp_format
*/

std::string PrintPart_CXX(const ATerm Part, t_pp_format pp_format);
/*Pre: Part is an ATerm containing a part of a mCRL2 specification or state
       formula
  Ret: A textual representation of Part pretty printed using method pp_format
*/

#endif //MCRL2_LIBPRINT_H
