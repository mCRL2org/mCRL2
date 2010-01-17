// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/alphabet_reduction.h
/// \brief Function for applying the alphabet axioms to a process specification.

#ifndef MCRL2_PROCESS_ALPHABET_REDUCTION_H
#define MCRL2_PROCESS_ALPHABET_REDUCTION_H

#include <iostream>
#include <string>
#include <sstream>
#include "mcrl2/process/process_specification.h"

namespace mcrl2 {

  namespace process {

    /// \brief     Applies alphabet reduction to a process specification.
    /// \param[in,out] p An mCRL2 process specificiation that has been
    ///            type checked.
    /// \post      Alphabet reductions have been applied to p.
    /// \exception mcrl2::runtime_error Alphabet reduction failed. 
    void apply_alphabet_reduction(process_specification& p);

  } // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_ALPHABET_REDUCTION_H

