#include "pbes_rewrite.h"
#include "atermpp/substitute.h"

#include "sort_functions.h"

using namespace lps;
using namespace pbes_expr;

// parameterized boolean expression
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
		pbes_expression left = pbes_expression_rewrite(lhs(p), data, rewriter);
		pbes_expression right = pbes_expression_rewrite(rhs(p), data, rewriter);
		//Options for left and right
		if (left.is_false() || right.is_false())
			result = false_();
		else if (left.is_true())
			result = right;
		else if (right.is_true())
			result = left;
		else 
			result = and_(left,right);
	}
	else if (is_or(p))
	{ // p = or(left, right)
		//Rewrite left and right as far as possible
		pbes_expression left = pbes_expression_rewrite(lhs(p), data, rewriter);
		pbes_expression right = pbes_expression_rewrite(rhs(p), data, rewriter);
		//Options for left and right
		if (left.is_true() || right.is_true())
			result = true_();
		else if (left.is_false())
			result = right;
		else if (right.is_false())
			result = left;
		else 
			result = or_(left,right);
	}
	else if (is_forall(p))
	{ // p = forall(data_expression_list, pbes_expression)
		data_variable_list data_vars = quant_vars(p);
		pbes_expression expr = pbes_expression_rewrite(quant_expr(p), data, rewriter);
		//If expression is true or false -> return it
		if (expr.is_true() || expr.is_false())
			result = expr;
		//If the forall  has only finite data variables, make a conjunction out of it.
		else if (check_finite_list(data.constructors(), get_sorts(data_vars)))
		{
			pbes_expression_list and_list = get_and_expressions(get_all_possible_expressions(data_vars, expr, data), data, rewriter);
			result = multi_and(and_list.begin(), and_list.end());
		}
		else
			//Probably some advanced stuff is needed here to check finiteness...
			result = forall(data_vars, expr);
	}
	else if (is_exists(p))
	{ // p = exists(data_expression_list, pbes_expression)
		data_variable_list data_vars = quant_vars(p);
		pbes_expression expr = pbes_expression_rewrite(quant_expr(p), data, rewriter);
		//If expression is true or false -> return it
		if (expr.is_true() || expr.is_false())
			result = expr;
		//If the exists  has only finite data variables, make a conjunction out of it.
		else if (check_finite_list(data.constructors(), get_sorts(data_vars)))
		{
			pbes_expression_list or_list = get_or_expressions(get_all_possible_expressions(data_vars, expr, data), data, rewriter);
			result = multi_or(or_list.begin(), or_list.end());
		}
		else 
			//Probably some advanced stuff is needed here to check finiteness...
			result = exists(data_vars, expr);
	}
	else if (is_propositional_variable_instantiation(p))
	{ // p is a propositional variable
		propositional_variable_instantiation propvar = p;
		identifier_string name = propvar.name();
		data_expression_list parameters = rewriter->rewriteList(propvar.parameters());
		result = pbes_expression(propositional_variable_instantiation(name, parameters));
	}
	else
	{ // p is a data_expression
		data_expression d = rewriter->rewrite(p);
		if (d.is_true())
			result = true_();
		else if (d.is_false())
			result = false_();
		else
			result = val(d);
	}
	
	return result;
}

pbes_expression_list get_all_possible_expressions(data_variable_list data_vars, pbes_expression pbexp, data_specification data)
{
	// Create a pbes_expression for each possible instantiations of the variables and put those in a list.
	pbes_expression_list result;	
	result = push_front(result, pbexp);
	for (data_variable_list::iterator vars = data_vars.begin(); vars != data_vars.end(); vars++)
	{
		pbes_expression_list intermediate;
		for (pbes_expression_list::iterator exp = result.begin(); exp != result.end(); exp++)
		{
			data_expression_list enumerations = enumerate_constructors(data.constructors(), vars->sort());
			for (data_expression_list::iterator enums = enumerations.begin(); enums != enumerations.end(); enums++)
			{
				pbes_expression toAdd = pbexp.substitute(make_substitution(*vars, *enums));
				intermediate = push_front(intermediate, toAdd);
			}
		}
		result = reverse(intermediate);
	}
	return result;
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

