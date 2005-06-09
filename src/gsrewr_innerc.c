/* $Id: gsrewr_innerc.c,v 1.3 2005/04/14 11:57:17 muck Exp $ */

#define NAME "rewr_innerc"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"
#include "gsrewr_innerc.h"
#include "libgsparse.h"

extern ATermList opid_eqns;
extern ATermList dataappl_eqns;

static ATermTable term2int;
static unsigned int num_opids;
static ATermAppl *int2term;
static ATermList *innerc_eqns;
static int true_num;
static void (*so_rewr_init)();
static ATerm (*so_rewr)(ATerm);

static bool ATisList(ATerm a)
{
	return (ATgetType(a) == AT_LIST);
}

static bool ATisInt(ATerm a)
{
	return (ATgetType(a) == AT_INT);
}

static ATerm OpId2Int(ATermAppl Term, bool add_opids)
{
	ATermInt i;

	if ( (i = (ATermInt) ATtableGet(term2int,(ATerm) Term)) == NULL )
	{
		if ( !add_opids )
		{
			return (ATerm) Term;
		}
		i = ATmakeInt(num_opids);
//ATfprintf(stderr,"%i := %p (%t)\n\n",num_opids,Term,Term);
		ATtablePut(term2int,(ATerm) Term,(ATerm) i);
		num_opids++;
	}

	return (ATerm) i;
}

static ATerm toInner(ATermAppl Term, bool add_opids)
{
	ATermList l;

	if ( !gsIsDataAppl(Term) )
	{
		if ( gsIsOpId(Term) )
		{
			return (ATerm) OpId2Int(Term,add_opids);
		} else {
			return (ATerm) Term;
		}
	}

	l = ATmakeList0();
	while ( gsIsDataAppl(Term) )
	{
		l = ATinsert(l,(ATerm) toInner(ATAgetArgument((ATermAppl) Term,1),add_opids));
		Term = ATAgetArgument(Term,0);
	}
	if ( gsIsOpId(Term) )
	{
		l = ATinsert(l,(ATerm) OpId2Int(Term,add_opids));
	} else {
		l = ATinsert(l,(ATerm) Term);
	}

	return (ATerm) l;
}

static ATermAppl fromInner(ATerm Term)
{
	ATermList l;
	ATerm t;
	ATermAppl a;

	if ( !ATisList(Term) )
	{
		if ( ATisInt(Term) )
		{
//ATfprintf(stderr,"%i -> %p (%t)\n\n",ATgetInt((ATermInt) Term),int2term[ATgetInt((ATermInt) Term)],int2term[ATgetInt((ATermInt) Term)]);
			return int2term[ATgetInt((ATermInt) Term)];
		} else {
			return (ATermAppl) Term;
		}
	}

	if ( ATisEmpty((ATermList) Term) )
	{
		ATfprintf(stderr,"%s: invalid inner format term (%t)\n",NAME,Term);
		exit(1);
	}
	
	l = (ATermList) Term;
	t = ATgetFirst(l);
	if ( ATisInt(t) )
	{
		a = int2term[ATgetInt((ATermInt) t)];
	} else {
		a = (ATermAppl) t;
	}
	l = ATgetNext(l);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		a = gsMakeDataAppl(a,fromInner(ATgetFirst(l)));
	}

	return a;
}

static char *whitespace_str = NULL;
static int whitespace_len;
static int whitespace_pos;
static char *whitespace(int len)
{
	int i;

	if ( whitespace_str == NULL )
	{
		whitespace_str = (char *) malloc((2*len+1)*sizeof(char));
		for (i=0; i<2*len; i++)
		{
			whitespace_str[i] = ' ';
		}
		whitespace_len = 2*len;
		whitespace_pos = len;
		whitespace_str[whitespace_pos] = 0;
	} else {
		if ( len > whitespace_len )
		{
			whitespace_str = (char *) realloc(whitespace_str,(2*len+1)*sizeof(char));
			for (i=whitespace_len; i<2*len; i++)
			{
				whitespace_str[i] = ' ';
			}
			whitespace_len = 2*len;
		}

		whitespace_str[whitespace_pos] = ' ';
		whitespace_pos = len;
		whitespace_str[whitespace_pos] = 0;
	}

	return whitespace_str;
}

void checkListArg(FILE *f, ATermList l, ATermList *n, int *d, int *ls)
{
	int l_ls = *ls;

	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		if ( ATisList(ATgetFirst(l)) )
		{
	fprintf(f,	"       %sif ( ATisList(ATgetFirst(l%i)) )\n"
			"       %s{\n"
			"       %s  ATermList l%i = (ATermList) ATgetFirst(l%i);\n"
			"       %s  l%i = ATgetNext(l%i);\n",
			whitespace((*d)*2),l_ls-1,whitespace((*d)*2),whitespace((*d)*2),l_ls,l_ls-1,whitespace((*d)*2),l_ls-1,l_ls-1
	       );
			(*d)++;
			(*ls)++;
			checkListArg(f,ATLgetFirst(l),n,d,ls);
		} else if ( ATisInt(ATgetFirst(l)) )
		{
	fprintf(f,	"       %sif ( ATisInt(ATgetFirst(l%i)) && (ATgetInt((ATermInt) ATgetFirst(l%i)) == %i) )\n"
			"       %s{\n"
			"       %s  l%i = ATgetNext(l%i);\n",
			whitespace((*d)*2),l_ls-1,l_ls-1,ATgetInt((ATermInt)ATgetFirst(l)),
			whitespace((*d)*2),
			whitespace((*d)*2),l_ls-1,l_ls-1
	       );
			(*d)++;
		} else {
			if ( ATindexOf(*n,ATgetFirst(l),0) >= 0 )
			{
	fprintf(f,	"       %svar_%s = ATgetFirst(l%i);\n"
			"       %sl%i = ATgetNext(l%i);\n",
			whitespace((*d)*2),ATgetName(ATgetAFun(ATAgetArgument(ATAgetFirst(l),0))),l_ls-1,
			whitespace((*d)*2),l_ls-1,l_ls-1
	       );
				*n = ATremoveAll(*n,ATgetFirst(l));
			} else {
	fprintf(f,	"       %sif ( ATisEqual(var_%s,ATgetFirst(l%i)) )\n"
			"       %s{\n"
			"       %s  l%i = ATgetNext(l%i);\n",
			whitespace((*d)*2),ATgetName(ATgetAFun(ATAgetArgument(ATAgetFirst(l),0))),l_ls-1,
			whitespace((*d)*2),
			whitespace((*d)*2),l_ls-1,l_ls-1
	       );
				(*d)++;
			}
		}
	}
}

void calcList(FILE *f, ATermList l, int d, int ls, bool incl_rest, int incl_num)
{
	ATerm a;
	int len;

	a = ATgetFirst(l);
	l = ATreverse(ATgetNext(l));
	len = ATgetLength(l);

	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		if ( ATisList(ATgetFirst(l)) )
		{
			// XXX Perhaps easier to rewrite now
	fprintf(f,	"       %s{\n"
			"       %s  ATermList l%i = ATmakeList0();\n"
			"\n",
			whitespace(d*2),
			whitespace(d*2),ls
	       );
			calcList(f,ATLgetFirst(l),d+1,ls+1,incl_rest,incl_num);
	fprintf(f,	"\n"
			"       %s  if ( ATisEmpty(ATgetNext(l%i)) )\n"
			"       %s  {\n"
			"       %s    l%i = ATinsert(l%i,ATgetFirst(l%i));\n"
			"       %s  } else {\n",
			whitespace(d*2),ls,
			whitespace(d*2),
			whitespace(d*2),ls-1,ls-1,ls,
			whitespace(d*2)
	       );
			if ( 0 && ATisEmpty(ATgetNext(l)) )
			{
	fprintf(f,	"       %s    l%i = ATconcat(l%i,l%i);\n",
			whitespace(d*2),ls-1,ls,ls-1
	       );
			} else {
	fprintf(f,	"       %s    l%i = ATinsert(l%i,(ATerm) l%i);\n",
			whitespace(d*2),ls-1,ls-1,ls
	       );
			}
	fprintf(f,	"       %s  }\n"
			"       %s}\n",
			whitespace(d*2),
			whitespace(d*2)
	       );
		} else if ( ATisInt(ATgetFirst(l)) )
		{
			// XXX Perhaps easier to rewrite now
			// as it is a constant
	fprintf(f,	"       %sif ( int2func[%i] != NULL )\n"
			"       %s{\n"
			"       %s  ATermList l%i = int2func[%i](ATmakeList0(),0);\n"
			"       %s  if ( ATisEmpty(ATgetNext(l%i)) )\n"
			"       %s  {\n"
			"       %s    l%i = ATinsert(l%i,ATgetFirst(l%i));\n"
			"       %s  } else {\n",
			whitespace(d*2),ATgetInt((ATermInt) ATgetFirst(l)),
			whitespace(d*2),
			whitespace(d*2),ls,ATgetInt((ATermInt) ATgetFirst(l)),
			whitespace(d*2),ls,
			whitespace(d*2),
			whitespace(d*2),ls-1,ls-1,ls,
			whitespace(d*2)
	       );
			if ( 0 && ATisEmpty(ATgetNext(l)) )
			{
	fprintf(f,	"       %s    l%i = ATconcat(l%i,l%i);\n",
			whitespace(d*2),ls-1,ls,ls-1
	       );
			} else {
	fprintf(f,	"       %s    l%i = ATinsert(l%i,(ATerm) l%i);\n",
			whitespace(d*2),ls-1,ls-1,ls
	       );
			}
	fprintf(f,	"       %s  }\n"
			"       %s} else {\n"
			"       %s  l%i = ATinsert(l%i,int2ATerm[%i]);\n"
			"       %s}\n",
			whitespace(d*2),
			whitespace(d*2),
			whitespace(d*2),ls-1,ls-1,ATgetInt((ATermInt) ATgetFirst(l)),
			whitespace(d*2)
	       );
		} else if ( !gsIsNil(ATAgetFirst(l)) )
		{
			if ( 0 && ATisEmpty(ATgetNext(l)) )
			{
	fprintf(f,	"       %sif ( ATisList(var_%s) )\n"
			"       %s{\n"
			"       %s  l%i = ATconcat((ATermList) var_%s,l%i);\n"
			"       %s} else {\n"
			"       %s  l%i = ATinsert(l%i,var_%s);\n"
			"       %s}\n",
			whitespace(d*2),ATgetName(ATgetAFun(ATAgetArgument(ATAgetFirst(l),0))),
			whitespace(d*2),
			whitespace(d*2),ls-1,ATgetName(ATgetAFun(ATAgetArgument(ATAgetFirst(l),0))),ls-1,
			whitespace(d*2),
			whitespace(d*2),ls-1,ls-1,ATgetName(ATgetAFun(ATAgetArgument(ATAgetFirst(l),0))),
			whitespace(d*2)
	       );
			} else {
	fprintf(f,	"       %sl%i = ATinsert(l%i,var_%s);\n",
			whitespace(d*2),ls-1,ls-1,ATgetName(ATgetAFun(ATAgetArgument(ATAgetFirst(l),0)))
	       );
			}
		}
	}

	if ( ATisInt(a) )
	{
	fprintf(f,	"       %sif ( int2func[%i] != NULL )\n"
			"       %s{\n"
			"       %s  l%i = int2func[%i](l%i,%s%i+%i);\n"
			"       %s} else {\n"
			"       %s  l%i = ATinsert(l%i,int2ATerm[%i]);\n"
			"       %s}\n",
			whitespace(d*2),ATgetInt((ATermInt) a),
			whitespace(d*2),
			whitespace(d*2),ls-1,ATgetInt((ATermInt) a),ls-1,incl_rest?"len-":"",incl_rest?incl_num:0,len,
			whitespace(d*2),
			whitespace(d*2),ls-1,ls-1,ATgetInt((ATermInt) a),
			whitespace(d*2)
	       );
	} else {
	fprintf(f,	"       %s{\n"
			"       %s  ATerm a;\n"
			"\n"
			"       %s  if ( ATisList(var_%s) )\n"
			"       %s  {\n"
			"       %s    a = ATgetFirst((ATermList) var_%s);\n"
			"       %s    l%i = ATconcat(ATgetNext((ATermList) var_%s),l%i);\n"
			"       %s  } else {\n"
			"       %s    a = var_%s;\n"
			"       %s  }\n"

//			"ATfprintf(stderr,\"--> %%t\\n\\n\",var_%s);\n"

			"       %s  if ( ATisInt(a) && (int2func[ATgetInt((ATermInt) a)] != NULL) )\n"
			"       %s  {\n"
			"       %s    l%i = int2func[ATgetInt((ATermInt) a)](l%i,ATgetLength(l%i));\n"
			"       %s  } else {\n"
			"       %s    l%i = ATinsert(l%i,a);\n"
			"       %s  }\n"
			"       %s}\n",
			whitespace(d*2),
			whitespace(d*2),
			whitespace(d*2),ATgetName(ATgetAFun(ATAgetArgument((ATermAppl) a,0))),
			whitespace(d*2),
			whitespace(d*2),ATgetName(ATgetAFun(ATAgetArgument((ATermAppl) a,0))),
			whitespace(d*2),ls-1,ATgetName(ATgetAFun(ATAgetArgument((ATermAppl) a,0))),ls-1,
			whitespace(d*2),
			whitespace(d*2),ATgetName(ATgetAFun(ATAgetArgument((ATermAppl) a,0))),
			whitespace(d*2),

//			ATgetName(ATgetAFun(ATAgetArgument((ATermAppl) a,0))),

			whitespace(d*2),
			whitespace(d*2),
			whitespace(d*2),ls-1,ls-1,ls-1,
			whitespace(d*2),
			whitespace(d*2),ls-1,ls-1,
			whitespace(d*2),
			whitespace(d*2)
	       );
/*	fprintf(f,	"       %sif ( ATisList(var_%s) )\n"
			"       %s{\n"
			"       %s  l%i = ATconcat((ATermList) var_%s,l%i);\n"
			"       %s} else {\n"
			"       %s  l%i = ATinsert(l%i,var_%s);\n"
			"       %s}\n",
			whitespace(d*2),ATgetName(ATgetAFun(ATAgetArgument((ATermAppl) a,0))),
			whitespace(d*2),
			whitespace(d*2),ls-1,ATgetName(ATgetAFun(ATAgetArgument((ATermAppl) a,0))),ls-1,
			whitespace(d*2),
			whitespace(d*2),ls-1,ls-1,ATgetName(ATgetAFun(ATAgetArgument((ATermAppl) a,0))),
			whitespace(d*2)
	       );*/
/*	fprintf(f,	"       %sl%i = ATinsert(l%i,var_%s);\n",
			whitespace(d*2),ls-1,ls-1,ATgetName(ATgetAFun(ATAgetArgument((ATermAppl) a,0)))
	       );*/
	}
}

void rewrite_init_innerc()
{
	ATermList l,m,n;
	ATermTable tmp_eqns;
	ATermInt i;
	int j,k,max,d,ls;
	FILE *f;
	char *s,*t;
	void *h;

	tmp_eqns = ATtableCreate(100,100); // XXX would be nice to know the number op OpIds
	term2int = ATtableCreate(100,100);

	true_num = ATgetInt((ATermInt) OpId2Int(gsMakeDataExprTrue(),true));
	ATprotectInt(&true_num);

	l = opid_eqns;
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		// XXX only adds the last rule where lhs is an opid; this might go "wrong" if this rule is removed later
		ATtablePut(tmp_eqns,OpId2Int(ATAgetArgument(ATAgetFirst(l),2),true),(ATerm) ATmakeList1((ATerm) ATmakeList4((ATerm) ATmakeList0(),toInner(ATAgetArgument(ATAgetFirst(l),1),true),(ATerm) ATmakeList0(),toInner(ATAgetArgument(ATAgetFirst(l),3),true))));
	}

	l = dataappl_eqns;
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		m = (ATermList) toInner(ATAgetArgument(ATAgetFirst(l),2),true);
		if ( (n = (ATermList) ATtableGet(tmp_eqns,ATgetFirst(m))) == NULL )
		{
			n = ATmakeList0();
		}
		n = ATinsert(n,(ATerm) ATmakeList4(ATgetArgument(ATAgetFirst(l),0),toInner(ATAgetArgument(ATAgetFirst(l),1),true),(ATerm) ATgetNext(m),toInner(ATAgetArgument(ATAgetFirst(l),3),true)));
		ATtablePut(tmp_eqns,ATgetFirst(m),(ATerm) n);
	}

	int2term = (ATermAppl *) malloc(num_opids*sizeof(ATermAppl));
	ATprotectArray((ATerm *) int2term,num_opids);
	innerc_eqns = (ATermList *) malloc(num_opids*sizeof(ATermList));
	ATprotectArray((ATerm *) innerc_eqns,num_opids);

	l = ATtableKeys(term2int);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		i = (ATermInt) ATtableGet(term2int,ATgetFirst(l));
		int2term[ATgetInt(i)] = ATAgetFirst(l);
		if ( (m = (ATermList) ATtableGet(tmp_eqns,(ATerm) i)) == NULL )
		{
			innerc_eqns[ATgetInt(i)] = NULL;
		} else {
			innerc_eqns[ATgetInt(i)] = ATreverse(m);
		}
	}

	ATtableDestroy(tmp_eqns);

	s = "innerc_XXXXXX"; // XXX
	t = (char *) malloc(100);
	sprintf(t,"%s.c",s);
	f = fopen(t,"w");

	//
	//  Print includes and defs
	//
	fprintf(f,	"#include \"aterm2.h\"\n"
			"\n"
			"#define ATisList(x) (ATgetType(x) == AT_LIST)\n"
			"#define ATisInt(x) (ATgetType(x) == AT_INT)\n"
			"\n"
			"typedef ATermList (*ftype)(ATermList, int);\n"
			"ftype *int2func;\n"
			"ATerm *int2ATerm;\n"
			"\n"
	       );

	//
	// Forward declarations of rewr_* functions
	//
/*	for (j=0;j<num_opids;j++)
	{
		if ( innerc_eqns[j] != NULL )
		{
	fprintf(f,	"ATermList rewr_%i(ATermList args, int len);\n",j);
		}
	}
	fprintf(f, "\n");*/

	//
	// Implement the equations of every op 
	//
	for (j=0;j<num_opids;j++)
	{
		if ( innerc_eqns[j] != NULL )
		{
      ATfprintf(f,	"// %t\n",int2term[j]);
      			//
			// Declarations and initialisation of arrays
			//
	fprintf(f,	"ATermList rewr_%i(ATermList args, int len)\n"
			"{\n"
			"  int i;\n"
			"  ATerm a[len];\n"
			"  ATermList rest[len+1];\n"
			"\n"

//			"ATfprintf(stderr,\"rewr_%i(%%t,%%i)\\n\\n\",args,len);\n"

			"  for (i=0; i<len; i++)\n"
			"  {\n"
			"    a[i] = ATgetFirst(args);\n"
			"    rest[i] = args;\n"
			"    args = ATgetNext(args);\n"
			"  }\n"
			"  rest[len] = args;\n"
			"\n"
			"  switch ( len )\n"
			"  {\n"
			"    default:\n",

//			j,

			j
	       );

			//
			// Implement every equation of the current op
			//
			l = innerc_eqns[j];
			max = 0;
			for (; !ATisEmpty(l); l=ATgetNext(l))
			{
				if ( max < ATgetLength(ATLelementAt(ATLgetFirst(l),2)) )
				{
					max = ATgetLength(ATLelementAt(ATLgetFirst(l),2));
				}
			}
			for (; max>=0; max-- )
			{
	fprintf(f,	"    case %i:\n",max);

//	fprintf(f,	"fprintf(stderr,\"case %i\\n\\n\");\n",max);

				l = innerc_eqns[j];
				for (; !ATisEmpty(l); l=ATgetNext(l))
				{
					if ( max == ATgetLength(ATLelementAt(ATLgetFirst(l),2)) )
					{
      ATfprintf(f,	"      // %t\n",ATLgetFirst(l));
						//
						// Declare equation variables
						//
						m = ATLelementAt(ATLgetFirst(l),0);
	fprintf(f,	"     {\n");
						for (; !ATisEmpty(m); m=ATgetNext(m))
						{
	fprintf(f,	"       ATerm var_%s;\n",ATgetName(ATgetAFun(ATAgetArgument(ATAgetFirst(m),0))));
						}
	fprintf(f,	"\n");
						//
						// Match every parameter
						//
						m = ATLelementAt(ATLgetFirst(l),2);
						n = ATLelementAt(ATLgetFirst(l),0);
						k = 0;
						d = 0;
						ls = 0;
						for (; !ATisEmpty(m); m=ATgetNext(m))
						{
							if ( ATisList(ATgetFirst(m)) )
							{

//	fprintf(f,	"fprintf(stderr,\"matching argument %i\\n\\n\");\n",k+1);

	fprintf(f,	"       %sif ( ATisList(a[%i]) )\n"
			"       %s{\n"
			"       %s  ATermList l%i = (ATermList) a[%i];\n",
			whitespace(d*2),k,whitespace(d*2),whitespace(d*2),ls,k
	       );
								d++;
								ls++;
								checkListArg(f,ATLgetFirst(m),&n,&d,&ls);
							} else if ( ATisInt(ATgetFirst(m)) )
							{
	fprintf(f,	"       %sif ( ATisInt(a[%i]) && (ATgetInt((ATermInt) a[%i]) == %i) )\n"
			"       %s{\n",
			whitespace(d*2),k,k,ATgetInt((ATermInt)ATgetFirst(m)),whitespace(d*2)
	       );
								d++;
							} else {
								if ( ATindexOf(n,ATgetFirst(m),0) >= 0 )
								{
	fprintf(f,	"       %svar_%s = a[%i];\n",whitespace(d*2),ATgetName(ATgetAFun(ATAgetArgument(ATAgetFirst(m),0))),k);
									n = ATremoveAll(n,ATgetFirst(m));
								} else {
	fprintf(f,	"       %sif ( ATisEqual(var_%s,a[%i]) )\n"
			"       %s{\n",
			whitespace(d*2),ATgetName(ATgetAFun(ATAgetArgument(ATAgetFirst(m),0))),k,whitespace(d*2)
	       );
									d++;
								}
							}
							k++;
						}
						
						//
						// Test condition of equation
						//
						if ( ATisList(ATelementAt(ATLgetFirst(l),1)) )
						{
							// XXX Perhaps easier to rewrite now
	fprintf(f,	"       %s{\n"
			"       %s  ATermList l%i = ATmakeList0();\n"
			"\n",
			whitespace(d*2),
			whitespace(d*2),ls
	       );
							calcList(f,ATLelementAt(ATLgetFirst(l),1),d+1,ls+1,false,0);
	fprintf(f,	"\n"
			"       %s  if ( ATgetInt((ATermInt) ATgetFirst(l%i)) == %i )\n"
			"       %s  {\n",
			whitespace(d*2),ls,true_num,
			whitespace(d*2)
	       );
							d += 2;
							ls++;
						} else if ( ATisInt(ATelementAt(ATLgetFirst(l),1)) )
						{
							// XXX Perhaps easier to rewrite now
							// as it is a constant
	fprintf(f,	"       %s{\n"
			"       %s  int i%i = %i;\n"
			"\n"
			"       %s  if ( int2func[%i] != NULL )\n"
			"       %s  {\n"
			"       %s    ATermList l%i = int2func[%i](ATmakeList0(),0);\n"
			"       %s    i%i = ATgetInt((ATermInt) ATgetFirst(l%i));\n"
			"       %s  }\n"
			"       %s  if ( i%i == %i )\n"
			"       %s  {\n",
			whitespace(d*2),
			whitespace(d*2),ls,ATgetInt((ATermInt) ATelementAt(ATLgetFirst(l),1)),
			whitespace(d*2),ATgetInt((ATermInt) ATelementAt(ATLgetFirst(l),1)),
			whitespace(d*2),
			whitespace(d*2),ls,ATgetInt((ATermInt) ATelementAt(ATLgetFirst(l),1)),
			whitespace(d*2),ls,ls,
			whitespace(d*2),
			whitespace(d*2),ls,true_num,
			whitespace(d*2)
	       );
							d += 2;
							ls++;
						} else if ( !gsIsNil(ATAelementAt(ATLgetFirst(l),1)) )
						{
	fprintf(f,	"       %sif ( ATisInt(var_%s) && (ATgetInt((ATermInt) var_%s) == %i) )\n"
			"       %s{\n",
			whitespace(d*2),ATgetName(ATgetAFun(ATAgetArgument(ATAelementAt(ATLgetFirst(l),1),0))),ATgetName(ATgetAFun(ATAgetArgument(ATAelementAt(ATLgetFirst(l),1),0))),true_num,whitespace(d*2)
	       );
							d++;
						}

						//
						// Create result
						//
						if ( ATisList(ATelementAt(ATLgetFirst(l),3)) )
						{
							// XXX Perhaps easier to rewrite now
	fprintf(f,	"       %s{\n"
			"       %s  ATermList l%i = rest[%i];\n"
			"\n",
			whitespace(d*2),
			whitespace(d*2),ls,max
	       );
							calcList(f,ATLelementAt(ATLgetFirst(l),3),d+1,ls+1,true,max);
	fprintf(f,	"\n"

//			"ATfprintf(stderr,\"(%i)return %%t\\n\\n\",l%i);\n"

			"       %s  return l%i;\n"
			"       %s}\n",

//			j,ls,

			whitespace(d*2),ls,
			whitespace(d*2)
	       );
						} else if ( ATisInt(ATelementAt(ATLgetFirst(l),3)) )
						{
							// XXX Perhaps easier to rewrite now
							// as it is a constant
	fprintf(f,	"       %sif ( int2func[%i] != NULL )\n"
			"       %s{\n"

//			"ATfprintf(stderr,\"(%i)pass\\n\\n\");\n"

			"       %s  return int2func[%i](rest[%i],len-%i);\n"
			"       %s} else {\n"

//			"ATfprintf(stderr,\"(%i)return %%t\\n\\n\",ATinsert(rest[%i],int2ATerm[%i]));\n"

			"       %s  return ATinsert(rest[%i],int2ATerm[%i]);\n"
			"       %s}\n",
			whitespace(d*2),ATgetInt((ATermInt) ATelementAt(ATLgetFirst(l),3)),
			whitespace(d*2),

//			j,

			whitespace(d*2),ATgetInt((ATermInt) ATelementAt(ATLgetFirst(l),3)),max,max,
			whitespace(d*2),

//			j,max,ATgetInt((ATermInt) ATelementAt(ATLgetFirst(l),3)),

			whitespace(d*2),max,ATgetInt((ATermInt) ATelementAt(ATLgetFirst(l),3)),
			whitespace(d*2)
	       );
						} else if ( !gsIsNil(ATAelementAt(ATLgetFirst(l),3)) )
						{
	fprintf(f,	"       %sif ( ATisList(var_%s) )\n"
			"       %s{\n"

//			"ATfprintf(stderr,\"(%i)return %%t\\n\\n\",ATconcat((ATermList) var_%s,rest[%i]));\n"

			"       %s  return ATconcat((ATermList) var_%s,rest[%i]);\n"
			"       %s} else {\n"

//			"ATfprintf(stderr,\"(%i)return %%t\\n\\n\",ATinsert(rest[%i],var_%s));\n"

			"       %s  return ATinsert(rest[%i],var_%s);\n"
			"       %s}\n",
			whitespace(d*2),ATgetName(ATgetAFun(ATAgetArgument(ATAelementAt(ATLgetFirst(l),3),0))),
			whitespace(d*2),

//			j,ATgetName(ATgetAFun(ATAgetArgument(ATAelementAt(ATLgetFirst(l),3),0))),max,

			whitespace(d*2),ATgetName(ATgetAFun(ATAgetArgument(ATAelementAt(ATLgetFirst(l),3),0))),max,
			whitespace(d*2),

//			j,max,ATgetName(ATgetAFun(ATAgetArgument(ATAelementAt(ATLgetFirst(l),3),0))),

			whitespace(d*2),max,ATgetName(ATgetAFun(ATAgetArgument(ATAelementAt(ATLgetFirst(l),3),0))),
			whitespace(d*2)
	       );
						}

						//
						// Close all brackets
						//
						for (; d>0; d--)
						{
	fprintf(f,	"       %s}\n",whitespace((d-1)*2));
						}
	fprintf(f,	"     }\n");
					}
				}
			}
			//
			// Finish up function
			//
	fprintf(f,	"      break;\n"
			"  }\n"

//			"ATfprintf(stderr,\"(%i)return %%t\\n\\n\",ATinsert(rest[0],int2ATerm[%i]));\n"

			"  return ATinsert(rest[0],int2ATerm[%i]);\n"
			"}\n"
			"\n",

//			j,j,

			j
	       );
		}
	}

	fprintf(f,	"ATerm rewrite_init()\n"
			"{\n"
			"  int i;\n"
			"\n"
	       );
	fprintf(f,	"  int2ATerm = (ATerm *) malloc(%i*sizeof(ATerm));\n"
			"  for (i=0; i<%i; i++)\n"
			"  {\n"
			"    int2ATerm[i] = (ATerm) ATmakeInt(i);\n"
			"  }\n"
			"\n",
			num_opids,num_opids
	       );
	fprintf(f,	"  int2func = (ftype *) malloc(%i*sizeof(ftype));\n",num_opids+100);
	for (j=0;j<num_opids;j++)
	{
		if ( innerc_eqns[j] == NULL )
		{
	fprintf(f,	"  int2func[%i] = NULL;\n",j);
		} else {
//	fprintf(f,	"  int2func[%i] = rewr_%i;\n",j,j);
      ATfprintf(f,	"  int2func[%i] = rewr_%i; // %t\n",j,j,int2term[j]);
		}
	}
	for (j=0; j<100; j++)
	fprintf(f,	"  int2func[%i] = NULL;\n",j+num_opids);
	fprintf(f,	"}\n"
			"\n"
			"ATerm rewrite(ATerm t)\n"
			"{\n"
			"  ATerm a;\n"
			"  ATermList l,n;\n"
			"  int len;\n"
			"\n"

//			"ATfprintf(stderr,\"rewrite(%%t)\\n\\n\",t);\n"

			"  if ( ATisList(t) )\n"
			"  {\n"
			"    a = ATgetFirst((ATermList) t);\n"
			"    l = ATgetNext((ATermList) t);\n"
			"  } else {\n"
			"    a = t;\n"
			"    l = ATmakeList0();\n"
			"  }\n"
			"\n"
			"  n = ATmakeList0();\n"
			"  len = 0;\n"
			"  for (; !ATisEmpty(l); l=ATgetNext(l))\n"
			"  {\n"

//			"ATfprintf(stderr,\"rewrite arg %%t\\n\\n\",ATgetFirst(l));\n"

			"    n = ATinsert(n, rewrite(ATgetFirst(l)));\n"
			"    len++;\n"
			"  }\n"
			"  l = ATreverse(n);\n"
			"\n"
			"  if ( ATisInt(a) && (int2func[ATgetInt((ATermInt) a)] != NULL) )\n"
			"  {\n"

//			"ATfprintf(stderr,\"calling int2func[%%i](%%t,%%i) (%%p)\\n\\n\",ATgetInt((ATermInt) a),l,len,int2func[ATgetInt((ATermInt) a)]);\n"

			"    l = int2func[ATgetInt((ATermInt) a)](l,len);\n"
			"    if ( ATisEmpty(ATgetNext(l)) )\n"
			"    {\n"
			"      return ATgetFirst(l);\n"
			"    } else {\n"
			"      return (ATerm) l;\n"
			"    }\n"
			"  } else {\n"
			"    if ( ATisEmpty(l) )\n"
			"    {\n"
			"      return a;\n"
			"    } else {\n"
			"      return (ATerm) ATinsert(l,a);\n"
			"    }\n"
			"  }\n"
			"}\n"
	       );

	fclose(f);

	sprintf(t,"gcc -O3 -rdynamic -c %s.c",s);
	system(t);
	sprintf(t,"gcc -shared -o %s.so %s.o",s,s);
	system(t);

	sprintf(t,"./%s.so",s);
//fprintf(stderr,"a %s\n",t);
//	h = dlopen(t,RTLD_LAZY);
	if ( (h = dlopen(t,RTLD_NOW)) == NULL )
	{
		fprintf(stderr,"error opening dll\n%s\n",dlerror());
	}
//fprintf(stderr,"%s\n",dlerror());
//fprintf(stderr,"b %p\n",h);
	so_rewr_init = dlsym(h,"rewrite_init");
//fprintf(stderr,"%s\n",dlerror());
	so_rewr = dlsym(h,"rewrite");
//fprintf(stderr,"%s\n",dlerror());
//fprintf(stderr,"c %p %p\n",so_rewr_init,so_rewr);
	so_rewr_init();
/*	{
		ATermAppl a;
		ATerm t;

		a = ATAgetArgument(ATAgetFirst(opid_eqns),2);
ATfprintf(stderr,"%t\n",a);
		t = toInner(a,false);
ATfprintf(stderr,"%t\n",t);
		t = so_rewr(t);
ATfprintf(stderr,"%t\n",t);
		a = fromInner(t);
ATfprintf(stderr,"%t\n",a);
	}*/
}

void rewrite_add_innerc(ATermAppl eqn)
{
	ATermList l,m;
	ATermAppl a;
	ATermInt i,j;
	unsigned int old_num;

	old_num = num_opids;

	a = ATAgetArgument(eqn,2);
	if ( gsIsOpId(a) )
	{
		j = (ATermInt) OpId2Int(a,true);
		m = ATmakeList4((ATerm) ATmakeList0(),ATgetArgument(eqn,1),(ATerm) ATmakeList0(),toInner(ATAgetArgument(eqn,3),true));
	} else {
		l = (ATermList) toInner(a,true);
		j = (ATermInt) ATgetFirst(l);
		m = ATmakeList4(ATgetArgument(eqn,0),ATgetArgument(eqn,1),(ATerm) ATgetNext(l),toInner(ATAgetArgument(eqn,3),true));
	}

	l = ATtableKeys(term2int);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		i = (ATermInt) ATtableGet(term2int,ATgetFirst(l));
		if ( ATgetInt(i) >= old_num )
		{
			int2term[ATgetInt(i)] = ATAgetFirst(l);
			innerc_eqns[ATgetInt(i)] = NULL;
		}
	}
	if ( innerc_eqns[ATgetInt(j)] == NULL )
	{
		innerc_eqns[ATgetInt(j)] = ATmakeList1((ATerm) m);
	} else {
		innerc_eqns[ATgetInt(j)] = ATappend(innerc_eqns[ATgetInt(j)],(ATerm) m);
	}
}

void rewrite_remove_innerc(ATermAppl eqn)
{
	ATermList l,m;
	ATermAppl a;
	ATerm t;

	a = ATAgetArgument(eqn,2);
	if ( gsIsOpId(a) )
	{
		t = OpId2Int(a,false);
		m = ATmakeList4((ATerm) ATmakeList0(),ATgetArgument(eqn,1),(ATerm) ATmakeList0(),toInner(ATAgetArgument(eqn,3),true));
	} else {
		l = (ATermList) toInner(a,false);
		t = ATgetFirst(l);
		m = ATmakeList4(ATgetArgument(eqn,0),ATgetArgument(eqn,1),(ATerm) ATgetNext(l),toInner(ATAgetArgument(eqn,3),true));
	}

	if ( ATisInt(t) )
	{
		l = innerc_eqns[ATgetInt((ATermInt) t)];
		m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			if ( !ATisEqual(m,ATgetFirst(l)) )
			{
				ATinsert(m,ATgetFirst(l));
			}
		}
		if ( ATisEmpty(m) )
		{
			innerc_eqns[ATgetInt((ATermInt) t)] = NULL;
		} else {
			innerc_eqns[ATgetInt((ATermInt) t)] = ATreverse(m);
		}
	}
}

ATerm rewrite_innerc(ATerm Term, int *b)
{
	ATerm a;
	ATermList l;
	int c,old_opids;
	ATermInt i;

	if ( ATisList(Term) )
	{
		l = ATmakeList0();
		*b = 0;
		for (; !ATisEmpty((ATermList) Term); Term=(ATerm)ATgetNext((ATermList) Term))
		{
			old_opids = num_opids;
			a = toInner(ATAgetFirst((ATermList) Term),true);
			if ( old_opids < num_opids )
			{
				ATunprotectArray((ATerm *) int2term);
				int2term = (ATermAppl *) realloc(int2term,num_opids*sizeof(ATermAppl));
				ATprotectArray((ATerm *) int2term,num_opids);
				l = ATtableKeys(term2int);
				c = 0;
				for (; !ATisEmpty(l); l=ATgetNext(l))
				{
					i = (ATermInt) ATtableGet(term2int,ATgetFirst(l));
					if ( ATgetInt(i) >= old_opids )
					{
						int2term[ATgetInt(i)] = ATAgetFirst(l);
						c++;
					}
				}
			}
			l = ATinsert(l,(ATerm) fromInner(so_rewr(a)));
		}
		return (ATerm) ATreverse(l);
	}

	old_opids = num_opids;
	a = toInner((ATermAppl) Term,true);
	if ( old_opids < num_opids )
	{
		ATunprotectArray((ATerm *) int2term);
		int2term = (ATermAppl *) realloc(int2term,num_opids*sizeof(ATermAppl));
		ATprotectArray((ATerm *) int2term,num_opids);
		l = ATtableKeys(term2int);
		c = 0;
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			i = (ATermInt) ATtableGet(term2int,ATgetFirst(l));
			if ( ATgetInt(i) >= old_opids )
			{
				int2term[ATgetInt(i)] = ATAgetFirst(l);
				c++;
			}
		}
	}
	return (ATerm) fromInner(so_rewr(a));
}
