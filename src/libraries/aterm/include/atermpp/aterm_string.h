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
  class aterm_string: public aterm
  {
    public:
      aterm_string()
      {} 

      aterm_string(ATermAppl t)
        : aterm(t)
      {
        assert(aterm_appl(t).argument_list().size() == 0);
      }
  
      /// Allow construction from an aterm. The aterm must be of the right type, and may have no children.
      ///
      aterm_string(aterm t)
        : aterm(t)
      {
        assert(t.type() == AT_APPL);
        assert(aterm_appl(t).argument_list().size() == 0);
      }

      /// Allow construction from a string.
      ///
      aterm_string(std::string s, bool quoted = true)
        : aterm(quoted ? str2appl(s) : make_term(s))
      {
        assert(type() == AT_APPL);
        assert(aterm_appl(m_term).argument_list().size() == 0);
      }

      operator std::string() const
      {
        return aterm(m_term).to_string();
      }

      operator ATermAppl() const
      {
        return reinterpret_cast<ATermAppl>(m_term);
      }
  };

  inline
  bool operator<(aterm_string x, aterm_string y)
  {
    return ATermAppl(x) < ATermAppl(y);
  }

  /// INTERNAL ONLY
  inline
  ATerm aterm_ptr(aterm_string& t)
  {
    return t;
  }
  
  /// INTERNAL ONLY
  inline
  ATerm aterm_ptr(const aterm_string& t)
  {
    return t;
  }

   template <>
   class aterm_protect_traits<aterm_string>
   {
     public:
       static void protect(aterm_string t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<aterm_string>::protect() " << t << std::endl;
#endif // ATERM_DEBUG_PROTECTION
         t.protect();
       }

       static void unprotect(aterm_string t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<aterm_string>::unprotect() " << t << std::endl;
#endif // ATERM_DEBUG_PROTECTION
         t.unprotect();
       }

       static void mark(aterm_string t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<aterm_string>::mark() " << t << std::endl;
#endif // ATERM_DEBUG_PROTECTION
         t.mark();
       }
   };

} // namespace atermpp

#endif // ATERM_STRING_H
