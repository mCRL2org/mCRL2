// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/detail/aterm_conversion.h
/// \brief Conversion functions for aterms.

#ifndef MCRL2_ATERMPP_DETAIL_ATERM_CONVERSION_H
#define MCRL2_ATERMPP_DETAIL_ATERM_CONVERSION_H

#include "mcrl2/atermpp/aterm.h"

namespace atermpp
{
namespace detail
{

using namespace aterm;

// --- term ---
/// \brief Conversion function
/// \param t A pointer
/// \return The pointer converted to an ATerm
inline
ATerm void2term(void* t)
{
  return reinterpret_cast<_ATerm *>(t);
}

/// \brief Conversion function
/// \param t A pointer
/// \return The pointer converted to an ATerm
inline
ATerm void2term(const void* t)
{
  return reinterpret_cast<_ATerm *>(const_cast<void*>(t));
}

/// \brief Conversion function
/// \param t A term
/// \return The ATerm converted to a void pointer
inline
void* term2void(ATerm t)
{
  return reinterpret_cast<void*>(&*t);
}

// --- list ---
/// \brief Conversion function
/// \param t A pointer
/// \return The pointer converted to an ATermList
inline
ATermList void2list(void* t)
{
  return reinterpret_cast<_ATermList*>(t);
}

/// \brief Conversion function
/// \param t A pointer
/// \return The pointer converted to an ATermList
inline
ATermList void2list(const void* t)
{
  return reinterpret_cast<_ATermList*>(const_cast<void*>(t));
}

/// \brief Conversion function
/// \param l A sequence of terms
/// \return The ATermList converted to a void pointer
inline
void* list2void(ATermList l)
{
  return reinterpret_cast<void*>(&*l);
}

// --- appl ---
/// \brief Conversion function
/// \param t A pointer
/// \return The pointer converted to an ATermAppl
inline
ATermAppl void2appl(void* t)
{
  return reinterpret_cast<_ATermAppl*>(t);
}

/// \brief Conversion function
/// \param t A pointer
/// \return The pointer converted to an ATermAppl
inline
ATermAppl void2appl(const void* t)
{
  return reinterpret_cast<_ATermAppl*>(const_cast<void*>(t));
}

/// \brief Conversion function
/// \param l A term
/// \return The ATermAppl converted to a void pointer
inline
void* appl2void(ATermAppl l)
{
  return reinterpret_cast<void*>(&*l);
}

// --- int  ---
/// \brief Conversion function
/// \param t A pointer
/// \return The pointer converted to an ATermInt
inline
ATermInt void2int(void* t)
{
  return reinterpret_cast<_ATermInt*>(t);
}

/// \brief Conversion function
/// \param t A pointer
/// \return The pointer converted to an ATermInt
inline
ATermInt void2int(const void* t)
{
  return reinterpret_cast<_ATermInt*>(const_cast<void*>(t));
}

/// \brief Conversion function
/// \param l An integer term
/// \return The ATermInt converted to a void pointer
inline
void* int2void(ATermInt l)
{
  return reinterpret_cast<void*>(&*l);
}

} // namespace detail

} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ATERM_CONVERSION_H
