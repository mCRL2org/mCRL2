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
      aterm_real()
      {}
      
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

      /// Conversion to ATermReal.
      ///
      operator ATermReal() const
      {
        return reinterpret_cast<ATermReal>(m_term);
      }

      /// Get the real value of the aterm_real.
      ///
      double value() const
      {
        return ATgetReal(reinterpret_cast<ATermReal>(m_term));
      }
  }; 

  inline
  bool operator<(aterm_real x, aterm_real y)
  {
    return ATermReal(x) < ATermReal(y);
  }

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

   template <>
   class aterm_protect_traits<aterm_real>
   {
     public:
       static void protect(aterm_real t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<aterm_real>::protect() " << t << std::endl;
#endif // ATERM_DEBUG_PROTECTION
         t.protect();
       }

       static void unprotect(aterm_real t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<aterm_real>::unprotect() " << t << std::endl;
#endif // ATERM_DEBUG_PROTECTION
         t.unprotect();
       }

       static void mark(aterm_real t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<aterm_real>::mark() " << t << std::endl;
#endif // ATERM_DEBUG_PROTECTION
         t.mark();
       }
   };

} // namespace atermpp

#endif // ATERM_REAL_H
