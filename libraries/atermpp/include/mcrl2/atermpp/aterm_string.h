// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_string.h
/// \brief Term containing a string.

#ifndef MCRL2_ATERMPP_ATERM_STRING_H
#define MCRL2_ATERMPP_ATERM_STRING_H

#include <string>
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/detail/utility.h"

namespace atermpp
{
  using detail::str2appl;

  /// \brief Term containing a string.
  class aterm_string: public aterm_appl
  {
    public:
      /// \brief Constructor.
      aterm_string()
      {}

      /// \brief Constructor.
      /// \param t A term
      aterm_string(ATermAppl t)
        : aterm_appl(t)
      {
        assert(aterm_appl(t).size() == 0);
      }

      /// \brief Constructor.
      /// \param t A term containing a string.
      aterm_string(aterm_appl t)
        : aterm_appl(t)
      {
        assert(t.size() == 0);
      }

      /// Allow construction from an aterm. The aterm must be of the right type, and may have no children.
      /// \param t A term containing a string.
      aterm_string(aterm t)
        : aterm_appl(t)
      {
        assert(t.type() == AT_APPL);
        assert(aterm_appl(t).size() == 0);
      }

      /// Allow construction from a string.
      /// \param s A string.
      /// \param quoted A boolean indicating if the string is quoted.
      aterm_string(std::string s, bool quoted = true)
        : aterm_appl(quoted ? str2appl(s) : make_term(s))
      {
        assert(type() == AT_APPL);
        assert(aterm_appl(m_term).size() == 0);
      }

      /// Assignment operator.
      /// \param t A term.
      aterm_string& operator=(aterm_base t)
      {
        assert(t.type() == AT_APPL);
        assert(aterm_appl(t).size() == 0);
        m_term = aterm_traits<aterm_base>::term(t);
        return *this;
      }

      /// \brief Conversion operator
      /// \return The term converted to string
      operator std::string() const
      {
        return function().name();
      }
  };

  /// \cond INTERNAL_DOCS
  template <>
  struct aterm_traits<aterm_string>
  {
    typedef ATermAppl aterm_type;
    static void protect(aterm_string t)   { t.protect(); }
    static void unprotect(aterm_string t) { t.unprotect(); }
    static void mark(aterm_string t)      { t.mark(); }
    static ATerm term(aterm_string t)     { return t.term(); }
    static ATerm* ptr(aterm_string& t)    { return &t.term(); }
  };
  /// \endcond

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_STRING_H
