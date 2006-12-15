// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/aterm_string.h
// date          : 18-09-2006
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

///////////////////////////////////////////////////////////////////////////////
/// \file aterm_string.h
/// Contains the definition of the aterm_string class.

#ifndef ATERM_STRING_H
#define ATERM_STRING_H

#include <string>
#include "atermpp/aterm.h"
#include "atermpp/aterm_appl.h"
#include "atermpp/detail/utility.h"

namespace atermpp
{
  using detail::str2appl;
  
  //---------------------------------------------------------//
  //                    aterm_string
  //---------------------------------------------------------//
  class aterm_string: public aterm_appl
  {
    public:
      aterm_string()
      {} 

      aterm_string(ATermAppl t)
        : aterm_appl(t)
      {
        assert(aterm_appl(t).size() == 0);
      }
  
      aterm_string(aterm_appl t)
        : aterm_appl(t)
      {
        assert(t.size() == 0);
      }
  
      /// Allow construction from an aterm. The aterm must be of the right type, and may have no children.
      ///
      aterm_string(aterm t)
        : aterm_appl(t)
      {
        assert(t.type() == AT_APPL);
        assert(aterm_appl(t).size() == 0);
      }

      /// Allow construction from a string.
      ///
      aterm_string(std::string s, bool quoted = true)
        : aterm_appl(quoted ? str2appl(s) : make_term(s))
      {
        assert(type() == AT_APPL);
        assert(aterm_appl(m_term).size() == 0);
      }

      aterm_string& operator=(aterm_base t)
      {
        assert(t.type() == AT_APPL);
        assert(aterm_appl(t).size() == 0);
        m_term = aterm_traits<aterm_base>::term(t);
        return *this;
      }

      operator std::string() const
      {
        return this->to_string();
      }
  };

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

} // namespace atermpp

#endif // ATERM_STRING_H
