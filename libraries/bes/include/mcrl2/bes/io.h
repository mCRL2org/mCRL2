// Author(s): anonymous
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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
#include "mcrl2/utilities/file_utility.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2
{

namespace bes
{

const std::vector<utilities::file_format>& bes_file_formats();

inline
const utilities::file_format& bes_format_internal() { return bes_file_formats()[0]; }
inline
const utilities::file_format& bes_format_pgsolver() { return bes_file_formats()[1]; }

inline
utilities::file_format guess_format(const std::string& filename)
{
  for (const utilities::file_format fmt: bes_file_formats())
  {
    if (fmt.matches(filename))
    {
      return fmt;
    }
  }
  return utilities::file_format();
}

// Implemented in pgsolver.cpp
void save_bes_pgsolver(const boolean_equation_system& bes, std::ostream& stream, bool maxpg=true);

/// \brief Save a BES in the format specified.
/// \param bes The bes to be stored
/// \param stream The name of the file to which the output is stored.
/// \param format Determines the format in which the result is written.
void save_bes(const boolean_equation_system& bes,
              std::ostream& stream,
              utilities::file_format format);

/// \brief Load bes from a stream.
/// \param bes The bes to which the result is loaded.
/// \param stream The file from which to load the BES.
/// \param format The format that should be assumed for the stream.
/// \param source The source from which the stream originates. Used for error messages.
void load_bes(boolean_equation_system& bes, std::istream& stream, utilities::file_format format, const std::string& source = "");

///
/// \brief save_bes Saves a BES to a file.
/// \param bes The BES to save.
/// \param filename The file to save the BES in.
/// \param format The format in which to save the BES.
///
void save_bes(const boolean_equation_system& bes,
              const std::string& filename,
              utilities::file_format format = utilities::file_format());

/// \brief Loads a BES from a file.
/// \param bes The object in which the result is stored.
/// \param filename The file from which to load the BES.
/// \param format An indication of the file format. If this is file_format() the
///        format of the file in infilename is guessed.
void load_bes(boolean_equation_system& bes,
              const std::string& filename,
              utilities::file_format format = utilities::file_format());

/// \brief Loads a PBES from a file. If the file stores a BES, then it is converted to a PBES.
/// \param pbes The object in which the result is stored.
/// \param filename The file from which to load the PBES.
/// \param format An indication of the file format. If this is file_format() the
///        format of the file in infilename is guessed.
void load_pbes(pbes_system::pbes& pbes,
               const std::string& filename,
               utilities::file_format format = utilities::file_format());

/// \brief Saves a PBES to a stream. If the PBES is not a BES and a BES file format is requested, an
///        exception is thrown.
/// \param pbes The object in which the PBES is stored.
/// \param stream The stream which to save the PBES to.
/// \param format The file format to store the PBES in.
///
/// This function converts the pbes_system::pbes to a boolean_equation_system if the requested file
/// format does not provide a save routine for pbes_system::pbes structures.
void save_pbes(const pbes_system::pbes& pbes,
               std::ostream& stream,
               const utilities::file_format& format = utilities::file_format());

/// \brief Saves a PBES to a file. If the PBES is not a BES and a BES file format is requested, an
///        exception is thrown.
/// \param pbes The object in which the PBES is stored.
/// \param filename The file which to save the PBES to.
/// \param format The file format to store the PBES in.
///
/// The format of the file in infilename is guessed.
void save_pbes(const pbes_system::pbes& pbes,
               const std::string& filename,
               utilities::file_format format = utilities::file_format());

/// \brief Writes the bes to a stream.
atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const boolean_equation_system& bes);

/// \brief Reads the bes from a stream.
atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, boolean_equation_system& bes);

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_IO_H
