// ======================================================================
//
// Copyright (c) 2005 TU/e
//
// ----------------------------------------------------------------------
//
// file          : libdataelm 
// date          : 06-12-2005
// version       : 0.1
//
// author(s)     : Muck van Weerdenburg  <M.J.van.Weerdenburg@tue.nl>
//
// ======================================================================

#include "lpe/specification.h"
#include "liblowlevel.h"

using namespace lpe;

static bool add_used_sort(ATermAppl expr, ATermIndexedSet s)
{
	if ( gsIsSortId(expr) )
	{
		ATbool n;
		ATindexedSetPut(s,(ATerm) expr,&n);
		return (n == ATtrue);
	} else { // gsIsSortArrow(expr)
		bool b = add_used_sort(ATAgetArgument(expr,0),s);
		bool c = add_used_sort(ATAgetArgument(expr,1),s);
		return b || c;
	}
}

static bool add_used(ATermAppl expr, ATermIndexedSet s)
{
	if ( gsIsOpId(expr) )
	{
		ATbool n;
		ATindexedSetPut(s,(ATerm) expr,&n);
		bool b = add_used_sort(ATAgetArgument(expr,1),s);
		return (n == ATtrue) || b;
	} else if ( gsIsDataAppl(expr) )
	{
		bool b = add_used(ATAgetArgument(expr,0),s);
		bool c = add_used(ATAgetArgument(expr,1),s);
		return b || c;
	} else if ( gsIsDataVarId(expr) )
	{
		return add_used_sort(ATAgetArgument(expr,1),s);
	}

	return false;
}

static bool add_used(data_expression_list l, ATermIndexedSet s)
{
	data_expression_list::iterator ib = l.begin();
	data_expression_list::iterator ie = l.end();
	bool b = false;
	for (; ib != ie; ib++)
	{
		bool c = add_used((ATermAppl) (*ib),s);
		b = b || c;
	}
	return b;
}

static bool add_sort(ATermAppl s, ATermIndexedSet used_data, ATermIndexedSet used_sorts, function_list constructors)
{
	if ( gsIsSortArrow(s) )
	{
		bool b = add_sort(ATAgetArgument(s,0),used_data,used_sorts,constructors);
		bool c = add_sort(ATAgetArgument(s,1),used_data,used_sorts,constructors);
		return b || c;
	} else { // gsIsSortId(s)
		ATbool n,m;
		ATindexedSetPut(used_data,(ATerm) s,&n);
		ATindexedSetPut(used_sorts,(ATerm) s,&m);
	
		bool b = false;
		if ( m == ATtrue )
		{
			function_list::iterator fb = constructors.begin();
			function_list::iterator fe = constructors.end();
			for (; fb != fe; fb++)
			{
				lpe::sort range = fb->range_sort();
	
				if ( range == s )
				{
					ATbool n;
				
					ATindexedSetPut(used_data,(ATerm) ((ATermAppl) (*fb)),&n);
	
					sort_list domain = fb->domain_sorts();
					sort_list::iterator db = domain.begin();
					sort_list::iterator de = domain.end();
					for (; db != de; db++)
					{
						bool c = add_sort(*db,used_data,used_sorts,constructors);
						b = b || c;
					}
				}
			}
	
		}

		return (n == ATtrue) || b;
	}
}

static bool is_used(ATermAppl expr, ATermIndexedSet s)
{
	if ( gsIsOpId(expr) )
	{
		return (ATindexedSetGetIndex(s,(ATerm) expr) >= 0);
	} else if ( gsIsDataAppl(expr) )
	{
		return is_used(ATAgetArgument(expr,0),s) && is_used(ATAgetArgument(expr,1),s);
	}

	return true;
}

ATermAppl removeUnusedData(ATermAppl ATSpec)
{
	specification spec(ATSpec);
	LPE l = spec.lpe();
	
	ATermTable used_data = ATtableCreate(2*(spec.sorts().size()+spec.constructors().size()+spec.mappings().size()),50);

	add_used(spec.initial_state(),used_data);

	data_variable_list::iterator vb = spec.initial_free_variables().begin();
	data_variable_list::iterator ve = spec.initial_free_variables().end();
	for (; vb != ve; vb++)
	{
		add_used_sort((ATermAppl) vb->type(),used_data);
	}
	vb = l.free_variables().begin();
	ve = l.free_variables().end();
	for (; vb != ve; vb++)
	{
		add_used_sort((ATermAppl) vb->type(),used_data);
	}

	summand_list::iterator b = l.summands().begin();
	summand_list::iterator e = l.summands().end();
	for (; b != e; b++)
	{
		data_variable_list::iterator sb = (*b).summation_variables().begin();
		data_variable_list::iterator se = (*b).summation_variables().end();
		for (; sb != se; sb++)
		{
			add_used_sort((ATermAppl) sb->type(),used_data);
		}

		add_used((ATermAppl) (*b).condition(),used_data);

		if ( !(*b).is_delta() )
		{
			action_list::iterator ab = (*b).actions().begin();
			action_list::iterator ae = (*b).actions().end();
			for (; ab != ae; ab++)
			{
				add_used((*ab).arguments(),used_data);
			}
		}

		if ( (*b).has_time() )
		{
			add_used((ATermAppl) (*b).time(),used_data);
		}

		data_assignment_list::iterator nb = (*b).assignments().begin();
		data_assignment_list::iterator ne = (*b).assignments().end();
		for (; nb != ne; nb++)
		{
			add_used((ATermAppl) (*nb).rhs(),used_data);
		}
	}

	ATermTable used_sorts = ATtableCreate(2*spec.sorts().size(),50);
	data_equation_list eqns = spec.equations();
	data_equation_list::iterator ee = eqns.end();
	sort_list sorts = spec.sorts();
	sort_list::iterator sse = sorts.end();
	function_list conss = spec.constructors();
	bool not_done = true;
	while ( not_done )
	{
		not_done = false;

		sort_list::iterator ssb = sorts.begin();
		for (; ssb != sse; ssb++)
		{
			if ( ATindexedSetGetIndex(used_data,(ATerm) ((ATermAppl) (*ssb))) >= 0 )
			{
				bool b = add_sort((ATermAppl) (*ssb),used_data,used_sorts,conss);
				not_done = not_done || b;
			}
		}

		data_equation_list::iterator eb = eqns.begin();
		for (; eb != ee; eb++)
		{
			if ( is_used((ATermAppl) (*eb).lhs(),used_data) )
			{
				bool b = add_used((ATermAppl) (*eb).condition(),used_data);
				bool c = add_used((ATermAppl) (*eb).rhs(),used_data);
				not_done = not_done || b || c;
			}
		}
	}

	sort_list new_sort;
	for (sort_list::iterator i=spec.sorts().begin(); i != spec.sorts().end(); i++)
	{
		if ( ATindexedSetGetIndex(used_data,(ATerm) ((ATermAppl) (*i))) >= 0 )
		{
			new_sort = push_front(new_sort,*i);
		}
	}
	new_sort = reverse(new_sort);

	function_list new_cons;
	for (function_list::iterator i=spec.constructors().begin(); i != spec.constructors().end(); i++)
	{
		if ( ATindexedSetGetIndex(used_data,(ATerm) ((ATermAppl) (*i))) >= 0 )
		{
			new_cons = push_front(new_cons,*i);
		}
	}
	new_cons = reverse(new_cons);

	function_list new_maps;
	for (function_list::iterator i=spec.mappings().begin(); i != spec.mappings().end(); i++)
	{
		if ( ATindexedSetGetIndex(used_data,(ATerm) ((ATermAppl) (*i))) >= 0 )
		{
			new_maps = push_front(new_maps,*i);
		}
	}
	new_maps = reverse(new_maps);

	data_equation_list new_eqns;
	for (data_equation_list::iterator i=spec.equations().begin(); i != spec.equations().end(); i++)
	{
		if ( is_used((ATermAppl) (*i).lhs(),used_data) )
		{
			new_eqns = push_front(new_eqns,*i);
		}
	}
	new_eqns = reverse(new_eqns);
	
	specification new_spec(new_sort,new_cons,new_maps,new_eqns,spec.action_labels(),spec.lpe(),spec.initial_free_variables(),spec.initial_variables(),spec.initial_state());

	ATtableDestroy(used_sorts);
	ATtableDestroy(used_data);

	return (ATermAppl) new_spec;
}
