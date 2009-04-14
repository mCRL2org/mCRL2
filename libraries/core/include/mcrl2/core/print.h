// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/print.h
/// \brief Functions for pretty printing ATerms.

#ifndef MCRL2_PRINT_H
#define MCRL2_PRINT_H

#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <ostream>
#include <string>
#include "aterm2.h"
#include "mcrl2/atermpp/atermpp.h"

namespace mcrl2 {
  namespace core {

/// \brief t_pp_format represents the available pretty print formats
typedef enum { ppDefault, ppDebug, ppInternal } t_pp_format;

/** \brief Extensions of the printf function. The following new
 *         conversion formats are supported:
 *         - '%P' for pretty printing ATerm's
 *         - '%T' for normal printing of ATerm's
 *         - '%F' for printing of AFun's
**/
int gsprintf(const char *format, ...);

/** \brief Extensions of the fprintf function. The following new
 *         conversion formats are supported:
 *         - '%P' for pretty printing ATerm's
 *         - '%T' for normal printing of ATerm's
 *         - '%F' for printing of AFun's
**/
int gsfprintf(FILE *stream, const char *format, ...);

/** \brief Extensions of the vfprintf functions. The following new
 *         conversion formats are supported:
 *         - '%P' for the pretty printing ATerm's
 *         - '%T' for the normal printing of ATerm's
 *         - '%F' for the printing of AFun's
**/
int gsvfprintf(FILE *stream, const char *format, va_list args);

/** \brief Print a textual description of an ATerm representation of an
 *         mCRL2 specification or expression to an output stream.
 *  \param[in] out_stream A pointer to a stream to which can be written.
 *  \param[in] part An ATerm representation of a part of an mCRL2
 *             specification or expression.
 *  \param[in] pp_format A pretty print format.
 *  \post A textual representation of part is written to out_stream using
 *        method pp_format.
**/
void PrintPart_C(FILE *out_stream, const ATerm part, t_pp_format pp_format = ppDefault);

/** \brief Print a textual description of an ATerm representation of an
 *         mCRL2 specification or expression to an output stream.
 *  \param[in] out_stream A stream to which can be written.
 *  \param[in] part An ATerm representation of a part of an mCRL2
 *             specification or expression.
 *  \param[in] pp_format A pretty print format.
 *  \post A textual representation of part is written to out_stream using
 *        method pp_format.
**/
void PrintPart_CXX(std::ostream &out_stream, const ATerm part,
  t_pp_format pp_format = ppDefault);

/** \brief Return a textual description of an ATerm representation of an
 *         mCRL2 specification or expression.
 *  \param[in] part An ATerm representation of a part of an mCRL2
 *             specification or expression.
 *  \param[in] pp_format A pretty print format.
 *  \return A textual representation of part according to method pp_format.
**/
std::string PrintPart_CXX(const ATerm part, t_pp_format pp_format = ppDefault);

/** \brief Return a textual description of an ATerm representation of an
 *         mCRL2 specification or expression.
 *  \param[in] part An ATerm representation of a part of an mCRL2
 *             specification or expression.
 *  \param[in] pp_format A pretty print format.
 *  \return A textual representation of part according to method pp_format.
**/
template <typename Term>
std::string pp(Term part, t_pp_format pp_format = ppDefault)
{
  return PrintPart_CXX(atermpp::aterm_traits<Term>::term(part), pp_format);
}

  }
}

#endif //MCRL2_PRINT_H
