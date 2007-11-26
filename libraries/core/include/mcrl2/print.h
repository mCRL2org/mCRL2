// Author(s): Aad Mathijssen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file print.h

#ifndef MCRL2_PRINT_H
#define MCRL2_PRINT_H

#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include "aterm2.h"

#include "mcrl2/print/types.h"

#ifdef __cplusplus
extern "C" {
#endif

//This file contains C specific printing functions

//Global preconditions:
//- the ATerm library has been initialised
//- gsEnableConstructorFunctions() has been called

void PrintPart_C(FILE *OutStream, const ATerm Part, t_pp_format pp_format);
/*Pre: OutStream points to a stream to which can be written
       Part is an ATerm containing a part of a mCRL2 specification
  Post:A textual (pretty printed) representation of Part is written to
       OutStream using method pp_format
*/

int gsprintf(const char *format, ...);
int gsfprintf(FILE *stream, const char *format, ...);
int gsvfprintf(FILE *stream, const char *format, va_list args);
//Extensions of the printf functions. The following new conversion formats are
//supported:
//- '%P' for the pretty printing ATerm's using the advanced method
//- '%T' for the normal printing of ATerm's 
//- '%F' for the printing of AFun's

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include <ostream>
#include <string>

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
#endif

#endif //MCRL2_PRINT_H
