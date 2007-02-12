// ======================================================================
//
// file          : pbes2bes
// date          : 24-01-2007
// version       : 0.0.6 - Prototype
//
// author(s)     : Alexander van Dam <avandam@damdonk.nl>
//
// ======================================================================


#define NAME "pbes2bes"
#define VERSION "0.0.6 - Prototype"
#define AUTHOR "Alexander van Dam"


//C++
#include <cstdio>
#include <exception>
#include <iostream>
#include <fstream>
#include <string>

//Boost
#include <boost/program_options.hpp>

//MCRL-specific
#include "liblowlevel.h"
#include "libprint_c.h"

//LPE-Framework
#include "lpe/pbes.h"
#include "lpe/pbes_utility.h"
#include "lpe/data_operators.h"
#include "lpe/sort.h"
//#include "lpe/sort_utility.h"

//ATERM-specific
#include "atermpp/substitute.h"
#include "atermpp/aterm_list.h"
#include "atermpp/utility.h"

//Tool-specific
#include "pbes_rewrite.h"		// PBES rewriter
#include "sort_functions.h"

using namespace std;
using namespace lpe;

using atermpp::make_substitution;

namespace po = boost::program_options;

//Type definitions
//----------------
typedef struct{
	bool opt_pretty;
	string infilename;
	string outfilename;
} t_tool_options;

typedef struct {
	pbes_equation pbeq;
	data_expression_list instantiation;
} naive_eqsys;

//Function declarations used by main program
//------------------------------------------
static t_tool_options parse_command_line(int argc, char** argv);
//Post: The command line options are parsed.
//      The program has aborted with a suitable error code, if:
//		- errors were encounterd
//		- non-standard behaviour was requested (help, version)
//Ret:	The parsed command line options

static pbes create_bes(pbes pbes_spec, t_tool_options tool_options);
//Post: tool_options.infilename contains a PBES ("-" indicates stdin)
//Ret:	The BES generated from the PBES

pbes load_pbes(t_tool_options tool_options);
//Post: tool_options.infilename contains a PBES ("-" indicates stdin)
//Ret: The pbes loaded from infile

void save_pbes(pbes pbes_spec, t_tool_options tool_options);
//Post: pbes_spec is saved
//Ret: -

sort_list get_eqsys_sorts(pbes pbes_spec);
//Post: The finiteness of the sorts are checked
//Ret: True if all sorts of the equation_system are finite, false otherwise


pbes do_naive_algorithm(pbes pbes_spec, t_tool_options tool_options);

data_expression_list get_finite_parameters(function_list fl, data_expression_list parameters);


//Main Program
//------------
int main(int argc, char** argv)
{
	//Initialise ATerm library and lowlevel-functions
	ATerm bottom;
	ATinit(argc, argv, &bottom);
	gsEnableConstructorFunctions();

	//Parse command line
	t_tool_options tool_options = parse_command_line(argc, argv);

	//Load PBES
	pbes pbes_spec = load_pbes(tool_options);

	//Process the pbes
	pbes_spec = create_bes(pbes_spec, tool_options);

	//Save PBES
	save_pbes(pbes_spec, tool_options);

	return 0;
}

//function create_bes
//-------------------
pbes create_bes(pbes pbes_spec, t_tool_options tool_options)
{
	//Do naive algorithm. In later stage a check for naive or improved is added.
	pbes_spec = do_naive_algorithm(pbes_spec, tool_options);

	//return new pbes
	return pbes_spec;
}

pbes do_naive_algorithm(pbes pbes_spec, t_tool_options tool_options)
{
	
	// Get all sorts of the lhs of the equation of the pbes
	gsVerboseMsg("Checking if all sorts are finite\n");
	sort_list eqsys_sorts = get_eqsys_sorts(pbes_spec);
	bool finite = check_finite_list(pbes_spec.data().constructors(), eqsys_sorts);

	// If not all sorts are finite throw error
	// This function will be deleted when the algorithm will leave infinite sorts as they are.
	if (!finite)
	{
		cerr << "One or more of the data sorts of the equation system are infinite." << endl;
		cerr << "Computation of all possible states will therefore be an infinite computation." << endl;
		cerr << "pbes2bes aborted." << endl;
		exit(1);
	}

	// Initial state to store
	propositional_variable_instantiation initial_state = pbes_spec.initial_state();

	// Struct used to store the equation system with the parameters it stands for.
	naive_eqsys eqsys_naive;
	vector< naive_eqsys > eqsys_naive_list;
	data_expression_list empty_data_expression_list;
	data_variable_list empty_data_variable_list;
	// Create a list of structs which contains all equations of the naive generated bes
	for (equation_system::iterator eq = pbes_spec.equations().begin(); eq != pbes_spec.equations().end(); eq++)
	{
		// List which holds all computed equations for eq.
		vector< naive_eqsys > eq_naive_list;
		eqsys_naive.instantiation = empty_data_expression_list;
		eqsys_naive.pbeq = *eq;
		eq_naive_list.push_back(eqsys_naive);
		for (data_variable_list::iterator p = eq->variable().parameters().begin(); p != eq->variable().parameters().end(); p++)
		{
			vector< naive_eqsys > eq_naive_currentp_list;
			for (vector< naive_eqsys >::iterator cur = eq_naive_list.begin(); cur != eq_naive_list.end(); cur++)
			{
				data_expression_list instantiation = cur->instantiation;
				data_expression_list enumerations = enumerate_constructors(pbes_spec.data().constructors(), p->sort());
				for (data_expression_list::iterator e = enumerations.begin(); e != enumerations.end(); e++)
				{
					pbes_expression rhs = cur->pbeq.formula().substitute(make_substitution(*p, *e));
					eqsys_naive.pbeq = pbes_equation(cur->pbeq.symbol(), cur->pbeq.variable(), rhs);
					eqsys_naive.instantiation = push_back(instantiation, *e);
					eq_naive_currentp_list.push_back(eqsys_naive);
				}
			}
			eq_naive_list = eq_naive_currentp_list;
		}
		eqsys_naive_list.insert(eqsys_naive_list.end(), eq_naive_list.begin(), eq_naive_list.end());
	} // eqsys_naive_list now contains all possible equations with the appropriate instantiations
	
	gsVerboseMsg("Total number of equations: %d\n", eqsys_naive_list.size());
	// Now create the pbes out of it.
	// eqsys_naive_list contains all needed information
	
	fresh_propositional_variable_generator generator;
	for (vector< naive_eqsys>::iterator i = eqsys_naive_list.begin(); i != eqsys_naive_list.end(); i++)
	{
		generator.add_to_context(i->pbeq);
	}
	
	// Create:
	// - Equation system for pbes
	// - 2 lists for replacing all the stuff.
	equation_system result_eqsys;
	propositional_variable_instantiation_list oldpropvarlist;
	propositional_variable_instantiation_list newpropvarlist;

	int count = 1;
	Rewriter *rewriter = createRewriter(pbes_spec.data());
	for (vector< naive_eqsys>::iterator i = eqsys_naive_list.begin(); i != eqsys_naive_list.end(); i++)
	{
		gsVerboseMsg("At equation %d\n", count);
		count++;
		pbes_equation cur_pbeq = i->pbeq;
		pbes_expression rhs = pbes_expression_rewrite(cur_pbeq.formula(), pbes_spec.data(), rewriter);
		data_expression_list instantiation = i->instantiation;
		propositional_variable_instantiation old_propvar = propositional_variable_instantiation(i->pbeq.variable().name(), i->instantiation);
		oldpropvarlist = push_front(oldpropvarlist, old_propvar);
		rhs = pbes_expression_rewrite(rhs, pbes_spec.data(), rewriter) ;
		data_expression_list inst;
		data_variable_list vars;

		propositional_variable new_propvar = generator( propositional_variable(i->pbeq.variable().name(), empty_data_variable_list) );
		propositional_variable_instantiation new_propvarinst = propositional_variable_instantiation(new_propvar.name(), empty_data_expression_list);
		newpropvarlist = push_front(newpropvarlist, new_propvarinst);

		if (initial_state == old_propvar)
			initial_state = new_propvarinst;
		cur_pbeq = pbes_equation(cur_pbeq.symbol(), new_propvar, rhs);

		result_eqsys.push_back(cur_pbeq);
	}
	delete rewriter;
	equation_system result;
	for (equation_system::iterator i = result_eqsys.begin(); i != result_eqsys.end(); i++)
	{
		result.push_back(pbes_equation(i->symbol(), i->variable(), i->formula().substitute(make_list_substitution(oldpropvarlist, newpropvarlist))));
	}
//	result_eqsys = result_eqsys.substitute(make_list_substitution(oldpropvarlist, newpropvarlist));

	pbes_spec = pbes(pbes_spec.data(), result, initial_state);

	return pbes_spec;
}

data_expression_list get_finite_parameters(function_list fl, data_expression_list parameters)
{
	data_expression_list result;
	for (data_expression_list::iterator i = parameters.begin(); i != parameters.end(); i++)
	{
		if (check_finite(fl, i->sort()))
			result = push_back(result, *i);
	}
	return result;
}



//function get_eqsys_sorts
//------------------------
sort_list get_eqsys_sorts(pbes pbes_spec)
{
	sort_list eqsys_sorts;
	set<lpe::sort> sorts;
	for (equation_system::iterator i = pbes_spec.equations().begin(); i != pbes_spec.equations().end(); i++)
	{
		for (data_variable_list::iterator j = i->variable().parameters().begin(); j != i->variable().parameters().end(); j++)
		{
			sorts.insert(j->sort());
		}
	}

	for (set<lpe::sort>::iterator i = sorts.begin(); i != sorts.end(); i++)
	{
		eqsys_sorts = push_front(eqsys_sorts, *i);
	}
	eqsys_sorts = reverse(eqsys_sorts);

	return eqsys_sorts;
}

//function load_pbes
//------------------
pbes load_pbes(t_tool_options tool_options)
{
	string infilename = tool_options.infilename;

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

//function save_pbes
//------------------
void save_pbes(pbes pbes_spec, t_tool_options tool_options)
{
	//Outfile string
	string outfilename = tool_options.outfilename;

	//Write PBES
	if (tool_options.opt_pretty)
	{
		if (!pbes_spec.save(outfilename, false))
		{
			gsErrorMsg("Could not save PBES to %s\n", outfilename.c_str());
			exit(1);
		}
	}
	else
	{
		if (!pbes_spec.save(outfilename, true))
		{
			gsErrorMsg("Could not save PBES to %s\n", outfilename.c_str());
			exit(1);
		}
	}
}

//function parse_command_line
//---------------------------
t_tool_options parse_command_line(int argc, char** argv)
{
	t_tool_options tool_options;
	bool opt_pretty = false;
	vector< string > file_names;

	po::options_description desc;

	desc.add_options()
			("external,e",	"return the result in external format")
			("verbose,v",	"turn on the display of short intermediate messages")
			("debug,d",		"turn on the display of detailed intermediate messages")
			("version",		"display version information")
			("help,h",		"display this help")
			;

	po::options_description hidden("Hidden options");
	hidden.add_options()
			("file_names",		po::value< vector< string > >(), "input/output files")
			;

	po::options_description cmdline_options;
	cmdline_options.add(desc).add(hidden);

	po::options_description visible("Allowed options");
	visible.add(desc);

	po::positional_options_description p;
	p.add("file_names", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		cerr << "Usage: " << argv[0] << " [OPTION]... [INFILE [OUTFILE]]" << endl;
		cerr << "Rewrites PBES from stdin or INFILE to an equivalent BES." << endl;
		cerr << "By default the result is written to stdout, otherwise to OUTFILE." << endl;
		cerr << endl;
		cerr << desc;

		exit(0);
	}

	if (vm.count("version"))
	{
		cerr << NAME << " " << VERSION <<  " (revision " << REVISION << ")" << endl;

		exit(0);
	}

	if (vm.count("debug"))
	{
		gsSetDebugMsg();
	}

	if (vm.count("verbose"))
	{
		gsSetVerboseMsg();
	}

	if (vm.count("external")) // Pretty printed version
	{
		opt_pretty = true;
	}

	if (vm.count("file_names"))
	{
		file_names = vm["file_names"].as< vector< string > >();
	}

	// Check for number of arguments and infilename/outfile
	string infilename;
	string outfilename;
	if (file_names.size() == 0)
	{
		/* Read from standard input */
		infilename = "-";
		outfilename = "-";
	}
	else if (2 < file_names.size())
	{
		cerr << NAME <<": Too many arguments" << endl;
		exit(1);
	}
	else
	{
		infilename = file_names[0];
		if (file_names.size() == 2)
		{
			outfilename = file_names[1];
		}
		else
		{
			outfilename = "-";
		}
	}
	tool_options.opt_pretty = opt_pretty;
	tool_options.infilename = infilename;
	tool_options.outfilename = outfilename;
	return tool_options;
}

