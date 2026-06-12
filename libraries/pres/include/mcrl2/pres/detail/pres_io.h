// Author(s): Jan Friso Groote. Based on pbes/detail/pbes_io.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_PRES_DETAIL_PRES_IO_H
#define MCRL2_PRES_DETAIL_PRES_IO_H

#include "mcrl2/pres/pres.h"

#include <fstream>

namespace mcrl2::pres_system
{

/// \brief Writes the pres to a stream.
atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const pres& pres);

/// \brief Reads a pres from a stream.
atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, pres& pres);

namespace detail
{

/// \brief Loads a PRES from filename, or from stdin if filename equals "".
pres load_pres(const std::string& filename);

/// \brief Saves an PRES to filename, or to stdout if filename equals "".
void save_pres(const pres& presspec, const std::string& filename);

} // namespace detail

} // namespace mcrl2::pres_system

#endif // MCRL2_PRES_DETAIL_PRES_IO_H
