// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_ATERMPP_ATERM_IO_TEXT_H
#define MCRL2_ATERMPP_ATERM_IO_TEXT_H

#include "mcrl2/atermpp/aterm_io.h"

namespace atermpp
{

/// \brief Writes terms in textual format to an output stream.
class text_aterm_output final : public aterm_output
{
public:
  /// \param newline When true each term is written on a new line.
  /// \param transformer A function transforming the function symbols before writing, see the type for details.
  text_aterm_output(std::ostream& os, bool newline = false);

  aterm_output& operator<<(const aterm& term) override;

  /// \brief Sets the given transformer to be applied to following writes.
  /// \todo This operator should not be necessary, but otherwise the aterm_output one cannot be used.
  aterm_output& operator<<(std::function<aterm_transformer> transformer)
  {
    m_transformer = transformer;
    return *this;
  }

private:
  /// \brief Writes a term in textual format on the same line.
  void write_term_line(const aterm& term);

  std::ostream& m_stream;

  bool m_newline = false; ///< Indicates that terms are separated by a newline.
};

/// \brief Reads terms in textual format from an input stream.
class text_aterm_input final : public aterm_input
{
public:
  text_aterm_input(std::istream& os);

  aterm get() override;

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

} // namespace atermpp

#endif // ATERM_IO_TEXT_H
