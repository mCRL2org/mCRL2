// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/algorithm.h
/// \brief Algorithms for ATerms.

#ifndef MCRL2_ATERMPP_ATERM_IO_H
#define MCRL2_ATERMPP_ATERM_IO_H

#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/utilities/bitstream.h"
#include "mcrl2/utilities/indexed_set.h"

namespace atermpp
{

bool is_binary_aterm_stream(std::istream& is);
bool is_binary_aterm_file(const std::string& filename);

/// \brief Streams terms to a stream in binary aterm format.
class binary_aterm_output
{
public:
  binary_aterm_output(std::ostream& os);
  ~binary_aterm_output();

  /// \brief Write the given term to this stream.
  void write_term(const aterm& term);

private:
  /// \brief Write a function symbol to the output stream.
  std::size_t write_function_symbol(const function_symbol& symbol);

  mcrl2::utilities::obitstream m_stream;

  mcrl2::utilities::indexed_set_large<aterm> m_terms; ///< An index of already written terms.
  mcrl2::utilities::indexed_set_large<function_symbol> m_function_symbol; ///< An index of already written function symbols.
};

/// \brief Streams terms to a stream in binary aterm format.
class binary_aterm_input
{
public:
  binary_aterm_input(std::istream& is);

  /// \brief Write a single term.
  aterm read_term();

private:
  mcrl2::utilities::ibitstream m_stream;

  std::deque<aterm> m_terms; ///< An index of already written terms.
  std::deque<function_symbol> m_function_symbol; ///< An index of already written function symbols.
};

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
