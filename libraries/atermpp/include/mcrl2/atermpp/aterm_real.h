// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_real.h
/// \brief Term containing a real value.

#ifndef MCRL2_ATERMPP_ATERM_REAL_H
#define MCRL2_ATERMPP_ATERM_REAL_H

#include <cassert>
#include "mcrl2/atermpp/aterm.h"

namespace atermpp
{
  ///////////////////////////////////////////////////////////////////////////////
  // aterm_real
  /// \brief Represents an term containing a real value.
  class aterm_real: public aterm_base
  {
    public:
      /// \brief Constructor.
      aterm_real()
      {}
      
      /// \brief Constructor.
      /// \param value A value.
      aterm_real(double value)
        : aterm_base(ATmakeReal(value))
      {}
      
      /// \brief Constructor.
      /// \param t A real-valued term
      aterm_real(ATermReal t)
        : aterm_base(t)
      {}
  
      /// \brief Constructor.
      /// \param t A term
      aterm_real(ATerm t)
        : aterm_base(t)
      {
        assert(type() == AT_REAL);
      }
  
      /// Allow construction from an aterm. The aterm must be of the right type.
      /// \param t A term.
      aterm_real(aterm t)
        : aterm_base(t)
      {
        assert(type() == AT_REAL);
      }

      /// \brief Conversion to ATermReal.
      /// \return The converted term
      operator ATermReal() const
      {
        return reinterpret_cast<ATermReal>(m_term);
      }

      /// Assignment operator.
      /// \param t A term.
      aterm_real& operator=(aterm_base t)
      {
        assert(t.type() == AT_REAL);
        m_term = aterm_traits<aterm_base>::term(t);
        return *this;
      }

      /// \brief Get the real value of the aterm_real.
      /// \return The value of the term.
      double value() const
      {
        return ATgetReal(reinterpret_cast<ATermReal>(m_term));
      }
  }; 

  /// \cond INTERNAL_DOCS
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
  /// \endcond

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_REAL_H
