// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/prover/utilities.h
/// \brief Miscellaneous utility functions

#ifndef UTILITIES_H
#define UTILITIES_H

#include <cmath>

#include "mcrl2/core/detail/struct_core.h"

namespace mcrl2
{
namespace data
{
namespace detail
{

/**
 * \brief Converts to textual representation for a boolean
 * \param[in] b the boolean to convert
 * \note for writing to stream consider using std::boolalpha
 **/
inline const char* bool_to_char_string(bool b)
{
  return (b) ? "true" : "false";
}

}
}
}

#endif
