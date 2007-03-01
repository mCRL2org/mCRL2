#include "sort_functions.h"

#include "lps/sort_utility.h"

using namespace lps;
using namespace std;

sort_list get_sorts(data_variable_list v)
{
	sort_list result;
	for (data_variable_list::iterator i = v.begin(); i != v.end(); i++)
	{
		result = push_back(result, i->sort());
	}
	return reverse(result);
}

bool check_finite(function_list fl, lps::sort s)
{
	return is_finite(fl, s);
}

bool check_finite_list(function_list fl, sort_list s)
{
	return is_finite(fl, s);
}

data_expression_list enumerate_constructors(function_list fl, lps::sort s)
{
	// All datasorts which are taken into account must be finite. Normally this is the case, because a check on finiteness is done in create_bes
	assert(is_finite(fl, s));
	// The resulting data_expression_list.
	data_expression_list ces;
	// Get all constructors of sort s
	function_list cl = get_constructors(fl,s);
	// For each constructor of sort s...
	for (function_list::iterator i = cl.begin(); i != cl.end(); i++)
	{
		// Get all domains of the constructor
		sort_list domains = i->domain_sorts();
		// Vector for all enumerated constructors
		vector< data_expression_list > argumentss;
		// For each sort of the constructor...
		for (sort_list::iterator j = domains.begin(); j != domains.end(); j++)
		{
			// Put all values which the sort can have in a list
			argumentss.push_back(enumerate_constructors(fl,*j));
		}
		// Create data_expression_list out of the values which a sort can have
		data_expression_list temp = create_data_expression_list(*i, argumentss);
		//concatenate ces and temp
		for (data_expression_list::iterator k = temp.begin(); k != temp.end(); k++)
		{
			ces = push_front(ces, *k);
		}
	}
	// Put ces in the correct order
	ces = reverse(ces);
	return ces;
}

data_expression_list create_data_expression_list(function f, vector< data_expression_list > dess)
{
	// Result list
	data_expression_list result;
	// At first put function f in result
	result = push_front(result, data_expression(f));
	for (vector< data_expression_list >::iterator i = dess.begin(); i != dess.end(); i++)
	{
		//*i is a list of constructor expressions that should be applied to the elements of result
		data_expression_list tmp;
		for (data_expression_list::iterator k = i->begin(); k != i->end(); k++)
		//*k is a constructor expression that should be applied to the elements of result
		{
			for (data_expression_list::iterator j = result.begin(); j != result.end(); j++)
			{
				// *j is a data expression
				//  apply *j to *k
				//  One of the constructor values is applied to f
				tmp = push_front(tmp, data_expression(data_application(*j, *k)));
			}
		}
		// Next iteration replace all values which are created in tmp
		result = tmp;
	}
	return result;
}
