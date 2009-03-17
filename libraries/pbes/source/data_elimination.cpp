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

#include <mcrl2/new_data/data_expression.h>
#include <mcrl2/new_data/data_elimination.h>
#include <mcrl2/pbes/pbes.h>
#include <mcrl2/pbes/data_elimination.h>

using namespace mcrl2::new_data;
using namespace mcrl2::pbes_system::pbes_expr;
using namespace mcrl2::pbes_system::accessors;

namespace mcrl2
{
namespace pbes_system
{

static void add_used(data_expression_list l, data_elimination &elim)
{
	for (data_expression_list::iterator i = l.begin(); i != l.end(); ++i)
	{
		elim.keep_data_from_expr(*i);
	}
}

static void add_used(propositional_variable_instantiation pvi, data_elimination &elim)
{
	add_used(pvi.parameters(),elim);
}

static void add_used(pbes_expression expr, data_elimination &elim)
{
	if ( is_data(expr) )
	{
		elim.keep_data_from_expr(val(expr));
	} else if ( is_and(expr) || is_or(expr) )
	{
		add_used(left(expr),elim);
		add_used(right(expr),elim);
	} else if ( is_forall(expr) || is_exists(expr) )
	{
		add_used(var(expr),elim);
		add_used(arg(expr),elim);
	} else if ( is_propositional_variable_instantiation(expr) )
	{
		add_used(param(expr),elim);
	}
}

pbes<> remove_unused_data(pbes<> spec, bool keep_basis)
{
	data_elimination elim;

	if ( keep_basis )
	{
		elim.keep_standard_data();
	}

	add_used(spec.initial_state(),elim);

	atermpp::set<variable>::iterator vb = spec.free_variables().begin();
	atermpp::set<variable>::iterator ve = spec.free_variables().end();
	for (; vb != ve; vb++)
	{
		elim.keep_sort(vb->sort());
	}

	atermpp::vector<pbes_equation>::iterator b = spec.equations().begin();
	atermpp::vector<pbes_equation>::iterator e = spec.equations().end();
	for (; b != e; b++)
	{
		variable_list::iterator sb = (*b).variable().parameters().begin();
		variable_list::iterator se = (*b).variable().parameters().end();
		for (; sb != se; sb++)
		{
			elim.keep_sort(sb->sort());
		}

		add_used((*b).formula(),elim);
	}

	spec.data() = elim.apply(spec.data());

	return spec;
}

}
}
