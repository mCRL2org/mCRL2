// Author(s): Aad Mathijssen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file libprint_c.h

#ifndef _LIBPRINT_C_H
#define _LIBPRINT_C_H

#include <stdio.h>
#include <aterm2.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include "libprint_types.h"

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

#endif
