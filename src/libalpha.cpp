#define NAME "libalpha"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <aterm2.h>
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "libalpha.h"

static ATermAppl gsApplyAlpha(ATermAppl a);
static ATermList gsaGetAlpha(ATermAppl a, unsigned length=0, ATermList ignore=ATmakeList0());

static inline ATermAppl INIT_KEY(void){return gsMakeProcVarId(gsString2ATermAppl("init"),ATmakeList0());}

static ATermTable alphas;
static ATermTable subs_alpha;
static ATermTable subs_alpha_rev;
static ATermTable form_pars;
static ATermTable procs;
static bool all_stable;

static AFun afunPair;
static ATermTable syncs;
static ATermTable untypes;
static ATermIndexedSet tmpIndexedSet;

static AFun  props_afun;
static ATermAppl pCRL_aterm, npCRL_aterm, mCRL_aterm, rec_aterm, nrec_aterm;
static ATermTable props;
static ATermTable deps;

  //from pnml2mcrl2
  //==================================================
  // ATappendAFun functions as ATmakeAFun
  //==================================================
  static AFun ATappendAFun(AFun id, const char *str) {
    // input: an AFun
    // output: an AFun appended with str

    char *name=ATgetName(id);
    char *buf = (char *) malloc(strlen(str)+strlen(name)+1);
    assert(buf);

    strcpy(buf,name);
    strcat(buf,str);

    AFun Res=ATmakeAFun(buf, ATgetArity(id), ATisQuoted(id));
    free(buf);

    return Res;
  }

  static AFun ATmakeAFunInt(long name, int arity, ATbool quoted) {
    // input: an integer value (name), it's arity and whether it is quoted or not
    // output: an AFun, as in ATmakeAFun, but now with a name from an integer value

    // on 128 bit architecture long cannot ocupy more than 256/3+2=87 8-ary digits, even less 10-ary
    char buf[90];
    sprintf(buf, "%ld", name);
    return ATmakeAFun(buf, arity, quoted);
  }

  //==================================================
  // ATmakeAFunInt0 functions as ATmakeAFunInt(name,0,ATtrue)
  //==================================================
  static inline AFun ATmakeAFunInt0(int name){
    return ATmakeAFunInt(name, 0, ATtrue);
  }

static inline ATermAppl Pair(ATerm ma1, ATerm ma2){
  return ATmakeAppl2(afunPair,ma1,ma2);
}

static inline ATermAppl Pair_allow(ATerm ma1, ATerm ma2){
  return ATmakeAppl2(ATappendAFun(afunPair,"allow"),ma1,ma2);
}

static inline void sPut(ATermList ma1, ATermList ma2, ATermList Result){
  ATtablePut(syncs,(ATerm)Pair((ATerm)ma1,(ATerm)ma2),(ATerm)Result);
}

static inline ATermList sGet(ATermList ma1, ATermList ma2){
  return (ATermList) ATtableGet(syncs,(ATerm)Pair((ATerm)ma1,(ATerm)ma2));
}

static inline void utPut(ATermList ma, ATermList Result){
  ATtablePut(untypes,(ATerm)ma,(ATerm)Result);
}

static inline ATermList utGet(ATermList ma){
  return (ATermList) ATtableGet(untypes,(ATerm)ma);
}

static inline ATermList gsaATinsertUnique(ATermList l, ATerm m){
  if(ATindexOf(l,m,0)<0)
    return ATinsert(l,m);
  else 
    return l;
}

static inline ATermList gsaATsortList(ATermList l){
  return ATsort(l,ATcompare);
}

static inline ATermList gsaATintersectList(ATermList l, ATermList m){
  ATermList r=ATmakeList0();
  for(;!ATisEmpty(l);l=ATgetNext(l))
    if(ATindexOf(m,ATgetFirst(l),0)>=0)
      r=ATinsert(r,ATgetFirst(l));
  return ATreverse(r);
}

static inline ATbool gsaATisDisjoint(ATermList l, ATermList m){
  return ATisEmpty(gsaATintersectList(l,m));
}

static inline void gsaATindexedSetPutList(ATermIndexedSet m, ATermList l){
  //add l into m
  ATbool b;
  for (; !ATisEmpty(l); l=ATgetNext(l)){
    ATindexedSetPut(m,ATgetFirst(l),&b);
  }
}

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
  
  //ATermList r=ATmakeList0();
  //for(;!ATisEmpty(MAct);MAct=ATgetNext(MAct))
  // r=ATinsert(r,(ATerm)untypeA(ATAgetFirst(MAct)));
  //return ATreverse(r);
  
  if(ATisEmpty(MAct)) return ATmakeList0(); 
  
  ATermList r=utGet(MAct);
  if(r) return r;

  r=ATinsert(untypeMA(ATgetNext(MAct)),(ATerm)untypeA(ATAgetFirst(MAct)));
  utPut(MAct,r);
  return r;
}

static inline ATermList typeMA(ATermList MAct){
  //returns "type signature" of MAct
  ATermList r=ATmakeList0();
  for(;!ATisEmpty(MAct);MAct=ATgetNext(MAct))
    r=ATinsert(r,(ATerm)typeA(ATAgetFirst(MAct)));
  return ATreverse(r);
}

static inline ATermList add_typeMA(ATermList ma, ATermList s){
  //adds type s to all actions in ma
  ATermList r=ATmakeList0();
  for(;!ATisEmpty(ma);ma=ATgetNext(ma))
    r=ATinsert(r,(ATerm)gsMakeActId(ATAgetFirst(ma),s));
  return gsaATsortList(r);
}

static inline ATermList untypeMAL(ATermList LMAct){
  //returns List of "untyped multiaction name" of List(MAct)
  assert(ATisEmpty(ATindexedSetElements(tmpIndexedSet))); 
  ATbool b; 
  for(;!ATisEmpty(LMAct);LMAct=ATgetNext(LMAct)){ 
    ATindexedSetPut(tmpIndexedSet,(ATerm)untypeMA(ATLgetFirst(LMAct)),&b); 
  } 
  LMAct = ATindexedSetElements(tmpIndexedSet); 
  ATindexedSetReset(tmpIndexedSet); 
  return LMAct; 
  
  //  ATermList r=ATmakeList0();
  //for(;!ATisEmpty(LMAct);LMAct=ATgetNext(LMAct)){ 
  // r=ATinsert(r,(ATerm)untypeMA(ATLgetFirst(LMAct))); 
  //}
  //return ATreverse(r);
}

static inline ATermList typeMAL(ATermList LMAct){
  //returns List of "type signature" of List(MAct)
  ATermList r=ATmakeList0();
  for(;!ATisEmpty(LMAct);LMAct=ATgetNext(LMAct))
    r=ATinsert(r,(ATerm)typeMA(ATLgetFirst(LMAct)));
  return ATreverse(r);
}

static ATermList sync_mact(ATermList a, ATermList b){
/*   ATermList c = ATmakeList0(); */
/*   while ( !(ATisEmpty(a) || ATisEmpty(b)) ){ */
/*     ATermAppl aa=ATAgetFirst(a); */
/*     ATermAppl bb=ATAgetFirst(b); */
/*     if ( ATcompare((ATerm)aa,(ATerm)bb) <=0 ) { */
/*       c = ATinsert(c,(ATerm)aa); */
/*       a = ATgetNext(a); */
/*     }  */
/*     else { */
/*       c = ATinsert(c,(ATerm)bb); */
/*       b = ATgetNext(b); */
/*     } */
/*   } */
/*   c=ATreverse(c); */
/*   if ( !ATisEmpty(a) ){ */
/*     c = ATconcat(c,a); */
/*   } else if ( !ATisEmpty(b) ) */
/*     { */
/*       c = ATconcat(c,b); */
/*     } */
/*   return c; */



/*   //return gsaATsortList(ATconcat(a,b)); */
/*   int n = ATgetLength(a); */
/*   int m = ATgetLength(b); */
/*   ATermAppl *c = malloc((n+m+1)*sizeof(ATermAppl)); */

/*   int i=-1; //current pos */
/*   while ( !(ATisEmpty(a) || ATisEmpty(b)) ){ */
/*     ATermAppl aa=ATAgetFirst(a); */
/*     ATermAppl bb=ATAgetFirst(b); */
/*     i++; */
/*     if ( ATcompare((ATerm)aa,(ATerm)bb) <=0 ) { */
/*       c[i] = aa; */
/*       a = ATgetNext(a); */
/*     }  */
/*     else { */
/*       c[i] = bb; */
/*       b = ATgetNext(b); */
/*     } */
/*   } */

/*   if(ATisEmpty(a)) a=b; */
/*   while ( !ATisEmpty(a) ){ */
/*     i++; */
/*     c[i] = ATAgetFirst(a); */
/*     a = ATgetNext(a); */
/*  } */
/*   ATermList r=ATmakeList0(); */
/*   for (;i>=0;i--) */
/*     r=ATinsert(r,(ATerm)c[i]); */
/*   free(c); */
/*   return r; */

  assert(a && b);

  if(ATisEmpty(a)) return b;
  if(ATisEmpty(b)) return a;
  
  ATermList c = sGet(a,b);
  if(c) return c;

  ATermAppl aa=ATAgetFirst(a); 
  ATermAppl bb=ATAgetFirst(b); 

  if ( ATcompare((ATerm)aa,(ATerm)bb) > 0 ) {
    ATermList t=a; a=b; b=t; //swap a and b
    aa=bb; //no need for bb
  } 
  
  c=ATgetNext(a);
  if(ATisEmpty(c))
    c=ATinsert(b,(ATerm)aa);
  else {
    c=ATinsert(sync_mact(c,b),(ATerm)aa);
  }

  sPut(a,b,c);
  sPut(b,a,c);
  return c; 
}

static ATermList merge_list(ATermList l, ATermList m){
  ATermList r=ATmakeList0();
  for (; !ATisEmpty(m); m=ATgetNext(m)){
    ATerm el=ATgetFirst(m);
    if(ATindexOf(l,el,0) < 0 ){
      r = ATinsert(r,el);
    }
  }
  return ATconcat(l,ATreverse(r));
}

static ATermList list_minus(ATermList l, ATermList m){
  ATermList n = ATmakeList0();
  for (; !ATisEmpty(l); l=ATgetNext(l)){
    if ( ATindexOf(m,ATgetFirst(l),0) < 0 ){
      n = ATinsert(n,ATgetFirst(l));
    }
  }
  return ATreverse(n);
}

static ATermList filter_block_list(ATermList l, ATermList H){
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
  ATermList m = ATmakeList0();
  for (; !ATisEmpty(l); l=ATgetNext(l)){
    ATermList new_ma=ATmakeList0();
    for(ATermList ma=ATLgetFirst(l);!ATisEmpty(ma); ma=ATgetNext(ma))
      if(ATindexOf(I,(ATerm)untypeA(ATAgetFirst(ma)),0)<0) 
	new_ma=ATinsert(new_ma,ATgetFirst(ma));
    if(ATgetLength(new_ma) && ATindexOf(m,(ATerm)new_ma,0)<0) m=ATinsert(m,(ATerm)ATreverse(new_ma));
  }
  return ATreverse(m);
}

static ATermList filter_allow_list(ATermList l, ATermList V){
  //filters l to contain only multiactions matching the untiped multiactions from V
  ATermList m=ATmakeList0();
  for (; !ATisEmpty(l); l=ATgetNext(l)){
    if(ATindexOf(V,(ATerm)gsMakeMultActName(untypeMA(ATLgetFirst(l))),0) >=0)
      m = ATinsert(m,(ATerm)ATLgetFirst(l));
  }
  return ATreverse(m);
}

static unsigned get_max_allowed_length(ATermList V){
  //returns the length of the longest allowed multiaction (min 1).
  unsigned m = 1;
  for (; !ATisEmpty(V); V=ATgetNext(V)){
    unsigned c=ATgetLength(ATLgetArgument(ATAgetFirst(V),0));
    if(c>m) m=c;
  }
  return m;
}

static ATermList optimize_allow_list(ATermList V, ATermList ul){
  //returns the subset of V that is in ul
  ATermList m = ATmakeList0();
  for (; !ATisEmpty(V); V=ATgetNext(V)){
    ATermAppl ma=ATAgetFirst(V);
    if(ATindexOf(ul,(ATerm)ATLgetArgument(ma,0),0) >=0)
      m = ATinsert(m,(ATerm)ma);
  }
  return ATreverse(m);
}

static ATermList sort_multiactions_allow(ATermList V){
  //sort the user defined multiactions in V
  ATermList m = ATmakeList0();
  for (; !ATisEmpty(V); V=ATgetNext(V))
    m = ATinsert(m,(ATerm)gsMakeMultActName(gsaATsortList(ATLgetArgument(ATAgetFirst(V),0))));

  return ATreverse(m);
}

static ATermList sort_multiactions_comm(ATermList C){
  //sort the user defined multiactions in C
  ATermList m = ATmakeList0();
  for (; !ATisEmpty(C); C=ATgetNext(C)){
    ATermAppl c=ATAgetFirst(C);
    ATermAppl lhs=ATAgetArgument(c,0);
    lhs=ATsetArgument(lhs,(ATerm)gsaATsortList(ATLgetArgument(lhs,0)),0);
    m = ATinsert(m,(ATerm)ATsetArgument(c,(ATerm)lhs,0));
  }

  return ATreverse(m);
}

static ATermList split_allow(ATermList V, ATermList ulp, ATermList ulq){
  //slits V according to the 2 alphabets (see paper) and returns the first part.
  //gsWarningMsg("split_allow: V: %T; ulp: %T; ulq: %T\n",V,ulp,ulq);
  ATermList m = ATmakeList0();

  ATermIndexedSet VV=ATindexedSetCreate(10000,80);
  gsaATindexedSetPutList(VV,V);

  for (; !ATisEmpty(ulp); ulp=ATgetNext(ulp)){
    ATermList up=ATLgetFirst(ulp);
    ATermAppl ma=gsMakeMultActName(ATLgetFirst(ulp));
    ATermList tulq=ulq;
    for (; !ATisEmpty(ulq); ulq=ATgetNext(ulq)){
      if(ATindexedSetGetIndex(VV,(ATerm)gsMakeMultActName(sync_mact(up,ATLgetFirst(ulq))))>=0){
	m = ATinsert(m,(ATerm)ma);
	break;
      }
    }
    ulq=tulq;
  }
  ATindexedSetDestroy(VV);

  //gsWarningMsg("split_allow: m: %T\n\n",ATreverse(m));
  return ATreverse(m);
}

static ATermList sync_list(ATermList l, ATermList m, unsigned length=0){
  ATermList n = ATmakeList0();
  for (; !ATisEmpty(l); l=ATgetNext(l)){
    ATermList ll=ATLgetFirst(l);
    for (ATermList o=m; !ATisEmpty(o); o=ATgetNext(o)){
      ATermList oo=ATLgetFirst(o);
      if(!length || unsigned(ATgetLength(ll))+unsigned(ATgetLength(oo))<=length){
	ATermList ma=sync_mact(ll,oo);
	if(ATindexOf(n,(ATerm)ma,0)<0)
	  n = ATinsert(n,(ATerm)ma);
      }
    }
  }
  return ATreverse(n);
}

static void sync_list_ht(ATermIndexedSet m, ATermList l1, ATermList l2, unsigned length=0){
  //put the synchronization of l1 and l2 into m (if length, then not longet than length)
  ATbool b;
  for (; !ATisEmpty(l1); l1=ATgetNext(l1)){
    ATermList ll=ATLgetFirst(l1);
    for (ATermList o=l2; !ATisEmpty(o); o=ATgetNext(o)){
      ATermList oo=ATLgetFirst(o);
      if(!length || unsigned(ATgetLength(ll))+unsigned(ATgetLength(oo))<=length)
	ATindexedSetPut(m,(ATerm)sync_mact(ll,oo),&b);
    }
  }
}

static inline void sync_list_into_ht(ATermIndexedSet m, ATermList l){
  //put the synchronization of m and l into m
  ATermList l1=ATindexedSetElements(m);
  ATindexedSetReset(m);
  sync_list_ht(m,l1,l);
}

static bool disjoint_multiaction(ATermList MAct, ATermList MActL){
  // true if no part of multiaction MAct is in a submultiaction of a multiaction from MActL (all untyped)
  for (; !ATisEmpty(MActL); MActL=ATgetNext(MActL)){
    if ( !gsaATisDisjoint(MAct,ATLgetFirst(MActL)) ){
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

static ATermList comm_lhs(ATermList C){
  ATermList l = ATmakeList0();
  for (; !ATisEmpty(C); C=ATgetNext(C)){
    l = ATconcat(l,ATLgetArgument(ATAgetArgument(ATAgetFirst(C),0),0));
  }
  return ATreverse(l);
}

static ATermList comm_rhs(ATermList C){
  ATermList l = ATmakeList0();
  for (; !ATisEmpty(C); C=ATgetNext(C)){
    ATermAppl a = ATAgetArgument(ATAgetFirst(C),1);
    if ( !gsIsNil(a) ) {
      l = ATinsert(l,(ATerm) a);
    }
  }
  return ATreverse(l);
}

static bool can_split_comm(ATermList C){
  ATermList lhs = comm_lhs(C);
  ATermList rhs = comm_rhs(C);
  bool b = true;
  for (; !ATisEmpty(lhs); lhs=ATgetNext(lhs)){
    if ( ATindexOf(rhs,ATgetFirst(lhs),0) >= 0 ) {
      b = false;
      break;
    }
  }
  return b;
}

static ATermList apply_rename(ATermList l, ATermList R){
  //applies R to a multiaction l
  if ( ATisEmpty(l) )
    return l;
  
  ATermList m=ATmakeList0();
  for (; !ATisEmpty(l); m=ATgetNext(l)){
    ATermList tR = R;
    ATermAppl a=ATAgetFirst(l);
    bool b=false;
    for (; !ATisEmpty(R); R=ATgetNext(R)){
      ATermAppl r=ATAgetFirst(R);
      if(ATisEqual(ATAgetArgument(a,0),ATAgetArgument(r,0))){
	m = ATinsert(m,(ATerm)ATsetArgument(a,(ATerm)ATAgetArgument(r,1),0));
	b=true;
	break;
      }
    }
    R = tR;
    if(!b) 
      m = ATinsert(m,(ATerm)a);
  }
  return gsaATsortList(m);
}

static ATermList apply_comms(ATermList l, ATermList C){
  //gives all possible results of application of C to a multiaction l
  //explanation: applying {a:Nat|b:Nat-c:Nat} to a|b can either give c, or a|b,
  //depending on the parameters of a and b. (in case a,b have no parameters,
  //the result is definitely c)
  //so the result is an alphabeth, not a single multiaction

  //ATermIndexedSet m=ATindexedSetCreate(10000,80);
  ATermList m = ATmakeList1((ATerm)ATmakeList0());
  ATermList r=l;
  while(ATgetLength(r) > 1 ){
    ATermAppl a = ATAgetFirst(r);
    r = ATgetNext(r);
    //gsWarningMsg("r: %T\n",r);
    bool applied=false;
    for (ATermList tC=C; !ATisEmpty(tC); tC=ATgetNext(tC)){
      ATermList c = ATLgetArgument(ATAgetArgument(ATAgetFirst(tC),0),0);
      if ( ATindexOf(c,ATgetArgument(a,0),0) >= 0 ){
	ATermList s = ATLgetArgument(a,1);
	ATermList tr = r;
	bool b=true;
	ATermList tc = c;
	c = ATremoveElement(c,(ATerm)ATAgetArgument(a,0));
	for (; !ATisEmpty(c); c=ATgetNext(c)){
	  ATermAppl act = gsMakeActId(ATAgetFirst(c),s);
	  if ( ATindexOf(tr,(ATerm) act,0) >= 0 ){
	    tr = ATremoveElement(tr,(ATerm) act);
	  } 
	  else {
	    b = false;
	    break;
	  }
	}
	c=tc;
	if ( b ){ //can apply c -- no other c can be applied to a multiaction containing "a" (rules for C)
	  applied=true;
	  r = tr;
	  ATermAppl rhs_c=ATAgetArgument(ATAgetFirst(tC),1);
	  ATermList tm=ATmakeList0();
	  if(!ATisEqual(s,ATmakeList0())){
	    tm=ATmakeList1((ATerm)add_typeMA(c,s));
	  }
	  if(!gsIsNil(rhs_c))
	    tm=merge_list(tm,ATmakeList1((ATerm)ATmakeList1((ATerm)gsMakeActId(rhs_c,s))));
	  else
	    tm=merge_list(tm,ATmakeList1((ATerm)ATmakeList0()));
	  //sync_list_into_ht(m,tm);
	  m=sync_list(m,tm);
	  break;
	}
      }
    }
    if(!applied){
      //sync_list_into_ht(m,ATmakeList1((ATerm)ATmakeList1((ATerm)a)));
      m=sync_list(m,ATmakeList1((ATerm)ATmakeList1((ATerm)a)));
    }
  }
  
  if(!ATisEmpty(r)){
    //sync_list_into_ht(m,ATmakeList1((ATerm)r));
    m=sync_list(m,ATmakeList1((ATerm)r));
  }

  //l = ATindexedSetElements(m);
  //ATindexedSetDestroy(m);
  //return l;
  return m;
 
}

static ATermList extend_allow_comm_with_alpha(ATermList V, ATermList C, ATermList l){
  //Extend V to contain communications of L with C
  //gsWarningMsg("extend_comm: V: %T; C: %P; l: %d\n",V,C,ATgetLength(l));
  ATermList r=ATreverse(V);

  {
    //make V a list of multiactions actions
    ATermList nV=ATmakeList0();
    for (; !ATisEmpty(V); V=ATgetNext(V) ){
      nV=ATinsert(nV,(ATerm)ATLgetArgument(ATAgetFirst(V),0));
    }
    V=ATreverse(nV);
  }
  V=ATinsert(V,(ATerm)ATmakeList0()); //to include possible communications to tau
  
  for (; !ATisEmpty(l); l=ATgetNext(l) ){
    ATermAppl ma=gsMakeMultActName(untypeMA(ATLgetFirst(l)));
    if(ATindexOf(r,(ATerm)ma,0)<0){
      ATermList mas=untypeMAL(apply_comms(ATLgetFirst(l),C));
      if(!gsaATisDisjoint(V,mas)) 
	r=ATinsert(r,(ATerm)ma);
    }
  }
  //gsWarningMsg("extend_comm done: r: %T;\n\n",ATreverse(r));
  return ATreverse(r);
}

static ATermList extend_allow_comm(ATermList V, ATermList C){
  //Extend V to allow communications the results of communications C 
  
  //gsWarningMsg("extend_allow_comm: V: %T; C: %P\n",V,C);
  
  //create a table with the reverse mappings of the actions in C
  ATermTable rev=ATtableCreate(10000,80);
  
  for (; !ATisEmpty(C); C=ATgetNext(C) ){
    ATermAppl c=ATAgetFirst(C);
    ATermAppl target=ATAgetArgument(c,1);
    if(gsIsNil(target)) continue;
    ATermList cur=ATLtableGet(rev,(ATerm)target);
    ATtablePut(rev,(ATerm)target,(cur)?(ATerm)ATinsert(cur,(ATerm)ATLgetArgument(ATAgetArgument(c,0),0)):
	       (ATerm)ATmakeList1((ATerm)ATLgetArgument(ATAgetArgument(c,0),0)));
  }

  // for all elements of V get a set of multiactions using the reverse mapping.
  ATermIndexedSet m = ATindexedSetCreate(10000,80);
  
  ATbool b;
  for (ATermList tV=V; !ATisEmpty(tV); tV=ATgetNext(tV) ){
    ATindexedSetPut(m,(ATerm)ATLgetArgument(ATAgetFirst(tV),0),&b);
  }

  for (ATermList tV=V; !ATisEmpty(tV); tV=ATgetNext(tV) ){
    ATermList v=ATLgetArgument(ATAgetFirst(tV),0);
    ATermList res=ATmakeList1((ATerm)ATmakeList0());
    for (; !ATisEmpty(v); v=ATgetNext(v) ){
      ATermAppl a=ATAgetFirst(v);
      ATermList r=ATLtableGet(rev,(ATerm)a);
      if(r) r=merge_list(r,ATmakeList1((ATerm)ATmakeList1((ATerm)a)));
      else r=ATmakeList1((ATerm)ATmakeList1((ATerm)a));
      res=sync_list(res,r);  
    }
    gsaATindexedSetPutList(m,res);
  }
   
  ATtableDestroy(rev);
  ATermList r=ATmakeList0();
  ATermList l=ATindexedSetElements(m);
  ATindexedSetDestroy(m);
  for ( ;!ATisEmpty(l); l=ATgetNext(l) ){
    r=ATinsert(r,(ATerm)gsMakeMultActName(ATLgetFirst(l)));
  }
  
  //gsWarningMsg("extend_allow_comm done: r: %T;\n\n",ATreverse(r));
  return ATreverse(r);
}

static ATermList get_comm_ignore_list(ATermList C){
  //returns all elements of C that are renamed to tau

  ATermList r=ATmakeList0();
  for (; !ATisEmpty(C); C=ATgetNext(C) ){
    ATermAppl c=ATAgetFirst(C);
    ATermAppl target=ATAgetArgument(c,1);
    if(gsIsNil(target)) 
      r=ATinsert(r,(ATerm)ATLgetArgument(ATAgetArgument(c,0),0));
  }
  
  return ATreverse(r);
}

static ATermList filter_comm_list(ATermList l, ATermList C){
  //apply C to all elements of l
  //gsWarningMsg("filter_comm_list: l: %d; C: %P\n",ATgetLength(l),C);

  ATermIndexedSet m=ATindexedSetCreate(10000,80);
  gsaATindexedSetPutList(m,l);
  //ATermList m=l;

  for (; !ATisEmpty(l); l=ATgetNext(l)){
    ATermList mas=apply_comms(ATLgetFirst(l),C);
    mas=ATremoveElement(mas,(ATerm)ATmakeList0());
    //m = merge_list(m,mas);
    gsaATindexedSetPutList(m,mas);
  }
  l = ATindexedSetElements(m);
  ATindexedSetDestroy(m);
  //gsWarningMsg("filter_comm_list: l: %d\n\n",ATgetLength(l));
  return l;
  //return m;
}

static ATermList filter_rename_list(ATermList l, ATermList R){
  //apply R to all elements of l
  ATermList m = ATmakeList0();
  for (; !ATisEmpty(l); l=ATgetNext(l)){
    m = ATinsert(m,(ATerm)apply_rename(ATLgetFirst(l),R));
  }
  return ATreverse(m);
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
    if(!l) l=gsaGetAlpha(a);
    
    l = filter_block_list(l,H);
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
    
    l = ATLtableGet(alphas,(ATerm) p);
    if(!l) l=gsaGetAlpha(a);
    l = filter_block_list(l,H);
    
    p = PushBlock(H,p);
    
    a = gsMakeHide(ATLgetArgument(a,0),p);
    
    ATtablePut(alphas,(ATerm) a,(ATerm) l);
    
    return a;
  } 
  else if ( gsIsRename(a) ){
    // XXX
    ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
    if(!l) l=gsaGetAlpha(a);
    a = gsMakeBlock(H,a);
    ATtablePut(alphas,(ATerm) a,(ATerm) l);
    return a;
  } 
  else if ( gsIsComm(a) ){
    //XXX
    ATermList l = ATLtableGet(alphas,(ATerm) a);
    if(!l) l=gsaGetAlpha(a);
    ATermList C = sort_multiactions_comm(ATLgetArgument(a,0));
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
	ATtablePut(alphas,(ATerm) a,(ATerm) filter_block_list(l,Ha));
      }
    
    return a;
  } 
  else if ( gsIsAllow(a) ){
    //XXX
    ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
    if(!l) l=gsaGetAlpha(a);
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
    if(!l) l=gsaGetAlpha(a);
    l = filter_hide_list(l,I);
    // XXX also adjust I?
    
    a = gsApplyAlpha(a);

    a = gsMakeHide(I,a);
    
    ATtablePut(alphas,(ATerm) a,(ATerm) l);
    
    return a;
  } 
  else if ( gsIsBlock(a) ){
    //XXX
    ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
    if(!l) l=gsaGetAlpha(a);
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
    if(!l) l=gsaGetAlpha(a);
    a = gsMakeHide(I,a);
    ATtablePut(alphas,(ATerm) a,(ATerm) filter_hide_list(l,I));
    return a;
  } 
  else if ( gsIsComm(a) ){
    //XXX
    ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
    if(!l) l=gsaGetAlpha(a);
    a = gsMakeHide(I,a);
    ATtablePut(alphas,(ATerm) a,(ATerm) filter_hide_list(l,I));
    return a;
  } 
  else if ( gsIsAllow(a) ){
    a = gsApplyAlpha(a);
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
  //gsWarningMsg("push allow\n");//: V: %T; a:%P\n",V,a);
  V=sort_multiactions_allow(V);
  if ( gsIsDelta(a) || gsIsTau(a) ){
    return a;
  } 
  else if ( gsIsAction(a) ){
    if(ATindexOf(V,(ATerm)gsMakeMultActName(ATmakeList1((ATerm)ATAgetArgument(ATAgetArgument(a,0),0))),0)<0)
      return gsMakeDelta();
    return gsApplyAlpha(a);
  }
  else if ( gsIsProcess(a) ){
    ATermAppl pn=ATAgetArgument(a,0);
    ATermList l = ATLtableGet(alphas,(ATerm)pn);
    if(!l){
      unsigned max_len=get_max_allowed_length(V);
      l = ATLtableGet(alphas,(ATerm)Pair((ATerm)ATmakeAppl0(ATmakeAFunInt0(max_len)),(ATerm)pn));
      if(!l)
	l = gsaGetAlpha(a,max_len);
    }
    else
      ATtablePut(alphas,(ATerm) a,(ATerm) l);
     
    {
      ATermList ll=l;
      l = filter_allow_list(ll,V);
      if(ATisEqual(l,ll)) 
	return a; //everything from alpha(a) is allowed by V -- no need in allow
    }
    
    ATermList ul=untypeMAL(l);
    V = optimize_allow_list(V,ul);
    
    // here we create (in case pn is not recursive) a new process equation to replace gsMakeAllow(V,a);
    // we call it pn_allow_i, where is is such that pn_allow_i is a fresh process name.
    // the parameters are the same as in pn.

    if(ATisEqual(ATAgetArgument(ATAtableGet(props,(ATerm)pn),1),nrec_aterm)){
      ATermAppl new_pn=ATAtableGet(subs_alpha,(ATerm)Pair_allow((ATerm)V,(ATerm)pn));
      if(!new_pn){
	//create a new 
	//process name with type pn, add _i 
	short i=1;
	do{
	  new_pn=ATsetArgument(pn,(ATerm)ATmakeAppl0(ATappendAFun(ATappendAFun(ATgetAFun(ATAgetArgument(pn,0)),"_allow_"),ATgetName(ATmakeAFunInt0(i)))),0);
	  i++;
	} while(ATtableGet(procs,(ATerm)new_pn));
	
	gsVerboseMsg("Created process %P\n", new_pn);
	ATermAppl p=ATAtableGet(procs,(ATerm)pn);
	assert(p);
	p=PushAllow(V,p);
	
	ATtablePut(procs,(ATerm)new_pn,(ATerm)p);
	l=ATLtableGet(alphas,(ATerm)p);
	ATtablePut(alphas,(ATerm)new_pn,(ATerm)l);
	
	// we save both direct and reverse mappings
	ATtablePut(subs_alpha,(ATerm)Pair_allow((ATerm)V,(ATerm)pn),(ATerm)new_pn);
	ATtablePut(subs_alpha_rev,(ATerm)new_pn,(ATerm)pn);
      }
      a = ATsetArgument(a,(ATerm)new_pn,0);
      ATtablePut(alphas,(ATerm) a,(ATerm) l);
    }
    else{
      a = gsMakeAllow(V,a);
      ATtablePut(alphas,(ATerm) a,(ATerm) filter_allow_list(l,V));
    }
    
    return a;
  } 
  else if ( gsIsBlock(a) ){
    ATermList H=ATLgetArgument(a,0);
    ATermAppl p=ATAgetArgument(a,1);

    ATermList l = ATLtableGet(alphas,(ATerm) p);
    if(!l) l=gsaGetAlpha(a);

    p = PushAllow(V,p);

    l = ATLtableGet(alphas,(ATerm) p);
    a = ATsetArgument(a,(ATerm)p,1);
    ATtablePut(alphas,(ATerm) a,(ATerm)filter_block_list(l,H));
    return a;
  } 
  else if ( gsIsHide(a) ){
    ATermList I=ATLgetArgument(a,0);
    ATermAppl p=ATAgetArgument(a,1);

    ATermList l = ATLtableGet(alphas,(ATerm) p);
    if(!l) l = gsaGetAlpha(p);

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
    if(!l) l=gsaGetAlpha(a);
    a = gsMakeAllow(V,a);
    ATtablePut(alphas,(ATerm) a,(ATerm) filter_allow_list(l,V));
    return a;
  } 
  else if ( gsIsAllow(a) ){
    return PushAllow(gsaATintersectList(V,sort_multiactions_allow(ATLgetArgument(a,0))),ATAgetArgument(a,1));
  } 
  else if ( gsIsComm(a) ){
    ATermList C=ATLgetArgument(a,0);
    C=sort_multiactions_comm(C);
    
    ATermAppl p=ATAgetArgument(a,1);
    ATermList V1;
    ATermList l = ATLtableGet(alphas,(ATerm) p);
    if(!l){
      // check if C has renamings to tau:
      ATermList ignore=get_comm_ignore_list(C);
      if(ATisEmpty(ignore)){
	V1=extend_allow_comm(V,C);
      }
      else {
	l = gsaGetAlpha(p); //XXX may be slow
	assert(l);
      }
    }
    if(l)
      V1 = extend_allow_comm_with_alpha(V,C,l);
    
    //gsWarningMsg("V1: %T\n\n",V1);
    p = PushAllow(V1,p);
    l = ATLtableGet(alphas,(ATerm) p);
    assert(l);
    
    l = filter_comm_list(l,C);
    a = ATsetArgument(a,(ATerm)p,1);
    ATtablePut(alphas,(ATerm) a,(ATerm)l);

    {
      ATermList ll=l;
      l = filter_allow_list(ll,V);
      if(ATisEqual(l,ll)) return a; //everything from alpha(a) is allowed by V -- no need in allow
    }
    
    V = optimize_allow_list(V,untypeMAL(l));
    a = gsMakeAllow(V,a);
    ATtablePut(alphas,(ATerm) a,(ATerm)filter_comm_list(l,C));
    return a;
  } 
  else if ( gsIsSync(a) || gsIsMerge(a) || gsIsLMerge(a) ){
    ATermAppl p = ATAgetArgument(a,0);
    ATermAppl q = ATAgetArgument(a,1);
    ATermList lp=ATLtableGet(alphas,(ATerm) p);
    ATermList lq=ATLtableGet(alphas,(ATerm) q);

    unsigned max_len=get_max_allowed_length(V);
    if(!lp) lp=gsaGetAlpha(p,max_len);
    if(!lq) lq=gsaGetAlpha(q,max_len);

    ATermList ulp = untypeMAL(lp);
    ATermList ulq = untypeMAL(lq);
    
    ATermList Vp=merge_list(V,split_allow(V,ulp,ulq));
    ATermList Vq=merge_list(V,split_allow(V,ulq,ulp));

    p=PushAllow(Vp,p);
    q=PushAllow(Vq,q);

    ATermList l,l2;
    l=ATLtableGet(alphas,(ATerm) p);
    l2=ATLtableGet(alphas,(ATerm) q);
    assert(l);
    //gsWarningMsg("a: %T\n\n", q);
    assert(l2);
    l=merge_list(merge_list(l,l2),sync_list(l,l2));
    a=ATsetArgument(ATsetArgument(a,(ATerm)q,1),(ATerm)p,0);
    ATtablePut(alphas,(ATerm) a,(ATerm) l);

    {
      ATermList ll=l;
      l = filter_allow_list(ll,V);
      if(ATisEqual(l,ll)) return a; //everything from alpha(a) is allowed by V -- no need in allow
    }
    
    ATermList ul=untypeMAL(l);
    V = optimize_allow_list(V,ul);
    a = gsMakeAllow(V,a);
    assert(l);
    ATtablePut(alphas,(ATerm) a,(ATerm)l);
    return a;
  }
  else if ( gsIsSum(a) || gsIsAtTime(a) || gsIsChoice(a) || gsIsSeq(a) 
	    || gsIsCond(a) || gsIsSync(a) || gsIsMerge(a) || gsIsLMerge(a) || gsIsBInit(a)){
    // all distributing rules together
    short ia1=0,ia2=1,args=2;
    if(gsIsCond(a) || gsIsSum(a)) { ia1=1; ia2=2; }
    if(gsIsSum(a) || gsIsAtTime(a) || gsIsBInit(a)) args=1; //second argument of BInit does not matter 
    
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

static ATermAppl PushComm(ATermList C, ATermAppl a){
  C=sort_multiactions_comm(C);
  //gsWarningMsg("push comm\n");//: C: %P; a:%P\n",C,a);
  if ( gsIsDelta(a) || gsIsTau(a) || gsIsAction(a) ){
    return a;
  } 
  else if ( gsIsProcess(a) ){
    ATermList l = ATLtableGet(alphas,(ATerm) a);
    if(!l) l=gsaGetAlpha(a);
    
    l = filter_comm_list(l,C);
    // XXX also adjust C?

    a = gsMakeComm(C,a);

    ATtablePut(alphas,(ATerm) a,(ATerm) l);

    return a;
  } 
  else if ( gsIsBlock(a) ){
    //XXX
    ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
    if(!l) l=gsaGetAlpha(a);
    a = gsMakeComm(C,a);
    ATtablePut(alphas,(ATerm) a,(ATerm) filter_comm_list(l,C));
    return a;
  } else if ( gsIsHide(a) ){
    //XXX
    ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
    if(!l) l=gsaGetAlpha(a);
    a = gsMakeComm(C,a);
    ATtablePut(alphas,(ATerm) a,(ATerm) filter_comm_list(l,C));
    return a;
  } else if ( gsIsRename(a) ){
    //XXX
    ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
    if(!l) l=gsaGetAlpha(a);
    a = gsMakeComm(C,a);
    ATtablePut(alphas,(ATerm) a,(ATerm) filter_comm_list(l,C));
    return a;
  } else if ( gsIsComm(a) ){
    //XXX
    ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
    if(!l) l=gsaGetAlpha(a);
    a = gsMakeComm(C,a);
    ATtablePut(alphas,(ATerm) a,(ATerm) filter_comm_list(l,C));
    return a;
  } else if ( gsIsAllow(a) ){
    //XXX
    ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
    if(!l) l=gsaGetAlpha(a);
    a = gsMakeComm(C,a);
    ATtablePut(alphas,(ATerm) a,(ATerm) filter_comm_list(l,C));
    return a;
  } 
  else if ( gsIsSync(a) || gsIsMerge(a) || gsIsLMerge(a) ){
    if ( can_split_comm(C) ){
      ATermAppl p = ATAgetArgument(a,0);
      ATermAppl q = ATAgetArgument(a,1);
      ATermList ulp = untypeMAL(ATLtableGet(alphas,(ATerm) p));
      ATermList ulq = untypeMAL(ATLtableGet(alphas,(ATerm) q));
      ATermList Cp = ATmakeList0();
      ATermList Cq = ATmakeList0();
      ATermList Ca = ATmakeList0();
      ATermList l;
      
      for (; !ATisEmpty(C); C=ATgetNext(C)){
	ATermAppl c=ATAgetFirst(C);
	ATermList lhs=ATLgetArgument(ATAgetArgument(c,0),0);
	bool bp = disjoint_multiaction(lhs,ulp);
	bool bq = disjoint_multiaction(lhs,ulq);
	
	if ( !bp ){
	  if ( !bq ){
	    Ca = ATinsert(Ca,(ATerm)c);
	  } 
	  else {
	    Cp = ATinsert(Cp,(ATerm)c);
	  }
	} 
	else 
	  if ( !bq ){
	    Cq = ATinsert(Cq,(ATerm)c);
	  }
      }
      if ( !(ATisEmpty(Cp) && ATisEmpty(Cq)) ){
	if ( !ATisEmpty(Cp) ){
	  p = PushComm(Cp,p);
	}
	if ( !ATisEmpty(Cq) ){
	  q = PushComm(Cq,q);
	}
	{
	  l=ATLtableGet(alphas,(ATerm) p);
	  ATermList l2=ATLtableGet(alphas,(ATerm) q);
	  //gsWarningMsg("tick l: %d\n", ATgetLength(l));
	  ATermIndexedSet m=ATindexedSetCreate(10000,80);
	  gsaATindexedSetPutList(m,l);
	  gsaATindexedSetPutList(m,l2);
	  sync_list_ht(m,l,l2);
	  l = ATindexedSetElements(m);
	  ATindexedSetDestroy(m);
	  //gsWarningMsg("tick l: %d\n\n", ATgetLength(l));
	}
	a=ATsetArgument(ATsetArgument(a,(ATerm)q,1),(ATerm)p,0);
	ATtablePut(alphas,(ATerm) a,(ATerm) l);
      } 
      else {
	l = ATLtableGet(alphas,(ATerm) a);
      }
      
      if ( !ATisEmpty(Ca) ){
	a = gsMakeComm(Ca,a);
	//gsWarningMsg("tick2 l: %d\n",ATgetLength(l));
	l = filter_comm_list(l,Ca);
	ATtablePut(alphas,(ATerm) a,(ATerm) l);
 	//gsWarningMsg("tick2 l: %d\n\n",ATgetLength(l));
     }
      return a;
    } 
    else {
      ATermList l = (ATermList) ATtableGet(alphas,(ATerm) a);
      a = gsMakeComm(C,a);
      ATtablePut(alphas,(ATerm) a,(ATerm) filter_comm_list(l,C));
      return a;
    }
  } 
  else if ( gsIsSum(a) || gsIsAtTime(a) || gsIsChoice(a) || gsIsSeq(a)
	    || gsIsCond(a) || gsIsSync(a) || gsIsMerge(a) || gsIsLMerge(a) || gsIsBInit(a)){
    // all distributing rules together
    short ia1=0,ia2=1,args=2;
    if(gsIsCond(a) || gsIsSum(a)) { ia1=1; ia2=2; }
    if(gsIsSum(a) || gsIsAtTime(a) || gsIsBInit(a)) args=1; //second argument of BInit does not matter 
    
    ATermAppl p,q;
    p = PushComm(C,ATAgetArgument(a,ia1));
    if(args==2) q = PushComm(C,ATAgetArgument(a,ia2));
    
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
  
static ATermList gsaGetAlpha(ATermAppl a, unsigned length, ATermList ignore){
  // calculate the alphabeth of a up to the length, ignoring the submultiactions from ignore (list of untyped multiactions) IF 
  // they have a common type (if !length, then the length is unlimited and ignore is not used)
  // updates the global hash table alphas (in case length==0 and ignore=ATmakeList0() writes the alphabet of a into alphas).
  // XXX ignore parameter not implemented yet
  ATermList l=NULL; //result

  if ( gsIsDelta (a) || gsIsTau(a) ){
    l = ATmakeList0();
  }
  else if ( gsIsAction(a) || gsIsDelta (a) || gsIsTau(a) ){
    l = ATLtableGet(alphas,(ATerm)ATAgetArgument(a,0));
    if(!l)
      l = ATmakeList1((ATerm)ATmakeList1(ATgetArgument(a,0)));
  } 
  else if ( gsIsProcess(a) ){
    ATermAppl pn=ATAgetArgument(a,0);
    l=ATLtableGet(alphas,(ATerm)pn);
    if(!l){
      //this should be a mCRL process (pCRL processes always have an entry). 
      //we apply the alphabeth reductions to its body and then we know the alphabet
      //gsWarningMsg("Exploring new mCRL process %T\n\n",pn);
      l=gsaGetAlpha(ATAtableGet(procs,(ATerm)pn),length,ignore);
      if(!length)
	ATtablePut(alphas,(ATerm)pn,(ATerm) l);
      else
	ATtablePut(alphas,(ATerm)Pair((ATerm)ATmakeAppl0(ATmakeAFunInt0(length)),(ATerm)pn),(ATerm)l);
    }
    assert(l);
  }
  else if ( gsIsBlock(a) ){
    ATermAppl p = ATAgetArgument(a,1);
    l=gsaGetAlpha(p,length,ignore);
    if(!length)
      ATtablePut(alphas,(ATerm) p,(ATerm) l); 
    
    l=filter_block_list(l,ATLgetArgument(a,0)); 
    assert(l);
  }
  else if ( gsIsHide(a) ){
    ATermAppl p = ATAgetArgument(a,1);
    if(!length){
      l=gsaGetAlpha(p);
      ATtablePut(alphas,(ATerm) p,(ATerm) l); 
    }
    else
      l=gsaGetAlpha(p,length);
    
    l=filter_hide_list(l,ATLgetArgument(a,0)); 
    assert(l);
  } 
  else if ( gsIsRename(a) ){
    ATermAppl p = ATAgetArgument(a,1);
    if(!length){
      l=gsaGetAlpha(p);
      ATtablePut(alphas,(ATerm) p,(ATerm) l); 
    }
    else
      l=gsaGetAlpha(p,length);
    
    l=filter_rename_list(l,ATLgetArgument(a,0)); 
    assert(l);
  } 
  else if ( gsIsAllow(a) ){
    ATermAppl p = ATAgetArgument(a,1);
    unsigned max_len = get_max_allowed_length(ATLgetArgument(a,0));
    if(length && max_len > length)
      max_len=length;

    l=gsaGetAlpha(p,max_len);
    
    l=filter_allow_list(l,ATLgetArgument(a,0)); 
    assert(l);
  } 
  else if ( gsIsComm(a) ){
    ATermAppl p = ATAgetArgument(a,1);
    
    l=gsaGetAlpha(p);
    ATtablePut(alphas,(ATerm) p,(ATerm) l); 
    
    l=filter_comm_list(l,ATLgetArgument(a,0)); 
    assert(l);
  } 
  else if ( gsIsSum(a) || gsIsAtTime(a) || gsIsChoice(a) || gsIsSeq(a) 
	    || gsIsCond(a) || gsIsSync(a) || gsIsMerge(a) || gsIsLMerge(a) || gsIsBInit(a)){
    // all distributing rules together
    short ia1=0,ia2=1,args=2;
    if(gsIsCond(a) || gsIsSum(a)) { ia1=1; ia2=2; }
    if(gsIsSum(a) || gsIsAtTime(a) || gsIsBInit(a)) args=1; //second argument of BInit does not matter 
    
    ATermList l2;
    l = gsaGetAlpha(ATAgetArgument(a,ia1),length);
    if(args==2) l2 = gsaGetAlpha(ATAgetArgument(a,ia2),length);
    
    if(args==2){
      ATermList l1=l;
      l = merge_list(l1,l2);
      if(gsIsSync(a) || gsIsMerge(a) || gsIsLMerge(a))
	l = merge_list(l,sync_list(l1,l2,length));
      //ATermIndexedSet m=ATindexedSetCreate(100,80);
      //gsaATindexedSetPutList(m,l);
      //gsaATindexedSetPutList(m,l2);
      //if(gsIsSync(a) || gsIsMerge(a) || gsIsLMerge(a))
      //	sync_list_ht(m,l,l2,length);
      //l = ATindexedSetElements(m);
      //ATindexedSetDestroy(m);
    }
    assert(l);
  }
  else {
    gsWarningMsg("a: %T\n\n", a);
    assert(0);
  }
  
  assert(l);
  
  if(!length){
    ATtablePut(alphas,(ATerm) a,(ATerm) l); 
  }
  
  return l;
}

static ATermAppl gsApplyAlpha(ATermAppl a){
  // apply the alpha reductions to a.
  // makes sure that the alphabet of a is in the table alphas after the function returns its value
  assert(all_stable);
  if ( gsIsDelta (a) || gsIsTau(a) ){
  }
  else if ( gsIsAction(a) ){
    //XXX may be do nothing?
    ATermList l = ATLtableGet(alphas,(ATerm)ATAgetArgument(a,0));
    if(!l){
      l = ATmakeList1((ATerm)ATmakeList1(ATgetArgument(a,0)));
      ATtablePut(alphas,(ATerm)ATAgetArgument(a,0),(ATerm)l); //for this action name+type
    }
    ATtablePut(alphas,(ATerm)a,(ATerm)l); //for this full action
  } 
  else if ( gsIsProcess(a) ){
    ATermAppl pn=ATAgetArgument(a,0);
    ATermList l=ATLtableGet(alphas,(ATerm)pn); // for this particular process term
    //XXX may be something else?
    if(!l){
      //this should be a mCRL process. 
      //we apply the alphabeth reductions to its body and then we know the alphabet
      //gsWarningMsg("Exploring new mCRL process %T\n\n",pn);
      ATermAppl new_p=gsApplyAlpha(ATAtableGet(procs,(ATerm)pn));
      //gsWarningMsg("new_p: %P\n\n",new_p);
      ATtablePut(procs,(ATerm)pn,(ATerm)new_p);
      ATtablePut(alphas,(ATerm)pn,(ATerm)ATLtableGet(alphas,(ATerm)new_p));
      l=ATLtableGet(alphas,(ATerm)pn);
    }
    assert(l);
    ATtablePut(alphas,(ATerm)a,(ATerm)l); //for this full process call
  }
  else if ( gsIsBlock(a) ){
    ATermAppl p = ATAgetArgument(a,1);
    a = PushBlock(ATLgetArgument(a,0),p); //takes care about l
  }
  else if ( gsIsHide(a) ){
    ATermAppl p = ATAgetArgument(a,1);
    a = PushHide(ATLgetArgument(a,0),p); //takes care about l
  } 
  else if ( gsIsRename(a) ){
    ATermAppl p = ATAgetArgument(a,1);
    //XXX
    p = gsApplyAlpha(p);
    a = ATsetArgument(a,(ATerm) p,1);
    ATtablePut(alphas,(ATerm) a,(ATerm)filter_rename_list(ATLtableGet(alphas,(ATerm)p),ATLgetArgument(a,0))); 
  } 
  else if ( gsIsAllow(a) ){
    ATermAppl p = ATAgetArgument(a,1);
    a = PushAllow(ATLgetArgument(a,0),p); //takes care about l
  } 
  else if ( gsIsComm(a) ){
    ATermAppl p = ATAgetArgument(a,1);
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
    
    if(args==2) 
    {
      ATermIndexedSet m=ATindexedSetCreate(10000,80);
      gsaATindexedSetPutList(m,l);
      if(args==2) gsaATindexedSetPutList(m,l2);
      if(gsIsSync(a) || gsIsMerge(a) || gsIsLMerge(a))
	sync_list_ht(m,l,l2);
      l = ATindexedSetElements(m);
      ATindexedSetDestroy(m);
    }
    
    a=ATsetArgument(a,(ATerm)p,ia1);
    if(args==2) a=ATsetArgument(a,(ATerm)q,ia2);
    
    ATtablePut(alphas,(ATerm) a,(ATerm) l);
  }

  assert(ATLtableGet(alphas,(ATerm) a));
  
  return a;
}

ATermList gsaGetDeps(ATermAppl a){
  //returns process names that a depends to (should be applied iteratevly).
  ATermList r=ATmakeList0();
  if ( gsIsDelta(a) || gsIsTau(a) || gsIsAction(a) ){
    return r;
  }
  else if ( gsIsProcess(a) ){
    ATermAppl pn=ATAgetArgument(a,0);
    r=ATmakeList1((ATerm)pn);
    ATermList dep=ATLtableGet(deps,(ATerm)pn);
    if(dep) return merge_list(r,dep);
    //ATtablePut(deps,(ATerm)pn,(ATerm)r);
    return r;
  }
  else if ( gsIsSum(a) || gsIsAtTime(a) || gsIsChoice(a) || gsIsSeq(a) 
	    || gsIsBlock(a) || gsIsHide(a) || gsIsRename(a) || gsIsAllow(a) || gsIsComm(a)
	    || gsIsCond(a) || gsIsSync(a) || gsIsMerge(a) || gsIsLMerge(a) || gsIsBInit(a)){
    // all distributing rules together
    short ia1=0,ia2=1,args=2;
    if(gsIsCond(a) || gsIsSum(a)
       || gsIsBlock(a) || gsIsHide(a) || gsIsRename(a) || gsIsAllow(a) || gsIsComm(a)
       ) { ia1=1; ia2=2; }
    if(gsIsSum(a) || gsIsAtTime(a) 
       || gsIsBlock(a) || gsIsHide(a) || gsIsRename(a) || gsIsAllow(a) || gsIsComm(a)
       ) args=1; //second argument of BInit does not matter 
    
    r = gsaGetDeps(ATAgetArgument(a,ia1));
    if(args==2) r = merge_list(r,gsaGetDeps(ATAgetArgument(a,ia2)));
    return r;
  }
  assert(0);
}

ATermAppl gsaGetProp(ATermAppl a, ATermAppl context){
  //returns the type of the term.
  ATermAppl r=pCRL_aterm;
  if ( gsIsDelta(a) || gsIsTau(a) || gsIsAction(a) ){
    return r;
  } 
  else if ( gsIsProcess(a) ){
    ATermAppl pn=ATAgetArgument(a,0);
    return ATAgetArgument(ATAtableGet(props,(ATerm)pn),0);
  }
  else if ( gsIsSum(a) || gsIsAtTime(a) || gsIsChoice(a) || gsIsSeq(a) 
	    || gsIsBlock(a) || gsIsHide(a) || gsIsRename(a) || gsIsAllow(a) || gsIsComm(a)
	    || gsIsCond(a) || gsIsSync(a) || gsIsMerge(a) || gsIsLMerge(a) || gsIsBInit(a)){
    // all distributing rules together
    short ia1=0,ia2=1,args=2;
    if(gsIsCond(a) || gsIsSum(a)
       || gsIsBlock(a) || gsIsHide(a) || gsIsRename(a) || gsIsAllow(a) || gsIsComm(a)
       ) { ia1=1; ia2=2; }
    if(gsIsSum(a) || gsIsAtTime(a) 
       || gsIsBlock(a) || gsIsHide(a) || gsIsRename(a) || gsIsAllow(a) || gsIsComm(a)
       ) args=1; 
    
    ATermAppl p,q;
    p = gsaGetProp(ATAgetArgument(a,ia1),context);
    if(args==2) q = gsaGetProp(ATAgetArgument(a,ia2),context);

    if(args==1) return p;
    if(ATisEqual(p,mCRL_aterm)) return p;
    if(ATisEqual(q,mCRL_aterm)) return q;
    if(gsIsMerge(a) || gsIsLMerge(a)) return mCRL_aterm;
    if(gsIsSync(a)){
      if(ATindexOf(gsaGetDeps(a),(ATerm)context,0)>=0)
	return mCRL_aterm;
      else 
	return pCRL_aterm;
    }
    
    return r;
  }
  assert(0);
}

ATermAppl gsaSubstNP(ATermTable subs_npCRL, ATermTable consts, ATermAppl a){
  //returns the type of the term.
  if ( gsIsDelta(a) || gsIsTau(a) || gsIsAction(a) ){
    return a;
  } 
  else if ( gsIsProcess(a) ){
    ATermAppl pn=ATAgetArgument(a,0);
    ATermList l=ATLtableGet(subs_npCRL,(ATerm)pn);
    if(!l) return a; //not an npCRL process call.
    
    //determine the value of the parameter.
    assert(ATgetLength(ATLgetArgument(a,1))==1); 
    ATermAppl par=ATAgetFirst(ATLgetArgument(a,1));
    ATermAppl k=NULL;
    if(gsIsNumber(par) && ATisEqual(ATAgetArgument(par,1),gsMakeSortIdPos())){
      k=ATAgetArgument(par,0);
    }
    else 
      if(gsIsOpId(par) && ATisEqual(ATAgetArgument(par,1),gsMakeSortIdPos())){
	k=ATAtableGet(consts,(ATerm)ATAgetArgument(par,0));
      }
    if(!k){
      gsWarningMsg("cannot evaluate the parameter value in process term %T\n as a positive number\n\n",a);
      return NULL;
    }
    
    //check if we have already seen such a process call (for k):
    {
      ATermList l1=l;
      for(;!ATisEmpty(l);l=ATgetNext(l)){
	ATermAppl pair=ATAgetFirst(l);
	if(ATisEqual(ATAgetArgument(pair,0),k))
	  return ATAgetArgument(pair,1);
      }
      l=l1;
    }

    //create a new entry and add
    //process name with type pn, add _k 
    ATermAppl new_pn=ATsetArgument(pn,(ATerm)ATmakeList0(),1);
    do{
      new_pn=ATsetArgument(new_pn,(ATerm)ATmakeAppl0(ATappendAFun(ATappendAFun(ATgetAFun(ATAgetArgument(new_pn,0)),"_"),ATgetName(ATgetAFun(k)))),0);  
    } while(ATtableGet(procs,(ATerm)new_pn));
    
    ATtablePut(subs_npCRL,(ATerm)pn,(ATerm)ATinsert(l,(ATerm)Pair((ATerm)k,(ATerm)new_pn)));
    return gsMakeProcess(new_pn,ATmakeList0());
  }
  else if ( gsIsSum(a) || gsIsAtTime(a) || gsIsChoice(a) || gsIsSeq(a) 
	    || gsIsBlock(a) || gsIsHide(a) || gsIsRename(a) || gsIsAllow(a) || gsIsComm(a)
	    || gsIsCond(a) || gsIsSync(a) || gsIsMerge(a) || gsIsLMerge(a) || gsIsBInit(a)){
    // all distributing rules together
    short ia1=0,ia2=1,args=2;
    if(gsIsCond(a) || gsIsSum(a)
       || gsIsBlock(a) || gsIsHide(a) || gsIsRename(a) || gsIsAllow(a) || gsIsComm(a)
       ) { ia1=1; ia2=2; }
    if(gsIsSum(a) || gsIsAtTime(a) 
       || gsIsBlock(a) || gsIsHide(a) || gsIsRename(a) || gsIsAllow(a) || gsIsComm(a)
       ) args=1; 
    
    ATermAppl p,q;
    p = gsaSubstNP(subs_npCRL,consts,ATAgetArgument(a,ia1));
    if(args==2) q = gsaSubstNP(subs_npCRL,consts,ATAgetArgument(a,ia2));

    a=ATsetArgument(a,(ATerm)p,ia1);
    if(args==2) a=ATsetArgument(a,(ATerm)q,ia2);
    return a;
  }
  assert(0);  
}

ATermAppl gsaGenNInst(ATermAppl number, ATermAppl P){
  //return a || composition of n processes P
  unsigned long n=atol(ATgetName(ATgetAFun(number)));
  ATermAppl r=gsMakeProcess(P,ATmakeList1((ATerm)gsMakeNumber(ATmakeAppl0(ATmakeAFunInt0(1)),gsMakeSortIdPos())));
  for(unsigned long i=2; i<=n; i++){
    r=gsMakeMerge(r,gsMakeProcess(P,ATmakeList1((ATerm)gsMakeNumber(ATmakeAppl0(ATmakeAFunInt0(i)),gsMakeSortIdPos()))));
  }

  return r;
}

ATermAppl gsAlpha(ATermAppl Spec){
  //create the tables
  afunPair=ATmakeAFun("p",2,ATfalse);
  ATprotectAFun(afunPair);
  syncs = ATtableCreate(10000,80);
  untypes = ATtableCreate(10000,80);
  form_pars= ATtableCreate(10000,80);

  procs = ATtableCreate(10000,80);
  tmpIndexedSet = ATindexedSetCreate(63,50);

  //fill in tables
  for(ATermList pr=ATLgetArgument(ATAgetArgument(Spec,5),0); !ATisEmpty(pr); pr=ATgetNext(pr)){
    ATermAppl p=ATAgetFirst(pr);
    ATermAppl pn=ATAgetArgument(p,1);
    ATtablePut(procs,(ATerm)pn,(ATerm)ATAgetArgument(p,3));
    ATtablePut(form_pars,(ATerm)pn,(ATerm)ATLgetArgument(p,2));
  }
  
  ATtablePut(procs,(ATerm)INIT_KEY(),(ATerm)ATAgetArgument(ATAgetArgument(Spec,6),1));

  //Calculate the dependensies of the processes.
  //we start from init and iterate on the processes init depends upon init until the system stabilises.
  deps=ATtableCreate(10000,80); //process dependensies : P(Pname,type) -> List(P(Pname,type))
  bool stable=false;
  while(!stable){
    //apply to each and compare with the old values.
    stable=true;
    ATermList todo=ATLtableGet(deps,(ATerm)INIT_KEY());
    if(todo) todo=ATinsert(todo,(ATerm)INIT_KEY());
    else todo=ATmakeList1((ATerm)INIT_KEY());

    for(; !ATisEmpty(todo); todo=ATgetNext(todo)){
      ATermAppl pn=ATAgetFirst(todo);
      ATermList old_dep=ATLtableGet(deps,(ATerm)pn);
      if(!old_dep){
	old_dep=ATmakeList0();
	ATtablePut(deps,(ATerm)pn,(ATerm)old_dep);
      }
      ATermList dep=gsaGetDeps(ATAtableGet(procs,(ATerm)pn));
      //gsWarningMsg("proc: %T, dep: %T; old_dep: %T\n\n", pn, dep, old_dep);
      if(!ATisEqual(dep,old_dep)){
	stable=false;
	ATtablePut(deps,(ATerm)pn,(ATerm)dep);
      }
    }
  }

  //Classify the *reachable from init* process equations in 3 parts:
  //pCRL
  //n-parallel pCRL
  //mCRL
  //also check if process name depends recursively on itself.
  //(mCRL processes cannot recursively depend on itself for the *current* linearizer to work)
  //n-parallel pCRL processes always recursively depend on themselves
  props_afun=ATmakeAFun("props",2,ATfalse);
  ATprotectAFun(props_afun);
  pCRL_aterm=gsString2ATermAppl("pCRL");
  npCRL_aterm=gsString2ATermAppl("npCRL");
  mCRL_aterm=gsString2ATermAppl("mCRL");
  rec_aterm=gsString2ATermAppl("rec");
  nrec_aterm=gsString2ATermAppl("nrec");
  props=ATtableCreate(10000,80); //process properties
  //ATprotectAppl(pCRL_aterm);
  //ATprotectAppl(npCRL_aterm);
  //ATprotectAppl(mCRL_aterm);
  //ATprotectAppl(rec_aterm);
  //ATprotectAppl(nrec_aterm);

  //recursive or not?
  ATermList todo=ATLtableGet(deps,(ATerm)INIT_KEY());
  if(todo) todo=ATinsert(todo,(ATerm)INIT_KEY());
  else todo=ATmakeList1((ATerm)INIT_KEY());
  for(; !ATisEmpty(todo); todo=ATgetNext(todo)){
    ATermAppl p=ATAgetFirst(todo);
    ATermList dep=ATLtableGet(deps,(ATerm)p);
    bool rec=(ATindexOf(dep,(ATerm)p,0)>=0);
    ATtablePut(props,(ATerm)p,(ATerm)ATmakeAppl2(props_afun,(ATerm)pCRL_aterm,(rec)?(ATerm)rec_aterm:(ATerm)nrec_aterm));
  }
  
  //mCRL, or pCRL
  stable=false;
  while(!stable){
    //apply to each and compare with the old values.
    stable=true;
    ATermList todo=ATLtableGet(deps,(ATerm)INIT_KEY());
    if(todo) todo=ATinsert(todo,(ATerm)INIT_KEY());
    else todo=ATmakeList1((ATerm)INIT_KEY());
    for(; !ATisEmpty(todo); todo=ATgetNext(todo)){
      ATermAppl p=ATAgetFirst(todo);
      ATermAppl prop=ATAtableGet(props,(ATerm)p);
      ATermAppl new_prop=ATsetArgument(prop,(ATerm)gsaGetProp(ATAtableGet(procs,(ATerm)p),p),0);
      if(!ATisEqual(prop,new_prop)){
	ATtablePut(props,(ATerm)p,(ATerm)new_prop);
	stable=false;
      }
    }
  }
  
  // get n-parallel pCRL processes and check if they really are
  // e.g. if mCRL and rec, check for npCRL, if not -- Warning.
  // put all npCRL processes as <nP>->[] into subs_npCRL.
  todo=ATLtableGet(deps,(ATerm)INIT_KEY());
  if(todo) todo=ATinsert(todo,(ATerm)INIT_KEY());
  else todo=ATmakeList1((ATerm)INIT_KEY());
  ATermAppl npCRL=ATmakeAppl2(props_afun,(ATerm)mCRL_aterm,(ATerm)rec_aterm);
  ATermTable subs_npCRL=ATtableCreate(10000,80);
  for(; !ATisEmpty(todo); todo=ATgetNext(todo)){
    ATermAppl p=ATAgetFirst(todo);
    if(ATisEqual(ATAtableGet(props,(ATerm)p),npCRL)){
      //check if this is really npCRL.
      //what we currently recognize is this:
      //<nP>(p:Pos)=(p>1)-><P>(p)||<nP>(max(p-1,1)),<P>(1)
      //where <nP> and <P> are process names parameterized by Pos;
      //the rest a literal; <P> does not depend on <nP>
      //
      //all calls of <nP> should be of the form <nP>(<m>), where <m>:->Pos is a map
      //and an equation <m>=<p> for some positive <p>
      //
      //later we can add more (For sort Nat, P(n)||nP(max(n-1,1)) in diff. order, P(n) instead of P(1))
      
      //p is <nP>
      //lets learn what <P> is
      ATermAppl P=NULL;
      ATermAppl body=ATAtableGet(procs,(ATerm)p);
      if(gsIsCond(body)){
	ATermAppl p1=ATAgetArgument(body,2);
	if(gsIsProcess(p1))
	  P=ATAgetArgument(p1,0);
      }
      
      bool good=false;
      if(P){
	//check
	good=true;
      }

      if(good){
	ATtablePut(props,(ATerm)p,(ATerm)ATmakeAppl2(props_afun,(ATerm)npCRL_aterm,(ATerm)rec_aterm));
	ATtablePut(subs_npCRL,(ATerm)p,(ATerm)ATmakeList0());
      }
      else{
	gsWarningMsg("proc: %T is a recursive parallel process not in n-parallel pCRL format\n in this case alphabet reductions may not stop, or may not be performed completely\n\n", p);
      }
    }
  }

  //=================== Begin removing npCRL processes

  //For every occurrence of an npCRL process call <nP>(k) replace it to nP_newn_k
  //if such process does not exist.
  //Remember all substitutions made for each <nP>.

  ////First make a table of Positive constants
  ATermTable consts=ATtableCreate(10000,80);
  for(ATermList l=ATLgetArgument(ATAgetArgument(Spec,3),0);!ATisEmpty(l);l=ATgetNext(l)){
    ATermAppl eq=ATAgetFirst(l);
    ATermAppl left=ATAgetArgument(eq,2);
    ATermAppl right=ATAgetArgument(eq,3);
    if(gsIsOpId(left) && ATisEqual(ATAgetArgument(left,1),gsMakeSortIdPos()) && 
       gsIsNumber(right) && ATisEqual(ATAgetArgument(right,1),gsMakeSortIdPos())){
      ATtablePut(consts,(ATerm)ATAgetArgument(left,0),(ATerm)ATAgetArgument(right,0));
    }
  }
  bool success=true;
  todo=ATLtableGet(deps,(ATerm)INIT_KEY());
  if(todo) todo=ATinsert(todo,(ATerm)INIT_KEY());
  else todo=ATmakeList1((ATerm)INIT_KEY());
  npCRL=ATmakeAppl2(props_afun,(ATerm)npCRL_aterm,(ATerm)rec_aterm);
  for(; !ATisEmpty(todo); todo=ATgetNext(todo)){
    ATermAppl p=ATAgetFirst(todo);
    if(!ATisEqual(ATAtableGet(props,(ATerm)p),npCRL)){
      ATermAppl new_p=gsaSubstNP(subs_npCRL,consts,ATAtableGet(procs,(ATerm)p));
      if(!new_p) {
	gsWarningMsg("could not replace all npCRL processes\n in this case alphabet reductions may not stop, or may not be performed completely\n\n", p);
	success=false;
	// no break; because we are not lasy
      }
      else{
	ATtablePut(procs,(ATerm)p,(ATerm)new_p);
	//gsWarningMsg("new_p: %P\n\n", new_p);
      }
    }
  }
  ATtableDestroy(consts);
  
  //If success -- replace all npCRL processes with their expansions.
  //Otherwise -- just add the expansions.
  todo=ATtableKeys(subs_npCRL);
  for(; !ATisEmpty(todo); todo=ATgetNext(todo)){
    ATermAppl nP=ATAgetFirst(todo);
    ATermAppl P=ATAgetArgument(ATAgetArgument(ATAtableGet(procs,(ATerm)nP),2),0);
    //get the list of substitutions and generate the process for each
    for(ATermList l=ATLtableGet(subs_npCRL,(ATerm)nP);!ATisEmpty(l);l=ATgetNext(l)){
      ATermAppl pair=ATAgetFirst(l);
      ATermAppl name=ATAgetArgument(pair,1);
      ATtablePut(procs,(ATerm)name,(ATerm)gsaGenNInst(ATAgetArgument(pair,0),P));
      ATtablePut(props,(ATerm)name,(ATerm)ATmakeAppl2(props_afun,(ATerm)mCRL_aterm,(ATerm)nrec_aterm));
      ATtablePut(deps,(ATerm)name,(ATerm)merge_list(ATmakeList1((ATerm)P),ATLtableGet(deps,(ATerm)P)));
    }
    if(success){
      //remove p from all tables
      ATtableRemove(procs,(ATerm)nP);
      ATtableRemove(props,(ATerm)nP);
      ATtableRemove(deps,(ATerm)nP);
    }
  }
  ATtableDestroy(subs_npCRL);

  //rebuild dependensies 
  ATtableReset(deps); //process dependensies : P(Pname,type) -> List(P(Pname,type))
  stable=false;
  while(!stable){
    //apply to each and compare with the old values.
    stable=true;
    ATermList todo=ATLtableGet(deps,(ATerm)INIT_KEY());
    if(todo) todo=ATinsert(todo,(ATerm)INIT_KEY());
    else todo=ATmakeList1((ATerm)INIT_KEY());

    for(; !ATisEmpty(todo); todo=ATgetNext(todo)){
      ATermAppl pn=ATAgetFirst(todo);
      ATermList old_dep=ATLtableGet(deps,(ATerm)pn);
      if(!old_dep){
	old_dep=ATmakeList0();
	ATtablePut(deps,(ATerm)pn,(ATerm)old_dep);
      }
      ATermList dep=gsaGetDeps(ATAtableGet(procs,(ATerm)pn));
      //gsWarningMsg("proc: %T, dep: %T; old_dep: %T\n\n", pn, dep, old_dep);
      if(!ATisEqual(dep,old_dep)){
	stable=false;
	ATtablePut(deps,(ATerm)pn,(ATerm)dep);
      }
    }
  }

  //======================== end removing npCRL procs

  //calculate the alphabets of the processes iteratively
  //for pCRL processes (for || processes this may be too expensive)
  alphas = ATtableCreate(10000,80);
  subs_alpha = ATtableCreate(10000,80);
  subs_alpha_rev = ATtableCreate(10000,80);

  todo=ATLtableGet(deps,(ATerm)INIT_KEY());
  if(todo) todo=ATinsert(todo,(ATerm)INIT_KEY());
  else todo=ATmakeList1((ATerm)INIT_KEY());
  for(ATermList pr=todo; !ATisEmpty(pr); pr=ATgetNext(pr)){
    ATermAppl pn=ATAgetFirst(pr);
    if(ATisEqual(ATAgetArgument(ATAtableGet(props,(ATerm)pn),0),mCRL_aterm)) continue;
    ATtablePut(alphas,(ATerm)pn,(ATerm)ATmakeList0());
  }
  
  ATtablePut(alphas,(ATerm)gsMakeDelta(),(ATerm) ATmakeList0());
  ATtablePut(alphas,(ATerm)gsMakeTau(),(ATerm) ATmakeList0());
  
  todo=ATLtableGet(deps,(ATerm)INIT_KEY());
  if(todo) todo=ATinsert(todo,(ATerm)INIT_KEY());
  else todo=ATmakeList1((ATerm)INIT_KEY());
  stable=false;
  all_stable=false;
  //possibly endless loop (X=a.X||X ;)
  while(!stable){
    //apply Alpha to each and compare with the old values.
    stable=true;
    for(ATermList pr=todo; !ATisEmpty(pr); pr=ATgetNext(pr)){
      ATermAppl pn=ATAgetFirst(pr);
      if(ATisEqual(ATAgetArgument(ATAtableGet(props,(ATerm)pn),0),mCRL_aterm)) continue;
      //if(ATisEqual(ATAgetArgument(ATAtableGet(props,(ATerm)pn)),1),nrec_aterm) continue; 
      ATermList old_l=ATLtableGet(alphas,(ATerm)pn);
      ATermList l=gsaGetAlpha(ATAtableGet(procs,(ATerm)pn));
      ATtablePut(alphas,(ATerm)pn,(ATerm)l);
      //gsWarningMsg("Exploring new pCRL process %T\n\n",pn);
      if(ATgetLength(old_l)!=ATgetLength(l) || !ATisEqual(list_minus(old_l,l),ATmakeList0()))
	stable=false;
    }
  }
  all_stable=true;

  // apply the reduction to init 
  // it will recursively trigger all mCRL processes that init depends upon.
  {
    ATermAppl pn=INIT_KEY();
    ATermAppl new_p=gsApplyAlpha(ATAtableGet(procs,(ATerm)pn));
    ATtablePut(procs,(ATerm)pn,(ATerm)new_p);
    ATtablePut(alphas,(ATerm)pn,(ATerm)ATLtableGet(alphas,(ATerm)new_p));
  }
  
  //recalculate the new dependensies again
  ATtableReset(deps); //process dependensies : P(Pname,type) -> List(P(Pname,type))
  stable=false;
  while(!stable){
    //apply to each and compare with the old values.
    stable=true;
    ATermList todo=ATLtableGet(deps,(ATerm)INIT_KEY());
    if(todo) todo=ATinsert(todo,(ATerm)INIT_KEY());
    else todo=ATmakeList1((ATerm)INIT_KEY());

    for(; !ATisEmpty(todo); todo=ATgetNext(todo)){
      ATermAppl pn=ATAgetFirst(todo);
      ATermList old_dep=ATLtableGet(deps,(ATerm)pn);
      if(!old_dep){
	old_dep=ATmakeList0();
	ATtablePut(deps,(ATerm)pn,(ATerm)old_dep);
      }
      ATermList dep=gsaGetDeps(ATAtableGet(procs,(ATerm)pn));
      //gsWarningMsg("proc: %T, dep: %T; old_dep: %T\n\n", pn, dep, old_dep);
      if(!ATisEqual(dep,old_dep)){
	stable=false;
	ATtablePut(deps,(ATerm)pn,(ATerm)dep);
      }
    }
  }

  //== write out the process equations
  //first the original ones (except deleted)
  ATermList new_pr=ATmakeList0();
  for(ATermList pr=ATLgetArgument(ATAgetArgument(Spec,5),0); !ATisEmpty(pr); pr=ATgetNext(pr)){
    ATermAppl p=ATAgetFirst(pr);
    ATermAppl pn=ATAgetArgument(p,1);
    ATermAppl res=ATAtableGet(procs,(ATerm)pn);
    if(res){
      new_pr=ATinsert(new_pr,(ATerm)ATsetArgument(p,(ATerm)res,3));
      ATtableRemove(procs,(ATerm)pn);
    }
  }
  //now the generated ones
  todo=ATLtableGet(deps,(ATerm)INIT_KEY());
  for(ATermList pr=todo; !ATisEmpty(pr); pr=ATgetNext(pr)){
    ATermAppl pn=ATAgetFirst(pr);
    if(ATisEqual(pn,INIT_KEY())) continue;
    ATermAppl res=ATAtableGet(procs,(ATerm)pn);
    if(res){
      ATermList fpars=ATmakeList0();
      //if generated during the alpha substitutions
      ATermAppl old_pn=ATAtableGet(subs_alpha_rev,(ATerm)pn);
      if(old_pn){
	ATermList fpars1=ATLtableGet(form_pars,(ATerm)old_pn);
	if(fpars1) fpars = fpars1;
      }
      new_pr=ATinsert(new_pr,(ATerm)gsMakeProcEqn(ATmakeList0(),pn,fpars,ATAtableGet(procs,(ATerm)pn)));
    }
  }
  new_pr=ATreverse(new_pr);

  gsDebugMsg("init: l:%T\n\n", ATLtableGet(alphas,(ATerm)ATAtableGet(procs,(ATerm)INIT_KEY())));

  Spec = ATsetArgument(Spec,(ATerm) gsMakeProcEqnSpec(new_pr),5);
  Spec = ATsetArgument(Spec,(ATerm) gsMakeInit(ATLgetArgument(ATAgetArgument(Spec,6),0),ATAtableGet(procs,(ATerm)INIT_KEY())),6);
  
  ATindexedSetDestroy(tmpIndexedSet);
  ATtableDestroy(alphas);
  ATtableDestroy(procs);
  ATtableDestroy(props);
  ATtableDestroy(deps);
  ATtableDestroy(form_pars);
  ATtableDestroy(subs_alpha);
  ATtableDestroy(subs_alpha_rev);
  ATtableDestroy(syncs);
  ATtableDestroy(untypes);
  return Spec;
}
