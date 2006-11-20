// ======================================================================
//
// file          : pbesinfo 
// date          : 07-11-2005
// version       : 0.0.1
//
// author(s)     : Alexander van Dam <avandam@damdonk.nl>
//
// ======================================================================
#define NAME "pbesinfo"
#define VERSION "0.0.1"
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
	
	parse_command_line(argc, argv);
	
	lpe::pbes pbes_specification;
	
	/// If PBES can be loaded from file_name, then
	///	- Show number of equations
	/// else 
	/// - Give error
	if (pbes_specification.load(file_name))
	{
		cout << "Input read from '" << ((file_name == "-") ? "standard input" : file_name) << "'" << endl << endl;
		
		cout << "Number of equations: " << pbes_specification.equations().size() << endl;
	}
	else
	{
		cerr << "Unable to load PBES from '" << file_name << "'" << endl;
	}
		
	return 0;
}
