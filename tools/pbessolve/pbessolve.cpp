// Author(s): Simona Orzan
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./pbessolve.cpp

#include "boost.hpp" // precompiled headers

#define NAME "pbessolve" 
#define AUTHOR "Simona Orzan" 
 
//C++ 
#include <cstdio> 
#include <exception> 
#include <iostream> 
#include <fstream> 
#include <string> 
#include <utility> 
 
#include <sstream> 
 
//LPS-Framework 
#include "mcrl2/pbes/pbes.h" 
#include "mcrl2/pbes/utility.h" 

//#include "mcrl2/pbes/utility.h" 
#include "mcrl2/data/data_operators.h" 
#include "mcrl2/data/sort_expression.h" 
//#include "mcrl2/data/sort_utility.h" 
 
//ATERM-specific 
#include "mcrl2/atermpp/substitute.h" 
#include "mcrl2/core/identifier_string.h" 
#include "mcrl2/atermpp/utility.h" 
#include "mcrl2/atermpp/indexed_set.h" 
#include "mcrl2/atermpp/table.h" 
#include "mcrl2/atermpp/vector.h" 
#include "mcrl2/atermpp/set.h" 
#include "gc.h" 
 
//Tool-specific 
#include "mcrl2/pbes/gauss.h" 
 
//MCRL2-specific 
#include "mcrl2/core/messaging.h" 
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/command_line_messaging.h"
#include "mcrl2/utilities/command_line_rewriting.h"

using namespace std; 
using namespace mcrl2::utilities;
using namespace mcrl2::core; 
using namespace mcrl2::lps; 
using namespace mcrl2::pbes_system; 
 
using atermpp::make_substitution; 
 
//Type definitions ====================== 
 
// the command line options 
struct t_tool_options { 
  string infilename;
  bool interactive;
  int bound; 
  bool pnf; 
  SMT_Solver_Type solver;
  RewriteStrategy rewrite_strategy;

  //t_tool_options constructor with default values
  t_tool_options() :
    infilename(""),
    interactive(false),
    bound(0),
    pnf(false),
    solver(solver_type_cvc),
    rewrite_strategy(GS_REWR_JITTY)
  {}
};
 
 



//======================================== 
 
 
 
 
//Local functions ======================== 
static bool parse_command_line(int argc, char** argv, t_tool_options& options); 

pbes_expression interpret_solution(pbes<> pbes_spec, 
				   atermpp::vector<pbes_equation> es_solution, 
                                   SMT_Solver_Type solver,
                                   RewriteStrategy rewrite_strategy);
//======================================== 
 
 
 
 
 
 
 
//MAIN =================================== 
int main(int argc, char** argv) 
{ 
  MCRL2_ATERM_INIT(argc, argv)
   
  try {
    //Parse command line 
    t_tool_options tool_options;
   
    if (parse_command_line(argc, argv, tool_options)) { 
      //Load the pbes 
      pbes<> pbes_spec;
      pbes_spec.load(tool_options.infilename); 

      //Solve the pbes. 
      //The solution will be returned as an equation system,  
      //in order to allow partial solutions. 
      //Every equation is the result of a  
      //(possibly interactive and/or bounded)  
      //approximation process 
      pbes_solver* ps = new pbes_solver
        (pbes_spec, tool_options.solver, tool_options.rewrite_strategy,
         tool_options.bound, tool_options.pnf, tool_options.interactive);

      atermpp::vector<pbes_equation> es_solution = ps->solve(); 

      //Interpret the solution in the initial state
      pbes_expression sol_initial_state = 
        interpret_solution(pbes_spec, es_solution, 
            	       tool_options.solver, tool_options.rewrite_strategy); 

      cout << "\nPBES solution: " << pp(sol_initial_state).c_str() << "\n";
    }
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
    
  return EXIT_SUCCESS; 
} 
//======================================== 
 
 
 
 
 
 
 
 
 
//======================================== 
bool parse_command_line(int ac, char** av, t_tool_options& tool_options) 
{ 
  interface_description clinterface(av[0], NAME, AUTHOR, "[OPTION]... [INFILE]\n",
                          "Solve the PBES in INFILE, and write the result to stdout. If INFILE is not\n"
                          "present, stdin is used.");

  clinterface.add_option("interactive", "turn on the manual guidance of the approximation process", 'i')
             .add_option("bound", make_mandatory_argument("NUM"), "limit the number of approximation steps to NUM", 'b')
             .add_option("pnf", "use the prenex normal form for the approximation", 'p')
             .add_option("solver", make_mandatory_argument("SOLVER"),
               "specify the SOLVER to be used by the prover:\n"
#if defined(HAVE_CVC)
               "  'cvc' for the SMT solver CVC3 (default),\n"
               "  'cvc-fast' for the SMT solver CVC3 (fast variant), or\n"
               "  'ario' for the SMT solver Ario"
#else
               "  'cvc' for the SMT solver CVC3 (default), or\n"
               "  'ario' for the SMT solver Ario"
#endif
               , 's');

  clinterface.add_rewriting_options();

  command_line_parser parser(clinterface, ac, av);

  if (parser.continue_execution()) {
    if (parser.options.count("interactive")) {
      tool_options.interactive = true;
    }
 
    if (parser.options.count("bound")) {
      tool_options.bound = parser.option_argument_as< unsigned int >("bound"); 
    }
 
    if (parser.options.count("pnf")) {
      tool_options.pnf = true;
    }
 
    if (parser.options.count("solver")) {
      string s = parser.option_argument("solver");
      if (s == "cvc") {
        tool_options.solver = solver_type_cvc;
#if defined(HAVE_CVC)
      } else if (s == "cvc-fast") {
        tool_options.solver = solver_type_cvc_fast;
#endif
      } else if (s == "ario") {
        tool_options.solver = solver_type_ario;
      } else {
        parser.error("argument '" + s + "' to option --solver or -s is invalid");
      }
    }
 
    tool_options.rewrite_strategy = parser.option_argument_as< RewriteStrategy >("rewriter");
 
    if (0 < parser.arguments.size()) {
      tool_options.infilename = parser.arguments[0];
    }
 
    if (1 < parser.arguments.size()) {
      parser.error("too many file arguments");
    }
  }

  return parser.continue_execution(); 
}
//======================================== 
 
 
 
 
 
 
 
 
 
//======================================== 
// evaluate solution in the initial state
pbes_expression interpret_solution (pbes<> pbes_spec, 
				    atermpp::vector<pbes_equation> es_solution, SMT_Solver_Type solver, RewriteStrategy rewrite_strategy)
{ 
  propositional_variable_instantiation s = pbes_spec.initial_state();
  data_expression_list del = s.parameters();
  
  // find the solution equation for state s
  atermpp::vector<pbes_equation>::iterator e;
  for (e = es_solution.begin(); 
       ((e != es_solution.end()) && (s.name() != e->variable().name())); 
       e++);
  if (e == es_solution.end()){
    gsErrorMsg("solution for variable %s not found!",pp(s).c_str());
    exit(1);
  }
  
  // instantiate the rhs with the actual parameters given by s
  pbes_expression result;
  data_variable_list dvl = e->variable().parameters();
  pbes_expression p = 
    e->formula().substitute(make_list_substitution(dvl,del));
 
 
  BDD_Prover* prover = new BDD_Prover(pbes_spec.data(), rewrite_strategy, 0, false, solver, false);
  int nq = 0;
  data_variable_list fv;
  result = pbes_expression_simplify(p, &nq, &fv, prover);
 
  // in the resulting expression, the predicate instances should
  // be further replaced with their solutions, etc.
  // How to do this with substitute functions, without decomposing 
  // the pbes_expression???
 
  return result;
} 
//======================================== 
 
 
 
