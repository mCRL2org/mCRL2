// ======================================================================
//
// file          : pbesinfo 
// date          : 21-11-2005
// version       : 0.0.3
//
// author(s)     : Alexander van Dam <avandam@damdonk.nl>
//
// ======================================================================
#define NAME "pbesinfo"
#define VERSION "0.0.2"
#define AUTHOR "Alexander van Dam"

//C++
#include <cstdio>
#include <exception>
#include <iostream>
//#include <vector>
#include <string>

//Boost
#include <boost/program_options.hpp>

//MCRL-specific
#include "lpe/pbes.h"

using namespace std;
using namespace lpe;

namespace po = boost::program_options;

// Name of the file to read input from
string file_name;

void parse_command_line(int argc, char** argv)
{
	po::options_description desc;
	
	desc.add_options()
			("help,h",		"display this help")
			("verbose,v",	"turn on the display of short intermediate messages")
			("debug,d",		"turn on the display of detailed intermediate messages")
			("version",		"display version information")
			;
	
	po::options_description hidden("Hidden options");
	hidden.add_options()
			("INFILE",		po::value<string>(), "input file")
			;
	
	po::options_description cmdline_options;
	cmdline_options.add(desc).add(hidden);
	
	po::options_description visible("Allowed options");
	visible.add(desc);
	
	po::positional_options_description p;
	p.add("INFILE", -1);
	
	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
	po::notify(vm);
	
	if (vm.count("help"))
	{
		cerr << "Usage: " << argv[0] << " [OPTION]... [INFILE]" << endl;
		cerr << "Print basic information on the PBES in INFILE." << endl;
		cerr << endl;
		cerr << desc;
		
		exit(0);
	}
	
	if (vm.count("version"))
	{
		cerr << "pbesinfo " << VERSION <<  " (revision " << REVISION << ")" << endl;
		
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
	
	file_name = (0 < vm.count("INFILE")) ? vm["INFILE"].as<string>() : "-";	
}

int main(int argc, char** argv)
{
	ATerm bottom;
	ATinit(argc, argv, &bottom);
	
	gsEnableConstructorFunctions();
	
	parse_command_line(argc, argv);
	
	lpe::pbes pbes_specification;
	
	/// If PBES can be loaded from file_name, then
	///	- Show number of equations
	/// - Show number of mu's / nu's.
	/// - Show Binding variables and their type
	/// else 
	/// - Give error
	if (pbes_specification.load(file_name))
	{
		// Get PBES equations. Makes a lot of function calls more readable.
		lpe::equation_system eqsys;
		eqsys = pbes_specification.equations();
		
		// Get number of mu's / nu's
		int mu = 0;
		int nu = 0;
		int fp_errors = 0;
		for (lpe::equation_system::iterator fp_i = eqsys.begin(); fp_i != eqsys.end(); fp_i++)
		{
			if (fp_i->symbol().is_mu())
			{
				mu++;						// The fixpoint is a mu
			}
			else if (fp_i->symbol().is_nu())
			{
				nu++;						// The fixpoint is a nu
			}
			else
				fp_errors++;				// The fixpoint is not a mu and not a nu. Probably an error has occurred.
		}
		// Show an error-message if there were some errors when reading the fixpoints.
		if (fp_errors != 0)
		{
			cerr << "Reading number of mu's and nu's had errors. Results may be incorrect" << endl;
		}
		
		cout << "Input read from '" << ((file_name == "-") ? "standard input" : file_name) << "'" << endl << endl;
		
		// Show number of equations
		cout << "Number of equations: " << eqsys.size() << endl;
		
		// Show number of mu's, nu's
		cout << "Number of mu's:      " << mu << endl;
		cout << "Number of nu's:      " << nu << endl;
		
		// Show Binding variables with their type
		int nr_predvar = 1;
		string sort_bool = "Bool";
		for (atermpp::set<propositional_variable>::iterator bv_i = eqsys.binding_variables().begin(); bv_i != eqsys.binding_variables().end(); bv_i++)
		{
			int size = bv_i->parameters().size();
			int nr_sorts = 1;
			if (nr_predvar == 1)
				cout << "Binding variables:   " << bv_i->name() << " :: ";
			else
				cout << "                     " << bv_i->name() << " :: ";
			for (atermpp::term_list_iterator<lpe::data_variable> dv_i = bv_i->parameters().begin(); dv_i != bv_i->parameters().end(); dv_i++)
			{
				cout << dv_i->type();
				if (nr_sorts < size)
				{
					cout << " x ";
					nr_sorts++;
				}
				else
					cout << " -> "<< lpe::sort(sort_bool);
			}
			cout << endl;
			nr_predvar++;
		}
	}
	else
	{
		cerr << "Unable to load PBES from '" << file_name << "'" << endl;
	}

	return 0;
}
