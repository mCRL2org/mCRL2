// Author(s): Jan Friso Groote. Based on bes/io.h
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/res/io.h
/// \brief add your file description here.

#ifndef MCRL2_RES_IO_H
#define MCRL2_RES_IO_H

#include "mcrl2/res/res_equation_system.h"
#include "mcrl2/utilities/file_utility.h"
#include "mcrl2/pres/pres.h"

namespace mcrl2
{

namespace res
{

const std::vector<utilities::file_format>& res_file_formats();

inline
const utilities::file_format& res_format_internal() { return res_file_formats()[0]; }

inline
utilities::file_format guess_format(const std::string& filename)
{
  for (const utilities::file_format& fmt: res_file_formats())
  {
    if (fmt.matches(filename))
    {
      return fmt;
    }
  }
  return utilities::file_format();
}

/// \brief Save a RES in the format specified.
/// \param res The res to be stored
/// \param stream The name of the file to which the output is stored.
/// \param format Determines the format in which the result is written.
void save_res(const res_equation_system& res,
              std::ostream& stream,
              utilities::file_format format);

/// \brief Load res from a stream.
/// \param res The res to which the result is loaded.
/// \param stream The file from which to load the RES.
/// \param format The format that should be assumed for the stream.
/// \param source The source from which the stream originates. Used for error messages.
void load_res(res_equation_system& res, std::istream& stream, utilities::file_format format, const std::string& source = "");

///
/// \brief save_res Saves a RES to a file.
/// \param res The RES to save.
/// \param filename The file to save the RES in.
/// \param format The format in which to save the RES.
///
void save_res(const res_equation_system& res,
              const std::string& filename,
              utilities::file_format format = utilities::file_format());

/// \brief Loads a RES from a file.
/// \param res The object in which the result is stored.
/// \param filename The file from which to load the RES.
/// \param format An indication of the file format. If this is file_format() the
///        format of the file in infilename is guessed.
void load_res(res_equation_system& res,
              const std::string& filename,
              utilities::file_format format = utilities::file_format());

/// \brief Loads a PRES from a file. If the file stores a RES, then it is converted to a PRES.
/// \param pres The object in which the result is stored.
/// \param filename The file from which to load the PRES.
/// \param format An indication of the file format. If this is file_format() the
///        format of the file in infilename is guessed.
void load_pres(pres_system::pres& pres,
               const std::string& filename,
               utilities::file_format format = utilities::file_format());

/// \brief Saves a PRES to a stream. If the PRES is not a RES and a RES file format is requested, an
///        exception is thrown.
/// \param pres The object in which the PRES is stored.
/// \param stream The stream which to save the PRES to.
/// \param format The file format to store the PRES in.
///
/// This function converts the pres_system::pres to a res_equation_system if the requested file
/// format does not provide a save routine for pres_system::pres structures.
void save_pres(const pres_system::pres& pres,
               std::ostream& stream,
               const utilities::file_format& format = utilities::file_format());

/// \brief Saves a PRES to a file. If the PRES is not a RES and a RES file format is requested, an
///        exception is thrown.
/// \param pres The object in which the PRES is stored.
/// \param filename The file which to save the PRES to.
/// \param format The file format to store the PRES in.
///
/// The format of the file in infilename is guessed.
void save_pres(const pres_system::pres& pres,
               const std::string& filename,
               utilities::file_format format = utilities::file_format());

/// \brief Writes the res to a stream.
atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const res_equation_system& res);

/// \brief Reads the res from a stream.
atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, res_equation_system& res);

} // namespace res

} // namespace mcrl2

#endif // MCRL2_RES_IO_H
