#ifndef MCRL2_PBES2BOOL_H
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#define MCRL2_PBES2BOOL_H

#include <string>
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/data/rewrite.h"

enum transformation_strategy {
     lazy,          // generate equations but do not optimize on the fly
     optimize,      // optimize by substituting true and false for already
                    // investigated variables in a rhs, while generating this rhs.
     on_the_fly,    // make a distinction between variables that occur somewhere,
                    // and variables that do not occur somewhere. When generating
                    // a rhs, optimize this rhs as in "optimize". If the rhs is
                    // equal to T or F, substitute this value throughout the
                    // equation system, and maintain which variables become unused
                    // by doing so, as these do not have to be investigated further.
                    // E.g. if a rhs is  X1 && X2, X1 does not occur elsewhere and
                    // X2 turns out to be equal to false, then X1 is moved to the
                    // set of irrelevant variables, and not investigated further.
     on_the_fly_with_fixed_points
                    // Do the same as with on the fly, but for each generated variable
                    // in the rhs, investigate whether this variable lies on a loop
                    // such that depending on its fixed point, it can be set to true
                    // or false. Due to the breadth first nature of the main algorithm
                    // the existence of such loops must be investigated separately
                    // for each variable, which can take a lot of time.
};



struct t_tool_options
{
   std::string opt_outputformat;
//   std::string opt_strategy;
   transformation_strategy opt_strategy;
   bool opt_precompile_pbes;
   RewriteStrategy rewrite_strategy;
   bool opt_use_hashtables;
   bool opt_construct_counter_example;
   bool opt_store_as_tree;
   bool opt_data_elm;
   std::string infilename;
   std::string outfilename;
   std::string opt_counter_example_file;

   t_tool_options()
     : opt_outputformat("none"),
       opt_strategy(lazy),
       opt_precompile_pbes(false),
       rewrite_strategy(GS_REWR_JITTY),
       opt_use_hashtables(false),
       opt_construct_counter_example(false),
       opt_store_as_tree(false),
       opt_data_elm(true)
   {}
};

/// Solve a pbes
void process(t_tool_options const& tool_options);

/// Solve a pbes.
// TODO: currently the pbes2bool algorithm expects input from a file, and
// writes the result to a file. That should be changed.
inline
bool pbes2bool(const mcrl2::pbes_system::pbes<>& p, t_tool_options options = t_tool_options())
{
  mcrl2::pbes_system::pbes<> q = p;
  options.infilename = "pbes2bool.in";
  options.outfilename = "pbes2bool.out";
  q.save(options.infilename);
  process(options);
  return true;
}

#endif // MCRL2_PBES2BOOL_H
