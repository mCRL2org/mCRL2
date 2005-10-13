/* $Id: gsrewr.c,v 1.2 2005/04/08 12:33:51 muck Exp $ */
#ifdef __cplusplus
extern "C" {
#endif

#define NAME "libgsalpha"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <aterm2.h>
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libprint_c.h"
#include "libgsalpha.h"

static ATermTable alphas;
static ATermList procs;

static ATermList synch_mact(ATermList a, ATermList b)
{
	ATermList c;

	c = ATmakeList0();
	while ( !(ATisEmpty(a) || ATisEmpty(b)) )
	{
		if ( ATgetFirst(a) <= ATgetFirst(b) ) //XXX yuck!
		{
			c = ATinsert(c,ATgetFirst(a));
			a = ATgetNext(a);
		} else {
			c = ATinsert(c,ATgetFirst(b));
			b = ATgetNext(b);
		}
	}
	if ( !ATisEmpty(a) )
	{
		c = ATconcat(c,a);
	} else if ( !ATisEmpty(b) )
	{
		c = ATconcat(c,b);
	}

	return c;
}

static ATermList merge_list(ATermList l, ATermList m)
{
//	gsprintf("merge_list(%p,%p)\n\n",l,m);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		if ( ATindexOf(m,ATgetFirst(l),0) < 0 )
		{
			m = ATinsert(m,ATgetFirst(l));
		}
	}

	return m;
}

static ATermList list_minus(ATermList l, ATermList m)
{
	ATermList n;

	n = ATmakeList0();
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		if ( ATindexOf(m,ATgetFirst(l),0) < 0 )
		{
			n = ATinsert(n,ATgetFirst(l));
		}
	}
	n = ATreverse(n);

	return n;
}

static ATermList filter(ATermList l, ATermList H)
{
	ATermList m;

	m = ATmakeList0();
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		ATermList tH = H;
		bool b;

		b = true;
		for (; !ATisEmpty(tH); tH=ATgetNext(tH))
		{
			if ( ATindexOf(ATLgetFirst(l),ATgetFirst(tH),0) >= 0 )
			{
				b = false;
				break;
			}
		}
		if ( b )
		{
			m = ATinsert(m,ATgetFirst(l));
		}
	}
	m = ATreverse(m);

	return m;
}

static ATermList sync_list(ATermList l, ATermList m)
{
	ATermList n;

//gsprintf("sync_list(%p,%p)\n",l,m);
//gsprintf("sync_list(%T,%T)\n",l,m);

	n = ATmakeList0();
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		for (ATermList o=m; !ATisEmpty(o); o=ATgetNext(o))
		{
			n = ATinsert(n,(ATerm) synch_mact(ATLgetFirst(l),ATLgetFirst(o)));
		}
	}

//gsprintf("%T\n\n",n);
	return n;
}

static bool disjoint(ATermList l, ATermList m)
{
	for (; !ATisEmpty(m); m=ATgetNext(m) )
	{
		if ( ATindexOf(l,ATgetArgument(ATAgetFirst(m),0),0) >= 0 )
		{
			return false;
		}
	}

	return true;
}

static bool disjoint_list(ATermList l, ATermList m)
{
	for (; !ATisEmpty(m); m=ATgetNext(m))
	{
		if ( !disjoint(l,ATLgetFirst(m)) )
		{
			return false;
		}
	}
	return true;
}

static ATermList comm_lhs(ATermList C)
{
	ATermList l;

	l = ATmakeList0();
	for (; !ATisEmpty(C); C=ATgetNext(C))
	{
		ATermList m = ATLgetArgument(ATAgetArgument(ATAgetFirst(C),0),0);

		for (; !ATisEmpty(m); m=ATgetNext(m))
		{
			l = ATinsert(l,ATgetFirst(m));
		}
	}
	l = ATreverse(l);

	return l;
}

static ATermList comm_rhs(ATermList C)
{
	ATermList l;

	l = ATmakeList0();
	for (; !ATisEmpty(C); C=ATgetNext(C))
	{
		ATermAppl a = ATAgetArgument(ATAgetFirst(C),1);

		if ( !gsIsNil(a) ) 
		{
			l = ATinsert(l,(ATerm) a);
		}
	}
	l = ATreverse(l);

	return l;
}

static bool can_split_comm(ATermList C)
{
	ATermList lhs = comm_lhs(C);
	ATermList rhs = comm_rhs(C);
	bool b;

	b = true;
	for (; !ATisEmpty(lhs); lhs=ATgetNext(lhs))
	{
		if ( ATindexOf(rhs,ATgetFirst(lhs),0) >= 0 )
		{
			b = false;
			break;
		}
	}

	return b;
}

static ATermList apply_comm(ATermList l, ATermList C)
{
	if ( ATisEmpty(l) || ATisEmpty(ATgetNext(l)) )
	{
		return ATmakeList1((ATerm) l);
	}

	ATermAppl a = ATAgetFirst(l);
	ATermList r = ATgetNext(l);
	ATermList m = ATmakeList0();
	ATermList tC = C;
	for (; !ATisEmpty(tC); tC=ATgetNext(tC))
	{
		ATermList c = ATLgetArgument(ATAgetArgument(ATAgetFirst(tC),0),0);
		if ( ATindexOf(c,ATgetArgument(a,0),0) >= 0 )
		{
			ATermList s = ATLgetArgument(a,1);
			ATermList tr = r;
			bool b;

			c = ATremoveElement(c,ATgetArgument(a,0));
			b = true;
			for (; !ATisEmpty(c); c=ATgetNext(c))
			{
				ATermAppl act = gsMakeAction(ATAgetFirst(c),s);
				if ( ATindexOf(tr,(ATerm) act,0) >= 0 )
				{
					tr = ATremoveElement(tr,(ATerm) act);
				} else {
					b = false;
					break;
				}
			}
			if ( b )
			{
				if ( gsIsNil(ATAgetArgument(ATAgetFirst(tC),1)) )
				{
					ATconcat(apply_comm(tr,C),m);
				} else {
					ATconcat(sync_list(ATmakeList1(ATgetArgument(ATAgetFirst(tC),1)),apply_comm(r,C)),m);
				}
			}
		}
	}
	m = ATconcat(sync_list(ATmakeList1((ATerm) ATmakeList1((ATerm) a)),apply_comm(r,C)),m);

	return m;
}

static ATermList apply_comm_list(ATermList l, ATermList C)
{
	ATermList m;

	m = ATmakeList0();
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		m = ATconcat(apply_comm(ATLgetFirst(l),C),m);
	}
	m = ATreverse(m);

	return m;
}

static ATermAppl PushBlock(ATermList H, ATermAppl a)
{
//gsprintf("%T ",H);PrintPart_C(stdout,a);printf("\n\n");
	if ( gsIsAction(a) )
	{
		if ( ATindexOf(H,ATgetArgument(ATAgetArgument(a,0),0),0) >= 0 )
		{
			return gsMakeDelta();
		} else {
			return a;
		}
	} else if ( gsIsProcess(a) )
	{
		ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);

		l = filter(l,H);
		// XXX also adjust H

		a = gsMakeBlock(H,a);

		ATtablePut(alphas,(ATerm) a,(ATerm) l);

		return a;
	} else if ( gsIsDelta(a) || gsIsTau(a) )
	{
		return a;
	} else if ( gsIsSum(a) )
	{
		ATermAppl p = ATAgetArgument(a,1);
		ATermList l;

		p = PushBlock(H,p);

		l = (ATermList) ATtableGet(alphas,(ATerm) p);

		a = ATsetArgument(a,(ATerm) p, 1);

		ATtablePut(alphas,(ATerm) a,(ATerm) l);

		return a;
	} else if ( gsIsBlock(a) )
	{
		a = PushBlock(merge_list(H,ATLgetArgument(a,0)),ATAgetArgument(a,1));

		return a;
	} else if ( gsIsHide(a) )
	{
		ATermAppl p = ATAgetArgument(a,1);
		ATermList l;

		H = list_minus(H,ATLgetArgument(a,0));
		
		l = (ATermList) ATtableGet(alphas,(ATerm) p);
		l = filter(l,H);

		p = PushBlock(H,p);

		a = gsMakeHide(ATLgetArgument(a,0),p);

		ATtablePut(alphas,(ATerm) a,(ATerm) l);

		return a;
	} else if ( gsIsRename(a) )
	{
		// XXX
		ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
		a = gsMakeBlock(H,a);
		ATtablePut(alphas,(ATerm) a,(ATerm) l);
		return a;
	} else if ( gsIsComm(a) )
	{
		//XXX
		ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
		ATermList C = ATLgetArgument(a,0);
		ATermList lhs = comm_lhs(C);
		ATermList rhs = comm_rhs(C);
		ATermList Ha = ATmakeList0();
		ATermList Hc = ATmakeList0();

		for (; !ATisEmpty(H); H=ATgetNext(H))
		{
			if ( (ATindexOf(lhs,ATgetFirst(H),0) >= 0) || (ATindexOf(rhs,ATgetFirst(H),0) >= 0) )
			{
				Ha = ATinsert(Ha,ATgetFirst(H));
			} else {
				Hc = ATinsert(Hc,ATgetFirst(H));
			}
		}

		if ( !ATisEmpty(Hc) )
		{
			a = PushBlock(Hc,ATAgetArgument(a,1));
			l = (ATermList) ATtableGet(alphas,(ATerm) a);
			a = gsMakeComm(C,a);
			ATtablePut(alphas,(ATerm) a,(ATerm) apply_comm_list(l,C));
		} else {
			l = (ATermList) ATtableGet(alphas,(ATerm) a);
		}

		if ( !ATisEmpty(Ha) )
		{
			a = gsMakeBlock(Ha,a);
			ATtablePut(alphas,(ATerm) a,(ATerm) filter(l,Ha));
		}

		return a;
	} else if ( gsIsAllow(a) )
	{
		//XXX
		ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
		a = gsMakeBlock(H,a);
		ATtablePut(alphas,(ATerm) a,(ATerm) l);
		return a;
	} else if ( gsIsSync(a) )
	{
		ATermAppl p = ATAgetArgument(a,0);
		ATermAppl q = ATAgetArgument(a,1);
		ATermList l;

		p = PushBlock(H,p);
		q = PushBlock(H,q);

		l = sync_list((ATermList) ATtableGet(alphas,(ATerm) p),(ATermList) ATtableGet(alphas,(ATerm) q));

		a = gsMakeSync(p,q);

		ATtablePut(alphas,(ATerm) a,(ATerm) l);

		return a;
	} else if ( gsIsAtTime(a) )
	{
		ATermAppl p = ATAgetArgument(a,0);
		ATermAppl c = ATAgetArgument(a,1);
		ATermList l;

		p = PushBlock(H,p);

		l = (ATermList) ATtableGet(alphas,(ATerm) p);

		a = gsMakeAtTime(p,c);

		ATtablePut(alphas,(ATerm) a,(ATerm) l);

		return a;
	} else if ( gsIsSeq(a) )
	{
		ATermAppl p = ATAgetArgument(a,0);
		ATermAppl q = ATAgetArgument(a,1);
		ATermList l;

		p = PushBlock(H,p);
		q = PushBlock(H,q);

		l = merge_list((ATermList) ATtableGet(alphas,(ATerm) p),(ATermList) ATtableGet(alphas,(ATerm) q));

		a = gsMakeSeq(p,q);

		ATtablePut(alphas,(ATerm) a,(ATerm) l);

		return a;
	} else if ( gsIsCond(a) )
	{
		ATermAppl c = ATAgetArgument(a,0);
		ATermAppl p = ATAgetArgument(a,1);
		ATermAppl q = ATAgetArgument(a,2);
		ATermList l;

		p = PushBlock(H,p);
		q = PushBlock(H,q);

		l = merge_list((ATermList) ATtableGet(alphas,(ATerm) p),(ATermList) ATtableGet(alphas,(ATerm) q));

		a = gsMakeCond(c,p,q);

		ATtablePut(alphas,(ATerm) a,(ATerm) l);

		return a;
	} else if ( gsIsBInit(a) )
	{
		// XXX
		ATermAppl p = ATAgetArgument(a,0);
		ATermAppl q = ATAgetArgument(a,1);
		ATermList l;

		p = PushBlock(H,p);
		q = PushBlock(H,q);

		l = merge_list((ATermList) ATtableGet(alphas,(ATerm) p),(ATermList) ATtableGet(alphas,(ATerm) q));

		a = gsMakeBInit(p,q);

		ATtablePut(alphas,(ATerm) a,(ATerm) l);

		return a;
	} else if ( gsIsMerge(a) )
	{
		ATermAppl p = ATAgetArgument(a,0);
		ATermAppl q = ATAgetArgument(a,1);
		ATermList l;

		p = PushBlock(H,p);
		q = PushBlock(H,q);

		l = merge_list((ATermList) ATtableGet(alphas,(ATerm) p),(ATermList) ATtableGet(alphas,(ATerm) q));

		a = gsMakeMerge(p,q);

		ATtablePut(alphas,(ATerm) a,(ATerm) l);

		return a;
	} else if ( gsIsLMerge(a) )
	{
		ATermAppl p = ATAgetArgument(a,0);
		ATermAppl q = ATAgetArgument(a,1);
		ATermList l;

		p = PushBlock(H,p);
		q = PushBlock(H,q);

		l = merge_list((ATermList) ATtableGet(alphas,(ATerm) p),(ATermList) ATtableGet(alphas,(ATerm) q));

		a = gsMakeLMerge(p,q);

		ATtablePut(alphas,(ATerm) a,(ATerm) l);

		return a;
	} else if ( gsIsChoice(a) )
	{
		ATermAppl p = ATAgetArgument(a,0);
		ATermAppl q = ATAgetArgument(a,1);
		ATermList l;

		p = PushBlock(H,p);
		q = PushBlock(H,q);

		l = merge_list((ATermList) ATtableGet(alphas,(ATerm) p),(ATermList) ATtableGet(alphas,(ATerm) q));

		a = gsMakeChoice(p,q);

		ATtablePut(alphas,(ATerm) a,(ATerm) l);

		return a;
	}

	return NULL;
}

static ATermAppl PushHide(ATermList I, ATermAppl a)
{
	return gsMakeHide(I,a);
}

static ATermAppl PushComm(ATermList C, ATermAppl a)
{
//gsprintf("%T ",C);PrintPart_C(stdout,a);printf("\n\n");
	if ( gsIsAction(a) )
	{
		return a;
	} else if ( gsIsProcess(a) )
	{
		ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);

		l = apply_comm_list(l,C);
		// XXX also adjust C?

		a = gsMakeComm(C,a);

		ATtablePut(alphas,(ATerm) a,(ATerm) l);

		return a;
	} else if ( gsIsDelta(a) || gsIsTau(a) )
	{
		return a;
	} else if ( gsIsSum(a) )
	{
		ATermAppl p = ATAgetArgument(a,1);
		ATermList l;

		p = PushComm(C,p);

		l = (ATermList) ATtableGet(alphas,(ATerm) p);

		a = ATsetArgument(a,(ATerm) p, 1);

		ATtablePut(alphas,(ATerm) a,(ATerm) l);

		return a;
	} else if ( gsIsBlock(a) )
	{
		//XXX
		ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
		a = gsMakeComm(C,a);
		ATtablePut(alphas,(ATerm) a,(ATerm) apply_comm_list(l,C));
		return a;
	} else if ( gsIsHide(a) )
	{
		//XXX
		ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
		a = gsMakeComm(C,a);
		ATtablePut(alphas,(ATerm) a,(ATerm) apply_comm_list(l,C));
		return a;
	} else if ( gsIsRename(a) )
	{
		//XXX
		ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
		a = gsMakeComm(C,a);
		ATtablePut(alphas,(ATerm) a,(ATerm) apply_comm_list(l,C));
		return a;
	} else if ( gsIsComm(a) )
	{
		//XXX
		ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
		a = gsMakeComm(C,a);
		ATtablePut(alphas,(ATerm) a,(ATerm) apply_comm_list(l,C));
		return a;
	} else if ( gsIsAllow(a) )
	{
		//XXX
		ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
		a = gsMakeComm(C,a);
		ATtablePut(alphas,(ATerm) a,(ATerm) apply_comm_list(l,C));
		return a;
	} else if ( gsIsSync(a) || gsIsMerge(a) || gsIsLMerge(a) )
	{
		if ( can_split_comm(C) )
		{
			ATermAppl p = ATAgetArgument(a,0);
			ATermAppl q = ATAgetArgument(a,1);
			ATermList lp = (ATermList) ATtableGet(alphas,(ATerm) p);
			ATermList lq = (ATermList) ATtableGet(alphas,(ATerm) q);
			ATermList Cp = ATmakeList0();
			ATermList Cq = ATmakeList0();
			ATermList Ca = ATmakeList0();
			ATermList l;

//gsprintf("split: %T   %T\n\n",lp,lq);

			for (; !ATisEmpty(C); C=ATgetNext(C))
			{
				bool bp = disjoint_list(ATLgetArgument(ATAgetArgument(ATAgetFirst(C),0),0),lp);
				bool bq = disjoint_list(ATLgetArgument(ATAgetArgument(ATAgetFirst(C),0),0),lq);
				
				if ( !bp )
				{
					if ( !bq )
					{
						Ca = ATinsert(Ca,ATgetFirst(C));
					} else {
						Cp = ATinsert(Cp,ATgetFirst(C));
					}
				} else if ( !bq )
				{
					Cq = ATinsert(Cq,ATgetFirst(C));
				}
			}

//gsprintf("split: %T   %T   %T\n\n",Cp,Cq,Ca);
			if ( !(ATisEmpty(Cp) && ATisEmpty(Cq)) )
			{
				if ( !ATisEmpty(Cp) )
				{
					p = PushComm(Cp,p);
				}
				if ( !ATisEmpty(Cq) )
				{
					q = PushComm(Cq,q);
				}
				l = merge_list((ATermList) ATtableGet(alphas,(ATerm) p),(ATermList) ATtableGet(alphas,(ATerm) q));
				if ( gsIsSync(a) )
				{
					a = gsMakeSync(p,q);
				} else if ( gsIsMerge(a) )
				{
					a = gsMakeMerge(p,q);
				} else if ( gsIsLMerge(a) )
				{
					a = gsMakeLMerge(p,q);
				}
				ATtablePut(alphas,(ATerm) a,(ATerm) l);
			} else {
				l = (ATermList) ATtableGet(alphas,(ATerm) a);
			}

			if ( !ATisEmpty(Ca) )
			{
				a = gsMakeComm(Ca,a);
				ATtablePut(alphas,(ATerm) a,(ATerm) apply_comm_list(l,Ca));
			}

			return a;
		} else {
//gsprintf("nosplit\n\n");			
			ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
			a = gsMakeComm(C,a);
			ATtablePut(alphas,(ATerm) a,(ATerm) apply_comm_list(l,C));
			return a;
		}
	} else if ( gsIsAtTime(a) )
	{
		ATermAppl p = ATAgetArgument(a,0);
		ATermAppl c = ATAgetArgument(a,1);
		ATermList l;

		p = PushComm(C,p);

		l = (ATermList) ATtableGet(alphas,(ATerm) p);

		a = gsMakeAtTime(p,c);

		ATtablePut(alphas,(ATerm) a,(ATerm) l);

		return a;
	} else if ( gsIsSeq(a) )
	{
		ATermAppl p = ATAgetArgument(a,0);
		ATermAppl q = ATAgetArgument(a,1);
		ATermList l;

		p = PushComm(C,p);
		q = PushComm(C,q);

		l = merge_list((ATermList) ATtableGet(alphas,(ATerm) p),(ATermList) ATtableGet(alphas,(ATerm) q));

		a = gsMakeSeq(p,q);

		ATtablePut(alphas,(ATerm) a,(ATerm) l);

		return a;
	} else if ( gsIsCond(a) )
	{
		ATermAppl c = ATAgetArgument(a,0);
		ATermAppl p = ATAgetArgument(a,1);
		ATermAppl q = ATAgetArgument(a,2);
		ATermList l;

		p = PushComm(C,p);
		q = PushComm(C,q);

		l = merge_list((ATermList) ATtableGet(alphas,(ATerm) p),(ATermList) ATtableGet(alphas,(ATerm) q));

		a = gsMakeCond(c,p,q);

		ATtablePut(alphas,(ATerm) a,(ATerm) l);

		return a;
	} else if ( gsIsBInit(a) )
	{
		// XXX
		ATermAppl p = ATAgetArgument(a,0);
		ATermAppl q = ATAgetArgument(a,1);
		ATermList l;

		p = PushComm(C,p);
		q = PushComm(C,q);

		l = merge_list((ATermList) ATtableGet(alphas,(ATerm) p),(ATermList) ATtableGet(alphas,(ATerm) q));

		a = gsMakeBInit(p,q);

		ATtablePut(alphas,(ATerm) a,(ATerm) l);

		return a;
	} else if ( gsIsChoice(a) )
	{
		ATermAppl p = ATAgetArgument(a,0);
		ATermAppl q = ATAgetArgument(a,1);
		ATermList l;

		p = PushComm(C,p);
		q = PushComm(C,q);

		l = merge_list((ATermList) ATtableGet(alphas,(ATerm) p),(ATermList) ATtableGet(alphas,(ATerm) q));

		a = gsMakeChoice(p,q);

		ATtablePut(alphas,(ATerm) a,(ATerm) l);

		return a;
	}

	return NULL;
}

static ATermAppl PushAllow(ATermList V, ATermAppl a)
{
	return gsMakeAllow(V,a);
}

ATermAppl ApplyAlpha(ATermAppl a)
{
	ATermList l = NULL;

//PrintPart_C(stdout,a);printf("\n\n");
	if ( gsIsProcEqn(a) )
	{
		if ( ATtableGet(alphas,ATgetArgument(a,1)) == NULL )
		{
			ATermAppl p = ATAgetArgument(a,3);
		
			ATtablePut(alphas,ATgetArgument(a,1),(ATerm) ATmakeList0());

			p = ApplyAlpha(p);

			l = (ATermList) ATtableGet(alphas,(ATerm) p);

//gsprintf("%T: %T\n",ATgetArgument(a,1),l);
			ATtablePut(alphas,ATgetArgument(a,1),(ATerm) l);

			a = ATsetArgument(a,(ATerm) p,3);
		}
		l = NULL;
	} else if ( gsIsAction(a) )
	{
		l = ATmakeList1((ATerm) ATmakeList1(ATgetArgument(a,0)));
//gsprintf("%T\n\n",l);
	} else if ( gsIsProcess(a) )
	{
//gsprintf("%T\n",a);
		l = (ATermList) ATtableGet(alphas,ATgetArgument(a,0));
//if (l == NULL ) gsprintf("(null)\n\n"); else gsprintf("%T\n\n",l);
		if ( l == NULL )
		{
			l = procs;
			procs = ATmakeList0();
			for (; !ATisEmpty(l); l=ATgetNext(l))
			{
				if ( ATisEqual(ATgetArgument(a,0),ATgetArgument(ATAgetFirst(l),1)) )
				{
					procs = ATinsert(procs,(ATerm) ApplyAlpha(ATAgetFirst(l)));
				} else {
					procs = ATinsert(procs,ATgetFirst(l));
				}
			}
			procs = ATreverse(procs);
			l = (ATermList) ATtableGet(alphas,ATgetArgument(a,0));
		}
	} else if ( gsIsDelta(a) || gsIsTau(a) )
	{
		l = NULL;
	} else if ( gsIsSum(a) )
	{
		ATermAppl p = ATAgetArgument(a,1);

		p = ApplyAlpha(p);

		l = (ATermList) ATtableGet(alphas,(ATerm) p);

		a = ATsetArgument(a,(ATerm) p, 1);
	} else if ( gsIsBlock(a) )
	{
		ATermAppl p = ATAgetArgument(a,1);

		p = ApplyAlpha(p);

		l = NULL;

		a = PushBlock(ATLgetArgument(a,0),p);
	} else if ( gsIsHide(a) )
	{
		ATermAppl p = ATAgetArgument(a,1);

		p = ApplyAlpha(p);

		l = (ATermList) ATtableGet(alphas,(ATerm) p);

		a = PushHide(ATLgetArgument(a,0),p);
	} else if ( gsIsRename(a) )
	{
		ATermAppl p = ATAgetArgument(a,1);

		p = ApplyAlpha(p);

		l = (ATermList) ATtableGet(alphas,(ATerm) p);

		a = ATsetArgument(a,(ATerm) p,1);
	} else if ( gsIsComm(a) )
	{
		ATermAppl p = ATAgetArgument(a,1);

		p = ApplyAlpha(p);

		l = (ATermList) ATtableGet(alphas,(ATerm) p);

		a = PushComm(ATLgetArgument(a,0),p);
	} else if ( gsIsAllow(a) )
	{
		ATermAppl p = ATAgetArgument(a,1);

		p = ApplyAlpha(p);

		l = (ATermList) ATtableGet(alphas,(ATerm) p);

		a = PushAllow(ATLgetArgument(a,0),p);
	} else if ( gsIsAtTime(a) )
	{
		ATermAppl p = ATAgetArgument(a,0);
		ATermAppl c = ATAgetArgument(a,1);

		p = ApplyAlpha(p);

		l = (ATermList) ATtableGet(alphas,(ATerm) p);

		a = gsMakeAtTime(p,c);
	} else if ( gsIsSeq(a) )
	{
		ATermAppl p = ATAgetArgument(a,0);
		ATermAppl q = ATAgetArgument(a,1);

		p = ApplyAlpha(p);
		q = ApplyAlpha(q);

		l = merge_list((ATermList) ATtableGet(alphas,(ATerm) p),(ATermList) ATtableGet(alphas,(ATerm) q));

		a = gsMakeSeq(p,q);
	} else if ( gsIsCond(a) )
	{
		ATermAppl c = ATAgetArgument(a,0);
		ATermAppl p = ATAgetArgument(a,1);
		ATermAppl q = ATAgetArgument(a,2);

		p = ApplyAlpha(p);
		q = ApplyAlpha(q);

		l = merge_list((ATermList) ATtableGet(alphas,(ATerm) p),(ATermList) ATtableGet(alphas,(ATerm) q));

		a = gsMakeCond(c,p,q);
	} else if ( gsIsBInit(a) )
	{
		// XXX
		ATermAppl p = ATAgetArgument(a,0);
		ATermAppl q = ATAgetArgument(a,1);

		p = ApplyAlpha(p);
		q = ApplyAlpha(q);

		l = merge_list((ATermList) ATtableGet(alphas,(ATerm) p),(ATermList) ATtableGet(alphas,(ATerm) q));

		a = gsMakeBInit(p,q);
	} else if ( gsIsMerge(a) )
	{
		ATermAppl p = ATAgetArgument(a,0);
		ATermAppl q = ATAgetArgument(a,1);

		p = ApplyAlpha(p);
		q = ApplyAlpha(q);

		ATermList l1=ATLtableGet(alphas,(ATerm) p);
		ATermList l2=ATLtableGet(alphas,(ATerm) q);

		l = merge_list(merge_list(l1,l2),sync_list(l1,l2));

		a = gsMakeMerge(p,q);
	} else if ( gsIsLMerge(a) )
	{
		ATermAppl p = ATAgetArgument(a,0);
		ATermAppl q = ATAgetArgument(a,1);

		p = ApplyAlpha(p);
		q = ApplyAlpha(q);

		ATermList l1=ATLtableGet(alphas,(ATerm) p);
		ATermList l2=ATLtableGet(alphas,(ATerm) q);

		l = merge_list(merge_list(l1,l2),sync_list(l1,l2));

		a = gsMakeLMerge(p,q);
	} else if ( gsIsSync(a) )
	{
		ATermAppl p = ATAgetArgument(a,0);
		ATermAppl q = ATAgetArgument(a,1);

		p = ApplyAlpha(p);
		q = ApplyAlpha(q);

		ATermList l1=ATLtableGet(alphas,(ATerm) p);
		ATermList l2=ATLtableGet(alphas,(ATerm) q);

		l = merge_list(merge_list(l1,l2),sync_list(l1,l2));

		a = gsMakeSync(p,q);
	} else if ( gsIsChoice(a) )
	{
		ATermAppl p = ATAgetArgument(a,0);
		ATermAppl q = ATAgetArgument(a,1);

		p = ApplyAlpha(p);
		q = ApplyAlpha(q);
		l = merge_list((ATermList) ATtableGet(alphas,(ATerm) p),(ATermList) ATtableGet(alphas,(ATerm) q));

		a = gsMakeChoice(p,q);
	}

	if ( (l != NULL) /*&& (ATtableGet(alphas,(ATerm) a) == NULL)*/ )
	{
		ATtablePut(alphas,(ATerm) a,(ATerm) l);
	}

//if ( l != NULL ) gsprintf("%T\n",ATtableGet(alphas,a));
//PrintPart_C(stdout,a);printf(".\n\n");
	return a;
}

ATermAppl Alpha(ATermAppl Spec)
{
	ATermAppl init;

	procs = ATLgetArgument(ATAgetArgument(Spec,5),0);
	ATprotectList(&procs);
	init = ATAgetArgument(ATAgetArgument(Spec,6),1);

	alphas = ATtableCreate(10000,80);
	ATtablePut(alphas,(ATerm) gsMakeDelta(),(ATerm) ATmakeList0());
	ATtablePut(alphas,(ATerm) gsMakeTau(),(ATerm) ATmakeList0());

	ATermList l = ATmakeList0();
	for (; !ATisEmpty(procs); procs=ATgetNext(procs))
	{
		l = ATinsert(l,(ATerm) ApplyAlpha(ATAgetFirst(procs)));
	}
	procs = ATreverse(l);

	init = ApplyAlpha(init);

	ATtableDestroy(alphas);

	Spec = ATsetArgument(Spec,(ATerm) gsMakeProcEqnSpec(procs),5);
	Spec = ATsetArgument(Spec,(ATerm) gsMakeInit(ATLgetArgument(ATAgetArgument(Spec,6),0),init),6);

	return Spec;
}

#ifdef __cplusplus
}
#endif
