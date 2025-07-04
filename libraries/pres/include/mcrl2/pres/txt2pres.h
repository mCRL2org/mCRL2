// Author(s): Jan Friso Groote. Based on pbes/txt2pbes.h by Aad Mathijssen, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/txt2pres.h
/// \brief Function for parsing a pres specification.

#ifndef MCRL2_PRES_TXT2PRES_H
#define MCRL2_PRES_TXT2PRES_H

#include "mcrl2/pres/algorithms.h"
#include "mcrl2/pres/io.h"
#include "mcrl2/pres/parse.h"

namespace mcrl2::pres_system
{

/// \brief Parses a PRES specification from an input stream
/// \param spec_stream A stream from which can be read
/// \param normalize  If true, the resulting PRES is normalized after reading.
/// \return The parsed PRES
inline
pres txt2pres(std::istream& spec_stream, bool normalize = true)
{
  pres result;
  spec_stream >> result;
  if (normalize)
  {
    mCRL2log(log::verbose) << "normalizing the PRES ..." << std::endl;
    pres_system::algorithms::normalize(result);
  }
  return result;
}

/// \brief Parses a PRES specification from a string
/// \param text A string
/// \param normalize  If true, the resulting PRES is normalized after reading.
/// \return The parsed PRES
inline
pres txt2pres(const std::string& text, bool normalize = true)
{
  std::stringstream from(text);
  return txt2pres(from, normalize);
}

} // namespace mcrl2::pres_system



#endif // MCRL2_PRES_TXT2PRES_H
