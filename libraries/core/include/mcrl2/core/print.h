// Author(s): Aad Mathijssen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file print.h
/// \brief Library for pretty printing of ATerms

#ifndef MCRL2_PRINT_H
#define MCRL2_PRINT_H

#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include "aterm2.h"

//Type definitions for both pretty pretty printers

///t_pp_format represents the available pretty print formats
typedef enum { ppDefault, ppDebug, ppInternal } t_pp_format;

#ifdef __cplusplus
extern "C" {
#endif

//This file contains C specific printing functions

//Global preconditions:
//- the ATerm library has been initialised
//- gsEnableConstructorFunctions() has been called

/*! Pre: OutStream points to a stream to which can be written
         Part is an ATerm containing a part of a mCRL2 specification
    Post:A textual (pretty printed) representation of Part is written to
         OutStream using method pp_format
*/
void PrintPart_C(FILE *OutStream, const ATerm Part, t_pp_format pp_format);

///Extensions of the printf function. The following new conversion formats are
///supported:
///- '%P' for the pretty printing ATerm's using the advanced method
///- '%T' for the normal printing of ATerm's 
///- '%F' for the printing of AFun's
int gsprintf(const char *format, ...);

///Extensions of the printf function. The following new conversion formats are
///supported:
///- '%P' for the pretty printing ATerm's using the advanced method
///- '%T' for the normal printing of ATerm's 
///- '%F' for the printing of AFun's
int gsfprintf(FILE *stream, const char *format, ...);

///Extensions of the vfprintf functions. The following new conversion formats are
///supported:
///- '%P' for the pretty printing ATerm's using the advanced method
///- '%T' for the normal printing of ATerm's 
///- '%F' for the printing of AFun's
int gsvfprintf(FILE *stream, const char *format, va_list args);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include <ostream>
#include <string>
#include "mcrl2/atermpp/atermpp.h"

//Global preconditions:
//- the ATerm library has been initialised
//- gsEnableConstructorFunctions() has been called

/*! Pre: OutStream points to a stream to which can be written
         Part is an ATerm containing a part of a mCRL2 specification or state
         formula
    Post:A textual representation of Part is written to OutStream using method
         pp_format
*/
void PrintPart_CXX(std::ostream &OutStream, const ATerm Part,
  t_pp_format pp_format);

/*! Pre: Part is an ATerm containing a part of a mCRL2 specification or state
         formula
    Ret: A textual representation of Part pretty printed using method pp_format
*/
std::string PrintPart_CXX(const ATerm Part, t_pp_format pp_format);

inline
std::string pretty_print(ATerm t)
{
  return PrintPart_CXX(t, ppDefault);
}

template <typename Term>
std::string pp(Term t)
{
  return pretty_print(atermpp::aterm_traits<Term>::term(t));
}

#endif

#endif //MCRL2_PRINT_H
