// Author(s): anonymous
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

#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/utilities/file_utility.h"
#include "mcrl2/utilities/logger.h"
#include <iostream>
#include <string>
#include <utility>

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
    result.push_back(utilities::file_format("pbes", "PBES in internal format", false));
    result.back().add_extension("pbes");
    result.push_back(utilities::file_format("text", "PBES in textual (mCRL2) format", true));
    result.back().add_extension("txt");
  }
  return result;
}

inline
bool is_pbes_file_format(const utilities::file_format& format)
{
  for (const utilities::file_format& i: pbes_file_formats())
  {
    if (i == format)
    {
      return true;
    }
  }
  return false;
}

inline
const utilities::file_format& pbes_format_internal() { return pbes_file_formats()[0]; }
inline
const utilities::file_format& pbes_format_text() { return pbes_file_formats()[1]; }

inline
const utilities::file_format guess_format(const std::string& filename)
{
  for (const utilities::file_format& it : pbes_file_formats())
  {
    if (it.matches(filename))
    {
      return it;
    }
  }
  return utilities::file_format();
}

///
/// \brief Save a PBES in the format specified.
/// \param pbes The PBES to be stored
/// \param stream The stream to which the output is saved.
/// \param format Determines the format in which the result is written. If unspecified, or
///        pbes_file_unknown is specified, then a default format is chosen.
///
inline
void save_pbes(const pbes& pbes,
               std::ostream& stream,
               utilities::file_format format=utilities::file_format())
{
  if (format == utilities::file_format())
  {
    format = pbes_format_internal();
  }
  mCRL2log(log::verbose) << "Saving result in " << format.shortname() << " format..." << std::endl;
  if (format == pbes_format_internal())
  {
    pbes.save(stream, true);
  }
  else
  if (format == pbes_format_text())
  {
    stream << pp(pbes);
  }
  else
  {
    throw mcrl2::runtime_error("Trying to save PBES in non-PBES format (" + format.shortname() + ")");
  }
}

/// \brief Load a PBES from file.
/// \param pbes The PBES to which the result is loaded.
/// \param stream The stream from which to load the PBES.
/// \param format The format that should be assumed for the file in infilename. If unspecified, or
///        pbes_file_unknown is specified, then a default format is chosen.
/// \param source The source from which the stream originates. Used for error messages.
inline
void load_pbes(pbes& pbes, std::istream& stream, utilities::file_format format, const std::string& source = "")
{
  if (format == utilities::file_format())
  {
    format = pbes_format_internal();
  }
  mCRL2log(log::verbose) << "Loading PBES in " << format.shortname() << " format..." << std::endl;
  if (format == pbes_format_internal())
  {
    pbes.load(stream, true, source);
  }
  else
  if (format == pbes_format_text())
  {
    stream >> pbes;
  }
  else
  {
    throw mcrl2::runtime_error("Trying to load PBES from non-PBES format (" + format.shortname() + ")");
  }
}

/// \brief save_pbes Saves a PBES to a file.
/// \param pbes The PBES to save.
/// \param filename The file to save the PBES in.
/// \param format The format in which to save the PBES.
/// \param welltypedness_check If set to false, skips checking whether pbes is well typed before
///                            saving it to file.
///
/// The format of the file in infilename is guessed if format is not given or if it is equal to
/// utilities::file_format().
inline
void save_pbes(const pbes &pbes, const std::string &filename,
               utilities::file_format format=utilities::file_format(),
               bool welltypedness_check=true)
{
  if (welltypedness_check)
  {
    assert(pbes.is_well_typed());
  }
  if (format == utilities::file_format())
  {
    format = guess_format(filename);
  }

  if (filename.empty())
  {
    save_pbes(pbes, std::cout, format);
  }
  else
  {
    std::ofstream filestream(filename,(format.text_format()?std::ios_base::out: std::ios_base::binary));
    if (!filestream.good())
    {
      throw mcrl2::runtime_error("Could not open file " + filename);
    }
    save_pbes(pbes, filestream, format);
  }
}

/// \brief Load pbes from file.
/// \param pbes The pbes to which the result is loaded.
/// \param filename The file from which to load the PBES.
/// \param format The format in which the PBES is stored in the file.
///
/// The format of the file in infilename is guessed if format is not given or if it is equal to
/// utilities::file_format().
inline
void load_pbes(pbes& pbes,
               const std::string& filename,
               utilities::file_format format=utilities::file_format())
{
  if (format == utilities::file_format())
  {
    format = guess_format(filename);
  }
 if (filename.empty())
  {
    load_pbes(pbes, std::cin, format);
  }
  else
  {
    std::ifstream filestream(filename,(format.text_format()?std::ios_base::in: std::ios_base::binary));
    if (!filestream.good())
    {
      throw mcrl2::runtime_error("Could not open file " + filename);
    }
    load_pbes(pbes, filestream, format, core::detail::file_source(filename));
  }
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_IO_H
