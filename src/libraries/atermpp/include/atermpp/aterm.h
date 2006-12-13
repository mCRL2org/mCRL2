// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/aterm.h
// date          : 25-10-2005
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

///////////////////////////////////////////////////////////////////////////////
/// \file aterm.h
/// Contains the definition of the aterm class and a few aterm functions.

#ifndef ATERM_ATERM_H
#define ATERM_ATERM_H

#include <string>
#include <iostream>
#include <cassert>
#include "aterm2.h"
#include "atermpp/aterm_traits.h"

namespace atermpp
{
  /// INTERNAL ONLY
  class aterm_base
  {
    template <typename T>
    friend struct aterm_traits;
    
    protected:
      ATerm m_term;
  
      const ATerm& term() const
      { return m_term; }
      
      ATerm& term()
      { return m_term; }

    public:
      aterm_base()
        : m_term(0)
      {}
      
      aterm_base(ATerm term)
        : m_term(term)
      {
      }

      aterm_base(ATermList term)
        : m_term(reinterpret_cast<ATerm>(term))
      {}
  
      aterm_base(ATermInt term)
        : m_term(reinterpret_cast<ATerm>(term))
      {
      }
  
      aterm_base(ATermReal term)
        : m_term(reinterpret_cast<ATerm>(term))
      {
      }
  
      aterm_base(ATermBlob term)
        : m_term(reinterpret_cast<ATerm>(term))
      {
      }
  
      aterm_base(ATermAppl term)
        : m_term(reinterpret_cast<ATerm>(term))
      {
      }

      aterm_base(const std::string& s)
        : m_term(ATmake(const_cast<char*>(s.c_str())))
      {}

      /// Protect the aterm.
      /// Protects the aterm from being freed at garbage collection.
      ///
      void protect()
      {
        ATprotect(&m_term);
      }

      /// Unprotect the aterm.
      /// Releases protection of the aterm which has previously been protected through a
      /// call to protect.
      ///
      void unprotect()
      {
        ATunprotect(&m_term);
      }

      /// Mark the aterm for not being garbage collected.
      ///
      void mark()
      {
        ATmarkTerm(m_term);
      }

      /// Return the type of term.
      /// Result is one of AT_APPL, AT_INT,
      /// AT_REAL, AT_LIST, AT_PLACEHOLDER, or AT_BLOB.
      ///
      int type() const
      { return ATgetType(m_term); }
      
      /// Writes the term to a string.
      ///
      std::string to_string() const
      { return std::string(ATwriteToString(m_term)); }
  };

  template <>                           
  struct aterm_traits<aterm_base>
  {                                     
    typedef ATerm aterm_type;           
    static void protect(aterm_base t)        { t.protect(); }
    static void unprotect(aterm_base t)      { t.unprotect(); }
    static void mark(aterm_base t)           { t.mark(); }
    static ATerm term(aterm_base t)          { return t.term(); }
    static ATerm* ptr(aterm_base& t)         { return &t.term(); }
  };

  /// Returns true if x has the default value of an aterm. In the ATerm Library
  /// this value is given by ATfalse.
  inline
  bool operator!(const aterm_base& x)
  {
    return ATisEqual(aterm_traits<aterm_base>::term(x), ATfalse);
  }

  /// Writes a string representation of the aterm t to the stream out.
  ///
  inline
  std::ostream& operator<<(std::ostream& out, const aterm_base& t)
  {
    return out << t.to_string();
  }

  ///////////////////////////////////////////////////////////////////////////////
  // aterm
  /// \brief Represents a generic term.
  ///
  class aterm: public aterm_base
  {
    public:
      aterm() {}

      aterm(aterm_base term)
        : aterm_base(term)
      { }

      aterm(ATerm term)
        : aterm_base(term)
      { }

      aterm(ATermList term)
        : aterm_base(term)
      { }
  
      aterm(ATermInt term)
        : aterm_base(term)
      { }
  
      aterm(ATermReal term)
        : aterm_base(term)
      { }
  
      aterm(ATermBlob term)
        : aterm_base(term)
      { }
  
      aterm(ATermAppl term)
        : aterm_base(term)
      { }

      aterm(const std::string& s)
        : aterm_base(s)
      { }

      // allow conversion to ATerm
      operator ATerm() const
      { return m_term; } 
  }; 

  template <>                           
  struct aterm_traits<aterm>            
  {                                     
    typedef ATerm aterm_type;           
    static void protect(aterm t)        { t.protect(); }
    static void unprotect(aterm t)      { t.unprotect(); }
    static void mark(aterm t)           { t.mark(); }
    static ATerm term(aterm t)          { return t.term(); }
    static ATerm* ptr(aterm& t)         { return &t.term(); }
  };

  /// Read an aterm from string.
  /// This function parses a character string into an aterm.
  ///
  inline
  aterm read_from_string(const std::string& s)
  {
    return ATreadFromString(s.c_str());
  }
  
  /// Read a aterm from a string in baf format.
  /// This function decodes a baf character string into an aterm.
  ///
  inline
  aterm read_from_binary_string(const std::string& s, unsigned int size)
  {
    return ATreadFromBinaryString(const_cast<char*>(s.c_str()), size);
  }
  
  /// Read a aterm from a string in taf format.
  /// This function decodes a taf character string into an aterm.
  ///
  inline
  aterm read_from_shared_string(const std::string& s, unsigned int size)
  {
    return ATreadFromSharedString(const_cast<char*>(s.c_str()), size);
  }
  
  /// Read an aterm from named binary or text file.
  /// This function reads an aterm file filename. A test is performed to see if the file
  /// is in baf, taf, or plain text. "-" is standard input's filename.
  ///
  inline
  aterm read_from_named_file(const std::string& name)
  {
    return ATreadFromNamedFile(name.c_str());
  }

  /// Writes term t to file named filename in textual format.
  /// This function writes aterm t in textual representation to file filename. "-" is
  /// standard output's filename.
  ///
  inline
  bool write_to_named_text_file(aterm t, const std::string& filename)
  {
    return ATwriteToNamedTextFile(t, filename.c_str()) == ATtrue;
  }

  /// Writes term t to file named filename in Binary aterm Format (baf).
  ///
  inline
  bool write_to_named_binary_file(aterm t, const std::string& filename)
  {
    return ATwriteToNamedBinaryFile(t, filename.c_str()) == ATtrue;
  }

  /// Annotate a term with a labeled annotation.
  /// Creates a version of t that is annotated with annotation and labeled by
  /// label.
  ///
  inline
  aterm set_annotation(aterm t, aterm label, aterm annotation)
  {
    return ATsetAnnotation(t, label, annotation);
  }

  /// Retrieves annotation of t with label label.
  /// This function can be used to retrieve a specific annotation of a term. If t has
  /// no annotations, or no annotation labeled with label exists, `aterm()` is returned. Otherwise the
  /// annotation is returned.
  ///
  inline
  aterm get_annotation(aterm t, aterm label)
  {
    return ATgetAnnotation(t, label);
  }

  /// Remove a specific annotation from a term.
  /// This function returns a version of t which has its annotation with label label
  /// removed. If t has no annotations, or no annotation labeled with label exists, t itself is returned.
  ///
  inline
  aterm remove_annotation(aterm t, aterm label)
  {
    return ATremoveAnnotation(t, label);
  }

  /// Initialize the ATerm++ Library. The specified argument t is used to mark the
  /// the bottom of the program stack. All aterms in the range [bottom_of_stack,...[
  /// will not be garbage collected.
  /// 
  inline
  void aterm_init(const aterm& bottom_of_stack)
  {
    ATerm a = bottom_of_stack;
    ATinit(0, 0, &a);
  }

  inline
  bool operator==(const aterm& x, const aterm& y)
  {
    return ATisEqual(x, y) == ATtrue;
  }
  
  inline
  bool operator==(const aterm& x, ATerm y)
  {
    return ATisEqual(x, y) == ATtrue;
  }
  
  inline
  bool operator==(const ATerm& x, aterm y)
  {
    return ATisEqual(x, y) == ATtrue;
  }

} // namespace atermpp

#include "atermpp/aterm_make_match.h"

#endif // ATERM_ATERM_H
