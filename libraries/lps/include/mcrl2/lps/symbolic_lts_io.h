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

#include "mcrl2/data/data_specification.h"
#include "mcrl2/utilities/bitstream.h"

#include <sylvan_ldd.hpp>

namespace mcrl2::lps
{

class symbolic_lts
{
public:
  data::data_specification data_spec;

  sylvan::ldds::ldd states;

};

/// \brief Writes symbolic LTS to the stream.
std::ostream& operator<<(std::ostream& stream, const symbolic_lts& lts);

/// \brief Reads symbolic LTS from the stream.
std::istream& operator>>(std::istream& stream, symbolic_lts& lts);

} // namespace mcrl2::lps

#endif // MCRL2_LPS_SYMBOLIC_LTS_IO_H