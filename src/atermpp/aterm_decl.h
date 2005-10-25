// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/aterm_decl.h
// date          : 25-10-2005
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

#ifndef ATERM_DECL_H
#define ATERM_DECL_H

/** @file
  * This is a C++ wrapper around the ATerm library.
  */

#include <string>
#include <iostream>
#include <cassert>
#include "aterm2.h"
#include "atermpp/aterm_conversion.h"

namespace atermpp
{
  // prototype
  class aterm_list;
  class aterm_appl;
  class aterm_blob;
  class aterm_int;
  class aterm_real;
  class aterm_place_holder;
  template <typename T> struct aterm_conversion;

  //---------------------------------------------------------//
  //                    aterm
  //---------------------------------------------------------//
  class aterm
  {
    friend class aterm_appl;

    protected:
      void* m_term;
  
    public:
      aterm()
        : m_term(0)
      {}
      
      aterm(ATerm term)
        : m_term(term)
      {
      }
  
      aterm(ATermList term)
        : m_term(term)
      {}
  
      aterm(ATermInt term)
        : m_term(term)
      {
      }
  
      aterm(ATermReal term)
        : m_term(term)
      {
      }
  
      aterm(ATermBlob term)
        : m_term(term)
      {
      }
  
      aterm(ATermAppl term)
        : m_term(term)
      {
      }

      aterm(ATermPlaceholder term)
        : m_term(term)
      {
      }
  
      aterm(const std::string& s)
        : m_term(ATmake(const_cast<char*>(s.c_str())))
      {}

      const ATerm& term() const
      { return reinterpret_cast<const ATerm&>(m_term); }
  
      ATerm& term()
      { return reinterpret_cast<ATerm&>(m_term); }

      /**
        * Protect the aterm.
        * Protects the aterm from being freed at garbage collection.
        **/
      void protect()
      {
        ATprotect(&term());
      }

      /**
        * Unprotect the aterm.
        * Releases protection of the aterm which has previously been protected through a
        * call to protect.
        **/
      void unprotect()
      {
        ATunprotect(&term());
      }

      ATerm to_ATerm() const
      { return reinterpret_cast<ATerm>(m_term); }

      aterm_blob to_aterm_blob() const;
      aterm_real to_aterm_real() const;
      aterm_int  to_aterm_int() const;
      aterm_list to_aterm_list() const;
      aterm_appl to_aterm_appl() const;

//      aterm to_aterm() const
//      {
//        return *this;
//      }

      // allow conversion to ATerm
      operator ATerm() const
      { return to_ATerm(); } 

      /**
        * Return the type of term.
        * Result is one of AT_APPL, AT_INT,
        * AT_REAL, AT_LIST, AT_PLACEHOLDER, or AT_BLOB.
        **/
      int type() const
      { return ATgetType(to_ATerm()); }
      
      /**
        * Writes the term to a string.
        **/
      std::string to_string() const
      { return std::string(ATwriteToString(to_ATerm())); }

      /** Retrieve the annotation with the given label.
        *
        **/
      aterm annotation(aterm label) const
      {
        return ATgetAnnotation(to_ATerm(), label.to_ATerm());
      }
  }; 

} // namespace atermpp

#endif // ATERM_DECL_H
