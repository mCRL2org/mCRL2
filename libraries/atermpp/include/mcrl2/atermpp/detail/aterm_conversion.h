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

#include "aterm2.h"

namespace { // unnamed namespace

  // --- term ---
  /// \brief Conversion function
  /// \param t A pointer
  /// \return The pointer converted to an ATerm
  inline
  ATerm void2term(void* t)
  { return reinterpret_cast<ATerm>(t); }

  /// \brief Conversion function
  /// \param t A pointer
  /// \return The pointer converted to an ATerm
  inline
  ATerm void2term(const void* t)
  { return reinterpret_cast<ATerm>(const_cast<void*>(t)); }
  
  /// \brief Conversion function
  /// \param t A term
  /// \return The ATerm converted to a void pointer
  inline
  void* term2void(ATerm t)
  { return reinterpret_cast<void*>(t); }
  
  // --- list ---
  /// \brief Conversion function
  /// \param t A pointer
  /// \return The pointer converted to an ATermList
  inline
  ATermList void2list(void* t)
  { return reinterpret_cast<ATermList>(t); }

  /// \brief Conversion function
  /// \param t A pointer
  /// \return The pointer converted to an ATermList
  inline
  ATermList void2list(const void* t)
  { return reinterpret_cast<ATermList>(const_cast<void*>(t)); }
  
  /// \brief Conversion function
  /// \param l A sequence of terms
  /// \return The ATermList converted to a void pointer
  inline
  void* list2void(ATermList l)
  { return reinterpret_cast<void*>(l); }
  
  // --- appl ---
  /// \brief Conversion function
  /// \param t A pointer
  /// \return The pointer converted to an ATermAppl
  inline
  ATermAppl void2appl(void* t)
  { return reinterpret_cast<ATermAppl>(t); }
  
  /// \brief Conversion function
  /// \param t A pointer
  /// \return The pointer converted to an ATermAppl
  inline
  ATermAppl void2appl(const void* t)
  { return reinterpret_cast<ATermAppl>(const_cast<void*>(t)); }
  
  /// \brief Conversion function
  /// \param l A term
  /// \return The ATermAppl converted to a void pointer
  inline
  void* appl2void(ATermAppl l)
  { return reinterpret_cast<void*>(l); }
  
  // --- real ---
  /// \brief Conversion function
  /// \param t A pointer
  /// \return The pointer converted to an ATermReal
  inline
  ATermReal void2real(void* t)
  { return reinterpret_cast<ATermReal>(t); }
  
  /// \brief Conversion function
  /// \param t A pointer
  /// \return The pointer converted to an ATermReal
  inline
  ATermReal void2real(const void* t)
  { return reinterpret_cast<ATermReal>(const_cast<void*>(t)); }
  
  /// \brief Conversion function
  /// \param l A real-valued term
  /// \return The ATermReal converted to a void pointer
  inline
  void* real2void(ATermReal l)
  { return reinterpret_cast<void*>(l); }
  
  // --- blob ---
  /// \brief Conversion function
  /// \param t A pointer
  /// \return The pointer converted to an ATermBlob
  inline
  ATermBlob void2blob(void* t)
  { return reinterpret_cast<ATermBlob>(t); }
  
  /// \brief Conversion function
  /// \param t A pointer
  /// \return The pointer converted to an ATermBlob
  inline
  ATermBlob void2blob(const void* t)
  { return reinterpret_cast<ATermBlob>(const_cast<void*>(t)); }
  
  /// \brief Conversion function
  /// \param l A term containing binary data
  /// \return The ATermBlob converted to a void pointer
  inline
  void* blob2void(ATermBlob l)
  { return reinterpret_cast<void*>(l); }
  
  // --- int  ---
  /// \brief Conversion function
  /// \param t A pointer
  /// \return The pointer converted to an ATermInt
  inline
  ATermInt void2int(void* t)
  { return reinterpret_cast<ATermInt>(t); }
  
  /// \brief Conversion function
  /// \param t A pointer
  /// \return The pointer converted to an ATermInt
  inline
  ATermInt void2int(const void* t)
  { return reinterpret_cast<ATermInt>(const_cast<void*>(t)); }
  
  /// \brief Conversion function
  /// \param l An integer term
  /// \return The ATermInt converted to a void pointer
  inline
  void* int2void(ATermInt l)
  { return reinterpret_cast<void*>(l); }

  // --- place_holder  ---
  /// \brief Conversion function
  /// \param t A pointer
  /// \return The pointer converted to an ATermPlaceholder
  inline
  ATermPlaceholder void2place_holder(void* t)
  { return reinterpret_cast<ATermPlaceholder>(t); }

} // unnamed namespace

#endif // MCRL2_ATERMPP_DETAIL_ATERM_CONVERSION_H
