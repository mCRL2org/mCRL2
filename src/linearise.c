
/* $Id: main.c,v 1.2 2004/11/23 12:36:17 uid523 Exp $ */

/* TODO:
 * Include time.
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

int to_toolbusfile=0;
int to_stdout=0;
int newtbf = 1;
int regular=0;
int regular2=0;
int cluster=0;
int nocluster=0;
int binary=0;
int oldstate=1;
int statenames=0;
int writemulti=0;

ATermAppl gsMakeOpIdEq(ATermAppl);

/* The writemulti variable can be set to 1 to indicate that
 * the init expression should not be evualated, but should be written
 * to file unevaluated. The result is multi LPO format.
 * The format is the similar to 2gen, but its process is a composition
 * of linear processes rather than a single linear process.
 *
 * Stefan Blom 6-2003
 */

int cid=0;

FILE *outfile;
FILE *toolbusfile;
FILE *infile;

#define P(msg)  fprintf(stderr,"%s\n",msg)

void usage(void)
{
  ATerror("Use mcrl -help for options");
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
P("-tbfile:   an LPO of the input file in toolbus term format file is ");
P("           written to file.tbf");
P("-tbf:      has the same effect as -tbfile");
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

static ATermAppl uniqueterm(ATermAppl sort);
static int occursintermlist(ATermAppl var, ATermList l);
static int occursinpCRLterm(ATermAppl var, ATermAppl p, int strict);
static ATermAppl getfreshvariable(char *s,ATermAppl sort);
static ATermList construct_renaming(ATermList pars1, ATermList pars2,
                ATermList *pars3, ATermList *pars4);
static void alphaconversion(ATermAppl procId, ATermList parameters);
static ATermAppl fresh_name(char *name);
static ATermAppl linMakeMultiAction(ATermList actionlist);

static ATermList ATinsertA(ATermList l, ATermAppl a)
{ return ATinsert(l,(ATerm)a);
}

static char *ATSgetArgument(ATermAppl appl, int nr)
{ return gsATermAppl2String(ATAgetArgument(appl, nr));
}

extern FILE *outfile;
int time_operators_used=0;
static ATermList seq_varnames=NULL;

char scratch1[ERRORLENGTH];

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
    { ATerror("Fail to resize objectdata to %d",2*maxobject); }
    
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
  strncpy(&c->s[0],"",MAXLEN);
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
  strncpy(&c->s[0],s,MAXLEN);
  return c;
}

/*****************  store and retrieve basic objects  ******************/
  

static long addObject(ATermAppl o, ATbool *isnew)
{ return ATindexedSetPut(objectIndexTable,(ATerm)o,isnew);
}

static long objectIndex(ATermAppl o)
{ return ATindexedSetGetIndex(objectIndexTable,(ATerm)o);
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

static int existsort(ATermAppl sortterm)
/* Delivers 0 if sort does not exists. Otherwise 1 
   indicating that the sort exists */
{ 
  if (sortterm==gsMakeSortExprBool()) return 1;
  if (sortterm==gsMakeSortExprInt()) return 1;
  if (sortterm==gsMakeSortExprNat()) return 1;
  if (sortterm==gsMakeSortExprPos()) return 1;
  if (gsIsSortArrow(sortterm))
  { return existsort(ATAgetArgument(sortterm,0)) && 
             existsort(ATAgetArgument(sortterm,1));
  }
  if (gsIsSortList(sortterm))
  { ATerror("SortList is not an implemented sort");
  }
  if (gsIsSortSet(sortterm))
  { ATerror("SortSet is not an implemented sort");
  }
  if (gsIsSortBag(sortterm))
  { ATerror("SortBag is not an implemented sort");
  }
  if (gsIsSortId(sortterm)) 
  { 
    long n=0;

    n=objectIndex(sortterm);
    if (n<0) return 0;
    if (objectdata[n].object==sort) return 1;
    return 0;
  }
  gsErrorMsg("Internal: Expected a sortterm %t",sortterm);
  return 0;
}

static void insertsort(ATermAppl sortterm)
{ 
  if (sortterm==gsMakeSortExprBool()) return;
  if (sortterm==gsMakeSortExprInt()) return;
  if (sortterm==gsMakeSortExprNat()) return;
  if (sortterm==gsMakeSortExprPos()) return;
  if (gsIsSortArrow(sortterm))
  { insertsort(ATAgetArgument(sortterm,0));
    insertsort(ATAgetArgument(sortterm,1));
    return;
  }
  if (gsIsSortList(sortterm))
  { ATerror("SortList is not an implemented sort");
  }
  if (gsIsSortSet(sortterm))
  { ATerror("SortSet is not an implemented sort");
  }
  if (gsIsSortBag(sortterm))
  { ATerror("SortBag is not an implemented sort");
  }
  if (gsIsSortId(sortterm)) 

  {
    long n=0;
    ATbool isnew=0;
    char *str=ATSgetArgument(sortterm,0);
    addString(str);
 
    n=addObject(sortterm,&isnew);
 
    if (isnew==0)
    { ATerror("Sort %t is added twice",sortterm); }
 
    newobject(n);

    objectdata[n].objectname=sortterm;
    objectdata[n].object=sort;
    objectdata[n].constructor=0;
    return;
  }
  ATerror("Expected a sortterm %t",sortterm);
}

static long insertConstructorOrFunction(ATermAppl constructor,objecttype type)
{ ATbool isnew=0;
  ATermAppl t=NULL;
  char *str=NULL;
  long m=0;
  long n=0;

  if (!gsIsOpId(constructor)) /* ,"OpId(<str>,<term>)",&str,&t)) */
  { ATerror("Internal: Expect operation declaration %t",constructor);
  };

  str=ATSgetArgument(constructor,0);
  t=ATAgetArgument(constructor,1);

  addString(str);
 
  n=addObject(constructor,&isnew);

  if (isnew==0)
  { ATerror("Function %t is added twice",constructor); }

  newobject(n);

  objectdata[n].objectname=constructor;
  objectdata[n].targetsort=getTargetSort(t);
  m=objectIndex(getTargetSort(ATAgetArgument(constructor,1)));
  assert(objectdata[m].object==sort);
  if (type==func) objectdata[m].constructor=1;
  objectdata[n].object=type;
  assert(existsort(ATAgetArgument(constructor,1)));
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

  for(ATermList l=ATLgetArgument(multiAction,0) ;
                l!=ATempty ;
                l=ATgetNext(l))
  { for(ATermList l1=ATLgetArgument(ATAgetArgument(ATAgetFirst(l),0),1) ;
                  l1!=ATempty ;
                  l1=ATgetNext(l1))
    { result=ATinsertA(result,
               gsMakeDataVarId(
                     fresh_name("a"),
                     ATAgetArgument(ATAgetFirst(l1),1)));
    }
  }
  return ATreverse(result);
}

static ATermList getarguments(ATermAppl multiAction)
{ ATermList result=ATempty;

  for(ATermList l=ATLgetArgument(multiAction,1) ;
                l!=ATempty ;
                l=ATgetNext(l))
  { for(ATermList l1=ATLgetArgument(ATAgetFirst(l),1) ;
                  l1!=ATempty ;
                  l1=ATgetNext(l1))
    { result=ATinsertA(result,ATAgetFirst(l1));
    }
  }
  ATreverse(result);
}

static ATermAppl makemultiaction(ATermList actionIds,ATermList args)
{ 
  ATermList result=ATempty;
  for(ATermList l=actionIds ; l!=ATempty ; l=ATgetNext(l))
  { ATermAppl actionId=ATAgetFirst(l);
    long arity=ATgetLength(ATLgetArgument(actionId,1));
    result=ATinsertA(result,
               gsMakeAction(ATAgetFirst(l),
                            ATgetSlice(args,0,arity-1)));
    args=ATgetTail(args,arity);
  }
  return linMakeMultiAction(ATreverse(result));
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

static int existsvariable(ATermAppl var)
/* Delivers -1 if the variable does not exists. 
   Otherwise a number>=0 is returned,
   indicating the index of the variable */
{ long n=objectIndex(var);
  if (n<0) return -1;
  if (objectdata[n].object==variable) return n;
  return -1;
}

static void insertvariable(ATermAppl var)
{ 
  ATbool isnew=0;
  long n=0;
  ATermAppl t=NULL;
  char *str=NULL;

  if (!gsIsDataVarId(var))
  { ATerror("Expect variable declaration %t",var);
  };

  str=ATSgetArgument(var,0);
  t=ATAgetArgument(var,1);

  addString(str);

  n=addObject(var,&isnew);

  if (isnew==0)
  { ATerror("Variable %t already exists",var); }

  newobject(n);

  objectdata[n].objectname=var;
  objectdata[n].object=variable;

#ifndef NDEBUG
  if (existsort(ATAgetArgument(var,1))<0)
  { ATerror("Variable %t has unknown sort",var); }
#endif
} 

static void removevariable(ATermAppl var)
{ long n=objectIndex(var);
  if (n<0) 
  { ATerror("Cannot remove a non existing variable %t",var); }
  if (objectdata[n].object!=variable) 
  { ATerror("Data structure is confused %t",var); }
  objectdata[n].object=none;
  removeObject(var);
}

static void resetvariables(ATermList vars)
{ /* remove the variables in reverse order */

  if (ATisEmpty(vars)) return;

  resetvariables(ATgetNext(vars));
  removevariable(ATAgetFirst(vars));
}

static void declarevariables(ATermList vars)
{ 
  for( ; !ATisEmpty(vars) ; vars=ATgetNext(vars))
  { insertvariable(ATAgetFirst(vars)); }
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

/************ storesig ****************************************************/

static void storesig(specificationbasictype *spec)
{ 
  ATermList sorts=spec->sorts;
  ATermList constr=spec->funcs; 
  ATermList maps=spec->maps;


/* First store the sorts */
  
  for( ; sorts!=ATempty ; sorts=ATgetNext(sorts) )
  { insertsort(ATAgetFirst(sorts)); 
  }
          
/* Now store the constructors */
  for( ; !ATisEmpty(constr) ; constr=ATgetNext(constr) )
  { insertconstructor(ATAgetFirst(constr),spec); 
  }

                 
/* Finally store the functions */
  for( ; !ATisEmpty(maps) ; maps=ATgetNext(maps) )
  { insertmapping(ATAgetFirst(maps),spec); 
  }
}

/************ storeact ****************************************************/

static long insertAction(ATermAppl actionId)
{ ATbool isnew=0;
  long n=addObject(actionId,&isnew);
  char *str=NULL;

  if (isnew==0)
  { ATerror("Action %t is added twice",actionId); }

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
      { ATerror("The term %t is not a sort in the declaration of actionId %t",
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

/************ read_internal_format  **********************************/

static specificationbasictype *read_internal_format(char *filename) 
{ FILE *inputfile;
  ATerm t=NULL;
  specificationbasictype *spec=malloc(sizeof(specificationbasictype));
  
  if (spec==NULL)
  { ATerror("Cannot allocate memory for elementary operations"); }

  inputfile=fopen(filename,"r");
  if (inputfile==NULL)
     ATerror("Fail to open inputfile %s",filename);

  t=ATreadFromFile(inputfile);
  
  if (!ATmatch(t,"SpecV1(SortSpec(<term>),ConsSpec(<term>),MapSpec(<term>),DataEqnSpec(<term>),ActSpec(<term>),ProcEqnSpec(<term>),Init(<term>))",
        &spec->sorts,
        &spec->funcs,
        &spec->maps,
        &spec->eqns,
        &spec->acts,
        &spec->procs,
        &spec->init))
  { ATerror("ATerm in %s does not have the expected format: %t",filename,t); 
  }

  return spec;

}

/************ storeprocs *************************************************/


static long insertProcDeclaration(
                  ATermAppl procId,
                  ATermList parameters,
                  ATermAppl body,
                  processstatustype s)
{ ATbool isnew=0;
  long n=0;
  char *str=NULL;

  if (!gsIsProcVarId(procId)) 
  { ATerror("Expect process declaration %t",procId);
  }

  str=ATSgetArgument(procId,0);
  addString(str);

  n=addObject(procId,&isnew);

  if (isnew==0)
  { ATerror("Process %t is added twice",procId); }

  newobject(n);

  objectdata[n].objectname=procId;
  objectdata[n].object=proc;
  objectdata[n].processbody=body;
  objectdata[n].processstatus=s;
  objectdata[n].parameters=parameters;
#ifndef NDEBUG
  { for(ATermList l=ATLgetArgument(procId,1); !ATisEmpty(l); 
                  l=ATgetNext(l))
    { 
      if (existsort(ATAgetFirst(l))<0)
      { ATerror("The term %t is not a sort in the declaration of action %t",
                              ATgetFirst(l),procId); }
    }
  }
#endif
  return n;
}


static void storeprocs(ATermList procs)
{ 
  for( ; !ATisEmpty(procs) ; procs=ATgetNext(procs))
  { ATermAppl p=ATAgetFirst(procs);
    insertProcDeclaration(
            ATAgetArgument(p,0),
            ATLgetArgument(p,1),
            ATAgetArgument(p,2),
            unknown); 
  }
} 

/************ storeinit *************************************************/


static ATermAppl storeinit(ATermAppl init)
{ /* init is used as the name of the initial process,
     because it cannot occur as a string in the input */

  ATermAppl initprocess=gsMakeProcVarId(
                         gsString2ATermAppl("init"),ATempty);
  insertProcDeclaration(
           initprocess,ATempty,init,unknown);
  return initprocess;
}

/********** basic symbols and local term constructors *****************/

static AFun summand_symbol=0, noTime_symbol=0, 
       terminated_symbol=0, delta_symbol=0, 
       initprocspec_symbol=0, multiAction_symbol=0,
       triple_symbol=0;
static ATermAppl noTime=NULL; 
static ATermList terminated=NULL;
static ATermList delta=NULL;
static ATermAppl terminationAction=NULL;
static ATermAppl terminatedProc=NULL;

static void initialize_symbols(void)
{ 
  triple_symbol=ATmakeAFun("triple",3,ATfalse);
  ATprotectAFun(triple_symbol);
  multiAction_symbol=ATmakeAFun("multiAction",1,ATfalse);
  ATprotectAFun(multiAction_symbol);
  initprocspec_symbol=ATmakeAFun("initprocspec",3,ATfalse);
  ATprotectAFun(initprocspec_symbol);
  summand_symbol=ATmakeAFun("smd",5,ATfalse);
  ATprotectAFun(summand_symbol);
  noTime_symbol=ATmakeAFun("noTime",0,ATfalse);
  ATprotectAFun(noTime_symbol);
  ATprotect((ATerm *)&noTime);
  noTime=ATmakeAppl0(noTime_symbol);
  terminated_symbol=ATmakeAFun("terminated",0,ATfalse);
  ATprotectAFun(terminated_symbol);
  ATprotect((ATerm *)&terminated);
  delta=(ATermList)ATmakeAppl0(delta_symbol);
  delta_symbol=ATmakeAFun("delta",0,ATfalse);
  ATprotectAFun(delta_symbol);
  ATprotect((ATerm *)&delta);
  delta=(ATermList)ATmakeAppl0(delta_symbol);
  ATprotect((ATerm *)&terminationAction);
  terminationAction=gsMakeActId(gsString2ATermAppl("Terminate"),ATempty);
  ATprotect((ATerm *)&terminatedProc);
  terminatedProc=gsMakeProcVarId(gsString2ATermAppl("Terminated**"),ATempty);
  insertProcDeclaration(
           terminatedProc,
           ATempty,
           gsMakeSeq(terminationAction,gsMakeDelta()),
           pCRL);
}

static ATermAppl linMakeTriple(
                    ATermList left, 
                    ATermList middle, 
                    ATermAppl right)
{ return ATmakeAppl3(
             triple_symbol,
             (ATerm)left,
             (ATerm) middle, 
             (ATerm)right);
}

static ATermAppl linMakeSummand(
                         ATermList varargs,
                         ATermList multiAction,
                         ATermAppl actionTime,
                         ATermAppl condition,
                         ATermList nextstate)
{ return ATmakeAppl5(summand_symbol,
                     (ATerm)varargs,
                     (ATerm)multiAction,
                     ((actionTime==noTime)?(ATerm)noTime:(ATerm)actionTime),
                     (ATerm)condition,
                     ((nextstate==terminated)?(ATerm)terminated:
                                              (ATerm)nextstate));
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

static int linIsMultiAction(ATermAppl multiAction)
{ return ATgetAFun(multiAction)==multiAction_symbol;
}

static ATermAppl linMakeMultiAction(ATermList actionlist)
{ return ATmakeAppl1(multiAction_symbol,(ATerm)actionlist);
}

static ATermAppl linInsertActionInMultiAction(
                           ATermAppl action,
                           ATermAppl multiAction)
{ assert(linIsMultiAction(multiAction));
  return linMakeMultiAction(
              linInsertActionInMultiActionList(
                      action,
                      ATLgetArgument(multiAction,1)));

}

static ATermList linMergeMultiActionList(ATermList ma1, ATermList ma2)
{
 
  for( ; ma1!=ATempty ; ma1=ATgetNext(ma1))
  { ma2=linInsertActionInMultiActionList(ATAgetFirst(ma1),ma2);
  }
  
  return ma2;
}

static ATermAppl linMergeMultiAction(ATermAppl ma1, ATermAppl ma2)
{
  assert(linIsMultiAction(ma1));
  assert(linIsMultiAction(ma2));
 
  return linMakeMultiAction(
                linMergeMultiActionList(
                      ATLgetArgument(ma1,0),
                      ATLgetArgument(ma2,0)));
}

static int linIsSummand(ATermAppl summand)
{ return ATgetAFun(summand)==summand_symbol;
}

static ATermList linGetSumVars(ATermAppl summand)
{ assert(linIsSummand(summand));
  return ATLgetArgument(summand,0);
}

static ATermList linGetMultiAction(ATermAppl summand)
{ assert(linIsSummand(summand));
  return ATLgetArgument(summand,1);
}

static ATermAppl linGetActionTime(ATermAppl summand)
{ 
  assert(linIsSummand(summand));
  return ATAgetArgument(summand,2);
}

static ATermAppl linGetCondition(ATermAppl summand)
{ assert(linIsSummand(summand));
  return ATAgetArgument(summand,3);
}

static ATermList linGetNextState(ATermAppl summand)
{ 
  assert(linIsSummand(summand));
  
  return ATLgetArgument(summand,4);
}

static int linIsNoTime(ATermAppl actionTime)
{ return actionTime==noTime;
}

static int linIsTerminated(ATermList nextState)
{ return nextState==terminated;
}

static int linIsDelta(ATermList multiAction)
{ return multiAction==delta;
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
    { ATerror("Choice operator occurs in a multi-action");
    }
    s1=determine_process_statusterm(ATAgetArgument(body,0),pCRL);
    s2=determine_process_statusterm(ATAgetArgument(body,1),pCRL);
    if ((s1==mCRL)||(s2==mCRL))
    { ATerror("mCRL operators occur within the scope of a choice operator");
    }
    return pCRL;
  }

  if (gsIsSeq(body))
  { if (status==multiAction) 
    { ATerror("Sequential operator occurs in a multi-action");
    }
    s1=determine_process_statusterm(ATAgetArgument(body,0),pCRL);
    s2=determine_process_statusterm(ATAgetArgument(body,1),pCRL);
    if ((s1==mCRL)||(s2==mCRL))
    { ATerror("mCRL operators occur within the scope of a sequential operator");
    }
    return pCRL;
  }

  if (gsIsMerge(body))
  { if (status!=mCRL)
    { ATerror("The parallel operator occurs in the scope of pCRL operators");
    }
    s1=determine_process_statusterm(ATAgetArgument(body,0),mCRL);
    s2=determine_process_statusterm(ATAgetArgument(body,1),mCRL);
    return mCRL;
  }

  if (gsIsLMerge(body))
  { ATerror("Cannot linearize because the specification contains a leftmerge");
  }

  if (gsIsCond(body))  
  { if (status==multiAction) 
    { ATerror("If-then(-else) occurs in a multi-action");
    }
    s1=determine_process_statusterm(ATAgetArgument(body,1),pCRL);
    s2=determine_process_statusterm(ATAgetArgument(body,2),pCRL);
    if ((s1==mCRL)||(s2==mCRL))
    { ATerror("mCRL operators occur in the scope of the if-then-else operator");
    }
    return pCRL;
  }

  if (gsIsSum(body)) 
  { if (status==multiAction)
    { ATerror("Sum operator occurs within a multi-action");
    }
    s1=determine_process_statusterm(ATAgetArgument(body,1),pCRL);
    if (s1==mCRL)
    { ATerror("mCRL operators occur in the scope of the sum operator");
    }
    return pCRL;
  }

  if (gsIsComm(body))
  { if (status!=mCRL)
    { ATerror("The communication operator occurs in the scope of pCRL operators");
    }
    s2=determine_process_statusterm(ATAgetArgument(body,1),mCRL);
    return mCRL;
  }

  if (gsIsBInit(body))
  { ATerror("Cannot linearize a specification with the bounded initialization operator");
  }

  if (gsIsAtTime(body)) 
  { if (status==multiAction)
    { ATerror("A time operator occurs in a multi-action");
    }
    s1=determine_process_statusterm(ATAgetArgument(body,0),pCRL);
    if ((s1==mCRL)||(s2==mCRL))
    { ATerror("A mCRL operator occurs in the scope of a time operator");
    }
    return pCRL;
  }

  if (gsIsSync(body))
  { 
    s1=determine_process_statusterm(ATAgetArgument(body,1),pCRL);
    s2=determine_process_statusterm(ATAgetArgument(body,2),pCRL);
    if ((s1!=multiAction)||(s2!=multiAction))
    { ATerror("Other objects than multi-actions occur in the scope of a synch operator");
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
    { ATerror("A hide operator occurs in the scope of pCRL operators");
    }
    s1=determine_process_statusterm(ATAgetArgument(body,1),mCRL);
    return mCRL;
  }

  if (gsIsRename(body))
  { if (status!=mCRL) 
    { ATerror("A rename operator occurs in the scope of pCRL operators");
    }
    s1=determine_process_statusterm(ATAgetArgument(body,1),mCRL);
    return mCRL;
  }

  if (gsIsAllow(body))
  { if (status!=mCRL) 
    { ATerror("An allow operator occurs in the scope of pCRL operators");
    }
    s1=determine_process_statusterm(ATAgetArgument(body,1),mCRL);
    return mCRL;
  }

  if (gsIsRestrict(body))
  { if (status!=mCRL) 
    { ATerror("A restrict operator occurs in the scope of pCRL operators");
    }
    s1=determine_process_statusterm(ATAgetArgument(body,1),mCRL);
    return mCRL;
  }

  ATerror("Internal error: Process has unexpected format %t",body);
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


/************ ssc ****************************************************/

static ATermAppl storedata(specificationbasictype *spec)
{   
  storesig(spec);    
  storeact(spec->acts);
  storeprocs(spec->procs);
  return storeinit(spec->init);
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
    { sprintf(str->s,"%s%d",name,i); }
  /* check that name does not already exist, otherwise,
     add some suffix and check again */
  return gsString2ATermAppl(str->s);
}

/****************  substitute_data and substitute_datalist***********/

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

/* static int occursinvar(char *s, ATerm l)
{ char *string1=NULL;
  if (l==emv_term) return 0;
  if (ATmatch(l,"ins(<str>,<str>,<term>)",&string1,&dummystring,&l))
     { if (streq(s,string1)) return 1;
       return occursinvar(s,l); }
  ATerror("Expect a variablelist %t", l);
  return 0;
} */

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
  if (gsIsDelta(p))
   { return 0; }
  if (gsIsTau(p))
   { return 0; }
  ATerror("Internal: Unexpected process format in occursinCRLterm %t",p);
  return 0;
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
                 ATermList vars, 
                 ATermList pars,
                 ATermAppl s_term)
{ 
  if (vars==ATempty) 
   { 
#ifndef NDEBUG
     if (pars!=ATempty) 
     { ATerror("Non matching vars and pars list");
     }
#endif
     return s_term;
   }

  if (s_term==ATAgetFirst(vars))
   { return ATAgetFirst(pars); }
  return substitute_variable_rec(ATgetNext(vars),ATgetNext(pars),s_term);
}


static ATermList substitute_datalist_rec(
                 ATermList vars, 
                 ATermList pars, 
                 ATermList tl);

static ATermAppl substitute_data_rec(
                 ATermList vars, 
                 ATermList pars,
                 ATermAppl t)
{ 
  if (gsIsDataAppl(t))
  { 
    return gsMakeDataAppl(
               substitute_data_rec(vars,pars,ATAgetArgument(t,0)),
               substitute_data_rec(vars,pars,ATAgetArgument(t,1)));
  }

  if ( gsIsDataVarId(t))
  { return substitute_variable_rec(vars,pars,t);
  }

  assert(gsIsOpId(t));

  return t; 
}

static ATermList substitute_datalist_rec(
                 ATermList vars, 
                 ATermList pars,
                 ATermList tl) 
{ 
  if (tl==ATempty) return tl;

  return ATinsertA(
           substitute_datalist_rec(vars,pars,ATgetNext(tl)),
           substitute_data_rec(vars,pars,ATAgetFirst(tl)));
}



static ATermList substitute_datalist(
                 ATermList vars, 
                 ATermList pars,
                 ATermList tl) 
{ 
  if (vars==ATempty) return tl;
  return substitute_datalist_rec(vars,pars,tl);
}

static ATermAppl substitute_data(
                 ATermList vars, 
                 ATermList pars,
                 ATermAppl t)
{
  if (vars==ATempty) return t;
  return substitute_data_rec(vars,pars,t);
}

static ATermList substitute_multiaction(
                 ATermList vars,
                 ATermList pars,
                 ATermList multiAction)
{ 
  ATermAppl action=NULL;
  if (multiAction==ATempty)
  { return ATempty;
  }

  action=ATAgetFirst(multiAction);
  assert(gsIsAction(action));

  return ATinsertA(substitute_multiaction(vars,pars,ATgetNext(multiAction)),
                   gsMakeAction(ATAgetArgument(action,0),
                                substitute_datalist(
                                       vars,
                                       pars,
                                       ATLgetArgument(action,1))));
}

static ATermAppl substitute_time(
                 ATermList vars,
                 ATermList pars,
                 ATermAppl time)
{ if (linIsNoTime(time))
  { return time;
  }
  return substitute_data(vars,pars,time);
}

static ATermAppl substitute_pCRLproc(
                 ATermList vars, 
                 ATermList pars,
                 ATermAppl p)
{ 
  if (gsIsChoice(p))  /* "Choice(<term>,<term>)" */
  { return gsMakeChoice(
                substitute_pCRLproc(vars,pars,ATAgetArgument(p,0)),
                substitute_pCRLproc(vars,pars,ATAgetArgument(p,1))); 
  }
  if (gsIsSeq(p))
  { return gsMakeSeq(
                substitute_pCRLproc(vars,pars,ATAgetArgument(p,0)),
                substitute_pCRLproc(vars,pars,ATAgetArgument(p,1)));
  }
  if (gsIsCond(p))
  { return gsMakeCond(
                substitute_data(vars,pars,ATAgetArgument(p,0)),
                substitute_pCRLproc(vars,pars,ATAgetArgument(p,1)),
                substitute_pCRLproc(vars,pars,ATAgetArgument(p,2)));
  }

  if (gsIsSum(p))   
  { ATermList sumvars=ATLgetArgument(p,0);

    alphaconvert(&sumvars,&vars,&pars,vars,pars);
    return gsMakeSum(
               sumvars,
               substitute_pCRLproc(vars,pars,ATAgetArgument(p,1))); 
  }

  if (gsIsProcess(p))
  { return gsMakeProcess(ATAgetArgument(p,0),
                substitute_datalist(vars,pars,ATLgetArgument(p,1))); 
  }
  
  if (gsIsAction(p))
  { return gsMakeAction(ATAgetArgument(p,0),
                substitute_datalist(vars,pars,ATLgetArgument(p,1)));
  }

  if (gsIsDelta(p))
     { return p; }

  if (gsIsTau(p))
     { return p; }

  if (linIsMultiAction)
  { return linMakeMultiAction(
                substitute_multiaction(
                      vars,
                      pars,
                      ATLgetArgument(p,0)));

  }

  ATerror("Expect a pCRL process %t",p);
  return NULL;
}

/* ATerm substitute_parlist(char *arg, char *par,ATerm l)
{ char *s1=NULL, *s2=NULL;
  ATerm t3=NULL;
  if (ATmatch(l,"ins(<str>,<str>,<term>)",&s1,&s2,&l))
   { if (strequal(par,s1))
        t3=ATmake("ins(<str>,<str>,<term>)",arg,s2,l);
     else t3=ATmake("ins(<str>,<str>,<term>)",s1,s2,
          substitute_parlist(arg,par,l));
   }
  else if (l==emv_term) 
        t3=l;
  else ATerror("Expect variablelist %t",l);
  
  return t3;
} */

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
         ATerror("Expect variable list %t",parameters);

  parameters=parameters_that_occur_in_body(ATgetNext(parameters),body);
  if (occursinpCRLterm(variable,body,0))
       return ATinsertA(parameters,variable);
  return parameters; 
}        


static ATermAppl newprocess(ATermList parameters, ATermAppl body,
              processstatustype ps)
{ 
  ATermAppl p=NULL;
  parameters=parameters_that_occur_in_body(parameters, body);
  insertProcDeclaration(
             gsMakeProcVarId(fresh_name("P"),parameters),
             parameters,
             body,
             ps);
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
    body1=substitute_pCRLproc(renamevars,renameterms,body1);
    time=substitute_data(renamevars,renameterms,time);
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
    ATermAppl newproc=newprocess(freevars,body,pCRL);
    return gsMakeAtTime(
              gsMakeProcess(
                 newproc,
                 ATLgetArgument(newproc,1)),
              time);
  }

  if (gsIsProcess(body))
  { return gsMakeAtTime(body,time);
  }

  if (linIsMultiAction(body))
  { return gsMakeAtTime(body,time);
  }

  ATerror("Internal: Expect pCRL process in wraptime %t",body);
  return NULL;
}

static ATermList variablesToParameters (ATermList vars)
{ 

#ifdef NDEBUG
  return vars;
#else
  ATermAppl var=NULL;
  if (vars==ATempty)
     return ATempty;

  var=ATAgetFirst(vars);

  if (!gsIsDataVarId(var))
  { ATerror("DataVarId expected %t\n",var); }
  
  return ATinsertA(ATgetNext(vars),var);
#endif
}

typedef enum { alt, sum, /* cond,*/ seq, name, multiaction } state;

static ATermAppl getfreshvariable(char *s, ATermAppl sort)
{ ATermAppl variable=NULL;
  variable=gsMakeDataVarId(fresh_name(s),sort);
  insertvariable(variable);
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
  else ATerror("Unexpected sort encountered %t",sort);

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
      { return gsMakeChoice(
                  bodytovarheadGNF(ATAgetArgument(body,0),alt,freevars,first),
                  bodytovarheadGNF(ATAgetArgument(body,1),alt,freevars,first));
      }  
     body=bodytovarheadGNF(body,alt,freevars,first);
     newproc=newprocess(freevars,body,pCRL);
     return gsMakeProcess(newproc,ATLgetArgument(newproc,1));
   }

  if (gsIsSum(body)) 
  { ATermList sumvars=ATLgetArgument(body,0);
    ATermAppl body1=ATAgetArgument(body,1);
    if (sum>=s)
    { ATermList renamevars=ATempty;
      ATermList renameterms=ATempty;
      alphaconvert(&sumvars,&renamevars,&renameterms,freevars,ATempty);
      body1=substitute_pCRLproc(renamevars,renameterms,body1);
      body1=bodytovarheadGNF(body1,sum,ATconcat(sumvars,freevars),first);
      return gsMakeSum(sumvars,body1);
    }
    body=bodytovarheadGNF(body,alt,freevars,first);
    newproc=newprocess(freevars,body1,pCRL);
    return gsMakeProcess(newproc,ATLgetArgument(newproc,1));
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
    newproc=newprocess(freevars,body,pCRL);
    return gsMakeProcess(newproc,ATLgetArgument(newproc,1));
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
    newproc=newprocess(freevars,body1,pCRL);
    return gsMakeProcess(newproc,ATLgetArgument(newproc,1));
  }

  if (gsIsAction(body))
  { ATbool isnew=0;
    ATermAppl ma=linMakeMultiAction(ATinsertA(ATempty,body)); 
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
                                  GNF);
    }
    return gsMakeProcess(objectdata[n].targetsort,getarguments(ma)); 
  }  

  if (gsIsSync(body))
  { ATbool isnew=0;
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
                                  GNF);
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
    newproc=newprocess(freevars,body1,pCRL);
    return gsMakeProcess(newproc,ATLgetArgument(newproc,1));
  }
  
  if (gsIsProcess(body))
  { return body;
  }  
  
  if (gsIsTau(body))
  { if (v==first) 
       return linMakeMultiAction(ATempty); 
    if (tau_process==NULL)
    { tau_process=newprocess(ATempty,linMakeMultiAction(ATempty),pCRL);
    }
    return gsMakeProcess(tau_process,ATempty); 
  }
  
  if (gsIsDelta(body))
  { if (v==first) 
       return body; 
    if (delta_process==NULL)
       delta_process=newprocess(ATempty,body,pCRL);
    return gsMakeProcess(delta_process,ATempty); 
  }

  ATerror("Internal: Unexpected process format in bodytovarheadGNF %t",body);
  return NULL;
}


static void procstovarheadGNF(ATermList procs)
{ /* transform the processes in procs into newprocs */
  for( ; (procs!=ATempty) ; procs=ATgetNext(procs))
  { ATermAppl proc=ATAgetFirst(procs);
    ATfprintf(stderr,"PCRLIST %t\n",procs);
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
    alphaconvert(&sumvars,&vars,&terms,ATempty,ATinsertA(ATempty,body2));
    return gsMakeSum(
          sumvars,
          putbehind(substitute_pCRLproc(
                       vars,
                       terms,
                       ATAgetArgument(body1,1)),
                    body2));
  }
  
  if (gsIsAction(body1))
  { /* return gsMakeSeq(body1,body2); */
    ATerror("Internal: expect only multiactions, no single actions");
  }

  if (linIsMultiAction(body1))
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
    ATerror("Internal: expect only multiactions, not a tau");
  }

  ATerror("Internal: Unexpected process format in putbehind %t",body1);
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
    alphaconvert(&sumvars,&vars,&terms,ATempty,ATinsertA(ATempty,condition));
    return gsMakeSum(
             sumvars,
             distribute_condition(ATAgetArgument(body1,1),condition));
   }
  
  if (gsIsAction(body1))
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

  ATerror("Internal: Unexpected process format in distribute condition %t",body1);
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
      gsIsAction(body1)||
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

  ATerror("Internal: Unexpected process format in distribute_sum %t",body1);
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
  
  for(walker=seq_varnames; (walker!=NULL);
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
    { long n=objectIndex(first);
      if (objectdata[n].canterminate)
         return ATinsertA(extract_names(sequence),first);
      else return ATinsertA(ATempty,first); 
    } 
  }

  ATerror("Expect sequence of process names (1) %t",sequence);
  return NULL;
}

static ATermList parscollect(ATermAppl oldbody, ATermAppl *newbody)
{ /* we expect that oldbody is a sequence of process references */ 

  if (gsIsProcess(oldbody))
  { ATermAppl procId=ATAgetArgument(oldbody,0); 
    ATermList parameters=ATLgetArgument(procId,1);
    *newbody=gsMakeProcess(procId,parameters);
    return parameters;
  }     

  if (gsIsSeq(oldbody))   
    /*      ,"Seq(name(<str>,<int>,<term>),<term>)",
           &string1,&n,&args,&oldbody)) */
  { ATermAppl first=ATAgetArgument(oldbody,0);
    if (gsIsProcess(first))
    { ATermAppl procId=ATAgetArgument(oldbody,0);
      ATermList pars=parscollect(ATAgetArgument(oldbody,1),newbody);
      ATermList pars1=ATempty, pars2=ATempty;
     
      construct_renaming(pars,ATLgetArgument(procId,1),&pars1,&pars2);

      *newbody=gsMakeSeq(
                 gsMakeProcess(
                   gsMakeProcVarId(ATAgetArgument(procId,0),pars1),
                   pars1),
                 *newbody);
      return ATconcat(pars1,pars);
    }
  }

  ATerror("Expect a sequence of process names (2) %t",oldbody);
  return NULL;
}

static ATermList argscollect(ATermAppl t)
{ 
  if (gsIsProcess(t))
  return ATLgetArgument(t,1);

  if (gsIsSeq(t))    
  { ATermAppl firstproc=ATAgetArgument(t,0);
    assert(gsIsProcess(firstproc));
    return ATconcat(ATLgetArgument(firstproc,1),argscollect(t));
  }

  ATerror("Expect a sequence of process names (3) %t",t);
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
    ATerror("Expect a sequence of process names %t",sequence);
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
      new_process=newprocess(pars,newbody,pCRL);
      objectdata[objectIndex(new_process)].representedprocesses=
                   (ATerm)process_names;
    }
    else 
    { new_process=newprocess(freevars,sequence,pCRL);
      objectdata[objectIndex(new_process)].representedprocess=
                   (ATerm)sequence;
    }
    seq_varnames=ATinsertA(seq_varnames,new_process);
    *todo=ATinsertA(*todo,new_process);
  }
  /* now we must construct arguments */
  if (regular2)
     args=argscollect(sequence);
  else
     args=ATLgetArgument(new_process,1);
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
  { return gsMakeChoice( 
              to_regular_form(ATAgetArgument(t,0),todo,freevars),
              to_regular_form(ATAgetArgument(t,1),todo,freevars));
  } 
  
  if (gsIsSeq(t)) 
  { ATermAppl firstact=ATAgetArgument(t,0);
    assert(linIsMultiAction(firstact));
    /* the sequence of variables in 
               the second argument must be replaced */
    return gsMakeSeq(
              firstact,
              create_regular_invocation(ATAgetArgument(t,1),todo,freevars));
  } 
  
  if (gsIsCond(t))
  { assert(gsIsDelta(ATAgetArgument(t,2)));
    return gsMakeCond(
              t,
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
  
  if (gsIsAction(t)||gsIsDelta(t)||gsIsTau(t)) 
  { return t;    
  }
  
  else ATerror("To regular form expects GNF %t",t);
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
    body1=substitute_pCRLproc(renamevars,renameterms,body1);
    time=substitute_data(renamevars,renameterms,time);
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

  if (linIsMultiAction(body))
  { return gsMakeAtTime(body,time);
  }

  ATerror("Internal: Expect pCRL process in distributeTime %t",body);
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
  { ATerror("Internal: Sync operator cannot occur here.");
  }

  if (gsIsChoice(body)) 
  { return gsMakeChoice(
             procstorealGNFbody(ATAgetArgument(body,0),first,todo,
                     regular,mode,freevars),
             procstorealGNFbody(ATAgetArgument(body,1),first,todo,
                     regular,mode,freevars));
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
  { ATerror("Internal: Expect only multiactions at this point");
  }
 
  if (linIsMultiAction(body))
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

    n=objectIndex(body);
    if (objectdata[n].processstatus==mCRL)
    { *todo=ATinsertA(*todo,t);
      return NULL; 
    }
    /* The variable is a pCRL process and v==first, so,
       we must now substitute */
    procstorealGNFrec(ATAgetArgument(body,0),first,todo,regular);
    t3=substitute_pCRLproc(
         ATLgetArgument(body,1),
         ATLgetArgument(t,1),
         objectdata[objectIndex(t)].processbody);
    if (regular)
             t3=to_regular_form(t3,todo,freevars);
    return t3;
  }
  
  if (gsIsDelta(body)) 
  { return body;
  }
  
  if (gsIsTau(body))
  { /* return body; */
    ATerror("Only expect multiactions, no tau");
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

  ATerror("Internal: Unexpected process format in procstorealGNF %t",body);
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
            ATerror("Something wrong with recursion");
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
  { ATerror("Unguarded recursion in process %t",procIdDecl);
  }

  if ((objectdata[n].processstatus==GNFbusy)||
      (objectdata[n].processstatus==GNF)||
      (objectdata[n].processstatus==mCRLdone))
  { return;
  }

  if (objectdata[n].processstatus==mCRLbusy)
  { ATerror("Unguarded recursion without pCRL operators");
  }
  
  ATerror("Strange process type");  
}

static void procstorealGNF(ATermAppl procsIdDecl, int regular)
{ ATermList todo=ATempty;

  todo=ATinsertA(todo,procsIdDecl);
  for(; (todo!=ATempty) ; )
    { procsIdDecl=ATAgetFirst(todo);
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

static ATermList pCRLprocs=NULL;

static int alreadyinpCRLprocs(ATermAppl procIdDecl)
{ ATermList walker=pCRLprocs;
  for(walker=pCRLprocs ; walker!=ATempty ; walker=ATgetNext(walker))
  { if (ATAgetFirst(walker)==procIdDecl) return 1; 
  }
  return 0;
}

static void makepCRLprocs(ATermAppl t)
{ 
  if (gsIsChoice(t)||gsIsSeq(t))
  { makepCRLprocs(ATAgetArgument(t,0));
    makepCRLprocs(ATAgetArgument(t,1)); 
    return; 
  }

  if (gsIsCond(t)||gsIsSum(t))
  { makepCRLprocs(ATAgetArgument(t,1)); 
    return; 
  }

  if (gsIsProcess(t)) 
  { t=ATAgetArgument(t,0); /* get procId */
    if (alreadyinpCRLprocs(t)) return;
    pCRLprocs=ATinsertA(pCRLprocs,t);
    makepCRLprocs(objectdata[objectIndex(t)].processbody);
    return;
  }

  if (linIsMultiAction(t)||gsIsDelta(t))
  { return; 
  }

  ATerror("unexpected process format %t",t);
}

/**************** Collectparameterlist ******************************/

static int alreadypresent(ATermAppl *var,ATermList vl,int n)
{ /* Note: variables can be different, although they have the
     same string, due to different types. */

  ATermAppl var1=NULL;

  if (vl==ATempty) return 0;
  var1=ATAgetFirst(vl);

  if (!gsIsDataVarId(var1))
  { ATerror("Expect variablelist %t",vl);
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

static ATermList collectparameterlist(void)
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


/* static ATermAppl makenewobject(
                   char *s, 
                   ATermList argsorts, 
                   ATermAppl targetsort, 
                   objecttype o,
                   specificationbasictype *spec, 
                   int exactstring)
{ 
  if (!exactstring) 
     s=fresh_name(s);
  
  insertobject(s,args,n,o);
  if (o==func)
   { spec->funcs=ATmake(
            "ins(f(<str>,<term>,<str>),<term>)",s,args,target,spec->funcs);
   }
  if (o==map)
   { spec->maps=ATmake(
            "ins(f(<str>,<term>,<str>),<term>)",s,args,target,spec->maps);
   }
  return s;
} */

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
  { ATerror("Cannot declare variables as section is not yet closed %t",
                localequationvariables);}
  localequationvariables=t1;
}

static void end_equation_section(void)
{ if (localequationvariables==NULL)
     ATerror("Cannot open an non ended equation section",NULL);
  
  localequationvariables=NULL;
}

static void newequation(
                ATermAppl condition,
                ATermAppl t2, 
                ATermAppl t3, 
                specificationbasictype *spec)
{ 
  if (localequationvariables==NULL)
     ATerror("Variables must be declared first! %t",t2);
  
  spec->eqns=ATinsertA(spec->eqns,
                  gsMakeDataEqn(
                     localequationvariables,
                     condition,
                     t2,
                     t3));
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
  { if (ATAgetArgument(ATAgetFirst(p1),1)!=
            ATAgetArgument(ATAgetFirst(p2),1))
       return 0;
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
     ATerror("Cannot allocate memory for stack data");
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
      insertvariable(name);
      stack->booleanStateVariables=
           ATinsertA(stack->booleanStateVariables,name);
    }
  }
  stack->next=stacklist;
  
  if (regular)
  { stack->opns=NULL;
    stack->stackvar=gsMakeDataVarId(fresh_name(s3),
                                    gsMakeSortExprPos());
    insertvariable(stack->stackvar);
  }
  else  
  { stack->opns=find_suitable_stack_operations(parameterlist,stacklist);
    stacklist=stack;

    if (stack->opns!=NULL)
    { stack->stackvar=gsMakeDataVarId(fresh_name(s3),
                                      stack->opns->stacksort);
      insertvariable(stack->stackvar);
    }
    else 
    { /* stack->opns == NULL */
      stack->opns=malloc(sizeof(stackoperations));
      if (stack->opns==NULL)
          ATerror("Cannot allocate memory for stack operations");

      stack->opns->sorts=ATempty;
      ATprotect((ATerm *)&(stack->opns->sorts));
      stack->opns->get=NULL;
      ATprotect((ATerm *)&(stack->opns->get));
      stack->opns->stacksort=makenewsort(fresh_name("Stack"),spec);
      stack->stackvar=gsMakeDataVarId(fresh_name(s3),
                                      stack->opns->stacksort);
      insertvariable(stack->stackvar);
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
        sprintf(scratch1,"get%s",ATSgetArgument(par,0));

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

static ATermAppl getvar(ATermAppl var,stacklisttype *stack)
{ ATermList walker=NULL; 
  ATermList getmappings=stack->opns->get;

  for(walker=stack->parameterlist ;
        walker!=ATempty ; walker=ATgetNext(walker))
  { if (ATAgetFirst(walker)==var)
    { return gsMakeDataAppl(ATAgetFirst(getmappings),stack->stackvar); 
    }
  }
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
  { sprintf(scratch1,"%d",i);
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
                 ATermList vars)
{ 
  if (gsIsOpId(t)) return t;

  if (gsIsDataVarId(t))
  { if (ATindexOf(vars,(ATerm)t,0)>=0)
    { /* t occurs in vars, so, t does not have to be reconstructed
         from the stack */
      return t;
    }
    else return getvar(t,stack); }

  if (gsIsDataAppl(t))
  { return gsMakeDataAppl(
            adapt_term_to_stack(ATAgetArgument(t,0),stack,vars),
            adapt_term_to_stack(ATAgetArgument(t,1),stack,vars));
  }
              
  ATerror("Expect a term");
  return NULL;
}

static ATermList adapt_termlist_to_stack(
                  ATermList tl, 
                  stacklisttype *stack, 
                  ATermList vars)
{ 
  if (tl==ATempty)
  { return ATempty;
  }

  return ATinsertA(
          adapt_termlist_to_stack(ATgetNext(tl),stack,vars),
          adapt_term_to_stack(ATAgetFirst(tl),stack,vars));
}

static ATermList adapt_multiaction_to_stack(
                   ATermList multiAction,
                   stacklisttype *stack,
                   ATermList vars)
{ ATermAppl action=NULL;
  if (multiAction==ATempty)
  { return ATempty;
  }

  action=ATAgetFirst(multiAction);
  assert(gsIsAction(action));

  return ATinsertA(
            adapt_multiaction_to_stack(ATgetNext(multiAction),stack,vars),
            gsMakeAction(
                  ATAgetArgument(action,0),
                  adapt_termlist_to_stack(
                           ATLgetArgument(action,1),
                           stack,
                           vars)));
}

static ATermAppl find(
               ATermAppl s, 
               ATermList pars, 
               ATermList args, 
               stacklisttype *stack,
               ATermList vars, 
               int regular)
{ 
  if (pars==ATempty) 
  { if (ATindexOf(stack->parameterlist,(ATerm)s,0)>=0);
    { return s;
    }
    return uniqueterm(ATAgetArgument(s,1));
  }

  if (s==ATAgetFirst(pars))
  { if (regular)
    { return ATAgetFirst(args);
    } 
    return adapt_term_to_stack(ATAgetFirst(args),stack,vars);
  }
  return find(s,ATgetNext(pars),ATgetNext(args),stack,vars,regular);
}


static ATermList findarguments(
                   ATermList pars,
                   ATermList parlist, 
                   ATermList args,
                   ATermList t2,
                   stacklisttype *stack, 
                   ATermList vars, 
                   int regular)
{ ATermAppl string1term=NULL;
  
  if (parlist==ATempty)
  { return t2; 
  }

  /* if (ATmatch(parlist,
        "ins(<term>,<term>,<term>)",
                 &string1term,&string2term,&parlist)) */
  string1term=ATAgetFirst(parlist);
  parlist=ATgetNext(parlist);

  return ATinsertA(
            findarguments(pars,parlist,args,t2,stack,vars,regular),
            find(string1term,pars,args,stack,vars,regular));
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
            stack->parameterlist,args,t2,stack,vars,regular);
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
           /*  ,"Seq(name(<str>,<int>,<term>),<term>)",&string1,&n,&t1,&t2)) */
  { if (regular)
         ATerror("Process is not regular, as it has stacking vars %t",t);
    process=ATAgetArgument(t,0);
    t2=ATAgetArgument(t,1);
    assert(gsIsProcess(procId)); 
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
              /* ,"name(<str>,<int>,<term>)",&string1,&n,&t1)) */
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
  
  ATerror("Expect seq or name %t",t);
  return NULL;
}

static int occursin(ATermAppl name,ATermList pars)
{ return (ATindexOf(pars,(ATerm)name,0)>=0);

  /* ATerm string1term=NULL, string2term=NULL; */

  /* for ( ; ATmatch(pars,"ins(<term>,<term>,<term>)",
                   &string1term,&string2term,&pars) ; ) * /
  for ( ; (ATgetAFun(pars)==ins3_symbol) ; )
    { 
      if (ATgetArgument(pars,0)==name) 
                return ATgetArgument(pars,1); 
      pars=ATgetArgument(pars,2);
    }
  return NULL; */
}

/* static ATermAppl uniquetermrec(ATermAppl, int l);

static ATermAppl  uniquetermlistrec(ATermList arg, int l)
{ ATerm t1=NULL, t2=NULL, t3=NULL;
  if (arg==NULL) 
     t3=emt_term;
  else
   { if (l>=3)
        t3=NULL;
        ATerror("Cannot find a limited size ATerm  of sort %t",

         ATmake("<str>",sortdata[arg->n].sortname->s)); 
     else 
      { t1=uniquetermrec(arg->n,l);
        if (t1==NULL) 
           t3=NULL;
        else
         { t2=uniquetermlistrec(arg->next,l);
           if (t2==NULL) 
              t3=NULL;
           else t3=ATmake("ins(<term>,<term>)",t1,t2);
   }  }  }
   
  return t3;
} */

static ATermAppl uniquetermrec(ATermAppl targetsort, int l)
{ int i;
  
  /* First search for a constant constructor */

  for (i=0 ; (i<maxobject) ; i++ )
  { if ((objectdata[i].object==func)&&
            (objectdata[i].targetsort==targetsort)&&
            (!gsIsSortArrow(ATAgetArgument(objectdata[i].objectname,1))))
    { return objectdata[i].objectname;
    }
  }

  /* Second search for a constant mapping */

  for (i=0 ; (i<maxobject) ; i++ )
  { if ((objectdata[i].object==map)&&
            (objectdata[i].targetsort==targetsort)&&
            (!gsIsSortArrow(ATAgetArgument(objectdata[i].objectname,1))))
    { return objectdata[i].objectname;
    }
  }

  /* Third construct a constant term */

  /* for (i=0 ; (i<maxobject) ; i++ )
  { if (((objectdata[i].object==map)||
                  (objectdata[i].object==func))&&
               (objectdata[i].targetsort==targetsort))
    { t=uniquetermlistrec(objectdata[i].args,l+1);
      if (t!=NULL) 
      { return ATmake("t(<str>,<term>)",objectdata[i].objectname,t);
      }
    }
  } */
  
  ATerror("Fail to generate unique term of sort %t\nGeneration of constant terms has only partly been implemented",targetsort);
  return NULL;
}

static ATermAppl uniqueterm(ATermAppl sort)
{ /* first try to locate a constant of required sort */
  int n;
  n=existsort(sort);
  if (n<=0) 
   { ATerror("Sort `%t' does not exist",sort);
   }
  return uniquetermrec(sort,0);
} 

static ATermList pushdummyrec(
                     ATermList totalpars, 
                     ATermList pars, 
                     stacklisttype *stack, 
                     int regular)
{ 
  ATermAppl par=NULL;
  
  if (totalpars==ATempty)
  { if (regular)
    { return ATempty;
    }
    return ATinsertA(ATempty,stack->opns->emptystack);
  }
  
  par=ATAgetFirst(totalpars);
  totalpars=ATgetNext(totalpars);

  if (!occursin(par,pars))
  { return ATinsertA(
               pushdummyrec(totalpars,pars,stack,regular),
               par);
  }
  return ATinsertA(
             pushdummyrec(totalpars,pars,stack,regular),
             uniqueterm(ATAgetArgument(par,1)));
}

static ATermList pushdummy(
                     ATermList parameters,
                     stacklisttype *stack, 
                     int regular)
{ 
  return pushdummyrec(stack->parameterlist,
              parameters,stack,regular);
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

  t=pushdummy(objectdata[objectIndex(initialProcId)].parameters,stack,regular);

  if (regular)
  { if (singlecontrolstate)
    { return t;
    }
    return processencoding(i,t,spec,stack); 
  }
  return ATinsertA(ATempty,
                   gsMakeDataAppl(
                        stack->opns->push,
                        ATAgetFirst(processencoding(i,t,spec,stack))));
}

/*************************  Routines for summands  **************************/



static int identicalActionIds(ATermList ma1, ATermList ma2)
{ 
  if (linIsDelta(ma1))
  { return linIsDelta(ma2);
  }

  if (ma1==ATempty)
  { return ma2==ATempty;
  }
  
  if (ma2==ATempty)
  return 0;

  if (ATAgetArgument(ATAgetFirst(ma1),0)==
        ATAgetArgument(ATAgetFirst(ma1),0))
  { return identicalActionIds(ATgetNext(ma1),
                              ATgetNext(ma2));
  }

  return 0;
}


static ATermList insert_summand(
                    ATermList sumlist, 
                    ATermList sumvars, 
                    ATermList multiAction,
                    ATermAppl actTime,
                    ATermAppl condition,
                    ATermList procargs) 
{ /* insert a new summand in sumlist; first try whether there is already
     a similar summand, such that this summand can be added with minimal
     increase of size. Otherwise add a fully new summand. Note that by
     a more careful matching, the number of summands can be reduced.

     Note also that a terminated term is indicated by taking procargs
     equal to NULL. */

  ATermList newsumlist=ATempty; 

  for(newsumlist=ATempty ;
        sumlist!=ATempty ; sumlist=ATgetNext(sumlist) )
  {
    /* (ATmatch(sumlist,"ins(smd(<term>,<str>,<term>,<term>,<term>),<term>)",
         &sumvars1,&actionId1,&actargs1,
                    &procargs1,&condition1,&sumlist));  */
    ATermAppl summand=ATAgetFirst(sumlist);
    ATermList multiAction1=linGetMultiAction(summand);

    if (identicalActionIds(multiAction,multiAction1))

    { ATermList renamingvariablelist=ATempty;
      ATermList renamingtermlist=ATempty;
      ATermList sumvars1=linGetSumVars(summand);

      if (variablesequal(sumvars,sumvars1, 
                    &renamingvariablelist,&renamingtermlist))
      { ATermList procargs1=linGetNextState(summand);
        ATermList procargs2=NULL;
        if (linIsTerminated(procargs))
        { procargs2=procargs;
        }
        else
        { procargs2=substitute_datalist(renamingvariablelist,
                                        renamingtermlist,
                                        procargs);
        }
        if (procargs1==procargs2)
        { ATermAppl actTime1=linGetActionTime(summand);
          ATermAppl actTime2=NULL;
          if (linIsNoTime(actTime))
          { actTime2=actTime;
          }
          else 
          { actTime2=substitute_data(renamingvariablelist,
                                     renamingtermlist,
                                     actTime);
          }
          if (actTime1==actTime2)
          { ATermList multiAction2=substitute_multiaction(
                                   renamingvariablelist,
                                   renamingtermlist,
                                   multiAction); 
            if (multiAction1==multiAction2)
            { ATermAppl condition1=linGetCondition(summand);
              ATermAppl condition2=gsMakeDataExprOr(
                                        condition1,
                                        substitute_data(
                                              renamingvariablelist,
                                              renamingtermlist,
                                              condition));
              return ATinsertA(ATconcat(newsumlist,ATgetNext(sumlist)),
                               linMakeSummand(
                                         sumvars1,
                                         multiAction1,
                                         actTime1,
                                         condition2,
                                         procargs1));
    }  } } } }
    newsumlist=ATinsertA(newsumlist,summand);
  } 

  /* There is no matching summand in sumlist. So, we add the summand
     at the beginning */
  
  return ATinsertA(newsumlist,
                   linMakeSummand(
                         sumvars,
                         multiAction,
                         actTime,
                         condition,
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
  ATermList multiAction=NULL;
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
  
  if (gsIsCond(summandterm))
    /* ,"Cond(<term>,<term>,Delta)",&condition1,&summandterm)) */
  { condition1=ATAgetArgument(summandterm,0);
    assert(gsIsDelta(ATAgetArgument(summandterm,2)));
    summandterm=ATAgetArgument(summandterm,1);
    if (!((regular)&&(singlestate)))
    { condition1=gsMakeDataExprAnd(
                     correctstatecond(procId,pCRLprocs,stack,regular,spec),
                     ((regular)?condition1:      
                                adapt_term_to_stack(condition1,
                                                    stack,
                                                    sumvars)));
    }
  }
  else 
  { if ((regular)&&(singlestate))
    { condition1=gsMakeDataExprTrue();
    }
    else 
    { condition1=correctstatecond(procId,pCRLprocs,stack,regular,spec);
    }
  }

  if (gsIsSeq(summandterm)) 
               /*  ,"Seq(<term>,<term>)",&t1,&t2)) */
  { /* only one summand is needed */
    ATermAppl t1=ATAgetArgument(summandterm,0);
    ATermAppl t2=ATAgetArgument(summandterm,1);
    if (gsIsAtTime(t1))
    { atTime=ATAgetArgument(t1,1);
      t1=ATAgetArgument(t1,0);
    }
    else
    { atTime=noTime;
    }

    if (gsIsDelta(t1))
    { multiAction=delta;
    }
    else if (gsIsTau(t1))
    { multiAction=ATempty;
    }
    else if (gsIsAction(t1))
    { multiAction=ATinsertA(ATempty,t1);
    }
    else 
    { multiAction=ATLgetArgument(t1,0);
    }

    procargs=make_procargs(t2,stack,
                pCRLprocs,sumvars,regular,singlestate,spec);
    if (!regular)
    { if (multiAction!=delta)
      { multiAction=adapt_multiaction_to_stack(
                      multiAction,stack,sumvars);          
      }
      if (atTime!=noTime)
      { atTime=adapt_term_to_stack(
                      atTime,stack,sumvars);
      }
    }            
    sumlist=insert_summand(sumlist,
                   sumvars,multiAction,atTime,condition1,procargs);
    return;
  }

  /* There is a single initial multiaction or deadlock, possibly timed*/

  if (gsIsAtTime(summandterm))
  { atTime=ATAgetArgument(summandterm,1);
    summandterm=ATAgetArgument(summandterm,0);
  } 
  else
  { atTime=noTime;
  } 
    
  if (gsIsDelta(summandterm))
  { multiAction=delta;
  } 
  else if (gsIsTau(summandterm))
  { multiAction=ATempty;
  } 
  else if (gsIsAction(summandterm))
  { multiAction=ATinsertA(ATempty,summandterm);
  } 
  else 
  { multiAction=ATLgetArgument(summandterm,0);
  } 
                 
  /* procargs=make_procargs(t2,stack,
                pCRLprocs,sumvars,regular,singlestate,spec);
    if (!regular)
    { if (multiAction!=delta)
      { multiAction=adapt_multiaction_to_stack(
                      multiAction,stack,sumvars);
      }
      if (atTime!=noTime)
      { atTime=adapt_term_to_stack(
                      atTime,stack,sumvars);
      }
    }
    sumlist=insert_summand(sumlist,
                   sumvars,multiAction,atTime,condition1,procargs);
    return; */
  if (regular)
  { sumlist=insert_summand(sumlist,
                   sumvars,multiAction,atTime,condition1,terminated);
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

  multiAction=adapt_multiaction_to_stack(multiAction,stack,sumvars);
  procargs=ATinsertA(ATempty,gsMakeDataAppl(stack->opns->pop,stack->stackvar));

  sumlist=insert_summand(sumlist,
                    sumvars,multiAction,atTime,condition2,procargs);
       
  if (canterminate==1)
  { condition2=gsMakeDataExprAnd(emptypops,condition1); 
    sumlist=insert_summand(sumlist,sumvars,
                  multiAction,atTime,condition2,terminated);
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
                 specificationbasictype *spec)
{ 
  if (gsIsChoice(body))
  { ATermAppl t1=ATAgetArgument(body,0);
    ATermAppl t2=ATAgetArgument(body,1);

    collectsumlistterm(procId,t1,pars,stack,
                 canterminate,regular,singlestate,spec);
    collectsumlistterm(procId,t2,pars,stack,
                 canterminate,regular,singlestate,spec);
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
              spec);
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
    w->size=n;
    if (n==2)
    { w->sortId=gsMakeSortExprBool(); 
      w->elementnames=ATinsertA(ATinsertA(ATempty,
                           gsMakeDataExprFalse()),gsMakeDataExprTrue());
    }
    else 
    { sprintf(scratch1,"Enum%d",n);
      w->sortId=makenewsort(fresh_name(scratch1),spec); 
      w->elementnames=ATempty;
      /* enumeratedtypes[i].elements=ATmake("ems"); */
      for(j=0 ; (j<n) ; j++)
      { /* Maak hier een naamlijst van sort elementen. */
        ATermAppl constructor=NULL;
        sprintf(scratch1,"e%d-%d",j,n);
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
        newequation(gsMakeDataExprTrue(),
                    gsMakeDataExprEq(v,v),
                    gsMakeDataExprTrue(),spec);
        for(l1=w->elementnames ; l1!=ATempty ; l1=ATgetNext(l1))
        { for(l2=w->elementnames ; l2!=ATempty ; l2=ATgetNext(l2))
          { ATermAppl el1=ATAgetFirst(l1);
            ATermAppl el2=ATAgetFirst(l2);
            if (el1!=el2)
            { newequation(gsMakeDataExprTrue(),
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
  ATermList w=NULL;
  for(w=e->functions; w!=ATempty; w=ATgetNext(w))
  { if (objectdata[objectIndex(ATAgetFirst(w))].targetsort==sort)
    return ATAgetFirst(w);
  };
  ATerror("Searching for nonexisting case function");
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
  newequation(gsMakeDataExprTrue(),
              makeApplTerm(functionname,ATinsertA(xxxterm,v)),
              v1,
              spec);
  end_equation_section();

  declare_equation_variables(vars);
  auxvars=vars;

  for(w=e->elementnames; w!=ATempty ; w=ATgetNext(w))
  { 
    newequation(
           gsMakeDataExprTrue(),
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

    sprintf(scratch1,"C%d-%s",e->size,ATSgetArgument(sort,0));
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
  et->next=enumeratedtypes;
  enumeratedtypes=et;
  et->etype=create_enumeratedtype(n,spec);
  
  et->var=gsMakeDataVarId(fresh_name("e"),et->etype->sortId);
  insertvariable(et->var);
  
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
         insertvariable(*var);
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
                     ATermList conditionlist)
{ 
  ATermAppl unique=uniqueterm(ATAgetArgument(var,1));
  ATermAppl newcondition=gsMakeDataExprEq(var,unique);
  return extend(newcondition,conditionlist);        
}   


static ATermAppl transform_matching_list(ATermList matchinglist)
{ ATermAppl var=NULL; 
  ATermAppl unique=NULL;
  if (matchinglist==ATempty)
     return gsMakeDataExprTrue();

  var=ATAgetFirst(matchinglist);
  unique=uniqueterm(ATAgetArgument(var,1));
  return gsMakeDataExprAnd( 
               transform_matching_list(ATgetNext(matchinglist)),
               gsMakeDataExprEq(var,unique));
}


static ATermList addcondition(
                     ATermList matchinglist, 
                     ATermList conditionlist)
{ 
  return ATinsertA(conditionlist,
                   transform_matching_list(matchinglist));
}

static ATermList merge_var(
                    ATermList v1, 
                    ATermList v2, 
                    ATermList *renamings, 
                    ATermList *conditionlist)
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
         /*  (ATmatch(v1,"ins(<str>,<str>,<term>)",&var,&sort,&v1)) ; ) */
  { ATermAppl var=ATAgetFirst(v1);
    if (!mergeoccursin(&var,v2,
            &matchinglist,&renamingpars,&renamingargs))
    { 
      result=ATinsertA(result,var);
      *conditionlist=extend_conditions(var,*conditionlist);
    }
  }
  /* ATfprintf(stderr,"Matchinglist: %t\nConditionlist %t\n",
                   matchinglist,*conditionlist); */
  *conditionlist=addcondition(matchinglist,*conditionlist);
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

static ATermAppl isSort(ATermAppl t)
{ ATerror("IsSort must still be implemented");
  return NULL;
}

static int summandsCanBeClustered(
              ATermAppl summand1,
              ATermAppl summand2)
{
  ATermList multiaction1=linGetMultiAction(summand1);
  ATermAppl actiontime1=linGetActionTime(summand1);
  ATermList nextstate1=linGetNextState(summand1);
  ATermList multiaction2=linGetMultiAction(summand2);
  ATermAppl actiontime2=linGetActionTime(summand2);
  ATermList nextstate2=linGetNextState(summand2);
 
  if (linIsNoTime(actiontime1)!=
      linIsNoTime(actiontime2))
  return 0;

  if (linIsDelta(multiaction1))
  { if (linIsDelta(multiaction2))
    { return 1;
    }
    else return 0;
  }

  if (linIsDelta(multiaction2))
  { /* multiaction1!=delta */
    return 0;
  }

  /* Here the multiactions are proper multi actions,
     both with or without a time indication */

  if (linIsTerminated(nextstate1)!=
      linIsTerminated(nextstate2))
  { return 0;
  }

  /* The actions can be clustered if they contain
     the same actions, with the same sorts for the 
     actions. We assume that the multiactions are
     ordered.
  */

  for( ; multiaction1!=ATempty ; multiaction1=ATgetNext(multiaction1) )
  { if (ATAgetArgument(ATAgetFirst(multiaction1),0)!=
           ATAgetArgument(ATAgetFirst(multiaction1),0))
    { return 0;
    }
    multiaction2=ATgetNext(multiaction2);
  }
  
  if (multiaction2!=ATempty)
  { return 0; 
  }

  return 0;
}

static ATermAppl collect_sum_arg_arg_cond(
                   enumtype *e,
                   int n,
                   ATermList sumlist,
                   ATermList gsorts)
{ /* This function gets a list of summands, with
     the same multiaction, time and termination 
     status. It yields a single clustered summand
     by introducing an auxiliary sum operator, with
     a variable of enumtype. In case binary is used,
     will use a number of summands of sort Bool */

  ATermList resultsum=ATempty;
  ATermAppl resultcondition=NULL;
  ATermList resultmultiaction=ATempty;
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

    resultsum=merge_var(sumvars,resultsum,&rename_list,&conditionlist); 
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
    ATermList auxargs=(ATermList)ATgetNext(auxrename_list);
    auxrename_list=(ATermList)ATgetNext(auxrename_list);

    auxresult1=substitute_data(auxpars,auxargs,condition); 
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
    ATermList multiaction=linGetMultiAction(summand);
    if (linIsDelta(multiaction))
    { multiActionIsDelta=1;
      break;
    }
    multiActionList=ATinsert(multiActionList,
                            (ATerm)linGetMultiAction(summand));
  }

  if (multiActionIsDelta)
  { resultmultiaction=delta;
  }
  else
  { 
    ATermList nextmultiActions=ATempty;
    long fcnt=0;
    ATermAppl f=NULL;
    ATermList resultf=ATempty;
    resultmultiaction=ATempty;
    multiActionList=ATreverse(multiActionList);

    for( ; (ATermList)ATgetFirst(multiActionList)!=ATempty ; 
                           multiActionList=ATreverse(nextmultiActions) )
    { auxresult=ATempty;
      auxrename_list=rename_list;
      equalterm=NULL;
      equaluptillnow=1;
      for(ATermList walker=sumlist; walker!=ATempty ; 
                                         walker=ATgetNext(walker))
      { 
        assert(auxrename_list!=ATempty);
        ATermList auxpars=(ATermList)ATgetFirst(auxrename_list);
        auxrename_list=(ATermList)ATgetNext(auxrename_list);
        ATermList auxargs=(ATermList)ATgetNext(auxrename_list);
        auxrename_list=(ATermList)ATgetNext(auxrename_list);

        nextmultiActions=
              ATinsert(nextmultiActions,
                       (ATerm)ATgetNext(
                            (ATermList)ATgetFirst(multiActionList)));

        f=(ATermAppl)ATelementAt((ATermList)ATgetFirst(multiActionList),fcnt);
        auxresult1=substitute_data(auxpars,auxargs,f); 

        if (equalterm==NULL)
        { equalterm=auxresult1;
        }
        else 
        { if (equaluptillnow==1)
          { equaluptillnow=(equalterm==auxresult1);
        } }
        auxresult=ATinsertA(auxresult,auxresult1);
      }
      if (equaluptillnow)
      { resultf=ATinsertA(resultf,equalterm);
      }
      else
      { if (binary==0)
        { resultf=ATinsertA(resultf,
                    gsMakeDataApplList(
                         find_case_function(e->etype,isSort(f)),
                         ATinsertA(auxresult,e->var)));
        }
        else
        { ATermAppl temp=construct_binary_case_tree(
                            n,
                            resultsum,
                            auxresult,
                            isSort(f),
                            e);
          resultf=ATinsertA(resultf,temp);
        }
      }
      fcnt++;
    }
    /* Now turn *resultf around */
    resultmultiaction=ATreverse(resultf);
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
    ATermList auxargs=(ATermList)ATgetNext(auxrename_list);
    auxrename_list=(ATermList)ATgetNext(auxrename_list);
    
    auxresult1=substitute_data(auxpars,auxargs,actiontime);
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
  if (linIsTerminated(linGetNextState(ATAgetFirst(sumlist))))
  { resultnextstate=terminated;
  }
  else
  { 
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
        ATermList auxargs=(ATermList)ATgetNext(auxrename_list);
        auxrename_list=(ATermList)ATgetNext(auxrename_list);

        ATermAppl nextstateparameter=(ATermAppl)ATelementAt(nextstate,fcnt);
        
        auxresult1=substitute_data(auxpars,auxargs,nextstateparameter); 
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
                         find_case_function(e->etype,ATAgetFirst(ffunct)),
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
  }
  return linMakeSummand(resultsum,
                        resultmultiaction,
                        resulttime,
                        resultcondition,
                        resultnextstate);
}

static ATermList getActionSorts(ATermList multiaction)
{ ATermList resultsorts;

  if (linIsDelta(multiaction))
  { return ATempty;
  }
  
  for(resultsorts=ATempty ; multiaction!=ATempty ; 
                  multiaction=ATgetNext(multiaction))
  { resultsorts=ATconcat(
                   ATLgetArgument(
                         ATAgetArgument(ATAgetFirst(multiaction),0),
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
    { ATermList actionsorts=getActionSorts(linGetMultiAction(ATAgetFirst(w1)));
      if (binary==0)
      { enumeratedtype=generate_enumerateddatatype(n,actionsorts,pars,spec); 
      }
      else 
      { enumeratedtype=generate_enumerateddatatype(2,actionsorts,pars,spec); 
      }

      result=ATinsertA(result,
             collect_sum_arg_arg_cond(enumeratedtype,n,w1,pars));

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
  stacklisttype *stack=NULL;
  int singlecontrolstate=0;
  int n=objectIndex(procId);

  pCRLprocs=ATinsertA(ATempty,procId);

  makepCRLprocs(objectdata[n].processbody);
  /* now pCRLprocs contains a list of all process id's in this
     pCRL process */
 
  /* collect the parameters, but assume that variables
     have a unique sort */
  if (ATgetLength(pCRLprocs)==1)
  { singlecontrolstate=1;
  }
  parameters=collectparameterlist();
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
{ return ATindexOf(set,(ATerm)actionName,0)>=0;
}

static ATermList hide(ATermList hidelist, ATermList multiaction)
{ ATermList resultactionlist=ATempty;

  if (linIsDelta(multiaction))
  { return delta; }

  for (ATermList walker=multiaction ;
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
  return ATreverse(resultactionlist);
}

static ATermAppl hidecomposition(ATermList hidelist, ATermAppl ips)
{  
  ATermList resultsumlist=ATempty;
  ATermList sourcesumlist=linGetSums(ips);
  
  for( ; sourcesumlist!=ATempty ; sourcesumlist=ATgetNext(sourcesumlist))
  { ATermAppl summand=ATAgetFirst(sourcesumlist);
    ATermList sumvars=linGetSumVars(summand);
    ATermList multiaction=linGetMultiAction(summand);
    ATermAppl actiontime=linGetActionTime(summand); 
    ATermAppl condition=linGetCondition(summand);
    ATermList nextstate=linGetNextState(summand);

    resultsumlist=ATinsertA(
                    resultsumlist,
                    linMakeSummand(
                           sumvars,
                           hide(hidelist,multiaction),
                           actiontime,
                           condition,
                           nextstate));
  }
  return linMakeInitProcSpec( 
             linGetInit(ips),linGetParameters(ips),resultsumlist);
}

/**************** allow  *************************************/

static ATermAppl allowcomposition(ATermList allowlist, ATermAppl ips)
{
  ATerror("The allow composition function still needs to be implemented");
  return NULL;
}


/**************** encapsulation *************************************/

static ATermList encap(ATermList encaplist, ATermList multiaction)
{ int actioninset=0;
  if (linIsDelta(multiaction))
  { return delta; }
  
  for (ATermList walker=multiaction ;
            walker!=ATempty ; walker=ATgetNext(walker) )
  { ATermAppl action=ATAgetFirst(walker);
    if (isinset(ATAgetArgument(ATAgetArgument(action,0),0),encaplist))
    { actioninset=1;
      break;
    }
  }

  /* reverse the actionlist to maintain the ordering */
  if (actioninset)
  { return delta; 
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
    ATermList multiaction=linGetMultiAction(summand);
    ATermAppl actiontime=linGetActionTime(summand);
    ATermAppl condition=linGetCondition(summand);
    ATermList nextstate=linGetNextState(summand);
 
    ATermList resultmultiaction=encap(encaplist,multiaction);

    if ((resultmultiaction!=delta) ||
        (actiontime!=noTime))
    { resultsumlist=ATinsertA(
                    resultsumlist,
                    linMakeSummand(
                           sumvars,
                           resultmultiaction,
                           actiontime,
                           condition,
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

static ATermList rename_actions(ATermList renamings, ATermList multiaction)
{ 
  ATermList resultactionlist=ATempty;
  if (linIsDelta(multiaction))
  { return delta; }
  
  for (ATermList walker=ATreverse(multiaction) ;
            walker!=ATempty ; walker=ATgetNext(walker) )
  { ATermAppl action=ATAgetFirst(walker);
    resultactionlist=linInsertActionInMultiActionList(
                          rename_action(renamings,action),
                          resultactionlist);
  } 

  return resultactionlist;
}

static ATermAppl renamecomposition(ATermList renamings, ATermAppl ips)
{
  ATermList resultsumlist=ATempty;
  ATermList sourcesumlist=linGetSums(ips);

  for( ; sourcesumlist!=ATempty ; sourcesumlist=ATgetNext(sourcesumlist))
  { ATermAppl summand=ATAgetFirst(sourcesumlist);
    ATermList sumvars=linGetSumVars(summand);
    ATermList multiaction=linGetMultiAction(summand);
    ATermAppl actiontime=linGetActionTime(summand);
    ATermAppl condition=linGetCondition(summand);
    ATermList nextstate=linGetNextState(summand);

    resultsumlist=ATinsertA(
                    resultsumlist,
                    linMakeSummand(
                           sumvars,
                           rename_actions(renamings,multiaction),
                           actiontime,
                           condition,
                           nextstate));
  }
  return linMakeInitProcSpec(
             linGetInit(ips),linGetParameters(ips),resultsumlist);
}

/**************** equalargs ****************************************/

static int autoapplysubstitution(ATermList *subargs, ATermList *subpars)
{ ATermList oldargs=ATempty, newargs=*subargs;
  
  do
  { 
    oldargs=newargs;
    newargs=substitute_datalist(*subpars,oldargs,oldargs);
  }
  while (oldargs!=newargs);
  *subargs=newargs;

  return 1;
}


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

/* static int equalargs(
               ATermList t1, 
               ATermList t2, 
               ATermList *sumlist, 
               ATerm *condition, 
               ATermList *subargs, 
               ATermList *subpars)
{  / * 
      returns 0 if types of terms or length do not match
      returns 1 if result is a valid ATerm  in t3, and condition defined.
      returns 2 if condition is not defined, but result is valid * /

  int n1=0,n2=0,n3=0;
  
  if ((t1==ATempty) && (t2==ATempty))
  { return 2;
  }

  ATermAppl u1=ATAgetFirst(t1);
  ATermAppl u2=ATAgetFirst(t2);
      / * if ((ATmatch (t1,"ins(<term>,<term>)",&u1,&t1))&&
             (ATmatch (t2,"ins(<term>,<term>)",&u2,&t2))) * /
  if ((welltyped1(u1,&n1)) && (welltyped1(u2,&n2)))
  { if (n1==n2)
    { n1=equalargs (t1,t2,sumlist,condition,subargs,subpars);
      if (n1==0) 
      { return 0; 
      }

      / * check whether u1 or u2 is a variable, for which
         the sum elimination theorem can be applied * /
      if (sumelimination(u1,u2,sumlist,subargs,subpars)>0)
             n3=n1;
           else 
           { t2=gsMakeDataExprEq(u1,u2);
             if (n1==2)
             { *condition=t2;
               n3=1; 
             }
             else
             { *condition=gsMakeDataExprAnd(t2,*condition);
               n3=1; 
             }
          } 
   }
    else 
    { return 0; / * different types * / 
    }
  }
  ATerror("Typing wrong");
  return NULL;
} */

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

      insertvariable(var3);
      t1=ATinsertA(construct_renaming(pars1,pars2,&t,&t2),var2);
          
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
  if (linIsDelta(multiaction))
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

static ATermList insertConditionInTriples(
                    ATermAppl condition,
                    ATermList triples,
                    ATermList result)
{ 
  for( ; triples!=ATempty ; triples=ATgetNext(triples))
  { ATermAppl triple=ATAgetFirst(triples);
    result=ATinsertA(result,
                     linMakeTriple( 
                        ATLgetArgument(triple,0),
                        ATLgetArgument(triple,1),
                        gsMakeDataExprAnd(
                          condition,
                          ATAgetArgument(triple,1))));
  }
  return result;
}


static ATermList insertMultiActionInTriples(
                       ATermAppl multiaction,
                       ATermList triples,
                       ATermList result)
{ 
  for( ; triples!=ATempty ; triples=ATgetNext(triples))
  { ATermAppl triple=ATAgetFirst(triples);
    result=ATinsertA(result,
              linMakeTriple(
                 linInsertActionInMultiActionList(
                       multiaction,
                       ATLgetArgument(triple,0)),
                 ATLgetArgument(triple,1),
                 ATAgetArgument(triple,2)));
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

  if (gsGetType(t1)!=gsGetType(t2))
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


static ATermList makeMultiActionConditionList_rec(
                   ATermList actionlabels,
                   ATermAppl targetaction,
                   ATermList multiaction,
                   ATermList equalterms)
{ /* This procedure tries to find a matching set
     of actionlabels in the multiaction, and replaces
     it with the targetaction. If no match can be
     found, the result is ATempty. There are more
     matches possible, for instance a|b->c in
     a(x)|a(y)|b(z) should yield c(x)|a(y) under 
     the condition that x=z and a(x)|c(y) under the
     condition that x!=y and y=z. The pairs of actions
     and conditions are returned in a list. equalterms are
     the variables to which the arguments of communicating
     action must be equal, and its value must be set if the 
     result is not empty.  */

  ATermList result=ATempty;
  assert(actionlabels!=ATempty);

  if (actionlabels==ATempty)
  { /* a match has been found, insert the targetaction
       in the multiaction, under condition true. */
    assert(equalterms!=NULL);
    return ATinsertA(ATempty,
              linMakeTriple(
                 linInsertActionInMultiActionList(
                    gsMakeAction(
                       gsMakeActId(
                          targetaction,
                          getsorts(equalterms)),
                       equalterms),
                    multiaction),
                 equalterms,
                 gsMakeDataExprTrue()));
  }

  if (multiaction==ATempty) /* and actionlabels!=ATempty */
  { /* no match has been found, return the emptylist */
    return ATempty;
  }

  ATermAppl firstaction=ATAgetFirst(actionlabels);
  ATermAppl firstmultiaction=ATAgetFirst(multiaction);
  if (firstaction==ATAgetArgument(ATAgetArgument(firstmultiaction,0),0))
  { /* The labels of the first action and multiaction match.
       First we handle the case where we assume that the first
       action indeed matches with this first multiaction */
    if (equalterms!=NULL)
    { /* there are already actions in the multiaction that took part in
         the communication */
      ATermAppl condition=pairwiseMatch(
                            equalterms,
                            ATLgetArgument(firstmultiaction,1));
      if (condition!=gsMakeDataExprFalse())
      { /* the new action can indeed match too */
        ATermList tempresult=makeMultiActionConditionList_rec(
                                ATgetNext(actionlabels),
                                targetaction,
                                ATgetNext(multiaction),
                                equalterms);
                                
        
        if (tempresult!=ATempty)
        { result=insertConditionInTriples(condition,tempresult,result);
        }
      }
      else
      { /* dataparameters do not match, ignore this case */ ;
      }
    }
    else 
    { /* *equalvar==NULL */
      equalterms=ATLgetArgument(firstmultiaction,1);
      ATermList tempresult=makeMultiActionConditionList_rec(
                                ATgetNext(actionlabels),
                                targetaction,
                                ATgetNext(multiaction),
                                equalterms);
                                
      result=ATconcat(tempresult,result);
    }

    /* Here we handle the case where the action and the 
     * multiaction can match, but we do not let them match */

    ATermList tempresult=makeMultiActionConditionList_rec(
                              actionlabels,
                              targetaction,
                              ATgetNext(multiaction),
                              equalterms);
    if (tempresult!=ATempty)
    { /* There is a communication possible, with dataparameters
         in equalterms */
      ATermAppl inversecondition=pairwiseMatch(
                            equalterms,
                            ATLgetArgument(firstmultiaction,1));
      if (inversecondition!=gsMakeDataExprTrue())
      { /* Apparently a match is possible */
        result=insertConditionInTriples(
                       gsMakeDataExprNot(inversecondition),
                       tempresult,
                       result);
      }
    }
  }
  else 
  { /* the first action in the actionlabels does not match with
       the first multiaction */

    ATermList tempresult=makeMultiActionConditionList_rec(
                          actionlabels,
                          targetaction,
                          ATgetNext(multiaction),
                          equalterms);
    
    result=insertMultiActionInTriples(
                  ATAgetFirst(multiaction),tempresult,result);
  }

  return result;
}

static ATermList makeMultiActionConditionList(
                   ATermList actionlabels,
                   ATermAppl targetaction,
                   ATermList multiaction)
{ 
  ATermList result=makeMultiActionConditionList_rec(
                   actionlabels,
                   targetaction,
                   multiaction,
                   NULL);
  if (result==ATempty)
  { return ATinsertA(ATempty,
              linMakeTriple(multiaction,ATempty,gsMakeDataExprTrue()));
  }
  return result;
}


static ATermAppl communicationcomposition(
                      ATermList communications,
                      ATermAppl ips)
{ 
  ATermList resultsumlist=ATempty;
  ATermList sourcesumlist=linGetSums(ips);

  for( ; sourcesumlist!=ATempty ; sourcesumlist=ATgetNext(sourcesumlist))
  { ATermAppl summand=ATAgetFirst(sourcesumlist);
    ATermList sumvars=linGetSumVars(summand);
    ATermList multiaction=linGetMultiAction(summand);
    ATermAppl actiontime=linGetActionTime(summand);
    ATermAppl condition=linGetCondition(summand);
    ATermList nextstate=linGetNextState(summand);

    for( ; communications!=ATempty ; 
                    communications=ATgetNext(communications))
    { ATermAppl commExpr=ATAgetFirst(communications);
      ATermList actionlabels=ATLgetArgument(ATAgetArgument(commExpr,0),0);
      ATermAppl targetaction=ATAgetArgument(commExpr,1);
      
      actionlabels=sortActionLabels(actionlabels);

      /* the multiactionconditionlist is a list containing
         triples, with a multiaction and the condition,
         expressing whether the multiaction can happen. All
         conditions exclude each other. Furthermore, the list
         is not empty. If no communications can take place,
         the original multiaction is delivered, with condition
         true. */

      ATermList multiactionconditionlist=
                     makeMultiActionConditionList(
                              actionlabels,
                              targetaction,
                              multiaction);

      for( ; multiactionconditionlist!=ATempty ;
               multiactionconditionlist=ATgetNext(multiactionconditionlist) )
      { ATermAppl multiactioncondition=ATAgetFirst(multiactionconditionlist);
        resultsumlist=ATinsertA(
                    resultsumlist,
                    linMakeSummand(
                           sumvars,
                           ATLgetArgument(multiactioncondition,0),
                           actiontime,
                           gsMakeDataExprAnd(
                             condition,
                             ATAgetArgument(multiactioncondition,2)),
                           nextstate));
      }
    }
  }
  return linMakeInitProcSpec(
             linGetInit(ips),linGetParameters(ips),resultsumlist);
}

static ATermAppl makesingleultimatedelaycondition(
                     ATermList sumvars,
                     ATermAppl timevariable,
                     ATermAppl actiontime)
{ ATermAppl result=gsMakeDataExprLessThan(timevariable,actiontime);

  /* for ( ; sumvars!=ATempty ; sumvars=ATgetNext(sumvars) )
  { ATermAppl sumvar=ATAgetFirst(sumvars);
    if (occursinterm(sumvar,actiontime))
    { result=gsMakeExists(sumvar,result);
      / * XXXXXXXXXXX te vervangen door gsMakeExists(sumvar,result); * /
    }
  } */
  return gsMakeExists(sumvars,result);
}

static ATermAppl getUltimateDelayCondition(
                 ATermList sumlist, 
                 ATermAppl timevariable)
{  
   ATermAppl result=gsMakeDataExprTrue();
   for (ATermList walker=sumlist; (walker!=ATempty);
                               walker=ATgetNext(walker))
   { ATermAppl summand=ATAgetFirst(walker);
     ATermList sumvars=linGetSumVars(summand);
     ATermAppl actiontime=linGetActionTime(summand);

     if (actiontime==noTime)
     { return gsMakeDataExprTrue();
     }

     result=gsMakeDataExprAnd(result,
              makesingleultimatedelaycondition(
                     sumvars,
                     timevariable,
                     actiontime));
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
                     specificationbasictype *spec)
/* this function does not work for terminating processes */


{ ATermList resultsumlist=NULL;
  ATermList rename1_list=NULL, rename2_list=NULL, allpars=NULL,
            sums1renaming=NULL, sums2renaming=NULL;
  
  /* ATermTable sums2_hashtable=NULL; / * sums2_hashtable stores lists
                                      of summands of proc2, 
                                      per action label. * /
     ATermList listwalker2=NULL; 

     sums2_hashtable=ATtableCreate(64,90); */
  
  allpars=ATconcat(par1,par3);
  resultsumlist=ATempty;

  /* first we enumerate the summands of t1 */

  ATermAppl timevar=getfreshvariable(
                       "timevariable",
                       gsMakeSortId(gsString2ATermAppl("Real")));
  ATermAppl ultimatedelaycondition=
                   getUltimateDelayCondition(sumlist2,timevar);

  for (ATermList walker1=sumlist1; (walker1!=ATempty); 
                        walker1=ATgetNext(walker1)) 
  { ATermAppl summand1=ATAgetFirst(walker1);

    ATermList sumvars1=linGetSumVars(summand1);
    ATermList sumvars1new=ATempty;
    ATermList multiaction1=linGetMultiAction(summand1);
    ATermAppl actiontime1=linGetActionTime(summand1); 
    ATermAppl condition1=linGetCondition(summand1);
    ATermList nextstate1=linGetNextState(summand1); 

    if (linIsTerminated(nextstate1))
    { ATerror("Internal: terminating processes are impossible at this point"); 
    }
    
    if (actiontime1==noTime)
    { if (ultimatedelaycondition!=gsMakeDataExprTrue())
      { actiontime1=timevar;
        sumvars1=ATinsertA(sumvars1,timevar);
        condition1=gsMakeDataExprAnd(ultimatedelaycondition,condition1);
      }
    }
    else
    { /* actiontime1!=noTime. Substitute the time expression for
         timevar in ultimatedelaycondition, and extend the condition */
      ultimatedelaycondition=
              substitute_data(
                 ATinsertA(ATempty,timevar),
                 ATinsertA(ATempty,actiontime1),
                 ultimatedelaycondition);
      condition1=gsMakeDataExprAnd(ultimatedelaycondition,condition1);
    }

    rename1_list=construct_renaming(
                     allpars,
                     sumvars1,
                     &sumvars1new,
                     &sums1renaming);

    resultsumlist=
      ATinsertA(
        resultsumlist,
        linMakeSummand(
           sumvars1new,
           substitute_multiaction(sums1renaming,rename1_list,multiaction1), 
           substitute_time(sums1renaming,rename1_list,actiontime1),
           substitute_data(sums1renaming,rename1_list,condition1),
           ATconcat(
               substitute_datalist(sums1renaming,rename1_list,nextstate1),
               par3)));
  }

  /* second we enumerate the summands of sumlist2 */

  /* for (walker2=t2;
          ATmatch(walker2,
              "ins(smd(<term>,<str>,<term>,i(<term>),<term>),<term>)",
          &sums2, &act2,&actargs2,&procargs2, &condition2,&walker2);) */

  ultimatedelaycondition=
                   getUltimateDelayCondition(sumlist1,timevar);

  for (ATermList walker2=sumlist2; walker2!=ATempty;
         walker2=ATgetNext(walker2) )
  { 
    ATermAppl summand2=ATAgetFirst(walker2);
    ATermList sumvars2=linGetSumVars(summand2);
    ATermList sumvars2new=ATempty;
    ATermList multiaction2=linGetMultiAction(summand2);
    ATermAppl actiontime2=linGetActionTime(summand2); 
    ATermAppl condition2=linGetCondition(summand2);
    ATermList nextstate2=linGetNextState(summand2); 

    if (linIsTerminated(nextstate2))
    { ATerror("Cannot deal with terminating processes"); 
    }
     
    if (actiontime2==noTime)
    { if (ultimatedelaycondition!=gsMakeDataExprTrue())
      { actiontime2=timevar;
        sumvars2=ATinsertA(sumvars2,timevar);
        condition2=gsMakeDataExprAnd(ultimatedelaycondition,condition2);
      }
    }
    else
    { /* actiontime1!=noTime. Substitute the time expression for
         timevar in ultimatedelaycondition, and extend the condition */
      ultimatedelaycondition=
              substitute_data(
                 ATinsertA(ATempty,timevar),
                 ATinsertA(ATempty,actiontime2),
                 ultimatedelaycondition);
      condition2=gsMakeDataExprAnd(ultimatedelaycondition,condition2);
    }

    rename2_list=construct_renaming(
                         allpars,
                         sumvars2,
                         &sumvars2new,
                         &sums2renaming);
    
    resultsumlist=
      ATinsertA(
        resultsumlist,
        linMakeSummand(
           sumvars2new,
           substitute_datalist(par2,rename_list,
               substitute_multiaction(sums2renaming,rename2_list,multiaction2)), 
           substitute_time(par2,rename_list,
               substitute_time(sums2renaming,rename2_list,actiontime2)),
           substitute_data(par2,rename_list,
               substitute_data(sums2renaming,rename2_list,condition2)),
           ATconcat(
               par1,substitute_datalist(par2,rename_list,
               substitute_datalist(sums2renaming,rename2_list,nextstate2)))));

  }

  /* thirdly we enumerate all communications */

  for (ATermList walker1=sumlist1; walker1!=ATempty;
                  walker1=ATgetNext(walker1))
  { ATermAppl summand1=ATAgetFirst(walker1);

    ATermList sumvars1=linGetSumVars(summand1);
    ATermList sumvars1new=ATempty;
    ATermList multiaction1=linGetMultiAction(summand1);
    ATermAppl actiontime1=linGetActionTime(summand1);
    ATermAppl condition1=linGetCondition(summand1);
    ATermList nextstate1=linGetNextState(summand1);


     
    if (linIsTerminated(nextstate1))
    { ATerror("Cannot deal with terminating processes\n"); 
    }

    rename1_list=construct_renaming(allpars,
                                    sumvars1,&sumvars1new,&sums1renaming);

    multiaction1=substitute_multiaction(
                           sums1renaming,
                           rename1_list,
                           multiaction1);
    nextstate1=substitute_datalist(sums1renaming,rename1_list,nextstate1);
    actiontime1= substitute_time(sums1renaming,rename1_list,actiontime1);
    condition1= substitute_data(sums1renaming,rename1_list,condition1);

    for (ATermList walker2=sumlist2; walker2!=ATempty;
         walker2=ATgetNext(walker2) )
    {
      ATermAppl summand2=ATAgetFirst(walker2);
      ATermList sumvars2=linGetSumVars(summand2);
      ATermList sumvars2new=ATempty;
      ATermList multiaction2=linGetMultiAction(summand2);
      ATermAppl actiontime2=linGetActionTime(summand2);
      ATermAppl condition2=linGetCondition(summand2);
      ATermList nextstate2=linGetNextState(summand2);

      if (linIsTerminated(nextstate2))
         { ATerror("Cannot deal with terminating processes"); }

      rename2_list=construct_renaming(
               ATconcat(sumvars1new,allpars),
               sumvars2,&sumvars2new,&sums2renaming);
      
      ATermList multiaction3=
        linMergeMultiActionList(
           multiaction1,
           substitute_multiaction(par2,rename_list,
              substitute_multiaction(sums2renaming,
                                     rename2_list,
                                     multiaction2)));
      ATermList allsums=ATconcat(sumvars1new,sumvars2new);
      actiontime2=substitute_time(par2,rename_list,
              substitute_time(sums2renaming,rename2_list,actiontime2));

      condition2=substitute_data(par2,rename_list,
              substitute_data(sums2renaming,rename2_list,condition2));
      ATermAppl condition3= gsMakeDataExprAnd(condition1,condition2);

      ATermAppl actiontime3=NULL;

      if (actiontime1==noTime)
      { if (actiontime2==noTime)
        { actiontime3=noTime;
        }
        else
        { /* actiontime2!=noTime */
          actiontime3=actiontime2;
        }
      }
      else
      { /* actiontime1!=noTime */
        if (actiontime2==noTime)
        { actiontime3=actiontime1;
        }
        else
        { /* actiontime1!=noTime && actiontime2!=noTime */
          actiontime3=actiontime1;
          condition3=gsMakeDataExprAnd(
                        condition3,
                        gsMakeDataExprEq(actiontime1,actiontime2));
        }
      }                         

      nextstate2=substitute_datalist(par2,rename_list,
              substitute_datalist(sums2renaming,rename2_list,nextstate2));
      ATermList nextstate3=ATconcat(nextstate1,nextstate2);
      
      resultsumlist=
        ATinsertA(
          resultsumlist,
          linMakeSummand(
           allsums,
           multiaction3,
           actiontime3,
           condition3,
           nextstate3));

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

  fprintf(stderr,"Parallel composition is being translated... ");

  result=combinesumlist(
               linGetSums(t1),
               linGetSums(t2),
               pars1,pars2renaming,pars3,renaming,spec);
  
  fprintf(stderr,"done.\n"); 
  return linMakeInitProcSpec(
               ATconcat(init1,init2),
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
                substitute_datalist(objectdata[n].parameters,args,init),
                pars,
                sums);
}

/**************** GENERATE LPEmCRL **********************************/


/*
 * The following functions encode the hide, encap and rename operators
 * as generalized parallel compositions, used in the multi LPO format.
 */

static ATermAppl encode_hide(ATermList hide_list,ATermAppl proc)
{
  ATermList op_list;
  op_list=ATmakeList0();
  for( ; hide_list!=ATempty ; hide_list=ATgetNext(hide_list))
  { op_list=ATinsert(op_list,ATmake("pair(<term>,\"Tau\")",
                 ATgetArgument(hide_list,0)));
  }

  return (ATermAppl)ATmake(
          "Merge(<term>,[],[],<term>,initprocspec(emt,emv,eml))",
          op_list,proc);
}

static ATermAppl encode_encap(ATermList encap_list,ATermAppl proc)
{ ATermList op_list;
  op_list=ATmakeList0();
  for( ; encap_list!=ATempty ; encap_list=ATgetNext(encap_list))
  { op_list=ATinsert(op_list,ATmake("pair(<term>,\"Delta\")",
                  ATgetArgument(encap_list,0)));
  }

  return (ATermAppl)ATmake(
          "Merge(<term>,[],[],<term>,initprocspec(emt,emv,eml))",
          op_list,proc);
}

static ATermAppl encode_rename(ATermList rename_list, ATermAppl proc)
{ ATermList op_list;
  ATerm emr_term;

  emr_term=ATmake("emr");
  op_list=ATmakeList0();
  for( ; rename_list!=ATempty ;
         rename_list=ATgetNext(rename_list))
  { op_list=ATinsert(op_list,
           ATmake("pair(<term>,<term>)",
              ATgetArgument(ATgetFirst(rename_list),0),
              ATgetArgument(ATgetFirst(rename_list),1)));
  }
  return (ATermAppl)ATmake(
          "Merge(<term>,[],[],<term>,initprocspec(emt,emv,eml))",
          op_list,proc);
}

static ATermAppl generateLPEmCRL(
               ATermAppl procIdDecl,
               int canterminate,
               specificationbasictype *spec, 
               int regular,
               int keep_structure);

static ATermAppl generateLPEmCRLterm(
                   ATermAppl t,
                   int canterminate,
                   specificationbasictype *spec,
                   int regular,
                   int keep_structure) 
{ 
  if (gsIsProcess(t)) 
  { ATermAppl t3=namecomposition(ATAgetArgument(t,0),ATLgetArgument(t,1),
                        generateLPEmCRL(
                              ATAgetArgument(t,0),
                              canterminate,
                              spec,
                              regular,0));
    if (keep_structure)
    { t3=(ATermAppl)ATmake("leaf(<term>,<term>)",ATgetArgument(t,0),t3);
    }
    return t3;
  }
  
  if (gsIsMerge(t)) 
  { ATermAppl t1=generateLPEmCRLterm(
                          ATAgetArgument(t,0),
                          canterminate,
                          spec,
                          regular,
                          keep_structure);
    ATermAppl t2=generateLPEmCRLterm(
                          ATAgetArgument(t,1),
                          canterminate,
                          spec,
                          regular,
                          keep_structure);
    if (keep_structure)
    { return (ATermAppl)ATmake("Merge([],<term>,[],<term>,<term>)",
                          ATmake("comm_term"),t1,t2);
    }
    return parallelcomposition(t1,t2,canterminate,spec);
  }
  
  if (gsIsHide(t)) 
  {  ATermAppl t2=generateLPEmCRLterm(
                          ATAgetArgument(t,1),
                          canterminate,
                          spec,
                          regular,
                          keep_structure);
     if (keep_structure) 
     { return encode_hide(ATLgetArgument(t,0),t2);
     }
     return hidecomposition(ATLgetArgument(t,0),t2);
  }

  if (gsIsAllow(t)) 
  { ATermAppl t2=generateLPEmCRLterm(
                          ATAgetArgument(t,1),
                          canterminate,
                          spec,
                          regular,
                          keep_structure);
    if (keep_structure) 
    { ATerror("Cannot process allow operator with flag keep-structure");
    }
    return allowcomposition(ATLgetArgument(t,0),t2);
  }

  if (gsIsRestrict(t)) 
  { ATermAppl t2=generateLPEmCRLterm(
                          ATAgetArgument(t,1),
                          canterminate,
                          spec,
                          regular,
                          keep_structure);
    if (keep_structure) 
    { encode_encap(ATLgetArgument(t,0),t2);
    }
    return encapcomposition(ATLgetArgument(t,0),t2);
  }
  
  if (gsIsRename(t))
  { ATermAppl t2=generateLPEmCRLterm(
                          ATAgetArgument(t,1),
                          canterminate,
                          spec,
                          regular,
                          keep_structure);

    if (keep_structure) 
    { return encode_rename(ATLgetArgument(t,0),t2);
    }
    return renamecomposition(ATLgetArgument(t,0),t2);
  }

  if (gsIsComm(t))
  { ATermAppl t1=generateLPEmCRLterm(
                          ATAgetArgument(t,1),
                          canterminate,
                          spec,
                          regular,
                          keep_structure);

    return communicationcomposition(ATLgetArgument(t,0),t1);
  }
   
  else ATerror("Expect mCRL term %t",t);

  return NULL;
}

/**************** GENERATE LPEmCRL **********************************/

static ATermAppl generateLPEmCRL(
                       ATermAppl procIdDecl,
                       int canterminate,
                       specificationbasictype *spec,
                       int regular,
                       int keep_structure)
{
/* generates a pair of a initial argument list and a
   linear process expression, representing the initial ATerm .
   If regular=1, then a regular version of the pCRL processes
   must be generated 

*/
  long n=objectIndex(procIdDecl);
  
  if ((objectdata[n].processstatus==GNF)||
      (objectdata[n].processstatus==pCRL)||
      (objectdata[n].processstatus==GNFalpha))
  { ATermAppl t3=generateLPEpCRL(procIdDecl,
        (canterminate&&objectdata[n].canterminate),spec,regular);
    if (keep_structure) 
    { return (ATermAppl)ATmake("leaf(<str>,<term>)","X",t3);
    }
    return t3;
  }
  /* process is a mCRLdone ATerm */
  if ((objectdata[n].processstatus==mCRLdone)||
              (objectdata[n].processstatus==mCRLlin)||
              (objectdata[n].processstatus==mCRL))
  { objectdata[n].processstatus=mCRLlin;
    return generateLPEmCRLterm(objectdata[n].processbody,
             (canterminate&&objectdata[n].canterminate),spec,
             regular,keep_structure); 
  }

  ATerror("Internal error. Laststatus: %d",objectdata[n].processstatus);
  return NULL;
}

/*********************** initialize_data **************/

static void initialize_data(void)
{ 
  ATprotect((ATerm *)&pcrlprocesses);
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
  ATprotect((ATerm *)&sumlist);
  ATprotect((ATerm *)&localequationvariables);
  
  initialize_symbols();
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
              substitute_data(varlist,tl,ATAgetArgument(t,1)));
  }  
  
  if (gsIsCond(t))
  { assert(gsIsDelta(ATAgetArgument(t,2)));
    return gsMakeCond(
              substitute_data(varlist,tl,ATAgetArgument(t,0)),
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
              substitute_datalist(varlist,tl,ATLgetArgument(t,1)));
  }  

  if (gsIsAction(t)) 
  { return gsMakeAction(ATAgetArgument(t,0),
              substitute_datalist(varlist,tl,ATLgetArgument(t,1)));
  }  
 
  if (linIsMultiAction(t))
  { return linMakeMultiAction(
               substitute_multiaction(varlist,tl,ATLgetArgument(t,0)));
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
  
  ATerror("Internal: Unexpected process format in alphaconversionterm %t",t);
  return NULL;
}

static void alphaconversion(ATermAppl procId, ATermList parameters)
{ 
  long n=objectIndex(procId);

  if (objectdata[n].processstatus==GNF)
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
  else ATerror("Unknown type %d in alphaconversion of %t",
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
              ATermIndexedSet visited)
{ 
  if (gsIsMerge(t))
  { /* the construction below is needed to guarantee that 
       both subterms are recursively investigated */
    int r1=canterminatebody(ATAgetArgument(t,0),stable,visited);
    int r2=canterminatebody(ATAgetArgument(t,1),stable,visited);
    return r1&&r2;
  }

  if (gsIsProcess(t))
  { return (canterminate_rec(ATAgetArgument(t,0),stable,visited));
  }

  if (gsIsHide(t)) 
  { return (canterminatebody(ATAgetArgument(t,1),stable,visited));
  }

  if (gsIsRename(t))
  { return (canterminatebody(ATAgetArgument(t,1),stable,visited));
  }

  if (gsIsAllow(t))
  { return (canterminatebody(ATAgetArgument(t,1),stable,visited));
  }

  if (gsIsRestrict(t))
  { return (canterminatebody(ATAgetArgument(t,1),stable,visited));
  }

  if (gsIsComm(t))
  { return (canterminatebody(ATAgetArgument(t,1),stable,visited));
  }

  if (gsIsChoice(t)) 
  { int r1=canterminatebody(ATAgetArgument(t,0),stable,visited);
    int r2=canterminatebody(ATAgetArgument(t,1),stable,visited);
    return r1||r2;
  }

  if (gsIsSeq(t))
  { int r1=canterminatebody(ATAgetArgument(t,0),stable,visited);
    int r2=canterminatebody(ATAgetArgument(t,1),stable,visited);
    return r1&&r2;
  }

  if (gsIsCond(t))
  { int r1=canterminatebody(ATAgetArgument(t,1),stable,visited);
    int r2=canterminatebody(ATAgetArgument(t,2),stable,visited);
    return r1||r2;
  }

  if (gsIsSum(t))
  { return (canterminatebody(ATAgetArgument(t,1),stable,visited));
  }

  if (gsIsAction(t)) 
  { return 1;
  }

  if (gsIsDelta(t))
  { return 0;
  }

  if (gsIsTau(t))
  { return 1;
  }

  if (gsIsAtTime(t))
  { return canterminatebody(ATAgetArgument(t,0),stable,visited);
  }

  if (gsIsSync(t))
  { int r1=canterminatebody(ATAgetArgument(t,0),stable,visited);
    int r2=canterminatebody(ATAgetArgument(t,1),stable,visited);
    return r1&&r2;
  }

  ATerror("Internal Error. Unexpected process format in canterminate%t",t);
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
  { int ct=canterminatebody(objectdata[n].processbody,stable,visited); 
    if (objectdata[n].canterminate!=ct)
    { objectdata[n].canterminate=ct;
      ATfprintf(stderr,"CANTERMINATE %t %d\n",procId,ct);
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
    { ATerror("Unguarded recursion in the mCRL part of the input");
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
  
  ATtablePut(visited,(ATerm)procId,(ATerm)newProcId);  

  if (objectdata[n].processstatus==mCRL)
  { 
    insertProcDeclaration(
                newProcId,
                objectdata[n].parameters,
                split_body(objectdata[n].processbody,
                           visited,
                           objectdata[n].parameters),
                mCRL);
    return newProcId;
  }

  if (objectdata[n].canterminate)
  { insertProcDeclaration(
                newProcId,
                objectdata[n].parameters,
                gsMakeSeq(objectdata[n].processbody,terminatedProc),
                pCRL);
    return newProcId;
  }
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
    { if (canterminatebody(t,NULL,NULL))
      { result=newprocess(parameters,gsMakeSeq(t,terminatedProc),pCRL);
      }
      else
      { result=newprocess(parameters,t,pCRL);
      }
    }
    else
    ATerror("Internal Error. Unexpected process format in split process %t",t);
  }

  ATtablePut(visited,(ATerm)t,(ATerm)ATgetArgument(result,0));
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

/**************** transform **************************************/

static ATermAppl transform(
                     ATermAppl init, 
                     specificationbasictype *spec,
                     int keep_structure)
{ ATermList pcrlprocesslist;
  ATermAppl t3=NULL;

  /* Then select the BPA processes, and check that the others
     are proper parallel processes */
  pcrlprocesslist=determine_process_status(init,mCRL);
  determinewhetherprocessescanterminate(init);
  splitmCRLandpCRLprocsAndAddTerminatedAction(init);

  if (pcrlprocesslist==ATempty) 
  { ATerror("There are no pCRL processes to be linearized"); }

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
           regular,
           keep_structure);
  if (cluster)
     t3=clusterfinalresult(t3,spec);
  return t3;
}

/*--- main program -----------------------------*/

static int main2(int argc, char *argv[],ATerm *stack_bottom)
{ 
  int i = 1;
  char *sname = NULL, *oname = NULL;
  specificationbasictype *spec;
  char messagebuffer[1024]="Unitialized messagebuffer";
  ATermAppl initial_process=NULL;
  ATermAppl result=NULL;
  char fname[1024], iname[1024];
  
  fname[0]='\0';
  to_toolbusfile=0;
  to_stdout=0;
  for(i = 1; i < argc; i++){
    if(strequal(argv[i], "-version")){
      version(); exit(0);
    } else if(strequal(argv[i], "-help")){
      help(); exit(0);
    } else if (strequal(argv[i], "-tbfile") || strequal(argv[i], "-tbf")){
      if (to_stdout==1)
         ATerror("Options -tbfile and -stdout cannot be used together");
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
    } else if(strequal(argv[i], "-statenames")){
      statenames=1;
    } else if(strequal(argv[i], "-multi")){
      writemulti=1;
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
      if (lastdot && !strcmp(lastdot,".test")) *lastdot = '\0';   
      break; 
    } 
  }
  if (to_stdout==0 && 
     (regular || nocluster || cluster || binary || !oldstate)) 
      to_toolbusfile=1; 
  if (!oldstate && !regular && !regular2)
    ATerror("Option -newstate can only be used with -regular or -regular2");

  ATinit(argc,argv,stack_bottom);
  ATprotect((ATerm *)&result);
  if (((argc < 2)||(sname==NULL)))
      usage();
  initialize_data();

  strcpy(iname, sname);
  infile=fopen(iname,"r");
  if (infile==NULL) { 
     if (ExtensionAdded(iname, ".test")) {
           infile=fopen(iname,"r");
          }
     }
  if (infile==NULL)
        ATerror ("Cannot open input file `%s'", iname);
  fclose(infile);
  if (to_toolbusfile)
   { sprintf(messagebuffer,"%s.tbf",oname);
     toolbusfile=fopen(messagebuffer,"w");
     if (toolbusfile==NULL)
        ATerror("Cannot open file for output"); }
    spec=read_internal_format(iname); 
                          /* if this terminates ok,  */
    initial_process=storedata(spec);
    if ((to_toolbusfile)||(to_stdout))
    { result=transform(initial_process,spec,(writemulti?1:0));
      ATfprintf(stderr,"RESULT %t\n",result);
      ATwriteToTextFile(ATmake(
         writemulti?"spec2genM(d(s(<term>,<term>,<term>),<term>),<term>)":
                    "spec2gen(d(s(<term>,<term>,<term>),<term>),<term>)",
                 spec->sorts,
                 spec->funcs,
                 spec->maps,
                 spec->eqns,
                 result),
                 to_stdout?stdout:toolbusfile);
    }
  else ATfprintf(stderr,"Input appears to be correct mCRL2");  
  return 0;
}

int main(int argc, char *argv[])
{
  ATerm stack_bottom;
  return main2(argc,argv,&stack_bottom);
}

