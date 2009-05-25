// Author(s): Alexander van Dam
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_rewrite.cpp
/// \brief Add your file description here.

#include "boost.hpp" // precompiled headers

#include "pbes_rewrite.h"
#include "mcrl2/pbes/utility.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/atermpp/substitute.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/pbes/find.h"

using namespace mcrl2::data;
using namespace mcrl2::data::detail;
using namespace mcrl2::pbes_system;
using namespace mcrl2::pbes_system::pbes_expr;
using namespace mcrl2::pbes_system::accessors;
using namespace mcrl2::core;

// parameterised boolean expression
//<PBExpr>       ::= <DataExpr>
//                 | PBESTrue
//                 | PBESFalse
//                 | PBESAnd(<PBExpr>, <PBExpr>)
//                 | PBESOr(<PBExpr>, <PBExpr>)
//                 | PBESForall(<DataVarId>+, <PBExpr>)
//                 | PBESExists(<DataVarId>+, <PBExpr>)
//                 | <PropVarInst>
pbes_expression pbes_expression_rewrite(pbes_expression p, data_specification data, Rewriter *rewriter)
{
	pbes_expression result;

	if (is_true(p))
	{ // p is True
		result = p;
	}
	else if (is_false(p))
	{ // p is False
		result = p;
	}
	else if (is_and(p))
	{ // p = and(left, right)
		//Rewrite left and right as far as possible
		pbes_expression l = pbes_expression_rewrite(left(p), data, rewriter);
		pbes_expression r = pbes_expression_rewrite(right(p), data, rewriter);
		//Options for left and right
		if (is_false(l) || is_false(r))
			result = false_();
		else if (is_true(l))
			result = r;
		else if (is_true(r))
			result = l;
		else if (l == r)
			result = l;
		else
			result = and_(l,r);
	}
	else if (is_or(p))
	{ // p = or(left, right)
		//Rewrite left and right as far as possible
		pbes_expression l = pbes_expression_rewrite(left(p), data, rewriter);
		pbes_expression r = pbes_expression_rewrite(right(p), data, rewriter);
		//Options for left and right
		if (is_true(l) || is_true(r))
			result = true_();
		else if (is_false(l))
			result = r;
		else if (is_false(r))
			result = l;
		else if (l == r)
			result = r;
		else
			result = or_(l,r);
	}
	else if (is_forall(p))
	{ // p = forall(data_expression_list, pbes_expression)
		variable_list data_vars = var(p);
		pbes_expression expr = pbes_expression_rewrite(arg(p), data, rewriter);

		//Remove data_vars which does not occur in expr
		variable_list occured_data_vars;
		for (variable_list::iterator i = data_vars.begin(); i != data_vars.end(); i++)
		{
			if (occurs_inL(expr, *i)) // The var occurs in expr
				occured_data_vars = push_front(occured_data_vars, *i);
		}

		// If no data_vars
		if (occured_data_vars.empty())
			result = expr;
		else
		{
			data_vars = occured_data_vars;
			if (is_true(expr) || is_false(expr))
				result = expr;

			//If the forall  has only finite data variables, make a conjunction out of it.
			else if (data.is_certainly_finite(mcrl2::data::make_sort_range(data_vars)))
			{
				pbes_expression_list and_list = get_and_expressions(get_all_possible_expressions(data_vars, expr, data), data, rewriter);
				result = join_and(and_list.begin(), and_list.end());
			}
			else if (has_propvarinsts(find_all_propositional_variable_instantiations(expr), data_vars))
			{
				gsErrorMsg("Quantor expression contains a data variable of the quantor in a propositional variable instantiation\n");
				gsErrorMsg("Cannot rewrite such an expression\n");
				gsErrorMsg("Aborting\n");
				exit(1);
			}
			else
				result = pbes_expr::forall(data_vars, expr);
		}
	}
	else if (is_exists(p))
	{ // p = exists(data_expression_list, pbes_expression)
		variable_list data_vars = var(p);
		pbes_expression expr = pbes_expression_rewrite(arg(p), data, rewriter);

		//Remove data_vars which does not occur in expr
		variable_list occured_data_vars;
		for (variable_list::iterator i = data_vars.begin(); i != data_vars.end(); i++)
		{
			if (occurs_inL(expr, *i)) // The var occurs in expr
				occured_data_vars = push_front(occured_data_vars, *i);
		}

		//If no data_vars remaining
		if (occured_data_vars.empty())
			result = expr;
		else
		{
			data_vars = occured_data_vars;
			if (is_true(expr) || is_false(expr))
				result = expr;
			//If the exists  has only finite data variables, make a conjunction out of it.
			else if (data.is_certainly_finite(mcrl2::data::make_sort_range(data_vars)))
			{
				pbes_expression_list or_list = get_or_expressions(get_all_possible_expressions(data_vars, expr, data), data, rewriter);
				result = join_or(or_list.begin(), or_list.end());
			}
			else if (has_propvarinsts(find_all_propositional_variable_instantiations(expr), data_vars))
			{
				gsErrorMsg("Quantor expression contains a data variable of the quantor in a propositional variable instantiation\n");
				gsErrorMsg("Cannot rewrite such an expression\n");
				gsErrorMsg("Aborting\n");
				exit(1);
			}
			else
				result = pbes_expr::exists(data_vars, expr);
		}
	}
	else if (is_propositional_variable_instantiation(p))
	{ // p is a propositional variable
		propositional_variable_instantiation propvar = p;
		identifier_string name = propvar.name();
                data_expression_list propvar_parameters(propvar.parameters());
		data_expression_list parameters(atermpp::term_list_iterator< mcrl2::data::data_expression >(
                                         rewriter->rewriteList(atermpp::term_list< mcrl2::data::data_expression >(propvar_parameters.begin(), propvar_parameters.end()))),
                                                atermpp::term_list_iterator< mcrl2::data::data_expression >());
		result = pbes_expression(propositional_variable_instantiation(name, parameters));
	}
	else
	{ // p is a data_expression
		data_expression d(rewriter->rewrite(p));
		if (d == sort_bool_::true_())
			result = true_();
		else if (d == sort_bool_::false_())
			result = false_();
		else
			result = val(d);
	}

	return result;
}

bool has_propvarinsts(std::set< propositional_variable_instantiation > propvars, variable_list data_vars)
{
	bool result = false;
	for (std::set< propositional_variable_instantiation >::iterator pvi = propvars.begin(); pvi != propvars.end(); pvi++)
	{
		for (data_expression_list::iterator del = pvi->parameters().begin(); del != pvi->parameters().end(); del++)
		{
			for (variable_list::iterator dvl = data_vars.begin(); dvl != data_vars.end(); dvl++)
			{
				if (data_expression(*del).is_variable())
				{
					if (variable(*del) == *dvl)
						result = true;
				}
			}
		}
	}
	return result;
}

bool element_in_propvarinstlist(variable_list vars, std::set< propositional_variable_instantiation > pvilist)
{
	bool result = false;
	for (std::set< propositional_variable_instantiation >::iterator pvi = pvilist.begin(); pvi != pvilist.end(); pvi++)
	{
		for (variable_list::iterator dvl = vars.begin(); dvl != vars.end(); dvl++)
		{
			if (occurs_inL(*dvl, *pvi))
			{
				result = true;
			}
		}
	}
	return result;
}

///\return variable v occurs in l.
bool occurs_inL(atermpp::aterm_appl l, variable v)
{
  return atermpp::find_if(l, mcrl2::data::detail::compare_variable(v)) != atermpp::aterm_appl();
}

pbes_expression_list get_all_possible_expressions(variable_list data_vars, pbes_expression pbexp, data_specification data)
{
	// Create a pbes_expression for each possible instantiations of the variables and put those in a list.
	atermpp::set< pbes_expression > set_result;
	set_result.insert(pbexp);
	for (variable_list::iterator vars = data_vars.begin(); vars != data_vars.end(); vars++)
	{
		atermpp::set< pbes_expression > intermediate;
		for (atermpp::set< pbes_expression >::iterator exp = set_result.begin(); exp != set_result.end(); exp++)
		{
			data_expression_vector enumerations = enumerate_constructors(data, vars->sort());
			for (data_expression_vector::iterator enums = enumerations.begin(); enums != enumerations.end(); enums++)
			{
				pbes_expression toAdd = *exp;
				toAdd = toAdd.substitute(make_substitution(*vars, *enums));
				intermediate.insert(toAdd);
			}
		}
		set_result = intermediate;
	}
	pbes_expression_list result;
	for (atermpp::set< pbes_expression >::iterator exp = set_result.begin(); exp != set_result.end(); exp++)
	{
		result = push_front(result, *exp);
	}

	return reverse(result);
}

pbes_expression_list get_and_expressions(pbes_expression_list and_list, data_specification data, Rewriter *rewriter)
{
	// From a pbes_expression_list: Remove all expressions which are true.
	// Return a list with only one element False, if an element in the original list is false.
	pbes_expression_list result;
	bool is_pbes_false = false;

	pbes_expression_list::iterator it_enum = and_list.begin();
	while ((it_enum != and_list.end()) && (!is_pbes_false))
	{
		pbes_expression current = pbes_expression_rewrite(*it_enum, data, rewriter);
		if (is_false(current))
		{
			pbes_expression_list list_false;
			list_false = push_front(list_false, false_());
			result = list_false;
			is_pbes_false = true;
		}
		else if (!is_true(current))
			result = push_front(result, current);
		it_enum++;
	}
	return reverse(result);
}

pbes_expression_list get_or_expressions(pbes_expression_list or_list, data_specification data, Rewriter *rewriter)
{
	// From a pbes_expression_list: Remove all expressions which are false.
	// Return a list with only one element False, if an element in the original list is true.
	pbes_expression_list result;
	bool is_pbes_true = false;

	pbes_expression_list::iterator it_enum = or_list.begin();
	while ((it_enum != or_list.end()) && (!is_pbes_true))
	{
		pbes_expression current = pbes_expression_rewrite(*it_enum, data, rewriter);
		if (is_true(current))
		{
			pbes_expression_list list_true;
			list_true = push_front(list_true, true_());
			result = list_true;
			is_pbes_true = true;
		}
		else if (!is_false(current))
			result = push_front(result, current);
		it_enum++;
	}
	return reverse(result);
}
