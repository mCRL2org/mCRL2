// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_reconstruct.h
///
/// \brief This file contains the public interface of the data reconstruction.


#ifndef MCRL2_DATA_RECONSTRUCT_H
#define MCRL2_DATA_RECONSTRUCT_H

#include <aterm2.h>
#include "mcrl2/core/struct.h"

namespace mcrl2 {

  namespace core {

    namespace detail {
/// \brief Reconstructs as much of expr as possible to the mcrl2 format.
/// \pre expr adheres to the internal format after data implementation
///       spec, if provided, adheres to the internal format after data
///       implementation.
ATerm reconstruct_exprs(ATerm expr, const ATermAppl spec = NULL);

/// \brief Reconstruct spec (in the internal format after data implementation)
///         to the mcrl2 format.
inline ATermAppl reconstruct_spec(ATermAppl spec) {
  assert(gsIsSpecV1(spec) || gsIsPBES(spec) || gsIsDataSpec(spec));
  return (ATermAppl) reconstruct_exprs((ATerm) spec, spec);
}
    }
   
  }

}

#endif

