//  Copyright 2007 Simona Orzan. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./pbessolve.cpp

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
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h" // must come after mcrl2/core/messaging.h

using namespace std; 
using namespace mcrl2::utilities;
using namespace mcrl2::core; 
using namespace mcrl2::lps; 
using namespace mcrl2::pbes_system; 
 
using atermpp::make_substitution; 
 
using namespace ::mcrl2::utilities;

//Type definitions ====================== 
 
// the command line options 
typedef struct{ 
  bool interactive;
  bool pnf; 
  int bound; 
  string solver;
  string rewriter;
} t_tool_options; 
 
 



//======================================== 
 
 
string infilename = "-"; 
 
 
//Local functions ======================== 
static t_tool_options parse_command_line(int argc, char** argv); 
pbes<> load_pbes();

pbes_expression interpret_solution(pbes<> pbes_spec, 
				   atermpp::vector<pbes_equation> es_solution, 
				   string solver, string rewriter); 
//======================================== 
 
 
 
 
 
 
 
//MAIN =================================== 
int main(int argc, char** argv) 
{ 
  MCRL2_ATERM_INIT(argc, argv)
   
  try {
    //Parse command line 
    t_tool_options tool_options = parse_command_line(argc, argv); 
     
    //Load the pbes 
    pbes<> pbes_spec = load_pbes(); 
     
    //Solve the pbes. 
    //The solution will be returned as an equation system,  
    //in order to allow partial solutions. 
    //Every equation is the result of a  
    //(possibly interactive and/or bounded)  
    //approximation process 
    pbes_solver* ps = new pbes_solver
      (pbes_spec, tool_options.solver, tool_options.rewriter,
       tool_options.bound, tool_options.pnf, tool_options.interactive);
    
    atermpp::vector<pbes_equation> es_solution = ps->solve(); 
     
    //Interpret the solution in the initial state
    pbes_expression sol_initial_state = 
      interpret_solution(pbes_spec, es_solution, 
          	       tool_options.solver, tool_options.rewriter); 
     
    cout << "\nPBES solution: " << pp(sol_initial_state).c_str() << "\n";
    
    return EXIT_SUCCESS; 
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_FAILURE;
} 
//======================================== 
 
 
 
 
//========================================= 
// Loads a PBES from a file. 
// (function copied from pbes2bes)
pbes<> load_pbes() 
{ 
  pbes<> pbes_spec; 
  if (infilename == "-") 
  {
    try
    {
      pbes_spec.load("-");
    }
    catch (std::runtime_error e)   
    { 
	    gsErrorMsg("Cannot open PBES from stdin\n"); 
	    exit(1); 
	  } 
  } 
  else 
  { 
    try
    {
      pbes_spec.load(infilename);
  }
    catch (std::runtime_error e)
    { 
      gsErrorMsg("Cannot open PBES from '%s'\n", infilename.c_str()); 
      exit(1); 
    } 
  } 
  return pbes_spec; 
} 
//======================================== 
 
 
 
 
 
//======================================== 
t_tool_options parse_command_line(int ac, char** av) 
{ 
  interface_description clinterface(av[0], NAME, AUTHOR, "[OPTION]... [INFILE]\n"
                          "Solve the PBES in INFILE, and write the result to stdout. If INFILE is not\n"
                          "present, stdin is used.");

  clinterface.add_option("interactive", "turn on the manual guidance of the approximation process", 'i')
             .add_option("bound", "limit the number of approximation steps\nExample: -b 10\n", 'b')
             .add_option("pnf", "use the prenex normal form for the approximation", 'p')
             .add_option("solver", make_optional_argument("SOLVER", "cvc"), "specify the solver to be used by the prover\n  Options are: ario, cvc, fast", 's');

  clinterface.add_rewriting_options();

  command_line_parser parser(clinterface, ac, av);

  t_tool_options tool_options; 

  tool_options.bound       = 0; 
  tool_options.solver      = "cvc";
  tool_options.pnf         = (0 < parser.options.count("pnf"));  
  tool_options.interactive = (0 < parser.options.count("interactive"));  
  tool_options.rewriter    = parser.option_argument("rewriter").c_str();

  if (parser.options.count("bound")) {
    tool_options.bound = parser.option_argument_as< int >("bound"); 
  }
  if (parser.options.count("solver")) {
    tool_options.solver = parser.option_argument("solver");
  }
  if (0 < parser.arguments.size()) {
    infilename = parser.arguments[0];
  }
  if (1 < parser.arguments.size()) {
    parser.error("too many file arguments");
  }

  return tool_options; 
} 
//======================================== 
 
 
 
 
 
 
 
 
 
//======================================== 
// evaluate solution in the initial state
pbes_expression interpret_solution (pbes<> pbes_spec, 
				    atermpp::vector<pbes_equation> es_solution, string solver, string rewriter) 
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
 
 
  SMT_Solver_Type sol = (solver == "ario") ? solver_type_ario: ((solver=="fast")?solver_type_cvc_fast:solver_type_cvc);
  RewriteStrategy rew = (rewriter == "inner") ? GS_REWR_INNER:
    ((rewriter == "innerc") ? GS_REWR_INNERC : 
     ((rewriter == "jitty") ? GS_REWR_JITTY : GS_REWR_JITTYC));
  BDD_Prover* prover = new BDD_Prover(pbes_spec.data(), rew, 0, false, sol, false);
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
 
 
 
