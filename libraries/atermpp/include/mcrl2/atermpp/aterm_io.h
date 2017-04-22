// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/algorithm.h
/// \brief Algorithms for ATerms.

#ifndef MCRL2_ATERMPP_ATERM_IO_H
#define MCRL2_ATERMPP_ATERM_IO_H

#include <iomanip>
#include "mcrl2/atermpp/aterm.h"

namespace atermpp
{

bool is_binary_aterm_stream(std::istream& is);
bool is_binary_aterm_file(const std::string& filename);

/// \brief Writes term t to a stream in binary aterm format.
/// \param t A term.
/// \param os An output stream
void write_term_to_binary_stream(const aterm& t, std::ostream& os);


/// \brief Reads a term from a stream in binary aterm format.
/// \param is An input stream.
/// \return The term which is read.
aterm read_term_from_binary_stream(std::istream& is);


/// \brief Writes term t to a stream in textual format.
/// \param t A term.
/// \param os An outputstream string
void write_term_to_text_stream(const aterm& t, std::ostream& os);


/// \brief Reads a term from a stream which contains the term in textual format.
/// \param is An input stream.
/// \return The term that is read from the input stream.
aterm read_term_from_text_stream(std::istream& is);

/// \brief Reads an aterm from a string. The string can be in either binary or text format.
/// \param s A string
/// \return The term corresponding to the string.
aterm read_term_from_string(const std::string& s);

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_IO_H
