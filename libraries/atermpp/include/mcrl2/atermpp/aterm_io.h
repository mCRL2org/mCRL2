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


#include "mcrl2/atermpp/aterm.h"

namespace atermpp
{

/// \brief Writes term t to file named filename in Binary aterm Format (baf).
/// \param t A term.
/// \param filename A string
/// \return True if the operation succeeded.
void write_term_to_binary_stream(const aterm &t, std::ostream &os);


/// \brief Reads a term from a file in Binary aterm Format (baf).
/// \param file A file in baf format.
/// \return The term which is read.
aterm read_term_from_binary_file(FILE* file);


/// \brief Reads a term from a file in Binary aterm Format (baf).
/// \param filename The name of a file in baf format.
/// \return The term which is read.
aterm read_term_from_binary_file(const std::string& filename);


/// \brief Writes term t to a file in textual format to stream os.
/// \detail This function writes aterm t in textual representation to the output stream.
///         Error handling and setting whether exceptions are being generated is the
///         responsibility of the surrounding function.
/// \param t A term.
/// \param os An outputstream string
/// \return True if the operation succeeded.
void write_term_to_text_stream(const aterm &t, std::ostream &os);


/// \brief Writes term t to a file in textual format.
/// This function writes aterm t in textual representation to file filename. "-" is
/// standard output's filename.
/// \param t A term.
/// \param filename A string.
/// \return True if the operation succeeded.
aterm read_term_from_text_file(FILE *file);


/// \brief Writes term t to file named filename in textual format.
/// This function writes aterm t in textual representation to file filename. "-" is
/// standard output's filename.
/// \param t A term.
/// \param filename A string.
/// \return True if the operation succeeded.
aterm read_term_from_text_file(const std::string& filename);


/// \brief Read a aterm from a string in baf format.
/// This function decodes a baf character string into an aterm.
/// \param s A string.
/// \param size A positive integer.
/// \return The term corresponding to the string.
aterm read_term_from_binary_string(const std::string& s);


/// \brief Read a aterm from a string in baf format.
/// This function decodes a baf character string into an aterm.
/// \param s A string
/// \param size A positive integer
/// \return The term corresponding to the string.
aterm read_term_from_text_string(const std::string& s);


/// \brief Read an aterm from named binary or text file.
/// This function reads an aterm file filename. A test is performed to see if the file
/// is in baf, taf, or plain text. "-" is standard input's filename.
/// \param name A string
/// \return A term that was read from a file.
aterm read_term_from_file(const std::string& name);


/// \brief Read an aterm from a file in binary or text format.
/// This function reads an aterm file filename. A test is performed to see if the file
/// is in baf, taf, or plain text. "-" is standard input's filename.
/// \param name A string
/// \return A term that was read from a file.
aterm read_term_from_file(FILE *file);


/// \brief Read an aterm from string.
/// This function parses a character string into an aterm.
/// \param s A string
/// \return The term corresponding to the string.
aterm read_term_from_string(const std::string& s);




} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_IO_H
