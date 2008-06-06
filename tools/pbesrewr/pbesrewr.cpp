// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesrewr.cpp
/// \brief Rewrites and simplifies the rhs's of a pbes

// ======================================================================
//
// file          : pbesrewr
// date          : 2-08-2007
// version       : 0.1.0
//
// author        : Jan Friso Groote <J.F.Groote@tue.nl>
//
// ======================================================================

#define NAME "pbesrewr"
#define AUTHOR "Jan Friso Groote"

//C++
#include <cstdio>
#include <exception>
#include <iostream>
#include <string>

//MCRL2-specific
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/utility.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/data/rewrite.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h" // after messaging.h and rewrite.h

using namespace std;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::lps;
using namespace mcrl2::pbes_system;

struct t_tool_options {
  string infile_name, outfile_name;
  RewriteStrategy rewrite_strategy;
};

// Name of the file to read input from

t_tool_options parse_command_line(int ac, char** av)
{
  interface_description clinterface(av[0], NAME, AUTHOR, "[OPTION]... [INFILE [OUTFILE]]\n",
    "Rewrite the PBES in INFILE, remove quantified variables and write the resulting PBES to OUTFILE. "
    "If INFILE is not present, stdin is used. If OUTFILE is not present, stdout is used.");

  clinterface.add_rewriting_options();

  command_line_parser parser(clinterface, ac, av);

  t_tool_options tool_options = { "-", "-", RewriteStrategyFromString(parser.option_argument("rewriter").c_str()) };

  if (2 < parser.arguments.size()) {
    parser.error("too many file arguments");
  }
  else {
    if (0 < parser.arguments.size()) {
      tool_options.infile_name = parser.arguments[0];
    }
    if (1 < parser.arguments.size()) {
      tool_options.outfile_name = parser.arguments[1];
    }
  }

  return tool_options;
}

int main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  try {
    t_tool_options tool_options = parse_command_line(argc, argv);
 
    pbes<> old_pbes_specification;
 
    /// If PBES can be loaded from file_name, then
    /// - Rewrite all data expressions
    /// - Translate all data expressions true and false to pbes expressions true and false
    /// - Simplify all and an or expressions
    /// - Remove all quantified variables that do not bind anything.

    old_pbes_specification.load(tool_options.infile_name);

    atermpp::vector<pbes_equation> old_eqsys = old_pbes_specification.equations();
    atermpp::vector<pbes_equation> new_eqsys;
  
    Rewriter *r=createRewriter(old_pbes_specification.data(),tool_options.rewrite_strategy);
    
    for (atermpp::vector<pbes_equation>::iterator fp_i = old_eqsys.begin(); fp_i != old_eqsys.end(); fp_i++)
    {
      new_eqsys.push_back(pbes_equation(
                               fp_i->symbol(),
                               fp_i->variable(),
                               pbes_expression_rewrite_and_simplify(fp_i->formula(),r)));
    }
    
    pbes<> new_pbes_specification(
             old_pbes_specification.data(),
             new_eqsys,
             old_pbes_specification.free_variables(),
             old_pbes_specification.initial_state());
    new_pbes_specification.save(tool_options.outfile_name);
    
    return EXIT_SUCCESS;
  }
  catch (mcrl2::runtime_error e)
  {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_FAILURE;
}
