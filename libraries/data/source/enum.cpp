// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "boost.hpp" // precompiled headers

#include "mcrl2/data/rewrite.h"
#include "mcrl2/data/enum.h"
#include "mcrl2/data/detail/enum/standard.h"

EnumeratorSolutions::~EnumeratorSolutions()
{
}

Enumerator::~Enumerator()
{
}

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
