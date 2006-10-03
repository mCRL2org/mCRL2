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
      aterm_int()
      {} 

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

      /// Conversion to ATermInt.
      ///
      operator ATermInt() const
      {
        return reinterpret_cast<ATermInt>(m_term);
      }

      /// Get the integer value of the aterm_int.
      ///
      int value() const
      {
        return ATgetInt(reinterpret_cast<ATermInt>(m_term));
      }
  };

  inline
  bool operator<(aterm_int x, aterm_int y)
  {
    return ATermInt(x) < ATermInt(y);
  }

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

   template <>
   class aterm_protect_traits<aterm_int>
   {
     public:
       static void protect(aterm_int t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<aterm_int>::protect() " << t << std::endl;
#endif // ATERM_DEBUG_PROTECTION
         t.protect();
       }

       static void unprotect(aterm_int t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<aterm_int>::unprotect() " << t << std::endl;
#endif // ATERM_DEBUG_PROTECTION
         t.unprotect();
       }

       static void mark(aterm_int t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<aterm_int>::mark() " << t << std::endl;
#endif // ATERM_DEBUG_PROTECTION
         t.mark();
       }
   };

} // namespace atermpp

#include "atermpp/aterm_make_match.h"

#endif // ATERM_INT_H
