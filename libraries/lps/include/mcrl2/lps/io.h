// Author(s): Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/io.h
/// \brief IO routines for linear process specifications.

#ifndef MCRL2_LPS_IO_H
#define MCRL2_LPS_IO_H

#include <utility>
#include <iostream>
#include <string>
#include "mcrl2/utilities/file_utility.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2
{

namespace lps
{

inline
const std::vector<utilities::file_format>& lps_file_formats()
{
  static std::vector<utilities::file_format> result;
  if (result.empty())
  {
    result.push_back(utilities::file_format("lps", "LPS in internal format", false));
    result.back().add_extension(".lps");
    result.push_back(utilities::file_format("lps_text", "LPS in internal textual format", true));
    result.back().add_extension(".aterm");
    result.push_back(utilities::file_format("text", "LPS in textual (mCRL2) format", true));
    result.back().add_extension(".txt");
  }
  return result;
}

inline
const utilities::file_format* lps_format_internal() { return &lps_file_formats()[0]; }
inline
const utilities::file_format* lps_format_internal_text() { return &lps_file_formats()[1]; }
inline
const utilities::file_format* lps_format_text() { return &lps_file_formats()[2]; }

inline
const utilities::file_format* guess_format(const std::string& filename)
{
  for (const utilities::file_format& it: lps_file_formats())
  {
    if (it.matches(filename))
    {
      return &it;
    }
  }
  return utilities::file_format::unknown();
}

///
/// \brief Save an LPS in the format specified.
/// \param spec The LPS to be stored
/// \param stream The stream to which the output is written.
/// \param format Determines the format in which the result is written. If unspecified, or
///        utilities::file_format::unknown() is specified, then a default format is chosen.
///
template <typename Specification>
void save_lps(const Specification& spec, std::ostream& stream,
               const utilities::file_format* format=utilities::file_format::unknown())
{
  if (format == utilities::file_format::unknown())
  {
    format = lps_format_internal();
  }
  mCRL2log(log::verbose) << "Saving LPS in " << format->shortname() << " format..." << std::endl;
  if (format == lps_format_internal())
  {
    spec.save(stream, true);
  }
  else
  if (format == lps_format_internal_text())
  {
    spec.save(stream, false);
  }
  else
  if (format == lps_format_text())
  {
    stream << pp(spec);
  }
  else
  {
    throw mcrl2::runtime_error("Trying to save LPS in non-LPS format (" + format->shortname() + ")");
  }
}

/// \brief Load LPS from file.
/// \param spec The LPS to which the result is loaded.
/// \param stream The stream from which to load the LPS (it is assumed to have been opened in the
///               right mode).
/// \param f The format that should be assumed for the file in infilename.
/// \param source The source from which the stream originates. Used for error messages.
template <typename Specification>
void load_lps(Specification& spec, std::istream& stream, const utilities::file_format* format, const std::string& source = "")
{
  if (format == utilities::file_format::unknown())
  {
    format = lps_format_internal();
  }
  mCRL2log(log::verbose) << "Loading LPS in " << format->shortname() << " format..." << std::endl;
  if (format == lps_format_internal())
  {
    spec.load(stream, true, source);
  }
  else
  if (format == lps_format_internal_text())
  {
    spec.load(stream, false, source);
  }
  else
  if (format == lps_format_text())
  {
    parse_lps(stream, spec);
  }
  else
  {
    throw mcrl2::runtime_error("Trying to load LPS from non-LPS format (" + format->shortname() + ")");
  }
}

/// \brief Saves an LPS to a file.
/// \param spec The LPS to save.
/// \param filename The file to save the LPS in.
/// \param format The format in which to save the LPS.
template <typename Specification>
void save_lps(const Specification &spec, const std::string &filename,
               const utilities::file_format* format=utilities::file_format::unknown())
{
  if (format == utilities::file_format::unknown())
  {
    format = guess_format(filename);
  }
  utilities::output_file file = format->open_output(filename);
  save_lps(spec, file.stream(), format);
  file.stream() << std::flush;
}

/// \brief Load LPS from file.
/// \param spec The LPS to which the result is loaded.
/// \param filename The file from which to load the LPS.
/// \param format The format in which the LPS is stored in the file.
/// \param source The source from which the stream originates. Used for error messages.
///
/// The format of the file in filename is guessed if format is not given, or if it is equal to
/// utilities::file_format::unknown().
template <typename Specification>
void load_lps(Specification& spec, const std::string& filename, const utilities::file_format* format=utilities::file_format::unknown())
{
  if (format == utilities::file_format::unknown())
  {
    format = guess_format(filename);
  }
  utilities::input_file file = format->open_input(filename);
  load_lps(spec, file.stream(), format, core::detail::file_source(filename));
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_IO_H
