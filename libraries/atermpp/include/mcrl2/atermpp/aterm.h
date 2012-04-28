// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm.h
/// \brief The aterm class.

#ifndef MCRL2_ATERMPP_ATERM_H
#define MCRL2_ATERMPP_ATERM_H

#include <string>
#include <iostream>
#include <cassert>
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/aterm/memory.h"
#include "mcrl2/atermpp/aterm_traits.h"

using namespace aterm;

/// \brief The main namespace for the ATerm++ library.
namespace atermpp
{
	
/// \brief Base class for aterm.
class aterm_base
{
    template <typename T>
    friend struct aterm_traits;

    template <typename T>
    friend struct aterm_appl_traits;

  protected:
    /// The wrapped ATerm.
    ATerm m_term;

    /// \brief Returns const reference to the wrapped ATerm.
    /// \return A const reference to the wrapped ATerm.
    const ATerm& term() const
    {
      return m_term;
    }

    /// \brief Returns reference to the wrapped ATerm.
    /// \return A reference to the wrapped ATerm.
    ATerm& term()
    {
      return m_term;
    }

  public:
    /// \brief Constructor.
    aterm_base()
      : m_term()
    {}

    /// \brief Constructor.
    /// \param term A term
    aterm_base(ATerm term)
      : m_term(term)
    {
      assert(m_term==ATerm() || m_term->reference_count>0);
    }

    /// \brief Constructor.
    /// \param term A sequence of terms
    aterm_base(ATermList term)
      : m_term(term)
    {
      assert(m_term==ATerm() || m_term->reference_count>0);
    }

    /// \brief Constructor.
    /// \param term An integer term
    aterm_base(ATermInt term)
      : m_term(term)
    {
      assert(m_term==ATerm() || m_term->reference_count>0);
    }

    /// \brief Constructor.
    /// \param term A term
    aterm_base(ATermAppl term)
      : m_term(term)
    {
      assert(m_term==ATerm() || m_term->reference_count>0);
      assert(m_term==ATerm() || m_term->reference_count>0);
    }

    /// \brief Protect the aterm.
    /// Protects the aterm from being freed at garbage collection.
    void protect() const
    {
      // ATprotect(&m_term);
    }

    /// \brief Unprotect the aterm.
    /// Releases protection of the aterm which has previously been protected through a
    /// call to protect.
    void unprotect() const
    {
      // ATunprotect(&m_term);
    }

    /// \brief Mark the aterm for not being garbage collected.
    void mark() const
    {
      // ATmarkTerm(m_term);
    }

    /// \brief Return the type of term.
    /// Result is one of AT_APPL, AT_INT,
    /// AT_REAL, AT_LIST, AT_PLACEHOLDER, or AT_BLOB.
    /// \return The type of the term.
    size_t type() const
    {
      return ATgetType(m_term);
    }

    /// \brief Writes the term to a string.
    /// \return A string representation of the term.
    std::string to_string() const
    {
      return m_term.to_string(); 
    }

    bool operator <(const aterm_base &t) const
    {
      return m_term<t.m_term;
    }
};

/// \cond INTERNAL_DOCS
template <>
struct aterm_traits<aterm_base>
{
  static void protect(const aterm_base& t)
  {
    t.protect();
  }
  static void unprotect(const aterm_base& t)
  {
    t.unprotect();
  }
  static void mark(const aterm_base& t)
  {
    t.mark();
  }
  static ATerm term(const aterm_base& t)
  {
    return t.term();
  }
};
/// \endcond

/// \brief Returns true if x has the default value of an aterm. In the ATerm Library
/// this value is given by ATfalse.
/// \param x A term.
/// \return True if the value of the term is ATfalse.
inline
bool operator!(const aterm_base& x)
{
  // return ATisEqual(aterm_traits<aterm_base>::term(x), false);
  return &*(aterm_traits<aterm_base>::term(x))==NULL;
}

/// \brief Writes a string representation of the aterm t to the stream out.
/// \param out An output stream.
/// \param t A term.
/// \return The stream to which the string representation has been written.
inline
std::ostream& operator<<(std::ostream& out, const aterm_base& t)
{
  return out << t.to_string();
}

///////////////////////////////////////////////////////////////////////////////
// aterm
/// \brief Represents a generic term.
class aterm: public aterm_base
{
  public:
    /// \brief Constructor.
    aterm() {}

    /// \brief Constructor.
    /// \param term A term.
    aterm(aterm_base term)
      : aterm_base(term)
    { }

    /// \brief Constructor.
    /// \param term A term
    aterm(ATerm term)
      : aterm_base(term)
    { }

    /// \brief Constructor.
    /// \param term A sequence of terms
    aterm(ATermList term)
      : aterm_base(term)
    { }

    /// \brief Constructor.
    /// \param term An integer term
    aterm(ATermInt term)
      : aterm_base(term)
    { }

    /// \brief Constructor.
    /// \param term A term containing binary data
    /* aterm(ATermBlob term)
      : aterm_base(term)
    { } */

    /// \brief Constructor.
    /// \param term A term
    aterm(ATermAppl term)
      : aterm_base(term)
    { }

    /// \brief Conversion operator.
    /// \return The wrapped ATerm pointer
    operator ATerm() const
    {
      return m_term;
    }
};

/// \cond INTERNAL_DOCS
template <>
struct aterm_traits<aterm>
{
  static void protect(const aterm& t)
  {
    t.protect();
  }
  static void unprotect(const aterm& t)
  {
    t.unprotect();
  }
  static void mark(const aterm& t)
  {
    t.mark();
  }
  static ATerm term(const aterm& t)
  {
    return t.term();
  }
};
/// \endcond

/// \brief Read an aterm from string.
/// This function parses a character string into an aterm.
/// \param s A string
/// \return The term corresponding to the string.
inline
aterm read_from_string(const std::string& s)
{
  return ATreadFromString(s.c_str());
}

/// \brief Read a aterm from a string in baf format.
/// This function decodes a baf character string into an aterm.
/// \param s A string
/// \param size A positive integer
/// \return The term corresponding to the string.
inline
aterm read_from_binary_string(const std::string& s, unsigned int size)
{
  return ATreadFromBinaryString(reinterpret_cast<const unsigned char*>(s.c_str()), size);
}

/// \brief Read an aterm from named binary or text file.
/// This function reads an aterm file filename. A test is performed to see if the file
/// is in baf, taf, or plain text. "-" is standard input's filename.
/// \param name A string
/// \return A term that was read from a file.
inline
aterm read_from_named_file(const std::string& name)
{
  return ATreadFromNamedFile(name.c_str());
}

/// \brief Writes term t to file named filename in textual format.
/// This function writes aterm t in textual representation to file filename. "-" is
/// standard output's filename.
/// \param t A term.
/// \param filename A string
/// \return True if the operation succeeded.
inline
bool write_to_named_text_file(aterm t, const std::string& filename)
{
  return ATwriteToNamedTextFile(t, filename.c_str()) == true;
}

/// \brief Writes term t to file named filename in Binary aterm Format (baf).
/// \param t A term.
/// \param filename A string
/// \return True if the operation succeeded.
inline
bool write_to_named_binary_file(aterm t, const std::string& filename)
{
  return ATwriteToNamedBinaryFile(t, filename.c_str()) == true;
}

/// \brief Writes term t to file named filename in Streamable aterm Format (saf).
/// \param t A term.
/// \param filename A string
/// \return True if the operation succeeded.
/* inline
bool write_to_named_saf_file(aterm t, const std::string& filename)
{
  return ATwriteToNamedSAFFile(t, filename.c_str()) == ATtrue;
}
*/

/// \brief Equality operator.
/// \param x A term.
/// \param y A term.
/// \return True if the terms are equal.
inline
bool operator==(const aterm& x, const aterm& y)
{
  return ATisEqual(x, y) == true;
} 

/// \brief Inequality operator.
/// \param x A term.
/// \param y A term.
/// \return True if the terms are not equal.
inline
bool operator!=(const aterm& x, const aterm& y)
{
  return ATisEqual(x, y) == false;
} 

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_H
