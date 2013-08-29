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
#include <string>
#include <cstring>
#include <fstream>
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
          throw mcrl2::runtime_error("could not open input file '" + filename + "' for reading.");
        }
        result=read_term_from_binary_stream(is);
      }
      else
      {
        is.open(filename.c_str());
        if (is.fail())
        {
          throw mcrl2::runtime_error("could not open input file '" + filename + "' for reading.");
        }
        result=read_term_from_text_stream(is);
      }
      is.close();
    }
  }
  catch (mcrl2::runtime_error& e)
  {
    throw mcrl2::runtime_error("could not read a valid aterm from " + ((filename.empty())?"stdin":("'" + filename + "'"))  + " (" + e.what() + ")");
  }
  catch (std::runtime_error&)
  {
    throw mcrl2::runtime_error("could not read a valid aterm from " + ((filename.empty())?"stdin":("'" + filename + "'")));
  }
  return result;
}


/// \brief Saves an aterm to the given file, or to stdout if filename is the empty string.
/// If writing fails an exception is thrown.
/// \param term A term
/// \param filename A string
/// \param binary If true the term is stored in binary format
inline
void save_aterm(aterm term, const std::string& filename, bool binary = true)
{
  using namespace std;

  if (filename.empty())
  {
    if (binary)
    {
      write_term_to_binary_stream(term, cout);
    }
    else
    {
      write_term_to_text_stream(term, cout);
    }
    if (cout.fail())
    {
      throw mcrl2::runtime_error("could not write aterm to standard out");
    }
  }
  else
  {
    std::ofstream os;
    if (binary)
    {
      os.open(filename.c_str(), std::ios::binary);
      write_term_to_binary_stream(term, os);
    }
    else
    {
      os.open(filename.c_str());
      write_term_to_text_stream(term, os);
    }
    if (os.fail())
    {
      throw mcrl2::runtime_error("could not write aterm to " + filename);
    }
    os.close();
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
