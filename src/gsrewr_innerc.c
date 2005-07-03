/* $Id: gsmewr_innerc.c,v 1.3 2005/04/14 11:57:17 muck Exp $ */

#define NAME "rewr_innerc"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"
#include "gsrewr_innerc.h"
#include "libgsparse.h"
#include "gsrewr_innerc_aux.h"
#include "gssubstitute.h"
#include "assert.h"

#ifndef INNERC_CFLAGS
#define INNERC_CFLAGS  ""
#endif
#ifndef INNERC_CPPFLAGS
#define INNERC_CPPFLAGS  ""
#endif
#ifndef INNERC_LDFLAGS
#define INNERC_LDFLAGS  ""
#endif

#define ATXgetArgument(x,y) ((unsigned int) ATgetArgument(x,y))

extern ATermList opid_eqns;
extern ATermList dataappl_eqns;

static ATermTable term2int;
static unsigned int num_opids=0;
static ATermAppl *int2term;
static ATermList *innerc_eqns;
static int true_num;
static void (*so_rewr_init)();
static ATermAppl (*so_rewr)(ATermAppl);

static ATermList ATinsertA(ATermList l, ATermAppl a)
{ return ATinsert(l,(ATerm)a);
}

static bool ATisAppl(ATerm a)
{
  return (ATgetType(a) == AT_APPL);
}

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

static ATerm Apply(ATermList l)
{
  char c[10];
  int n=ATgetLength(l);
  sprintf(c,"appl#%d",n);
  
  return (ATerm)ATmakeApplList(ATmakeAFun(c,n,ATfalse),l);
}

static ATerm toInnerc(ATerm Term)
{
  if ( !ATisList(Term) )
  {
    if ( ATisInt(Term) )
    { return Apply(ATinsert(ATempty,Term));
    } 
    else 
    if (gsIsDataVarId((ATermAppl)Term))
    { return Term;
    }
    else
    {
      ATerror("%s: Do not deal with application terms correctly\n%t\n\n",NAME,Term);
    }
  }

  if ( ATisEmpty((ATermList) Term) )
  {
    ATerror("%s: invalid inner format term (%t)\n",NAME,Term);
  }

  ATermList l=ATinsert(ATempty,ATgetFirst((ATermList)Term));
  for( ATermList l1=ATgetNext((ATermList)Term) ; 
       l1!=ATempty ; 
       l1=ATgetNext(l1))
  { l=ATinsert(l,toInnerc(ATgetFirst(l1)));
  }
  l=ATreverse(l);
  ATerm r=Apply((ATermList) l);
  // ATfprintf(stderr,"RESULT: %t\n%t\n%t\n\n",Term,r,l);
  return r;
}

ATerm to_rewrite_format_innerc(ATermAppl t)
{ 
  return toInnerc(toInner(t,true));
}

ATermAppl from_rewrite_format_innerc(ATerm t)
{ 
  int n=ATgetArity(ATgetAFun(t));
  if (n==1)
  { ATerm t1=ATgetArgument(t,0);
    if (ATisInt(t1)) 
    { return fromInner(t1);
    }
  }
  
  return fromInner((ATerm)ATgetArguments((ATermAppl)t));
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

char checkArg_prefix[] = "a";
void checkArg(FILE *f, ATerm t, ATermList *n, int *d, int *ls, char *pref, int k)
{
  if ( ATisList(t) )
  {
  fprintf(f,  "    %sif ( isAppl(%s%i) && ATisEqual(ATgetArgument(%s%i,0),int2ATerm%i) )\n"// && (ATgetArity(ATgetAFun(arg%i)) == 1 )\n"
      "    %s{\n",
      whitespace((*d)*2),pref,(k<0)?*ls:k,pref,(k<0)?*ls:k,ATgetInt((ATermInt) ATgetFirst((ATermList) t)),whitespace((*d)*2)//,k
         );
    (*d)++;
    ATermList l = (ATermList) t;
    l = ATgetNext(l);
    int i = 1;
    int oldls = *ls;
    for (; !ATisEmpty(l); l=ATgetNext(l))
    {
  fprintf(f,  "    %s{\n"
      "    %s  ATermAppl %s%i = (ATermAppl) ATgetArgument(%s%i,%i);\n",
      whitespace((*d)*2),whitespace((*d)*2),checkArg_prefix,(*ls)+1,pref,(k<0)?oldls:k,i
         );
      (*d)++;
      (*ls)++;
      checkArg(f,ATgetFirst(l),n,d,ls,checkArg_prefix,-1);
      i++;
    }
  } else if ( ATisInt(t) )
  {
//  fprintf(f,  "    %sif ( isAppl(%s%i) && ATisEqual(ATgetArgument(%s%i,0),int2ATerm%i) )\n"// && (ATgetArity(ATgetAFun(arg%i)) == 1 )\n"
//      whitespace((*d)*2),pref,(k<0)?*ls:k,pref,(k<0)?*ls:k,ATgetInt((ATermInt) t),whitespace((*d)*2)//,k
//         );
  fprintf(f,  "    %sif ( ATisEqual(%s%i,rewrAppl%i) )\n"// && (ATgetArity(ATgetAFun(arg%i)) == 1 )\n"
      "    %s{\n",
      whitespace((*d)*2),pref,(k<0)?*ls:k,ATgetInt((ATermInt) t),whitespace((*d)*2)//,k
         );
    (*d)++;
  } else {
    if ( ATindexOf(*n,t,0) >= 0 )
    {
  fprintf(f,  "    %svar_%s_%x = %s%i;\n",
      whitespace((*d)*2),ATgetName(ATgetAFun(ATAgetArgument((ATermAppl) t,0))),ATXgetArgument((ATermAppl) t,1),pref,(k<0)?*ls:k
         );
      *n = ATremoveAll(*n,t);
    } else {
  fprintf(f,  "    %sif ( ATisEqual(var_%s_%x,%s%i) )\n"
      "    %s{\n",
      whitespace((*d)*2),ATgetName(ATgetAFun(ATAgetArgument((ATermAppl) t,0))),ATXgetArgument((ATermAppl) t,1),pref,(k<0)?*ls:k,
      whitespace((*d)*2)
         );
      (*d)++;
    }
  }
}

void calcTerm(FILE *f, ATerm t, int startarg)
{
  if ( ATisList(t) )
  {
    int arity = ATgetLength((ATermList) t)-1;
    ATermList l;
    bool b = false;
    bool v = false;

    if ( !ATisInt(ATgetFirst((ATermList) t)) )
    {
      if ( arity == 0 )
      {
        calcTerm(f,ATgetFirst((ATermList) t),0);
        return;
      }

      v = true;
      fprintf(f,"(isAppl(");
      calcTerm(f,ATgetFirst((ATermList) t),0);
      fprintf(f,")?varFunc%i(",arity);
      calcTerm(f,ATgetFirst((ATermList) t),0);
      l = ATgetNext((ATermList) t);
      int i = startarg;
      for (; !ATisEmpty(l); l=ATgetNext(l))
      {
        fprintf(f,",");
        if ( ATisAppl(ATgetFirst(l)) && gsIsNil(ATAgetFirst(l)) )
        {
          fprintf(f,"arg%i",i);
        } else {
          calcTerm(f,ATgetFirst(l),0);
        }
        i++;
      }
      fprintf(f,"):");
    }

    if ( ATisInt(ATgetFirst((ATermList) t)) && (l = innerc_eqns[ATgetInt((ATermInt) ATgetFirst((ATermList) t))]) != NULL )
    {
      for (; !ATisEmpty(l); l=ATgetNext(l))
      {
        if ( ATgetLength(ATLelementAt(ATLgetFirst(l),2)) <= arity )
        {
          b = true;
          break;
        }
      }
    }

    if ( b )
    {
      fprintf(f,"rewr_%i_%i(",ATgetInt((ATermInt) ATgetFirst((ATermList) t)),arity);
    } else {
      if ( arity == 0 )
      {
        fprintf(f,"(rewrAppl%i",ATgetInt((ATermInt) ATgetFirst((ATermList) t)));
      } else {
        if ( arity > 5 )
        {
          fprintf(f,"ATmakeAppl(appl%i,",arity);
        } else {
          fprintf(f,"ATmakeAppl%i(appl%i,",arity+1,arity);
        }
        if ( ATisInt(ATgetFirst((ATermList) t)) )
        {
          fprintf(f,"(ATerm) int2ATerm%i",ATgetInt((ATermInt) ATgetFirst((ATermList) t)));
        } else {
          fprintf(f,"(ATerm) ");
          calcTerm(f,ATgetFirst((ATermList) t),0);
        }
      }
    }
    l = ATgetNext((ATermList) t);
    bool c = !b;
    int i = startarg;
    for (; !ATisEmpty(l); l=ATgetNext(l))
    {
      if ( c )
      {
        fprintf(f,",");
      } else {
        c = true;
      }
      if ( !b )
      {
        fprintf(f,"(ATerm) ");
      }
      if ( ATisAppl(ATgetFirst(l)) && gsIsNil(ATAgetFirst(l)) )
      {
        fprintf(f,"arg%i",i);
      } else {
        calcTerm(f,ATgetFirst(l),0);
      }
      i++;
    }
    fprintf(f,")");
    
    if ( v )
    {
      fprintf(f,")");
    }
  } else if ( ATisInt(t) )
  {
    ATermList l;
    bool b = false;
    if ( (l = innerc_eqns[ATgetInt((ATermInt) t)]) != NULL )
    {
      for (; !ATisEmpty(l); l=ATgetNext(l))
      {
        if ( ATgetLength(ATLelementAt(ATLgetFirst(l),2)) == 0 )
        {
          b = true;
          break;
        }
      }
    }

    if ( b )
    {
      fprintf(f,"rewr_%i_0()",ATgetInt((ATermInt) t));
    } else {
//      fprintf(f,"ATmakeAppl1(appl0,int2ATerm%i)",ATgetInt((ATermInt) t));
      fprintf(f,"rewrAppl%i",ATgetInt((ATermInt) t));
    }
  } else {
    fprintf(f,"var_%s_%x",ATgetName(ATgetAFun(ATAgetArgument((ATermAppl) t,0))),ATXgetArgument((ATermAppl) t,1));
  }
}

ATerm add_args(ATerm a, int num)
{
  if ( num == 0 )
  {
    return a;
  } else {
    ATermList l;

    if ( ATisList(a) )
    {
      l = (ATermList) a;
    } else {
      l = ATmakeList1(a);
    }

    while ( num > 0 )
    {
      l = ATappend(l,(ATerm) gsMakeNil());
      num--;
    }
    return (ATerm) l;
  }
}

int get_startarg(ATerm a)
{
  if ( ATisList(a) )
  {
    return -ATgetLength(a)+1;
  } else {
    return 0;
  }
}

int getArity(ATermAppl op)
{
  ATermAppl sort = ATAgetArgument(op,1);
  int arity = 0;

  while ( gsIsSortArrow(sort) )
  {
    sort = ATAgetArgument(sort,1);
    arity++;
  }

  return arity;
}

static void CompileRewriteSystem(void)
{ ATermList l,m,n;
  ATermTable tmp_eqns;
  ATermInt i;
  int j,k,d,ls;
  FILE *f;
  char *s,*t;
  void *h;        


  tmp_eqns = ATtableCreate(100,75); // XXX would be nice to know the number op OpIds
  term2int = ATtableCreate(100,75);

  true_num = ATgetInt((ATermInt) OpId2Int(gsMakeDataExprTrue(),true));

  l = opid_eqns;
//  ATfprintf(stderr,"OPIDEQNS %t\n\n",l);
  for (; !ATisEmpty(l); l=ATgetNext(l))
  {
    // XXX only adds the last rule where lhs is an opid; this might go "wrong" if 
    // this rule is removed later
    ATtablePut(
      tmp_eqns,
      OpId2Int(ATAgetArgument(ATAgetFirst(l),2),true),
      (ATerm) ATmakeList1((ATerm) 
                ATmakeList4(
                  (ATerm) ATmakeList0(),
                  toInner(ATAgetArgument(ATAgetFirst(l),1),true),   // condition.
                  (ATerm) ATmakeList0(),
                  toInner(ATAgetArgument(ATAgetFirst(l),3),true)))); // rhs.
  }

  l = dataappl_eqns;
//  ATfprintf(stderr,"DATAAPPL_EQNS %t\n\n",l);
  for (; !ATisEmpty(l); l=ATgetNext(l))
  {
    m = (ATermList) toInner(ATAgetArgument(ATAgetFirst(l),2),true);
    if ( (n = (ATermList) ATtableGet(tmp_eqns,ATgetFirst(m))) == NULL )
    {
      n = ATempty;
    }
    n = ATinsert(n,
          (ATerm) ATmakeList4(
                    ATgetArgument(ATAgetFirst(l),0),
                    toInner(ATAgetArgument(ATAgetFirst(l),1),true),
                    (ATerm) ATgetNext(m),
                    toInner(ATAgetArgument(ATAgetFirst(l),3),true)));
    ATtablePut(tmp_eqns,ATgetFirst(m),(ATerm) n);
  }

  int2term = (ATermAppl *) malloc(num_opids*sizeof(ATermAppl));
//  memset(int2term,0,num_opids*sizeof(ATermAppl));
  ATprotectArray((ATerm *) int2term,num_opids);
  innerc_eqns = (ATermList *) malloc(num_opids*sizeof(ATermList));
//  memset(innerc_eqns,0,num_opids*sizeof(ATermAppl));
  ATprotectArray((ATerm *) innerc_eqns,num_opids);

  l = ATtableKeys(term2int);
//  ATfprintf(stderr,"TERM2INT %t\n",l);
  for (; !ATisEmpty(l); l=ATgetNext(l))
  {
    i = (ATermInt) ATtableGet(term2int,ATgetFirst(l));
    int2term[ATgetInt(i)] = ATAgetFirst(l);
    m = (ATermList) ATtableGet(tmp_eqns,(ATerm) i);
    if ( m==NULL )
    {
    // ATfprintf(stderr,"TERMS %t %t NULL\n",ATgetFirst(l),i);
      innerc_eqns[ATgetInt(i)] = ATempty;
    } else {
    // ATfprintf(stderr,"TERMS %t %t %t\n",ATgetFirst(l),i,m);
      innerc_eqns[ATgetInt(i)] = ATreverse(m);
    }
  }

  ATtableDestroy(tmp_eqns);

  s = (char *) malloc(20);
  sprintf(s,"innerc_%i",getpid());
  t = (char *) malloc(100+strlen(INNERC_LDFLAGS)+strlen(INNERC_CFLAGS)+strlen(INNERC_CPPFLAGS));
  sprintf(t,"%s.c",s);
  f = fopen(t,"w");

  //
  //  Print includes
  //
  fprintf(f,  "#include <stdlib.h>\n"
      "#include <string.h>\n"
      "#include \"aterm2.h\"\n"
      "#include \"assert.h\"\n"
      "#include \"gsfunc.h\"\n"
      "#include \"gslowlevel.h\"\n"
      "#include \"gssubstitute.h\"\n"
      "\n"
      "ATermAppl rewrite(ATermAppl);\n"
      "\n"
         );

  //
  // Forward declarations of rewr_* functions
  //
  int max_arity = 0;
  for (j=0;j<num_opids;j++)
  {
    /* if ( innerc_eqns[j] != NULL )
    { */
      int arity = getArity(int2term[j]);
      if ( arity > max_arity )
      {
        max_arity = arity;
      }

  /* Declare the function that gets function j in normal form */
  fprintf(f,  "static ATermAppl rewr_%i_nnf(ATermAppl);\n",j);

      for (int a=0; a<=arity; a++)
      {
    /*  
        l = innerc_eqns[j];
        for (; !ATisEmpty(l); l=ATgetNext(l))
        {
          if ( a == ATgetLength(ATLelementAt(ATLgetFirst(l),2)) )
          {
            b = true;
            break;
          }
        }
        if ( b ) */
        {
  fprintf(f,  "static ATermAppl rewr_%i_%i(",j,a);
          for (int i=0; i<a; i++)
          {
  fprintf(f,  (i==0)?"ATermAppl arg%i":", ATermAppl arg%i",i);
          }
  fprintf(f,  ");\n");

        }
      }
    /* } */
  }
  fprintf(f,  "\n\n");

  //
  // Print defs
  //
  fprintf(f,  
      "#define ATisInt(x) (ATgetType(x) == AT_INT)\n"
      "#define isAppl(x) (ATgetAFun(x) != varAFun)\n"
      "\n"
         );
  for (int i=0; i<num_opids; i++)
  {
  fprintf(f,  "static ATerm int2ATerm%i;\n",i);
  fprintf(f,  "static ATermAppl rewrAppl%i;\n",i);
  }
  fprintf(f,  "\n"
      "static AFun *apples;\n"
      "static AFun varAFun;\n"
      "static AFun dataapplAFun;\n"
      "static AFun opidAFun;\n"
         );
  for (int i=0; i<=max_arity; i++)
  {
  fprintf(f,      "static AFun appl%i;\n",i);
  fprintf(f,  "typedef ATermAppl (*ftype%i)(",i);
    for (int j=0; j<i; j++)
    {
      if ( j == 0 )
      {
  fprintf(f,              "ATermAppl");
      } else {
  fprintf(f,              ",ATermAppl");
      }
    }
  fprintf(f,              ");\n");
  fprintf(f,  "\n"
              "ftype%i *int2func%i;\n",i,i);
  }
  fprintf(f,  "ftype1 *int2func;\n");

  //
  // Implement auxiliary functions
  //
  fprintf(f,  "static int num_apples = 0;\n"
      "#define GET_APPL_INC 5\n"
      "static AFun getAppl(int arity)\n"
      "{\n"
      "  int old_num = num_apples;\n"
      "\n"
      "  if ( apples == NULL )\n"
      "  {\n"
      "    apples = (AFun *) malloc(0*sizeof(AFun));\n"
      "  }\n"
      "\n"
      "  while ( arity >= num_apples )\n"
      "  {\n"
      "    num_apples += GET_APPL_INC;\n"
      "  }\n"
      "  if ( old_num < num_apples )\n"
      "  {\n"
      "    apples = realloc(apples,num_apples*sizeof(AFun));\n"
      "    for (int i=old_num; i<num_apples; i++)\n"
      "    {\n"
      "      char c[10];\n"
      "      sprintf(c,\"appl#%%i\",i+1);\n"
      "      apples[i] = ATmakeAFun(c,i+1,ATfalse);\n"
      "      ATprotectAFun(apples[i]);\n"
      "    }\n"
      "  }\n"
      "\n"
      "  return apples[arity];\n"
      "}\n"
      "\n"
         );
        
  for (int i=1; i<=max_arity; i++)
  { fprintf(f, 
      "static ATermAppl varFunc%i(ATermAppl a",i);
    for (int j=0; j<i; j++)
    { fprintf(f, ", ATermAppl arg%i",j); 
    }
    fprintf(f, ")\n" 
      "{\n" );
    fprintf(f,
      "  int arity = ATgetArity(ATgetAFun(a));\n"
      "  if ( arity == 1 )\n"
      "  {\n"
      "    if ( ATisInt(ATgetArgument(a,0)) && \n"
      "       (ATgetInt((ATermInt) ATgetArgument(a,0)) < %i) && \n"
      "       (int2func%i[ATgetInt((ATermInt) ATgetArgument(a,0))] != NULL) )\n"
      "    {\n"
      "       return int2func%i[ATgetInt((ATermInt) ATgetArgument(a,0))](",
         num_opids,i,i);
    for (int j=0; j<i; j++)
    {
      if ( j == 0 )
      { fprintf(f,"(ATermAppl) arg%i",j);
      } 
      else 
      { fprintf(f, ", (ATermAppl) arg%i",j);
      }
    }
    fprintf(f,");\n"
      "    }\n"
      "    else\n"
      "    {\n"
      "      return ATmakeAppl(appl%i,ATgetArgument(a,0)", i);


    for (int j=0; j<i; j++)
    { fprintf(f, ",arg%i",j);
    }
    fprintf(f, ");\n"
      "    }\n"
      "  } else {\n"
      "    ATerm args[arity+%i];\n"
      "\n"
      "    for (int i=0; i<arity; i++)\n"
      "    {\n"
      "      args[i] = ATgetArgument(a,i);\n"
      "    }\n",i);
    for (int j=0; j<i; j++)
    {        
      fprintf(f,
      "    args[arity+%i] = (ATerm) arg%i;\n",j,j);
    }
    fprintf(f,
      "    if ( ATisInt(args[0]) && (ATgetInt((ATermInt) args[0]) < %i) )\n"
      "    {\n"
//                      "  ATprintf(\"switch %%i\\n\",i);\n"
      "      switch ( arity+%i-1 )\n"
      "      {\n",num_opids,i);
    for (int j=i; j<=max_arity; j++)
    { fprintf(f,
      "        case %i:\n"
      "          if ( int2func%i[ATgetInt((ATermInt) args[0])] != NULL )\n"
      "          {\n"
      "            return int2func%i[ATgetInt((ATermInt) args[0])](",j,j,j);
      for (int k=0; k<j; k++)
      {
        if ( k == 0 )
        {  
          fprintf(f,"(ATermAppl) args[%i]",k+1);
        }   
        else 
        {        
          fprintf(f,", (ATermAppl) args[%i]",k+1);
        } 
      }
      fprintf(f,");\n"
        "          }\n"
        "          break;\n");
    }
    fprintf(f,
      "        default:\n"
      "          break;\n"
      "      }\n"
      "    }\n"
      "\n"
      "    return ATmakeApplArray(getAppl(arity+%i-1),args);\n"
      "  }\n"
      "}\n"
      "\n",i);
  }

  //
  // Implement the equations of every operation.
  //
  for (j=0;j<num_opids;j++)
  {
    /* if ( innerc_eqns[j] != NULL ) */

    int arity = getArity(int2term[j]);

    ATfprintf(f,  "// %t\n",int2term[j]);
    fprintf(f,  "static ATermAppl rewr_%i_nnf(ATermAppl t)\n"
                "{\n",j);
    if (arity>0)
    { fprintf(f,  "  int arity=ATgetArity(ATgetAFun(t))-1;\n");
    }
    for (int a=arity; a>=0; a--)
    { 
      if (a>0) 
      { fprintf(f,  "  if (arity==%i)\n"
                  "  { ",a);
      }
      else
      { fprintf(f,  "  ");
      }
      fprintf(f,  "return rewr_%i_%i(",j,a);
      for(int i=0; i<a; i++)
      { if (i>0) 
        { fprintf(f,",");
        }
        fprintf(f,  "rewrite((ATermAppl)ATgetArgument(t,%i))",i+1);
      }
      fprintf(f,  ");\n");
      if (a>0) 
      { fprintf(f,  "  }\n");
      }
    }
    fprintf(f,  "}\n"
                "\n");
            
    for (int a=0; a<=arity; a++)
    {
      fprintf(f,  "static ATermAppl rewr_%i_%i(",j,a);
      for (int i=0; i<a; i++)
      {
        fprintf(f,  (i==0)?"ATermAppl arg%i":", ATermAppl arg%i",i);
      }
      fprintf(f,  ")\n"
                  "{\n"
             );

// Implement every equation of the current operation
      ATfprintf(stderr,"LIST %t\n",innerc_eqns[j]);
      int b=false;
      
      for (l=innerc_eqns[j]; (!ATisEmpty(l) && !b); l=ATgetNext(l))
      {
        if ( ATgetLength(ATLelementAt(ATLgetFirst(l),2)) <=a )
        { b=true;
        }
      }

// If b holds, we must generate a function that performs rewriting
// In any case, if no rule is applicable (esp when !b), at the end
// of this rewrite function, function symbol j is applied to the 
// arguments.

      if (b)
      { for (int a2 = 0; a2 <= a; a2++ )
        {
          l = innerc_eqns[j];
          int z = 0;
          for (; !ATisEmpty(l); l=ATgetNext(l))
          {
            z++;
              if ( ATgetLength(ATLelementAt(ATLgetFirst(l),2)) == a2 )
              {
      ATfprintf(f,  "  // %t\n",ATLgetFirst(l));
//      ATfprintf(f,  "  printf(\"rewr_%i_%i: %i\\n\");\n",j,a,z);
                //
                // Declare equation variables
                //
                m = ATLelementAt(ATLgetFirst(l),0);
  fprintf(f,  "  {\n");
                for (; !ATisEmpty(m); m=ATgetNext(m))
                {
                  if ( ATindexOf(ATgetNext(m),ATgetFirst(m),0) < 0 ) // XXX to avoid doubles in list
  fprintf(f,  "    ATermAppl var_%s_%x;\n",ATgetName(ATgetAFun(ATAgetArgument(ATAgetFirst(m),0))),ATXgetArgument(ATAgetFirst(m),1));
                }
  fprintf(f,  "\n");
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
                  checkArg(f,ATgetFirst(m),&n,&d,&ls,"arg",k);
                  k++;
                }
            
                //
                // Test condition of equation
                //
//      ATfprintf(f,  "  printf(\"rewr_%i_%i: %i match\\n\");\n",j,a,z);
                if ( ATisList(ATelementAt(ATLgetFirst(l),1)) || ATisInt(ATelementAt(ATLgetFirst(l),1)) || !gsIsNil(ATAelementAt(ATLgetFirst(l),1)) )
                {
  fprintf(f,  "    %sif ( ATisEqual(",whitespace(d*2));
                  calcTerm(f,ATelementAt(ATLgetFirst(l),1),0);
  fprintf(f,            ",rewrAppl%i) )\n"
      "    %s{\n",
      true_num,whitespace(d*2)
    );
                  d++;
                }

//      ATfprintf(f,  "  printf(\"rewr_%i_%i: %i match+cond\\n\");\n",j,a,z);
                //
                // Create result
                //
  fprintf(f,  "    %sreturn ",whitespace(d*2));
                calcTerm(f,add_args(ATelementAt(ATLgetFirst(l),3),a-a2),get_startarg(ATelementAt(ATLgetFirst(l),3)));
  fprintf(f,          ";\n");

                //
                // Close all brackets
                //
                for (; d>0; d--)
                {
  fprintf(f,  "    %s}\n",whitespace((d-1)*2));
                }
  fprintf(f,  "  }\n");
              }
            }
          }
       }
          //
          // Finish up function
          //
          if ( a == 0 )
          {
  fprintf(f,  "  return (rewrAppl%i",
      j
         );
          } else {
            if ( a > 5 )
            {
  fprintf(f,  "  return ATmakeAppl(appl%i,(ATerm) int2ATerm%i",
      a,j
         );
            } else {
  fprintf(f,  "  return ATmakeAppl%i(appl%i,(ATerm) int2ATerm%i",
      a+1,a,j
         );
            }
          }
          for (int i=0; i<a; i++)
          {
  fprintf(f,                 ",(ATerm) arg%i",i);
          }
  fprintf(f,                 ");\n"
      "}\n"
         );
        }
  fprintf(f,  "\n");
  }

  fprintf(f,  "void rewrite_init()\n"
      "{\n"
      "  varAFun = ATmakeAFun(\"DataVarId\", 2, ATfalse);\n"
      "  ATprotectAFun(varAFun);\n"
      "  dataapplAFun = ATmakeAFun(\"DataAppl\", 2, ATfalse);\n"
      "  ATprotectAFun(dataapplAFun);\n"
      "  opidAFun = ATmakeAFun(\"OpId\", 2, ATfalse);\n"
      "  ATprotectAFun(opidAFun);\n"
      "\n"
      "  apples = NULL;\n"
      "  getAppl(%i);\n",
      max_arity
         );
  for (int i=0; i<=max_arity; i++)
  {
  fprintf(f,  "  appl%i = apples[%i];\n",i,i);
  }
  fprintf(f,  "\n");
  for (int i=0; i<num_opids; i++)
  {
  fprintf(f,  "  int2ATerm%i = (ATerm) ATmakeInt(%i);\n",i,i);
  fprintf(f,  "  ATprotect(&int2ATerm%i);\n",i);
  fprintf(f,  "  rewrAppl%i = ATmakeAppl(appl0,int2ATerm%i);\n",i,i);
  fprintf(f,  "  ATprotectAppl(&rewrAppl%i);\n",i);
  }
/*      "  int2ATerm = (ATerm *) malloc(%i*sizeof(ATerm));\n"
//      "  memset(int2ATerm,0,%i*sizeof(ATerm));\n"
      "  ATprotectArray(int2ATerm,%i);\n"
      "  for (i=0; i<%i; i++)\n"
      "  {\n"
      "    int2ATerm[i] = (ATerm) ATmakeInt(i);\n"
      "  }\n"
      "\n",
      num_opids,num_opids,num_opids//,num_opids
         );*/

  /* put the functions that start the rewriting in the array int2func */
  fprintf(f,  "\n");
  fprintf(f,  "\n");
  fprintf(f,  "  int2func = (ftype1 *) malloc(%i*sizeof(ftype1));\n",num_opids);
  for (j=0;j<num_opids;j++)
  { ATfprintf(f,  "  int2func[%i] = rewr_%i_nnf; // %t\n",j,j,int2term[j]);
  }
  fprintf(f,  "}\n"
      "\n"
      "ATermAppl rewrite(ATermAppl t)\n"
      "{\n"
//      "ATfprintf(stderr,\"REWRITE: %%t\\n\",t);"
      "  if ( gsIsDataVarId(t) )\n"
      "  { // the ATerm t is a variable. Substitute\n"
      "    ATermAppl r=RWapplySubstitution(t);\n"
      "    assert(r!=NULL);\n"
      "    return r;\n"
      "  }  \n"
      "  // The ATerm t must be rewritten \n"
      "  assert(isAppl(t));\n"
      "  assert(ATisInt(ATgetArgument(t,0)));\n"
      "  long function_index = ATgetInt((ATermInt)ATgetArgument(t,0));\n"
//      "  ATfprintf(stderr,\"FUNCTION %%i %%t\\n\",function_index,t); \n"
      "  assert( function_index < %i);\n"
      "  return int2func[function_index](t);\n"
      "}\n",
      num_opids
         );

  fclose(f);
  fprintf(stderr,"Compiling rewriter...");fflush(stderr);
#ifdef __WXMAC__
  sprintf(t,"gcc -c %s %s -g %s.c",INNERC_CPPFLAGS,INNERC_CFLAGS,s);
  ATfprintf(stderr,"%s\n",t);
  system(t);
  sprintf(t,"gcc %s -bundle -undefined dynamic_lookup -g -o %s.so %s.o",INNERC_LDFLAGS,s,s);
  ATfprintf(stderr,"%s\n",t);
  system(t);
#else
  sprintf(t,"gcc -c %s %s -Wno-unused -O3 -rdynamic %s.c",INNERC_CPPFLAGS,INNERC_CFLAGS,s);
  system(t);
  sprintf(t,"gcc %s -Wno-unused -shared -o %s.so %s.o",INNERC_LDFLAGS,s,s);
  system(t);
#endif
  fprintf(stderr,"done.\n");

  sprintf(t,"./%s.so",s);
  if ( (h = dlopen(t,RTLD_NOW)) == NULL )
  {
    fprintf(stderr,"error opening dll\n%s\n",dlerror());
  }
  so_rewr_init = (void (*)()) dlsym(h,"rewrite_init");
  so_rewr = (ATermAppl (*)(ATermAppl)) dlsym(h,"rewrite");
  so_rewr_init();

  free(t);
  free(s);
}

void RWrewrite_init_innerc(void)
{ CompileRewriteSystem();
}

void RWrewrite_add_innerc(ATermAppl eqn)
{
  assert(0); // Does not work.
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

void RWrewrite_remove_innerc(ATermAppl eqn)
{
  assert(0); // Does not work.
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

ATermList RWrewritelist_innerc(ATermList l)
{
  if (l==ATempty)
  { return ATempty;
  }

  return ATinsertA(
           RWrewritelist_innerc(ATgetNext(l)),
           so_rewr(ATAgetFirst(l)));
}

ATermAppl RWrewrite_innerc(ATermAppl Term)
{
  ATfprintf(stderr,"IN: %t\n",Term);
  ATermAppl r=so_rewr(Term);
  ATfprintf(stderr,"OUT: %t\n\n",r);
  return r;
}

