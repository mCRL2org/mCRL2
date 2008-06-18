// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/data_elimination.h
/// \brief This file provides data elimination for Parametrised Boolean
///        Equation System specifications.

#ifndef MCRL2_PBES_DATA_ELIMINATION_H
#define MCRL2_PBES_DATA_ELIMINATION_H

#include <mcrl2/pbes/pbes.h>

namespace mcrl2
{
namespace pbes_system
{

/// \brief                Remove all unused data parts from the given PBES. That
///                       is, if a sort or function is not directly or
///                       indirectly used in the process definition, it does
///                       not occur in resulting specification.
/// \param[in] spec       An mCRL2 Parametrised Boolean Equation System
///                       specification.
/// \param[in] keep_basis Boolean to indicate whether or not to preserve
///                       standard data.
pbes<> remove_unused_data(pbes<> spec, bool keep_basis = true);

}
}

#endif
