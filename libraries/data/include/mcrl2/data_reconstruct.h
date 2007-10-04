// Author(s): Jeroen Keiren
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_reconstruct.h
//
//This file contains the public interface of the data reconstruction.


#ifndef MCRL2_DATA_RECONSTRUCT_H
#define MCRL2_DATA_RECONSTRUCT_H

#include <aterm2.h>
#include "libstruct.h"

ATerm reconstruct_exprs(ATerm expr, const ATermAppl spec = NULL);

inline ATermAppl reconstruct_spec(ATermAppl spec) {
  assert(gsIsSpecV1(spec));
  return (ATermAppl) reconstruct_exprs((ATerm) spec, spec);
}

#endif

