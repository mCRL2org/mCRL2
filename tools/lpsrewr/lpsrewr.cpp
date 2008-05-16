// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsrewr.cpp

#define NAME "lpsrewr"
#define AUTHOR "Aad Mathijssen"

#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <aterm2.h>
#include "mcrl2/core/struct.h"
#include "mcrl2/data/rewrite.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h" // after messaging.h and rewrite.h

using namespace ::mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2;

//Functions used by the main program
static bool is_valid_lps(ATermAppl spec);
static ATermAppl rewrite_lps(ATermAppl Spec);

//Globally used rewriter
static Rewriter *rewr;

struct tool_options_type {
  bool            benchmark;
  bool            bench_times;
  std::string     infilename;
  std::string     outfilename;
  RewriteStrategy strategy;
};

tool_options_type parse_command_line(int ac, char** av) {
  interface_description clinterface(av[0], NAME, AUTHOR, "[OPTION]... [INFILE [OUTFILE]]\n"
    "Rewrite data expressions of the LPS in INFILE and save the result to OUTFILE."
    "If OUTFILE is not present, stdout is used. If INFILE is not present, stdin is"
    "used.");

  clinterface.add_rewriting_options();

  clinterface.add_option("benchmark", make_mandatory_argument("NUM"),
    "rewrite data expressions NUM times; do not save output", 'b');

  command_line_parser parser(clinterface, ac, av);

  tool_options_type options;

  options.benchmark = 0 < parser.options.count("benchmark");
  options.strategy  = parser.option_argument_as< RewriteStrategy >("rewriter");

  if (options.benchmark) {
    options.bench_times = parser.option_argument_as< unsigned long >("benchmark");
  }

  if (2 < parser.arguments.size()) {
    parser.error("too many file arguments");
  }
  else {
    if (0 < parser.arguments.size()) {
      options.infilename = parser.arguments[0];
    }
    if (1 < parser.arguments.size()) {
      options.outfilename = parser.arguments[1];
    }
  }

  return options;
}

//Main program

int main(int argc, char **argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  try {
    tool_options_type options(parse_command_line(argc, argv));

    if (options.benchmark && !options.outfilename.empty()) {
      gsWarningMsg("output will not be saved to '%s'\n", options.outfilename.c_str());
    }

    ATermAppl result;

    if (options.infilename.empty()) {
      gsVerboseMsg("reading LPS from stdin\n");

      result = (ATermAppl) ATreadFromFile(stdin);

      if (result == 0) {
        throw std::runtime_error("could not read LPS from '" + options.infilename + "'");
      }
      if (!is_valid_lps(result)) {
        throw std::runtime_error("stdin does not contain an LPS");
      }
    }
    else {
      gsVerboseMsg("reading LPS from '%s'\n", options.infilename.c_str());

      FILE *in_stream = fopen(options.infilename.c_str(), "rb");

      if (in_stream == 0) {
        throw std::runtime_error("could not open input file '" + options.infilename + "' for reading");
      }

      result = (ATermAppl) ATreadFromFile(in_stream);

      fclose(in_stream);

      if (result == 0) {
        throw std::runtime_error("could not read LPS from '" + options.infilename + "'");
      }
      if (!is_valid_lps(result)) {
        throw std::runtime_error("'" + options.infilename + "' does not contain an LPS");
      }
    }

    assert(is_valid_lps(result));

    //initialise rewriter
    if (gsVerbose) {
      fprintf(stderr, "initialising rewriter ");
      PrintRewriteStrategy(stderr, options.strategy);  
      fprintf(stderr, "...\n");
    }
    rewr = createRewriter(mcrl2::data::data_specification(ATAgetArgument(result,0)), options.strategy);

    //rewrite result
    if (options.benchmark) {
      //rewrite result options.bench_times
      gsVerboseMsg("rewriting LPS %lu times...\n", options.bench_times);
      for (unsigned long i = 0; i < options.bench_times; i++) {
        rewrite_lps(result);
      }
    } else {
      gsVerboseMsg("rewriting LPS...\n");
      result = rewrite_lps(result);
      //store the result
      if (options.outfilename.empty()) {
        gsVerboseMsg("saving result to stdout...\n");
        ATwriteToSAFFile((ATerm) result, stdout);
      } else { //outfilename != NULL
        gsVerboseMsg("saving result to '%s'...\n", options.outfilename.c_str());
        //open output filename
        FILE *outstream = fopen(options.outfilename.c_str(), "wb");
        if (outstream == NULL) {
          throw std::runtime_error("cannot open output file '" + options.outfilename + "'");
        }
        ATwriteToSAFFile((ATerm) result, outstream);
        fclose(outstream);
      }
    }
    delete rewr;
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_FAILURE;
}

bool is_valid_lps(ATermAppl spec)
{
  if (gsIsSpecV1(spec)) {
    return gsIsLinearProcess(ATAgetArgument(spec,2));
  }

  return false;
}

static ATermAppl rewrite_lps(ATermAppl Spec)
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

  //rewrite LPS summands
  ATermAppl LPS = ATAgetArgument(Spec, 2);
  ATermList LinearProcessSummands = ATLgetArgument(LPS, 2);
  l = ATmakeList0();
  for (; !ATisEmpty(LinearProcessSummands); LinearProcessSummands = ATgetNext(LinearProcessSummands)) {
    //rewrite LPS summand
    ATermAppl LinearProcessSummand = ATAgetFirst(LinearProcessSummands);
    ATermList LPSVars = ATLgetArgument(LinearProcessSummand, 0);
    ATermAppl Cond = ATAgetArgument(LinearProcessSummand, 1);
    Cond = rewr->rewrite(Cond);
    ATermAppl MultAct = ATAgetArgument(LinearProcessSummand, 2);
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
    ATermAppl Time = ATAgetArgument(LinearProcessSummand, 3);
    if ( !gsIsNil(Time) ) {
      Time = rewr->rewrite(Time);
    }
    ATermList Assignments = ATLgetArgument(LinearProcessSummand, 4);
    ATermList m = ATmakeList0();
    for (; !ATisEmpty(Assignments); Assignments = ATgetNext(Assignments)) {
      ATermAppl Assignment = ATAgetFirst(Assignments);
      ATermAppl RHS = ATAgetArgument(Assignment, 1);
      RHS = rewr->rewrite(RHS);
      Assignment = ATsetArgument(Assignment, (ATerm) RHS, 1);
      m = ATinsert(m, (ATerm) Assignment);
    }
    Assignments = ATreverse(m);
    LinearProcessSummand = gsMakeLinearProcessSummand(LPSVars, Cond, MultAct, Time, Assignments);
    l = ATinsert(l, (ATerm) LinearProcessSummand);
  }
  LinearProcessSummands = ATreverse(l);
  LPS = ATsetArgument(LPS, (ATerm) LinearProcessSummands, 2);
  Spec = ATsetArgument(Spec, (ATerm) LPS, 2);
 
  //rewrite initial state
  ATermAppl LPSInit = ATAgetArgument(Spec, 3);
  ATermList Assignments = ATLgetArgument(LPSInit, 1);
  l = ATmakeList0();
  for (; !ATisEmpty(Assignments); Assignments = ATgetNext(Assignments)) {
    ATermAppl Assignment = ATAgetFirst(Assignments);
    ATermAppl RHS = ATAgetArgument(Assignment, 1);
    RHS = rewr->rewrite(RHS);
    Assignment = ATsetArgument(Assignment, (ATerm) RHS, 1);
    l = ATinsert(l, (ATerm) Assignment);
  }
  Assignments = ATreverse(l);
  LPSInit = ATsetArgument(LPSInit, (ATerm) Assignments, 1);
  Spec = ATsetArgument(Spec, (ATerm) LPSInit, 3);
 
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
  } else if ( gsIsIfThenElse(p) )
  {
    return gsMakeIfThenElse(rewr->rewrite(ATAgetArgument(p,0)),rewrite_proc(ATAgetArgument(p,1)),rewrite_proc(ATAgetArgument(p,2)));
  } else if ( gsIsIfThen(p) )
  {
    return gsMakeIfThen(rewr->rewrite(ATAgetArgument(p,0)),rewrite_proc(ATAgetArgument(p,1))));
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

static ATermAppl rewrite_nolps(ATermAppl Spec)
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
