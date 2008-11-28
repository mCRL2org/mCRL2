// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_int.h
/// \brief Term containing an integer.

#ifndef MCRL2_ATERMPP_ATERM_INT_H
#define MCRL2_ATERMPP_ATERM_INT_H

#include "mcrl2/atermpp/aterm.h"

namespace atermpp
{
  /// \brief Term containing an integer.
  ///
  class aterm_int: public aterm_base
  {
    public:
      /// Constructor.
      ///
      aterm_int()
      {} 

      /// Constructor.
      ///
      /// \param t A term containing an int.
      aterm_int(ATermInt t)
        : aterm_base(t)
      {}
  
      /// Allow construction from an aterm. The aterm must be of the right type.
      ///
      /// \param t A term.
      aterm_int(aterm t)
        : aterm_base(t)
      {
        assert(type() == AT_INT);
      }

      /// Constructor.
      ///
      /// \param value An integer value.
      aterm_int(int value)
        : aterm_base(ATmakeInt(value))
      {}

      /// Conversion to ATermInt.
      ///
      operator ATermInt() const
      {
        return reinterpret_cast<ATermInt>(m_term);
      }

      /// Assignment operator.
      ///
      /// \param t A term.
      aterm_int& operator=(aterm_base t)
      {
        assert(t.type() == AT_INT);
        m_term = aterm_traits<aterm_base>::term(t);
        return *this;
      }

      /// Get the integer value of the aterm_int.
      ///
      /// \return The value of the term.
      int value() const
      {
        return ATgetInt(reinterpret_cast<ATermInt>(m_term));
      }
  };

  /// \cond INTERNAL_DOCS
  template <>
  struct aterm_traits<aterm_int>
  {
    typedef ATermInt aterm_type;
    static void protect(aterm_int t)   { t.protect(); }
    static void unprotect(aterm_int t) { t.unprotect(); }
    static void mark(aterm_int t)      { t.mark(); }
    static ATerm term(aterm_int t)     { return t.term(); }
    static ATerm* ptr(aterm_int& t)    { return &t.term(); }
  };
  /// \endcond

} // namespace atermpp

#include "mcrl2/atermpp/aterm_make_match.h"

#endif // MCRL2_ATERMPP_ATERM_INT_H
