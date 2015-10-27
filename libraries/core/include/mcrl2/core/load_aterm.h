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
/// \param source The source from which the stream originates (the empty string is used for an unknown source).
/// \param format The format that is being read (for example "LPS" or "PBES").
inline
atermpp::aterm load_aterm(std::istream& stream, bool binary = true, const std::string& format = "ATerm", const std::string& source = "")
{
  atermpp::aterm result;
  try
  {
    result = binary ? atermpp::read_term_from_binary_stream(stream) : atermpp::read_term_from_text_stream(stream);
  }
  catch (atermpp::baf_version_error&)
  {
    if (source.empty())
    {
      throw mcrl2::runtime_error("Version mismatch encountered while reading a " + format + ". This may be caused by attempting to read a file in an outdated format.");
    }
    else
    {
      throw mcrl2::runtime_error("Version mismatch encountered while reading a " + format + " from " + source + ". This may be caused by attempting to read a file in an outdated format.");
    }
  }
  catch (atermpp::aterm_io_error&)
  {
    if (source.empty())
    {
      throw mcrl2::runtime_error("Failed to read a valid " + format);
    }
    else
    {
      throw mcrl2::runtime_error("Failed to read a valid " + format + " from " + source);
    }
  }
  return result;
}

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_LOAD_ATERM_H
