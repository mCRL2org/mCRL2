// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file atermpp/detail/aterm_conversion.h
/// \brief Add your file description here.

#ifndef MCRL2_ATERMPP_DETAIL_ATERM_CONVERSION_H
#define MCRL2_ATERMPP_DETAIL_ATERM_CONVERSION_H

namespace { // unnamed namespace

  // --- term ---
  inline
  ATerm void2term(void* t)
  { return reinterpret_cast<ATerm>(t); }

  inline
  const ATerm void2term(const void* t)
  { return reinterpret_cast<ATerm>(const_cast<void*>(t)); }
  
  inline
  void* term2void(ATerm t)
  { return reinterpret_cast<void*>(t); }
  
  // --- list ---
  inline
  ATermList void2list(void* t)
  { return reinterpret_cast<ATermList>(t); }

  inline
  const ATermList void2list(const void* t)
  { return reinterpret_cast<ATermList>(const_cast<void*>(t)); }
  
  inline
  void* list2void(ATermList l)
  { return reinterpret_cast<void*>(l); }
  
  // --- appl ---
  inline
  ATermAppl void2appl(void* t)
  { return reinterpret_cast<ATermAppl>(t); }
  
  inline
  const ATermAppl void2appl(const void* t)
  { return reinterpret_cast<ATermAppl>(const_cast<void*>(t)); }
  
  inline
  void* appl2void(ATermAppl l)
  { return reinterpret_cast<void*>(l); }
  
  // --- real ---
  inline
  ATermReal void2real(void* t)
  { return reinterpret_cast<ATermReal>(t); }
  
  inline
  const ATermReal void2real(const void* t)
  { return reinterpret_cast<ATermReal>(const_cast<void*>(t)); }
  
  inline
  void* real2void(ATermReal l)
  { return reinterpret_cast<void*>(l); }
  
  // --- blob ---
  inline
  ATermBlob void2blob(void* t)
  { return reinterpret_cast<ATermBlob>(t); }
  
  inline
  const ATermBlob void2blob(const void* t)
  { return reinterpret_cast<ATermBlob>(const_cast<void*>(t)); }
  
  inline
  void* blob2void(ATermBlob l)
  { return reinterpret_cast<void*>(l); }
  
  // --- int  ---
  inline
  ATermInt void2int(void* t)
  { return reinterpret_cast<ATermInt>(t); }
  
  inline
  const ATermInt void2int(const void* t)
  { return reinterpret_cast<ATermInt>(const_cast<void*>(t)); }
  
  inline
  void* int2void(ATermInt l)
  { return reinterpret_cast<void*>(l); }

  // --- place_holder  ---
  inline
  ATermPlaceholder void2place_holder(void* t)
  { return reinterpret_cast<ATermPlaceholder>(t); }

} // unnamed namespace

#endif // MCRL2_ATERMPP_DETAIL_ATERM_CONVERSION_H
