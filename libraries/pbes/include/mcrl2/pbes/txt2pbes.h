// Author(s): Aad Mathijssen, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/txt2pbes.h
/// \brief Function for parsing a pbes specification.

#ifndef MCRL2_PBES_TXT2PBES_H
#define MCRL2_PBES_TXT2PBES_H

#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/utilities/logger.h"
#include <sstream>

namespace mcrl2
{

namespace pbes_system
{

/// \brief Parses a PBES specification from an input stream
/// \param spec_stream A stream from which can be read
/// \param normalize  If true, the resulting PBES is normalized after reading.
/// \return The parsed PBES
inline
pbes txt2pbes(std::istream& spec_stream, bool normalize = true)
{
  pbes result;
  spec_stream >> result;
  if (normalize)
  {
    mCRL2log(log::verbose) << "normalizing the PBES ..." << std::endl;
    pbes_system::algorithms::normalize(result);
  }
  return result;
}

/// \brief Parses a PBES specification from a string
/// \param text A string
/// \param normalize  If true, the resulting PBES is normalized after reading.
/// \return The parsed PBES
inline
pbes txt2pbes(const std::string& text, bool normalize = true)
{
  std::stringstream from(text);
  return txt2pbes(from, normalize);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TXT2PBES_H
