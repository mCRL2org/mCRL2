// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file atermpp/aterm_place_holder.h
/// \brief Add your file description here.

#ifndef MCRL2_ATERMPP_ATERM_PLACE_HOLDER_H
#define MCRL2_ATERMPP_ATERM_PLACE_HOLDER_H

#include "atermpp/aterm.h"

namespace atermpp
{
  //---------------------------------------------------------//
  //                     aterm_place_holder
  //---------------------------------------------------------//
  class aterm_place_holder: public aterm_base
  {
   public:
      aterm_place_holder(ATermPlaceholder t)
        : aterm_base(t)
      {}
  
      aterm_place_holder(ATerm t)
        : aterm_base(ATmakePlaceholder(t))
      {}
  
      /// Build an aterm_place_holder of a specific type. The type is taken from the type
      /// parameter.
      ///
      aterm_place_holder(aterm type)
        : aterm_base(ATmakePlaceholder(type))
      {}
  };
  
} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_PLACE_HOLDER_H
