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

#include <sstream>

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
#include "lpe/identifier_string.h"
#include "atermpp/utility.h"
#include "atermpp/indexed_set.h"

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
	string opt_outputformat;
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

void save_pbes_in_cwi_format(pbes pbes_spec, string outfilename);
//Post: pbes_spec is saved in cwi-format
//Ret: -

string convert_rhs_to_cwi(pbes_expression p, atermpp::indexed_set *variables);

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
	if (!pbes_spec.is_well_formed())
	{
		gsErrorMsg("The PBES is not well formed. Pbes2bes cannot handle this kind of PBES's\nComputation aborted.");
		exit(1);
	}
	if (!pbes_spec.is_closed())
	{
		gsErrorMsg("The PBES is not closed. Pbes2bes cannot handle this kind of PBES's\nComputation aborted.");
		exit(1);
	}
	pbes_spec = do_naive_algorithm(pbes_spec, tool_options);

	//return new pbes
	return pbes_spec;
}

pbes do_naive_algorithm(pbes pbes_spec, t_tool_options tool_options)
{
	pbes result;

	bool change_propvarinst_initial_state = false;

	bool is_bes_system = true;		// True if the resulting equation system is a BES, false otherwise
	int nr_of_equations = 0;		// Number of equations computed
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
		identifier_string propvar_name = propvar.name();
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
			if (++nr_of_equations % 100 == 0)
			{
				gsVerboseMsg("At Boolean equation %d\n", nr_of_equations);
			}
		}
	}

	// rewrite initial state
	propositional_variable_instantiation new_initial_state = create_propositional_variable_instantiation(initial_state, change_propvarinst_initial_state, sort_list_is_finite); 

	result = pbes(data, result_es, new_initial_state);

	gsVerboseMsg("Number of Boolean equations generated: %d\n", nr_of_equations);
	
	delete rewriter;
	
	return result;
}

propositional_variable create_propositional_variable(propositional_variable propvar, data_expression_list finite_inst, bool change_propvar, map<lpe::sort, bool> sort_list_is_finite)
{
	string propvar_name_current = propvar.name();
	bool is_first = true;
	if (change_propvar)
	{
		propvar_name_current += "[";
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
	bool is_first = true;
	if (change_propvar)
	{
		propvarinst_name_current += "[";
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
			if (is_data_operation(deli->head()))
			{
				propvarinst_name_current += data_operation(deli->head()).name();
			}
			else if (is_data_variable(deli->head()))
			{
				gsErrorMsg("The PBES contains a free variable in the propositional variable instantiation.\n");
				gsErrorMsg("Try using mcrl22lpe with the flag -f or --no-freevars to solve this.\n");
				exit(1);
			}
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
	if (tool_options.opt_outputformat == "external")
	{
		if (!pbes_spec.save(outfilename, false))
		{
			gsErrorMsg("Could not save PBES to %s\n", outfilename.c_str());
			exit(1);
		}
	}
	else if (tool_options.opt_outputformat == "binary")
	{
		if (!pbes_spec.save(outfilename, true))
		{
			gsErrorMsg("Could not save PBES to %s\n", outfilename.c_str());
			exit(1);
		}
	}
	else if (tool_options.opt_outputformat == "cwi")
	{
		if (pbes_spec.equations().is_bes())
		{
			save_pbes_in_cwi_format(pbes_spec, outfilename);
		}
		else
		{
			gsWarningMsg("Result is not a BES. Result is written in binary format.\n");
			if (!pbes_spec.save(outfilename, true))
			{
				gsErrorMsg("Could not save PBES to %s\n", outfilename.c_str());
				exit(1);
			}
				
		}
	}
}

void save_pbes_in_cwi_format(pbes pbes_spec, string outfilename)
{
	equation_system eqsys = pbes_spec.equations();
	atermpp::indexed_set *variables = new atermpp::indexed_set(2*eqsys.size(), 50);
	// First fill indexed set
	for (equation_system::iterator eq = eqsys.begin(); eq != eqsys.end(); eq++)
	{
		variables->put(eq->variable());
	} // because pbes is closed, all variables are done at this point
	
	// Then convert it.
	
	ofstream outputfile;
   	outputfile.open(outfilename.c_str(), ios::trunc);
	for (equation_system::iterator eq = eqsys.begin(); eq != eqsys.end(); eq++)
	{
		string fp;
		(eq->symbol().is_mu())?fp = "min":fp = "max";
		if (eq->variable().parameters().size() != 0)
		{
			gsErrorMsg("The used equation system is not a BES. Could not save this in CWI-format.\n");
			exit(1);
		}
	    string variable;
		stringstream variable_stream;
		variable_stream << variables->index(eq->variable());
		variable = variable_stream.str();
		string rhs = convert_rhs_to_cwi(eq->formula(), variables);

		string equation = fp + " " + variable + " = " + rhs + "\n";
		if (outputfile.is_open())
			outputfile << equation;
		else
		{
			gsErrorMsg("Could not save PBES to %s\n", outfilename.c_str());
			exit(1);
		}
	}
	outputfile.close();
}

string convert_rhs_to_cwi(pbes_expression p, atermpp::indexed_set *variables)
{
	string result;
	if (pbes_expr::is_true(p))
		result = "T";
	else if (pbes_expr::is_true(p))
		result = "F";
	else if (pbes_expr::is_and(p))
	{
		string left = convert_rhs_to_cwi(arg1(p), variables);
		string right = convert_rhs_to_cwi(arg2(p), variables);
		if ((left == "false") || (right == "false"))
			result = "false";
		else if (left == "true")
			result = right;
		else if (right == "true")
			result = left;
		else
			result = "(" + left + "&" + right + ")";
	}
	else if (pbes_expr::is_or(p))
	{
		string left = convert_rhs_to_cwi(arg1(p), variables);
		string right = convert_rhs_to_cwi(arg2(p), variables);
		if ((left == "true") || (right == "true"))
			result = "true";
		else if (left == "false")
			result = right;
		else if (right == "false")
			result = left;
		else
			result = "(" + left + "|" + right + ")";
	}
	else if (pbes_expr::is_propositional_variable_instantiation(p))
	{
		propositional_variable_instantiation propvarinst = propositional_variable_instantiation(p);
		data_variable_list empty;
		propositional_variable propvar = propositional_variable(propvarinst.name(), empty);
		long variable = variables->index(propvar);
		if (variable < 0)
		{
			gsErrorMsg("Error: The BES is not closed. Write to cwi-format failed.");
			exit(1);
		}
		else
		{
			stringstream variable_stream;
			variable_stream << variable;
			result = variable_stream.str();
		}
	}
	else
	{
		gsErrorMsg("The used equation system is not a BES. Could not save this in CWI-format.\n");
		exit(1);
	}
	return result;
}

//function parse_command_line
//---------------------------
t_tool_options parse_command_line(int argc, char** argv)
{
	t_tool_options tool_options;
	string opt_outputformat;
	vector< string > file_names;

	po::options_description desc;

	desc.add_options()
			("output,o",	po::value<string>(&opt_outputformat)->default_value("binary"), "use outputformat arg (default 'binary');"
			 				"available outputformats are binary, external and cwi")
			("external,e",	"return the result in external format")
			("cwi,c", 		"return the result in the format used at CWI")
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

	if (vm.count("output")) // Pretty printed version
	{
		opt_outputformat = vm["output"].as< string >();
		if (!((opt_outputformat == "external") || (opt_outputformat == "binary") || (opt_outputformat == "cwi")))
		{
			gsErrorMsg("Unknown outputformat specified. Available outputformats are binary, external and cwi");
			exit(1);
		}
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
	tool_options.opt_outputformat = opt_outputformat;
	tool_options.infilename = infilename;
	tool_options.outfilename = outfilename;
	return tool_options;
}

