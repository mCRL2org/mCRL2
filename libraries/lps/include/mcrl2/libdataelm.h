// Author(s): Muck van Weerdenburg
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file libdataelm.h

#include <aterm2.h>
#include <mcrl2/lps/specification.h>
#include <mcrl2/pbes/pbes.h>

lps::specification remove_unused_data(lps::specification spec, bool keep_basis = true);
lps::pbes remove_unused_data(lps::pbes spec, bool keep_basis = true);

ATermAppl removeUnusedData(ATermAppl Spec, bool keep_basis = true); // deprecated
