/*Id: main.c,v 1.2 2004/11/23 12:36:17 uid523 Exp $ */

/* TODO:
 * Verwerk vrije procesvariabelen op correcte wijze.
 * Apply sum elimination.
 * Apply rewriting.
 * Put renaming, hiding, encapsulation, and visibility
 * operators as far inside the parallel operators as
 * is possible.
 *
 * The program should be extensively tested */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#define MAIN
#include "linearise.h"
#include "libgsparse.h"
#include "gslowlevel.h"
#include "gstypecheck.h"
#include "gsdataimpl.h"
#include "libgsrewrite.h"

#define STRINGLENGTH 256

static int to_toolbusfile=0;
static int to_stdout=0;
static int regular=0;
static int regular2=0;
static int cluster=0;
static int nocluster=0;
static int binary=0;
static int oldstate=1;
static int statenames=0;
static int mayrewrite=1;
static int allowFreeDataVariablesInProcesses=1;

FILE *outfile;
FILE *toolbusfile;
FILE *infile;

#define P(msg)  fprintf(stderr,"%s\n",msg)

void usage(void)
{
  ATerror("Use mcrl -help for options\n");
}

void version(void)
{
  fprintf(stderr,"version: Timed mCRL parser and LPO generator. Version %s\n", VERSION);
}

void help(void)
{
P("");
P("Timed mCRL parser and (untimed) LPO generator");
P("=============================================");
P("");
P("Usage: mcrl [options] [file]");
P("");
P("The following options can be used:");
P("-stack:    an LPO of the input file in toolbus term format file is ");
P("           translated using stack datatypes. Result in written to file.lpo.");
P("-stdout:   an LPO in toolbus term format is generated, and written");
P("           to stdout");
P("-regular:  it is assumed that the input file is regular, and the");
P("           output LPO will be generated in regular form");
P("-regular2: a variant of regular where much more datavariables are");
P("           being used. Regular2 is sometimes successful where");
P("           the use of -regular leads to non termination of this tool");
P("-cluster:  all actions in the output are clustered ");
P("-nocluster:no actions are clustered, not even in intermediate LPOs");
P("-binary:   use binary, instead of n-ary, case functions when clustering.");
P("           In the presence of -newstate, state variables use binary encoding.");
P("-multi     Write the term before the final composition of LPOs");
P("-newstate: mcrl will encode state variables using enumerated types.");
P("           -newstate is only allowed in the presence of -regular or -regular2.");
P("           Using the flag -binary in addition will lead mcrl to encode");
P("           the state by a vector of boolean variables.");
P("           By default (i.e. without -newstate), the functions");
P("           one, x2p1 and x2p0 will be used.");
P("-statenames: mcrl will use meaningful names for the state variables,");
P("           derived from the specification.");
P("-no-rewrite: do not use a rewriter while linearising. If the rewrite system");
P("           does not terminate, the lineariser will only terminate, if the");
P("           rewrite system is not used");
P("-nofreevars: the lineariser will not introduce free data variables in");
P("           processes, but instead use arbitrary constants.");
P("-help:     yields this message");
P("-version:  get a version of the lineariser");
P("");
P("Except with the options help and version, a filename containing");
P("a timed mCRL description must be given. This program checks the syntax");
P("and the static semantics of a timed mCRL specification, and with ");
P("proper flags can transform a subclass of untimed mCRL specifications");
P("to linear process operators (LPOs)");
}

static ATbool ExtensionAdded(char *filename, char *suffix) {
     char *lastdot = strrchr(filename,'.');
     if (!lastdot || strcmp(lastdot, suffix)) {
          strcat(filename, suffix);
          return ATtrue;
          }
     return ATfalse;
     }


/* This file describes a C implementation of the abstract datatype
   describing abstract data types with both functions and constructors
   in Dams and Groote, A specification of an instantiator, for use in
   the context with the Toolbus.
   
   For every function described in Dams and Groote, a function of type
   `ATerm ' is generated. Type `ATerm ' is the type of terms that can be
   handled by the toolbus, and is described in Klint, A Guide to
   Toolbus Programming.  We use in particular functions ATmake,
   ATmatch, TBread, TBwrite, ATfprintf, TBsprintf 
 */
   
/* PREAMBLE */

typedef struct specificationbasictype {
            ATermList sorts;     /* storage place for sorts */
            ATermList funcs;     /* storage place for functions */
            ATermList maps;      /* storage place for constructors */
            ATermList eqns;      /* storage place for equations */
            ATermList acts;      /* storage place for actions */
            ATermList procdatavars; /* storage place for free variables occurring
                                      in processes ranging over data */
            ATermList procs;     /* storage place for processes,
                                         uses alt, seq, par, lmer, cond,sum,
                                         com, bound, at, name, delta,
                                         tau, hide, rename, encap */
            ATermList initdatavars; /* storage place for free variables in
                                       init clause */
            ATermAppl init;      /* storage place for initial process */
} specificationbasictype;


typedef struct string {
  char s[STRINGLENGTH];
  struct string *next; } string;

static int canterminatebody(
              ATermAppl t,
              int *stable,
              ATermIndexedSet visited,
              int allowrecursion);
static ATermAppl storeinit(ATermAppl init);
static void storeprocs(ATermList procs);
static ATermList getsorts(ATermList l);
static ATermList sortActionLabels(ATermList actionlabels);
static ATermAppl dummyterm(ATermAppl sort,specificationbasictype *spec);
static int occursintermlist(ATermAppl var, ATermList l);
static int occursinpCRLterm(ATermAppl var, ATermAppl p, int strict);
static ATermAppl getfreshvariable(char *s,ATermAppl sort);
static ATermList construct_renaming(ATermList pars1, ATermList pars2,
                ATermList *pars3, ATermList *pars4);
static void alphaconversion(ATermAppl procId, ATermList parameters);
static ATermAppl fresh_name(char *name);
static void declare_equation_variables(ATermList t1);
static void end_equation_section(void);
static void newequation(
                ATermAppl condition,
                ATermAppl t2, 
                ATermAppl t3, 
                specificationbasictype *spec);


static ATermList ATinsertA(ATermList l, ATermAppl a)
{ return ATinsert(l,(ATerm)a);
}

static char *ATSgetArgument(ATermAppl appl, int nr)
{ return gsATermAppl2String(ATAgetArgument(appl, nr));
}

extern FILE *outfile;
int time_operators_used=0;
static ATermList seq_varnames=NULL;

char scratch1[STRINGLENGTH];

/* Below we provide the basic functions to store and retrieve
   information about the signature of a datatype */

ATermIndexedSet objectIndexTable=NULL;
ATermIndexedSet stringTable=NULL;

typedef struct enumeratedtype {
  int size;
  ATermAppl sortId;
  ATermList elementnames; 
  ATermList functions;
  struct enumeratedtype *next;
} enumeratedtype;

static enumeratedtype *enumeratedtypelist=NULL;

static enumeratedtype *create_enumeratedtype
                (int n,specificationbasictype *spec);

typedef enum { none, 
               map, 
               func, 
               act, 
               proc, 
               variable, 
               sort, 
               multiact } objecttype;
typedef enum { unknown, 
               mCRL, 
               mCRLdone, 
               mCRLbusy, 
               mCRLlin, 
               pCRL, 
               multiAction,
               GNF,    
               GNFalpha, 
               GNFbusy,
               error } processstatustype;

typedef struct objectdatatype {
  ATermAppl objectname;
  int constructor;
  ATerm representedprocesses; /* for regular this is a single process, and
                                 for regular2 this is a list of processes */
  ATerm representedprocess;
  ATermAppl targetsort; /* for actions target sort is used to
                           indicate the process representing this action. */
  ATermAppl processbody;
  ATermList parameters;
  processstatustype processstatus;
  objecttype object; 
  int canterminate; } objectdatatype;
  
static objectdatatype *objectdata=NULL;
static int  maxobject=0;

static void newobject(int n)
{ 
  assert(n>=0);

  if (n>=maxobject)
  { int newsize=(n>=2*maxobject?
                  (n<1024?1024:(n+1)):2*maxobject);
    objectdata=(maxobject==0?
                 malloc(newsize*sizeof(objectdatatype)):
                 realloc(objectdata,newsize*sizeof(objectdatatype)));
    if (objectdata==NULL)
    { ATerror("Fail to resize objectdata to %d\n",2*maxobject); }
    
    for(long i=maxobject ; i<newsize ; i++)
    { objectdata[i].objectname=NULL;
      ATprotect((ATerm *)&(objectdata[i].objectname));
      objectdata[i].constructor=0;
      objectdata[i].representedprocess=NULL;
      ATprotect(&(objectdata[i].representedprocess));
      objectdata[i].representedprocesses=NULL;
      objectdata[i].targetsort=0;
      objectdata[i].processbody=NULL;
      ATprotect((ATerm *)&(objectdata[i].processbody));
      objectdata[i].parameters=NULL;
      ATprotect((ATerm*)&(objectdata[i].parameters));
      objectdata[i].processstatus=unknown;
      objectdata[i].object=none;
      objectdata[i].canterminate=0; 
    }
    maxobject=newsize;
  }
  objectdata[n].objectname=NULL;
  objectdata[n].constructor=-1;
/*  args;
    representedprocesses; 
    representedprocess; */
  objectdata[n].targetsort=0; /* for actions target sort is used to
                                    indicate the process representing 
                                    this action. */
  objectdata[n].processbody=NULL;
  objectdata[n].parameters=NULL;
  objectdata[n].processstatus=-1;
  objectdata[n].object=none;
  objectdata[n].canterminate=0;

}

static int strequal(char *s1,char *s2)
{ 
  return (strcmp(s1,s2)==0);
}


static int variablesequal(ATermList t1, ATermList t2, /* XXXXXXXXXXXXXXX */
                ATermList *renamingvariablelist,
                ATermList *renamingtermlist)
{ /* return 1 iff t1 equals t2, modulo a renaming; */
  ATermAppl var1=NULL, var2=NULL;

  *renamingvariablelist=ATempty;
  *renamingtermlist=ATempty;

  for(; t1!=ATempty ; t1=ATgetNext(t1) )
  { if (t2==ATempty) 
    { return 0;
    }
    var1=ATAgetFirst(t1);
    var2=ATAgetFirst(t2);
    if (ATgetArgument(var1,1)!=ATgetArgument(var2,1))
    { return 0;
    }
    if (var1!=var2) 
    { *renamingvariablelist=ATinsertA(*renamingvariablelist,var1);        
      *renamingtermlist=ATinsertA(*renamingtermlist,var2);
    }
  }
  if (t2==ATempty) 
  { return 1;
  }          
  return 0; 
}

static string *emptystringlist =NULL;

/* static void release_string(string *c)
{
  
  c->next = emptystringlist;
  emptystringlist = c;
  strncpy(&c->s[0],"",STRINGLENGTH);
} */

static string *new_string(char *s)
{
  string *c;
  
  if (emptystringlist == NULL) {
    { c = (string *)malloc(sizeof(string));
      if (c==NULL) return NULL;}
  } else {
    c = emptystringlist;
    emptystringlist = emptystringlist->next;
  }
  c->next=NULL;
  strncpy(&c->s[0],s,STRINGLENGTH);
  return c;
}

/*****************  store and retrieve basic objects  ******************/
  

static long addObject(ATermAppl o, ATbool *isnew)
{ return ATindexedSetPut(objectIndexTable,(ATerm)o,isnew);
}

static long objectIndex(ATermAppl o)
{ long result=ATindexedSetGetIndex(objectIndexTable,(ATerm)o);
  assert(result>=0); /* object index must always return the index
                        of an existing object, because at the
                        places where objectIndex is used, no
                        checks take place */
  return result;
}

static long existsObjectIndex(ATermAppl o)
{ /* returns negative number if object does not exists,
     otherwise a number >=0, indicating the index of the 
     object */
  return ATindexedSetGetIndex(objectIndexTable,(ATerm)o);
}

static void removeObject(ATermAppl o)
{ ATindexedSetRemove(objectIndexTable,(ATerm)o);
}

static void addString(char *str)
{ ATindexedSetPut(stringTable,ATmake("<str>",str),NULL);
}

static ATbool existsString(char *str)
{ return ATindexedSetGetIndex(stringTable,ATmake("<str>",str))>=0;
}

static ATermAppl getTargetSort(ATermAppl sortterm)
{
  if ((sortterm==gsMakeSortExprBool())||
      (sortterm==gsMakeSortExprInt())||
      (sortterm==gsMakeSortExprNat())||
      (sortterm==gsMakeSortExprPos())||
      (gsIsSortId(sortterm)))
  { return sortterm;
  }
  if (gsIsSortArrow(sortterm))
  { return getTargetSort(ATAgetArgument(sortterm,1));
  }

  gsErrorMsg("Internal: Expect a sortterm %t",sortterm);
  return NULL;
}

static ATermList linGetSorts(ATermList l)
{ if (l==ATempty) return ATempty;

  return ATinsertA(linGetSorts(ATgetNext(l)),
                   gsGetSort(ATAgetFirst(l)));
}

static int existsort(ATermAppl sortterm)
/* Delivers 0 if sort does not exists. Otherwise 1 
   indicating that the sort exists */
{ 
  /* if (sortterm==gsMakeSortExprBool()) return 1;
     if (sortterm==gsMakeSortExprInt()) return 1;
     if (sortterm==gsMakeSortExprNat()) return 1;
     if (sortterm==gsMakeSortExprPos()) return 1; */
  if (gsIsSortArrow(sortterm))
  { return existsort(ATAgetArgument(sortterm,0)) && 
             existsort(ATAgetArgument(sortterm,1));
  }
  if (gsIsSortId(sortterm)) 
  { 
    long n=0;

    n=existsObjectIndex(sortterm);
    if (n<0) 
    { 
      return 0;
    }
    if (objectdata[n].object==sort) return 1;
    return 0;
  }
  assert(0);
  ATerror("Internal: Expected a sortterm (1) %t\n",sortterm);
  return 0;
}

static void insertsort(ATermAppl sortterm)
{ 
  /* if (sortterm==gsMakeSortExprBool()) return;
     if (sortterm==gsMakeSortExprInt()) return;
     if (sortterm==gsMakeSortExprNat()) return;
     if (sortterm==gsMakeSortExprPos()) return; */
  if (gsIsSortArrow(sortterm))
  { insertsort(ATAgetArgument(sortterm,0));
    insertsort(ATAgetArgument(sortterm,1));
    return;
  }
  /* if (gsIsSortList(sortterm))
  { ATerror("SortList is not an implemented sort\n");
  }
  if (gsIsSortSet(sortterm))
  { ATerror("SortSet is not an implemented sort\n");
  }
  if (gsIsSortBag(sortterm))
  { ATerror("SortBag is not an implemented sort\n");
  } */
  if (gsIsSortId(sortterm)) 

  {
    long n=0;
    ATbool isnew=0;
    char *str=ATSgetArgument(sortterm,0);
    addString(str);
 
    n=addObject(sortterm,&isnew);
 
    if (isnew==0)
    { ATerror("Sort %t is added twice\n",sortterm); }
 
    newobject(n);

    objectdata[n].objectname=sortterm;
    objectdata[n].object=sort;
    objectdata[n].constructor=0;
    return;
  }
  ATerror("Internal: Expected a sortterm (2)  %t\n",sortterm);
}

static long insertConstructorOrFunction(ATermAppl constructor,objecttype type)
{ ATbool isnew=0;
  ATermAppl t=NULL;
  char *str=NULL;
  long m=0;
  long n=0;

  if (!gsIsOpId(constructor)) 
  { ATerror("Internal: Expect operation declaration %t\n",constructor);
  };

  str=ATSgetArgument(constructor,0);
  t=ATAgetArgument(constructor,1);
  assert(existsort(t));

  addString(str);
 
  n=addObject(constructor,&isnew);

  if (isnew==0)
  { ATerror("Function %t is added twice\n",constructor); }

  newobject(n);

  objectdata[n].objectname=constructor;
  objectdata[n].targetsort=getTargetSort(t);
  m=objectIndex(objectdata[n].targetsort);
  assert(objectdata[m].object==sort);
  if (type==func) objectdata[m].constructor=1;
  objectdata[n].object=type;
  return n;
}

static long insertconstructor(
               ATermAppl constructor, 
               specificationbasictype *spec)
{ spec->funcs=ATinsertA(spec->funcs,constructor);
  return insertConstructorOrFunction(constructor,func); 
}

static long insertmapping(
               ATermAppl mapping,
               specificationbasictype *spec)
{ spec->maps=ATinsertA(spec->maps,mapping);
  return insertConstructorOrFunction(mapping,map); 
}

static ATermList getnames(ATermAppl multiAction)
{ ATermList result=ATempty;
  for(ATermList l=ATLgetArgument(multiAction,0) ;
                l!=ATempty ;
                l=ATgetNext(l))
  { result=ATinsertA(result,ATAgetArgument(ATAgetFirst(l),0));

  }
  return ATreverse(result);
}

static ATermList getparameters(ATermAppl multiAction)
{ ATermList result=ATempty;

  assert(gsIsMultAct(multiAction));
  for(ATermList l=ATLgetArgument(multiAction,0) ;
                l!=ATempty ;
                l=ATgetNext(l))
  { for(ATermList l1=ATLgetArgument(ATAgetArgument(ATAgetFirst(l),0),1) ;
                  l1!=ATempty ;
                  l1=ATgetNext(l1))
    { result=ATinsertA(
               result,
               getfreshvariable("a",ATAgetFirst(l1)));
    }
  }
  return ATreverse(result);
}

static ATermList getarguments(ATermAppl multiAction)
{ ATermList result=ATempty;

  for(ATermList l=ATLgetArgument(multiAction,0) ;
                l!=ATempty ;
                l=ATgetNext(l))
  { for(ATermList l1=ATLgetArgument(ATAgetFirst(l),1) ;
                  l1!=ATempty ;
                  l1=ATgetNext(l1))
    { result=ATinsertA(result,ATAgetFirst(l1));
    }
  }
  return ATreverse(result);
}

static ATermAppl makemultiaction(ATermList actionIds,ATermList args)
{ 
  ATermList result=ATempty;
  for(ATermList l=actionIds ; l!=ATempty ; l=ATgetNext(l))
  { ATermAppl actionId=ATAgetFirst(l);
    long arity=ATgetLength(ATLgetArgument(actionId,1));
    result=ATinsertA(result,
               gsMakeAction(actionId,
                            ATgetSlice(args,0,arity)));
    args=ATgetTail(args,arity);
  }

  assert(args==ATempty);
  return gsMakeMultAct(ATreverse(result));
}

static long addMultiAction(ATermAppl multiAction, ATbool *isnew)
{
  ATermList actionnames=getnames(multiAction);
  long n=addObject((ATermAppl)actionnames,isnew);
  
  
  if (*isnew)
  { 
    newobject(n);
    objectdata[n].parameters=getparameters(multiAction);
    objectdata[n].objectname=(ATermAppl)actionnames;
    objectdata[n].object=multiact;
    objectdata[n].processbody=
                     makemultiaction(actionnames,
                                     objectdata[n].parameters);
  }
  return n;
}

/* static int existsvariable(ATermAppl var)
/ * Delivers -1 if the variable does not exists. 
   Otherwise a number>=0 is returned,
   indicating the index of the variable * /
{ long n=existsObjectIndex(var);
  if (n<0) return -1;
  if (objectdata[n].object==variable) return n;
  return -1;
} */

static void insertvariable(ATermAppl var, ATbool mustbenew)
{ 
  ATbool isnew=0;
  long n=0;
  ATermAppl t=NULL;
  char *str=NULL;

  assert(gsIsDataVarId(var));

  str=ATSgetArgument(var,0);
  t=ATAgetArgument(var,1);

  addString(str);

  n=addObject(var,&isnew);

  if ((isnew==0)&&(mustbenew))
  { ATerror("Variable %t already exists\n",var); }

  newobject(n);

  objectdata[n].objectname=var;
  objectdata[n].object=variable;

#ifndef NDEBUG
  if (existsort(ATAgetArgument(var,1))<0)
  { ATerror("Variable %t has unknown sort\n",var); }
#endif
} 

static void removevariable(ATermAppl var)
{ long n=existsObjectIndex(var);
  if (n<0) 
  { ATerror("Cannot remove a non existing variable %t\n",var); }
  if (objectdata[n].object!=variable) 
  { ATerror("Data structure is confused %t\n",var); }
  objectdata[n].object=none;
  removeObject(var);
}

static void resetvariables(ATermList vars)
{ /* remove the variables in reverse order */

  if (ATisEmpty(vars)) return;

  resetvariables(ATgetNext(vars));
  removevariable(ATAgetFirst(vars));
}

static void insertvariables(ATermList vars,ATbool mustbenew)
{ 
  for( ; !ATisEmpty(vars) ; vars=ATgetNext(vars))
  { insertvariable(ATAgetFirst(vars),mustbenew); }
}

/************ upperpowerof2 *********************************************/

static int upperpowerof2(int i)
/* function yields n for the smallest value n such that 
   2^n>=i. This constitutes the number of bits necessary
   to represent a number smaller than i. i is assumed to
   be at least 1. */
{ int n=0;
  int powerof2=1;
  for( ; powerof2< i ; n++)
  { powerof2=2*powerof2; }
  return n;
}

static ATermAppl RewriteTerm(ATermAppl t)
{ if (mayrewrite) return gsRewriteTerm(t);
  return t;
}

/************ storeact ****************************************************/

static long insertAction(ATermAppl actionId)
{ ATbool isnew=0;
  long n=addObject(actionId,&isnew);
  char *str=NULL;

  if (isnew==0)
  { ATerror("Action %t is added twice\n",actionId); }

  str=ATSgetArgument(actionId,0);
  addString(str);

  newobject(n);

  objectdata[n].objectname=actionId;

  objectdata[n].object=act;
  objectdata[n].targetsort=NULL;
#ifndef NDEBUG
  { for(ATermList l=ATLgetArgument(actionId,1); !ATisEmpty(l); 
                  l=ATgetNext(l))
    { 
      if (existsort(ATAgetFirst(l))<0)
      { ATerror("The term %t is not a sort in the declaration of actionId %t\n",
                              ATgetFirst(l),actionId); }
    }
  }
#endif
  return n;
}


static void storeact(ATermList acts)
{ 
  for( ; !ATisEmpty(acts) ; acts=ATgetNext(acts))
  { insertAction(ATAgetFirst(acts)); }
} 

/************ read_input_file  **********************************/

static specificationbasictype *read_input_file(char *filename) 
{ FILE *inputfile;
  ATermAppl t=NULL;
  specificationbasictype *spec=malloc(sizeof(specificationbasictype));
  spec->sorts=NULL;
  ATprotect((ATerm *)&(spec->sorts));
  spec->funcs=NULL;
  ATprotect((ATerm *)&(spec->funcs));
  spec->maps=NULL;
  ATprotect((ATerm *)&(spec->maps));
  spec->eqns=NULL;
  ATprotect((ATerm *)&(spec->eqns));
  spec->acts=NULL;
  ATprotect((ATerm *)&(spec->acts));
  spec->procdatavars=NULL;
  ATprotect((ATerm *)&(spec->procdatavars));
  spec->procs=NULL;
  ATprotect((ATerm *)&(spec->procs));
  spec->initdatavars=NULL;
  ATprotect((ATerm *)&(spec->initdatavars));
  spec->init=NULL;
  ATprotect((ATerm *)&(spec->init));
  
  if (spec==NULL)
  { ATerror("Cannot allocate memory for elementary operations\n"); }

  inputfile=fopen(filename,"r");
  if (inputfile==NULL)
     ATerror("Fail to open inputfile %s\n",filename);

  t=gsParseSpecification(inputfile);
  if (t==NULL)
  { ATerror("Parse error\n");
  }
  

  assert(gsIsSpecV1(t));

/* First store the sorts */
  
  spec->sorts=ATempty;
  for(ATermList sorts=ATLgetArgument(ATAgetArgument(t,0),0) ; 
      sorts!=ATempty ; 
      sorts=ATgetNext(sorts) )
  { insertsort(ATAgetFirst(sorts)); 
  }
          
  spec->funcs=ATempty;
/* Now store the constructors */
  for(ATermList constr=ATLgetArgument(ATAgetArgument(t,1),0) ; 
      !ATisEmpty(constr) ; 
      constr=ATgetNext(constr) )
  { 
    insertconstructor(ATAgetFirst(constr),spec); 
  }

  spec->maps=ATempty;
/* Finally store the functions */
  for(ATermList maps=ATLgetArgument(ATAgetArgument(t,2),0) ; 
      !ATisEmpty(maps) ; 
      maps=ATgetNext(maps) )
  { insertmapping(ATAgetFirst(maps),spec); 
  }


/* And very finally store the equations */
  if (mayrewrite) gsRewriteInit(gsMakeDataEqnSpec(ATempty),GS_REWR_INNER);
  spec->eqns=ATempty;
  for(ATermList eqns=ATLgetArgument(ATAgetArgument(t,3),0) ;
      !ATisEmpty(eqns) ;
      eqns=ATgetNext(eqns) )
  { ATermAppl eqn=ATAgetFirst(eqns);
    declare_equation_variables(ATLgetArgument(eqn,0));
    newequation(ATAgetArgument(eqn,1),ATAgetArgument(eqn,2),ATAgetArgument(eqn,3),spec);
    end_equation_section();
  }

  spec->eqns=ATLgetArgument(ATAgetArgument(t,3),0);

  spec->acts=ATLgetArgument(ATAgetArgument(t,4),0);
  storeact(spec->acts);
  spec->procdatavars=ATempty;
  spec->procs=ATLgetArgument(ATAgetArgument(t,5),0);
  storeprocs(spec->procs);
  spec->initdatavars=ATLgetArgument(ATAgetArgument(t,6),0);
  spec->init=storeinit(ATAgetArgument(ATAgetArgument(t,6),1));

  return spec;

}

/************ storeprocs *************************************************/


static long insertProcDeclaration(
                  ATermAppl procId,
                  ATermList parameters,
                  ATermAppl body,
                  processstatustype s,
                  int canterminate)
{ ATbool isnew=0;
  long n=0;
  char *str=NULL;

  if (!gsIsProcVarId(procId)) 
  { ATerror("Expect process declaration %t\n",procId);
  }

  str=ATSgetArgument(procId,0);
  addString(str);

  n=addObject(procId,&isnew);

  if (isnew==0)
  { ATerror("Process %t is added twice\n",procId); }

  newobject(n);

  objectdata[n].objectname=procId;
  objectdata[n].object=proc;
  objectdata[n].processbody=body;
  objectdata[n].canterminate=canterminate;
  objectdata[n].processstatus=s;
  objectdata[n].parameters=parameters;
  insertvariables(parameters,0);
#ifndef NDEBUG
  { for(ATermList l=ATLgetArgument(procId,1); !ATisEmpty(l); 
                  l=ATgetNext(l))
    { 
      if (existsort(ATAgetFirst(l))<0)
      { ATerror("The term %t is not a sort in the declaration of action %t\n",
                              ATgetFirst(l),procId); }
    }
  }
#endif
  return n;
}


static void storeprocs(ATermList procs)
{ 
  for( ; !ATisEmpty(procs) ; procs=ATgetNext(procs))
  { 
    ATermAppl p=ATAgetFirst(procs);
    insertProcDeclaration(
            ATAgetArgument(p,1),
            ATLgetArgument(p,2),
            ATAgetArgument(p,3),
            unknown,0); 
  }
} 

/************ storeinit *************************************************/


static ATermAppl storeinit(ATermAppl init)
{ /* init is used as the name of the initial process,
     because it cannot occur as a string in the input */

  ATermAppl initprocess=gsMakeProcVarId(
                         gsString2ATermAppl("init"),ATempty);
  insertProcDeclaration(
           initprocess,ATempty,init,unknown,0);
  return initprocess;
}

/********** basic symbols and local term constructors *****************/

static AFun initprocspec_symbol=0, tuple_symbol=0;
static ATermAppl terminationAction=NULL;
static ATermAppl terminatedProcId=NULL; 

static void initialize_symbols(void)
{ 
  tuple_symbol=ATmakeAFun("tuple",2,ATfalse);
  ATprotectAFun(tuple_symbol);
  initprocspec_symbol=ATmakeAFun("initprocspec",3,ATfalse);
  ATprotectAFun(initprocspec_symbol);
  ATprotectAppl(&terminationAction);
  terminationAction=gsMakeMultAct(
             ATinsertA(
               ATempty,
               gsMakeAction(gsMakeActId(fresh_name("Terminate"),ATempty),ATempty)));
  ATprotectAppl(&terminatedProcId);
  terminatedProcId=gsMakeProcVarId(gsString2ATermAppl("Terminated**"),ATempty);
  insertProcDeclaration(
           terminatedProcId,
           ATempty,
           gsMakeSeq(terminationAction,gsMakeDelta()),
           pCRL,0);
}

static ATermAppl linMakeTuple(
                    ATermList first, 
                    ATermAppl last)
{ return ATmakeAppl2(
             tuple_symbol,
             (ATerm)first,
             (ATerm)last);
}

static int actioncompare(ATermAppl a1, ATermAppl a2)
{ 
  /* first compare the strings in the actions */

  if (strcmp(ATSgetArgument(a1,0),ATSgetArgument(a2,0))>0)
  { return 1; }
  
  if (strcmp(ATSgetArgument(a1,0),ATSgetArgument(a2,0))==0)
  { /* the strings are equal; the sorts are used to 
       determine the ordering */

    return ATLgetArgument(a1,1)>ATLgetArgument(a2,1);
  }
  
  return 0;
  
}

static ATermList linInsertActionInMultiActionList(
                           ATermAppl action,
                           ATermList multiAction)
{ /* store the action in the multiAction, alphabetically
     sorted on the actionname in the actionId. Note that
     the empty multiAction represents tau. */
  
  ATermAppl firstAction=NULL;

  if (multiAction==ATempty)
  { return ATinsertA(ATempty,action);
  }
  firstAction=ATAgetFirst(multiAction);  

  /* Actions are compared on the basis of their position
     in memory, to order them. As the aterm library maintains
     pointers to objects that are not garbage collected, this
     is a safe way to do this. */
  if (actioncompare(ATAgetArgument(action,0),
          ATAgetArgument(firstAction,0)))
  { return ATinsertA(linInsertActionInMultiActionList(
                           action,
                           ATgetNext(multiAction)),
                     firstAction); 
  }
 
  return ATinsertA(multiAction,action);
}                

/* static ATermAppl linInsertActionInMultiAction(
                           ATermAppl action,
                           ATermAppl multiAction)
{ assert(gsIsMultAct(multiAction));
  return gsMakeMultAct(
              linInsertActionInMultiActionList(
                      action,
                      ATLgetArgument(multiAction,1)));

} */

static ATermList linMergeMultiActionList(ATermList ma1, ATermList ma2)
{
 
  for( ; ma1!=ATempty ; ma1=ATgetNext(ma1))
  { ma2=linInsertActionInMultiActionList(ATAgetFirst(ma1),ma2);
  }
  
  return ma2;
}

static ATermAppl linMergeMultiAction(ATermAppl ma1, ATermAppl ma2)
{
  assert(gsIsMultAct(ma1));
  assert(gsIsMultAct(ma2));
 
  return gsMakeMultAct(
                linMergeMultiActionList(
                      ATLgetArgument(ma1,0),
                      ATLgetArgument(ma2,0)));
}

/* static int gsIsMultActrmAppl summand)
{ return ATgetAFun(summand)==summand_symbol;
} */

static ATermList linGetSumVars(ATermAppl summand)
{ assert(gsIsLPESummand(summand));
  return ATLgetArgument(summand,0);
}

static ATermAppl linGetCondition(ATermAppl summand)
{ assert(gsIsLPESummand(summand));
  return ATAgetArgument(summand,1);
}

static ATermAppl linGetMultiAction(ATermAppl summand)
{ assert(gsIsLPESummand(summand));
  return ATAgetArgument(summand,2);
}

static ATermAppl linGetActionTime(ATermAppl summand)
{ 
  assert(gsIsLPESummand(summand));
  return ATAgetArgument(summand,3);
}

static ATermList linGetNextState(ATermAppl summand)
{ 
  assert(gsIsLPESummand(summand));
  
  return ATLgetArgument(summand,4);
}



/************** determine_process_status ********************************/

static ATermList determine_process_status_rec(
                ATermAppl procDecl,
                processstatustype status);

static processstatustype determine_process_statusterm(
                ATermAppl body, 
                processstatustype status)
/* determine whether a process is a mCRL process, with hide, rename,
   encap and parallel processes around process names.
   If a process contains, only alt, seq, names, conds and sums,
   delta and tau then the process is a pCRL.
   otherwise, i.e. if the process contains lmer, com, bound and
   at, or have parallel, encap, hide and rename operators in
   combination with other operators this system delivers unknown and
   an appropriate message is left in emsg */

{ /* In this procedure it is determined whether a process
     is of type mCRL, pCRL or a multiAction. pCRL processes
     occur strictly within mCRL processes, and multiActions
     occur strictly within pCRL processes. Processes that pass
     this procedure can be linearised. Bounded initialisation,
     the leftmerge and synchronization merge on the highest
     level are filtered out. */

  processstatustype s1=unknown, s2=unknown;

  if (gsIsChoice(body))
  { if (status==multiAction) 
    { ATerror("Choice operator occurs in a multi-action\n");
    }
    s1=determine_process_statusterm(ATAgetArgument(body,0),pCRL);
    s2=determine_process_statusterm(ATAgetArgument(body,1),pCRL);
    if ((s1==mCRL)||(s2==mCRL))
    { ATerror("mCRL operators occur within the scope of a choice operator\n");
    }
    return pCRL;
  }

  if (gsIsSeq(body))
  { if (status==multiAction) 
    { ATerror("Sequential operator occurs in a multi-action\n");
    }
    s1=determine_process_statusterm(ATAgetArgument(body,0),pCRL);
    s2=determine_process_statusterm(ATAgetArgument(body,1),pCRL);
    if ((s1==mCRL)||(s2==mCRL))
    { ATerror("mCRL operators occur within the scope of a sequential operator\n");
    }
    return pCRL;
  }

  if (gsIsMerge(body))
  { if (status!=mCRL)
    { ATerror("The parallel operator occurs in the scope of pCRL operators\n");
    }
    s1=determine_process_statusterm(ATAgetArgument(body,0),mCRL);
    s2=determine_process_statusterm(ATAgetArgument(body,1),mCRL);
    return mCRL;
  }

  if (gsIsLMerge(body))
  { ATerror("Cannot linearize because the specification contains a leftmerge\n");
  }

  if (gsIsCond(body))  
  { if (status==multiAction) 
    { ATerror("If-then(-else) occurs in a multi-action\n");
    }
    s1=determine_process_statusterm(ATAgetArgument(body,1),pCRL);
    s2=determine_process_statusterm(ATAgetArgument(body,2),pCRL);
    if ((s1==mCRL)||(s2==mCRL))
    { ATerror("mCRL operators occur in the scope of the if-then-else operator\n");
    }
    return pCRL;
  }

  if (gsIsSum(body)) 
  { /* insert the variable names of variables, to avoid
       that this variable name will be reused later on */
    insertvariables(ATLgetArgument(body,0),0);
    if (status==multiAction)
    { ATerror("Sum operator occurs within a multi-action\n");
    }
    s1=determine_process_statusterm(ATAgetArgument(body,1),pCRL);
    if (s1==mCRL)
    { ATerror("mCRL operators occur in the scope of the sum operator\n");
    }
    return pCRL;
  }

  if (gsIsComm(body))
  { if (status!=mCRL)
    { ATerror("The communication operator occurs in the scope of pCRL operators\n");
    }
    s2=determine_process_statusterm(ATAgetArgument(body,1),mCRL);
    return mCRL;
  }

  if (gsIsBInit(body))
  { ATerror("Cannot linearize a specification with the bounded initialization operator\n");
  }

  if (gsIsAtTime(body)) 
  { if (status==multiAction)
    { ATerror("A time operator occurs in a multi-action\n");
    }
    s1=determine_process_statusterm(ATAgetArgument(body,0),pCRL);
    if ((s1==mCRL)||(s2==mCRL))
    { ATerror("A mCRL operator occurs in the scope of a time operator\n");
    }
    return pCRL;
  }

  if (gsIsSync(body))
  { 
    s1=determine_process_statusterm(ATAgetArgument(body,0),pCRL);
    s2=determine_process_statusterm(ATAgetArgument(body,1),pCRL);
    if ((s1!=multiAction)||(s2!=multiAction))
    { ATerror("Other objects than multi-actions occur in the scope of a synch operator\n");
    }
    return multiAction;
  }

  if (gsIsAction(body))
  { return multiAction;
  }

  if (gsIsProcess(body))
  { determine_process_status_rec(ATAgetArgument(body,0),status);
    return status;
  }

  if (gsIsDelta(body))
  { return pCRL;
  }

  if (gsIsTau(body)) 
  { return multiAction;
  }

  if (gsIsHide(body))
  { if (status!=mCRL) 
    { ATerror("A hide operator occurs in the scope of pCRL operators\n");
    }
    s1=determine_process_statusterm(ATAgetArgument(body,1),mCRL);
    return mCRL;
  }

  if (gsIsRename(body))
  { if (status!=mCRL) 
    { ATerror("A rename operator occurs in the scope of pCRL operators\n");
    }
    s1=determine_process_statusterm(ATAgetArgument(body,1),mCRL);
    return mCRL;
  }

  if (gsIsAllow(body))
  { if (status!=mCRL) 
    { ATerror("An allow operator occurs in the scope of pCRL operators\n");
    }
    s1=determine_process_statusterm(ATAgetArgument(body,1),mCRL);
    return mCRL;
  }

  if (gsIsRestrict(body))
  { if (status!=mCRL) 
    { ATerror("A restrict operator occurs in the scope of pCRL operators\n");
    }
    s1=determine_process_statusterm(ATAgetArgument(body,1),mCRL);
    return mCRL;
  }

  ATerror("Internal error: Process has unexpected format (2) %t\n",body);
  return error;
} 

static ATermList pcrlprocesses=NULL;

static ATermList determine_process_status_rec(
                   ATermAppl procDecl,
                   processstatustype status)
{ processstatustype s;
  int n=objectIndex(procDecl);
  assert(n>=0); /* if this fails, the process does not exist */
  s=objectdata[n].processstatus;

  if (s==unknown) 
  { objectdata[n].processstatus=status;
    if (status==pCRL)
    {  if (ATindexOf(pcrlprocesses,(ATerm)procDecl,0)<0)
      { pcrlprocesses=ATinsertA(pcrlprocesses,procDecl); 
      }
      determine_process_statusterm(objectdata[n].processbody,pCRL);
      return pcrlprocesses;
    }
    /* status==mCRL */
    s=determine_process_statusterm(objectdata[n].processbody,mCRL);
    if (s!=status)
    { /* s==pCRL and status==mCRL */
      objectdata[n].processstatus=s;
      if (ATindexOf(pcrlprocesses,(ATerm)procDecl,0)<0)
      { pcrlprocesses=ATinsertA(pcrlprocesses,procDecl);
      }
      determine_process_statusterm(objectdata[n].processbody,pCRL); 
    }
  }
  if (s==mCRL)
  { if (status==pCRL)
    { objectdata[n].processstatus=pCRL;
      if (ATindexOf(pcrlprocesses,(ATerm)procDecl,0)<0)
      { pcrlprocesses=ATinsertA(pcrlprocesses,procDecl);
      }
      determine_process_statusterm(objectdata[n].processbody,pCRL); 
    }
  }
  return pcrlprocesses;
}

static ATermList determine_process_status(
               ATermAppl initprocess,
               processstatustype status)
{ pcrlprocesses=ATempty;
  return determine_process_status_rec(initprocess,status);
}

/***********  collect pcrlprocessen **********************************/

static void collectPcrlProcesses_rec(ATermAppl procDecl, ATermIndexedSet visited);

static void collectPcrlProcesses_term(ATermAppl body, ATermIndexedSet visited)

{ 
  if (gsIsCond(body))  
  { 
    collectPcrlProcesses_term(ATAgetArgument(body,1),visited);
    collectPcrlProcesses_term(ATAgetArgument(body,2),visited);
    return;
  }

  if ((gsIsChoice(body))||
      (gsIsSeq(body))||
      (gsIsMerge(body))||
      (gsIsSync(body)))
  { 
    collectPcrlProcesses_term(ATAgetArgument(body,0),visited);
    collectPcrlProcesses_term(ATAgetArgument(body,1),visited);
    return ;
  }

  if (gsIsSum(body)) 
  { 
    collectPcrlProcesses_term(ATAgetArgument(body,1),visited);
    return;
  }

  if (gsIsAtTime(body)) 
  { 
    collectPcrlProcesses_term(ATAgetArgument(body,0),visited);
    return;
  }

  if (gsIsProcess(body))
  { collectPcrlProcesses_rec(ATAgetArgument(body,0),visited);
    return;
  }

  if ((gsIsHide(body))||
      (gsIsRename(body))||
      (gsIsAllow(body))||
      (gsIsComm(body))||
      (gsIsRestrict(body)))
  { 
    collectPcrlProcesses_term(ATAgetArgument(body,1),visited);
    return;
  }

  if ((gsIsDelta(body))||
      (gsIsTau(body))||
      (gsIsMultAct(body))||
      (gsIsAction(body)))
  { return;
  }

  ATerror("Internal error: Process has unexpected format (1) %t\n",body);
} 

static ATermList localpcrlprocesses=NULL;

static void collectPcrlProcesses_rec(
                   ATermAppl procDecl,
                   ATermIndexedSet visited)
{ ATbool new=0;
  ATindexedSetPut(visited,(ATerm)procDecl,&new);

  if (new)
  { int n=objectIndex(procDecl);
    assert(n>=0); /* if this fails, the process does not exist */

    if (objectdata[n].processstatus==pCRL)
    { localpcrlprocesses=ATinsertA(localpcrlprocesses,procDecl); 
    }
    collectPcrlProcesses_term(objectdata[n].processbody,visited);
  }
}

static ATermList collectPcrlProcesses(
               ATermAppl initprocess)
{ localpcrlprocesses=ATempty;
  ATermIndexedSet visited=ATindexedSetCreate(128,50);
  collectPcrlProcesses_rec(initprocess,visited);
  ATindexedSetDestroy(visited);
  return localpcrlprocesses;
}

/************ correctopenterm ********************************************/ 


static ATermAppl fresh_name(char *name)
{ /* it still has to be checked whether a name is already being used 
     in that case a new name has to be generated. The result
     is of type ATermAppl, because the string is generally used
     in this form. */
  string *str;
  int i;
  str=new_string(name);
  for( i=0 ; (existsString(str->s)) ; i++)
    { snprintf(str->s,STRINGLENGTH,"%s%d",name,i); }
  /* check that name does not already exist, otherwise,
     add some suffix and check again */
  return gsString2ATermAppl(str->s);
}

/****************  occursinterm *** occursintermlist ***********/

static int occursinterm(ATermAppl var, ATermAppl t)
{ 
  if (gsIsDataVarId(t))
  { return var==t; }

  if (gsIsOpId(t))
  { return 0; }

  assert(gsIsDataAppl(t));

  return occursinterm(var,ATAgetArgument(t,0))||
         occursinterm(var,ATAgetArgument(t,1));
}


static int occursintermlist(ATermAppl var, ATermList l)
{ 
  for( ; l!=ATempty ; l=ATgetNext(l))
  { if (occursinterm(var,ATAgetFirst(l)))
    return 1;
  }
  return 0;
}

static int occursinmultiaction(ATermAppl var, ATermList ma)
{ 
  for( ; ma!=ATempty ; ma=ATgetNext(ma) )
  { if (occursintermlist(var,ATLgetArgument(ATAgetFirst(ma),1)))
    { return 1; 
    }
  }
  return 0;
}

static int occursinpCRLterm(ATermAppl var, ATermAppl p, int strict)
{ 
  if (gsIsChoice(p))
  { return occursinpCRLterm(var,ATAgetArgument(p,0),strict)||
           occursinpCRLterm(var,ATAgetArgument(p,1),strict);
  }
  if (gsIsSeq(p))
  { return occursinpCRLterm(var,ATAgetArgument(p,0),strict)||
           occursinpCRLterm(var,ATAgetArgument(p,1),strict);
  } 
  if (gsIsCond(p))
  { return occursinterm(var,ATAgetArgument(p,0))||
           occursinpCRLterm(var,ATAgetArgument(p,1),strict)||
           occursinpCRLterm(var,ATAgetArgument(p,2),strict);
  }
  if (gsIsSum(p)) 
  { if (strict)
       return occursintermlist(var,ATLgetArgument(p,0))||
              occursinpCRLterm(var,ATAgetArgument(p,1),strict);
    /* below appears better? , but leads
       to errors. Should be investigated. */
     else 
       return 
          (!occursintermlist(var,ATLgetArgument(p,0)))&&
          occursinpCRLterm(var,ATAgetArgument(p,1),strict);
  }
  if (gsIsProcess(p))
  { return occursintermlist(var,ATLgetArgument(p,1));
  }
  if (gsIsAction(p)) 
  { return occursintermlist(var,ATLgetArgument(p,1));
  }
  if (gsIsMultAct(p)) 
  { return occursinmultiaction(var,ATLgetArgument(p,0));
  }
  if (gsIsDelta(p))
   { return 0; }
  if (gsIsTau(p))
   { return 0; }
  ATerror("Internal: Unexpected process format in occursinCRLterm %t\n",p);
  return 0;
}

static void alphaconvertprocess(
           ATermList *sumvars,
           ATermList *rename_vars,
           ATermList *rename_terms,
           ATermAppl p)
{ /* This function replaces the variables in sumvars
     by unique ones if these variables occur in occurvars
     or occurterms. It extends rename_vars and rename 
     terms to rename the replaced variables to new ones. */
  ATermList newsumvars=ATempty;

  for(ATermList l=*sumvars ; l!=ATempty ; l=ATgetNext(l))
  { ATermAppl var=ATAgetFirst(l);
    if (occursinpCRLterm(var,p,1))
    { ATermAppl newvar=getfreshvariable(
                          ATSgetArgument(var,0),
                          ATAgetArgument(var,1));
      newsumvars=ATinsertA(newsumvars,newvar);
      if (rename_vars!=NULL)
         *rename_vars=ATinsertA(*rename_vars,var);
      if (rename_terms!=NULL)
         *rename_terms=ATinsertA(*rename_terms,newvar);
    }
    else
      newsumvars=ATinsertA(newsumvars,var);
  }
  *sumvars=ATreverse(newsumvars);
}


static void alphaconvert(
           ATermList *sumvars, 
           ATermList *rename_vars, 
           ATermList *rename_terms,
           ATermList occurvars,
           ATermList occurterms)
{ /* This function replaces the variables in sumvars
     by unique ones if these variables occur in occurvars
     or occurterms. It extends rename_vars and rename 
     terms to rename the replaced variables to new ones. */
  ATermList newsumvars=ATempty;

  for(ATermList l=*sumvars ; l!=ATempty ; l=ATgetNext(l))
  { ATermAppl var=ATAgetFirst(l);
    if (occursintermlist(var,occurvars) ||
        occursintermlist(var,occurterms))
    { ATermAppl newvar=getfreshvariable(
                          ATSgetArgument(var,0),
                          ATAgetArgument(var,1));
      newsumvars=ATinsertA(newsumvars,newvar);
      if (rename_vars!=NULL) 
         *rename_vars=ATinsertA(*rename_vars,var);
      if (rename_terms!=NULL)
         *rename_terms=ATinsertA(*rename_terms,newvar);
    }
    else
      newsumvars=ATinsertA(newsumvars,var);
  }
  *sumvars=ATreverse(newsumvars);
}

/******************* substitute *****************************************/
static ATermAppl substitute_variable_rec(
                 ATermList terms, 
                 ATermList vars,
                 ATermAppl s_term)
{ 
  if (terms==ATempty) 
   { 
#ifndef NDEBUG
     if (vars!=ATempty) 
     { ATerror("Non matching terms and vars list\n");
     }
#endif
     return s_term;
   }
  assert(gsIsDataVarId(ATAgetFirst(vars)));
  assert(gsGetSort(ATAgetFirst(terms))==gsGetSort(ATAgetFirst(vars)));
  if (s_term==ATAgetFirst(vars))
   { return ATAgetFirst(terms); }
  return substitute_variable_rec(ATgetNext(terms),ATgetNext(vars),s_term);
}


static ATermList substitute_datalist_rec(
                 ATermList terms, 
                 ATermList vars, 
                 ATermList tl);

static ATermAppl substitute_data_rec(
                 ATermList terms, 
                 ATermList vars,
                 ATermAppl t)
{ 
  /* if (gsIsNil(t)), dont care terms do not exist anymore. Therefore,
                      Nil is no valid term.
  { return t;
  } */

  if (gsIsDataAppl(t))
  { 
    return gsMakeDataAppl(
               substitute_data_rec(terms,vars,ATAgetArgument(t,0)),
               substitute_data_rec(terms,vars,ATAgetArgument(t,1)));
  }

  if ( gsIsDataVarId(t))
  { return substitute_variable_rec(terms,vars,t);
  }

  /* Exists en forall do not occur in terms.
  if (gsIsExists(t))
  { ATfprintf(stderr,"Warning: no renaming of variable in exists\n");
    return gsMakeExists(
                 ATLgetArgument(t,0),
                 substitute_data_rec(terms,vars,ATAgetArgument(t,1)));
                   
  } 

  if (gsIsForall(t))
  { ATfprintf(stderr,"Warning: no renaming of variable in forall\n");
    return gsMakeForall(
                 ATLgetArgument(t,0),
                 substitute_data_rec(terms,vars,ATAgetArgument(t,1)));
                   
  } */

  assert(gsIsOpId(t));
  return t; 
}

static ATermList substitute_datalist_rec(
                 ATermList terms, 
                 ATermList vars,
                 ATermList tl) 
{ 
  if (tl==ATempty) return tl;

  return ATinsertA(
           substitute_datalist_rec(terms,vars,ATgetNext(tl)),
           substitute_data_rec(terms,vars,ATAgetFirst(tl)));
}



static ATermList substitute_datalist(
                 ATermList terms, 
                 ATermList vars,
                 ATermList tl) 
{ 
  if (terms==ATempty) return tl;
  return substitute_datalist_rec(terms,vars,tl);
}

static ATermAppl substitute_data(
                 ATermList terms, 
                 ATermList vars,
                 ATermAppl t)
{
  if (terms==ATempty) return t;
  return substitute_data_rec(terms,vars,t);
}

static ATermList substitute_multiaction_rec(
                 ATermList terms,
                 ATermList vars,
                 ATermList multiAction)
{ 
  ATermAppl action=NULL;
  if (multiAction==ATempty)
  { return ATempty;
  }

  action=ATAgetFirst(multiAction);
  assert(gsIsAction(action));

  return ATinsertA(substitute_multiaction_rec(terms,vars,ATgetNext(multiAction)),
                   gsMakeAction(ATAgetArgument(action,0),
                                substitute_datalist(
                                       terms,
                                       vars,
                                       ATLgetArgument(action,1))));
}

static ATermAppl substitute_multiaction(
                 ATermList terms,
                 ATermList vars,
                 ATermAppl multiAction)
{ 
  if (gsIsDelta(multiAction))
  return multiAction;

  assert(gsIsMultAct(multiAction));
  return gsMakeMultAct(substitute_multiaction_rec(terms,vars,ATLgetArgument(multiAction,0)));

}

static ATermList substitute_assignmentlist(
                 ATermList terms,
                 ATermList vars,
                 ATermList assignments,
                 ATermList parameters,
                 int replacelhs)
{ /* precondition: the variables in the assignment occur in
     the same sequence as in the parameters, which stands for the
     total list of parameters.

     This function replaces the variables in vars by the terms in terms
     in the right hand side of the assignments. If for some variable
     occuring in the parameterlist no assignment is present, whereas
     this variable occurs in vars, an assignment for it is added.

     If replacelhs is true, it is assumed that the parameters contains
     the variables after replacing the vars by terms. In this case
     the terms must be variables themselves. The result is that in the
     lhs and the rhs the variables are renamed. */


  if (parameters==ATempty)
  { assert(assignments==ATempty);
    return ATempty; 
  }
  ATermAppl parameter=ATAgetFirst(parameters);

  if (assignments!=ATempty)
  { ATermAppl assignment=ATAgetFirst(assignments);
    ATermAppl lhs=ATAgetArgument(assignment,0);
    if (replacelhs) 
    { lhs=substitute_data(terms,vars,lhs);
      assert(gsIsDataVarId(lhs));
    }
    if (parameter==lhs)
    { /* The assignment refers to parameter par. Substitute its
         righthandside and check whether the left and right
         handside have become equal, in which case no assignment
         is necessary anymore */
      ATermAppl rhs=substitute_data(terms,vars,ATAgetArgument(assignment,1));
      if (lhs==rhs)
      { return substitute_assignmentlist(
                    terms,
                    vars,
                    ATgetNext(assignments),
                    ATgetNext(parameters),
                    replacelhs);
      }
      return ATinsertA(
                substitute_assignmentlist(
                    terms,
                    vars,
                    ATgetNext(assignments),
                    ATgetNext(parameters),
                    replacelhs),
                gsMakeAssignment(lhs,rhs));
    }
  }

  /* Here the first parameter is not equal to the first
     assignment. So, we must find out whether a value
     for this variable is substituted, that is different
     from the variable, in which case an assignment must 
     be added. */

  if (!replacelhs)
  { /* if replacelhs holds the left and right side of
       all new assignments will be the same */
    ATermAppl rhs=substitute_data(terms,vars,parameter);
    if (parameter==rhs)
    { /* no assignment needs to be added */
      return substitute_assignmentlist(
                      terms,
                      vars,
                      assignments,
                      ATgetNext(parameters),
                      replacelhs);
    }
    /* an assignment is necessary */
    return ATinsertA(
              substitute_assignmentlist(
                  terms,
                  vars,
                  assignments,
                  ATgetNext(parameters),
                  replacelhs),
              gsMakeAssignment(parameter,rhs));
  }
  return substitute_assignmentlist(
                      terms,
                      vars,
                      assignments,
                      ATgetNext(parameters),
                      replacelhs);
}

static ATermAppl substitute_time(
                 ATermList terms,
                 ATermList vars,
                 ATermAppl time)
{ if (gsIsNil(time))
  { return time;
  }
  return substitute_data(terms,vars,time);
}

static ATermAppl substitute_pCRLproc(
                 ATermList terms, 
                 ATermList vars,
                 ATermAppl p)
{ 
  if (gsIsChoice(p))  
  { return gsMakeChoice(
                substitute_pCRLproc(terms,vars,ATAgetArgument(p,0)),
                substitute_pCRLproc(terms,vars,ATAgetArgument(p,1))); 
  }
  if (gsIsSeq(p))
  { return gsMakeSeq(
                substitute_pCRLproc(terms,vars,ATAgetArgument(p,0)),
                substitute_pCRLproc(terms,vars,ATAgetArgument(p,1)));
  }
  if (gsIsSync(p))
  { return gsMakeSync(
                substitute_pCRLproc(terms,vars,ATAgetArgument(p,0)),
                substitute_pCRLproc(terms,vars,ATAgetArgument(p,1)));
  }
  if (gsIsCond(p))
  { return gsMakeCond(
                substitute_data(terms,vars,ATAgetArgument(p,0)),
                substitute_pCRLproc(terms,vars,ATAgetArgument(p,1)),
                substitute_pCRLproc(terms,vars,ATAgetArgument(p,2)));
  }

  if (gsIsSum(p))   
  { ATermList sumargs=ATLgetArgument(p,0);

    alphaconvert(&sumargs,&terms,&vars,terms,vars);
    return gsMakeSum(
               sumargs,
               substitute_pCRLproc(terms,vars,ATAgetArgument(p,1))); 
  }

  if (gsIsProcess(p))
  { return gsMakeProcess(ATAgetArgument(p,0),
                substitute_datalist(terms,vars,ATLgetArgument(p,1))); 
  }
  
  if (gsIsAction(p))
  { return gsMakeAction(ATAgetArgument(p,0),
                substitute_datalist(terms,vars,ATLgetArgument(p,1)));
  }

  if (gsIsAtTime(p))
  { 
    return gsMakeAtTime(
               substitute_pCRLproc(terms,vars,ATAgetArgument(p,0)),
               substitute_data(terms,vars,ATAgetArgument(p,1)));
  }

  if (gsIsDelta(p))
     { return p; }

  if (gsIsTau(p))
     { return p; }

  if (gsIsMultAct(p))
  { return substitute_multiaction(
                      terms,
                      vars,
                      p);

  }

  ATerror("Internal error: Expect a pCRL process %t\n",p);
  return NULL;
}

/********************************************************************/
/*                                                                  */
/*   BELOW THE PROCEDURES ARE GIVEN TO TRANFORM PROCESSES TO        */
/*   LINEAR PROCESSES.                                              */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/********************************************************************/

typedef enum { first, later } variableposition;

/****************  tovarheadGNF  *********************************/

static ATermList parameters_that_occur_in_body(
              ATermList parameters, 
              ATermAppl  body)
{ 
  ATermAppl variable=NULL;
  if (parameters==ATempty)
      return parameters;
  
  variable=ATAgetFirst(parameters);
  if (!gsIsDataVarId(variable))
         ATerror("Expect variable list %t\n",parameters);

  parameters=parameters_that_occur_in_body(ATgetNext(parameters),body);
  if (occursinpCRLterm(variable,body,0))
       return ATinsertA(parameters,variable);
  return parameters; 
}        

static ATermAppl newprocess(ATermList parameters, ATermAppl body,
              processstatustype ps, int canterminate)
{ 
  parameters=parameters_that_occur_in_body(parameters, body);
  ATermAppl p=gsMakeProcVarId(fresh_name("P"),linGetSorts(parameters));
  insertProcDeclaration(
             p,
             parameters,
             body,
             ps,
             canterminate);
  return p;
}


static ATermAppl wraptime(
                    ATermAppl body, 
                    ATermAppl time,
                    ATermList freevars)
{
  if (gsIsChoice(body))
  { return gsMakeChoice(
              wraptime(ATAgetArgument(body,0),time,freevars),
              wraptime(ATAgetArgument(body,1),time,freevars));
  }

  if (gsIsSum(body))
  { ATermList sumvars=ATLgetArgument(body,0);
    ATermAppl body1=ATAgetArgument(body,1);
    ATermList renamevars=ATempty;
    ATermList renameterms=ATempty;
    alphaconvert(&sumvars,&renamevars,&renameterms,freevars,ATempty);
    body1=substitute_pCRLproc(renameterms,renamevars,body1);
    time=substitute_data(renameterms,renamevars,time);
    body1=wraptime(body1,time,ATconcat(sumvars,freevars));
    return gsMakeSum(sumvars,body1);
  }

  if (gsIsCond(body))
  { return gsMakeCond(
              ATAgetArgument(body,0),
              wraptime(ATAgetArgument(body,1),time,freevars),
              gsMakeDelta());
  }

  if (gsIsSeq(body))
  { return gsMakeSeq(
              wraptime(ATAgetArgument(body,0),time,freevars),
              ATAgetArgument(body,1));
  }

  if (gsIsAtTime(body))
  { /* make a new process */
    ATermAppl newproc=newprocess(freevars,body,pCRL,canterminatebody(body,NULL,NULL,0));
    return gsMakeAtTime(
              gsMakeProcess(
                 newproc,
                 objectdata[objectIndex(newproc)].parameters),
              time);
  }

  if (gsIsProcess(body))
  { return gsMakeAtTime(body,time);
  }

  if (gsIsMultAct(body))
  { return gsMakeAtTime(body,time);
  }

  ATerror("Internal: Expect pCRL process in wraptime %t\n",body);
  return NULL;
}

typedef enum { alt, sum, /* cond,*/ seq, name, multiaction } state;

static ATermAppl getfreshvariable(char *s, ATermAppl sort)
{ ATermAppl variable=NULL;
  variable=gsMakeDataVarId(fresh_name(s),sort);
  insertvariable(variable,1);
  return variable; 
}

static ATermList make_pars(ATermList sortlist)
{ /* this function returns a list of variables, 
     corresponding to the sorts in sortlist */

  ATermAppl sort=NULL;
  char *s=NULL;

  if (sortlist==NULL)
     return ATempty;

  sort=ATAgetFirst(sortlist);
  s=NULL;
  assert(gsIsSortId(sort));
  if (gsIsSortId(sort))
  { s=ATSgetArgument(sort,0); }
  else ATerror("Unexpected sort encountered %t\n",sort);

  return ATinsertA(
            make_pars(ATgetNext(sortlist)),
            getfreshvariable(s,sort));
} 


/* the following variables give the indices of the processes that represent tau
     and delta, respectively */
     
static ATermAppl tau_process=NULL;
static ATermAppl delta_process=NULL;

static ATermAppl bodytovarheadGNF(
            ATermAppl body, 
            state s,
            ATermList freevars, 
            variableposition v)
{ /* it is assumed that we only receive processes with
     operators alt, seq, sum, cond, name, delta, tau, sync, AtTime in it */


  ATermAppl newproc=NULL;

  if (gsIsChoice(body)) 
   { if (alt>=s)
      { 
        ATermAppl body1=bodytovarheadGNF(ATAgetArgument(body,0),alt,freevars,first);
        ATermAppl body2=bodytovarheadGNF(ATAgetArgument(body,1),alt,freevars,first);
        return gsMakeChoice(body1,body2);
      }  
     body=bodytovarheadGNF(body,alt,freevars,first);
     newproc=newprocess(freevars,body,pCRL,canterminatebody(body,NULL,NULL,0));
     return gsMakeProcess(newproc,objectdata[objectIndex(newproc)].parameters);
   }

  if (gsIsSum(body)) 
  { 
    if (sum>=s)
    { ATermList renamevars=ATempty;
      ATermList sumvars=ATLgetArgument(body,0);
      ATermAppl body1=ATAgetArgument(body,1);
      ATermList renameterms=ATempty;
      alphaconvert(&sumvars,&renamevars,&renameterms,freevars,ATempty);
      body1=substitute_pCRLproc(renameterms,renamevars,body1);
      body1=bodytovarheadGNF(body1,sum,ATconcat(sumvars,freevars),first);
      return gsMakeSum(sumvars,body1);
    }
    body=bodytovarheadGNF(body,alt,freevars,first);
    newproc=newprocess(freevars,body,pCRL,canterminatebody(body,NULL,NULL,0));
    return gsMakeProcess(newproc,objectdata[objectIndex(newproc)].parameters);
  }
  
  if (gsIsCond(body))
  { ATermAppl condition=ATAgetArgument(body,0);
    ATermAppl body1=ATAgetArgument(body,1);
    ATermAppl body2=ATAgetArgument(body,2);

    if ((s<=sum) && ((gsIsDelta(body1))||(gsIsDelta(body2))))
    { if (gsIsDelta(body2))
      { return gsMakeCond(
                condition,
                bodytovarheadGNF(body1,seq,freevars,first),
                gsMakeDelta());
      }
      /* body1==ATmake("Delta") */
      { 
        return gsMakeCond(
                gsMakeDataExprNot(condition),
                bodytovarheadGNF(body2,seq,freevars,first),
                gsMakeDelta());
    } } 
    if (alt==s) /* body1!=Delta and body1!=Delta */
    { return 
        gsMakeChoice(
          gsMakeCond(
                condition,
                bodytovarheadGNF(body1,seq,freevars,first),
                gsMakeDelta()),
          gsMakeCond(
                gsMakeDataExprNot(condition),
                bodytovarheadGNF(body2,seq,freevars,first),
                gsMakeDelta()));
    }     
    body=bodytovarheadGNF(body,alt,freevars,first);
    newproc=newprocess(freevars,body,pCRL,canterminatebody(body,NULL,NULL,0));
    return gsMakeProcess(newproc,objectdata[objectIndex(newproc)].parameters);
  } 

  if (gsIsSeq(body))
  { ATermAppl body1=ATAgetArgument(body,0);
    ATermAppl body2=ATAgetArgument(body,1);

    if (seq>=s)
    { body1=bodytovarheadGNF(body1,name,freevars,v);
      body2=bodytovarheadGNF(body2,seq,freevars,later);
      return gsMakeSeq(body1,body2);
    } 
    body1=bodytovarheadGNF(body,alt,freevars,first);
    newproc=newprocess(freevars,body1,pCRL,canterminatebody(body1,NULL,NULL,0));
    return gsMakeProcess(newproc,objectdata[objectIndex(newproc)].parameters);
  }

  if (gsIsAction(body))
  { ATbool isnew=0;
    ATermAppl ma=gsMakeMultAct(ATinsertA(ATempty,body)); 
    if ((s==multiaction)||(v==first))
    { return ma;
    }
    
    long n=addMultiAction(ma,&isnew); 
  
    
    if (objectdata[n].targetsort==NULL)
    { /* this action does not yet have a corresponding process, which
         must be constructed. The resulting process is stored in 
         the variable targetsort in objectdata. */
      objectdata[n].targetsort=newprocess(
                                  objectdata[n].parameters,
                                  objectdata[n].processbody,
                                  GNF,1);
    }
    return gsMakeProcess(objectdata[n].targetsort,getarguments(ma)); 
  }  

  if (gsIsMultAct(body))
  { ATbool isnew=0;
    if ((s==multiaction)||(v==first))
    { return body;
    }

    long n=addMultiAction(body,&isnew);
   
    if (objectdata[n].targetsort==NULL)
    { /* this action does not yet have a corresponding process, which
         must be constructed. The resulting process is stored in 
         the variable targetsort in objectdata. */
      objectdata[n].targetsort=newprocess(
                                  objectdata[n].parameters,
                                  objectdata[n].processbody,
                                  GNF,1);
    }
    return gsMakeProcess(objectdata[n].targetsort,getarguments(body));
  } 


  if (gsIsSync(body))
  { 
    ATbool isnew=0;
    ATermAppl body1=ATAgetArgument(body,0);
    ATermAppl body2=ATAgetArgument(body,1);
    ATermAppl ma=linMergeMultiAction(
                         bodytovarheadGNF(body1,multiaction,freevars,v),
                         bodytovarheadGNF(body2,multiaction,freevars,v));

    if ((s==multiaction)||(v==first))
    { return ma;
    }

    long n=addMultiAction(ma,&isnew);
   
    if (objectdata[n].targetsort==NULL)
    { /* this action does not yet have a corresponding process, which
         must be constructed. The resulting process is stored in 
         the variable targetsort in objectdata. */
      objectdata[n].targetsort=newprocess(
                                  objectdata[n].parameters,
                                  objectdata[n].processbody,
                                  GNF,1);
    }
    return gsMakeProcess(objectdata[n].targetsort,getarguments(ma));
 
  }

  if (gsIsAtTime(body))
  { ATermAppl body1=bodytovarheadGNF(
                         ATAgetArgument(body,0),
                         s,
                         freevars,
                         first);
    if (v==first)
    { /* put the time operator around the first action 
         or process */
      
      ATermAppl time=ATAgetArgument(body,1);
      return wraptime(body1,time,freevars);
    }

    /* make a new process, containing this process */
    newproc=newprocess(freevars,body1,pCRL,canterminatebody(body1,NULL,NULL,0));
    return gsMakeProcess(newproc,objectdata[objectIndex(newproc)].parameters);
  }
  
  if (gsIsProcess(body))
  { return body;
  }  
  
  if (gsIsTau(body))
  { if (v==first) 
    {  
      return gsMakeMultAct(ATempty); 
    }
    if (tau_process==NULL)
    { tau_process=newprocess(ATempty,gsMakeMultAct(ATempty),pCRL,1);
    }
    return gsMakeProcess(tau_process,ATempty); 
  }
  
  if (gsIsDelta(body))
  { if (v==first) 
       return body; 
    if (delta_process==NULL)
       delta_process=newprocess(ATempty,body,pCRL,0);
    return gsMakeProcess(delta_process,ATempty); 
  }

  ATerror("Internal: Unexpected process format in bodytovarheadGNF %t\n",body);
  return NULL;
}


static void procstovarheadGNF(ATermList procs)
{ /* transform the processes in procs into newprocs */
  for( ; (procs!=ATempty) ; procs=ATgetNext(procs))
  { ATermAppl proc=ATAgetFirst(procs);
    long n=objectIndex(proc);
    objectdata[n].processbody=bodytovarheadGNF(
                objectdata[n].processbody,
                alt, 
                objectdata[n].parameters,
                first);
  }
}

/**************** towards real GREIBACH normal form **************/

typedef enum {terminating,infinite} terminationstatus;

static ATermAppl putbehind(ATermAppl body1, ATermAppl body2)
{ 
  if (gsIsChoice(body1)) 
  { return gsMakeChoice(
             putbehind(ATAgetArgument(body1,0),body2),
             putbehind(ATAgetArgument(body1,1),body2)); 
  }

  if (gsIsSeq(body1))
  { return gsMakeSeq(
             ATAgetArgument(body1,0),
             putbehind(ATAgetArgument(body1,1),body2));
   }

  if (gsIsCond(body1))
  { assert(gsIsDelta(ATAgetArgument(body1,2)));
     return gsMakeCond(
             ATAgetArgument(body1,0),
             putbehind(ATAgetArgument(body1,1),body2),
             gsMakeDelta());
  }
  
  if (gsIsSum(body1))
  { /* we must take care that no variables in body2 are
        inadvertently bound */
    ATermList sumvars=ATLgetArgument(body1,0);
    ATermList vars=ATempty;
    ATermList terms=ATempty;
    alphaconvertprocess(&sumvars,&vars,&terms,body2);
    return gsMakeSum(
               sumvars,
               putbehind(substitute_pCRLproc(
                            terms,
                            vars,
                            ATAgetArgument(body1,1)),
                    body2));
  }
  
  if (gsIsAction(body1))
  { /* return gsMakeSeq(body1,body2); */
    ATerror("Internal: expect only multiactions, no single actions\n");
  }

  if (gsIsMultAct(body1))
  { return gsMakeSeq(body1,body2);
  }
  
  if (gsIsProcess(body1))
  { return gsMakeSeq(body1,body2);
  }
  
  if (gsIsDelta(body1)) 
  { return body1;
  }
  
  if (gsIsTau(body1))
  { /* return gsMakeSeq(body1,body2); */
    ATerror("Internal: expect only multiactions, not a tau\n");
  }
  
  if (gsIsAtTime(body1))
  { return gsMakeSeq(body1,body2); 
  }

  ATerror("Internal: Unexpected process format in putbehind %t\n",body1);
  return NULL;
}

static ATermAppl distribute_condition(
                    ATermAppl body1,
                    ATermAppl condition)
{ 
  if (gsIsChoice(body1)) 
  { return gsMakeChoice(
               distribute_condition(ATAgetArgument(body1,0),condition),
               distribute_condition(ATAgetArgument(body1,1),condition));
  }
  
  if (gsIsSeq(body1))
  { return gsMakeCond(condition,body1,gsMakeDelta());
  }
  
  if (gsIsCond(body1)) 
  { assert(gsIsDelta(ATAgetArgument(body1,2)));
    return gsMakeCond(
              gsMakeDataExprAnd(ATAgetArgument(body1,0),condition),
              ATAgetArgument(body1,1),
              gsMakeDelta());
  }
  
  if (gsIsSum(body1))
  { /* we must take care that no variables in condition are
        inadvertently bound */
    ATermList sumvars=ATLgetArgument(body1,0);
    ATermList vars=ATempty;
    ATermList terms=ATempty;
    alphaconvert(&sumvars,&vars,&terms,ATempty,
                       ATinsertA(ATempty,condition));
    return gsMakeSum(
             sumvars,
             distribute_condition(ATAgetArgument(body1,1),condition));
   }
  
  if (gsIsAction(body1))
   { return gsMakeCond(condition,body1,gsMakeDelta());
   }
  
  if (gsIsMultAct(body1))
   { return gsMakeCond(condition,body1,gsMakeDelta());
   }
  
  if (gsIsProcess(body1))
   { return gsMakeCond(condition,body1,gsMakeDelta());
   }
  
  if (gsIsDelta(body1))
   { return body1;
   }
  
  if (gsIsTau(body1))
  { return gsMakeCond(condition,body1,gsMakeDelta());
  }

  ATerror("Internal: Unexpected process format in distribute condition %t\n",body1);
  return NULL;
}

static ATermAppl distribute_sum(ATermList sumvars,ATermAppl body1)
{ 
  if (gsIsChoice(body1))
  { return gsMakeChoice(
               distribute_sum(sumvars,ATAgetArgument(body1,0)),
               distribute_sum(sumvars,ATAgetArgument(body1,1)));
  }
  
  if (gsIsSeq(body1)||
      gsIsCond(body1)||
      gsIsMultAct(body1)||
      gsIsProcess(body1))
  { return gsMakeSum(sumvars,body1);
  }
  
  if (gsIsSum(body1))
  { return gsMakeSum(
             ATconcat(sumvars,ATLgetArgument(body1,0)),
             ATAgetArgument(body1,1));
  }
  
  if (gsIsDelta(body1)||
      gsIsTau(body1))
  { return body1;
  }

  ATerror("Internal: Unexpected process format in distribute_sum %t\n",body1);
  return NULL;
}

static ATermAppl exists_variable_for_sequence(
                     ATermList process_names,
                     ATermAppl process_body)
{ ATermList walker=NULL;

  if (regular2)
  { for(walker=seq_varnames; (walker!=ATempty);
                    walker=ATgetNext(walker))
    { ATermAppl process=ATAgetFirst(walker);
      if (process_names==
            (ATermList)objectdata[objectIndex(process)].representedprocesses)
      return process;
    }
    return NULL;  
  }
  
  for(walker=seq_varnames; (walker!=ATempty);
         walker=ATgetNext(walker))
  { ATermAppl process=ATAgetFirst(walker);
    if (process_body==
             (ATermAppl)objectdata[objectIndex(process)].representedprocess)
     return process;
  }
  return NULL;  
}

static void procstorealGNFrec(ATermAppl procIdDecl, variableposition v, 
       ATermList *todo, int regular);

static ATermList extract_names(ATermAppl sequence)
{ 
  if (gsIsAction(sequence)||gsIsProcess(sequence))
  { return ATinsertA(ATempty,sequence);
  }

  if (gsIsSeq(sequence))
  { ATermAppl first=ATAgetArgument(sequence,0);
    if (gsIsProcess(first))
    { long n=objectIndex(ATAgetArgument(first,0));
      if (objectdata[n].canterminate)
         return ATinsertA(
                  extract_names(ATAgetArgument(sequence,1)),
                  first);
      else return ATinsertA(ATempty,first); 
    } 
  }

  ATerror("Expect sequence of process names (1) %t\n",sequence);
  return NULL;
}

static ATermList parscollect(ATermAppl oldbody, ATermAppl *newbody)
{ /* we expect that oldbody is a sequence of process references */ 


  if (gsIsProcess(oldbody))
  { ATermAppl procId=ATAgetArgument(oldbody,0); 
    ATermList parameters=objectdata[objectIndex(procId)].parameters;
    *newbody=gsMakeProcess(procId,parameters);
    return parameters;
  }     

  if (gsIsSeq(oldbody))   
  { ATermAppl first=ATAgetArgument(oldbody,0);
    if (gsIsProcess(first))
    { ATermAppl procId=ATAgetArgument(first,0);
      ATermList pars=parscollect(ATAgetArgument(oldbody,1),newbody);
      ATermList pars1=ATempty, pars2=ATempty;
     
      construct_renaming(pars,objectdata[objectIndex(procId)].parameters,&pars1,&pars2);

      *newbody=gsMakeSeq(
                 gsMakeProcess(procId,pars1),
                 *newbody);
      return ATconcat(pars1,pars);
    }
  } 

  ATerror("Expect a sequence of process names (2) %t\n",oldbody);
  return NULL;
}

static ATermList argscollect(ATermAppl t)
{ 
  if (gsIsProcess(t))
  return ATLgetArgument(t,1);

  if (gsIsSeq(t))    
  { ATermAppl firstproc=ATAgetArgument(t,0);
    assert(gsIsProcess(firstproc));
    return ATconcat(ATLgetArgument(firstproc,1),argscollect(ATAgetArgument(t,1)));
  }

  ATerror("Expect a sequence of process names (3) %t\n",t);
  return NULL;      
}

static ATermAppl create_regular_invocation(
         ATermAppl sequence,
         ATermList *todo,
         ATermList freevars)
{ ATermList process_names=NULL;
  ATermAppl new_process=NULL;
  ATermList args=NULL;

  /* Sequence consists of a sequence of process references, 
     concatenated with the sequential composition operator */
  process_names=extract_names(sequence);
  assert(process_names!=ATempty);

  if (ATgetLength(process_names)==1)
  { /* length of list equals 1 */
    if (gsIsProcess(sequence)) 
       return sequence;
    if (gsIsSeq(sequence))
       return ATAgetArgument(sequence,0);
    ATerror("Expect a sequence of process names %t\n",sequence);
  }
  /* There is more than one process name in the sequence,
     so, we must replace them by a single name */
  
  /* We first start out by searching whether
     there is already a variable with a matching sequence
     of variables */
  new_process=exists_variable_for_sequence(process_names,sequence);

  if (new_process==NULL)
  { /* There does not exist an appropriate variable,
       so, make it and return its index in n */
    ATermAppl newbody=NULL;   
    if (regular2)
    { ATermList pars=parscollect(sequence,&newbody);
      new_process=newprocess(pars,newbody,pCRL,canterminatebody(newbody,NULL,NULL,0));
      objectdata[objectIndex(new_process)].representedprocesses=
                   (ATerm)process_names;
    }
    else 
    { new_process=newprocess(freevars,sequence,pCRL,
                        canterminatebody(sequence,NULL,NULL,0));
      objectdata[objectIndex(new_process)].representedprocess=
                   (ATerm)sequence;
    }
    seq_varnames=ATinsertA(seq_varnames,new_process);
    *todo=ATinsertA(*todo,new_process);
  }
  /* now we must construct arguments */
  if (regular2)
  { args=argscollect(sequence);
  }
  else
  { args=objectdata[objectIndex(new_process)].parameters;
  }
  return gsMakeProcess(new_process,args);
}

static ATermAppl to_regular_form(
                    ATermAppl t,
                    ATermList *todo,
                    ATermList freevars)
/* t has the form of the sum, and condition over actions 
   each followed by a sequence of variables. We replace
   this variable by a single one, putting the new variable
   on the todo list, to be transformed to regular form also. */
{ 
  if (gsIsChoice(t)) 
  { ATermAppl t1=to_regular_form(ATAgetArgument(t,0),todo,freevars);
    ATermAppl t2=to_regular_form(ATAgetArgument(t,1),todo,freevars);
    return gsMakeChoice(t1,t2); 
  } 
  
  if (gsIsSeq(t)) 
  { ATermAppl firstact=ATAgetArgument(t,0);
    assert(gsIsMultAct(firstact)||gsIsAtTime(firstact));
    /* the sequence of variables in 
               the second argument must be replaced */
    return gsMakeSeq(
              firstact,
              create_regular_invocation(ATAgetArgument(t,1),todo,freevars));
  } 
  
  if (gsIsCond(t))
  { assert(gsIsDelta(ATAgetArgument(t,2)));
    return gsMakeCond(
              ATAgetArgument(t,0),
              to_regular_form(ATAgetArgument(t,1),todo,freevars),
              gsMakeDelta());

  } 
  
  if (gsIsSum(t))
  { ATermList sumvars=ATLgetArgument(t,0);
    return gsMakeSum(
              sumvars,
              to_regular_form(
                    ATAgetArgument(t,1),
                    todo,
                    ATconcat(sumvars,freevars)));
  }
  
  if (gsIsMultAct(t)||gsIsDelta(t)||gsIsTau(t)) 
  { return t;    
  }
  
  else ATerror("To regular form expects GNF %t\n",t);
  return NULL;
}

static ATermAppl distributeTime(
                    ATermAppl body, 
                    ATermAppl time,
                    ATermList freevars,
                    ATermAppl *timecondition)
{
  if (gsIsChoice(body))
  { return gsMakeChoice(
              distributeTime(ATAgetArgument(body,0),
                             time,freevars,timecondition),
              distributeTime(ATAgetArgument(body,1),
                             time,freevars,timecondition));
  }

  if (gsIsSum(body))
  { ATermList sumvars=ATLgetArgument(body,0);
    ATermAppl body1=ATAgetArgument(body,1);
    ATermList renamevars=ATempty;
    ATermList renameterms=ATempty;
    alphaconvert(&sumvars,&renamevars,&renameterms,freevars,ATempty);
    body1=substitute_pCRLproc(renameterms,renamevars,body1);
    time=substitute_data(renameterms,renamevars,time);
    body1=distributeTime(body1,time,ATconcat(sumvars,freevars),timecondition);
    return gsMakeSum(sumvars,body1);
  }

  if (gsIsCond(body))
  { ATermAppl timecondition=gsMakeDataExprTrue();
    ATermAppl body1=distributeTime(
                       ATAgetArgument(body,1),
                       time,
                       freevars,
                       &timecondition);
    
    return gsMakeCond(
              gsMakeDataExprAnd(ATAgetArgument(body,0),timecondition),
              body1,
              gsMakeDelta());
  }

  if (gsIsSeq(body))
  { return gsMakeSeq(
              distributeTime(ATAgetArgument(body,0),
                             time,freevars,timecondition),
              ATAgetArgument(body,1));
  }

  if (gsIsAtTime(body))
  { /* make a new process */
    *timecondition=gsMakeDataExprEq(time,ATAgetArgument(body,1));
    return body;
  }

  if (gsIsMultAct(body))
  { return gsMakeAtTime(body,time);
  }

  ATerror("Internal: Expect pCRL process in distributeTime %t\n",body);
  return NULL;
}



static ATermAppl procstorealGNFbody(
            ATermAppl body, 
            variableposition v,
            ATermList *todo, 
            int regular, 
            processstatustype mode,
            ATermList freevars)
/* This process delivers the transformation of body
   to GNF with actions as a head symbol, or it
   delivers NULL if body is not a pCRL process.
   If regular=1, then an attempt is made to obtain a
   GNF where one action is always followed by a
   variable. */
{ 

  if (gsIsAtTime(body))
  { ATermAppl timecondition=NULL;
    ATermAppl body1=procstorealGNFbody(
                         ATAgetArgument(body,0),
                         first,
                         todo,
                         regular,
                         mode,
                         freevars);
    return distributeTime(
               body1,
               ATAgetArgument(body,1),
               freevars,
               &timecondition);
  }

  if (gsIsSync(body))
  { ATerror("Internal: Sync operator cannot occur here.\n");
  }

  if (gsIsChoice(body)) 
  { ATermAppl body1=procstorealGNFbody(ATAgetArgument(body,0),first,todo,
                     regular,mode,freevars);
    ATermAppl body2=procstorealGNFbody(ATAgetArgument(body,1),first,todo,
                     regular,mode,freevars);
    return gsMakeChoice(body1,body2);
  }

  if (gsIsSeq(body))     
  { ATermAppl body1=procstorealGNFbody(ATAgetArgument(body,0),v,
                   todo,regular,mode,freevars);
    ATermAppl body2=procstorealGNFbody(ATAgetArgument(body,1),later,
                   todo,regular,mode,freevars);
    ATermAppl t3=putbehind(body1,body2);
    if ((regular) && (v==first))   
    { /* We must transform t3 to regular form */
      t3=to_regular_form(t3,todo,freevars);
    }
    return t3;
  } 
  
  if (gsIsCond(body))  
  { return distribute_condition(
              procstorealGNFbody(ATAgetArgument(body,1),first,
                        todo,regular,mode,freevars),
              ATAgetArgument(body,0));
  }  
  
  if (gsIsSum(body))
  { ATermList sumvars=ATLgetArgument(body,0);
    return distribute_sum(sumvars,
             procstorealGNFbody(ATAgetArgument(body,1),first,
                  todo,regular,mode,ATconcat(sumvars,freevars)));
  }
  
  if (gsIsAction(body))
  { ATerror("Internal: Expect only multiactions at this point\n");
  }
 
  if (gsIsMultAct(body))
  { return body;
  }
  
  if (gsIsProcess(body)) 
  { ATermAppl t3;
    ATermAppl t=ATAgetArgument(body,0);
    long n;
    if (v==later)
    { if ((!regular)||(mode=mCRL)) 
          *todo=ATinsertA(*todo,t);
            /* single = in `mode=mCRL' is important, otherwise crash 
               I do not understand the reason for this at this moment
               JFG (9/5/2000) */
      return body;
    }

    n=objectIndex(ATAgetArgument(body,0));
    if (objectdata[n].processstatus==mCRL)
    { *todo=ATinsertA(*todo,t);
      return NULL; 
    }
    /* The variable is a pCRL process and v==first, so,
       we must now substitute */
    procstorealGNFrec(ATAgetArgument(body,0),first,todo,regular);
    long m=objectIndex(t);
    t3=substitute_pCRLproc(
         ATLgetArgument(body,1),
         objectdata[m].parameters,
         objectdata[m].processbody);
    if (regular)
             t3=to_regular_form(t3,todo,freevars);
    return t3;
  }
  
  if (gsIsDelta(body)) 
  { return body;
  }
  
  if (gsIsTau(body))
  { /* return body; */
    ATerror("Internal: Only expect multiactions, no tau\n");
  }
  
  if (gsIsMerge(body)) 
  { procstorealGNFbody(ATAgetArgument(body,0),later,
                     todo,regular,mode,freevars);
    procstorealGNFbody(ATAgetArgument(body,1),later,
                     todo,regular,mode,freevars);
    return NULL;
  }
  
  if (gsIsHide(body)|| 
      gsIsRename(body)||
      gsIsAllow(body)||
      gsIsRestrict(body)||
      gsIsComm(body))
  { procstorealGNFbody(ATAgetArgument(body,1),later,
                   todo,regular,mode,freevars);
    return NULL;
  }

  ATerror("Internal: Unexpected process format in procstorealGNF %t\n",body);
  return NULL;
}


static void procstorealGNFrec(
                ATermAppl procIdDecl, 
                variableposition v, 
                ATermList *todo, 
                int regular)

/* Do a depth first search on process variables and substitute
   for the headvariable of a pCRL process, in case it is a process, 
   such that we obtain a Greibach Normal Form. All pCRL processes will
   be labelled with GNF to indicate that they are in
   Greibach Normal Form. */

{ long n=objectIndex(procIdDecl);
  ATermAppl t=NULL;


  if (objectdata[n].processstatus==pCRL)
  { objectdata[n].processstatus=GNFbusy;
    t=procstorealGNFbody(objectdata[n].processbody,first,
              todo,regular,pCRL,objectdata[n].parameters);
    if (objectdata[n].processstatus!=GNFbusy)
            ATerror("Something wrong with recursion\n");
    objectdata[n].processbody=t;
    objectdata[n].processstatus=GNF;
    return;
  }

  if (objectdata[n].processstatus==mCRL)
  { objectdata[n].processstatus=mCRLbusy;
    t=procstorealGNFbody(objectdata[n].processbody,first,todo,
             regular,mCRL,objectdata[n].parameters);
    /* if t is not equal to NULL,
       the body of this process is itself a processidentifier */
       
    objectdata[n].processstatus=mCRLdone; 
    return;
  }

  if ((objectdata[n].processstatus==GNFbusy) && (v==first))
  { ATerror("Unguarded recursion in process %t\n",procIdDecl);
  }

  if ((objectdata[n].processstatus==GNFbusy)||
      (objectdata[n].processstatus==GNF)||
      (objectdata[n].processstatus==mCRLdone)||
      (objectdata[n].processstatus==multiAction))
  { return;
  }

  if (objectdata[n].processstatus==mCRLbusy)
  { ATerror("Unguarded recursion without pCRL operators\n");
  }
  
  ATerror("Internal: Strange process type %d\n",objectdata[n].processstatus);  
}

static void procstorealGNF(ATermAppl procsIdDecl, int regular)
{ ATermList todo=ATempty;

  todo=ATinsertA(todo,procsIdDecl);
  for(; (todo!=ATempty) ; )
    { 
      procsIdDecl=ATAgetFirst(todo);
      todo=ATgetNext(todo);
      procstorealGNFrec(procsIdDecl,first,&todo,regular);
    }
}


/**************** GENERATE LPE **********************************/
/*                                                              */
/*                                                              */
/*                                                              */
/*                                                              */
/*                                                              */
/*                                                              */
/****************************************************************/


/**************** Make pCRL procs  ******************************/


static int alreadyinpCRLprocs(ATermAppl procIdDecl,ATermList pCRLprocs)
{ ATermList walker=pCRLprocs;
  for(walker=pCRLprocs ; walker!=ATempty ; walker=ATgetNext(walker))
  { if (ATAgetFirst(walker)==procIdDecl) return 1; 
  }
  return 0;
}

static ATermList LocalpCRLprocs=NULL;

static void makepCRLprocs_rec(ATermAppl t)
{ 
  if (gsIsChoice(t)||gsIsSeq(t))
  { makepCRLprocs_rec(ATAgetArgument(t,0));
    makepCRLprocs_rec(ATAgetArgument(t,1)); 
    return;
  }

  if (gsIsCond(t)||gsIsSum(t))
  { makepCRLprocs_rec(ATAgetArgument(t,1)); 
    return;
  }

  if (gsIsProcess(t)) 
  { t=ATAgetArgument(t,0); /* get procId */
    if (alreadyinpCRLprocs(t,LocalpCRLprocs)) 
    { return;
    }
    LocalpCRLprocs=ATinsertA(LocalpCRLprocs,t);
    makepCRLprocs_rec(objectdata[objectIndex(t)].processbody);
    return;
  }

  if (gsIsMultAct(t)||gsIsDelta(t)||gsIsAtTime(t))
  { 
    return; 
  }

  ATerror("Internal: Unexpected process format %t in makepCRLprocs_rec\n",t);
}

static ATermList makepCRLprocs(ATermAppl t, ATermList pCRLprocs)
{ 
  LocalpCRLprocs=pCRLprocs;
  makepCRLprocs_rec(t);
  return LocalpCRLprocs;
}
/**************** Collectparameterlist ******************************/

static int alreadypresent(ATermAppl *var,ATermList vl,int n)
{ /* Note: variables can be different, although they have the
     same string, due to different types. */

  ATermAppl var1=NULL;

  if (vl==ATempty) return 0;
  var1=ATAgetFirst(vl);

  if (!gsIsDataVarId(var1))
  { ATerror("Expect variablelist %t\n",vl);
  }

  if (*var==var1)
  { return 1;
  }

  vl=ATgetNext(vl);
  return alreadypresent(var,vl,n);
}

static ATermList joinparameters(ATermList par1,ATermList par2,int n)
{ ATermAppl var2=NULL;

  if (par2==ATempty) 
     return par1;
  
  var2=ATAgetFirst(par2);
  assert(gsIsDataVarId(var2));
  par2=ATgetNext(par2);

  if (alreadypresent(&var2,par1,n))
  { return joinparameters(par1,par2,n);
  }
  return ATinsertA(joinparameters(par1,par2,n),var2);
}

static ATermList collectparameterlist(ATermList pCRLprocs)
{ ATermList walker=NULL;
  ATermList parameters=ATempty;
  for (walker=pCRLprocs ; walker!=ATempty ; walker=ATgetNext(walker))
    { long n=objectIndex(ATAgetFirst(walker));
      parameters=joinparameters(parameters,objectdata[n].parameters,n);
    }
  return parameters;
}

/**************** makenewsort  **********************************/

static ATermAppl makenewsort(
                    ATermAppl sortname, 
                    specificationbasictype *spec)
{ ATermAppl newSort=NULL;
  newSort=gsMakeSortId(sortname);
  insertsort(newSort);
  spec->sorts=ATinsertA(spec->sorts,newSort); 
  return newSort;
}

/****************  Declare local datatypes  ******************************/

static ATermAppl makeApplTerm(ATermAppl s, ATermList l)
{ ATermAppl result=s;
  for( ; l!=ATempty ; l=ATgetNext(l))
  { result=gsMakeDataAppl(result,ATAgetFirst(l));
  }
  return result;
}

static ATermList localequationvariables=NULL;

static void declare_equation_variables(ATermList t1)
{ 
  if (localequationvariables!=NULL)
  { ATerror("Cannot declare variables as section is not yet closed %t\n",
                localequationvariables);}
  localequationvariables=t1;
}

static void end_equation_section(void)
{ if (localequationvariables==NULL)
     ATerror("Cannot open an non ended equation section\n",NULL);
  
  localequationvariables=NULL;
}

static void newequation(
                ATermAppl condition,
                ATermAppl t2, 
                ATermAppl t3, 
                specificationbasictype *spec)
{ 
  if (localequationvariables==NULL)
     ATerror("Variables must be declared first! %t\n",t2);
  
  ATermAppl eqn=gsMakeDataEqn(
                     localequationvariables,
                     ((condition==NULL)?gsMakeNil():condition),
                     t2,
                     t3);
  if (mayrewrite) gsRewriteAddEqn(eqn);
  spec->eqns=ATinsertA(spec->eqns,eqn);
}


typedef struct 
   { 
     ATermAppl stacksort;
     ATermList sorts;
     ATermList get;
     ATermAppl push;
     ATermAppl emptystack;
     ATermAppl empty; 
     ATermAppl pop;
     ATermAppl getstate; } stackoperations;

typedef struct stacklisttype
   {
     stackoperations *opns;  
     ATermList parameterlist;      
     ATermAppl stackvar;
     struct stacklisttype *next;
     int no_of_states; 
     /* the boolean state variables occur in reverse
        order, i.e. the least significant first, whereas
        in parameter lists, the order is reversed. */
     ATermList booleanStateVariables; } stacklisttype;

static void declare_control_state(
                specificationbasictype *spec, 
                ATermList pCRLprocs)
{ int i=0;
  for(i=0 ; pCRLprocs!=ATempty; i++)
  { pCRLprocs=ATgetNext(pCRLprocs);
  }
  /* i is the number of states */
  create_enumeratedtype(i,spec);
}  

/* All datatypes for different stacks that are being generated
   are stored in the following list, such that it can be investigated
   whether a suitable stacktype already exist, before generating a new
one */
   
static stacklisttype *stacklist=NULL;   

static void makepushargsvarsrec(
               ATermList *t1,
               ATermList *t2, 
               ATermList sorts)
{ ATermAppl v=NULL;
  ATermAppl sort=NULL;

  if (sorts==ATempty)
  { return;
  }
  makepushargsvarsrec(t1,t2,ATgetNext(sorts));
  
  sort=ATAgetFirst(sorts);
  v=getfreshvariable("v",sort);
  *t1=ATinsertA(*t1,v);
  *t2=ATinsertA(*t2,v);
  
}

static void makepushargsvars(
         ATermAppl *t1,
         ATermList *t2, 
         ATermAppl var0, 
         stacklisttype *stack)
{ /* t1 contains push(var0,v1,..,vn,stackvar), t2 the list of variables
     state, var0,v1,...,vn,stackvar */

  ATermList t=ATempty, v=ATempty;
  
  t=ATinsertA(ATempty,stack->stackvar);
  v=ATinsertA(ATempty,stack->stackvar);

  makepushargsvarsrec(&t,&v,stack->opns->sorts);
  *t1=makeApplTerm(stack->opns->push,ATinsertA(t,var0));
  *t2=ATinsertA(v,var0);
}

static int matchsorts(ATermList p1,ATermList p2)
{ 
  for( ; p1!=ATempty ; p1=ATgetNext(p1))
  { if (p2==ATempty) return 0;
    if (ATAgetArgument(ATAgetFirst(p1),1)!=
            ATAgetArgument(ATAgetFirst(p2),1))
    { return 0;
    }
    p2=ATgetNext(p2);
  }

  if (p2==ATempty)
     return 1;
  return 0;
}

static stackoperations *find_suitable_stack_operations(
                           ATermList parameters,
                           stacklisttype *stacklist)
{ if (stacklist==NULL) 
  { return NULL; 
  }
  if (matchsorts(parameters, stacklist->parameterlist))
  { return stacklist->opns; 
  }
  return find_suitable_stack_operations(parameters,stacklist->next);
}

stacklisttype *new_stack(
                 ATermList parameterlist,
                 specificationbasictype *spec, 
                 int regular,
                 ATermList pCRLprocs)
{ ATermList walker=NULL; 
  ATermAppl tempsorts=NULL;
  ATermAppl var0=NULL;

  ATermAppl t1=NULL;
  ATermList t2=NULL;
  
  char* s3;
  stacklisttype *stack;
  int no_of_states=0,i=0;
  ATermList last=ATempty;

  for(no_of_states=0 ; pCRLprocs!=ATempty ; pCRLprocs=ATgetNext(pCRLprocs)) 
  {
    no_of_states++;
    last = pCRLprocs;
  }

  s3 = (statenames ? ATSgetArgument(ATAgetFirst(last),0) : "s");
  
  stack=malloc(sizeof(stacklisttype));
  if (stack==NULL)
     ATerror("Cannot allocate memory for stack data\n");
  stack->parameterlist=ATempty; 
  ATprotect((ATerm *)&stack->parameterlist); 
  stack->parameterlist=parameterlist;
  stack->no_of_states=no_of_states;
  stack->booleanStateVariables=ATempty;
  ATprotect((ATerm *)&stack->booleanStateVariables);
  if ((binary==1) && (oldstate==0))
  { i=upperpowerof2(no_of_states);
    for( ; i>0 ; i--)
    { ATermAppl name=gsMakeDataVarId(fresh_name("bst"),
                                     gsMakeSortExprBool());
      insertvariable(name,1);
      stack->booleanStateVariables=
           ATinsertA(stack->booleanStateVariables,name);
    }
  }
  stack->next=stacklist;
  
  if (regular)
  { stack->opns=NULL;
    stack->stackvar=gsMakeDataVarId(fresh_name(s3),
                                    gsMakeSortExprPos());
    insertvariable(stack->stackvar,1);
  }
  else  
  { stack->opns=find_suitable_stack_operations(parameterlist,stacklist);
    stacklist=stack;

    if (stack->opns!=NULL)
    { stack->stackvar=gsMakeDataVarId(fresh_name(s3),
                                      stack->opns->stacksort);
      insertvariable(stack->stackvar,1);
    }
    else 
    { /* stack->opns == NULL */
      stack->opns=malloc(sizeof(stackoperations));
      if (stack->opns==NULL)
          ATerror("Cannot allocate memory for stack operations\n");

      stack->opns->sorts=ATempty;
      ATprotect((ATerm *)&(stack->opns->sorts));
      stack->opns->get=NULL;
      ATprotect((ATerm *)&(stack->opns->get));
      stack->opns->stacksort=makenewsort(fresh_name("Stack"),spec);
      stack->stackvar=gsMakeDataVarId(fresh_name(s3),
                                      stack->opns->stacksort);
      insertvariable(stack->stackvar,1);
      stack->opns->sorts=ATempty;
      stack->opns->get=ATempty;
      for( walker=parameterlist ; 
           walker!=ATempty ; 
           walker=ATgetNext(walker)) 
      { ATermAppl par=ATAgetFirst(parameterlist);
        ATermAppl sort=ATAgetArgument(par,1);
        ATermAppl getmap=NULL;
        assert(gsIsDataVarId(par));
        stack->opns->sorts=ATinsertA(
                  stack->opns->sorts,
                  sort);
        snprintf(scratch1,STRINGLENGTH,"get%s",ATSgetArgument(par,0));

        getmap=gsMakeOpId(fresh_name(scratch1),
                   gsMakeSortArrow(stack->opns->stacksort,sort));
        insertmapping(getmap,spec);
        stack->opns->get=ATinsertA(stack->opns->get,getmap);
      }
      /* reverse stack->get */
      stack->opns->get=ATreverse(stack->opns->get);

      /* construct the sort of the push operator */
      walker=ATreverse(stack->opns->sorts);
      for (tempsorts=stack->opns->stacksort; 
           walker!=ATempty ; walker=ATgetNext(walker))
      { tempsorts=gsMakeSortArrow(ATAgetFirst(walker),tempsorts);
      }

      if (oldstate)
      { tempsorts=gsMakeSortArrow(gsMakeSortExprPos(),tempsorts);
      }
      else if (binary)
      { ATerror("Cannot combine stacks with binary\n");
      }
      else /* enumerated type */
      { ATerror("Cannot combine stacks with an enumerated type\n");
      }

      /* XX insert equations for get mappings */

      if (oldstate)
      { stack->opns->getstate=
             gsMakeOpId(fresh_name("getstate"),
                        gsMakeSortArrow(
                           stack->opns->stacksort,
                           gsMakeSortExprPos())); 
        insertmapping(stack->opns->getstate,spec);
      }
      else if (binary)
      { ATerror("Cannot combine stacks with binary\n");
      }
      else /* enumerated type */
      { ATerror("Cannot combine stacks with an enumerated type\n");
      }

      stack->opns->push=
              gsMakeOpId(fresh_name("push"),tempsorts);
      insertconstructor(stack->opns->push,spec);
      stack->opns->emptystack=
              gsMakeOpId(fresh_name("emptystack"),
                         stack->opns->stacksort);
      insertconstructor(stack->opns->emptystack,spec);

      stack->opns->empty=
              gsMakeOpId(
                 fresh_name("isempty"),
                 gsMakeSortArrow(stack->opns->stacksort,gsMakeSortExprBool()));
      insertmapping(stack->opns->empty,spec);

      stack->opns->pop=
           gsMakeOpId(
               fresh_name("pop"),
               gsMakeSortArrow(stack->opns->stacksort,
                               stack->opns->stacksort));
      insertmapping(stack->opns->pop,spec);

  
      /* Generate equations for get, pop and isempty */
      var0=getfreshvariable("svr",gsMakeSortExprPos());
      makepushargsvars(&t1,&t2,var0,stack);
      declare_equation_variables(t2);
      newequation(
              NULL,
              gsMakeDataAppl(stack->opns->empty,stack->opns->emptystack),
              gsMakeDataExprTrue(),
              spec);
      /* t1 contains push(var0,v1,..,vn,stackvar), t2 the list of variables
         state, var0,v1,...,vn,stackvar*/
      newequation(
              NULL,
              gsMakeDataAppl(stack->opns->empty,t1),
              gsMakeDataExprFalse(),
              spec);
      newequation(
              NULL,
              gsMakeDataAppl(stack->opns->pop,t1),
              stack->stackvar,
              spec);
      newequation(
              NULL,
              gsMakeDataAppl(stack->opns->getstate,t1),
              var0,
              spec);

      t2=ATgetNext(t2);
      for(walker=stack->opns->get;
          walker!=ATempty;
          walker=ATgetNext(walker))
      { 
        newequation(
              NULL,
              gsMakeDataAppl(ATAgetFirst(walker),t1), 
              ATAgetFirst(t2),
              spec);
        t2=ATgetNext(t2);
      }
      end_equation_section();
    }
  }
  
  return stack;
}

static ATermAppl getvar(ATermAppl var,stacklisttype *stack, specificationbasictype *spec)
{ ATermList walker=NULL; 
  ATermList getmappings=stack->opns->get;
  

  /* first search whether the variable is a free process variable */  

  for(walker=spec->procdatavars ;
               walker!=ATempty ; walker=ATgetNext(walker))
  { if (ATAgetFirst(walker)==var)
    { return var;
    }
  }

  /* otherwise find out whether the variable matches a parameter */

  for(walker=stack->parameterlist ;
        walker!=ATempty ; walker=ATgetNext(walker))
  { if (ATAgetFirst(walker)==var)
    { return gsMakeDataAppl(ATAgetFirst(getmappings),stack->stackvar); 
    }
    assert(getmappings!=ATempty);
    getmappings=ATgetNext(getmappings);
  }
  assert(0); /* Hier zou je niet mogen komen, omdat dat 
                gezocht wordt naar een niet bestaande variabele */
  return var;
} 

ATermList sumlist=NULL;

/* static ATerm processencoding_rec(int i)
{ ATerm t3=NULL;
  
  if (i==1) 
     t3=ATmake("t(<str>,emt))",controlstate->one);
  else
   {if ((i % 2)==0)
       t3=ATmake("t(<str>,ins(<term>,emt))",controlstate->x2plus0,
              processencoding_rec(i/2));
    else
       t3=ATmake("t(<str>,ins(<term>,emt))",controlstate->x2plus1,
               processencoding_rec((i-1)/2));
   }
  return t3; 
} */

static ATermList processencoding(
                     int i,
                     ATermList t,
                     specificationbasictype *spec,
                     stacklisttype *stack)
{
  if (oldstate)
  { snprintf(scratch1,STRINGLENGTH,"%d",i);
    return ATinsertA(t,gsMakeDataExprPos(scratch1));
    /* return ATmake("ins(<term>,<term>)",processencoding_rec(i),t); */
  }

  i=i-1; /* below we count from 0 instead from 1 as done in the 
            first version of the prover */

  if (binary==0)
  { ATermList l=NULL;
    enumeratedtype *e=NULL;
    e=create_enumeratedtype(stack->no_of_states,spec);
    l=e->elementnames;
    for( ; i>0 ; i--){l=ATgetNext(l);}
    return ATinsertA(t,ATAgetFirst(l));
  }
  /* else a sequence of boolean values needs to be generated,
     representing the value i, when there are l->n elements */
  { 
    int k=upperpowerof2(stack->no_of_states);
    for( ; k>0 ; k--)
    { if ((i % 2)==0)
      { t=ATinsertA(t,gsMakeDataExprFalse());
        i=i/2;
      }
      else 
      { t=ATinsertA(t,gsMakeDataExprTrue());
        i=(i-1)/2;
      }
    }
    return t;  
  }
}

static ATermAppl correctstatecond(
              ATermAppl procId, 
              ATermList pCRLproc,
              stacklisttype *stack, 
              int regular, 
              specificationbasictype *spec)
{ 
  int i;
  ATermAppl t3=NULL;
  ATermList vars=NULL;
 
  for(i=1 ; ATAgetFirst(pCRLproc)!=procId ; pCRLproc=ATgetNext(pCRLproc))
  { i++; }
  /* i is the index of the current process */

  if (oldstate)
  { if (regular)
    { return gsMakeDataExprEq(stack->stackvar,
               ATAgetFirst(processencoding(i,ATempty,spec,stack)));
    }
    return gsMakeDataExprEq(
             gsMakeDataAppl(stack->opns->getstate,stack->stackvar),
             ATAgetFirst(processencoding(i,ATempty,spec,stack)));
  } 

  if (binary==0) /* Here a state encoding using enumerated types
                    must be declared */
  { create_enumeratedtype(stack->no_of_states,spec); 
    if (regular)
    { return gsMakeDataExprEq(
                  stack->stackvar,
                  ATAgetFirst(processencoding(i,ATempty,spec,stack)));
    }
    return gsMakeDataExprEq(
             gsMakeDataAppl(stack->opns->getstate, stack->stackvar),
             ATAgetFirst(processencoding(i,ATempty,spec,stack)));
  }
  
  /* in this case we must encode the condition using
     boolean variables */

  vars=stack->booleanStateVariables;

  i=i-1; /* start counting from 0, instead from 1 */
  for(  ; !ATisEmpty(vars) ; vars=ATgetNext(vars))
  { if ((i % 2)==0)
    { if (t3==NULL)
      { t3=gsMakeDataExprNot(ATAgetFirst(vars));
      }
      else 
      { t3=gsMakeDataExprAnd(
             gsMakeDataExprNot(ATAgetFirst(vars)),
             t3);
      }
      i=i/2;
    }
    else
    { if (t3==NULL)
      { t3=ATAgetFirst(vars);
      }
      else 
      { t3=gsMakeDataExprAnd(ATAgetFirst(vars),t3);
      }
      i=(i-1)/2;
    }

  }
  assert(i==0);
  return t3;
}

static ATermAppl adapt_term_to_stack(
                 ATermAppl t, 
                 stacklisttype *stack,
                 ATermList vars,
                 specificationbasictype *spec)
{ 
  if (gsIsOpId(t)) return t;

  if (gsIsDataVarId(t))
  { if (ATindexOf(vars,(ATerm)t,0)>=0)
    { /* t occurs in vars, so, t does not have to be reconstructed
         from the stack */
      return t;
    }
    else return getvar(t,stack,spec); }

  if (gsIsDataAppl(t))
  { return gsMakeDataAppl(
            adapt_term_to_stack(ATAgetArgument(t,0),stack,vars,spec),
            adapt_term_to_stack(ATAgetArgument(t,1),stack,vars,spec));
  }
              
  ATerror("Expect a term\n");
  return NULL;
}

static ATermList adapt_termlist_to_stack(
                  ATermList tl, 
                  stacklisttype *stack, 
                  ATermList vars,
                  specificationbasictype *spec)
{ 
  if (tl==ATempty)
  { return ATempty;
  }

  return ATinsertA(
          adapt_termlist_to_stack(ATgetNext(tl),stack,vars,spec),
          adapt_term_to_stack(ATAgetFirst(tl),stack,vars,spec));
}

static ATermList adapt_multiaction_to_stack_rec(
                   ATermList multiAction,
                   stacklisttype *stack,
                   ATermList vars,
                   specificationbasictype *spec)
{ ATermAppl action=NULL;
  if (multiAction==ATempty)
  { return ATempty;
  }

  action=ATAgetFirst(multiAction);
  assert(gsIsAction(action));

  return ATinsertA(
            adapt_multiaction_to_stack_rec(ATgetNext(multiAction),stack,vars,spec),
            gsMakeAction(
                  ATAgetArgument(action,0),
                  adapt_termlist_to_stack(
                           ATLgetArgument(action,1),
                           stack,
                           vars,spec)));
}

ATermAppl adapt_multiaction_to_stack(
                   ATermAppl multiAction,
                   stacklisttype *stack,
                   ATermList vars,
                   specificationbasictype *spec)
{
  if (multiAction==gsMakeDelta())
  { return multiAction;
  }

  assert(gsIsMultAct(multiAction));
  return gsMakeMultAct(
           adapt_multiaction_to_stack_rec(
               ATLgetArgument(multiAction,0),
               stack,
               vars,spec));
}

static ATermAppl find(
               ATermAppl s, 
               ATermList pars, 
               ATermList args, 
               stacklisttype *stack,
               ATermList vars, 
               int regular,
               specificationbasictype *spec)
{ /* We generate the value for variable s in the list of
     the parameters of the process. If s is equal to some
     variable in pars, it is an argument of the current 
     process, and it must be replaced by the corresponding
     argument in args.
       If s does not occur in pars, it must be replaced
     by a dummy value.
  */

  long n=ATindexOf(pars,(ATerm)s,0);
  ATermAppl result=NULL;
  if (n>=0)
  { result=(ATermAppl)ATelementAt(args,n);
  }
  else
  { result=dummyterm(ATAgetArgument(s,1),spec); 
  /*result=((regular)?newProcDataVar(ATAgetArgument(s,1),spec):dummyterm(ATAgetArgument(s,1),spec));  */
  }
 
  if (regular)
  { 
    return result;
  }
  return adapt_term_to_stack(result,stack,vars,spec);
}


static ATermList findarguments(
                   ATermList pars,
                   ATermList parlist, 
                   ATermList args,
                   ATermList t2,
                   stacklisttype *stack, 
                   ATermList vars, 
                   int regular,
                   specificationbasictype *spec)
{ ATermAppl string1term=NULL;
  
  if (parlist==ATempty)
  { return t2; 
  }

  string1term=ATAgetFirst(parlist);
  parlist=ATgetNext(parlist);

  return ATinsertA(
            findarguments(pars,parlist,args,t2,stack,vars,regular,spec),
            find(string1term,pars,args,stack,vars,regular,spec));
}


static ATermList push(
              ATermAppl procId,
              ATermList args,
              ATermList t2,
              stacklisttype *stack,
              ATermList pCRLprcs, 
              ATermList vars,
              int regular,
              int singlestate,
              specificationbasictype *spec)
{ 
  int i;
  ATermList t=NULL;
  
  t=findarguments(objectdata[objectIndex(procId)].parameters, 
            stack->parameterlist,args,t2,stack,vars,regular,spec);

  for(i=1 ; ATAgetFirst(pCRLprcs)!=procId ; pCRLprcs=ATgetNext(pCRLprcs))
  { i++; 
  }

  if (regular)   
  { if (singlestate)
    { return t;
    }
    return processencoding(i,t,spec,stack); 
  }

  return ATinsertA(
            ATempty,makeApplTerm(
                        stack->opns->push,
                        processencoding(i,t,spec,stack))); 
}


static ATermList make_procargs(
                    ATermAppl t,
                    stacklisttype *stack,
                    ATermList pcrlprcs,
                    ATermList vars, 
                    int regular, 
                    int singlestate,
                    specificationbasictype *spec)
{ /* t is a sequential composition of process variables */
  ATermList t1=NULL;
  ATermAppl t2=NULL; 
  ATermList t3=NULL;
  ATermAppl process=NULL;
  ATermAppl procId=NULL;

  if (gsIsSeq(t))   
  { if (regular)
    { ATerror("Process is not regular, as it has stacking vars %t\n",t);
    }
    process=ATAgetArgument(t,0);
    t2=ATAgetArgument(t,1);
    assert(gsIsProcess(process)); 
    procId=ATAgetArgument(process,0);
    t1=ATLgetArgument(process,1);
    assert(gsIsProcVarId(procId));

    if (objectdata[objectIndex(procId)].canterminate==1)
    { t3=make_procargs(t2,stack,pcrlprcs,
                        vars,regular,singlestate,spec);
      t3=push(procId,t1,t3,stack,pcrlprcs,vars,regular,singlestate,spec);
      return ATinsertA(ATempty,ATAgetFirst(t3));
    }

    t3=push(procId,t1,ATinsertA(ATempty,stack->opns->emptystack),
                 stack,pcrlprcs,vars,regular,singlestate,spec);
    return ATinsertA(ATempty,ATAgetFirst(t3));
  } 
  
  if (gsIsProcess(t)) 
  { procId=ATAgetArgument(t,0);
    t1=ATLgetArgument(t,1);
    assert(gsIsProcVarId(procId));

    if (regular)
    { return push(procId,
                  t1,
                  ATempty,
                  stack,
                  pcrlprcs,
                  vars,
                  regular,
                  singlestate,
                  spec);
    }
    if (objectdata[objectIndex(procId)].canterminate==1)
    { t3=push(procId,
              t1,
              ATinsertA(ATempty,
                        gsMakeDataAppl(stack->opns->pop,stack->stackvar)),
              stack,
              pcrlprcs,
              vars,
              regular,
              singlestate,
              spec);
      return ATinsertA(ATempty,ATAgetFirst(t3));
    }
    t3= push(procId,
             t1,
             ATinsertA(ATempty,stack->opns->emptystack),
             stack,
             pcrlprcs,
             vars,
             regular,
             singlestate,
             spec);
    return ATinsertA(ATempty,ATAgetFirst(t3));
  }
  
  ATerror("Expect seq or name %t\n",t);
  return NULL;
}

static int occursin(ATermAppl name,ATermList pars)
{ return (ATindexOf(pars,(ATerm)name,0)>=0);
}

static ATermAppl dummyterm(
                    ATermAppl targetsort, 
                    specificationbasictype *spec)
{ /* This procedure yields a term of the requested sort.
     First, it tries to find a constant constructor. If it cannot
     be found, a constant mapping is sought. If this cannot be
     found a new dummy constant mapping of the requested sort is made. */

  if (allowFreeDataVariablesInProcesses)
  { ATermAppl newVariable=gsMakeDataVarId(fresh_name("freevar"),targetsort);
    spec->procdatavars=ATinsertA(spec->procdatavars,newVariable);
    insertvariable(newVariable,1);
    return newVariable;
  }
  
  /* First search for a constant constructor */

  for (int i=0 ; (i<maxobject) ; i++ )
  { if ((objectdata[i].object==func)&&
        (ATAgetArgument(objectdata[i].objectname,1)==targetsort))
    { return objectdata[i].objectname;
    }
  }

  /* Second search for a constant mapping */

  for (int i=0 ; (i<maxobject) ; i++ )
  { if ((objectdata[i].object==map)&&
        (ATAgetArgument(objectdata[i].objectname,1)==targetsort))
    { return objectdata[i].objectname;
    }
  }

  /* Third construct a new constant, and yield it. */

  snprintf(scratch1,STRINGLENGTH,"dummy%s",gsATermAppl2String(ATAgetArgument(targetsort,0)));
  ATermAppl dummymapping=gsMakeOpId(fresh_name(scratch1),targetsort);
  insertmapping(dummymapping,spec);
  return dummymapping;
  
}

static ATermList pushdummyrec(
                     ATermList totalpars, 
                     ATermList pars, 
                     stacklisttype *stack, 
                     int regular,
                     specificationbasictype *spec)
{ /* totalpars is the total list of parameters of the
     aggregated pCRL process. The variable pars contains
     the list of all variables occuring in the initial
     process. This means all variables that occur in
     totalpars, but not in pars can be set to a default
     value, which is nil, created by gsMakeNil(). */

  if (totalpars==ATempty)
  { if (regular)
    { return ATempty;
    }
    return ATinsertA(ATempty,stack->opns->emptystack);
  }
  
  ATermAppl par=ATAgetFirst(totalpars);
  totalpars=ATgetNext(totalpars);

  if (occursin(par,pars))
  { return ATinsertA(
               pushdummyrec(totalpars,pars,stack,regular,spec),
               par);
  }
  /* otherwise the value of this argument is irrelevant, so
     make it Nil, if a regular translation is made. If a translation
     with stacks is made, then yield a default `unique' term. */
  return ATinsertA(
             pushdummyrec(totalpars,pars,stack,regular,spec),
             /* ((regular)?gsMakeNil():dummyterm(ATAgetArgument(par,1),spec))); */
                                    dummyterm(ATAgetArgument(par,1),spec)); 
}

static ATermList pushdummy(
                     ATermList parameters,
                     stacklisttype *stack, 
                     int regular,
                     specificationbasictype *spec)
{ 
  return pushdummyrec(stack->parameterlist,
              parameters,stack,regular,spec);
}

static ATermList make_initialstate(
                     ATermAppl initialProcId,
                     stacklisttype *stack,
                     ATermList pcrlprcs, 
                     int regular, 
                     int singlecontrolstate,
                     specificationbasictype *spec)
{ ATermList t=NULL;
  int i;
   
  for(i=1 ; ATAgetFirst(pcrlprcs)!=initialProcId ; 
                               pcrlprcs=ATgetNext(pcrlprcs))
  { i++; }
  /* i is the index of the initial state */

  t=pushdummy(objectdata[objectIndex(initialProcId)].parameters,stack,regular,spec);

  if (regular)
  { if (singlecontrolstate)
    { return t;
    }
    return processencoding(i,t,spec,stack); 
  }
  return ATinsertA(ATempty,
                   gsMakeDataApplList(
                        stack->opns->push,
                           processencoding(i,t,spec,stack)));
}

/*************************  Routines for summands  **************************/

static ATermList dummyparameterlist(stacklisttype *stack, ATbool singlestate)
{ if (singlestate)
  { return stack->parameterlist;
  }

  return ATinsertA(stack->parameterlist,stack->stackvar);
}

static int identicalActionIds_rec(ATermList ma1, ATermList ma2)
{
  if (ma1==ATempty)
  { return ma2==ATempty;
  }

  if (ma2==ATempty)
  return 0;

  if (ATAgetArgument(ATAgetFirst(ma1),0)==
        ATAgetArgument(ATAgetFirst(ma1),0))
  { return identicalActionIds_rec(ATgetNext(ma1),
                              ATgetNext(ma2));
  }

  return 0;
}

static int identicalActionIds(ATermAppl ma1, ATermAppl ma2)
{ 
  if (gsIsDelta(ma1))
  { return gsIsDelta(ma2);
  }

  if (gsIsDelta(ma2))
  { return 0;
  }

  return identicalActionIds_rec(ATLgetArgument(ma1,0),ATLgetArgument(ma2,0));
}

static ATermList insert_summand(
                    ATermList sumlist, 
                    ATermList sumvars, 
                    ATermAppl condition,
                    ATermAppl multiAction,
                    ATermAppl actTime,
                    ATermList procargs) 
{ /* insert a new summand in sumlist; first try whether there is already
     a similar summand, such that this summand can be added with minimal
     increase of size. Otherwise add a fully new summand. Note that by
     a more careful matching, the number of summands can be reduced.

     Note also that a terminated term is indicated by taking procargs
     equal to NULL. */

  ATermList newsumlist=ATempty; 
  if (gsIsDelta(multiAction) && gsIsNil(actTime))
  { return sumlist;
  }

  for(newsumlist=ATempty ;
        sumlist!=ATempty ; sumlist=ATgetNext(sumlist) )
  {
    ATermAppl summand=ATAgetFirst(sumlist);
    ATermAppl multiAction1=linGetMultiAction(summand);

    if (identicalActionIds(multiAction,multiAction1))

    { ATermList renamingvariablelist=ATempty;
      ATermList renamingtermlist=ATempty;
      ATermList sumvars1=linGetSumVars(summand);

      if (variablesequal(sumvars,sumvars1, 
                    &renamingvariablelist,&renamingtermlist))
      { ATermList procargs1=linGetNextState(summand);
        ATermList procargs2=substitute_datalist(
                                        renamingtermlist,
                                        renamingvariablelist,
                                        procargs);
        if (procargs1==procargs2)
        { ATermAppl actTime1=linGetActionTime(summand);
          ATermAppl actTime2=NULL;
          if (gsIsNil(actTime))
          { actTime2=actTime;
          }
          else 
          { actTime2=substitute_data(
                         renamingtermlist,
                         renamingvariablelist,
                         actTime);
          }
          if (actTime1==actTime2)
          { ATermAppl multiAction2=substitute_multiaction(
                                   renamingtermlist,
                                   renamingvariablelist,
                                   multiAction); 
            if (multiAction1==multiAction2)
            { ATermAppl condition1=linGetCondition(summand);
              ATermAppl condition2=gsMakeDataExprOr(
                                        condition1,
                                        substitute_data(
                                              renamingtermlist,
                                              renamingvariablelist,
                                              condition));
              return ATinsertA(ATconcat(newsumlist,ATgetNext(sumlist)),
                               gsMakeLPESummand(
                                         sumvars1,
                                         condition2,
                                         multiAction1,
                                         actTime1,
                                         procargs1));
    } } } } }
    newsumlist=ATinsertA(newsumlist,summand);
  } 

  /* There is no matching summand in sumlist. So, we add the summand
     at the beginning */
  
  return ATinsertA(newsumlist,
                   gsMakeLPESummand(
                         sumvars,
                         condition,
                         multiAction,
                         actTime,
                         procargs)); 
  
}

static void add_summands(
               ATermAppl procId,
               ATermAppl summandterm, 
               ATermList pars, 
               ATermList pCRLprocs, 
               stacklisttype *stack,
               int canterminate, 
               int regular,
               int singlestate, 
               specificationbasictype *spec)
{ ATermList sumvars=ATempty;
  ATermAppl atTime=NULL;
  ATermAppl multiAction=NULL;
  ATermList procargs=NULL;
  ATermAppl condition1=NULL,condition2=NULL;
  ATermAppl emptypops=NULL, notemptypops=NULL;


  /* remove the sum operators; collect the sum variables in the
     list sumvars */
  for( ; gsIsSum(summandterm) ; )
  { sumvars=ATconcat(ATLgetArgument(summandterm,0),sumvars);
    summandterm=ATAgetArgument(summandterm,1);
  }
  
  /* translate the condition */       
  

  if ((regular)&&(singlestate))
  { condition1=gsMakeDataExprTrue();
  }
  else 
  { condition1=correctstatecond(procId,pCRLprocs,stack,regular,spec);
  }
  
  for( ; (gsIsCond(summandterm)) ; )
  { 
    ATermAppl localcondition=ATAgetArgument(summandterm,0);
    if (!((regular)&&(singlestate)))
    { condition1=gsMakeDataExprAnd(
                     condition1,
                     ((regular)?localcondition:      
                                adapt_term_to_stack(
                                       localcondition,
                                       stack,
                                       sumvars,spec)));
    }
    else
    { /* regular and singlestate */
      condition1=gsMakeDataExprAnd(localcondition,condition1);
    }
    assert(gsIsDelta(ATAgetArgument(summandterm,2)));
    summandterm=ATAgetArgument(summandterm,1);
  }


  if (gsIsSeq(summandterm)) 
  { /* only one summand is needed */
    ATermAppl t1=ATAgetArgument(summandterm,0);
    ATermAppl t2=ATAgetArgument(summandterm,1);
    if (gsIsAtTime(t1))
    { atTime=ATAgetArgument(t1,1);
      t1=ATAgetArgument(t1,0);
    }
    else
    { atTime=gsMakeNil();
    }

    if (t1==gsMakeDelta())
    { multiAction=gsMakeDelta();
    }
    else if (gsIsTau(t1))
    { multiAction=gsMakeMultAct(ATempty);
    }
    else if (gsIsAction(t1))
    { multiAction=gsMakeMultAct(ATinsertA(ATempty,t1));
    }
    else 
    { multiAction=t1;
    }

    procargs=make_procargs(t2,stack,
                pCRLprocs,sumvars,regular,singlestate,spec);
    if (!regular)
    { if (!gsIsDelta(multiAction))
      { multiAction=adapt_multiaction_to_stack(
                      multiAction,stack,sumvars,spec);          
      }
      if (!gsIsNil(atTime))
      { atTime=adapt_term_to_stack(
                      atTime,stack,sumvars,spec);
      }
    }            
    sumlist=insert_summand(sumlist,
                   sumvars,RewriteTerm(condition1),multiAction,atTime,procargs);
    return;
  }

  /* There is a single initial multiaction or deadlock, possibly timed*/

  if (gsIsAtTime(summandterm))
  { atTime=ATAgetArgument(summandterm,1);
    summandterm=ATAgetArgument(summandterm,0);
  } 
  else
  { atTime=gsMakeNil();
  } 
    
  if (gsIsDelta(summandterm))
  { multiAction=gsMakeDelta();
  } 
  else if (gsIsTau(summandterm))
  { multiAction=gsMakeMultAct(ATempty);
  } 
  else if (gsIsAction(summandterm))
  { multiAction=gsMakeMultAct(ATinsertA(ATempty,summandterm));
  } 
  else if (gsIsMultAct(summandterm))
  { multiAction=summandterm;
  } 
  else ATerror("Internal: Expected multiaction %t\n",summandterm);

  if (regular)
  { if (!gsIsDelta(multiAction))
    { ATerror("Internal: with the flag regular terminating processes should not exist\n");
    }
    sumlist=insert_summand(sumlist,
                   sumvars,
                   RewriteTerm(condition1),
                   multiAction,
                   atTime,
                   dummyparameterlist(stack,singlestate));
    return;
  }

  /* in this case we have two possibilities: the process
     can or cannot terminate after the action. So, we must
     generate two conditions. For regular processes, we assume
     that processes do not terminate */
  /* first we generate the non terminating summands */

            
  if (canterminate==1)
  { emptypops=gsMakeDataAppl(stack->opns->empty,
                   gsMakeDataAppl(stack->opns->pop,stack->stackvar));
    notemptypops=gsMakeDataExprNot(emptypops);
    condition2=gsMakeDataExprAnd(notemptypops,condition1); 
  }
  else condition2=condition1;

  multiAction=adapt_multiaction_to_stack(multiAction,stack,sumvars,spec);
  procargs=ATinsertA(ATempty,gsMakeDataAppl(stack->opns->pop,stack->stackvar));

  sumlist=insert_summand(sumlist,
                    sumvars,
                    RewriteTerm(condition2),
                    multiAction,
                    atTime,
                    procargs);
       
  if (canterminate==1)
  { condition2=gsMakeDataExprAnd(emptypops,condition1); 
    sumlist=insert_summand(sumlist,
                  sumvars,
                  RewriteTerm(condition2),
                  multiAction,
                  atTime,
                  NULL); 
  }
 
  return;
}


static void collectsumlistterm(
                 ATermAppl procId, 
                 ATermAppl body, 
                 ATermList pars, 
                 stacklisttype *stack,
                 int canterminate,
                 int regular,
                 int singlestate,
                 specificationbasictype *spec,
                 ATermList pCRLprocs)
{ 
  if (gsIsChoice(body))
  { ATermAppl t1=ATAgetArgument(body,0);
    ATermAppl t2=ATAgetArgument(body,1);

    collectsumlistterm(procId,t1,pars,stack,
                 canterminate,regular,singlestate,spec,pCRLprocs);
    collectsumlistterm(procId,t2,pars,stack,
                 canterminate,regular,singlestate,spec,pCRLprocs);
    return;
  }
  else 
  { add_summands(procId,body,pars,pCRLprocs,stack,
                 canterminate,regular,singlestate,spec);
  }   
}

static ATermList collectsumlist(
                 ATermList pCRLprocs, 
                 ATermList pars, 
                 stacklisttype *stack,
                 int canterminate, 
                 int regular, 
                 int singlestate,
                 specificationbasictype *spec)
{ ATermList walker=NULL;
  
  sumlist=ATempty;

  for(walker=pCRLprocs ; walker!=ATempty ; walker=ATgetNext(walker))
  { ATermAppl procId=ATAgetFirst(walker);
    collectsumlistterm(
              procId,
              objectdata[objectIndex(procId)].processbody,
              pars,
              stack,
              (canterminate&&objectdata[objectIndex(procId)].canterminate),
              regular,
              singlestate,
              spec,
              pCRLprocs);
  }
  
  return sumlist;
}


/**************** Cluster Actions **********************************/


static enumeratedtype *create_enumeratedtype
                (int n,specificationbasictype *spec)
{ enumeratedtype *w=NULL;
  int j=0;
  ATermAppl equalitymapping=NULL;


  for(w=enumeratedtypelist; ((w!=NULL)&&(w->size!=n));
                w=w->next){};
  if (w==NULL)
  { w=malloc(sizeof(enumeratedtype));
    w->sortId=NULL;
    ATprotect((ATerm *)&(w->sortId));
    w->elementnames=NULL;
    ATprotect((ATerm *)&(w->elementnames));
    w->functions=NULL;
    ATprotect((ATerm *)&(w->functions));

    w->size=n;
    if (n==2)
    { w->sortId=gsMakeSortExprBool(); 
      w->elementnames=ATinsertA(ATinsertA(ATempty,
                           gsMakeDataExprFalse()),gsMakeDataExprTrue());
    }
    else 
    { snprintf(scratch1,STRINGLENGTH,"Enum%d",n);
      w->sortId=makenewsort(fresh_name(scratch1),spec); 
      w->elementnames=ATempty;
      /* enumeratedtypes[i].elements=ATmake("ems"); */
      for(j=0 ; (j<n) ; j++)
      { /* Maak hier een naamlijst van sort elementen. */
        ATermAppl constructor=NULL;
        snprintf(scratch1,STRINGLENGTH,"e%d-%d",j,n);
        constructor=gsMakeOpId(fresh_name(scratch1),w->sortId);
        insertconstructor(constructor,spec);
        w->elementnames=ATinsertA(w->elementnames,constructor);
      }
      
      equalitymapping=gsMakeOpIdEq(w->sortId);
      insertmapping(equalitymapping,spec);
      { /* Generate equations */
        ATermList l1=NULL, l2=NULL;
        ATermAppl v=getfreshvariable("v_enum",w->sortId);

        declare_equation_variables(ATinsertA(ATempty,v));
        newequation(NULL,
                    gsMakeDataExprEq(v,v),
                    gsMakeDataExprTrue(),spec);
        for(l1=w->elementnames ; l1!=ATempty ; l1=ATgetNext(l1))
        { for(l2=w->elementnames ; l2!=ATempty ; l2=ATgetNext(l2))
          { ATermAppl el1=ATAgetFirst(l1);
            ATermAppl el2=ATAgetFirst(l2);
            if (el1!=el2)
            { 
              newequation(NULL,
                          gsMakeDataExprEq(el1,el2),
                          gsMakeDataExprFalse(),spec);
            }
          }
        }
        end_equation_section();
      }
    }
    w->functions=ATempty;
    w->next=enumeratedtypelist;
    enumeratedtypelist=w;
  } 
  return w;
}

static ATermAppl find_case_function(enumeratedtype *e, ATermAppl sort)
{ 
  for(ATermList w=e->functions; w!=ATempty; w=ATgetNext(w))
  { 
    ATermAppl w1=ATAgetFirst(w);
    ATermAppl wsort=
               ATAgetArgument(
                 ATAgetArgument(ATAgetArgument(w1,1),1),0);
    if (wsort==sort)
    { return w1;
    }
  };
assert(0);
  ATerror("Internal: Searching for nonexisting case function on sort %t\n",sort);
  return NULL;
}

static void define_equations_for_case_function(
                enumeratedtype *e, 
                ATermAppl functionname, 
                ATermAppl sortname, 
                specificationbasictype *spec)
{ int j=0;
  
  ATermAppl v=NULL, v1=NULL;
  ATermList auxvars=ATempty, vars=ATempty, args=ATempty;
  ATermList xxxterm=ATempty;
  ATermList w=NULL;

  v1=getfreshvariable("x",e->sortId);
  for(j=0; (j<e->size); j++)
  { v=getfreshvariable("y",sortname);
    vars=ATinsertA(vars,v); 
    args=ATinsertA(args,v); 
    xxxterm=ATinsertA(xxxterm,v1);
  }

   /* I generate here an equation of the form
      C(e,x,x,x,...x)=x for a variable x. */
  v=getfreshvariable("e",e->sortId);
  declare_equation_variables( ATinsertA(ATinsertA(ATempty,v),v1));
  newequation(NULL,
              makeApplTerm(functionname,ATinsertA(xxxterm,v)),
              v1,
              spec);
  end_equation_section();

  declare_equation_variables(vars);
  auxvars=vars;

  for(w=e->elementnames; w!=ATempty ; w=ATgetNext(w))
  { 
    newequation(
           NULL,
           makeApplTerm(functionname,ATinsertA(args,ATAgetFirst(w))),
           ATAgetFirst(auxvars),
           spec);
    
    auxvars=ATgetNext(auxvars);
  }
  end_equation_section(); 
  
}


static void create_case_function_on_enumeratedtype(
                   ATermAppl sort,
                   enumeratedtype *e,
                   specificationbasictype *spec)
{ ATermList w=NULL;
  int j=0;
  
  /* first find out whether the function exists already, in which
       case nothing need to be done */
  for(w=e->functions; (w!=ATempty) &&
          (objectdata[objectIndex(ATAgetFirst(w))].targetsort!=sort);
           w=ATgetNext(w)) {};
  if (w==ATempty)
  { /* The function does not exist;
       Create a new function of enumeratedtype e, on sort */
       
    ATermAppl newsort=sort;
    ATermAppl casefunction=NULL;
    w=ATempty;
    for(j=0; (j<e->size) ; j++)
    {
      newsort=gsMakeSortArrow(sort,newsort);
    }

    newsort=gsMakeSortArrow(e->sortId,newsort);
    snprintf(scratch1,STRINGLENGTH,"C%d-%s",e->size,
         ((gsIsSortArrow(newsort))?"fun":ATSgetArgument(sort,0)));
    casefunction=gsMakeOpId(
                      fresh_name(scratch1),
                      newsort);
    insertmapping(casefunction,spec); 
    e->functions=ATinsertA(e->functions,casefunction);

    define_equations_for_case_function(e,casefunction,sort,spec);
  }
}

typedef struct enumtype {
 
  enumeratedtype *etype;
  ATermAppl var;
  struct enumtype *next;
} enumtype;

static enumtype *enumeratedtypes=NULL;

static enumtype *generate_enumerateddatatype(
                      int n, 
                      ATermList fsorts, 
                      ATermList gsorts, 
                      specificationbasictype *spec)
{ 
  enumtype *et=NULL;
  ATermList w=ATempty;
  
  et=malloc(sizeof(enumtype));
  et->var=NULL;
  ATprotect((ATerm *)&(et->var));
  et->next=enumeratedtypes;
  enumeratedtypes=et;
  et->etype=create_enumeratedtype(n,spec);
  
  et->var=gsMakeDataVarId(fresh_name("e"),et->etype->sortId);
  insertvariable(et->var,1);
  
  for(w=fsorts; w!=ATempty; w=ATgetNext(w))
  { create_case_function_on_enumeratedtype(
                  ATAgetFirst(w),
                  et->etype,
                  spec);
  }
     
  for(w=gsorts; w!=ATempty; w=ATgetNext(w))
  { create_case_function_on_enumeratedtype(
                  ATAgetFirst(w),
                  et->etype,
                  spec);
  }

  create_case_function_on_enumeratedtype(
                  gsMakeSortExprBool(),
                  et->etype,
                  spec);

  
  return et;
}

/************** Merge summands using enumerated type ***********************/


static int count_summands(ATermList t)
{ return ATgetLength(t);
}

static int mergeoccursin(
               ATermAppl *var, 
               ATermList v,
               ATermList *matchinglist,
               ATermList *pars, 
               ATermList *args)
{ 
  ATermList auxmatchinglist=ATempty;
  
  int result=0;
  
  /* First find out whether var:sort can be matched on a
     term in the matching list */
  
  /* first find out whether the variable occurs in the matching
     list, so, they can be joined */

  for( ; (*matchinglist)!=ATempty ; )
  { ATermAppl var1=ATAgetFirst(*matchinglist);
    *matchinglist=ATgetNext(*matchinglist);
    if (ATAgetArgument(*var,1)==ATAgetArgument(var1,1))
    { /* sorts match, so, we join the variables */
      result=1;
      if (*var!=var1)
      { *pars=ATinsertA(*pars,*var);
        *args=ATinsertA(*args,var1);
      }
         /* copy remainder of matching list */

      for( ; *matchinglist!=ATempty ; 
                 *matchinglist=ATgetNext(*matchinglist))
      { auxmatchinglist=
              ATinsertA(auxmatchinglist,ATAgetFirst(*matchinglist));
      } 
    }
    else
    { auxmatchinglist=ATinsertA(auxmatchinglist,var1);
    }
  }
  
  /* turn auxmatchinglist back in normal order, and put result
     in *matchinglist */
  
  *matchinglist=ATreverse(auxmatchinglist);

  if (result==0)
   { /* in this case no matching argument has been found.
     So, we must find out whether *var is an allowed variable, not
     occuring in the variablelist v.
     And if not, we must replace it by a new one. */
     for( ; v!=ATempty ; v=ATgetNext(v) )
     { ATermAppl var1=ATAgetFirst(v);
       if (strequal(ATSgetArgument(*var,0),
                    ATSgetArgument(var1,0)))
       { *var=getfreshvariable(ATSgetArgument(*var,0),
                               ATAgetArgument(*var,1));
         insertvariable(*var,1);
         *pars=ATinsertA(*pars,var1); 
         *args=ATinsertA(*args,*var); 
         v=ATempty;
       }
     }
   }

  return result;
}

static ATermList extend(ATermAppl c, ATermList cl)
{ 
  if (cl==ATempty)
  { return ATempty;
  }

  return ATinsertA(extend(c,ATgetNext(cl)),
                   gsMakeDataExprAnd(c,ATAgetFirst(cl)));
}

static ATermList extend_conditions(
                     ATermAppl var,
                     ATermList conditionlist,
                     specificationbasictype *spec)
{ 
  ATermAppl unique=dummyterm(ATAgetArgument(var,1),spec);
  ATermAppl newcondition=gsMakeDataExprEq(var,unique);
  return extend(newcondition,conditionlist);        
}   


static ATermAppl transform_matching_list(
                    ATermList matchinglist,
                    specificationbasictype *spec)
{ ATermAppl var=NULL; 
  ATermAppl unique=NULL;
  if (matchinglist==ATempty)
     return gsMakeDataExprTrue();

  var=ATAgetFirst(matchinglist);
  unique=dummyterm(ATAgetArgument(var,1),spec);
  return gsMakeDataExprAnd( 
               transform_matching_list(ATgetNext(matchinglist),spec),
               gsMakeDataExprEq(var,unique));
}


static ATermList addcondition(
                     ATermList matchinglist, 
                     ATermList conditionlist,
                     specificationbasictype *spec)
{ 
  return ATinsertA(conditionlist,
                   transform_matching_list(matchinglist,spec));
}

static ATermList merge_var(
                    ATermList v1, 
                    ATermList v2, 
                    ATermList *renamings, 
                    ATermList *conditionlist,
                    specificationbasictype *spec)
{ 
  ATermList result=ATempty;
  ATermList renamingargs=ATempty, renamingpars=ATempty;
  ATermList matchinglist=v2;
  
  /* If the sequence of sum variables is reversed,
   * the variables are merged in the same sequence for all
   * summands (due to a suggestion of Muck van Weerdenburg) */

  if (v2==ATempty)
  { v1=ATreverse(v1);
  }

  result=v2;
  for( ; v1!=ATempty ; v1=ATgetNext(v1))
  { ATermAppl var=ATAgetFirst(v1);
    if (!mergeoccursin(&var,v2,
            &matchinglist,&renamingpars,&renamingargs))
    { 
      result=ATinsertA(result,var);
      *conditionlist=extend_conditions(var,*conditionlist,spec);
    }
  }
  *conditionlist=addcondition(matchinglist,*conditionlist,spec);
  *renamings=ATinsert(ATinsert(*renamings,
                               (ATerm)renamingpars),
                      (ATerm)renamingargs);
  return result;
}

static ATermList make_binary_sums(
                      int n, 
                      ATermAppl enumtypename, 
                      ATermAppl *condition, 
                      ATermList tail)
{ ATermList result=ATempty;
  
  assert(n>1);
  *condition=NULL;
 
  n=n-1;
  for(result=tail ; (n>0) ; n=n/2)
  { ATermAppl sumvar=getfreshvariable("e",enumtypename);
    result=ATinsertA(result,sumvar); 
    if ((n % 2)==0)
    { if ((*condition)==NULL)
      { *condition=sumvar;
      }
      else 
      { *condition=gsMakeDataExprAnd(sumvar,*condition);
      }      
    }
    else
    { if ((*condition)==NULL)
      { *condition=gsMakeDataExprTrue();
      }
      else 
      { *condition=gsMakeDataExprOr(sumvar,*condition);
      }      
    }
  }
  return result;
}

static ATermAppl getcasefunction(ATermAppl thesort,enumtype *e)
{ 
  return find_case_function(e->etype, thesort);
}

static ATermAppl construct_binary_case_tree_rec(
                       int n, 
                       ATermList sums, 
                       ATermList *terms, 
                       ATermAppl termsort,
                       enumtype *e)
{ ATermAppl t=NULL,t1=NULL;
  ATermAppl casevar;

  assert(*terms!=ATempty);

  if (n<=0)
  { assert(*terms!=ATempty);
    return ATAgetFirst(*terms);
  }

  assert(sums!=ATempty);
  casevar=ATAgetFirst(sums);
  sums=ATgetNext(sums);
  
  t=construct_binary_case_tree_rec(n / 2,sums,terms,termsort,e);
  if (t==NULL) return NULL;

  if (*terms==ATempty)
  { return t; }

  t1=construct_binary_case_tree_rec(n / 2,sums,terms,termsort,e);
  if (t1==NULL) return NULL;
  if (t==t1)
  { return t; 
  }
  return gsMakeDataAppl(gsMakeDataAppl(gsMakeDataAppl(
              getcasefunction(termsort,e),casevar),t),t1);
}

static ATermAppl construct_binary_case_tree(
                        int n, 
                        ATermList sums, 
                        ATermList terms, 
                        ATermAppl termsort, 
                        enumtype *e)
{ 
  return construct_binary_case_tree_rec(n-1,sums,&terms,termsort,e);
}

static int summandsCanBeClustered(
              ATermAppl summand1,
              ATermAppl summand2)
{
  ATermAppl multiaction1=linGetMultiAction(summand1);
  ATermAppl actiontime1=linGetActionTime(summand1);
  ATermAppl multiaction2=linGetMultiAction(summand2);
  ATermAppl actiontime2=linGetActionTime(summand2);
 
  if (gsIsNil(actiontime1)!=
      gsIsNil(actiontime2))
  return 0;

  if (gsIsDelta(multiaction1))
  { if (gsIsDelta(multiaction2))
    { return 1;
    }
    else return 0;
  }

  if (gsIsDelta(multiaction2))
  { /* multiaction1!=delta */
    return 0;
  }

  /* Here the multiactions are proper multi actions,
     both with or without a time indication */

  /* The actions can be clustered if they contain
     the same actions, with the same sorts for the 
     actions. We assume that the multiactions are
     ordered.
  */

  ATermList multiactionlist1=ATLgetArgument(multiaction1,0);
  ATermList multiactionlist2=ATLgetArgument(multiaction2,0);
  for( ; multiactionlist1!=ATempty ; 
              multiactionlist1=ATgetNext(multiactionlist1) )
  { if (ATAgetArgument(ATAgetFirst(multiactionlist1),0)!=
           ATAgetArgument(ATAgetFirst(multiactionlist2),0))
    { return 0;
    }
    multiactionlist2=ATgetNext(multiactionlist2);
  }
  
  if (multiactionlist2!=ATempty)
  { return 0; 
  }

  return 1;
}

static ATermAppl collect_sum_arg_arg_cond(
                   enumtype *e,
                   int n,
                   ATermList sumlist,
                   ATermList gsorts,
                   specificationbasictype *spec)
{ /* This function gets a list of summands, with
     the same multiaction and time 
     status. It yields a single clustered summand
     by introducing an auxiliary sum operator, with
     a variable of enumtype. In case binary is used,
     will use a number of summands of sort Bool */

  ATermList resultsum=ATempty;
  ATermAppl resultcondition=NULL;
  ATermAppl resultmultiaction=NULL;
  ATermList resultmultiactionlist=ATempty;
  ATermAppl resulttime=NULL;
  ATermList resultnextstate=ATempty;

  ATermList auxresult=ATempty;
  ATermAppl auxresult1=NULL;
  ATermList rename_list=ATempty;
  ATermList auxrename_list=ATempty;
  /* rename list is a list of pairs of variable and term lists */
  ATermList conditionlist=ATempty;
  ATermAppl binarysumcondition=NULL;
  ATermAppl equalterm=NULL;
  int equaluptillnow=1;
  
  for(ATermList walker=sumlist; 
        walker!=ATempty; walker=ATgetNext(walker))
          
  { ATermAppl summand=ATAgetFirst(walker);
    ATermList sumvars=linGetSumVars(summand);

    resultsum=merge_var(sumvars,resultsum,&rename_list,&conditionlist,spec); 
  }
  
  if (binary)
  { resultsum=make_binary_sums(
                     n,
                     e->etype->sortId,
                     &binarysumcondition,
                     resultsum);
  }
  else     
  { resultsum=ATinsertA(resultsum,e->var);
  }

  /* Change the order of the rename list, note that the
     pairs in the list are reversed too */
  rename_list=ATreverse(rename_list);

  /* we construct the resulting condition */
  auxresult=ATempty;
  auxrename_list=rename_list;
  equalterm=NULL;
  equaluptillnow=1;
  for(ATermList walker=sumlist; walker!=ATempty; 
                  walker=ATgetNext(walker))

  { ATermAppl summand=ATAgetFirst(walker);
    ATermAppl condition=linGetCondition(summand); 

    assert(auxrename_list!=ATempty);
    ATermList auxpars=(ATermList)ATgetFirst(auxrename_list);
    auxrename_list=(ATermList)ATgetNext(auxrename_list);
    ATermList auxargs=(ATermList)ATgetFirst(auxrename_list);
    auxrename_list=(ATermList)ATgetNext(auxrename_list);
    auxresult1=substitute_data(auxargs,auxpars,condition); 
    if (equalterm==NULL)
    { equalterm=auxresult1;
    }
    else 
    { if (equaluptillnow==1)
      { equaluptillnow=(auxresult1==equalterm); 
      }
    }
    auxresult=ATinsertA(auxresult,auxresult1);
  }
  if (binary==1)
  { resultcondition=construct_binary_case_tree(n,
                resultsum,auxresult,gsMakeSortExprBool(),e);
    resultcondition=gsMakeDataExprAnd(binarysumcondition,resultcondition);        
    resultcondition=gsMakeDataExprAnd(
                   construct_binary_case_tree(n,
                       resultsum,conditionlist,gsMakeSortExprBool(),e),
                   resultcondition);
  }
  else
  { 
    if (equaluptillnow==1)
    { resultcondition=gsMakeDataExprAnd(
                  gsMakeDataApplList(
                          find_case_function(e->etype,gsMakeSortExprBool()),
                          ATinsertA(conditionlist,e->var)),
                  equalterm);        
    }
    else 
    { resultcondition=gsMakeDataApplList(
                     find_case_function(e->etype,gsMakeSortExprBool()),
                     ATinsertA(auxresult,e->var)); 
      resultcondition=gsMakeDataExprAnd(
                  gsMakeDataApplList(
                          find_case_function(e->etype,gsMakeSortExprBool()),
                          ATinsertA(conditionlist,e->var)),
                  resultcondition);
    }
  }
  
  /* now we construct the arguments of the action */
  /* First we collect all multi-actions in a separate list
     of multiactions */
  ATermList multiActionList=ATempty;
  int multiActionIsDelta=0;

  for (ATermList walker=sumlist; walker!=ATempty ; walker=ATgetNext(walker))
  { ATermAppl summand=ATAgetFirst(walker);
    ATermAppl multiaction=linGetMultiAction(summand);
    if (gsIsDelta(multiaction))
    { multiActionIsDelta=1;
      break;
    }
    multiActionList=ATinsertA(multiActionList,multiaction);
  }

  multiActionList=ATreverse(multiActionList);

  if (multiActionIsDelta)
  { resultmultiaction=gsMakeDelta();
  }
  else
  { resultmultiactionlist=ATempty;
    ATermList resultf=ATempty;
    /* ATermList nextmultiActions=ATempty; */
    long multiactioncount=ATgetLength(
                            ATLgetArgument(
                              ATAgetFirst(multiActionList),
                              0));
    for( ; multiactioncount>0 ; multiactioncount-- )
    { 
      long fcnt=ATgetLength(
                     ATgetArgument(
                       ATelementAt(
                         ATLgetArgument(ATAgetFirst(multiActionList),0),
                         multiactioncount-1),1));
      ATermAppl f=NULL;

      for( ; fcnt>0 ; fcnt-- )
      { auxresult=ATempty;
        auxrename_list=rename_list;
        equalterm=NULL;
        equaluptillnow=1;
        ATermList multiactionwalker=multiActionList;
        for(ATermList walker=sumlist; walker!=ATempty ; 
                                           walker=ATgetNext(walker))
        { 
          assert(auxrename_list!=ATempty);
          ATermList auxpars=(ATermList)ATgetFirst(auxrename_list);
          auxrename_list=(ATermList)ATgetNext(auxrename_list);
          ATermList auxargs=(ATermList)ATgetFirst(auxrename_list);
          auxrename_list=(ATermList)ATgetNext(auxrename_list);
  
          /* nextmultiActions=
                ATinsert(nextmultiActions,
                         (ATerm)ATgetNext(
                              (ATermList)ATgetFirst(multiActionList)));*/
  
          f=(ATermAppl)ATelementAt(
              ATLgetArgument(
                (ATermAppl)ATelementAt(
                  ATLgetArgument(ATAgetFirst(multiactionwalker),0),
                  multiactioncount-1),
                1),
              fcnt-1);
          auxresult1=substitute_data(auxargs,auxpars,f); 
    
          if (equalterm==NULL)
          { equalterm=auxresult1;
          }
          else 
          { if (equaluptillnow==1)
            { equaluptillnow=(equalterm==auxresult1);
          } }
          auxresult=ATinsertA(auxresult,auxresult1);
          multiactionwalker=ATgetNext(multiactionwalker);
        }
        if (equaluptillnow)
        { resultf=ATinsertA(resultf,equalterm);
        }
        else
        { if (binary==0)
          { resultf=ATinsertA(resultf,
                      gsMakeDataApplList(
                           find_case_function(e->etype,gsGetSort(f)),
                           ATinsertA(auxresult,e->var)));
          }
          else
          { ATermAppl temp=construct_binary_case_tree(
                              n,
                              resultsum,
                              auxresult,
                              gsGetSort(f),
                              e);
            resultf=ATinsertA(resultf,temp);
          }
        }
      }
      resultmultiactionlist=
            ATinsertA(
              resultmultiactionlist,
              gsMakeAction(
                ATAgetArgument(
                  (ATermAppl)ATelementAt(
                     ATLgetArgument(ATAgetFirst(multiActionList),0),
                     multiactioncount-1),
                  0),
                resultf));
    }
  } 

  /* Construct resulttime, the time of the action ... */

  auxresult=ATempty;
  auxrename_list=rename_list;
  equalterm=NULL;
  equaluptillnow=1;
  for(ATermList walker=sumlist; walker!=ATempty;
                  walker=ATgetNext(walker))
                            
  { ATermAppl summand=ATAgetFirst(walker);
    ATermAppl actiontime=linGetActionTime(summand);
  
    assert(auxrename_list!=ATempty);
    ATermList auxpars=(ATermList)ATgetFirst(auxrename_list);
    auxrename_list=(ATermList)ATgetNext(auxrename_list);
    ATermList auxargs=(ATermList)ATgetFirst(auxrename_list);
    auxrename_list=(ATermList)ATgetNext(auxrename_list);
    
    auxresult1=substitute_time(auxargs,auxpars,actiontime);
    if (equalterm==NULL)
    { equalterm=auxresult1;
    }
    else
    { if (equaluptillnow==1)
      { equaluptillnow=(auxresult1==equalterm); 
      }
    } 
    auxresult=ATinsertA(auxresult,auxresult1);
  }   
  if (binary==1)
  { resulttime=construct_binary_case_tree(n,
                resultsum,auxresult,gsMakeSortExprBool(),e);
  }
  else
  {
    if (equaluptillnow==1)
    { resulttime=equalterm;
    }
    else
    { resulttime=gsMakeDataApplList(
                     find_case_function(e->etype,gsMakeSortExprBool()),
                     ATinsertA(auxresult,e->var));
    }
  }

  /* now we construct the arguments of the invoked
     process, i.e. the new function g */
  long fcnt=0;
  resultnextstate=ATempty;
  for(ATermList ffunct=gsorts ; ffunct!=ATempty ; ffunct=ATgetNext(ffunct))
  { 
    auxresult=ATempty;
    auxrename_list=rename_list;
    equalterm=NULL;
    equaluptillnow=1;
    for(ATermList walker=sumlist; walker!=ATempty ; walker=ATgetNext(walker))
    { 
      ATermAppl summand=ATAgetFirst(walker);
      ATermList nextstate=linGetNextState(summand);

      assert(auxrename_list!=ATempty);
      ATermList auxpars=(ATermList)ATgetFirst(auxrename_list);
      auxrename_list=(ATermList)ATgetNext(auxrename_list);
      ATermList auxargs=(ATermList)ATgetFirst(auxrename_list);
      auxrename_list=(ATermList)ATgetNext(auxrename_list);

      ATermAppl nextstateparameter=(ATermAppl)ATelementAt(nextstate,fcnt);
      
      auxresult1=substitute_data(auxargs,auxpars,nextstateparameter); 
      if (equalterm==NULL)
      { equalterm=auxresult1;
      }
      else if (equaluptillnow==1)
      { equaluptillnow=(equalterm==auxresult1);
      }

      auxresult=ATinsertA(auxresult,auxresult1);
    }
    if (equaluptillnow)
    { resultnextstate=ATinsertA(resultnextstate,equalterm);
    }
    else
    { if (binary==0)
      { resultnextstate=
             ATinsertA(resultnextstate,
                  gsMakeDataApplList(
                       find_case_function(
                            e->etype,
                            ATAgetArgument(
                              ATAgetFirst(ffunct),
                              1)),
                       ATinsertA(auxresult,e->var)));
      }
      else 
      { ATermAppl temp=construct_binary_case_tree(
                           n,
                           resultsum,
                           auxresult,ATAgetFirst(ffunct),e);
        resultnextstate=ATinsertA(resultnextstate,temp);
      }
    }
    fcnt++;
  }
  /* Now turn *resultg around */
  resultnextstate=ATreverse(resultnextstate);
  
  return gsMakeLPESummand(resultsum,
                        resultcondition,
                        ((multiActionIsDelta)?
                            resultmultiaction:
                            gsMakeMultAct(resultmultiactionlist)),
                        resulttime,
                        resultnextstate);
}

static ATermList getActionSorts(ATermList actionlist)
{ ATermList resultsorts;

  for(resultsorts=ATempty ; actionlist!=ATempty ; 
                  actionlist=ATgetNext(actionlist))
  { resultsorts=ATconcat(
                   ATLgetArgument(
                         ATAgetArgument(ATAgetFirst(actionlist),0),
                         1),
                   resultsorts);
  }
  return resultsorts;
}

static ATermList  cluster_actions(
                       ATermList sums,
                       ATermList pars,
                       specificationbasictype *spec)
{ enumtype *enumeratedtype=NULL; 
  int n=0;
   /* We cluster first the summands with the action
      occurring in the first summand of sums. 
      These summands are first stored in w1. 
      The remaining summands are stored in w2. */
      
  ATermList result=ATempty;
  for( ; (sums!=ATempty) ; )
  { ATermList w1=ATempty;
    ATermList w2=ATempty;
    
    ATermAppl summand1=ATAgetFirst(sums);

    sums=ATgetNext(sums);

    w1=ATinsertA(w1,summand1); 
    for(ATermList w3=sums; (w3!=ATempty) ; w3=ATgetNext(w3))    
    { ATermAppl summand2=ATAgetFirst(w3);
      if (summandsCanBeClustered(summand1,summand2))
      { w1=ATinsertA(w1,summand2);
      }
      else 
      { w2=ATinsertA(w2,summand2);
      }
    }
    sums=w2;
    /* In w1 we now find all the summands labelled with
       similar multiactions, actiontime and terminationstatus. 
       We must now construct its clustered form. */
    n=count_summands(w1);

    
    if (n>1)
    { 
      ATermAppl multiaction=linGetMultiAction(ATAgetFirst(w1));
      ATermList actionsorts=ATempty;
      if (gsIsDelta(multiaction))
      { actionsorts=ATempty;
      }
      else
      { actionsorts=getActionSorts(ATLgetArgument(multiaction,0));
      }
      if (binary==0)
      { enumeratedtype=generate_enumerateddatatype(
                      n,actionsorts,getsorts(pars),spec); 
      }
      else 
      { enumeratedtype=generate_enumerateddatatype(
                      2,actionsorts,getsorts(pars),spec); 
      }

      result=ATinsertA(result,
             collect_sum_arg_arg_cond(enumeratedtype,n,w1,pars,spec));

    }
    else 
    { result=ATconcat(w1,result);
    }
  }
  return result;
}

/**************** Cluster final result ******************************/

static ATermAppl linMakeInitProcSpec(
                       ATermList init,
                       ATermList parameters,
                       ATermList sums)
{ return ATmakeAppl3(initprocspec_symbol,
                     (ATerm)init, 
                     (ATerm)parameters,
                     (ATerm)sums);
}

static int linIsInitProcSpec(ATermAppl initprocspec)
{
  return ATgetAFun(initprocspec)==initprocspec_symbol;
}

static ATermList linGetInit(ATermAppl initprocspec)
{ 
  assert(linIsInitProcSpec(initprocspec));
  return (ATermList)ATgetArgument(initprocspec,0);
}

static ATermList linGetParameters(ATermAppl initprocspec)
{ 
  assert(linIsInitProcSpec(initprocspec));
  return (ATermList)ATgetArgument(initprocspec,1);
}

static ATermList linGetSums(ATermAppl initprocspec)
{ 
  assert(linIsInitProcSpec(initprocspec));
  return (ATermList)ATgetArgument(initprocspec,2);
}


static ATermAppl clusterfinalresult(ATermAppl t,specificationbasictype *spec)
{ 
  return linMakeInitProcSpec(
                 linGetInit(t),
                 linGetParameters(t),
                 cluster_actions(linGetSums(t),linGetParameters(t),spec));
}

/**************** GENERATE LPEpCRL **********************************/


/* The variable regular indicates that we are interested in generating 
   a LPO assuming the pCRL term under consideration is regular */

static ATermAppl generateLPEpCRL(ATermAppl procId, int canterminate,
                       specificationbasictype *spec,int regular)
/* A pair of initial state and linear process equation must be extracted
   from the underlying GNF */
{ ATermList parameters=NULL,sums=NULL,initial=NULL;
  ATermList pCRLprocs=NULL;
  stacklisttype *stack=NULL;
  int singlecontrolstate=0;
  int n=objectIndex(procId);
   

  pCRLprocs=ATinsertA(ATempty,procId);

  pCRLprocs=makepCRLprocs(objectdata[n].processbody,pCRLprocs);
  /* now pCRLprocs contains a list of all process id's in this
     pCRL process */
 
  /* collect the parameters, but assume that variables
     have a unique sort */
  if (ATgetLength(pCRLprocs)==1)
  { singlecontrolstate=1;
  }
  parameters=collectparameterlist(pCRLprocs);
  alphaconversion(procId,parameters);
  if ((!singlecontrolstate)||(!regular)) 
         declare_control_state(spec,pCRLprocs);
  stack=new_stack(parameters,spec,regular,pCRLprocs);
  initial=make_initialstate(procId,stack,pCRLprocs,
                              regular,singlecontrolstate,spec);
     
  sums=collectsumlist(pCRLprocs,parameters,stack,
           (canterminate&&objectdata[n].canterminate),regular,
               singlecontrolstate,spec);


  if (regular)
  { if (!nocluster) 
    { if (oldstate)
      { sums=cluster_actions(sums,
              ((!singlecontrolstate)?
                    ATinsertA(stack->parameterlist,stack->stackvar):
                    stack->parameterlist),
               spec); 
      }
      else if (binary)
      { ATermList l=NULL;
        ATermList vars=stack->parameterlist;
        for(l=stack->booleanStateVariables; !ATisEmpty(l) ; l=ATgetNext(l))
        { vars=ATinsertA(vars,ATAgetFirst(l));
        }
        sums=cluster_actions(sums,vars,spec);
        create_enumeratedtype(stack->no_of_states,spec);
        sums=cluster_actions(sums,
                 ((!singlecontrolstate)?
                    ATinsertA(stack->parameterlist,stack->stackvar):
                    stack->parameterlist),
                 spec);
      }
    }
  }
  else /* not regular, use a stack */
  { if (!nocluster)
    { sums=cluster_actions(sums,ATinsertA(ATempty,stack->stackvar),spec); 
    }
  }

  /* now the summands have been collected in the variable sums. We only
     need to put the result in `initprocspec' and return it.
  */

  if (regular)
  { if (oldstate)
    { return linMakeInitProcSpec(
                    initial,
                    (singlecontrolstate?
                           stack->parameterlist:
                           ATinsertA(stack->parameterlist,stack->stackvar)),
                    sums); 
    } 
    if (binary)
    { ATermList l=NULL;
      ATermList vars=stack->parameterlist;
      for(l=stack->booleanStateVariables; !ATisEmpty(l) ; l=ATgetNext(l))
      { vars=ATinsertA(vars,ATAgetFirst(l));
      }

      return linMakeInitProcSpec(initial,vars,sums);
    } 
    /* normal enumerated type */
    { create_enumeratedtype(stack->no_of_states,spec);
      return linMakeInitProcSpec(
                  initial,
                  (singlecontrolstate?
                        stack->parameterlist:
                        ATinsertA(stack->parameterlist,stack->stackvar)),
                  sums);
    }
  }
  
  /* a process with a stack is returned */

  return linMakeInitProcSpec(
                 initial,
                 ATinsertA(ATempty,stack->stackvar),
                 sums); 
}


/**************** hiding *****************************************/

static int isinset(ATermAppl actionName, ATermList set)
{ 
  return ATindexOf(set,(ATerm)actionName,0)>=0;
}

static ATermAppl hide(ATermList hidelist, ATermAppl multiaction)
{ ATermList resultactionlist=ATempty;

  if (gsIsDelta(multiaction))
  { return multiaction; }

  for (ATermList walker=ATLgetArgument(multiaction,0);
            walker!=ATempty ; walker=ATgetNext(walker) )
  { ATermAppl action=ATAgetFirst(walker);
    if (!isinset(ATAgetArgument(
                    ATAgetArgument(action,0),
                    0),
                 hidelist))
    { resultactionlist=ATinsertA(resultactionlist,action);
    }
  }

  /* reverse the actionlist to maintain the ordering */
  return gsMakeMultAct(ATreverse(resultactionlist));
}

static ATermAppl hidecomposition(ATermList hidelist, ATermAppl ips)
{  
  ATermList resultsumlist=ATempty;
  ATermList sourcesumlist=linGetSums(ips);
  
  for( ; sourcesumlist!=ATempty ; sourcesumlist=ATgetNext(sourcesumlist))
  { ATermAppl summand=ATAgetFirst(sourcesumlist);
    ATermList sumvars=linGetSumVars(summand);
    ATermAppl multiaction=linGetMultiAction(summand);
    ATermAppl actiontime=linGetActionTime(summand); 
    ATermAppl condition=linGetCondition(summand);
    ATermList nextstate=linGetNextState(summand);

    resultsumlist=ATinsertA(
                    resultsumlist,
                    gsMakeLPESummand(
                           sumvars,
                           condition,
                           hide(hidelist,multiaction),
                           actiontime,
                           nextstate));
  }
  return linMakeInitProcSpec( 
             linGetInit(ips),linGetParameters(ips),resultsumlist);
}

/**************** allow  *************************************/

static ATermList sortMultiActionLabels(ATermList l)
{
  ATermList result=ATempty;
  for( ; l!=ATempty ; l=ATgetNext(l))
  { result=ATinsert(
             result,
             (ATerm)sortActionLabels(ATLgetArgument(ATAgetFirst(l),0)));
  }

  return result;
}

static int allowsingleaction(ATermList allowaction, ATermAppl multiaction)
{ 
  for (ATermList walker=ATLgetArgument(multiaction,0);
              walker!=ATempty ; walker=ATgetNext(walker) )
  { 
    ATermAppl action=ATAgetFirst(walker);
    
    if (ATisEmpty(allowaction))
    { return 0;
    }
    assert(gsIsAction(action));
    assert(gsIsActId(ATAgetArgument(action,0)));
    if (strcmp(ATgetName(ATgetAFun(ATgetFirst(allowaction))),
                 ATgetName(ATgetAFun(ATgetArgument(ATgetArgument(action,0),0))))!=0)
    { return 0;
    }
    allowaction=ATgetNext(allowaction);
  }
  if (allowaction==ATempty)
  { return 1;
  }
  return 0;
}

static int allow(ATermList allowlist, ATermAppl multiaction)
{ 
  if (gsIsDelta(multiaction))
  { return 0; }

  /* The empty multiaction, i.e. tau, is never blocked by allow */
  if (ATLgetArgument(multiaction,0)==ATempty)
  { return 1; }

  for( ; allowlist!=ATempty ; allowlist=ATgetNext(allowlist))
  { if (allowsingleaction(ATLgetFirst(allowlist),multiaction))
    { 
      return 1;
    }
  }
  return 0;
}

static ATermAppl allowcomposition(ATermList allowlist, ATermAppl ips)
{
  ATermList resultsumlist=ATempty;
  ATermList sourcesumlist=linGetSums(ips);
  allowlist=sortMultiActionLabels(allowlist);

  for( ; sourcesumlist!=ATempty ; sourcesumlist=ATgetNext(sourcesumlist))
  { ATermAppl summand=ATAgetFirst(sourcesumlist);
    ATermList sumvars=linGetSumVars(summand);
    ATermAppl multiaction=linGetMultiAction(summand);
    ATermAppl actiontime=linGetActionTime(summand);
    ATermAppl condition=linGetCondition(summand);
    ATermList nextstate=linGetNextState(summand);

    if (allow(allowlist,multiaction))
    { 
      resultsumlist=ATinsertA(
                    resultsumlist,
                    summand); }
    else
    { 
      if (!gsIsNil(actiontime))
      { resultsumlist=ATinsertA(
                      resultsumlist,
                      gsMakeLPESummand(
                             sumvars,
                             condition,
                             gsMakeDelta(),
                             actiontime,
                             nextstate));
      }
    }
  }
  return linMakeInitProcSpec(
             linGetInit(ips),linGetParameters(ips),resultsumlist);
}


/**************** encapsulation *************************************/

static ATermAppl encap(ATermList encaplist, ATermAppl multiaction)
{ int actioninset=0;
  if (gsIsDelta(multiaction))
  { return multiaction ; }
  
  for (ATermList walker=ATLgetArgument(multiaction,0) ;
            walker!=ATempty ; walker=ATgetNext(walker) )
  { ATermAppl action=ATAgetFirst(walker);
    if (isinset(ATAgetArgument(ATAgetArgument(action,0),0),encaplist))
    { actioninset=1;
      break;
    }
  }

  /* reverse the actionlist to maintain the ordering */
  if (actioninset)
  { return gsMakeDelta() ; 
  }
  return multiaction;
}


static ATermAppl encapcomposition(ATermList encaplist , ATermAppl ips)
{  
  ATermList resultsumlist=ATempty;
  ATermList sourcesumlist=linGetSums(ips);

  for( ; sourcesumlist!=ATempty ; sourcesumlist=ATgetNext(sourcesumlist))
  { ATermAppl summand=ATAgetFirst(sourcesumlist);
    ATermList sumvars=linGetSumVars(summand);
    ATermAppl multiaction=linGetMultiAction(summand);
    ATermAppl actiontime=linGetActionTime(summand);
    ATermAppl condition=linGetCondition(summand);
    ATermList nextstate=linGetNextState(summand);
 
    ATermAppl resultmultiaction=encap(encaplist,multiaction);

    if ((!gsIsDelta(resultmultiaction)) ||
        (actiontime!=gsMakeNil()))
    { 
      resultsumlist=ATinsertA(
                    resultsumlist,
                    gsMakeLPESummand(
                           sumvars,
                           condition,
                           resultmultiaction,
                           actiontime,
                           nextstate));
    }
  }
  return linMakeInitProcSpec(
             linGetInit(ips),linGetParameters(ips),resultsumlist);
}

/**************** renaming ******************************************/

static ATermAppl rename_action(ATermList renamings, ATermAppl action)
{ ATermAppl actionId=ATAgetArgument(action,0);
  char *s=ATSgetArgument(actionId,0);
  for ( ; renamings!=ATempty ; renamings=ATgetNext(renamings))
  { ATermAppl renaming=ATAgetFirst(renamings);
    if (strequal(s,ATSgetArgument(renaming,0)))
    { return gsMakeAction(
                   gsMakeActId(
                         gsString2ATermAppl(ATSgetArgument(renaming,1)),
                         ATLgetArgument(actionId,1)),
                   ATLgetArgument(action,1));
    }
  }
  return action;
}

static ATermAppl rename_actions(ATermList renamings, ATermAppl multiaction)
{ 
  ATermList resultactionlist=ATempty;
  if (gsIsDelta(multiaction))
  { return gsMakeDelta(); }
  
  for (ATermList walker=ATreverse(ATLgetArgument(multiaction,0)) ;
            walker!=ATempty ; walker=ATgetNext(walker) )
  { ATermAppl action=ATAgetFirst(walker);
    resultactionlist=linInsertActionInMultiActionList(
                          rename_action(renamings,action),
                          resultactionlist);
  } 

  return gsMakeMultAct(resultactionlist);
}

static ATermAppl renamecomposition(ATermList renamings, ATermAppl ips)
{
  ATermList resultsumlist=ATempty;
  ATermList sourcesumlist=linGetSums(ips);

  for( ; sourcesumlist!=ATempty ; sourcesumlist=ATgetNext(sourcesumlist))
  { ATermAppl summand=ATAgetFirst(sourcesumlist);
    ATermList sumvars=linGetSumVars(summand);
    ATermAppl multiaction=linGetMultiAction(summand);
    ATermAppl actiontime=linGetActionTime(summand);
    ATermAppl condition=linGetCondition(summand);
    ATermList nextstate=linGetNextState(summand);

    resultsumlist=ATinsertA(
                    resultsumlist,
                    gsMakeLPESummand(
                           sumvars,
                           condition,
                           rename_actions(renamings,multiaction),
                           actiontime,
                           nextstate));
  }
  return linMakeInitProcSpec(
             linGetInit(ips),linGetParameters(ips),resultsumlist);
}

/**************** equalargs ****************************************/

static int occursinvarandremove(ATermAppl var, ATermList *vl)
{ int result=0;
  ATermAppl var1=NULL;
  
  if (*vl==ATempty)
  { return 0;
  }
  *vl=ATgetNext(*vl);
  var1=ATAgetFirst(*vl);
  if (var==var1)
    return 1; 

  result=occursinvarandremove(var,vl);
  *vl=ATinsertA(*vl,var1); 
  return result;
}

static int sumelimination(
                   ATermAppl u1, 
                   ATermAppl u2,
                   ATermList *sumlist, 
                   ATermList *subargs,
                   ATermList *subpars)
{ int result=0;
  
  if (gsIsDataVarId(u1) && (occursinvarandremove(u1,sumlist)))
  { result=1;
    *subargs=ATinsertA(*subargs,u2); 
    *subpars=ATinsertA(*subpars,u1);
  }
  else if (gsIsDataVarId(u2) && (occursinvarandremove(u2,sumlist)))
  { result=1;
    *subargs=ATinsertA(*subargs,u1);
    *subpars=ATinsertA(*subpars,u2);
  }
  return result;
}

/********************** construct renaming **************************/

static ATermList construct_renaming(
                   ATermList pars1, 
                   ATermList pars2, 
                   ATermList *pars3, 
                   ATermList *pars4)
{ /* check whether the variables in pars2 are unique,
     wrt to those in pars1, and deliver:
     - in pars3 a list of renamed parameters pars2, such that
       pars3 is unique with respect to pars1;
     - in pars4 a list of parameters that need to be renamed;
     - as a return result, new values for the parameters in pars4.
       This allows using substitute_data(list) to rename
       action and process arguments and conditions to adapt
       to the new parameter names.
   */
     
  ATermList t=NULL, t1=NULL, t2=NULL;
  
  if (pars2==ATempty)
  { *pars3=ATempty;
    t1=ATempty;
    *pars4=ATempty;
  }
  else 
  { ATermAppl var2=ATAgetFirst(pars2);
    pars2=ATgetNext(pars2);

    if (occursin(var2,pars1))
    { ATermAppl var3=gsMakeDataVarId(
                        fresh_name(ATSgetArgument(var2,0)),
                        ATAgetArgument(var2,1)); 

      insertvariable(var3,1);
      t1=ATinsertA(construct_renaming(pars1,pars2,&t,&t2),var3);
          
      *pars4=ATinsertA(t2,var2);
      *pars3=ATinsertA(t,var3);
    }
    else 
    { t1=construct_renaming(pars1,pars2,&t,pars4);
      *pars3=ATinsertA(t,var2);
    }

  }
  return t1;
}

/**************** communication operator composition ****************/

/* static ATermList encap(ATermList encaplist, ATermList multiaction)
{ int actioninset=0;
  if (gsIsDelta(multiaction))
  { return delta; }
 
  for (ATermList walker=multiaction ;
            walker!=ATempty ; walker=ATgetNext(walker) )
  { ATermAppl action=ATAgetFirst(walker);
    if (isinset(ATAgetArgument(ATAgetArgument(action,0),0),encaplist))
    { actioninset=1;
      break;
    }
  }
            
  / * reverse the actionlist to maintain the ordering * /
  if (actioninset)
  { return delta;         
  }                       
  return multiaction;
}
*/

static ATermList insertActionLabel(
                      ATermAppl action, 
                      ATermList actionlabels)  
{ /* assume actionlabels is sorted, and put
     action at the proper place to yield a sorted
     list */
  if (actionlabels==ATempty)
  { return ATinsertA(ATempty,action);
  }

  ATermAppl firstAction=ATAgetFirst(actionlabels);


  if (strcmp(gsATermAppl2String(action),
               gsATermAppl2String(firstAction))<0)
  { return ATinsertA(actionlabels,action);
  }
 
  return ATinsertA(insertActionLabel(
                       action,
                       ATgetNext(actionlabels)),
                   firstAction);
}

static ATermList sortActionLabels(ATermList actionlabels)
{ 
  ATermList result=ATempty;
  for( ; actionlabels!=ATempty ; 
                 actionlabels=ATgetNext(actionlabels))
  { result=insertActionLabel(ATAgetFirst(actionlabels),result);
  }
  return result;
}

static ATermList getsorts(ATermList l)
{ if (l==ATempty) return ATempty;

  return ATinsertA(getsorts(ATgetNext(l)),
                   ATAgetArgument(ATAgetFirst(l),1));

}

static ATermAppl pairwiseMatch(ATermList l1, ATermList l2)
{
  if (l1==ATempty)
  { if (l2==ATempty)
    { return gsMakeDataExprTrue();
    }
    return gsMakeDataExprFalse();
  }

  if (l2==ATempty)
  { return gsMakeDataExprFalse();
  }

  ATermAppl t1=ATAgetFirst(l1);
  ATermAppl t2=ATAgetFirst(l2);

  if (gsGetSort(t1)!=gsGetSort(t2))
  { return gsMakeDataExprFalse();
  }

  ATermAppl result=pairwiseMatch(ATgetNext(l1),ATgetNext(l2));

  if (result==gsMakeDataExprFalse())
  { return result;
  }

  return gsMakeDataExprAnd(
            gsMakeDataExprEq(t1,t2),
            result);
}

static ATermList addActionCondition(
                     ATermAppl firstaction,
                     ATermAppl condition,
                     ATermList L,
                     ATermList S)
{ 
  /* if firstaction==NULL, it should not be added */
  for( ; L!=ATempty ; L=ATgetNext(L))
  { ATermAppl firsttuple=ATAgetFirst(L);
    S=ATinsertA(S,
        linMakeTuple(
          ((firstaction!=NULL)?
               ATinsertA(ATLgetArgument(firsttuple,0), firstaction):
               ATLgetArgument(firsttuple,0)),
          gsMakeDataExprAnd(ATAgetArgument(firsttuple,1),condition)));
  }
  return S;
}

static ATermAppl can_communicate(ATermList m,ATermList C)
{ /* this function indicates whether the actions in m
     consisting of actions and data occur in C, such that
     a communication can take place. If not NULL is delivered,
     otherwise the resulting action is the result. If the
     resulting action is tau, or nil, the result is nil. */
  
  for( ; C!=ATempty ; C=ATgetNext(C))
  { ATermAppl commExpr=ATAgetFirst(C);
    assert(gsIsCommExpr(commExpr));
    ATermList lhs=ATLgetArgument(ATAgetArgument(commExpr,0),0);
    int canCommunicate=1;
    ATermList mwalker=m;
    for( ; ((lhs!=ATempty) && (mwalker!=ATempty)) ; lhs=ATgetNext(lhs))
    { 
      ATermAppl actionname=ATAgetArgument(ATAgetArgument(ATAgetFirst(mwalker),0),0);
      ATermAppl commname=ATAgetFirst(lhs);
      if (actionname!=commname)
      { canCommunicate=0;
        break;
      }
      mwalker=ATgetNext(mwalker);
    }
    if ((canCommunicate) && (mwalker==ATempty) && (lhs==ATempty))
    { ATermAppl rhs=ATAgetArgument(commExpr,1);
      if (rhs==gsMakeTau())
      { return gsMakeNil();
      }
      return gsMakeActId(rhs,ATLgetArgument(ATAgetFirst(m),1));
    }
  }
  return NULL;
}
  

static ATermList makeMultiActionConditionList(
                   ATermList multiaction,
                   ATermList communications);

static ATermList phi(ATermList m,
                     ATermList d,
                     ATermList w,
                     ATermList n,
                     ATermList C)
{ /* phi is a function that yields a list of pairs
     indicating how the actions in m|w|n can communicate.
     The pairs contain the resulting multi action and
     a condition on data indicating when communication
     can take place. In the communication all actions of
     m, none of w and a subset of n can take part in the
     communication. d is the data parameter of the communication
     and C contains a list of multiaction action pairs indicating
     possible commmunications */

  if (n==ATempty)
  { ATermAppl c=can_communicate(m,C); /* returns NULL if no communication
                                         is possible */
    if (c!=NULL)
    { ATermList T=makeMultiActionConditionList(w,C);
      return addActionCondition(
                   ((ATAgetArgument(c,0)==gsMakeNil())?NULL:gsMakeAction(c,d)),
                   gsMakeDataExprTrue(),
                   T,
                   ATempty);
    }
    /* c==NULL, actions in m cannot communicate */
    return ATempty;
  }
  /* if n=[a(f)] \oplus o */
  ATermAppl firstaction=ATAgetFirst(n);
  ATermList o=ATgetNext(n);
  ATermList T=phi(ATappend(m,(ATerm)firstaction),d,w,o,C);
  return addActionCondition(
                NULL,
                pairwiseMatch(d,ATLgetArgument(firstaction,1)),
                T,
                phi(m,d,ATappend(w,(ATerm)firstaction),o,C));
}


static ATermAppl makeNegatedConjunction(ATermList S)
{ ATermAppl result=gsMakeDataExprTrue();
  for( ; S!=ATempty ; S=ATgetNext(S) )
  { result=gsMakeDataExprAnd(
             gsMakeDataExprNot(ATAgetArgument(ATAgetFirst(S),1)),result);
  }
  return result; 
}

static ATermList makeMultiActionConditionList(
                   ATermList multiaction,
                   ATermList communications)
{ /* This is the function gamma(m,C) provided
     by Muck van Weerdenburg in Calculation of 
     Communication with open terms [1]. */

  if (multiaction==ATempty)
  { return ATinsertA(ATempty,linMakeTuple(ATempty,gsMakeDataExprTrue()));
  }

  ATermAppl firstaction=ATAgetFirst(multiaction);
  ATermList remainingmultiaction=ATgetNext(multiaction); /* This is m in [1] */

  ATermList S=phi(ATinsertA(ATempty,firstaction),
                  ATLgetArgument(firstaction,1),
                  ATempty,
                  remainingmultiaction,
                  communications);
  ATermList T=makeMultiActionConditionList(
                  remainingmultiaction,
                  communications);
  ATermAppl b=makeNegatedConjunction(S);
  S=addActionCondition(firstaction,b,T,S);
  return S;

}

static ATermAppl communicationcomposition(
                      ATermList communications,
                      ATermAppl ips)
{ /* We follow the implementation of Muck van Weerdenburg, described in 
     a note: Calculation of communication with open terms. */

  /* first we sort the multiactions in communications */

  ATermList resultingCommunications=ATempty;
  for( ; communications!=ATempty ; communications=ATgetNext(communications))
  { ATermAppl commExpr=ATAgetFirst(communications);
    ATermList source=ATLgetArgument(ATAgetArgument(commExpr,0),0);
    ATermAppl target=ATAgetArgument(commExpr,1);
    resultingCommunications=ATinsertA(resultingCommunications,
               gsMakeCommExpr(gsMakeMultActName(sortActionLabels(source)),target));
  }
  communications=resultingCommunications;

  
  ATermList resultsumlist=ATempty;

  for(ATermList sourcesumlist=linGetSums(ips) ;
                sourcesumlist!=ATempty ; 
                sourcesumlist=ATgetNext(sourcesumlist))
  { ATermAppl summand=ATAgetFirst(sourcesumlist);
    ATermList sumvars=linGetSumVars(summand);
    ATermAppl multiaction=linGetMultiAction(summand);
    ATermAppl actiontime=linGetActionTime(summand);
    ATermAppl condition=linGetCondition(summand);
    ATermList nextstate=linGetNextState(summand);

      /* the multiactionconditionlist is a list containing
         tuples, with a multiaction and the condition,
         expressing whether the multiaction can happen. All
         conditions exclude each other. Furthermore, the list
         is not empty. If no communications can take place,
         the original multiaction is delivered, with condition
         true. */

    ATermList multiactionconditionlist=
                     makeMultiActionConditionList(
                              ATLgetArgument(multiaction,0),
                              communications);
    // ATfprintf(stderr,"Multiactionlist: %t\n",multiactionconditionlist);

    assert(multiactionconditionlist!=ATempty);
    for( ; multiactionconditionlist!=ATempty ;
               multiactionconditionlist=ATgetNext(multiactionconditionlist) )
    { ATermAppl multiactioncondition=ATAgetFirst(multiactionconditionlist);
      ATermAppl freshcondition=RewriteTerm(
                    gsMakeDataExprAnd(
                    condition,
                    ATAgetArgument(multiactioncondition,1)));
      if (freshcondition!=gsMakeDataExprFalse())
      { resultsumlist=ATinsertA(
                    resultsumlist,
                    gsMakeLPESummand(
                           sumvars,
                           freshcondition,
                           gsMakeMultAct(ATLgetArgument(multiactioncondition,0)),
                           actiontime,
                           nextstate));
      }
    }
  }
  return linMakeInitProcSpec(
             linGetInit(ips),linGetParameters(ips),resultsumlist);
}

static ATermAppl makesingleultimatedelaycondition(
                     ATermList sumvars,
                     ATermList freevars,
                     ATermAppl timevariable,
                     ATermAppl actiontime,
                     specificationbasictype *spec)
{ 
  ATermAppl result=gsMakeDataExprLT(timevariable,actiontime);
  ATermList variables=ATinsertA(ATempty,timevariable);

  for ( ; freevars!=ATempty ; freevars=ATgetNext(freevars) )
  { ATermAppl freevar=ATAgetFirst(freevars);
    if (occursinterm(freevar,result))
    { variables=ATinsertA(variables,freevar);
    }
  } 

  declare_equation_variables(variables); 
  for ( ; sumvars!=ATempty ; sumvars=ATgetNext(sumvars) )
  { ATermAppl sumvar=ATAgetFirst(sumvars);
    if (occursinterm(sumvar,result))
    { /* make a new process equation */
      ATermList extendedvariables=ATappend(variables,(ATerm)sumvar);
      ATermAppl newfunction=gsMakeOpId(fresh_name("ExistsFun"),
                                       gsMakeSortArrowList(getsorts(extendedvariables),gsMakeSortExprBool()));
      
      newequation(NULL,gsMakeDataApplList(newfunction,extendedvariables),result,spec);
      result=gsMakeDataExprExists(gsMakeDataApplList(newfunction,variables));
    }
  } 
  end_equation_section();
 
  return result;
}

static ATermAppl getUltimateDelayCondition(
                 ATermList sumlist, 
                 ATermList freevars,
                 ATermAppl timevariable,
                 specificationbasictype *spec)
{  
   ATermAppl result=gsMakeDataExprTrue();
   for (ATermList walker=sumlist; (walker!=ATempty);
                               walker=ATgetNext(walker))
   { ATermAppl summand=ATAgetFirst(walker);
     ATermList sumvars=linGetSumVars(summand);
     ATermAppl actiontime=linGetActionTime(summand);

     if (actiontime==gsMakeNil())
     { return gsMakeDataExprTrue();
     }

     result=gsMakeDataExprAnd(result,
              makesingleultimatedelaycondition(
                     sumvars,
                     freevars,
                     timevariable,
                     actiontime,
                     spec));
  }
  return result;
}


/**************** parallel composition ******************************/

static ATermList combinesumlist(
                     ATermList sumlist1, 
                     ATermList sumlist2, 
                     ATermList par1, 
                     ATermList par2,
                     ATermList par3,
                     ATermList rename_list, 
                     specificationbasictype *spec,
                     ATermList parametersOfsumlist2)


{ ATermList resultsumlist=NULL;
  ATermList rename1_list=NULL, rename2_list=NULL, allpars=NULL,
            sums1renaming=NULL, sums2renaming=NULL;
  
  allpars=ATconcat(par1,par3);
  resultsumlist=ATempty;

  /* first we enumerate the summands of t1 */

  ATermAppl realsort=gsMakeSortId(gsString2ATermAppl("Real"));
  if (!existsort(gsMakeSortId(gsString2ATermAppl("Real"))))
  { realsort=gsMakeSortId(gsString2ATermAppl("Nat"));
  }
  ATermAppl timevar=getfreshvariable(
                       "timevar",realsort);
  ATermAppl ultimatedelaycondition=
               substitute_data(rename_list,par2,
                   getUltimateDelayCondition(sumlist2,parametersOfsumlist2,timevar,spec));

  for (ATermList walker1=sumlist1; (walker1!=ATempty); 
                        walker1=ATgetNext(walker1)) 
  { ATermAppl summand1=ATAgetFirst(walker1);

    ATermList sumvars1=linGetSumVars(summand1);
    ATermList sumvars1new=ATempty;
    ATermAppl multiaction1=linGetMultiAction(summand1);
    ATermAppl actiontime1=linGetActionTime(summand1); 
    ATermAppl condition1=linGetCondition(summand1);
    ATermList nextstate1=linGetNextState(summand1); 

    rename1_list=construct_renaming(
                       allpars,
                       sumvars1,
                       &sumvars1new,
                       &sums1renaming);

    condition1=substitute_data(rename1_list,sums1renaming,condition1);
    actiontime1=substitute_time(rename1_list,sums1renaming,actiontime1);

    if (multiaction1!=terminationAction)
    { if (actiontime1==gsMakeNil())
      { if (ultimatedelaycondition!=gsMakeDataExprTrue())
        { actiontime1=timevar;
          sumvars1=ATinsertA(sumvars1,timevar);
          condition1=gsMakeDataExprAnd(ultimatedelaycondition,condition1);
        }
      }
      else
      { /* actiontime1!=nil. Substitute the time expression for
           timevar in ultimatedelaycondition, and extend the condition */
        ultimatedelaycondition=
                substitute_data(
                   ATinsertA(ATempty,actiontime1),
                   ATinsertA(ATempty,timevar),
                   ultimatedelaycondition);
        condition1=gsMakeDataExprAnd(ultimatedelaycondition,condition1);
      }
  
      condition1=RewriteTerm(condition1);
      if (condition1!=gsMakeDataExprFalse())
      { resultsumlist=
          ATinsertA(
            resultsumlist,
            gsMakeLPESummand(
               sumvars1new,
               condition1,
               substitute_multiaction(rename1_list,sums1renaming,multiaction1), 
               actiontime1,
               substitute_assignmentlist(
                               rename1_list,
                               sums1renaming,
                               nextstate1,
                               par1,
                               0)));
      }
    }
  }
  /* second we enumerate the summands of sumlist2 */

  ultimatedelaycondition=
                   getUltimateDelayCondition(sumlist1,par1,timevar,spec);

  for (ATermList walker2=sumlist2; walker2!=ATempty;
         walker2=ATgetNext(walker2) )
  { 
    ATermAppl summand2=ATAgetFirst(walker2);
    ATermList sumvars2=linGetSumVars(summand2);
    ATermList sumvars2new=ATempty;
    ATermAppl multiaction2=linGetMultiAction(summand2);
    ATermAppl actiontime2=linGetActionTime(summand2); 
    ATermAppl condition2=linGetCondition(summand2);
    ATermList nextstate2=linGetNextState(summand2); 

    if (multiaction2!=terminationAction)
    { 

      rename2_list=construct_renaming(
                           allpars,
                           sumvars2,
                           &sumvars2new,
                           &sums2renaming);
      actiontime2=substitute_time(rename_list,par2,
                   substitute_time(rename2_list,sums2renaming,actiontime2)),
      condition2=substitute_data(rename_list,par2,
                     substitute_data(rename2_list,sums2renaming,condition2));

      if (actiontime2==gsMakeNil())
      { if (ultimatedelaycondition!=gsMakeDataExprTrue())
        { actiontime2=timevar;
          sumvars2=ATinsertA(sumvars2,timevar);
          condition2=gsMakeDataExprAnd(ultimatedelaycondition,condition2);
        }
      }
      else
      { /* actiontime1!=gsMakeNil(). Substitute the time expression for
           timevar in ultimatedelaycondition, and extend the condition */
        ultimatedelaycondition=
                substitute_data(
                   ATinsertA(ATempty,actiontime2),
                   ATinsertA(ATempty,timevar),
                   ultimatedelaycondition);
        condition2=gsMakeDataExprAnd(ultimatedelaycondition,condition2);
      }
  
      condition2=RewriteTerm(condition2);
      if (condition2!=gsMakeDataExprFalse())
      { resultsumlist=
          ATinsertA(
            resultsumlist,
            gsMakeLPESummand(
               sumvars2new,
               condition2,
               substitute_multiaction(rename_list,par2,
                   substitute_multiaction(rename2_list,sums2renaming,multiaction2)), 
               actiontime2,
               substitute_assignmentlist(
                         rename2_list,
                         sums2renaming,
                         substitute_assignmentlist(
                               rename_list,
                               par2,
                               nextstate2,
                               par3,
                               1),
                         par3,
                         0)));
      }
    }
  }

  /* thirdly we enumerate all communications */

  for (ATermList walker1=sumlist1; walker1!=ATempty;
                  walker1=ATgetNext(walker1))
  { ATermAppl summand1=ATAgetFirst(walker1);

    ATermList sumvars1=linGetSumVars(summand1);
    ATermList sumvars1new=ATempty;
    ATermAppl multiaction1=linGetMultiAction(summand1);
    ATermAppl actiontime1=linGetActionTime(summand1);
    ATermAppl condition1=linGetCondition(summand1);
    ATermList nextstate1=linGetNextState(summand1);

    rename1_list=construct_renaming(allpars,
                                    sumvars1,&sumvars1new,&sums1renaming);

    multiaction1=substitute_multiaction(
                           rename1_list,
                           sums1renaming,
                           multiaction1);
    nextstate1=substitute_assignmentlist(
                     rename1_list,
                     sums1renaming,
                     nextstate1,
                     par1,
                     0);
    actiontime1= substitute_time(rename1_list,sums1renaming,actiontime1);
    condition1= substitute_data(rename1_list,sums1renaming,condition1);

    for (ATermList walker2=sumlist2; walker2!=ATempty;
         walker2=ATgetNext(walker2) )
    {
      ATermAppl summand2=ATAgetFirst(walker2);
      ATermList sumvars2=linGetSumVars(summand2);
      ATermList sumvars2new=ATempty;
      ATermAppl multiaction2=linGetMultiAction(summand2);
      ATermAppl actiontime2=linGetActionTime(summand2);
      ATermAppl condition2=linGetCondition(summand2);
      ATermList nextstate2=linGetNextState(summand2);

      rename2_list=construct_renaming(
               ATconcat(sumvars1new,allpars),
               sumvars2,&sumvars2new,&sums2renaming);
      
      if ((multiaction1==terminationAction)==(multiaction2==terminationAction))
      { ATermAppl multiaction3=NULL;
        if ((multiaction1==terminationAction)&&(multiaction2==terminationAction))
        { multiaction3=terminationAction;
        }
        else 
        { multiaction3=linMergeMultiAction(
                             multiaction1,
                             substitute_multiaction(rename_list,par2,
                                substitute_multiaction(
                                             rename2_list,
                                             sums2renaming,
                                             multiaction2)));
        }
        ATermList allsums=ATconcat(sumvars1new,sumvars2new);
        actiontime2=substitute_time(rename_list,par2,
                substitute_time(rename2_list,sums2renaming,actiontime2));
  
        condition2=substitute_data(rename_list,par2,
                substitute_data(rename2_list,sums2renaming,condition2));
        ATermAppl condition3= gsMakeDataExprAnd(condition1,condition2);
  
        ATermAppl actiontime3=NULL;
  
        if (actiontime1==gsMakeNil())
        { if (actiontime2==gsMakeNil())
          { actiontime3=gsMakeNil();
          }
          else
          { /* actiontime2!=gsMakeNil() */
            actiontime3=actiontime2;
          }
        }
        else
        { /* actiontime1!=gsMakeNil() */
          if (actiontime2==gsMakeNil())
          { actiontime3=actiontime1;
          }
          else
          { /* actiontime1!=gsMakeNil() && actiontime2!=gsMakeNil() */
            actiontime3=actiontime1;
            condition3=gsMakeDataExprAnd(
                          condition3,
                          gsMakeDataExprEq(actiontime1,actiontime2));
          }
        }                         
  
        nextstate2=substitute_assignmentlist(
                     rename2_list,
                     sums2renaming,
                     substitute_assignmentlist(
                          rename_list,
                          par2,
                          nextstate2,
                          par3,
                          1),
                     par3,
                     0);
  
        ATermList nextstate3=ATconcat(nextstate1,nextstate2);
        
        condition3=RewriteTerm(condition3);
        if (condition3!=gsMakeDataExprFalse())
        { resultsumlist=
            ATinsertA(
              resultsumlist,
              gsMakeLPESummand(
                allsums,
                condition3,
                multiaction3,
                actiontime3,
                nextstate3));
        }
      }
    }
  }

  return resultsumlist;
}


static ATermAppl parallelcomposition(
                     ATermAppl t1, 
                     ATermAppl t2, 
                     int canterminate,
                     specificationbasictype *spec)
{ 
  ATermList init1=NULL, pars1=NULL;
  ATermList init2=NULL, pars2=NULL;
  ATermList pars3=NULL;
  ATermList renaming=NULL;
  ATermList result=NULL;
  ATermList pars2renaming=NULL;
  
  init1=linGetInit(t1);
  init2=linGetInit(t2);
  pars1=linGetParameters(t1);
  pars2=linGetParameters(t2);

  renaming=construct_renaming(pars1,pars2,&pars3,&pars2renaming); 


  ATfprintf(stderr,"Parallel composition is being translated... ");

  ATfprintf(stderr,"%d   %d   ",
              ATgetLength(linGetSums(t1)),
              ATgetLength(linGetSums(t2)));

  result=combinesumlist(
               linGetSums(t1),
               linGetSums(t2),
               pars1,pars2renaming,pars3,renaming,spec,pars2);
  
  fprintf(stderr,"%d   done.\n",ATgetLength(result)); 
  return linMakeInitProcSpec(
               ATconcat(init1,
                        substitute_assignmentlist(
                                  renaming,
                                  pars2renaming,
                                  init2,
                                  pars3,
                                  1)),
               ATconcat(pars1,pars3),
               result);
}

/****************    single name    ******************************/

static ATermAppl namecomposition(
                     ATermAppl t1, 
                     ATermList args, 
                     ATermAppl t)
{ 
  long n=objectIndex(t1);
  ATermList init=linGetInit(t);
  ATermList pars=linGetParameters(t);
  ATermList sums=linGetSums(t); 
  return linMakeInitProcSpec(
                substitute_assignmentlist(args,objectdata[n].parameters,init,pars,0),
                pars,
                sums);
}

/**************** GENERATE LPEmCRL **********************************/


static ATermAppl generateLPEmCRL(
               ATermAppl procIdDecl,
               int canterminate,
               specificationbasictype *spec, 
               int regular);

static ATermAppl generateLPEmCRLterm(
                   ATermAppl t,
                   int canterminate,
                   specificationbasictype *spec,
                   int regular)
{ 
  if (gsIsProcess(t)) 
  { 
    ATermAppl t3=namecomposition(ATAgetArgument(t,0),ATLgetArgument(t,1),
                        generateLPEmCRL(
                              ATAgetArgument(t,0),
                              canterminate,
                              spec,
                              regular));
    return t3;
  }
  
  if (gsIsMerge(t)) 
  { ATermAppl t1=generateLPEmCRLterm(
                          ATAgetArgument(t,0),
                          canterminate,
                          spec,
                          regular);
    ATermAppl t2=generateLPEmCRLterm(
                          ATAgetArgument(t,1),
                          canterminate,
                          spec,
                          regular);
    return parallelcomposition(t1,t2,canterminate,spec);
  }
  
  if (gsIsHide(t)) 
  {  ATermAppl t2=generateLPEmCRLterm(
                          ATAgetArgument(t,1),
                          canterminate,
                          spec,
                          regular);
     return hidecomposition(ATLgetArgument(t,0),t2);
  }

  if (gsIsAllow(t)) 
  { ATermAppl t2=generateLPEmCRLterm(
                          ATAgetArgument(t,1),
                          canterminate,
                          spec,
                          regular);
    return allowcomposition(ATLgetArgument(t,0),t2);
  }

  if (gsIsRestrict(t)) 
  { ATermAppl t2=generateLPEmCRLterm(
                          ATAgetArgument(t,1),
                          canterminate,
                          spec,
                          regular);
    return encapcomposition(ATLgetArgument(t,0),t2);
  }
  
  if (gsIsRename(t))
  { ATermAppl t2=generateLPEmCRLterm(
                          ATAgetArgument(t,1),
                          canterminate,
                          spec,
                          regular);

    return renamecomposition(ATLgetArgument(t,0),t2);
  }

  if (gsIsComm(t))
  { ATermAppl t1=generateLPEmCRLterm(
                          ATAgetArgument(t,1),
                          canterminate,
                          spec,
                          regular);

    return communicationcomposition(ATLgetArgument(t,0),t1);
  }
   
  else ATerror("Expect mCRL term %t\n",t);

  return NULL;
}

/**************** Replace arguments by assignments  ******************/

static ATermList replaceArgumentsByAssignments(ATermList args,ATermList pars)
{ ATermList resultargs=ATempty;

  for( ; pars!=ATempty ; pars=ATgetNext(pars))
  { ATermAppl par=ATAgetFirst(pars);
    ATermAppl arg=ATAgetFirst(args);

    if (par!=arg)
    { /* The argument is different from the parameter. Therefore
         add Assignment(par,arg) to the outputlist. If a rewriter
         will become available, it might be useful to simplify
         arg first, to minimize the number of assignments */

      resultargs=ATinsertA(resultargs,gsMakeAssignment(par,arg));
    }
    args=ATgetNext(args); 
  }

  assert(args==ATempty);
  resultargs=ATreverse(resultargs);
  return resultargs;
}

static ATermAppl replaceArgumentsByAssignmentsIPS(ATermAppl ips)
{
  assert(linIsInitProcSpec(ips));
  ATermList parameters=linGetParameters(ips);
  ATermList init=replaceArgumentsByAssignments(linGetInit(ips),parameters);
  ATermList sums=linGetSums(ips);
  ATermList resultsums=ATempty;

  for( ; sums!=ATempty ; sums=ATgetNext(sums))
  { ATermAppl summand=ATAgetFirst(sums);
    assert(gsIsLPESummand(summand));
    ATermList DataVarIds=ATLgetArgument(summand,0);
    ATermAppl BoolExpr=ATAgetArgument(summand,1);
    ATermAppl MultAcOrDelta=ATAgetArgument(summand,2);
    ATermAppl TimeExprOrNil=ATAgetArgument(summand,3);
    ATermList Assignments=replaceArgumentsByAssignments(
                        ATLgetArgument(summand,4),
                        parameters);
    resultsums=ATinsertA(resultsums,
                      gsMakeLPESummand(
                          DataVarIds,
                          BoolExpr,
                          MultAcOrDelta,
                          TimeExprOrNil,
                          Assignments));
  }

  return linMakeInitProcSpec(init,parameters,resultsums);

}

/**************** GENERATE LPEmCRL **********************************/

static ATermAppl generateLPEmCRL(
                       ATermAppl procIdDecl,
                       int canterminate,
                       specificationbasictype *spec,
                       int regular)
{
/* generates a pair of a initial argument list and a
   linear process expression, representing the initial ATerm .
   If regular=1, then a regular version of the pCRL processes
   must be generated 

*/
  long n=objectIndex(procIdDecl);

  if ((objectdata[n].processstatus==GNF)||
      (objectdata[n].processstatus==pCRL)||
      (objectdata[n].processstatus==GNFalpha)||
      (objectdata[n].processstatus==multiAction))
  { ATermAppl t3=generateLPEpCRL(procIdDecl,
        (canterminate&&objectdata[n].canterminate),spec,regular);
    t3=replaceArgumentsByAssignmentsIPS(t3); 
    return t3;
  }
  /* process is a mCRLdone ATerm */
  if ((objectdata[n].processstatus==mCRLdone)||
              (objectdata[n].processstatus==mCRLlin)||
              (objectdata[n].processstatus==mCRL))
  { objectdata[n].processstatus=mCRLlin;
    return generateLPEmCRLterm(objectdata[n].processbody,
             (canterminate&&objectdata[n].canterminate),spec,
             regular); 
  }

  ATerror("Internal error. Laststatus: %d\n",objectdata[n].processstatus);
  return NULL;
}

/*********************** initialize_data **************/

static void initialize_data(void)
{ 
  ATprotectList(&pcrlprocesses);
  pcrlprocesses=ATempty;
  gsEnableConstructorFunctions();
  objectIndexTable=ATindexedSetCreate(1024,75);
  stringTable=ATindexedSetCreate(1024,75);
  objectdata=NULL;
  time_operators_used=0;
  seq_varnames=ATempty;
  enumeratedtypes=NULL;
  enumeratedtypelist=NULL;
  stacklist=NULL;
  ATprotectList(&sumlist);
  ATprotectList(&localequationvariables);
  
}

/**************** alphaconversion ********************************/


static ATermAppl alphaconversionterm(
                      ATermAppl t,
                      ATermList parameters,
                      ATermList varlist,
                      ATermList tl)
{ 
  if (gsIsChoice(t)) 
  { return gsMakeChoice(
              alphaconversionterm(ATAgetArgument(t,0),parameters,varlist,tl),
              alphaconversionterm(ATAgetArgument(t,1),parameters,varlist,tl));
  }  
  
  if (gsIsSeq(t)) 
  { return gsMakeSeq(
              alphaconversionterm(ATAgetArgument(t,0),parameters,varlist,tl),
              alphaconversionterm(ATAgetArgument(t,1),parameters,varlist,tl));
  }  
  
  if (gsIsSync(t)) 
  { return gsMakeSync(
              alphaconversionterm(ATAgetArgument(t,0),parameters,varlist,tl),
              alphaconversionterm(ATAgetArgument(t,1),parameters,varlist,tl));
  }  
  
  if (gsIsBInit(t)) 
  { return gsMakeBInit(
              alphaconversionterm(ATAgetArgument(t,0),parameters,varlist,tl),
              alphaconversionterm(ATAgetArgument(t,1),parameters,varlist,tl));
  }  
  
  if (gsIsMerge(t))
  { alphaconversionterm(ATAgetArgument(t,0),parameters,varlist,tl),
    alphaconversionterm(ATAgetArgument(t,1),parameters,varlist,tl); 
    return NULL;
  }  
  
  if (gsIsLMerge(t))
  { alphaconversionterm(ATAgetArgument(t,0),parameters,varlist,tl),
    alphaconversionterm(ATAgetArgument(t,1),parameters,varlist,tl); 
    return NULL;
  }  
  
  if (gsIsAtTime(t))
  { 
    return gsMakeAtTime(
              alphaconversionterm(ATAgetArgument(t,0),parameters,varlist,tl),
              substitute_data(tl,varlist,ATAgetArgument(t,1)));
  }  
  
  if (gsIsCond(t))
  { assert(gsIsDelta(ATAgetArgument(t,2)));
    return gsMakeCond(
              substitute_data(tl,varlist,ATAgetArgument(t,0)),
              alphaconversionterm(ATAgetArgument(t,1),parameters,varlist,tl),
              gsMakeDelta());
  }  
  
  if (gsIsSum(t)) 
  { ATermList sumvars=ATLgetArgument(t,0);

    alphaconvert(&sumvars,&varlist,&tl,ATempty,parameters);
    return gsMakeSum(sumvars,
                     alphaconversionterm(
                            ATAgetArgument(t,1),
                            ATconcat(sumvars,parameters),varlist,tl));
  }  
  
  if (gsIsProcess(t))
  { ATermAppl procId=ATAgetArgument(t,0);
    alphaconversion(procId,parameters);
    return gsMakeProcess(procId,
              substitute_datalist(tl,varlist,ATLgetArgument(t,1)));
  }  

  if (gsIsAction(t)) 
  { return gsMakeAction(ATAgetArgument(t,0),
              substitute_datalist(tl,varlist,ATLgetArgument(t,1)));
  }  
 
  if (gsIsMultAct(t))
  { return substitute_multiaction(tl,varlist,t);
  }

  if (gsIsDelta(t))
  { return t;
  }
  
  if (gsIsTau(t))
  { return t; 
  }
  
  if (gsIsHide(t))
  { alphaconversionterm(ATAgetArgument(t,1),parameters,varlist,tl);
    return NULL;
  }  
  
  if (gsIsRename(t))
  { alphaconversionterm(ATAgetArgument(t,1),parameters,varlist,tl);
    return NULL;
  }  

  if (gsIsComm(t))
  { alphaconversionterm(ATAgetArgument(t,1),parameters,varlist,tl);
    return NULL;
  }  
  
  if (gsIsAllow(t)) 
  { alphaconversionterm(ATAgetArgument(t,1),parameters,varlist,tl);
    return NULL; 
  }  
  
  if (gsIsRestrict(t))
  { alphaconversionterm(ATAgetArgument(t,1),parameters,varlist,tl);
    return NULL;
  }  
  
  ATerror("Internal: Unexpected process format in alphaconversionterm %t\n",t);
  return NULL;
}

static void alphaconversion(ATermAppl procId, ATermList parameters)
{ 
  long n=objectIndex(procId);

  if ((objectdata[n].processstatus==GNF)||
      (objectdata[n].processstatus==multiAction))
   { objectdata[n].processstatus=GNFalpha;
     objectdata[n].processbody=
       alphaconversionterm(objectdata[n].processbody,
            parameters,ATempty,ATempty);
   }
  else
  if (objectdata[n].processstatus==mCRLdone)
   { alphaconversionterm(objectdata[n].processbody,
            parameters,ATempty,ATempty);
     
   }
  else 
  if (objectdata[n].processstatus==GNFalpha)
     return;
  else ATerror("Unknown type %d in alphaconversion of %t\n",
                       objectdata[n].processstatus,procId); 
  return;
}

/***** determinewhetherprocessescanterminate(init); **********/

static int canterminate_rec(
              ATermAppl procId,
              int *stable,
              ATermIndexedSet visited);

static int canterminatebody(
              ATermAppl t,
              int *stable,
              ATermIndexedSet visited,
              int allowrecursion)
{ 
  if (gsIsMerge(t))
  { /* the construction below is needed to guarantee that 
       both subterms are recursively investigated */
    int r1=canterminatebody(ATAgetArgument(t,0),stable,visited,allowrecursion);
    int r2=canterminatebody(ATAgetArgument(t,1),stable,visited,allowrecursion);
    return r1&&r2;
  }

  if (gsIsProcess(t))
  { if (allowrecursion)
    { return (canterminate_rec(ATAgetArgument(t,0),stable,visited));
    }
    return objectdata[objectIndex(ATAgetArgument(t,0))].canterminate;
  }

  if (gsIsHide(t)) 
  { return (canterminatebody(ATAgetArgument(t,1),stable,visited,allowrecursion));
  }

  if (gsIsRename(t))
  { return (canterminatebody(ATAgetArgument(t,1),stable,visited,allowrecursion));
  }

  if (gsIsAllow(t))
  { return (canterminatebody(ATAgetArgument(t,1),stable,visited,allowrecursion));
  }

  if (gsIsRestrict(t))
  { return (canterminatebody(ATAgetArgument(t,1),stable,visited,allowrecursion));
  }

  if (gsIsComm(t))
  { return (canterminatebody(ATAgetArgument(t,1),stable,visited,allowrecursion));
  }

  if (gsIsChoice(t)) 
  { int r1=canterminatebody(ATAgetArgument(t,0),stable,visited,allowrecursion);
    int r2=canterminatebody(ATAgetArgument(t,1),stable,visited,allowrecursion);
    return r1||r2;
  }

  if (gsIsSeq(t))
  { int r1=canterminatebody(ATAgetArgument(t,0),stable,visited,allowrecursion);
    int r2=canterminatebody(ATAgetArgument(t,1),stable,visited,allowrecursion);
    return r1&&r2;
  }

  if (gsIsCond(t))
  { int r1=canterminatebody(ATAgetArgument(t,1),stable,visited,allowrecursion);
    int r2=canterminatebody(ATAgetArgument(t,2),stable,visited,allowrecursion);
    return r1||r2;
  }

  if (gsIsSum(t))
  { return (canterminatebody(ATAgetArgument(t,1),stable,visited,allowrecursion));
  }

  if (gsIsAction(t)) 
  { return 1;
  }

  if (gsIsMultAct(t)) 
  { return 1;
  }

  if (gsIsDelta(t))
  { return 0;
  }

  if (gsIsTau(t))
  { return 1;
  }

  if (gsIsAtTime(t))
  { return canterminatebody(ATAgetArgument(t,0),stable,visited,allowrecursion);
  }

  if (gsIsSync(t))
  { int r1=canterminatebody(ATAgetArgument(t,0),stable,visited,allowrecursion);
    int r2=canterminatebody(ATAgetArgument(t,1),stable,visited,allowrecursion);
    return r1&&r2;
  }

  ATerror("Internal Error. Unexpected process format in canterminate%t\n",t);
  return 0;
}

static int canterminate_rec(
              ATermAppl procId, 
              int *stable,
              ATermIndexedSet visited)
{ long n=objectIndex(procId);
  ATbool new=0;
 
  if (visited!=NULL)
  { ATindexedSetPut(visited,(ATerm)procId,&new);
  }
  if (new)
  { int ct=canterminatebody(objectdata[n].processbody,stable,visited,1); 
    if (objectdata[n].canterminate!=ct)
    { objectdata[n].canterminate=ct;
      if (stable!=NULL) 
      { *stable=0;
      }
    }
  }
  return (objectdata[n].canterminate);
}

static void determinewhetherprocessescanterminate(ATermAppl procId)
{ int stable=0;
  ATermIndexedSet visited=ATindexedSetCreate(64,50);
  while (!stable) 
  { stable=1;
    canterminate_rec(procId,&stable,visited);
    ATindexedSetReset(visited);
  }
  ATindexedSetDestroy(visited);
}

/*****  distinguishmCRLandpCRLprocsAndAddTerminatedAction  ******/

static ATermAppl split_body(
                     ATermAppl t, 
                     ATermTable visited,
                     ATermList parameters);

static ATermAppl split_process(ATermAppl procId, ATermTable visited)
{ 
  long n=objectIndex(procId);
  ATermAppl newProcId=NULL;
  ATermAppl result=(ATermAppl)ATtableGet(visited,(ATerm)procId);

  if (result!=NULL)
  { if (objectdata[n].processstatus==mCRL)
    { ATerror("Unguarded recursion in the mCRL part of the input\n");
    }
    assert((objectdata[n].processstatus==pCRL) ||
           (objectdata[n].processstatus==multiAction));
    return result;
  }

  if ((objectdata[n].processstatus!=mCRL)&&
         (objectdata[n].canterminate==0))
  { /* no new process needs to be constructed */
    return procId;
  }

  newProcId=gsMakeProcVarId(
              fresh_name(gsATermAppl2String(ATAgetArgument(procId,0))),
              ATLgetArgument(procId,1));
  

  if (objectdata[n].processstatus==mCRL)
  { 
    ATtablePut(visited,(ATerm)procId,(ATerm)newProcId);  
    insertProcDeclaration(
                newProcId,
                objectdata[n].parameters,
                split_body(objectdata[n].processbody,
                           visited,
                           objectdata[n].parameters),
                mCRL,0);
    return newProcId;
  }

  if (objectdata[n].canterminate)
  { ATtablePut(visited,(ATerm)procId,(ATerm)newProcId);
    insertProcDeclaration(
                newProcId,
                objectdata[n].parameters,
                gsMakeSeq(objectdata[n].processbody,
                          gsMakeProcess(terminatedProcId,ATempty)),
                pCRL,canterminatebody(objectdata[n].processbody,NULL,NULL,0)); 
    return newProcId;
  }
  ATtablePut(visited,(ATerm)procId,(ATerm)procId);
  return procId;
}

static ATermAppl split_body(
                    ATermAppl t, 
                    ATermTable visited,
                    ATermList parameters)
{
  ATermAppl result=NULL;

  result=(ATermAppl)ATtableGet(visited,(ATerm)t);

  if (result==NULL)
  { if (gsIsMerge(t))
    { result=gsMakeMerge(
                  split_body(ATAgetArgument(t,0),visited,parameters),
                  split_body(ATAgetArgument(t,1),visited,parameters));
    }
    else
    if (gsIsProcess(t))
    { result=gsMakeProcess(
                   split_process(ATAgetArgument(t,0),visited),
                   ATLgetArgument(t,1));
    }
    else
    if (gsIsHide(t))
    { result=gsMakeHide(
                   ATLgetArgument(t,0),
                   split_body(ATAgetArgument(t,1),visited,parameters));
    }
    else 
    if (gsIsRename(t))
    { result=gsMakeRename(
                   ATLgetArgument(t,0),
                   split_body(ATAgetArgument(t,1),visited,parameters));
    }
    else
    if (gsIsAllow(t))
    { result=gsMakeAllow(
                   ATLgetArgument(t,0),
                 split_body(ATAgetArgument(t,1),visited,parameters));
    }
    else
    if (gsIsRestrict(t))
    { result=gsMakeRestrict(
                   ATLgetArgument(t,0),
                   split_body(ATAgetArgument(t,1),visited,parameters));
    }
    else
    if (gsIsComm(t))
    { result=gsMakeComm(
                   ATLgetArgument(t,0),
                   split_body(ATAgetArgument(t,1),visited,parameters));
    }
    else
    if (gsIsChoice(t)||
        gsIsSeq(t)||
        gsIsCond(t)||
        gsIsSum(t)||
        gsIsAction(t)||
        gsIsDelta(t)||
        gsIsTau(t)||
        gsIsAtTime(t)||
        gsIsSync(t))
    { 
      if (canterminatebody(t,NULL,NULL,0))
      { ATermAppl p=newprocess(parameters,
                               gsMakeSeq(t,gsMakeProcess(terminatedProcId,ATempty)),
                               pCRL,
                               0);
        result=gsMakeProcess(p,objectdata[objectIndex(p)].parameters);
      }
      else
      { ATermAppl p=newprocess(parameters,t,pCRL,0);
        result=gsMakeProcess(p,objectdata[objectIndex(p)].parameters);
      }
    }
    else
    ATerror("Internal Error. Unexpected process format in split process %t\n",t);
  }

  ATtablePut(visited,(ATerm)t,(ATerm)result);
  return result;
}


static ATermAppl splitmCRLandpCRLprocsAndAddTerminatedAction(ATermAppl procId)
{
  ATermTable visitedprocesses=ATtableCreate(1024,75);
  ATermAppl result;

  result=split_process(procId,visitedprocesses);
  ATtableDestroy(visitedprocesses);
  return result;

}

/**************** AddTerminationActionIfNecessary ****************/

static void AddTerminationActionIfNecessary(
                 specificationbasictype *spec,
                 ATermAppl ips)
{ ATermList summands=ATLgetArgument(ips,2);

  for ( ; summands!=ATempty ; summands=ATgetNext(summands))
  { ATermAppl summand=ATAgetFirst(summands);
    ATermAppl multiaction=ATAgetArgument(summand,2);
    if (multiaction==terminationAction)
    { spec->acts=ATinsertA(spec->acts,
          ATAgetArgument(
            ATAgetFirst(ATLgetArgument(terminationAction,0)),
            0));
      return;
    }
  }
}

/**************** transform **************************************/

static ATermAppl transform(
                     ATermAppl init, 
                     specificationbasictype *spec)
{ ATermList pcrlprocesslist;
  ATermAppl t3=NULL;

  /* Then select the BPA processes, and check that the others
     are proper parallel processes */
  determine_process_status(init,mCRL);
  determinewhetherprocessescanterminate(init);
  init=splitmCRLandpCRLprocsAndAddTerminatedAction(init);
  pcrlprocesslist=collectPcrlProcesses(init);

  if (pcrlprocesslist==ATempty) 
  { ATerror("There are no pCRL processes to be linearized\n"); }

  /* Second, transform into GNF with possibly variables as a head,
     but no actions in the tail */
  procstovarheadGNF(pcrlprocesslist);
  pcrlprocesslist=NULL;
  /* Third, transform to GNF by subsitution, such that the
     first variable in a sequence is always an actionvariable */
  procstorealGNF(init,regular);
  
  t3=generateLPEmCRL(
           init,
           objectdata[objectIndex(init)].canterminate,
           spec,
           regular);
  if (cluster)
     t3=clusterfinalresult(t3,spec);

  AddTerminationActionIfNecessary(spec,t3);
  return t3;
}

/*--- main program -----------------------------*/

static int main2(int argc, char *argv[],ATerm *stack_bottom)
{ 
  int i = 1;
  char *sname = NULL, *oname = NULL;
  specificationbasictype *spec;
  char messagebuffer[STRINGLENGTH]="Unitialized messagebuffer";
  ATermAppl initial_process=NULL;
  ATermAppl result=NULL;
  char fname[STRINGLENGTH], iname[STRINGLENGTH];
  
  fname[0]='\0';
  to_toolbusfile=0;
  to_stdout=0;
  for(i = 1; i < argc; i++){
    if(strequal(argv[i], "-version")){
      version(); exit(0);
    } else if(strequal(argv[i], "-help")){
      help(); exit(0);
    } else if (strequal(argv[i], "-stack")){
      if (to_stdout==1)
         ATerror("Options -stack and -stdout cannot be used together\n");
      to_toolbusfile=1;
    } else if(strequal(argv[i], "-stdout")){
      to_toolbusfile=0;
      to_stdout=1;
    } else if(strequal(argv[i], "-regular")){
      regular=1;
    } else if(strequal(argv[i], "-regular2")){
      regular2=1;
      regular=1;
    } else if(strequal(argv[i], "-cluster")){
      cluster=1;
      binary=1;
      fprintf(stderr,"lineariser: -cluster also sets -binary\n");
    } else if(strequal(argv[i], "-nocluster")){
      nocluster=1;
    } else if(strequal(argv[i], "-binary")){
      binary=1;
    } else if(strequal(argv[i], "-newstate")){
      oldstate=0;
    } else if(strequal(argv[i], "-nofreevars")){
      allowFreeDataVariablesInProcesses=0;
    } else if(strequal(argv[i], "-statenames")){
      statenames=1;
    } else if(strequal(argv[i], "-no-rewrite")){
      mayrewrite=0;
    } else if(strequal(argv[i], "-at-termtable")){
      i++;
    } else if(strequal(argv[i], "-at-symboltable")){
      i++;
    } else if((argv[i][0]=='-') && (argv[i][1]=='a') && (argv[i][2]=='t')){ 

    } else if(argv[i][0]=='-'){
      fprintf(stderr,"Unknown option %s ignored\n",argv[i]);
    } else {
      char *lastdot = NULL;
      sname = argv[i];
      strcpy(fname, sname);
      oname = fname;
      if ((strlen(fname)>3) && (strrchr(fname,'/')!=NULL))
         oname = strrchr(oname,'/')+1;
      lastdot = strrchr(oname,'.');
      if (lastdot && !strcmp(lastdot,".gs")) *lastdot = '\0';   
      break; 
    } 
  }
  if (to_stdout==0 && 
     (regular || nocluster || cluster || binary || !oldstate)) 
      to_toolbusfile=1; 
  if (!oldstate && !regular && !regular2)
    ATerror("Option -newstate can only be used with -regular or -regular2\n");

  ATinit(argc,argv,stack_bottom);
  ATprotectAppl(&result);
  if (((argc < 2)||(sname==NULL)))
      usage();
  initialize_data();

  strcpy(iname, sname);
  infile=fopen(iname,"r");
  if (infile==NULL) { 
     if (ExtensionAdded(iname, ".gs")) {
           infile=fopen(iname,"r");
          }
     }
  if (infile==NULL)
        ATerror ("Cannot open input file `%s'\n", iname);
  fclose(infile);
  if (to_toolbusfile)
   { snprintf(messagebuffer,STRINGLENGTH,"%s.lpo",oname);
     toolbusfile=fopen(messagebuffer,"w");
     if (toolbusfile==NULL)
        ATerror("Cannot open file for output\n"); }
    spec=read_input_file(iname); 

    initial_process=spec->init;
    initialize_symbols(); /* This must be done after storing the data,
                             to avoid a possible name conflict with action
                             Terminate */
    if ((to_toolbusfile)||(to_stdout))
    { result=transform(initial_process,spec);
      ATwriteToTextFile(
         (ATerm)gsMakeSpecV1(
                 gsMakeSortSpec(spec->sorts),
                 gsMakeConsSpec(spec->funcs),
                 gsMakeMapSpec(spec->maps),
                 gsMakeDataEqnSpec(spec->eqns),
                 gsMakeActSpec(spec->acts),
                 gsMakeLPE(spec->procdatavars, ATLgetArgument(result,1),ATLgetArgument(result,2)),
                 gsMakeLPEInit(spec->procdatavars, ATLgetArgument(result,0))),
                 to_stdout?stdout:toolbusfile);
    }
  else ATfprintf(stderr,"The file %s contains a correctly typed mCRL2 specification\n",
                          iname);  
  return 0;
}

int main(int argc, char *argv[])
{
  ATerm stack_bottom;
  return main2(argc,argv,&stack_bottom);
}

