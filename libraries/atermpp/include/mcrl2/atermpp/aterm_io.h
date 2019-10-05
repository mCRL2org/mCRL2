// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_ATERMPP_ATERM_IO_H
#define MCRL2_ATERMPP_ATERM_IO_H

#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_int.h"

namespace atermpp
{

/// \brief A function that is applied to all terms. The resulting term should only use
///        a subset of the original arguments (i.e. not introduce new terms).
/// \details Typical usage is removing the index traits from function symbols that represent operators.
using aterm_transformer = aterm_appl(const aterm_appl&);

/// \brief The default transformer that maps each term to itself.
inline aterm_appl identity(const aterm_appl& x) { return x; }

/// \brief The interface for a class that writes aterm to a stream.
class aterm_ostream
{
public:
  virtual ~aterm_ostream();

  /// \brief Write the given term to the stream, this aterm is also returned from
  ///        the corresponding aterm_input::read_term() call.
  virtual aterm_ostream& operator<<(const aterm& term) = 0;

  /// \brief Sets the given transformer to be applied to following writes.
  aterm_ostream& operator<<(std::function<aterm_transformer> transformer)
  {
    m_transformer = transformer;
    return *this;
  }

protected:
  std::function<aterm_transformer> m_transformer = identity;
};

/// \brief The interface for a class that reads aterm from a stream.
class aterm_istream
{
public:
  virtual ~aterm_istream();

  /// \brief Reads a single term from this stream.
  /// \details The default constructed term aterm() indicates the end of the stream.
  virtual aterm get() = 0;

  /// \brief Reads a single term from this stream.
  /// \details The default constructed term aterm() indicates the end of the stream.
  aterm_istream& operator>>(aterm& output) { output = get(); return *this; }

  /// \brief Sets the given transformer to be applied to following reads.
  aterm_istream& operator>>(std::function<aterm_transformer> transformer)
  {
    m_transformer = transformer;
    return *this;
  }

protected:
  std::function<aterm_transformer> m_transformer = identity;
};

// These are utility functions.

/// \brief Send the term in textual form to the ostream.
std::ostream& operator<<(std::ostream& out, const aterm& t);

/// \param t The input aterm.
/// \return A string representation of the given term derived from an aterm.
inline std::string pp(const aterm& t)
{
  std::ostringstream oss;
  oss << t;
  return oss.str();
}

/// \brief Sends the name of a function symbol to an ostream.
/// \param out The out stream.
/// \param f The function symbol to be output.
/// \return The stream.
inline
std::ostream& operator<<(std::ostream& out, const function_symbol& f)
{
  return out << f.name();
}

/// \brief Prints the name of a function symbol as a string.
/// \param f The function symbol.
/// \return The string representation of r.
inline const std::string& pp(const function_symbol& f)
{
  return f.name();
}

/// \brief Writes term t to a stream in binary aterm format.
void write_term_to_binary_stream(const aterm& t, std::ostream& os);

/// \brief Reads a term from a stream in binary aterm format.
aterm read_term_from_binary_stream(std::istream& is);

/// \brief Writes term t to a stream in textual format.
void write_term_to_text_stream(const aterm& t, std::ostream& os);

/// \brief Reads a term from a stream which contains the term in textual format.
aterm read_term_from_text_stream(std::istream& is);

/// \brief Reads an aterm from a string. The string can be in either binary or text format.
aterm read_term_from_string(const std::string& s);

/// \brief Reads an aterm_list from a string. The string can be in either binary or text format.
/// \details If the input is not a string, an aterm is returned of the wrong type.
/// \return The term corresponding to the string.
inline aterm_list read_list_from_string(const std::string& s)
{
  const aterm_list l = down_cast<aterm_list>(read_term_from_string(s));
  assert(l.type_is_list());
  return l;
}

/// \brief Reads an aterm_int from a string. The string can be in either binary or text format.
/// \details If the input is not an int, an aterm is returned of the wrong type.
/// \return The aterm_int corresponding to the string.
inline aterm_int read_int_from_string(const std::string& s)
{
  const aterm_int n = down_cast<aterm_int>(read_term_from_string(s));
  assert(n.type_is_int());
  return n;
}

/// \brief Reads an aterm_appl from a string. The string can be in either binary or text format.
/// \details If the input is not an aterm_appl, an aterm is returned of the wrong type.
/// \return The term corresponding to the string.
inline aterm_appl read_appl_from_string(const std::string& s)
{
  const aterm_appl a = down_cast<aterm_appl>(read_term_from_string(s));
  assert(a.type_is_appl());
  return a;
}


} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_IO_H
