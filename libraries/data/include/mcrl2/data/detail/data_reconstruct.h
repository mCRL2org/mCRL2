// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/data_reconstruct.h
///
/// \brief Reconstruct data types from mCRL2 specification and expressions.


#ifndef MCRL2_DATA_RECONSTRUCT_H
#define MCRL2_DATA_RECONSTRUCT_H

#include <aterm2.h>
#include "mcrl2/core/detail/struct.h"

namespace mcrl2 {
  namespace data {
    namespace detail {

/// \brief     Reconstruct data types of an mCRL2 expression after data
///            implementation with respect to a type checked mCRL2
///            specification.
/// \param[in] expr An ATerm representation of an mCRL2 expression that
///            adheres to the internal ATerm structure after data
///            implementation.
/// \param[in] spec An ATerm representation of an mCRL2 process specification,
///            LPS, PBES or data specification that adheres to the internal
///            ATerm structure after data implementation, or NULL.
/// \post      The data types of expr are reconstructed with respect to spec.
/// \return    If data reconstruction went well, an equivalent version
///            of expr is returned that adheres to the internal ATerm
///            structure after type checking.
///            If something went wrong, an appropriate error message is printed
///            and NULL is returned.
ATerm reconstruct_exprs(ATerm expr, const ATermAppl spec = NULL);

/// \brief     Reconstruct data types of an mCRL2 specification
///            after data implementation.
/// \param[in] spec An ATerm representation of an mCRL2 process specification,
///            LPS, PBES or data specification that adheres to the internal
///            ATerm structure after data implementation.
/// \post      The data types of spec are reconstructed.
/// \return    If data reconstruction went well, an equivalent version
///            of spec is returned that adheres to the internal ATerm
///            structure after type checking.
///            If something went wrong, an appropriate error
///            message is printed and NULL is returned.
inline ATermAppl reconstruct_spec(ATermAppl spec) {
  assert(mcrl2::core::detail::gsIsProcSpec(spec) ||
         mcrl2::core::detail::gsIsLinProcSpec(spec) ||
         mcrl2::core::detail::gsIsPBES(spec) ||
	 mcrl2::core::detail::gsIsDataSpec(spec));
  return (ATermAppl) reconstruct_exprs((ATerm) spec, spec);
}

    }
  }
}

#endif

