// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/aterm_place_holder.h
// date          : 25-10-2005
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

///////////////////////////////////////////////////////////////////////////////
/// \file aterm_place_holder.h
/// Contains the definition of the aterm_place_holder class.

#ifndef ATERM_PLACE_HOLDER_H
#define ATERM_PLACE_HOLDER_H

#include "atermpp/aterm.h"

namespace atermpp
{
  //---------------------------------------------------------//
  //                     aterm_place_holder
  //---------------------------------------------------------//
  class aterm_place_holder: public aterm
  {
   public:
      aterm_place_holder(ATermPlaceholder t)
        : aterm(t)
      {}
  
      aterm_place_holder(ATerm t)
        : aterm(t)
      {}
  
      /// Build an aterm_place_holder of a specific type. The type is taken from the type
      /// parameter.
      ///
      aterm_place_holder(aterm type)
        : aterm(ATmakePlaceholder(type.to_ATerm()))
      {}
      
      /// Get the type of the aterm_place_holder.
      ///
      aterm type()
      {
        return aterm(ATgetPlaceholder(void2place_holder(m_term)));
      }
  };
  
} // namespace atermpp

#endif // ATERM_PLACE_HOLDER_H
