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

#include <mcrl2/core/data_implementation.h>
#include <mcrl2/core/detail/struct.h>
#include <mcrl2/core/messaging.h>
#include <mcrl2/core/aterm_ext.h>
#include <mcrl2/data/data_specification.h>
#include <mcrl2/data/sort.h>
#include <mcrl2/data/data_operation.h>
#include <mcrl2/data/data_variable.h>
#include <mcrl2/data/data_elimination.h>

using namespace mcrl2::core;
using namespace mcrl2::core::detail;

using namespace mcrl2::data;


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
				sort_expression range = result_sort(fb->sort());
	
				if ( range == s )
				{
					ATbool n;
				
					ATindexedSetPut(used_data,(ATerm) ((ATermAppl) (*fb)),&n);
	
					sort_expression_list domain = domain_sorts(fb->sort());
					sort_expression_list::iterator db = domain.begin();
					sort_expression_list::iterator de = domain.end();
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

static void add_basis(ATermTable used_data)
{
	/* Add sorts/functions that should always be available */
	data_specification data(implement_data_data_spec(gsMakeEmptyDataSpec()));
	for (sort_expression_list::iterator i = data.sorts().begin(); i != data.sorts().end(); i++)
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

static data_specification build_reduced_data_spec(data_specification dspec, ATermTable used_data, bool keep_basis)
{
	ATermTable used_sorts = ATtableCreate(2*dspec.sorts().size(),50);
	data_equation_list eqns = dspec.equations();
	data_equation_list::iterator ee = eqns.end();
	sort_expression_list sorts = dspec.sorts();
	sort_expression_list::iterator sse = sorts.end();
	data_operation_list conss = dspec.constructors();
	bool not_done = true;
	while ( not_done )
	{
		not_done = false;

		sort_expression_list::iterator ssb = sorts.begin();
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

	sort_expression_list new_sort;
	for (sort_expression_list::iterator i=dspec.sorts().begin(); i != dspec.sorts().end(); i++)
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


data_elimination::data_elimination()
{
  used_data = ATtableCreate(2000,50);
  keep_basis = false;
}

data_elimination::~data_elimination()
{
  ATtableDestroy(used_data);
}

void data_elimination::keep_sort(sort_expression s)
{
  add_used_sort((ATermAppl) s, used_data);
}

void data_elimination::keep_function(data_operation op)
{
  add_used((ATermAppl) op, used_data);
}

void data_elimination::keep_standard_data()
{
  add_basis(used_data);
  keep_basis = true;
}

void data_elimination::keep_data_from_expr(data_expression expr)
{
  add_used(expr, used_data);
}

data_specification data_elimination::apply(data_specification spec)
{
  return build_reduced_data_spec(spec,used_data,keep_basis);
}
