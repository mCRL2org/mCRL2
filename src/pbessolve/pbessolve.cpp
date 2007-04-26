// ======================================================================
//
// file          : pbessolve
// date          : 19-04-2007
// version       : 0.0.1
//
// author(s)     : Simona Orzan
//
// ======================================================================


#define NAME "pbessolve"
#define VERSION "0.0.1"
#define AUTHOR "Simona Orzan"


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
#include "liblowlevel.h"
#include "libprint_c.h"

//LPS-Framework
#include "lps/pbes.h"
#include "lps/pbes_utility.h"
#include "lps/data_operators.h"
#include "lps/sort.h"
//#include "lps/sort_utility.h"

//ATERM-specific
#include "atermpp/substitute.h"
#include "lps/identifier_string.h"
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

using atermpp::make_substitution;

namespace po = boost::program_options;

//Type definitions ======================

// the command line options
typedef struct{
 bool interactive;
 int bound;
} t_tool_options;


// the possible answers of PBESsolve
typedef enum{TRUE, FALSE, UNDECIDED} t_result;

//========================================


string infilename;


//Local functions ========================
static t_tool_options parse_command_line(int argc, char** argv);
pbes load_pbes();
t_result interpret_solution(pbes pbes_spec, equation_system es_solution);
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
  equation_system es_solution = solve_pbes(pbes_spec, tool_options.interactive, tool_options.bound);
  
  //Interpret the solution
  t_result result = interpret_solution(pbes_spec, es_solution);
  
  //Print result
  
  return 0;
}
//========================================





//copied from pbes2bes ===================
pbes load_pbes()
{
  pbes pbes_spec;
  if (infilename == "-")
    {
      if (!pbes_spec.load("-"))
	{
	  gsErrorMsg("Cannot open PBES from stdin\n");
	  exit(1);
	}
    }
  else
    {
      if (!pbes_spec.load(infilename))
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
    cerr << "Usage: " << argv[0] << " [OPTION]... [INFILE [OUTFILE]]" << endl;
    cerr << "Solves the PBES from stdin or INFILE." << endl;
    cerr << "By default the result is written to stdout, otherwise to OUTFILE." << endl;
    cerr << endl;
    cerr << desc;
    exit(0);
  }
  
  if (vm.count("version")) {
    cerr << NAME << " " << VERSION <<  " (revision " << REVISION << ")" << endl;
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
t_result interpret_solution (pbes pbes_spec, equation_system es_solution)
{
  // To implement.
  return UNDECIDED;
}
//========================================



