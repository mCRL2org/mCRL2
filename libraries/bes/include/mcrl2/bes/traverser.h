// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/traverser.h
/// \brief add your file description here.

#ifndef MCRL2_BES_TRAVERSER_H
#define MCRL2_BES_TRAVERSER_H

#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/core/traverser.h"

namespace mcrl2 {

namespace bes {

  template <typename Derived>
  class traverser: public core::traverser<Derived>
  {
    public:
      typedef core::traverser<Derived> super;
      using super::operator();

      // TODO: This code should be generated!!
      // Moreover, this if statement can be replaced by an array lookup.
      void operator()(boolean_expression const& x)
      {
        if (is_true(x))
        {
          static_cast<Derived&>(*this)(true_(x));
        }
        else if (is_false(x))
        {
          static_cast<Derived&>(*this)(false_(x));
        }
        else if (is_not(x))
        {
          static_cast<Derived&>(*this)(not_(x));
        }
        else if (is_and(x))
        {
          static_cast<Derived&>(*this)(and_(x));
        }
        else if (is_or(x))
        {
          static_cast<Derived&>(*this)(or_(x));
        }
        else if (is_imp(x))
        {
          static_cast<Derived&>(*this)(imp(x));
        }
      }

      void operator()(const core::identifier_string& s)
      {  }

      void operator()(const fixpoint_symbol& s)
      {  }

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/bes/detail/traverser.inc.h"
  };

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_TRAVERSER_H
