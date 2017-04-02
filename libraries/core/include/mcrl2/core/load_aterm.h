// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/load_aterm.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_LOAD_ATERM_H
#define MCRL2_CORE_LOAD_ATERM_H

#include <istream>
#include <string>
#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/utilities/exception.h"

namespace mcrl2 {

namespace core {

namespace detail {

inline
std::string file_source(const std::string& filename)
{
  if (filename.empty() || filename == "-")
  {
    return "standard input";
  }
  return filename;
}

} // namespace detail

/// \brief Attempts to read an aterm from a stream.
/// \param[in] stream The stream from which the term is read.
/// \param[in] binary A boolean indicating whether the stream is in binary of textual format.
/// \param[in] format The format that is being read (for example "LPS" or "PBES").
/// \param[in] source The source from which the stream originates (the empty string is used for an unknown source).
/// \exception Throws a mcrl2 runtime error when an error occurs when reading the term.
inline
atermpp::aterm load_aterm(std::istream& stream, bool binary = true, const std::string& format = "aterm", const std::string& source = "")
{
  atermpp::aterm result;
  try
  {
    result = binary ? atermpp::read_term_from_binary_stream(stream) : atermpp::read_term_from_text_stream(stream);
  }
  catch (std::exception &e)
  {
    if (source.empty())
    {
      throw mcrl2::runtime_error("Failed to read a valid " + format + " from the input.\n" + e.what());
    }
    else 
    {
      throw mcrl2::runtime_error("Failed to read a valid " + format + " from the input " + source + ".\n" + e.what());
    }
  }
  return result;
}

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_LOAD_ATERM_H
