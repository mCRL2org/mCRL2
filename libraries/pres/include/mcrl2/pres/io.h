// Author(s): Jan Friso Groote. Based on io.h for pbesses.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/io.h
/// \brief IO routines for boolean equation systems.

#ifndef MCRL2_PRES_IO_H
#define MCRL2_PRES_IO_H

#include "mcrl2/pres/pres.h"
#include "mcrl2/utilities/file_utility.h"



namespace mcrl2::pres_system
{

const std::vector<utilities::file_format>& pres_file_formats();

inline
bool is_pres_file_format(const utilities::file_format& format)
{
  for (const utilities::file_format& i: pres_file_formats())
  {
    if (i == format)
    {
      return true;
    }
  }
  return false;
}

inline
const utilities::file_format& pres_format_internal() { return pres_file_formats()[0]; }
inline
const utilities::file_format& pres_format_text() { return pres_file_formats()[1]; }

inline
const utilities::file_format guess_format(const std::string& filename)
{
  for (const utilities::file_format& it : pres_file_formats())
  {
    if (it.matches(filename))
    {
      return it;
    }
  }
  return utilities::file_format();
}

/// \brief Save a PRES in the format specified.
/// \param pres The PRES to be stored
/// \param stream The stream to which the output is saved.
/// \param format Determines the format in which the result is written. If unspecified, or
///        pres_file_unknown is specified, then a default format is chosen.
void save_pres(const pres& pres,
               std::ostream& stream,
               utilities::file_format format = utilities::file_format());

/// \brief Load a PRES from file.
/// \param pres The PRES to which the result is loaded.
/// \param stream The stream from which to load the PRES.
/// \param format The format that should be assumed for the file in infilename. If unspecified, or
///        pres_file_unknown is specified, then a default format is chosen.
/// \param source The source from which the stream originates. Used for error messages.
void load_pres(pres& pres, std::istream& stream, utilities::file_format format, const std::string& source = "");

/// \brief save_pres Saves a PRES to a file.
/// \param pres The PRES to save.
/// \param filename The file to save the PRES in.
/// \param format The format in which to save the PRES.
/// \param welltypedness_check If set to false, skips checking whether pres is well typed before
///                            saving it to file.
///
/// The format of the file in infilename is guessed if format is not given or if it is equal to
/// utilities::file_format().
void save_pres(const pres& pres, const std::string& filename,
               utilities::file_format format = utilities::file_format(),
               bool welltypedness_check = true);

/// \brief Load pres from file.
/// \param pres The pres to which the result is loaded.
/// \param filename The file from which to load the PRES.
/// \param format The format in which the PRES is stored in the file.
///
/// The format of the file in infilename is guessed if format is not given or if it is equal to
/// utilities::file_format().
void load_pres(pres& pres,
               const std::string& filename,
               utilities::file_format format = utilities::file_format());

} // namespace mcrl2::pres_system



#endif // MCRL2_PRES_IO_H
