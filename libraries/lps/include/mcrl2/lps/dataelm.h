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

mcrl2::lps::specification remove_unused_data(mcrl2::lps::specification spec, bool keep_basis = true);
mcrl2::pbes_system::pbes<> remove_unused_data(mcrl2::pbes_system::pbes<> spec, bool keep_basis = true);

ATermAppl removeUnusedData(ATermAppl Spec, bool keep_basis = true); // deprecated
