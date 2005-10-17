/* $Id: gsrewr.c,v 1.2 2005/04/08 12:33:51 muck Exp $ */
#ifdef __cplusplus
extern "C" {
#endif

#define NAME "libgsalpha"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <aterm2.h>
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libprint_c.h"
#include "libgsalpha.h"

static ATermTable alphas;
static ATermTable procs;
static ATermAppl gsApplyAlpha(ATermAppl a);

static inline ATermAppl untypeA(ATermAppl Act){
  //returns the untyped action name of Act
  return ATAgetArgument(Act,0);
}

static inline ATermList typeA(ATermAppl Act){
  //returns the type of Act
  return ATLgetArgument(Act,1);
}

static inline ATermList untypeMA(ATermList MAct){
  //returns "untyped multiaction name" of MAct
  ATermList r=ATmakeList0();
  for(;!ATisEmpty(MAct);MAct=ATgetNext(MAct))
    r=ATinsert(r,(ATerm)untypeA(ATAgetFirst(MAct)));
  return ATreverse(r);
}

static inline ATermList typeMA(ATermList MAct){
  //returns "type signature" of MAct
  ATermList r=ATmakeList0();
  for(;!ATisEmpty(MAct);MAct=ATgetNext(MAct))
    r=ATinsert(r,(ATerm)typeA(ATAgetFirst(MAct)));
  return ATreverse(r);
}

static inline ATermList untypeMAL(ATermList LMAct){
  //returns List of "untyped multiaction name" of List(MAct)
  ATermList r=ATmakeList0();
  for(;!ATisEmpty(LMAct);LMAct=ATgetNext(LMAct))
    r=ATinsert(r,(ATerm)untypeMA(ATLgetFirst(LMAct)));
  return ATreverse(r);
}

static inline ATermList typeMAL(ATermList LMAct){
  //returns List of "type signature" of List(MAct)
  ATermList r=ATmakeList0();
  for(;!ATisEmpty(LMAct);LMAct=ATgetNext(LMAct))
    r=ATinsert(r,(ATerm)typeMA(ATLgetFirst(LMAct)));
  return ATreverse(r);
}

static ATermList sync_mact(ATermList a, ATermList b)
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

static ATermList intersect_list(ATermList l, ATermList m)
{
	ATermList n;

	n = ATmakeList0();
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		if ( ATindexOf(m,ATgetFirst(l),0) >= 0 )
		{
			n = ATinsert(n,ATgetFirst(l));
		}
	}
	n = ATreverse(n);

	return n;
}

static ATermList filter_restrict_list(ATermList l, ATermList H){
  //filters l not to contain untyped actions from H
  
  ATermList m = ATmakeList0();

  for (; !ATisEmpty(l); l=ATgetNext(l)){
    ATermList tH = H;
    bool b = true;
    ATermList ma=untypeMA(ATLgetFirst(l));
    for (; !ATisEmpty(tH); tH=ATgetNext(tH)){
      if ( ATindexOf(ma,(ATerm)ATAgetFirst(tH),0) >= 0 ){
	b = false;
	break;
      }
    }
    if ( b ){
      m = ATinsert(m,(ATerm)ATLgetFirst(l));
    }
  }
  return ATreverse(m);
}

static ATermList filter_hide_list(ATermList l, ATermList I){
  //filters l renaming untyped actions from I to tau
  ATermList m;

  m = ATmakeList0();
  for (; !ATisEmpty(l); l=ATgetNext(l)){
    ATermList new_ma=ATmakeList0();
    for(ATermList ma=ATLgetFirst(l);!ATisEmpty(ma); ma=ATgetNext(ma))
      if(ATindexOf(I,(ATerm)untypeA(ATAgetFirst(ma)),0)<0) 
	new_ma=ATinsert(new_ma,ATgetFirst(ma));
    if(ATgetLength(new_ma) && ATindexOf(m,(ATerm)new_ma,0)<0) m=ATinsert(m,(ATerm)ATreverse(new_ma));
  }
  
  m = ATreverse(m);
  return m;
}

static ATermList filter_allow_list(ATermList l, ATermList V){
  //filters l to contain only multiactions matching the untiped multiactions from V
  ATermList m = ATmakeList0();
  for (; !ATisEmpty(l); l=ATgetNext(l)){
    ATermList tV = V;
    bool b=false;
    
    for (; !ATisEmpty(tV); tV=ATgetNext(tV)){
      if ( ATisEqual(untypeMA(ATLgetFirst(l)),ATLgetArgument(ATAgetFirst(tV),0)) ){
	b = true;
	break;
      }
    }
    if ( b ){
      m = ATinsert(m,ATgetFirst(l));
    }
  }
  return ATreverse(m);
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
			n = ATinsert(n,(ATerm) sync_mact(ATLgetFirst(l),ATLgetFirst(o)));
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

static ATermList apply_hide(ATermList I, ATermList MAct){
  //apply hiding I to MAct
  ATermList r=ATmakeList0();

  for (; !ATisEmpty(MAct); MAct=ATgetNext(MAct) ){
    ATermAppl Act=ATAgetFirst(MAct);
    if(ATindexOf(I,(ATerm)Act,0)<0) 
      r=ATinsert(r,(ATerm)Act);
  }
  return ATreverse(r);
}

static ATermList extend_hide(ATermList V, ATermList I, ATermList L){
  // Extend V to contain hidings of L with theta_I
  ATermList r=ATmakeList0();

  for (; !ATisEmpty(L); L=ATgetNext(L) ){
    ATermAppl ma=gsMakeMultActName(ATLgetFirst(L));
    ATermAppl maH=gsMakeMultActName(apply_hide(I,ATLgetFirst(L)));
    if((ATisEqual(maH,gsMakeMultActName(ATmakeList0())) || ATindexOf(V,(ATerm)maH,0)>=0 )&& ATindexOf(r,(ATerm)ma,0)<0) 
      r=ATinsert(r,(ATerm)ma);
  }
  return ATreverse(r);
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

static ATermList apply_comm(ATermList l, ATermList C){
  //applies C to a multiaction l
  if ( ATisEmpty(l) || ATisEmpty(ATgetNext(l)) )
    return l;

  ATermAppl a = ATAgetFirst(l);
  ATermList r = ATgetNext(l);
  ATermList m = ATmakeList0();
  ATermList tC = C;

  bool ca=false;

  for (; !ATisEmpty(tC); tC=ATgetNext(tC)){
    ATermList c = ATLgetArgument(ATAgetArgument(ATAgetFirst(tC),0),0);
    if ( ATindexOf(c,ATgetArgument(a,0),0) >= 0 ){
      ATermList s = ATLgetArgument(a,1);
      ATermList tr = r;
      bool b=true;
	
      c = ATremoveElement(c,ATgetArgument(a,0));
      for (; !ATisEmpty(c); c=ATgetNext(c)){
	ATermAppl act = gsMakeActId(ATAgetFirst(c),s);
	if ( ATindexOf(tr,(ATerm) act,0) >= 0 ){
	  tr = ATremoveElement(tr,(ATerm) act);
	} else {
	  b = false;
	  break;
	}
      }
      if ( b ){
	ca=true;
	if ( gsIsNil(ATAgetArgument(ATAgetFirst(tC),1)) ){
	  m=apply_comm(tr,C);
	  break;
	} 
	else {
	  m=sync_mact(ATmakeList1((ATerm)gsMakeActId(ATAgetArgument(ATAgetFirst(tC),1),s)),apply_comm(tr,C));
	}
      }
    }
  }
  
  if(!ca) m = sync_mact(ATmakeList1((ATerm) a),apply_comm(r,C));
  
  //gsDebugMsg("apply_comm done: l: %T m: %T; C: %P\n\n",l,m,C);
  
  return m;
}

static ATermList filter_comm_list(ATermList l, ATermList C){
  //apply C to all elements of l
  ATermList m;
  
  m = ATmakeList0();
  for (; !ATisEmpty(l); l=ATgetNext(l)){
    m = ATinsert(m,(ATerm)apply_comm(ATLgetFirst(l),C));
  }
  m = ATreverse(m);
  
  return m;
}

static ATermAppl PushBlock(ATermList H, ATermAppl a){
  if ( gsIsDelta(a) || gsIsTau(a) ){
    return a;
  } 
  else if ( gsIsAction(a) ){
    return (ATindexOf(H,(ATerm)ATAgetArgument(a,0),0)>=0)?gsMakeDelta():a;
  } 
  else if ( gsIsProcess(a) ){
    ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
    
    l = filter_restrict_list(l,H);
    // XXX also adjust H
    
    a = gsMakeBlock(H,a);
    
    ATtablePut(alphas,(ATerm) a,(ATerm) l);
    
    return a;
  } 
  else if ( gsIsBlock(a) ){
    return PushBlock(merge_list(H,ATLgetArgument(a,0)),ATAgetArgument(a,1));
  } 
  else if ( gsIsHide(a) ){
    ATermAppl p = ATAgetArgument(a,1);
    ATermList l;
    
    H = list_minus(H,ATLgetArgument(a,0));
    
    l = (ATermList) ATtableGet(alphas,(ATerm) p);
    l = filter_restrict_list(l,H);
    
    p = PushBlock(H,p);
    
    a = gsMakeHide(ATLgetArgument(a,0),p);
    
    ATtablePut(alphas,(ATerm) a,(ATerm) l);
    
    return a;
  } 
  else if ( gsIsRename(a) ){
    // XXX
    ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
    a = gsMakeBlock(H,a);
    ATtablePut(alphas,(ATerm) a,(ATerm) l);
    return a;
  } 
  else if ( gsIsComm(a) ){
    //XXX
    ATermList l = ATLtableGet(alphas,(ATerm) a);
    assert(l);
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
	l = ATLtableGet(alphas,(ATerm) a);
	a = gsMakeComm(C,a);
	ATtablePut(alphas,(ATerm) a,(ATerm) filter_comm_list(l,C));
      } else {
      l = (ATermList) ATtableGet(alphas,(ATerm) a);
    }
    
    if ( !ATisEmpty(Ha) )
      {
	a = gsMakeBlock(Ha,a);
	ATtablePut(alphas,(ATerm) a,(ATerm) filter_restrict_list(l,Ha));
      }
    
    return a;
  } 
  else if ( gsIsAllow(a) ){
    //XXX
    ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
    a = gsMakeBlock(H,a);
    ATtablePut(alphas,(ATerm) a,(ATerm) l);
    return a;
  }
  else if ( gsIsSum(a) || gsIsAtTime(a) || gsIsChoice(a) || gsIsSeq(a) 
	    || gsIsCond(a) || gsIsSync(a) || gsIsMerge(a) || gsIsLMerge(a) || gsIsBInit(a)){
    // all distributing rules together
    short ia1=0,ia2=1,args=2;
    if(gsIsCond(a) || gsIsSum(a)) { ia1=1; ia2=2; }
    if(gsIsSum(a) || gsIsAtTime(a) || gsIsBInit(a)) args=1; //second argument of BInit does not matter 
    
    ATermAppl p,q;
    p = PushBlock(H,ATAgetArgument(a,ia1));
    if(args==2) q = PushBlock(H,ATAgetArgument(a,ia2));
    
    ATermList l,l1,l2;
    l=l1=ATLtableGet(alphas,(ATerm) p);
    if(args==2) l2=ATLtableGet(alphas,(ATerm) q);
    
    if(args==2) l = merge_list(l,l2);
    if(gsIsSync(a) || gsIsMerge(a) || gsIsLMerge(a)) 
      l = merge_list(l,sync_list(l1,l2));
    
    a=ATsetArgument(a,(ATerm)p,ia1);
    if(args==2) a=ATsetArgument(a,(ATerm)q,ia2);
    
    ATtablePut(alphas,(ATerm) a,(ATerm) l);
    
    return a;
  }

  assert(0);
}

static ATermAppl PushHide(ATermList I, ATermAppl a){
  if ( gsIsDelta(a) || gsIsTau(a) ){
    return a;
  } 
  else if ( gsIsAction(a) ){
    return (ATindexOf(I,(ATerm)ATAgetArgument(a,0),0)>=0)?gsMakeTau():a;
  } 
  else if ( gsIsProcess(a) ){
    ATermList l = ATLtableGet(alphas,(ATerm) a);
    
    l = filter_hide_list(l,I);
    // XXX also adjust I?
    
    a = gsMakeHide(I,a);
    
    ATtablePut(alphas,(ATerm) a,(ATerm) l);
    
    return a;
  } 
  else if ( gsIsBlock(a) ){
    //XXX
    ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
    a = gsMakeHide(I,a);
    ATtablePut(alphas,(ATerm) a,(ATerm) filter_hide_list(l,I));
    return a;
  }
  else if ( gsIsHide(a) ){
    return PushHide(merge_list(I,ATLgetArgument(a,0)),ATAgetArgument(a,1));
  } 
  else if ( gsIsRename(a) ){
    //XXX
    ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
    a = gsMakeHide(I,a);
    ATtablePut(alphas,(ATerm) a,(ATerm) filter_hide_list(l,I));
    return a;
  } 
  else if ( gsIsComm(a) ){
    //XXX
    ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
    a = gsMakeHide(I,a);
    ATtablePut(alphas,(ATerm) a,(ATerm) filter_hide_list(l,I));
    return a;
  } 
  else if ( gsIsAllow(a) ){
    //XXX
    ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
    a = gsMakeHide(I,a);
    ATtablePut(alphas,(ATerm) a,(ATerm) filter_hide_list(l,I));
    return a;
  } 
  else if ( gsIsSum(a) || gsIsAtTime(a) || gsIsChoice(a) || gsIsSeq(a) 
	      || gsIsCond(a) || gsIsSync(a) || gsIsMerge(a) || gsIsLMerge(a) || gsIsBInit(a)){
    // all distributing rules together
    short ia1=0,ia2=1,args=2;
    if(gsIsCond(a) || gsIsSum(a)) { ia1=1; ia2=2; }
    if(gsIsSum(a) || gsIsAtTime(a) || gsIsBInit(a)) args=1; //second argument of BInit does not matter 
    
    ATermAppl p,q;
    p = PushHide(I,ATAgetArgument(a,ia1));
    if(args==2) q = PushHide(I,ATAgetArgument(a,ia2));
    
    ATermList l,l1,l2;
    l=l1=ATLtableGet(alphas,(ATerm) p);
    if(args==2) l2=ATLtableGet(alphas,(ATerm) q);
    
    if(args==2) l = merge_list(l,l2);
    if(gsIsSync(a) || gsIsMerge(a) || gsIsLMerge(a)) 
      l = merge_list(l,sync_list(l1,l2));
    
    a=ATsetArgument(a,(ATerm)p,ia1);
    if(args==2) a=ATsetArgument(a,(ATerm)q,ia2);
    
    ATtablePut(alphas,(ATerm) a,(ATerm) l);
    
    return a;
  }
  assert(0);
}

static ATermAppl PushAllow(ATermList V, ATermAppl a){
  if ( gsIsDelta(a) || gsIsTau(a) ){
    return a;
  } 
  else if ( gsIsAction(a) ){
    return (ATindexOf(V,(ATerm)gsMakeMultActName(ATmakeList1((ATerm)ATAgetArgument(ATAgetArgument(a,0),0))),
		      0)>=0)?a:gsMakeDelta();
  }
  else if ( gsIsProcess(a) ){
    ATermList l = ATLtableGet(alphas,(ATerm) a);
    
    l = filter_allow_list(l,V);
    // XXX also adjust V?
    
    a = gsMakeAllow(V,a);
    
    ATtablePut(alphas,(ATerm) a,(ATerm) l);
    
    return a;
  } 
  else if ( gsIsHide(a) ){
    ATermList I=ATLgetArgument(a,0);
    ATermAppl p=ATAgetArgument(a,1);

    ATermList l = ATLtableGet(alphas,(ATerm) p);
    assert(l);

    ATermList V1 = extend_hide(V,I,untypeMAL(l));

    p = PushAllow(V1,p);

    l = ATLtableGet(alphas,(ATerm) p);
    a = ATsetArgument(a,(ATerm)p,1);
    ATtablePut(alphas,(ATerm) a,(ATerm)filter_hide_list(l,I));
    return a;
  } 
  else if ( gsIsRename(a) ){
    //XXX
    ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
    a = gsMakeAllow(V,a);
    ATtablePut(alphas,(ATerm) a,(ATerm) filter_allow_list(l,V));
    return a;
  } 
  else if ( gsIsAllow(a) ){
    return PushAllow(intersect_list(V,ATLgetArgument(a,0)),ATAgetArgument(a,1));
  } 
  else if ( gsIsComm(a) ){
    //XXX
    ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
    a = gsMakeAllow(V,a);
    ATtablePut(alphas,(ATerm) a,(ATerm) filter_allow_list(l,V));
    return a;
  } 
  else if ( gsIsSync(a) || gsIsMerge(a) || gsIsLMerge(a) ){
    if ( can_split_comm(V) )
      {
	ATermAppl p = ATAgetArgument(a,0);
			ATermAppl q = ATAgetArgument(a,1);
			ATermList lp = (ATermList) ATtableGet(alphas,(ATerm) p);
			ATermList lq = (ATermList) ATtableGet(alphas,(ATerm) q);
			ATermList Vp = ATmakeList0();
			ATermList Vq = ATmakeList0();
			ATermList Va = ATmakeList0();
			ATermList l;

//gsprintf("split: %T   %T\n\n",lp,lq);

			for (; !ATisEmpty(V); V=ATgetNext(V))
			{
				bool bp = disjoint_list(ATLgetArgument(ATAgetArgument(ATAgetFirst(V),0),0),lp);
				bool bq = disjoint_list(ATLgetArgument(ATAgetArgument(ATAgetFirst(V),0),0),lq);
				
				if ( !bp )
				{
					if ( !bq )
					{
						Va = ATinsert(Va,ATgetFirst(V));
					} else {
						Vp = ATinsert(Vp,ATgetFirst(V));
					}
				} else if ( !bq )
				{
					Vq = ATinsert(Vq,ATgetFirst(V));
				}
			}

//gsprintf("split: %T   %T   %T\n\n",Vp,Vq,Va);
			if ( !(ATisEmpty(Vp) && ATisEmpty(Vq)) )
			{
				if ( !ATisEmpty(Vp) )
				{
					p = PushAllow(Vp,p);
				}
				if ( !ATisEmpty(Vq) )
				{
					q = PushAllow(Vq,q);
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

			if ( !ATisEmpty(Va) )
			{
				a = gsMakeAllow(Va,a);
				ATtablePut(alphas,(ATerm) a,(ATerm) filter_allow_list(l,Va));
			}

			return a;
		} else {
//gsprintf("nosplit\n\n");			
			ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
			a = gsMakeAllow(V,a);
			ATtablePut(alphas,(ATerm) a,(ATerm) filter_allow_list(l,V));
			return a;
		}
  }
  else if ( gsIsBlock(a) || gsIsSum(a) || gsIsAtTime(a) || gsIsChoice(a) || gsIsSeq(a) 
	    || gsIsCond(a) || gsIsSync(a) || gsIsMerge(a) || gsIsLMerge(a) || gsIsBInit(a)){
    // all distributing rules together
    short ia1=0,ia2=1,args=2;
    if(gsIsBlock(a) || gsIsCond(a) || gsIsSum(a)) { ia1=1; ia2=2; }
    if(gsIsBlock(a) || gsIsSum(a) || gsIsAtTime(a) || gsIsBInit(a)) args=1; //second argument of BInit does not matter 
    
    ATermAppl p,q;
    p = PushAllow(V,ATAgetArgument(a,ia1));
    if(args==2) q = PushAllow(V,ATAgetArgument(a,ia2));
    
    ATermList l,l1,l2;
    l=l1=ATLtableGet(alphas,(ATerm) p);
    if(args==2) l2=ATLtableGet(alphas,(ATerm) q);
    
    if(args==2) l = merge_list(l,l2);
    if(gsIsSync(a) || gsIsMerge(a) || gsIsLMerge(a)) 
      l = merge_list(l,sync_list(l1,l2));
    
    a=ATsetArgument(a,(ATerm)p,ia1);
    if(args==2) a=ATsetArgument(a,(ATerm)q,ia2);
    
    ATtablePut(alphas,(ATerm) a,(ATerm) l);
    
    return a;
  }
  assert(0);
}

static ATermAppl PushComm(ATermList C, ATermAppl a)
{
//gsprintf("%T ",C);PrintPart_C(stdout,a);printf("\n\n");
	if ( gsIsAction(a) )
	{
		return a;
	} else if ( gsIsProcess(a) )
	{
		ATermList l = ATLtableGet(alphas,(ATerm) a);

		l = filter_comm_list(l,C);
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
		ATtablePut(alphas,(ATerm) a,(ATerm) filter_comm_list(l,C));
		return a;
	} else if ( gsIsHide(a) )
	{
		//XXX
		ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
		a = gsMakeComm(C,a);
		ATtablePut(alphas,(ATerm) a,(ATerm) filter_comm_list(l,C));
		return a;
	} else if ( gsIsRename(a) )
	{
		//XXX
		ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
		a = gsMakeComm(C,a);
		ATtablePut(alphas,(ATerm) a,(ATerm) filter_comm_list(l,C));
		return a;
	} else if ( gsIsComm(a) )
	{
		//XXX
		ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
		a = gsMakeComm(C,a);
		ATtablePut(alphas,(ATerm) a,(ATerm) filter_comm_list(l,C));
		return a;
	} else if ( gsIsAllow(a) )
	{
		//XXX
		ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
		a = gsMakeComm(C,a);
		ATtablePut(alphas,(ATerm) a,(ATerm) filter_comm_list(l,C));
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
				ATtablePut(alphas,(ATerm) a,(ATerm) filter_comm_list(l,Ca));
			}

			return a;
		} else {
//gsprintf("nosplit\n\n");			
			ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
			a = gsMakeComm(C,a);
			ATtablePut(alphas,(ATerm) a,(ATerm) filter_comm_list(l,C));
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
  
static ATermAppl gsApplyAlpha(ATermAppl a){
  if ( gsIsAction(a) ){
    ATermList l = ATLtableGet(alphas,(ATerm)ATAgetArgument(a,0));
    if(!l){
      l = ATmakeList1((ATerm)ATmakeList1(ATgetArgument(a,0)));
      ATtablePut(alphas,(ATerm)ATAgetArgument(a,0),(ATerm)l); //for this action name+type
    }
    ATtablePut(alphas,(ATerm)a,(ATerm)l); //for this full action
  } 
  else if ( gsIsProcess(a) ){
    ATermList l=ATLtableGet(alphas,(ATerm)ATAgetArgument(a,0)); // for this particular process term
    assert(l);
    ATtablePut(alphas,(ATerm)a,(ATerm)l); //for this full process call
  }
  else if ( gsIsBlock(a) ){
    ATermAppl p = ATAgetArgument(a,1);
    
    p = gsApplyAlpha(p);
    a = PushBlock(ATLgetArgument(a,0),p); //takes care about l
  } 
  else if ( gsIsHide(a) ){
    ATermAppl p = ATAgetArgument(a,1);
    
    p = gsApplyAlpha(p);
    a = PushHide(ATLgetArgument(a,0),p); //takes care about l
  } 
  else if ( gsIsRename(a) ){
    ATermAppl p = ATAgetArgument(a,1);
    
    p = gsApplyAlpha(p);
    
    //l=NULL; //ATLtableGet(alphas,(ATerm) p);
    a = ATsetArgument(a,(ATerm) p,1);
    // XXX apply renaming to l
  } 
  else if ( gsIsAllow(a) ){
    ATermAppl p = ATAgetArgument(a,1);
    
    p = gsApplyAlpha(p);
    a = PushAllow(ATLgetArgument(a,0),p); //takes care about l
  } 
  else if ( gsIsComm(a) ){
    ATermAppl p = ATAgetArgument(a,1);
    
    p = gsApplyAlpha(p);
    a = PushComm(ATLgetArgument(a,0),p); //takes care about l
  } 
  else if ( gsIsSum(a) || gsIsAtTime(a) || gsIsChoice(a) || gsIsSeq(a) 
	    || gsIsCond(a) || gsIsSync(a) || gsIsMerge(a) || gsIsLMerge(a) || gsIsBInit(a)){
    // all distributing rules together
    short ia1=0,ia2=1,args=2;
    if(gsIsCond(a) || gsIsSum(a)) { ia1=1; ia2=2; }
    if(gsIsSum(a) || gsIsAtTime(a) || gsIsBInit(a)) args=1; //second argument of BInit does not matter 
    
    ATermAppl p,q;
    p = gsApplyAlpha(ATAgetArgument(a,ia1));
    if(args==2) q = gsApplyAlpha(ATAgetArgument(a,ia2));

    ATermList l,l1,l2;
    l=l1=ATLtableGet(alphas,(ATerm) p);
    if(args==2) l2=ATLtableGet(alphas,(ATerm) q);
    
    if(args==2) l = merge_list(l,l2);
    if(gsIsSync(a) || gsIsMerge(a) || gsIsLMerge(a)) 
      l = merge_list(l,sync_list(l1,l2));
    
    a=ATsetArgument(a,(ATerm)p,ia1);
    if(args==2) a=ATsetArgument(a,(ATerm)q,ia2);
    
    ATtablePut(alphas,(ATerm) a,(ATerm) l);
  }

  assert(ATLtableGet(alphas,(ATerm) a));
  
  return a;
}

ATermAppl gsAlpha(ATermAppl Spec){
  //create the tables
  procs = ATtableCreate(10000,80);
  alphas = ATtableCreate(10000,80);

  //fill in tables the tables
  for(ATermList pr=ATLgetArgument(ATAgetArgument(Spec,5),0); !ATisEmpty(pr); pr=ATgetNext(pr)){
    ATermAppl p=ATAgetFirst(pr);
    ATermAppl pn=ATAgetArgument(p,1);
    ATtablePut(procs,(ATerm)pn,(ATerm)ATAgetArgument(p,3));
    ATtablePut(alphas,(ATerm)pn,(ATerm)ATmakeList0());
  }
  ATtablePut(alphas,(ATerm) gsMakeDelta(),(ATerm) ATmakeList0());
  ATtablePut(alphas,(ATerm) gsMakeTau(),(ATerm) ATmakeList0());

  bool stable=false;
  //possibly endless loop (X=a.X||X ;)
  while(!stable){
    //apply Alpha to each and compare with the old values.
    stable=true;
    for(ATermList pr=ATLgetArgument(ATAgetArgument(Spec,5),0); !ATisEmpty(pr); pr=ATgetNext(pr)){
      ATermAppl pn=ATAgetArgument(ATAgetFirst(pr),1);
      ATermList old_l=ATLtableGet(alphas,(ATerm)pn);
      ATermAppl new_p=gsApplyAlpha(ATAtableGet(procs,(ATerm)pn));
      ATtablePut(alphas,(ATerm)pn,(ATerm)ATLtableGet(alphas,(ATerm)new_p));
      ATermList l=ATLtableGet(alphas,(ATerm)pn);
      if(ATgetLength(old_l)!=ATgetLength(l) || !ATisEqual(list_minus(old_l,l),ATmakeList0()))
	stable=false;
    }
  }
  
  // apply alpha to init
  ATermAppl init=ATAgetArgument(ATAgetArgument(Spec,6),1);;
  init = gsApplyAlpha(init);

  ATermList new_pr=ATmakeList0();
  for(ATermList pr=ATLgetArgument(ATAgetArgument(Spec,5),0); !ATisEmpty(pr); pr=ATgetNext(pr)){
    ATermAppl p=ATAgetFirst(pr);
    ATermAppl pn=ATAgetArgument(p,1);
    new_pr=ATinsert(new_pr,(ATerm)ATsetArgument(p,ATtableGet(procs,(ATerm)pn),3));
    gsDebugMsg("proc: %T; l:%T\n\n", pn, ATtableGet(alphas,(ATerm)pn));
  }
  gsDebugMsg("init: l:%T\n\n", ATtableGet(alphas,(ATerm)init));

  ATtableDestroy(alphas);
  ATtableDestroy(procs);
  
  Spec = ATsetArgument(Spec,(ATerm) gsMakeProcEqnSpec(new_pr),5);
  Spec = ATsetArgument(Spec,(ATerm) gsMakeInit(ATLgetArgument(ATAgetArgument(Spec,6),0),init),6);
  
  return Spec;
}

#ifdef __cplusplus
}
#endif
