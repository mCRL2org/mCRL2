// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file atermpp/aterm_real.h
/// \brief Add your file description here.

#ifndef MCRL2_ATERMPP_ATERM_REAL_H
#define MCRL2_ATERMPP_ATERM_REAL_H

#include <cassert>
#include "atermpp/aterm.h"

namespace atermpp
{
  ///////////////////////////////////////////////////////////////////////////////
  // aterm_real
  /// \brief Represents an term containing a real value.
  ///
  class aterm_real: public aterm_base
  {
    public:
      aterm_real()
      {}
      
      aterm_real(double value)
        : aterm_base(ATmakeReal(value))
      {}
      
      aterm_real(ATermReal t)
        : aterm_base(t)
      {}
  
      aterm_real(ATerm t)
        : aterm_base(t)
      {
        assert(type() == AT_REAL);
      }
  
      /// Allow construction from an aterm. The aterm must be of the right type.
      ///
      aterm_real(aterm t)
        : aterm_base(t)
      {
        assert(type() == AT_REAL);
      }

      /// Conversion to ATermReal.
      ///
      operator ATermReal() const
      {
        return reinterpret_cast<ATermReal>(m_term);
      }

      aterm_real& operator=(aterm_base t)
      {
        assert(t.type() == AT_REAL);
        m_term = aterm_traits<aterm_base>::term(t);
        return *this;
      }

      /// Get the real value of the aterm_real.
      ///
      double value() const
      {
        return ATgetReal(reinterpret_cast<ATermReal>(m_term));
      }
  }; 

  template <>
  struct aterm_traits<aterm_real>
  {
    typedef ATermReal aterm_type;
    static void protect(aterm_real t)   { t.protect(); }
    static void unprotect(aterm_real t) { t.unprotect(); }
    static void mark(aterm_real t)      { t.mark(); }
    static ATerm term(aterm_real t)     { return t.term(); }
    static ATerm* ptr(aterm_real& t)    { return &t.term(); }
  };

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_REAL_H
