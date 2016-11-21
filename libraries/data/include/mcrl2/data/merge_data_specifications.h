// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/merge_data_specifications.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_MERGE_DATA_SPECIFICATIONS_H
#define MCRL2_DATA_MERGE_DATA_SPECIFICATIONS_H

#include "mcrl2/data/data_specification.h"

namespace mcrl2 {

namespace data {

/// \brief Merges two data specifications. Throws an exception if conflicts are detected.
inline
data_specification merge_data_specifications(const data_specification& dataspec1, const data_specification& dataspec2)
{
  data_specification result = dataspec1 + dataspec2;
  // TODO: how can we check if there was a conflict???
  return result;
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_MERGE_DATA_SPECIFICATIONS_H
