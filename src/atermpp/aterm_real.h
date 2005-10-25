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

#ifndef ATERM_REAL_H
#define ATERM_REAL_H

/** @file
  * This is a C++ wrapper around the ATerm library.
  */

#include <cassert>
#include "atermpp/aterm.h"

namespace atermpp
{
  //---------------------------------------------------------//
  //                    aterm_real
  //---------------------------------------------------------//
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
  
      /**
        * Get the real value of the aterm_real.
        **/
      double value() const
      {
        return ATgetReal(reinterpret_cast<ATermReal>(m_term));
      }
  }; 

} // namespace atermpp

#include "atermpp/aterm_make_match.h"

#endif // ATERM_REAL_H
