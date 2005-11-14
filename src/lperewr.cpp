#define NAME "lperewr"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <aterm2.h>
#include <assert.h>
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "libgsrewrite.h"

static bool gsIsLPESpec(ATermAppl Spec)
{
  return gsIsLPE(ATAgetArgument(Spec,5));
}

static ATermAppl rewrite_proc(ATermAppl p)
{
  ATermList l,m;

  if ( gsIsAction(p) || gsIsProcess(p) )
  {
    return ATmakeAppl2(ATgetAFun(p),ATgetArgument(p,0),(ATerm) gsRewriteTerms(ATLgetArgument(p,1)));
  } else if ( gsIsAtTime(p) )
  {
    return ATmakeAppl2(ATgetAFun(p),ATgetArgument(p,0),(ATerm) gsRewriteTerm(ATAgetArgument(p,1)));
  } else if ( gsIsSum(p) || gsIsAllow(p) || gsIsBlock(p) || gsIsHide(p) || gsIsRename(p) || gsIsComm(p) )
  {
    return ATmakeAppl2(ATgetAFun(p),ATgetArgument(p,0),(ATerm) rewrite_proc(ATAgetArgument(p,1)));
  } else if ( gsIsCond(p) )
  {
    return gsMakeCond(gsRewriteTerm(ATAgetArgument(p,0)),rewrite_proc(ATAgetArgument(p,1)),rewrite_proc(ATAgetArgument(p,2)));
  } else {
    l = ATgetArguments(p);
    m = ATmakeList0();
    for (; !ATisEmpty(l); l=ATgetNext(l))
    {
      m = ATinsert(m,(ATerm) rewrite_proc(ATAgetFirst(l)));
    }
    return ATmakeApplList(ATgetAFun(p),ATreverse(m));
  }
}

static ATermAppl rewrite_nolpe(ATermAppl Spec)
{
  ATermAppl a;
  ATermList l,m;

  l = ATLgetArgument(ATAgetArgument(Spec,5),0);
  m = ATmakeList0();
  for (; !ATisEmpty(l); l=ATgetNext(l))
  {
    a = ATAgetArgument(ATAgetFirst(l),3);
    a = rewrite_proc(a);
    m = ATinsert(m,(ATerm) ATsetArgument(ATAgetFirst(l),(ATerm) a,3));
  }
  m = ATreverse(m);
  Spec = ATsetArgument(Spec,(ATerm) gsMakeProcEqnSpec(m),5);

  a = ATAgetArgument(ATAgetArgument(Spec,6),1);
  a = rewrite_proc(a);
  Spec = ATsetArgument(Spec,(ATerm) gsMakeInit(ATLgetArgument(ATAgetArgument(Spec,6),0),a),6);

  return Spec;
}

static ATermAppl rewrite_lpe(ATermAppl Spec)
{
  ATermList l;
  //rewrite data equations
  ATermAppl DataEqnSpec = ATAgetArgument(Spec, 3);
  ATermList DataEqns = ATLgetArgument(DataEqnSpec, 0);
  l = ATmakeList0();
  for (; !ATisEmpty(DataEqns); DataEqns = ATgetNext(DataEqns)) {
    //rewrite data equation
    ATermAppl DataEqn = ATAgetFirst(DataEqns);
    //rewrite condition
    ATermAppl Cond = ATAgetArgument(DataEqn, 1);
    if (!gsIsNil(Cond)) {
      Cond = gsRewriteTerm(Cond);
      DataEqn = ATsetArgument(DataEqn, (ATerm) Cond, 1);
    }
    //rewrite right hand side
    ATermAppl RHS = ATAgetArgument(DataEqn, 3);
    RHS = gsRewriteTerm(RHS);
    DataEqn = ATsetArgument(DataEqn, (ATerm) RHS, 3);
    l = ATinsert(l, (ATerm) DataEqn);
  }
  DataEqns = ATreverse(l);
  DataEqnSpec = ATsetArgument(DataEqnSpec, (ATerm) DataEqns, 0);
  Spec = ATsetArgument(Spec, (ATerm) DataEqnSpec, 3);

  //rewrite LPE summands
  ATermAppl LPE = ATAgetArgument(Spec, 5);
  ATermList LPESummands = ATLgetArgument(LPE, 2);
  l = ATmakeList0();
  for (; !ATisEmpty(LPESummands); LPESummands = ATgetNext(LPESummands)) {
    //rewrite LPE summand
    ATermAppl LPESummand = ATAgetFirst(LPESummands);
    ATermList LPEVars = ATLgetArgument(LPESummand, 0);
    ATermAppl Cond = ATAgetArgument(LPESummand, 1);
    Cond = gsRewriteTerm(Cond);
    ATermAppl MultAct = ATAgetArgument(LPESummand, 2);
    if ( gsIsMultAct(MultAct) ) {
      ATermList Acts = ATLgetArgument(MultAct, 0);
      ATermList m = ATmakeList0();
      for (; !ATisEmpty(Acts); Acts = ATgetNext(Acts))
      {
        ATermAppl Act = ATAgetFirst(Acts);
        ATermList Pars = ATLgetArgument(Act, 1);
        Pars = gsRewriteTerms(Pars);
        Act = ATsetArgument(Act, (ATerm) Pars, 1);
        m = ATinsert(m, (ATerm) Act);
      }
      MultAct = gsMakeMultAct(ATreverse(m));
    }
    ATermAppl Time = ATAgetArgument(LPESummand, 3);
    if ( !gsIsNil(Time) ) {
      Time = gsRewriteTerm(Time);
    }
    ATermList Assignments = ATLgetArgument(LPESummand, 4);
    ATermList m = ATmakeList0();
    for (; !ATisEmpty(Assignments); Assignments = ATgetNext(Assignments)) {
      ATermAppl Assignment = ATAgetFirst(Assignments);
      ATermAppl RHS = ATAgetArgument(Assignment, 1);
      RHS = gsRewriteTerm(RHS);
      Assignment = ATsetArgument(Assignment, (ATerm) RHS, 1);
      m = ATinsert(m, (ATerm) Assignment);
    }
    Assignments = ATreverse(m);
    LPESummand = gsMakeLPESummand(LPEVars, Cond, MultAct, Time, Assignments);
    l = ATinsert(l, (ATerm) LPESummand);
  }
  LPESummands = ATreverse(l);
  LPE = ATsetArgument(LPE, (ATerm) LPESummands, 2);
  Spec = ATsetArgument(Spec, (ATerm) LPE, 5);
 
  //rewrite initial state
  ATermAppl LPEInit = ATAgetArgument(Spec, 6);
  ATermList Assignments = ATLgetArgument(LPEInit, 1);
  l = ATmakeList0();
  for (; !ATisEmpty(Assignments); Assignments = ATgetNext(Assignments)) {
    ATermAppl Assignment = ATAgetFirst(Assignments);
    ATermAppl RHS = ATAgetArgument(Assignment, 1);
    RHS = gsRewriteTerm(RHS);
    Assignment = ATsetArgument(Assignment, (ATerm) RHS, 1);
    l = ATinsert(l, (ATerm) Assignment);
  }
  Assignments = ATreverse(l);
  LPEInit = ATsetArgument(LPEInit, (ATerm) Assignments, 1);
  Spec = ATsetArgument(Spec, (ATerm) LPEInit, 6);
 
  return Spec;
}

static void print_help(FILE *f, char *Name)
{
  fprintf(f,
    "Usage: %s [OPTION]... [INFILE [OUTFILE]]\n"
    "Rewrite data expressions in INFILE and save the result to OUTFILE. If OUTFILE is\n"
    "not present, stdout is used. If INFILE is not present, stdin is used. To use\n"
    "stdin and save the output into a file, use '-' for INFILE.\n"
    "\n"
    "Mandatory arguments to long options are mandatory for short options too.\n"
    "  -h, --help            display this help message\n"
    "  -q, --quiet           do not print any unrequested information\n"
    "  -v, --verbose         print information about the rewriting process\n"
    "  -b, --benchmark=NUM   rewrites specification NUM times\n"
    "  -R, --rewriter=NAME   use rewriter NAME (default 'inner3')\n",
    Name
  );
}

int main(int argc, char **argv)
{
  FILE *InStream, *OutStream;
  ATerm stackbot;
  ATermAppl Spec;
  #define sopts "hqvb:R:"
  struct option lopts[] = {
    { "help",    no_argument,    NULL,  'h' },
    { "quiet",    no_argument,    NULL,  'q' },
    { "verbose",    no_argument,    NULL,  'v' },
    { "benchmark",    required_argument,  NULL,  'b' },
    { "rewriter",    required_argument,  NULL,  'R' },
    { 0, 0, 0, 0 }
  };
  int opt,i,bench_times;
  RewriteStrategy strat;
  bool quiet,verbose,benchmark;

  ATinit(argc,argv,&stackbot);

  quiet = false;
  verbose = false;
  benchmark = false;
  bench_times = 0;
  strat = GS_REWR_INNER3;
  while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
  {
    switch ( opt )
    {
      case 'h':
        print_help(stderr, argv[0]);
        return 0;
      case 'q':
        quiet = true;
        break;
      case 'v':
        verbose = true;
        break;
      case 'b':
        benchmark = true;
        bench_times = strtoul(optarg,NULL,0);
        break;
      case 'R':
        strat = RewriteStrategyFromString(optarg);
        if ( strat == GS_REWR_INVALID )
        {
          gsErrorMsg("invalid rewrite strategy '%s'\n",optarg);
          return 1;
        }
        break;
      default:
        break;
    }
  }
  if ( quiet && verbose )
  {
    gsErrorMsg("options -q/--quiet and -v/--verbose cannot be used together\n");
    return 1;
  }
  if ( quiet )
    gsSetQuietMsg();
  if ( verbose )
    gsSetVerboseMsg();

  InStream = stdin;
  char *InFileName = NULL;
  if ( optind < argc && strcmp(argv[optind],"-") )
  {
    InFileName = argv[optind];
    if ( (InStream = fopen(InFileName,"rb")) == NULL )
    {
      gsErrorMsg("could not open input file '%s' for reading: ", InFileName);
      perror(NULL);
      return 1;
    }
  }

  OutStream = stdout;
  char *OutFileName = NULL;
  if ( !benchmark )
  {
    if ( optind+1 < argc )
    {
      OutFileName = argv[optind+1];
      if ( (OutStream = fopen(OutFileName, "wb")) == NULL )
      {
        gsErrorMsg("could not open output file '%s' for writing: ",
          OutFileName);
        perror(NULL);
        return 1;
      }
    }
  }


  if ( InStream == stdin )
    gsVerboseMsg("reading input from stdin...\n");
  else
    gsVerboseMsg("reading input from '%s'...\n", InFileName);
  Spec = (ATermAppl) ATreadFromFile(InStream);
  if ( Spec == NULL )
  {
    if (InStream == stdin) {
      gsErrorMsg("could not read LPE from stdin\n");
    } else {
      gsErrorMsg("could not read LPE from '%s'\n", InFileName);
      fclose(InStream);
    }
    return 1;
  }
  assert(Spec != NULL);
  gsEnableConstructorFunctions();
  if (!gsIsSpecV1(Spec)) {
    if (InStream == stdin) {
      gsErrorMsg("stdin does not contain an LPE\n");
    } else {
      gsErrorMsg("'%s' does not contain an LPE\n", InFileName);
      fclose(InStream);
    }
    return false;
  }
  assert(gsIsSpecV1(Spec));
  gsVerboseMsg("initialising rewriter...\n");
  gsRewriteInit(ATAgetArgument(Spec,3),strat);

  if ( benchmark )
  {
    gsVerboseMsg("benchmarking %i times...\n",bench_times);
    if ( gsIsLPESpec(Spec) )
    {
      for(i=0; i<bench_times; i++)
      {
        rewrite_lpe(Spec);
      }
    } else {
      for(i=0; i<bench_times; i++)
      {
        rewrite_nolpe(Spec);
      }
    }
  } else {
    gsVerboseMsg("rewriting input...\n",bench_times);
    if ( gsIsLPESpec(Spec) )
    {
      Spec = rewrite_lpe(Spec);
    } else {
      Spec = rewrite_nolpe(Spec);
    }

    if ( OutStream == stdout )
      gsVerboseMsg("writing result to stdout...\n");
    else
      gsVerboseMsg("writing result to '%s'...\n", OutFileName);
    ATwriteToBinaryFile((ATerm) Spec, OutStream);
    if ( OutStream != stdout )
    {
      fclose(OutStream);
    }
  }
}
