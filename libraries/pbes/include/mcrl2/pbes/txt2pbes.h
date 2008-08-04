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
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/data_implementation.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2 {

namespace pbes_system {

  /// Parses a pbes specification.
  /// \param text The text that is parsed.
  /// \result The resulting pbes.
  inline
  pbes<> txt2pbes(const std::string& text)
  {
    std::stringstream from(text);
    ATermAppl result = core::parse_pbes_spec(from);
    if (result == NULL) {
      throw mcrl2::runtime_error("parsing failed");
    }

    result = core::type_check_pbes_spec(result);
    if (result == NULL) {
      throw mcrl2::runtime_error("type checking failed");
    }

    result = core::implement_data_pbes_spec(result);
    if (result == NULL) {
      throw mcrl2::runtime_error("data implementation failed");
    }

    return pbes<>(result);
  }

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TXT2PBES_H
