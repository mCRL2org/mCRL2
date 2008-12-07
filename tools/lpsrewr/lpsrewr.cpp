// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsrewr.cpp

#include "boost.hpp" // precompiled headers

#define NAME "lpsrewr"
#define AUTHOR "Aad Mathijssen"

#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <aterm2.h>
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/detail/aterm_io.h"
#include "mcrl2/data/rewrite.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/command_line_messaging.h"
#include "mcrl2/utilities/command_line_rewriting.h"

using namespace mcrl2::utilities;
using namespace mcrl2::core;

//Functions used by the main program
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
  interface_description clinterface(av[0], NAME, AUTHOR, "[OPTION]... [INFILE [OUTFILE]]\n",
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
  if (0 < parser.arguments.size()) {
    options.infilename = parser.arguments[0];
  }
  if (1 < parser.arguments.size()) {
    options.outfilename = parser.arguments[1];
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

    std::string str_in = (options.infilename.empty())?"stdin":("'" + options.infilename+ "'");
    gsVerboseMsg("reading LPS from %s\n", str_in.c_str());
    ATermAppl result = (ATermAppl) mcrl2::core::detail::load_aterm(options.infilename);
    if (!mcrl2::core::detail::gsIsLinProcSpec(result)) {
      throw mcrl2::runtime_error(str_in + " does not contain an LPS");
    }

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
          throw mcrl2::runtime_error("cannot open output file '" + options.outfilename + "'");
        }
        ATwriteToSAFFile((ATerm) result, outstream);
        fclose(outstream);
      }
    }
    delete rewr;
    return EXIT_SUCCESS;
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
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
    if (!mcrl2::core::detail::gsIsNil(Cond)) {
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
    if ( mcrl2::core::detail::gsIsMultAct(MultAct) ) {
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
      MultAct = mcrl2::core::detail::gsMakeMultAct(ATreverse(m));
    }
    ATermAppl Time = ATAgetArgument(LinearProcessSummand, 3);
    if ( !mcrl2::core::detail::gsIsNil(Time) ) {
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
    LinearProcessSummand = mcrl2::core::detail::gsMakeLinearProcessSummand(LPSVars, Cond, MultAct, Time, Assignments);
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
