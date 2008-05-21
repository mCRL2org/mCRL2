// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file libdataelm.h

#include <aterm2.h>
#include <mcrl2/lps/specification.h>
#include <mcrl2/pbes/pbes.h>

using namespace mcrl2::lps;
using namespace mcrl2::pbes_system;

specification remove_unused_data(specification spec, bool keep_basis = true);
pbes<> remove_unused_data(pbes<> spec, bool keep_basis = true);

ATermAppl removeUnusedData(ATermAppl Spec, bool keep_basis = true); // deprecated
