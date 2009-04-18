Deprecated should not be used anymore.



// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes2bool.h
/// \brief Contains includable parts of pbes2bool.cpp


#ifndef MCRL2_PBES2BOOL_H
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#define MCRL2_PBES2BOOL_H

//C++
#include <ostream>
#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <utility>

//Tool-specific
#include "bes_deprecated.h"

//PBES
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/new_data/rewriter.h"

//LPS-Framework
#include "mcrl2/data/data_operators.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/pbes/data_elimination.h"

//ATERM-specific
#include "mcrl2/atermpp/substitute.h"
#include "mcrl2/atermpp/utility.h"
#include "mcrl2/atermpp/indexed_set.h"
#include "mcrl2/atermpp/table.h"
#include "mcrl2/atermpp/map.h"
#include "_aterm.h"

//MCRL-specific
#include "mcrl2/core/messaging.h"

/// \brief Strategies for the pbes2bool algorithm.
/* enum transformation_strategy {
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
}; */

/// \brief Options for the pbes2bool algorithm.
struct t_tool_options
{
   /// \brief The output format
   std::string opt_outputformat;

   /// \brief The strategy
   bes::transformation_strategy opt_strategy;

   // \brief The precompile option Deprecated.
   // bool opt_precompile_pbes;

   /// \brief The rewrite strategy
<<<<<<< .mine
   // RewriteStrategy rewrite_strategy;
=======
   mcrl2::new_data::rewriter::strategy rewrite_strategy;
>>>>>>> .r5969

   /// \brief The hashtable option
   bool opt_use_hashtables;

   /// \brief The counter example option
   bool opt_construct_counter_example;

   /// \brief The tree storage option
   bool opt_store_as_tree;

   /// \brief The data elimination option
   bool opt_data_elm;

   /// \brief The input file name
   // std::string infilename;

   /// \brief The output file name
   std::string outfilename;

   /// \brief The counter example file name
   std::string opt_counter_example_file;

   /// \brief Constructor
   t_tool_options()
     : opt_outputformat("none"),
<<<<<<< .mine
       opt_strategy(bes::lazy),
       // opt_precompile_pbes(false),
       // rewrite_strategy(GS_REWR_JITTY),
=======
       opt_strategy(lazy),
       opt_precompile_pbes(false),
       rewrite_strategy(mcrl2::new_data::rewriter::jitty),
>>>>>>> .r5969
       opt_use_hashtables(false),
       opt_construct_counter_example(false),
       opt_store_as_tree(false),
       opt_data_elm(true),
       outfilename(""),
       opt_counter_example_file("")
   {}
};


bool solve_bes(const t_tool_options &,
                      bes::boolean_equation_system & /* , atermpp::indexed_set &*/);

/// Solve a pbes
/* void process(t_tool_options const& tool_options); */

/// Algorithm for solving a pbes, using instantiation.
/// \param pbes_spec A pbes which does not contain global or free variables. 
///          It is assumed that the spec does not contain negations and implications.
///          This can be achieved, using the normalize() method.
/// \param options Options for the algorithm
/// \return The solution of the pbes

template <typename Container, typename PbesRewriter>
bool pbes2bool(const mcrl2::pbes_system::pbes<Container>& pbes_spec,
               PbesRewriter pbesr,
               t_tool_options tool_options = t_tool_options())
{
  //Load PBES
  /* pbes<> pbes_spec;
     pbes_spec.load(tool_options.infilename); */

  if (!pbes_spec.is_closed())
  { throw mcrl2::runtime_error("The pbes contains free pbes variables \n");
  }

  //Process the pbes

  if (tool_options.opt_use_hashtables)
  { bes::use_hashtables();
  }
  // atermpp::indexed_set variable_index(10000, 50);
  bes::boolean_equation_system bes_equations(pbes_spec,
                         pbesr,
                         tool_options.opt_strategy,
                         tool_options.opt_store_as_tree,
                         tool_options.opt_construct_counter_example,
                         tool_options.opt_use_hashtables);

  // data_specification data = pbes_spec.data();
  // Rewriter *rewriter = createRewriter(data,tool_options.rewrite_strategy);
  // assert(rewriter != 0);

  // calculate_bes(pbes_spec, tool_options,bes_equations,variable_index,pbesr);
  /* if (!tool_options.opt_construct_counter_example)
  { variable_index.reset();
  } */

  if (tool_options.opt_outputformat == "cwi")
  { // in CWI format only if the result is a BES, otherwise Binary
    save_bes_in_cwi_format(tool_options.outfilename,bes_equations);
    return true;
  }
  if (tool_options.opt_outputformat == "vasy")
  { //Save resulting bes if necessary.
    save_bes_in_vasy_format(tool_options.outfilename,bes_equations);
    return true;
  }
  if (tool_options.opt_outputformat == "pbes")
  { //Save resulting bes if necessary.
    save_bes_in_pbes_format(tool_options.outfilename,bes_equations,pbes_spec);
    return true;
  }
  
  bool result=solve_bes(tool_options,bes_equations);

  if (tool_options.opt_construct_counter_example)
  { bes_equations.print_counter_example(
                            tool_options.opt_store_as_tree,
                            tool_options.opt_counter_example_file);
  }
  return result;
  // delete rewriter;
}
/* {
  mcrl2::pbes_system::pbes<> q = p;
  // options.infilename = "pbes2bool.in";
  // options.outfilename = "pbes2bool.out";
  q.save(options.infilename);
  process(options);
  return true;
} */

#endif // MCRL2_PBES2BOOL_H
