// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LPS_SYMBOLIC_LTS_IO_H
#define MCRL2_LPS_SYMBOLIC_LTS_IO_H

#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/lps/symbolic_lts.h"

namespace mcrl2::lps
{

/// \brief Writes symbolic LTS to the stream.
std::ostream& operator<<(std::ostream& stream, const symbolic_lts& lts);

/// \brief Reads symbolic LTS from the stream.
std::istream& operator>>(std::istream& stream, symbolic_lts& lts);

} // namespace mcrl2::lps

#endif // MCRL2_ENABLE_SYLVAN

#endif // MCRL2_LPS_SYMBOLIC_LTS_IO_H