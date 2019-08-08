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
#include "mcrl2/utilities/bitstream.h"
#include "mcrl2/utilities/indexed_set.h"

namespace atermpp
{

/// \brief The interface for a class that writes aterm to a stream.
class aterm_output
{
public:
  virtual ~aterm_output();

  /// \brief Write the given term to the stream, this aterm is also returned from
  ///        the corresponding aterm_input::read_term() call.
  virtual void write_term(const aterm& term) = 0;

};

/// \brief The interface for a class that reads aterm from a stream.
class aterm_input
{
public:
  virtual ~aterm_input();

  /// \brief Reads a single term from this stream.
  /// \details The default constructed term aterm() indicates the end of the stream.
  virtual aterm read_term() = 0;

};

/// \brief Writes terms in a streamable binary aterm format to an output stream.
/// \details The streamable aterm format:
///
///          Aterms (and function symbols) are written as packets (with an identifier in the header) and their
///          indices are derived from the number of aterms, resp. symbols, that occur before them in this stream. For each term
///          we first ensure that its arguments and symbol are written to the stream (avoiding duplicates). Then its
///          symbol index followed by a number of indices (depending on the arity) for its argments are written as integers.
///          Packet headers also contain a special value to indicate that the read term should be visible as output as opposed to
///          being only a subterm.
///          The start of the stream is a zero followed by a header and a version and a term with function symbol index zero
///          indicates the end of the stream.
///
class binary_aterm_output : public aterm_output
{
public:
  /// \brief Provide the output stream to which the terms are written.
  binary_aterm_output(std::ostream& os);
  ~binary_aterm_output() override;

  /// \brief Writes an aterm in a compact binary format that keeps subterms shared. The term that is
  ///        written itself is not shared whenever it occurs as the argument of another term.
  void write_term(const aterm& term) override;

private:
  /// \brief Write a function symbol to the output stream.
  std::size_t write_function_symbol(const function_symbol& symbol);

  mcrl2::utilities::obitstream m_stream;

  /// \returns The number of bits needed to index terms.
  unsigned int term_index_width();

  /// \returns The number of bits needed to index function symbols.
  unsigned int function_symbol_index_width();

  unsigned int m_term_index_width; ///< caches the result of term_index_width().
  unsigned int m_function_symbol_index_width; ///< caches the result of function_symbol_index_width().

  mcrl2::utilities::indexed_set_large<aterm> m_terms; ///< An index of already written terms.
  mcrl2::utilities::indexed_set_large<function_symbol> m_function_symbols; ///< An index of already written function symbols.
};

/// \brief Reads terms from a stream in the steamable binary aterm format.
class binary_aterm_input : public aterm_input
{
public:
  /// \brief Provide the input stream from which terms are read.
  binary_aterm_input(std::istream& is);

  aterm read_term() override;

private:
  mcrl2::utilities::ibitstream m_stream;

  /// \returns The number of bits needed to index terms.
  unsigned int term_index_width();

  /// \returns The number of bits needed to index function symbols.
  unsigned int function_symbol_index_width();

  unsigned int m_term_index_width; ///< caches the result of term_index_width().
  unsigned int m_function_symbol_index_width; ///< caches the result of function_symbol_index_width().

  std::deque<aterm> m_terms; ///< An index of read terms.
  std::deque<function_symbol> m_function_symbols; ///< An index of read function symbols.
};

/// \brief Writes terms in textual format to an output stream.
class text_aterm_output : public aterm_output
{
public:
  /// \param newline When true each term is written on a new line.
  text_aterm_output(std::ostream& os, bool newline = false);

  void write_term(const aterm& term) override;

private:
  /// \brief Writes a term in textual format on the same line.
  void write_term_line(const aterm& term);

  std::ostream& m_stream;

  bool m_newline = false; ///< Indicates that terms are separated by a newline.
};

/// \brief Reads terms in textual format from an input stream.
class text_aterm_input : public aterm_input
{
public:
  text_aterm_input(std::istream& os);

  aterm read_term() override;

private:
  /// \brief Parse a term from the input stream and return it.
  aterm parse_aterm(int& character);

  /// \brief Parses an "f"(t0, ..., tn) application as an aterm_appl.
  aterm_appl parse_aterm_appl(const std::string& function_name, int& character);

  /// \brief Parses an std::size_t as an aterm_int.
  aterm_int parse_aterm_int(int& character);

  /// \brief Parses a list of arguments [...] as terms.
  aterm_list parse_aterm_list(int& character, char begin, char end);

  /// \brief Reads a quoted string from the stream.
  /// \returns The parsed string and the first character after this string that is not whitespace.
  std::string parse_quoted_string(int& character);

  /// \brief Reads an unquoted string from the stream.
  std::string parse_unquoted_string(int& character);

  /// \returns A string indicating the parse error position.
  std::string print_parse_error_position();

  /// \returns The first character that is not whitespace or end-of-file (EOF).
  /// \param skip_whitespace, returns the next non space character.
  /// \param required Throw error when the next character is EOL.
  int next_char(bool skip_whitespace = true, bool required = false);

  std::istream& m_stream;

  std::size_t m_line = 0; ///< The line number of the current character.
  std::size_t m_column = 0; ///< The column of the current character.

  int character; ///< The last character that was read.

  std::deque<char> m_history; ///< Stores the characters that have been read so-far.
  std::size_t m_history_limit = 64; ///< Determines the maximum number of characters that are stored.
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
