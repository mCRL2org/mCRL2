// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/aterm_int.h
// date          : 25-10-2005
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

///////////////////////////////////////////////////////////////////////////////
/// \file aterm_int.h
/// Contains the definition of the aterm_int class.

#ifndef ATERM_INT_H
#define ATERM_INT_H

#include "atermpp/aterm.h"

namespace atermpp
{
  //---------------------------------------------------------//
  //                    aterm_int
  //---------------------------------------------------------//
  class aterm_int: public aterm
  {
    public:
      aterm_int(ATermInt t)
        : aterm(t)
      {}
  
      /// Allow construction from an aterm. The aterm must be of the right type.
      ///
      aterm_int(aterm t)
        : aterm(t)
      {
        assert(type() == AT_INT);
      }

      aterm_int(int value)
        : aterm(ATmakeInt(value))
      {}
      
      /// Get the integer value of the aterm_int.
      ///
      int value() const
      {
        return ATgetInt(reinterpret_cast<ATermInt>(m_term));
      }
  };

  /// INTERNAL ONLY
  inline
  ATerm aterm_ptr(aterm_int& t)
  {
    return t;
  }
  
  /// INTERNAL ONLY
  inline
  ATerm aterm_ptr(const aterm_int& t)
  {
    return t;
  }

} // namespace atermpp

#include "atermpp/aterm_make_match.h"

#endif // ATERM_INT_H
