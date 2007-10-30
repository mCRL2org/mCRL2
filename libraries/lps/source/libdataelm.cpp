// Author(s): Muck van Weerdenburg 
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file libdataelm.cpp

#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/mcrl22lps.h"
#include "mcrl2/lps/lin_std.h"
#include "mcrl2/data/data_specification.h"
#include "print/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "struct/libstruct.h"
#include <mcrl2/pbes/pbes.h>
#include <mcrl2/data/data_variable.h>

#ifdef __cplusplus
using namespace ::mcrl2::utilities;
#endif

using namespace lps;
using namespace pbes_expr;

//Prototype
static bool add_used_sort(ATermAppl expr, ATermIndexedSet s);

static bool add_used_sorts(ATermList exprs, ATermIndexedSet s)
{
        bool result = false;
        for(ATermList l = exprs; !ATisEmpty(l); l = ATgetNext(l))
        {
                //First add_used_sort because of potential conditional
                //evaluation of second argument of ||
                bool b = add_used_sort(ATAgetFirst(l), s);
                result = result || b;
        }
        return result;
}

static bool add_used_sort(ATermAppl expr, ATermIndexedSet s)
{
	if ( gsIsSortId(expr) )
	{
		ATbool n;
		ATindexedSetPut(s,(ATerm) expr,&n);
		return (n == ATtrue);
	} else { // gsIsSortArrow(expr)
                bool b = add_used_sorts(ATLgetArgument(expr,0),s);
		bool c = add_used_sort(ATAgetArgument(expr,1),s);
		return b || c;
	}
}

//Prototype
static bool add_used(data_expression_list l, ATermIndexedSet s);

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
		bool c = add_used(ATLgetArgument(expr,1),s);
		return b || c;
	} else if ( gsIsDataVarId(expr) )
	{
		return add_used_sort(ATAgetArgument(expr,1),s);
	}

	return false;
}

static bool add_used(data_expression expr, ATermIndexedSet s)
{
  return add_used((ATermAppl) expr, s);
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

static bool add_used(propositional_variable_instantiation pvi, ATermIndexedSet s)
{
	return add_used(pvi.parameters(),s);
}

static bool add_used(pbes_expression expr, ATermIndexedSet s)
{
	if ( is_data(expr) )
	{
		return add_used(val_arg(expr),s);
	} else if ( is_and(expr) || is_or(expr) )
	{
		bool b = add_used(lhs(expr),s);
		bool c = add_used(rhs(expr),s);
		return b || c;
	} else if ( is_forall(expr) || is_exists(expr) )
	{
		bool b = add_used(make_data_expression_list(quant_vars(expr)),s);
		bool c = add_used(quant_expr(expr),s);
		return b || c;
	} else if ( is_propositional_variable_instantiation(expr) )
	{
		return add_used(var_val(expr),s);
	}

	return false;
}

//Prototype
static bool add_sort(ATermAppl s, ATermIndexedSet used_data, ATermIndexedSet used_sorts, data_operation_list constructors);

static bool add_sorts(ATermList l, ATermIndexedSet used_data, ATermIndexedSet used_sorts, data_operation_list constructors)
{
        bool result = false;
        for(ATermList l1 = l; !ATisEmpty(l1); l1=ATgetNext(l1))
        {
                //First add_sort (because of potential conditional execution of second argument ||)
                bool b = add_sort(ATAgetFirst(l1),used_data,used_sorts,constructors);
                result = result || b;
        }
        return result;
}

static bool add_sort(ATermAppl s, ATermIndexedSet used_data, ATermIndexedSet used_sorts, data_operation_list constructors)
{
	if ( gsIsSortArrow(s) )
	{
		bool b = add_sorts(ATLgetArgument(s,0),used_data,used_sorts,constructors);
		bool c = add_sort(ATAgetArgument(s,1),used_data,used_sorts,constructors);
		return b || c;
	} else { // gsIsSortId(s)
		ATbool n,m;
		ATindexedSetPut(used_data,(ATerm) s,&n);
		ATindexedSetPut(used_sorts,(ATerm) s,&m);
	
		bool b = false;
		if ( m == ATtrue )
		{
			data_operation_list::iterator fb = constructors.begin();
			data_operation_list::iterator fe = constructors.end();
			for (; fb != fe; fb++)
			{
				lps::sort range = fb->sort().range_sort();
	
				if ( range == s )
				{
					ATbool n;
				
					ATindexedSetPut(used_data,(ATerm) ((ATermAppl) (*fb)),&n);
	
					sort_list domain = fb->sort().domain_sorts();
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

//Prototype
static bool is_used(ATermAppl exprs, ATermIndexedSet s);

static bool is_used(ATermList exprs, ATermIndexedSet s)
{
        bool result = true;
        for( ; !ATisEmpty(exprs) && result ; exprs=ATgetNext(exprs))
        {
                result = result && is_used(ATAgetFirst(exprs), s);
        }

        return result;
}

static bool is_used(ATermAppl expr, ATermIndexedSet s)
{
	if ( gsIsOpId(expr) )
	{
		return (ATindexedSetGetIndex(s,(ATerm) expr) >= 0);
	} else if ( gsIsDataAppl(expr) )
	{
		return is_used(ATAgetArgument(expr,0),s) && is_used(ATLgetArgument(expr,1),s);
	}

	return true;
}

static ATermTable initialise_used_data(data_specification dspec, bool keep_basis)
{
	ATermTable used_data = ATtableCreate(2*(dspec.sorts().size()+dspec.constructors().size()+dspec.mappings().size()),50);

	if ( keep_basis )
	{
		/* Add sorts/functions that should always be available */
		specification basis_spec = mcrl22lps("init delta;");
		data_specification data = basis_spec.data();
		for (sort_list::iterator i = data.sorts().begin(); i != data.sorts().end(); i++)
		{
			add_used_sort(*i,used_data);
			add_used(gsMakeOpIdIf(*i),used_data);
			add_used(gsMakeOpIdEq(*i),used_data);
			add_used(gsMakeOpIdNeq(*i),used_data);
		}
		for (data_operation_list::iterator i = data.constructors().begin(); i != data.constructors().end(); i++)
		{
			add_used((ATermAppl) *i,used_data);
		}
		for (data_operation_list::iterator i = data.mappings().begin(); i != data.mappings().end(); i++)
		{
			add_used((ATermAppl) *i,used_data);
		}
	}

	return used_data;
}

data_specification build_reduced_data_spec(data_specification dspec, ATermTable used_data, bool keep_basis)
{
	ATermTable used_sorts = ATtableCreate(2*dspec.sorts().size(),50);
	data_equation_list eqns = dspec.equations();
	data_equation_list::iterator ee = eqns.end();
	sort_list sorts = dspec.sorts();
	sort_list::iterator sse = sorts.end();
	data_operation_list conss = dspec.constructors();
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
				if ( keep_basis )
				{
					add_used(gsMakeOpIdIf(*ssb),used_data);
					add_used(gsMakeOpIdEq(*ssb),used_data);
					add_used(gsMakeOpIdNeq(*ssb),used_data);
				}
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
	for (sort_list::iterator i=dspec.sorts().begin(); i != dspec.sorts().end(); i++)
	{
		if ( ATindexedSetGetIndex(used_data,(ATerm) ((ATermAppl) (*i))) >= 0 )
		{
			new_sort = push_front(new_sort,*i);
		}
	}
	new_sort = reverse(new_sort);

	data_operation_list new_cons;
	for (data_operation_list::iterator i=dspec.constructors().begin(); i != dspec.constructors().end(); i++)
	{
		if ( ATindexedSetGetIndex(used_data,(ATerm) ((ATermAppl) (*i))) >= 0 )
		{
			new_cons = push_front(new_cons,*i);
		}
	}
	new_cons = reverse(new_cons);

	data_operation_list new_maps;
	for (data_operation_list::iterator i=dspec.mappings().begin(); i != dspec.mappings().end(); i++)
	{
		if ( ATindexedSetGetIndex(used_data,(ATerm) ((ATermAppl) (*i))) >= 0 )
		{
			new_maps = push_front(new_maps,*i);
		}
	}
	new_maps = reverse(new_maps);

	data_equation_list new_eqns;
	for (data_equation_list::iterator i=dspec.equations().begin(); i != dspec.equations().end(); i++)
	{
		if ( is_used((ATermAppl) (*i).lhs(),used_data) )
		{
			new_eqns = push_front(new_eqns,*i);
		}
	}
	new_eqns = reverse(new_eqns);
	
	ATtableDestroy(used_sorts);
	
	data_specification new_data(new_sort,new_cons,new_maps,new_eqns);

	return new_data;
}

specification remove_unused_data(specification spec, bool keep_basis)
{
	ATermTable used_data = initialise_used_data(spec.data(),keep_basis);

	linear_process l = spec.process();

	action_label_list::iterator ab = spec.action_labels().begin();
	action_label_list::iterator ae = spec.action_labels().end();
	for (; ab != ae; ab++)
	{
		sort_list::iterator sb = ab->sorts().begin();
		sort_list::iterator se = ab->sorts().end();
		for (; sb != se; sb++)
		{
			add_used_sort((ATermAppl) *sb,used_data);
		}
	}

	add_used(spec.initial_process().state(),used_data);

	data_variable_list::iterator vb = spec.initial_process().free_variables().begin();
	data_variable_list::iterator ve = spec.initial_process().free_variables().end();
	for (; vb != ve; vb++)
	{
		add_used_sort((ATermAppl) vb->sort(),used_data);
	}
	vb = l.free_variables().begin();
	ve = l.free_variables().end();
	for (; vb != ve; vb++)
	{
		add_used_sort((ATermAppl) vb->sort(),used_data);
	}

	summand_list::iterator b = l.summands().begin();
	summand_list::iterator e = l.summands().end();
	for (; b != e; b++)
	{
		data_variable_list::iterator sb = (*b).summation_variables().begin();
		data_variable_list::iterator se = (*b).summation_variables().end();
		for (; sb != se; sb++)
		{
			add_used_sort((ATermAppl) sb->sort(),used_data);
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

	specification new_spec = set_data_specification(spec, build_reduced_data_spec(spec.data(),used_data,keep_basis));

	ATtableDestroy(used_data);

	return new_spec;
}

pbes<> remove_unused_data(pbes<> spec, bool keep_basis)
{
	ATermTable used_data = initialise_used_data(spec.data(),keep_basis);

	add_used(spec.initial_state(),used_data);

	atermpp::set<data_variable>::iterator vb = spec.free_variables().begin();
	atermpp::set<data_variable>::iterator ve = spec.free_variables().end();
	for (; vb != ve; vb++)
	{
		add_used_sort((ATermAppl) vb->sort(),used_data);
	}

	atermpp::vector<pbes_equation>::iterator b = spec.equations().begin();
	atermpp::vector<pbes_equation>::iterator e = spec.equations().end();
	for (; b != e; b++)
	{
		data_variable_list::iterator sb = (*b).variable().parameters().begin();
		data_variable_list::iterator se = (*b).variable().parameters().end();
		for (; sb != se; sb++)
		{
			add_used_sort((ATermAppl) sb->sort(),used_data);
		}

		add_used((*b).formula(),used_data);
	}

	spec.data() = build_reduced_data_spec(spec.data(),used_data,keep_basis);

	ATtableDestroy(used_data);

	return spec;
}

ATermAppl removeUnusedData(ATermAppl ATSpec, bool keep_basis) // deprecated
{
	specification spec(ATSpec);
	spec = remove_unused_data(spec,keep_basis);
	return (ATermAppl) spec;
}
