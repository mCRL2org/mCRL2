#ifndef __LIBPRINT_CXX_H
#define __LIBPRINT_CXX_H

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

#endif //__LIBPRINT_CXX_H
