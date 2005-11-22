// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/aterm_real.h
// date          : 25-10-2005
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

///////////////////////////////////////////////////////////////////////////////
/// \file aterm_real.h
/// Contains the definition of the aterm_real class.

#ifndef ATERM_REAL_H
#define ATERM_REAL_H

#include <cassert>
#include "atermpp/aterm.h"

namespace atermpp
{
  ///////////////////////////////////////////////////////////////////////////////
  // aterm_real
  /// \brief Represents an term containing a real value.
  ///
  class aterm_real: public aterm
  {
    public:
      aterm_real(double value)
        : aterm(ATmakeReal(value))
      {}
      
      aterm_real(ATermReal t)
        : aterm(t)
      {}
  
      aterm_real(ATerm t)
        : aterm(t)
      {
        assert(type() == AT_REAL);
      }
  
      /// Allow construction from an aterm. The aterm must be of the right type.
      ///
      aterm_real(aterm t)
        : aterm(t)
      {
        assert(type() == AT_REAL);
      }

      /// Get the real value of the aterm_real.
      ///
      double value() const
      {
        return ATgetReal(reinterpret_cast<ATermReal>(m_term));
      }
  }; 

  /// INTERNAL ONLY
  inline
  ATerm aterm_ptr(aterm_real& t)
  {
    return t;
  }
  
  /// INTERNAL ONLY
  inline
  ATerm aterm_ptr(const aterm_real& t)
  {
    return t;
  }

} // namespace atermpp

#endif // ATERM_REAL_H
