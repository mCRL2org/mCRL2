// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/text_utility.h
/// \brief String manipulation functions.

#ifndef MCRL2_CORE_TEXT_UTILITY_H
#define MCRL2_CORE_TEXT_UTILITY_H

#include <string>
#include <vector>
#include <sstream>
#include <iterator>

namespace mcrl2 {

namespace core {

  /// \brief Split a string into paragraphs.
  /// \param text A string
  /// \return The paragraphs of <tt>text</tt>
  std::vector<std::string> split_paragraphs(const std::string& text);

  /// \brief Split the text.
  /// \param line A string
  /// \param separators A string
  /// \return The splitted text
  std::vector<std::string> split(const std::string& line, const std::string& separators);

  /// \brief Read text from a file.
  /// \param filename A string
  /// \param warn If true, a warning is printed to standard error if the file is not found
  /// \return The contents of the file
  std::string read_text(const std::string& filename, bool warn=false);

  /// \brief Remove comments from a text (everything from '%' until end of line).
  /// \param text A string
  /// \return The removal result
  std::string remove_comments(const std::string& text);

  /// \brief Removes whitespace from a string.
  /// \param text A string
  /// \return The removal result
  std::string remove_whitespace(const std::string& text);

  /// \brief Regular expression replacement in a string.
  /// \param src A string
  /// \param dest A string
  /// \param text A string
  /// \return The transformed string
  std::string regex_replace(const std::string& src, const std::string& dest, const std::string& text);

  /// \brief Split a string using a regular expression separator.
  /// \param text A string
  /// \param sep A string
  /// \return The splitted string
  std::vector<std::string> regex_split(const std::string& text, const std::string& sep);

  /// \brief Joins a sequence of strings. This is a replacement for
  /// boost::algorithm::join, since it gives stack overflow errors with
  /// Visual C++ express 9.0 under some circumstances.
  template <typename Container>
  std::string string_join(const Container& c, const std::string& separator)
  {
    std::ostringstream out;
    for (typename Container::const_iterator i = c.begin(); i != c.end(); ++i)
    {
      if (i != c.begin())
      {
        out << separator;
      }
      out << *i;
    }
    return out.str();
  }

  /// \brief Apply word wrapping to a text.
  /// \param text A string of text.
  /// \param max_line_length The maximum line length.
  /// \return The wrapped text.
  std::string word_wrap_text(const std::string& text, unsigned int max_line_length = 78);

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_TEXT_UTILITY_H
