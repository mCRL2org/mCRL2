// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_elimination.cpp

#include "boost.hpp" // precompiled headers

#include <mcrl2/data/data_expression.h>
#include <mcrl2/data/data_elimination.h>
#include <mcrl2/lps/specification.h>
#include <mcrl2/lps/data_elimination.h>

using namespace mcrl2::data;

namespace mcrl2
{
namespace lps
{

static void add_used(data_expression_list l, data_elimination &elim)
{
	for (data_expression_list::iterator i = l.begin(); i != l.end(); ++i)
	{
		elim.keep_data_from_expr(*i);
	}
}

specification remove_unused_data(specification spec, bool keep_basis)
{
	data_elimination elim;

	if ( keep_basis )
	{
		elim.keep_standard_data();
	}

	linear_process l = spec.process();

	action_label_list::iterator ab = spec.action_labels().begin();
	action_label_list::iterator ae = spec.action_labels().end();
	for (; ab != ae; ab++)
	{
		sort_expression_list::iterator sb = ab->sorts().begin();
		sort_expression_list::iterator se = ab->sorts().end();
		for (; sb != se; sb++)
		{
			elim.keep_sort(*sb);
		}
	}

	add_used(spec.initial_process().state(),elim);

	variable_list::iterator vb = spec.initial_process().free_variables().begin();
	variable_list::iterator ve = spec.initial_process().free_variables().end();
	for (; vb != ve; vb++)
	{
		elim.keep_sort(vb->sort());
	}
	vb = l.free_variables().begin();
	ve = l.free_variables().end();
	for (; vb != ve; vb++)
	{
		elim.keep_sort(vb->sort());
	}

	summand_list::iterator b = l.summands().begin();
	summand_list::iterator e = l.summands().end();
	for (; b != e; b++)
	{
		variable_list::iterator sb = (*b).summation_variables().begin();
		variable_list::iterator se = (*b).summation_variables().end();
		for (; sb != se; sb++)
		{
			elim.keep_sort(sb->sort());
		}

		elim.keep_data_from_expr((*b).condition());

		if ( !(*b).is_delta() )
		{
			action_list::iterator ab = (*b).actions().begin();
			action_list::iterator ae = (*b).actions().end();
			for (; ab != ae; ab++)
			{
				add_used((*ab).arguments(),elim);
			}
		}

		if ( (*b).has_time() )
		{
			elim.keep_data_from_expr((*b).time());
		}

		assignment_list::iterator nb = (*b).assignments().begin();
		assignment_list::iterator ne = (*b).assignments().end();
		for (; nb != ne; nb++)
		{
			elim.keep_data_from_expr((*nb).rhs());
		}
	}

	spec.data() = elim.apply(spec.data());
	return spec;
}

}
}

ATermAppl removeUnusedData(ATermAppl ATSpec, bool keep_basis) // deprecated
{
	mcrl2::lps::specification spec(ATSpec);
	spec = mcrl2::lps::remove_unused_data(spec,keep_basis);
	return specification_to_aterm(spec);
}
