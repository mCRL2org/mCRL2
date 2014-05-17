// Author(s): Aad Mathijssen, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/detail/aterm_io.h
/// \brief Load a file containing an aterm.

#ifndef MCRL2_CORE_DETAIL_ATERM_IO_H
#define MCRL2_CORE_DETAIL_ATERM_IO_H

#include <cstdio>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <string>
#include "mcrl2/utilities/exception.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_io.h"

namespace mcrl2
{

namespace core
{

namespace detail
{

using namespace atermpp;

/// \brief Loads an aterm from the given file, or from stdin if filename is the empty string.
/// If reading fails an exception is thrown.
/// \param filename A string
/// \return The loaded term
inline
aterm load_aterm(const std::string& filename)
{
  aterm result;
  try
  {
    //open filename for reading as stream
    if (filename.empty())
    {
      if(atermpp::is_binary_aterm_file(filename))
      {
        result=read_term_from_binary_stream(std::cin);
      }
      else
      {
        result=read_term_from_text_stream(std::cin);
      }
    }
    else
    {
      std::ifstream is;
      if(atermpp::is_binary_aterm_file(filename))
      {
        is.open(filename.c_str(), std::ios::binary);
        if (is.fail())
        {
          throw atermpp::aterm_io_error("Could not open input file.");
        }
        result=read_term_from_binary_stream(is);
      }
      else
      {
        is.open(filename.c_str());
        if (is.fail())
        {
          throw atermpp::aterm_io_error("Could not open input file.");
        }
        result=read_term_from_text_stream(is);
      }
      is.close();
    }
  }
  catch (std::runtime_error& e)
  {
    std::ostringstream ss;
    ss << "Could not read a valid aterm from ";
    if (filename.empty())
    {
      ss << "stdin. Caused by:\n";
    }
    else
    {
      ss << "'" << filename << "'. Caused by:\n";
    }
    ss << e.what();
    throw atermpp::aterm_io_error(ss.str());
  }
  return result;
}

/// \brief Saves an aterm to the given stream.
/// \param term A term
/// \param stream An output stream
/// \param binary If true the term is stored in binary format
inline
void save_aterm(aterm term, std::ostream& stream, bool binary = true)
{
  if (binary)
  {
    write_term_to_binary_stream(term, stream);
  }
  else
  {
    write_term_to_text_stream(term, stream);
  }
}

} // namespace detail

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_DETAIL_ATERM_IO_H
