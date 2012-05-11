// Author(s): Aad Mathijssen, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/detail/aterm_io.h
/// \brief Load a file containing an ATerm.

#ifndef MCRL2_CORE_DETAIL_ATERM_IO_H
#define MCRL2_CORE_DETAIL_ATERM_IO_H

#include <cstdio>
#include <cerrno>
#include <string>
#include <cstring>
#include "mcrl2/exception.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_io.h"

namespace mcrl2
{

namespace core
{

namespace detail
{

using namespace atermpp;

/// \brief Loads an ATerm from the given file, or from stdin if filename is the empty string.
/// If reading fails an exception is thrown.
/// \param filename A string
/// \return The loaded term
inline
aterm load_aterm(const std::string& filename)
{
  //open filename for reading as stream
  FILE* stream = NULL;
  if (filename.empty())
  {
    stream = stdin;
  }
  else
  {
    stream = fopen(filename.c_str(), "rb");
  }
  if (stream == NULL)
  {
    std::string err_msg(strerror(errno));
    if (err_msg.length() > 0 && err_msg[err_msg.length()-1] == '\n')
    {
      err_msg.replace(err_msg.length()-1, 1, "");
    }
    throw mcrl2::runtime_error("could not open input file '" + filename + "' for reading (" + err_msg + ")");
  }
  //read term from stream
  aterm term = read_from_file(stream);
  if (stream != stdin)
  {
    fclose(stream);
  }
  if (term == NULL)
  {
    throw mcrl2::runtime_error("could not read a valid ATerm from " + ((stream == stdin)?"stdin":("'" + filename + "'")));
  }
  return term;
}

/// \brief Saves an ATerm to the given file, or to stdout if filename is the empty string.
/// If writing fails an exception is thrown.
/// \param term A term
/// \param filename A string
/// \param binary If true the term is stored in binary format
inline
void save_aterm(aterm term, const std::string& filename, bool binary = true)
{
  //open filename for writing as stream
  FILE* stream = NULL;
  if (filename.empty())
  {
    stream = stdout;
  }
  else
  {
    stream = fopen(filename.c_str(), binary?"wb":"w");
  }
  if (stream == NULL)
  {
    std::string err_msg(strerror(errno));
    if (err_msg.length() > 0 && err_msg[err_msg.length()-1] == '\n')
    {
      err_msg.replace(err_msg.length()-1, 1, "");
    }
    throw mcrl2::runtime_error("could not open output file '" + filename + "' for writing (" + err_msg + ")");
  }
  //write specification to stream
  bool result;
  if (binary)
  {
    result = ATwriteToSAFFile(term, stream);
  }
  else
  {
    result = ATwriteToTextFile(term, stream);
  }
  if (stream != stdout)
  {
    fclose(stream);
  }
  if (result == false)
  {
    throw mcrl2::runtime_error("could not write ATerm to " + ((stream == stdout)?"stdout":("'" + filename + "'")));
  }
}

/// \brief Specialization for aterm_appl.
inline
void save_aterm(atermpp::aterm_appl term, const std::string& filename, bool binary = true)
{
  aterm_appl t = term;
  save_aterm(static_cast<aterm>(t), filename, binary);
}

} // namespace detail

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_DETAIL_ATERM_IO_H
