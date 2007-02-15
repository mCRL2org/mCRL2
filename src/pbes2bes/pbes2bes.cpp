// ======================================================================
//
// file          : pbes2bes
// date          : 14-02-2007
// version       : 0.0.7 - Prototype
//
// author(s)     : Alexander van Dam <avandam@damdonk.nl>
//
// ======================================================================


#define NAME "pbes2bes"
#define VERSION "0.0.7 - Prototype"
#define AUTHOR "Alexander van Dam"


//C++
#include <cstdio>
#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>

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
#include "atermpp/table.h"

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
	data_variable_list original;		// Complete instantiation of a PBE
	data_variable_list original_finite;	// List of all original finite sorts
	data_expression_list finite_inst;	// List of values of enumerated finite data sorts
	data_variable_list infinite_inst;	// List of all infinite data sorts
} t_instantiations;

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

pbes do_naive_algorithm(pbes pbes_spec, t_tool_options tool_options);

propositional_variable_instantiation create_propositional_variable_instantiation(propositional_variable_instantiation propvar, bool change_propvar, map<lpe::sort, bool> sort_list_is_finite);

propositional_variable create_propositional_variable(propositional_variable propvar, data_expression_list finite_inst, bool change_propvar, map<lpe::sort, bool> sort_list_is_finite);

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
	pbes result;

	bool change_propvarinst_initial_state;

	bool is_bes_system = true;		// True if the resulting equation system is a BES, false otherwise
	//int nr_of_equations = 0;		// Number of equations computed
	propositional_variable_instantiation initial_state = pbes_spec.initial_state();
	equation_system eqsys = pbes_spec.equations();
	data_specification data = pbes_spec.data();

	// map which contains if a data sort is finite
	map<lpe::sort, bool> sort_list_is_finite;
	for (equation_system::iterator eq_i = eqsys.begin(); eq_i != eqsys.end(); eq_i++)
	{
		for (data_variable_list::iterator p = eq_i->variable().parameters().begin(); p != eq_i->variable().parameters().end(); p++)
		{
			lpe::sort cur_sort = p->sort();
			if (sort_list_is_finite.find(cur_sort) == sort_list_is_finite.end())
			{ // If the data sort isn't in the map yet, add it.
				bool finite;
				check_finite(data.constructors(), cur_sort)?finite = true:finite = false;
				sort_list_is_finite.insert(pair<lpe::sort,bool>(cur_sort, finite));
			}
		}
	}
	
	
	Rewriter *rewriter = createRewriter(data);
	
	equation_system result_es;

	for (equation_system::iterator eq_i = eqsys.begin(); eq_i != eqsys.end(); eq_i++)
	{
		propositional_variable propvar = eq_i->variable();
		aterm_string propvar_name = propvar.name();
		string propvar_name_string = propvar_name;				// string representation of the propvar_name
		data_variable_list parameters = propvar.parameters();
		
		gsVerboseMsg("Rewriting PBES equation with propositional variable %s\n", propvar_name_string.c_str());
		vector< t_instantiations > instantiation_list;
		t_instantiations instantiation;
				
		data_variable_list empty_data_variable_list;
		data_expression_list empty_data_expression_list;

		instantiation.original = parameters;
		instantiation.original_finite = empty_data_variable_list;
		instantiation.finite_inst = empty_data_expression_list;
		instantiation.infinite_inst = empty_data_variable_list;
		
		instantiation_list.push_back(instantiation);
		
		for (data_variable_list::iterator p = parameters.begin(); p != parameters.end(); p++)
		{
			vector< t_instantiations > current_instantiation_list;
			if (check_finite(data.constructors(), p->sort()))
			{ // Data sort is finite, enumerate it
				data_expression_list enumerations = enumerate_constructors(data.constructors(), p->sort());
				for (vector< t_instantiations >::iterator inst_i = instantiation_list.begin(); inst_i != instantiation_list.end(); inst_i++)
				{
					for (data_expression_list::iterator e = enumerations.begin(); e != enumerations.end(); e++)
					{	
						t_instantiations new_values;
						new_values.original = inst_i->original;
						new_values.original_finite = push_back(inst_i->original_finite, *p);
						new_values.finite_inst = push_back(inst_i->finite_inst, *e);
						new_values.infinite_inst = inst_i->infinite_inst;
						current_instantiation_list.push_back(new_values);
					}
				}
			}
			else
			{ // Data sort is infinite don't enumerate; resulting system is a PBES
				is_bes_system = false;
				for (vector< t_instantiations >::iterator inst_i = instantiation_list.begin(); inst_i != instantiation_list.end(); inst_i++)
				{
					t_instantiations new_values;
					new_values.original = inst_i->original;
					new_values.original_finite = inst_i->original_finite;
					new_values.finite_inst = inst_i->finite_inst;
					new_values.infinite_inst = push_back(inst_i->infinite_inst, *p);
					current_instantiation_list.push_back(new_values);
				}
			}
			instantiation_list = current_instantiation_list;
		}

		for (vector< t_instantiations >::iterator inst_i = instantiation_list.begin(); inst_i != instantiation_list.end(); inst_i++)
		{
			bool change_propvar = inst_i->finite_inst.size() > 0; // Has the propvar to be changed?
			if (propvar.name() == initial_state.name())
				change_propvarinst_initial_state = change_propvar;
			// Rewrite lhs
			propositional_variable current_propvar = create_propositional_variable(propvar, inst_i->finite_inst, change_propvar, sort_list_is_finite);	
			
			// Rewrite rhs and replace PropVars with BES-equivalents in rhs
			pbes_equation current = *eq_i;

			pbes_expression current_exp = pbes_expression_rewrite(current.formula().substitute(make_list_substitution(inst_i->original_finite, inst_i->finite_inst)), data, rewriter);
			current_exp = pbes_expression_rewrite(current_exp, data, rewriter);
			//cout << current_exp << endl;
			propositional_variable_instantiation_list oldpropvarinst_list;
			propositional_variable_instantiation_list newpropvarinst_list;
			set< propositional_variable_instantiation > propvarinst_set = find_propositional_variable_instantiations(current_exp);
			for (set< propositional_variable_instantiation >::iterator pvi = propvarinst_set.begin(); pvi != propvarinst_set.end(); pvi++)
			{
				oldpropvarinst_list = push_front(oldpropvarinst_list, *pvi);
				propositional_variable_instantiation propvarinst_toAdd = create_propositional_variable_instantiation(*pvi, change_propvar, sort_list_is_finite);
				newpropvarinst_list = push_front(newpropvarinst_list, propvarinst_toAdd);
			}
			current_exp = current_exp.substitute(make_list_substitution(oldpropvarinst_list, newpropvarinst_list));

			result_es.push_back(pbes_equation(eq_i->symbol(), current_propvar, current_exp));
		}
	}

	// rewrite initial state
	propositional_variable_instantiation new_initial_state = create_propositional_variable_instantiation(initial_state, change_propvarinst_initial_state, sort_list_is_finite); 

	result = pbes(data, result_es, new_initial_state);
	
	delete rewriter;
	
	return result;
}

propositional_variable create_propositional_variable(propositional_variable propvar, data_expression_list finite_inst, bool change_propvar, map<lpe::sort, bool> sort_list_is_finite)
{
	string propvar_name_current = propvar.name();
	bool is_first;
	if (change_propvar)
	{
		propvar_name_current += "[";
		is_first = true;
	}
	data_variable_list parameters_current;
	for (data_variable_list::iterator deli = propvar.parameters().begin(); deli != propvar.parameters().end(); deli++)
	{
		if (sort_list_is_finite.find(deli->sort())->second)
		{
			if (is_first)
				is_first = false;
			else
					propvar_name_current += ";";
			propvar_name_current += data_operation(front(finite_inst).head()).name();
			finite_inst = pop_front(finite_inst);
		}
		else
		{
			parameters_current = push_back(parameters_current, *deli);
		}
	}
	if (change_propvar)
	{
		propvar_name_current += "]";
	}

	return propositional_variable(propvar_name_current, parameters_current);
}

propositional_variable_instantiation create_propositional_variable_instantiation(propositional_variable_instantiation propvarinst, bool change_propvar, map<lpe::sort, bool> sort_list_is_finite)
{
	string propvarinst_name_current = propvarinst.name();
	bool is_first;
	if (change_propvar)
	{
		propvarinst_name_current += "[";
		is_first = true;
	}
	data_expression_list parameters_current;
	for (data_expression_list::iterator deli = propvarinst.parameters().begin(); deli != propvarinst.parameters().end(); deli++)
	{
		if (sort_list_is_finite.find(deli->sort())->second)
		{
			if (is_first)
				is_first = false;
			else
					propvarinst_name_current += ";";
			//cout << deli->head() << endl;
			propvarinst_name_current += data_operation(deli->head()).name();
		}
		else
		{
			parameters_current = push_back(parameters_current, *deli);
		}
	}
	if (change_propvar)
	{
		propvarinst_name_current += "]";
	}

	return propositional_variable_instantiation(propvarinst_name_current, parameters_current);
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

