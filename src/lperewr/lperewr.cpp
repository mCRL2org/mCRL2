#define NAME "lperewr"
#define VERSION "0.2"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <aterm2.h>
#include <assert.h>
#include <limits.h>
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "librewrite.h"

//Functions used by the main program
static void print_help(char *Name);
static void print_version(void);
static void print_more_info(char *Name);
static bool is_valid_lpe(ATermAppl spec);
static ATermAppl rewrite_lpe(ATermAppl Spec);

//Globally used rewriter
static Rewriter *rewr;

//Main program

int main(int argc, char **argv)
{
  //declarations for getopt
  bool opt_benchmark = false;
  unsigned long int  opt_bench_times = 0;
  RewriteStrategy opt_strat = GS_REWR_INNER;
  #define SHORT_OPTIONS "hqvb:R:"
  #define VERSION_OPTION CHAR_MAX + 1
  struct option long_options[] = {
    { "help",      no_argument,        NULL,  'h' },
    { "version",   no_argument,        NULL,  VERSION_OPTION },
    { "quiet",     no_argument,        NULL,  'q' },
    { "verbose",   no_argument,        NULL,  'v' },
    { "benchmark", required_argument,  NULL,  'b' },
    { "rewriter",  required_argument,  NULL,  'R' },
    { 0, 0, 0, 0 }
  };
  int option;
  //parse options
  while ((option = getopt_long(argc, argv, SHORT_OPTIONS, long_options, NULL)) != -1) {
    switch (option) {
      case 'h': /* help */
        print_help(argv[0]);
        return 0;
      case VERSION_OPTION: /* version */
        print_version();
        return 0;
      case 'q': /* quite */
        gsSetQuietMsg();
        break;
      case 'v': /* verbose */
        gsSetVerboseMsg();
        break;
      case 'b': /* benchmark */
        opt_benchmark = true;
        opt_bench_times = strtoul(optarg,NULL,0);
        break;
      case 'R': /* rewriter */
        opt_strat = RewriteStrategyFromString(optarg);
        if ( opt_strat == GS_REWR_INVALID ) {
          gsErrorMsg("invalid rewrite strategy '%s'\n", optarg);
          return 1;
        }
        break;
      default:
        print_more_info(argv[0]);
        return 1;
    }
  }
  //check for wrong number of arguments
  char *infilename   = NULL;
  char *outfilename = NULL;
  int noargc; //non-option argument count
  noargc = argc - optind;
  if (noargc > 2) {
    fprintf(stderr, "%s: too many arguments\n", NAME);
    print_more_info(argv[0]);
    return 1;
  } else {
    //noargc >= 0 && noargc <= 2
    if (noargc > 0) {
      infilename = strdup(argv[optind]);
    }
    if (noargc == 2) {
      outfilename = strdup(argv[optind + 1]);
      //check if input and output files are the same; disabled since it is not
      //problematic
      /*
      if (strcmp(infilename,outfilename) == 0) {
        gsErrorMsg("input and output files are the same\n");
        free(infilename);
        free(outfilename);
        return 1;
      }
      */      
      if (opt_benchmark) {
        gsWarningMsg("output will not be saved to '%s'\n",outfilename);
      }      
    }
  }

  //initialise ATerm library
  ATerm stackbot;
  ATinit(argc,argv,&stackbot);

  //open infilename
  FILE *instream;
  if (infilename == NULL) {
    instream = stdin;
  } else {
    instream = fopen(infilename, "rb");
    if (instream == NULL) {
      gsErrorMsg("cannot open input file '%s'\n", infilename);
      return 1;
    }
  }
  assert(instream != NULL);

  //read LPE from instream
  if ( instream == stdin )
    gsVerboseMsg("reading input from stdin...\n");
  else
    gsVerboseMsg("reading input from '%s'...\n", infilename);
  ATermAppl result = (ATermAppl) ATreadFromFile(instream);
  ATprotectAppl(&result); // ATerms in main are not safe
  if ( result == NULL )
  {
    if (instream == stdin) {
      gsErrorMsg("could not read LPE from stdin\n");
    } else {
      gsErrorMsg("could not read LPE from '%s'\n", infilename);
      fclose(instream);
    }
    free(infilename);
    free(outfilename);
    return 1;
  }
  assert(result != NULL);

  //check if result is an LPE
  gsEnableConstructorFunctions();
  if (!is_valid_lpe(result)) {
    if (instream == stdin) {
      gsErrorMsg("stdin does not contain an LPE\n");
    } else {
      gsErrorMsg("'%s' does not contain an LPE\n", infilename);
      fclose(instream);
    }
    free(infilename);
    free(outfilename);
    return 1;
  }
  assert(is_valid_lpe(result));

  //initialise rewriter
  if (gsVerbose) {
    fprintf(stderr, "initialising rewriter ");
    PrintRewriteStrategy(stderr, opt_strat);  
    fprintf(stderr, "...\n");
  }
  rewr = createRewriter(lpe::data_specification(ATAgetArgument(result,0)), opt_strat);

  //rewrite result
  if (opt_benchmark) {
    //rewrite result opt_bench_times
    gsVerboseMsg("rewriting LPE %lu times...\n", opt_bench_times);
    for (unsigned long i = 0; i < opt_bench_times; i++) {
      rewrite_lpe(result);
    }
  } else {
    gsVerboseMsg("rewriting LPE...\n");
    result = rewrite_lpe(result);
    //store the result
    if (outfilename == NULL) {
      gsVerboseMsg("saving result to stdout...\n");
      ATwriteToBinaryFile((ATerm) result, stdout);
      fprintf(stdout, "\n");
    } else { //outfilename != NULL
      //open output filename
      FILE *outstream = fopen(outfilename, "wb");
      if (outstream == NULL) {
        gsErrorMsg("cannot open output file '%s'\n", outfilename);
        free(infilename);
        free(outfilename);
        return 1;
      }
      gsVerboseMsg("saving result to '%s'...\n", outfilename);
      ATwriteToBinaryFile((ATerm) result, outstream);
      fclose(outstream);
    }
  }
  delete rewr;
  ATunprotectAppl(&result);
  free(infilename);
  free(outfilename);
  return 0;
}

static void print_help(char *Name)
{
  fprintf(stderr,
    "Usage: %s [OPTION]... [INFILE [OUTFILE]]\n"
    "Rewrite data expressions of the LPE in INFILE and save the result to OUTFILE.\n"
    "If OUTFILE is not present, stdout is used. If INFILE is not present, stdin is\n"
    "used.\n"
    "\n"
    "Mandatory arguments to long options are mandatory for short options too.\n"
    "  -h, --help            display this help message and terminate\n"
    "      --version         display version information and terminate\n"
    "  -q, --quiet           do not display warning messages\n"
    "  -v, --verbose         display concise intermediate messages\n"
    "  -b, --benchmark=NUM   rewrite specification NUM times; do not save output\n"
    "  -R, --rewriter=NAME   use rewriter NAME (default 'inner')\n",
    Name
  );
}

void print_version(void)
{
  fprintf(stderr,"%s %s (revision %s)\n", NAME, VERSION, REVISION);
}

void print_more_info(char *Name)
{
  fprintf(stderr, "Use %s --help for options\n", Name);
}

bool is_valid_lpe(ATermAppl spec)
{
  if (gsIsSpecV1(spec)) {
    return gsIsLPE(ATAgetArgument(spec,2));
  } else {
    return false;
  }
}

static ATermAppl rewrite_lpe(ATermAppl Spec)
{
  ATermList l;
  //rewrite data equations
  ATermAppl DataSpec = ATAgetArgument(Spec, 0);
  ATermAppl DataEqnSpec = ATAgetArgument(DataSpec, 3);
  ATermList DataEqns = ATLgetArgument(DataEqnSpec, 0);
  l = ATmakeList0();
  for (; !ATisEmpty(DataEqns); DataEqns = ATgetNext(DataEqns)) {
    //rewrite data equation
    ATermAppl DataEqn = ATAgetFirst(DataEqns);
    //rewrite condition
    ATermAppl Cond = ATAgetArgument(DataEqn, 1);
    if (!gsIsNil(Cond)) {
      Cond = rewr->rewrite(Cond);
      DataEqn = ATsetArgument(DataEqn, (ATerm) Cond, 1);
    }
    //rewrite right hand side
    ATermAppl RHS = ATAgetArgument(DataEqn, 3);
    RHS = rewr->rewrite(RHS);
    DataEqn = ATsetArgument(DataEqn, (ATerm) RHS, 3);
    l = ATinsert(l, (ATerm) DataEqn);
  }
  DataEqns = ATreverse(l);
  DataEqnSpec = ATsetArgument(DataEqnSpec, (ATerm) DataEqns, 0);
  DataSpec = ATsetArgument(DataSpec, (ATerm) DataEqnSpec, 3);
  Spec = ATsetArgument(Spec, (ATerm) DataSpec, 0);

  //rewrite LPE summands
  ATermAppl LPE = ATAgetArgument(Spec, 2);
  ATermList LPESummands = ATLgetArgument(LPE, 2);
  l = ATmakeList0();
  for (; !ATisEmpty(LPESummands); LPESummands = ATgetNext(LPESummands)) {
    //rewrite LPE summand
    ATermAppl LPESummand = ATAgetFirst(LPESummands);
    ATermList LPEVars = ATLgetArgument(LPESummand, 0);
    ATermAppl Cond = ATAgetArgument(LPESummand, 1);
    Cond = rewr->rewrite(Cond);
    ATermAppl MultAct = ATAgetArgument(LPESummand, 2);
    if ( gsIsMultAct(MultAct) ) {
      ATermList Acts = ATLgetArgument(MultAct, 0);
      ATermList m = ATmakeList0();
      for (; !ATisEmpty(Acts); Acts = ATgetNext(Acts))
      {
        ATermAppl Act = ATAgetFirst(Acts);
        ATermList Pars = ATLgetArgument(Act, 1);
        Pars = rewr->rewriteList(Pars);
        Act = ATsetArgument(Act, (ATerm) Pars, 1);
        m = ATinsert(m, (ATerm) Act);
      }
      MultAct = gsMakeMultAct(ATreverse(m));
    }
    ATermAppl Time = ATAgetArgument(LPESummand, 3);
    if ( !gsIsNil(Time) ) {
      Time = rewr->rewrite(Time);
    }
    ATermList Assignments = ATLgetArgument(LPESummand, 4);
    ATermList m = ATmakeList0();
    for (; !ATisEmpty(Assignments); Assignments = ATgetNext(Assignments)) {
      ATermAppl Assignment = ATAgetFirst(Assignments);
      ATermAppl RHS = ATAgetArgument(Assignment, 1);
      RHS = rewr->rewrite(RHS);
      Assignment = ATsetArgument(Assignment, (ATerm) RHS, 1);
      m = ATinsert(m, (ATerm) Assignment);
    }
    Assignments = ATreverse(m);
    LPESummand = gsMakeLPESummand(LPEVars, Cond, MultAct, Time, Assignments);
    l = ATinsert(l, (ATerm) LPESummand);
  }
  LPESummands = ATreverse(l);
  LPE = ATsetArgument(LPE, (ATerm) LPESummands, 2);
  Spec = ATsetArgument(Spec, (ATerm) LPE, 2);
 
  //rewrite initial state
  ATermAppl LPEInit = ATAgetArgument(Spec, 3);
  ATermList Assignments = ATLgetArgument(LPEInit, 1);
  l = ATmakeList0();
  for (; !ATisEmpty(Assignments); Assignments = ATgetNext(Assignments)) {
    ATermAppl Assignment = ATAgetFirst(Assignments);
    ATermAppl RHS = ATAgetArgument(Assignment, 1);
    RHS = rewr->rewrite(RHS);
    Assignment = ATsetArgument(Assignment, (ATerm) RHS, 1);
    l = ATinsert(l, (ATerm) Assignment);
  }
  Assignments = ATreverse(l);
  LPEInit = ATsetArgument(LPEInit, (ATerm) Assignments, 1);
  Spec = ATsetArgument(Spec, (ATerm) LPEInit, 3);
 
  return Spec;
}

/*
ATermAppl rewrite_proc(ATermAppl p)
{
  ATermList l,m;

  if ( gsIsAction(p) || gsIsProcess(p) )
  {
    return ATmakeAppl2(ATgetAFun(p),ATgetArgument(p,0),(ATerm) rewr->rewriteList(ATLgetArgument(p,1)));
  } else if ( gsIsAtTime(p) )
  {
    return ATmakeAppl2(ATgetAFun(p),ATgetArgument(p,0),(ATerm) rewr->rewrite(ATAgetArgument(p,1)));
  } else if ( gsIsSum(p) || gsIsAllow(p) || gsIsBlock(p) || gsIsHide(p) || gsIsRename(p) || gsIsComm(p) )
  {
    return ATmakeAppl2(ATgetAFun(p),ATgetArgument(p,0),(ATerm) rewrite_proc(ATAgetArgument(p,1)));
  } else if ( gsIsCond(p) )
  {
    return gsMakeCond(rewr->rewrite(ATAgetArgument(p,0)),rewrite_proc(ATAgetArgument(p,1)),rewrite_proc(ATAgetArgument(p,2)));
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
*/
