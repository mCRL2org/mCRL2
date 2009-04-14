// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "boost.hpp" // precompiled headers

#include "mcrl2/new_data/detail/enum/enumerator.h"
#include "mcrl2/new_data/detail/enum/standard.h"

namespace mcrl2 {
  namespace new_data {
    namespace detail {
Enumerator *createEnumerator(ATermAppl data_spec, Rewriter *r, bool clean_up_rewriter, EnumerateStrategy strategy)
{
	switch ( strategy )
	{
		case ENUM_STANDARD:
			return new EnumeratorStandard(data_spec, r,clean_up_rewriter);
		default:
			return NULL;
	}
}
    } // namespace detail
  } // namespace new_data
} // namespace mcrl2
