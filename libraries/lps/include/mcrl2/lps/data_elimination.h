// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/data_elimination.h
/// \brief This file provides data elimination for Linear Process
///        Specifications.

#ifndef MCRL2_LPS_DATA_ELIMINATION_H
#define MCRL2_LPS_DATA_ELIMINATION_H

#include <aterm2.h>
#include "mcrl2/lps/specification.h"

namespace mcrl2
{
namespace lps
{

/// \brief                Remove all unused data parts from the given LPS. That
///                       is, if a sort or function is not directly or
///                       indirectly used in the process definition, it does
///                       not occur in resulting specification.
/// \param[in] spec       An mCRL2 Linear Process Specification.
/// \param[in] keep_basis Boolean to indicate whether or not to preserve
///                       standard data.
specification remove_unused_data(lps::specification & spec, bool keep_basis = true);

}
}

#endif
