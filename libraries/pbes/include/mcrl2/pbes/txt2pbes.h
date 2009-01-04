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

#include <sstream>
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_parse.h"

namespace mcrl2 {

namespace pbes_system {

  /// \brief Parses a PBES specification.
  /// \param text A string
  /// \return The parsed PBES
  inline
  pbes<> txt2pbes(const std::string& text)
  {
    std::stringstream from(text);
    pbes<> result;
    from >> result;
    try {
      result.normalize();
    }
    catch (std::exception& /* e */)
    {
      throw mcrl2::runtime_error("PBES is not monotonic");
    }
    return result;
  }

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TXT2PBES_H
