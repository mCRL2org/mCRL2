// Author(s): Wieger Wesselink, Alexander van Dam, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/file_formats.h
/// \brief File format routines for (p)besses.

#ifndef MCRL2_PBES_FILE_FORMATS_H
#define MCRL2_PBES_FILE_FORMATS_H

#include <utility>
#include <iostream>
#include <string>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/exception.h"

namespace mcrl2
{

namespace pbes_system
{

/// \brief Supported BES and PBES file formats.
enum pbes_file_format
{
  pbes_file_pbes,
  pbes_file_bes,
  pbes_file_cwi,
  pbes_file_pgsolver,
  pbes_file_unknown
};

/// \brief Transform string to output format
inline
std::string file_format_to_string(const pbes_file_format f)
{
  const std::string formats[] = {"pbes", "bes", "cwi", "pgsolver", "unknown"};
  return formats[f];
}

/// \brief Parse output format
inline
pbes_file_format file_format_from_string(const std::string& s)
{
  if(s == "bes")
  {
    return pbes_file_bes;
  }
  else if(s == "cwi")
  {
    return pbes_file_cwi;
  }
  else if (s == "pgsolver")
  {
    return pbes_file_pgsolver;
  }
  else if (s == "pbes")
  {
    return pbes_file_pbes;
  }
  else
  {
    throw mcrl2::runtime_error("Unknown (P)BES format \"" + s + "\" specified.");
  }
}

/// \brief Stream operator for file format
/// \param is An input stream
/// \param t A file format
/// \return The input stream
template <typename Tool>
inline
std::istream& operator>>(std::istream& is, pbes_system::pbes_file_format& t)
{
  std::string s;
  is >> s;
  try
  {
    t = file_format_from_string(s);
  }
  catch (std::runtime_error)
  {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

/// \brief Guess the file format of the file in filename.
/// \param filename The file of which to guess the format.
inline
pbes_file_format guess_format(const std::string& filename)
{
  std::string::size_type pos = filename.find_last_of('.');

  if (pos != std::string::npos)
  {
    std::string ext = filename.substr(pos+1);

    if (ext == "bes")
    {
      mCRL2log(log::verbose) << "Detected BES extension.\n";
      return pbes_file_bes;
    }
    else if (ext == "cwi")
    {
      mCRL2log(log::verbose) << "Detected CWI extension.\n";
      return pbes_file_cwi;
    }
    else if (ext == "pg" || ext == "gm")
    {
      mCRL2log(log::verbose) << "Detected PGSolver extension.\n";
      return pbes_file_pgsolver;
    }
    else if (ext == "pbes")
    {
      mCRL2log(log::verbose) << "Detected PBES extension.\n";
      return pbes_file_pbes;
    }
    else
    {
      mCRL2log(log::verbose) << "Failed to determine file format from file name. "
                             << " The detected extension was " << ext << std::endl
                             << "; defaulting to PBES.";

      return pbes_file_pbes;
    }
  }
  return pbes_file_pbes;
}

} // pbes_system
} // mcrl2

#endif // MCRL2_PBES_FILE_FORMATS_H
