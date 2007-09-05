//  Copyright 2007 Simona Orzan. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./pbessolve.cpp

 
#define NAME "pbessolve" 
#define VERSION "0.0.1" 
 
 
 
//C++ 
#include <cstdio> 
#include <exception> 
#include <iostream> 
#include <fstream> 
#include <string> 
#include <utility> 
 
#include <sstream> 
 
//Boost 
#include <boost/program_options.hpp> 
 
//MCRL-specific 
#include "print/messaging.h" 

//LPS-Framework 
#include "mcrl2/pbes/pbes.h" 

//#include "mcrl2/pbes/utility.h" 
#include "mcrl2/data/data_operators.h" 
#include "mcrl2/data/sort.h" 
//#include "mcrl2/data/sort_utility.h" 
 
//ATERM-specific 
#include "atermpp/substitute.h" 
#include "mcrl2/basic/identifier_string.h" 
#include "atermpp/utility.h" 
#include "atermpp/indexed_set.h" 
#include "atermpp/table.h" 
#include "atermpp/vector.h" 
#include "atermpp/set.h" 
#include "gc.h" 
 
//Tool-specific 
#include "gauss.h" 
 
using namespace std; 
using namespace lps; 
using namespace mcrl2::utilities; 
 
using atermpp::make_substitution; 
 
using namespace ::mcrl2::utilities;

namespace po = boost::program_options; 
 
//Type definitions ====================== 
 
// the command line options 
typedef struct{ 
  bool interactive; 
  int bound; 
} t_tool_options; 
 
 



//======================================== 
 
 
string infilename; 
 
 
//Local functions ======================== 
static t_tool_options parse_command_line(int argc, char** argv); 
pbes load_pbes();
pbes_expression interpret_solution(pbes pbes_spec, 
				   equation_system es_solution); 
//======================================== 
 
 
 
 
 
 
 
//MAIN =================================== 
int main(int argc, char** argv) 
{ 
  //Initialise ATerm library and lowlevel-functions 
  ATerm bottom; 
  ATinit(argc, argv, &bottom); 
  gsEnableConstructorFunctions(); 
   
  //Parse command line 
  t_tool_options tool_options = parse_command_line(argc, argv); 
   
  //Load the pbes 
  pbes pbes_spec = load_pbes(); 
   
  //Solve the pbes. 
  //The solution will be returned as an equation system,  
  //in order to allow partial solutions. 
  //Every equation is the result of a  
  //(possibly interactive and/or bounded)  
  //approximation process 
  equation_system es_solution = 
    solve_pbes(pbes_spec, tool_options.interactive, 
	       tool_options.bound); 
   
  //Interpret the solution in the initial state
  pbes_expression sol_initial_state = 
    interpret_solution(pbes_spec, es_solution); 
   
  cout << "PBES solution: " << pp(sol_initial_state).c_str() << "\n";

  return 0; 
} 
//======================================== 
 
 
 
 
//========================================= 
// Loads a PBES from a file. 
// (function copied from pbes2bes)
pbes load_pbes() 
{ 
  pbes pbes_spec; 
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
t_tool_options parse_command_line(int argc, char** argv) 
{ 
  t_tool_options tool_options; 
  int opt_bound = 0; 
 
  tool_options.interactive = false; 
   
  po::options_description desc; 
  desc.add_options() 
    ("interactive,i","turn on the manual guidance of the approximation process") 
    ("bound,b",po::value<int>(&opt_bound)->default_value(0), "limit the number of approximation steps\nExample: -b 10\n")
    ("verbose,v",	"turn on the display of short intermediate messages") 
    ("debug,d",		"turn on the display of detailed intermediate messages") 
    ("version",		"display version information") 
    ("help,h",		"display this help") 
    ; 
   
  po::options_description hidden("Hidden options"); 
  hidden.add_options() 
    ("INFILE",              po::value<string>(), "input file") 
    ; 
   
  //  po::options_description visible("Allowed options"); 
  //  visible.add(desc); 
   
  po::options_description cmdline_options; 
  cmdline_options.add(desc).add(hidden);  
   
  po::positional_options_description p; 
  p.add("INFILE", -1); 
   
  po::variables_map vm; 
  po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm); 
  po::notify(vm); 
   
  if (vm.count("help")) { 
    cerr << "Usage: " << argv[0] << " [OPTION]... [INFILE]" << endl; 
    cerr << "Solves the PBES from stdin or INFILE." << endl; 
    cerr << "The result is written to stdout." << endl; 
    cerr << endl; 
    cerr << desc; 
    exit(0); 
  } 
   
  if (vm.count("version")) { 
    cerr << NAME << " " << VERSION  << endl; 
    exit(0); 
  } 
   
  if (vm.count("debug")) 
    gsSetDebugMsg(); 
   
  if (vm.count("verbose")) 
    gsSetVerboseMsg(); 
   
  if (vm.count("interactive")) 
    tool_options.interactive = true;  
   
  if (vm.count("bound")) 
    opt_bound = vm["bound"].as<int>(); 
   
  infilename = (0 < vm.count("INFILE")) ? vm["INFILE"].as<string>() : "-"; 
  tool_options.bound = opt_bound; 
  return tool_options; 
} 
//======================================== 
 
 
 
 
 
 
 
 
 
//======================================== 
// evaluate solution in the initial state
pbes_expression interpret_solution (pbes pbes_spec, 
				    equation_system es_solution) 
{ 

  propositional_variable_instantiation s = pbes_spec.initial_state().variable();
  data_expression_list del = s.parameters();
  
  // find the solution equation for state s
  equation_system::iterator e;
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
  
  SMT_Solver_Type sol = solver_type_cvc_lite_fast;
  BDD_Prover* prover = new BDD_Prover(pbes_spec.data(), GS_REWR_INNER, 0, false, sol, false);
  result = rewrite_pbes_expression(p, prover);


  // in the resulting expression, the predicate instances should
  // be further replaced with their solutions, etc.
  // How to do this with substitute functions, without decomposing 
  // the pbes_expression???

  return result;
} 
//======================================== 
 
 
 
