// Author(s): Wieger Wesselink, Alexander van Dam
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/io.h
/// \brief IO routines for boolean equation systems.

#ifndef MCRL2_PBES_IO_H
#define MCRL2_PBES_IO_H

#include <utility>
#include <iostream>
#include <string>
#include "mcrl2/utilities/file_utility.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2
{

namespace pbes_system
{

inline
const std::vector<utilities::file_format>& pbes_file_formats()
{
  static std::vector<utilities::file_format> result;
  if (result.empty())
  {
    result.push_back(utilities::file_format("pbes", "PBES in internal format"));
    result.back().add_extension(".pbes");
    result.push_back(utilities::file_format("pbes_text", "PBES in internal textual format"));
    result.back().add_extension(".aterm");
    result.push_back(utilities::file_format("text", "PBES in textual (mCRL2) format"));
    result.back().add_extension(".txt");
  }
  return result;
}

inline
bool is_pbes_file_format(const utilities::file_format* format)
{
  for (size_t i = 0; i < pbes_file_formats().size(); ++i)
  {
    if (&pbes_file_formats()[i] == format)
    {
      return true;
    }
  }
  return false;
}

inline
const utilities::file_format* pbes_format_internal() { return &pbes_file_formats()[0]; }
inline
const utilities::file_format* pbes_format_internal_text() { return &pbes_file_formats()[1]; }
inline
const utilities::file_format* pbes_format_text() { return &pbes_file_formats()[2]; }

inline
const utilities::file_format* guess_format(const std::string& filename)
{
  for (auto it = pbes_file_formats().begin(); it != pbes_file_formats().end(); ++it)
  {
    if (it->matches(filename))
    {
      return &*it;
    }
  }
  return utilities::file_format::unknown();
}

///
/// \brief Save a PBES in the format specified.
/// \param pbes The pbes to be stored
/// \param filename The name of the file to which the output is stored.
/// \param format Determines the format in which the result is written. If unspecified, or
///        pbes_file_unknown is specified, then a default format is chosen.
///
inline
void save_pbes(const pbes& pbes, std::ostream& stream,
               const utilities::file_format* format=utilities::file_format::unknown())
{
  if (format == utilities::file_format::unknown())
  {
    format = pbes_format_internal();
  }
  mCRL2log(log::verbose) << "Saving result in " << format->shortname() << " format..." << std::endl;
  if (format == pbes_format_internal())
  {
    pbes.save(stream, true);
  }
  else
  if (format == pbes_format_internal_text())
  {
    pbes.save(stream, false);
  }
  else
  if (format == pbes_format_text())
  {
    stream << pp(pbes);
  }
  else
  {
    throw mcrl2::runtime_error("Trying to save PBES in non-PBES format (" + format->shortname() + ")");
  }
}

/// \brief Load pbes from file.
/// \param p The pbes to which the result is loaded.
/// \param infilename The file from which to load the PBES.
/// \param f The format that should be assumed for the file in infilename.
inline
void load_pbes(pbes& pbes, std::istream& stream, const utilities::file_format* format)
{
  if (format == utilities::file_format::unknown())
  {
    format = pbes_format_internal();
  }
  mCRL2log(log::verbose) << "Loading PBES in " << format->shortname() << " format..." << std::endl;
  if (format == pbes_format_internal())
  {
    pbes.load(stream, true);
  }
  else
  if (format == pbes_format_internal_text())
  {
    pbes.load(stream, false);
  }
  else
  if (format == pbes_format_text())
  {
    stream >> pbes;
  }
  else
  {
    throw mcrl2::runtime_error("Trying to load PBES from non-PBES format (" + format->shortname() + ")");
  }
}

///
/// \brief save_pbes Saves a PBES to a file.
/// \param pbes The PBES to save.
/// \param filename The file to save the PBES in.
/// \param format The format in which to save the PBES.
///
inline
void save_pbes(const pbes &pbes, const std::string &filename,
               const utilities::file_format* format=utilities::file_format::unknown(),
               bool welltypedness_check=true)
{
  if (welltypedness_check)
  {
    assert(pbes.is_well_typed());
  }
  if (format == utilities::file_format::unknown())
  {
    format = guess_format(filename);
  }
  if (filename.empty() || filename == "-")
  {
    save_pbes(pbes, std::cout, format);
  }
  else
  {
    std::ofstream out(filename.c_str());
    if (out.good())
    {
      save_pbes(pbes, out, format);
    }
    else
    {
      throw mcrl2::runtime_error("Could not open file " + filename);
    }
  }
}

/// \brief Load pbes from file.
/// \param p The pbes to which the result is loaded.
/// \param infilename The file from which to load the PBES.
///
/// The format of the file in infilename is guessed.
inline
void load_pbes(pbes& pbes, const std::string& filename,
               const utilities::file_format* format=utilities::file_format::unknown())
{
  if (format == utilities::file_format::unknown())
  {
    format = guess_format(filename);
  }
  if (filename.empty() || filename == "-")
  {
    load_pbes(pbes, std::cin, format);
  }
  else
  {
    std::ifstream in(filename.c_str());
    if (in.good())
    {
      load_pbes(pbes, in, format);
    }
    else
    {
      throw mcrl2::runtime_error("Could not open file " + filename);
    }
  }
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_IO_H
