// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/io.h
/// \brief add your file description here.

#ifndef MCRL2_BES_IO_H
#define MCRL2_BES_IO_H

#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/pbesinst_conversion.h"
#include "mcrl2/bes/bes2pbes.h"
#include "mcrl2/bes/pg_parse.h"
#include "mcrl2/bes/parse.h"

#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/io.h"

#include <fstream>

namespace mcrl2
{

namespace bes
{

inline
const utilities::file_format* bes_file_formats(size_t i)
{
  static std::vector<utilities::file_format> result;
  if (result.empty())
  {
    result.push_back(utilities::file_format("bes", "BES in internal format", false));
    result.back().add_extension(".bes");
    result.push_back(utilities::file_format("bes_text", "BES in internal textual format", true));
    result.back().add_extension(".aterm");
    result.push_back(utilities::file_format("cwi", "BES in CWI format", true));
    result.back().add_extension(".cwi");
    result.push_back(utilities::file_format("pgsolver", "BES in PGSolver format", true));
    result.back().add_extension(".gm");
    result.back().add_extension(".pg");
  }
  if (i < result.size())
  {
    return &result[i];
  }
  return nullptr;
}

inline
const utilities::file_format* bes_format_internal() { return bes_file_formats(0); }
inline
const utilities::file_format* bes_format_internal_text() { return bes_file_formats(1); }
inline
const utilities::file_format* bes_format_cwi() { return bes_file_formats(2); }
inline
const utilities::file_format* bes_format_pgsolver() { return bes_file_formats(3); }

inline
const utilities::file_format* guess_format(const std::string& filename)
{
  const utilities::file_format* fmt;
  size_t i;
  for (i = 0, fmt = bes_file_formats(i); fmt != nullptr; fmt = bes_file_formats(++i))
  {
    if (fmt->matches(filename))
    {
      return fmt;
    }
  }
  return utilities::file_format::unknown();
}

void save_bes_pgsolver(const boolean_equation_system& bes, std::ostream& stream, bool maxpg=true);

void save_bes_cwi(const boolean_equation_system& bes, std::ostream& stream);

void save_bes_cwi(const pbes_system::pbes& bes, std::ostream& stream);

/// \brief Save a BES in the format specified.
/// \param bes The bes to be stored
/// \param stream The name of the file to which the output is stored.
/// \param format Determines the format in which the result is written.
/// \param aterm_ascii Determines, if output_format is bes, whether the file
///        is written is ascii format.
inline
void save_bes(const boolean_equation_system& bes, std::ostream& stream,
              const utilities::file_format* format)
{
  if (format == utilities::file_format::unknown())
  {
    format = bes_format_internal();
  }
  mCRL2log(log::verbose) << "Saving result in " << format->shortname() << " format..." << std::endl;
  if (format == bes_format_internal())
  {
    bes.save(stream, true);
  }
  else
  if (format == bes_format_internal_text())
  {
    bes.save(stream, false);
  }
  else
  if (format == bes_format_cwi())
  {
    save_bes_cwi(bes, stream);
  }
  else
  if (format == bes_format_pgsolver())
  {
    save_bes_pgsolver(bes, stream);
  }
  else
  if (format == pbes_system::pbes_format_text())
  {
    stream << bes;
  }
  else
  if (pbes_system::is_pbes_file_format(format))
  {
    save_pbes(bes2pbes(bes), stream, format);
  }
  else
  {
    throw mcrl2::runtime_error("Trying to save BES in non-BES format (" + format->shortname() + ")");
  }
}

/// \brief Load bes from a stream.
/// \param bes The bes to which the result is loaded.
/// \param stream The file from which to load the BES.
/// \param format The format that should be assumed for the stream.
/// \param source The source from which the stream originates. Used for error messages.
inline
void load_bes(boolean_equation_system& bes, std::istream& stream, const utilities::file_format* format, const std::string& source = "")
{
  if (format == utilities::file_format::unknown())
  {
    format = bes_format_internal();
  }
  mCRL2log(log::verbose) << "Loading BES in " << format->shortname() << " format..." << std::endl;
  if (format == bes_format_internal())
  {
    bes.load(stream, true);
  }
  else
  if (format == bes_format_internal_text())
  {
    bes.load(stream, false);
  }
  else
  if (format == bes_format_cwi())
  {
    throw mcrl2::runtime_error("Loading a BES from a CWI file is not implemented");
  }
  else
  if (format == bes_format_pgsolver())
  {
    parse_pgsolver(stream, bes);
  }
  else
  if (format == pbes_system::pbes_format_text())
  {
    stream >> bes;
  }
  else
  if (pbes_system::is_pbes_file_format(format))
  {
    pbes_system::pbes pbes;
    load_pbes(pbes, stream, format, source);
    if(!pbes_system::algorithms::is_bes(pbes))
    {
      throw mcrl2::runtime_error("The PBES that was loaded is not a BES");
    }
    bes = bes::pbesinst_conversion(pbes);
  }
  else
  {
    throw mcrl2::runtime_error("Trying to load BES from non-BES format (" + format->shortname() + ")");
  }
}

///
/// \brief save_bes Saves a BES to a file.
/// \param bes The BES to save.
/// \param filename The file to save the BES in.
/// \param format The format in which to save the BES.
///
inline
void save_bes(const boolean_equation_system &bes, const std::string &filename,
              const utilities::file_format* format=utilities::file_format::unknown())
{
  if (format == utilities::file_format::unknown())
  {
    format = guess_format(filename);
  }
  utilities::output_file file = format->open_output(filename);
  save_bes(bes, file.stream(), format);
}

/// \brief Loads a BES from a file.
/// \param bes The object in which the result is stored.
/// \param filename The file from which to load the BES.
///
/// The format of the file in infilename is guessed.
inline
void load_bes(boolean_equation_system& bes, const std::string& filename, const utilities::file_format* format=utilities::file_format::unknown())
{
  if (format == utilities::file_format::unknown())
  {
    format = guess_format(filename);
  }
  utilities::input_file file = format->open_input(filename);
  load_bes(bes, file.stream(), format, core::detail::file_source(filename));
}

/// \brief Loads a PBES from a file. If the file stores a BES, then it is converted to a PBES.
/// \param pbes The object in which the result is stored.
/// \param filename The file from which to load the PBES.
///
/// The format of the file in infilename is guessed.
inline
void load_pbes(pbes_system::pbes& pbes, const std::string& filename,
               const utilities::file_format* format=utilities::file_format::unknown())
{
  if (format == utilities::file_format::unknown())
  {
    format = pbes_system::guess_format(filename);
    if (format == utilities::file_format::unknown())
    {
      format = guess_format(filename);
    }
  }
  if (pbes_system::is_pbes_file_format(format))
  {
    pbes_system::load_pbes(pbes, filename, format);
    return;
  }
  boolean_equation_system bes;
  load_bes(bes, filename, format);
  pbes = bes2pbes(bes);
}

/// \brief Saves a PBES to a stream. If the PBES is not a BES and a BES file format is requested, an
///        exception is thrown.
/// \param pbes The object in which the PBES is stored.
/// \param stream The stream which to save the PBES to.
/// \param format The file format to store the PBES in.
///
/// This function converts the pbes_system::pbes to a boolean_equation_system if the requested file
/// format does not provide a save routine for pbes_system::pbes structures.
inline
void save_pbes(const pbes_system::pbes& pbes, std::ostream& stream,
               const utilities::file_format* format=utilities::file_format::unknown())
{
  if (pbes_system::is_pbes_file_format(format) || format == utilities::file_format::unknown())
  {
    pbes_system::save_pbes(pbes, stream, format);
  }
  else
  {
    if (pbes_system::algorithms::is_bes(pbes))
    {
      if (format == bes_format_cwi())
      {
        save_bes_cwi(pbes, stream);
      }
      else
      {
        save_bes(pbesinst_conversion(pbes), stream, format);
      }
    }
    else
    {
      throw mcrl2::runtime_error("Trying to save a PBES with data parameters as a BES.");
    }
  }
}

/// \brief Saves a PBES to a file. If the PBES is not a BES and a BES file format is requested, an
///        exception is thrown.
/// \param pbes The object in which the PBES is stored.
/// \param filename The file which to save the PBES to.
/// \param format The file format to store the PBES in.
///
/// The format of the file in infilename is guessed.
inline
void save_pbes(const pbes_system::pbes& pbes, const std::string& filename,
               const utilities::file_format* format=utilities::file_format::unknown())
{
  if (format == utilities::file_format::unknown())
  {
    format = guess_format(filename);
  }
  utilities::output_file file = format->open_output(filename);
  bes::save_pbes(pbes, file.stream(), format);
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_IO_H
