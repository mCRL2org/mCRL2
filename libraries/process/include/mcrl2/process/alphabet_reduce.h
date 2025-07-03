// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/alphabet_reduce.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_ALPHABET_REDUCE_H
#define MCRL2_PROCESS_ALPHABET_REDUCE_H

#include "mcrl2/process/process_specification.h"

namespace mcrl2::process
{

/// \brief Applies alphabet reduction to a process specification.
/// \param procspec A process specification
/// \param duplicate_equation_limit If the number of equations is less than
/// duplicate_equation_limit, the remove duplicate equations procedure is applied.
/// Note that this procedure is not efficient, so it should not be used if the number
/// of equations is big.
void alphabet_reduce(process_specification& procspec, std::size_t duplicate_equation_limit = (std::numeric_limits<size_t>::max)());

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_ALPHABET_REDUCE_H
