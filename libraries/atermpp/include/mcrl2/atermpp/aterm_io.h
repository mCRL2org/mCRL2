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
// #include "mcrl2/atermpp/detail/safio.h"
#include "mcrl2/atermpp/detail/bafio.h"

namespace atermpp
{

aterm read_from_file(FILE *file);
bool ATwriteToTextFile(const aterm &t, FILE* f);
unsigned char* ATwriteToBinaryString(const aterm &t, size_t* len);
aterm ATreadFromBinaryString(const unsigned char* s, size_t size);
bool ATwriteToBinaryFile(const aterm &t, FILE* file);



/// \brief Read an aterm from string.
/// This function parses a character string into an aterm.
/// \param s A string
/// \return The term corresponding to the string.
aterm read_term_from_string(const std::string& s);

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

aterm read_from_named_file(const std::string& name);

/// \brief Writes term t to file named filename in textual format.
/// This function writes aterm t in textual representation to file filename. "-" is
/// standard output's filename.
/// \param t A term.
/// \param filename A string
/// \return True if the operation succeeded.
bool write_to_named_text_file(aterm t, const std::string& filename);

/// \brief Writes term t to file named filename in Binary aterm Format (baf).
/// \param t A term.
/// \param filename A string
/// \return True if the operation succeeded.
/* inline bool write_to_named_binary_file(aterm t, const std::string& filename)
{
  return ATwriteToNamedBinaryFile(t, filename.c_str()) == true;
} */

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_IO_H
