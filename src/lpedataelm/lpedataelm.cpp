// ======================================================================
//
// Copyright (c) 2005 TU/e
//
// ----------------------------------------------------------------------
//
// file          : lpedataelm 
// date          : 06-12-2005
// version       : 0.1
//
// author(s)     : Muck van Weerdenburg  <M.J.van.Weerdenburg@tue.nl>
//
// ======================================================================

//C/C++
#include <stdio.h>
#include <errno.h>
#include <iostream>

//Boost
#include <boost/program_options.hpp>

//mCRL2
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "libdataelm.h"
#include "lpe/specification.h"

using namespace std;
using namespace lpe;

namespace po = boost::program_options;

#define VERSION "0.1"

static ATerm readSpec(FILE *f)
{
	return ATreadFromFile(f);
}

static ATerm readSpec(string filename)
{
	FILE *f;

	f = fopen(filename.c_str(),"rb");
	if ( f == NULL )
	{
		cerr << "cannot read input: " << strerror(errno) << endl;
		return NULL;
	}

	ATerm r = readSpec(f);

	fclose(f);

	return r;
}

static void writeSpec(FILE *f, ATerm Spec)
{
	ATwriteToBinaryFile(Spec,f);
}

static void writeSpec(string filename, ATerm Spec)
{
	FILE *f;

	f = fopen(filename.c_str(),"wb");
	if ( f == NULL )
	{
		cerr << "cannot write output: " << strerror(errno) << endl;
		return;
	}

	writeSpec(f,Spec);

	fclose(f);
}

int main(int argc, char** argv)
{
	ATerm bot;
	ATinit(argc,argv,&bot);
	gsEnableConstructorFunctions();

	vector< string > filenames;

	try {
		po::options_description desc;
		desc.add_options()
			("help,h",         "display this help")
			("version",        "display version information")
			("quiet,q",        "do not display warning messages")
			("verbose,v",      "display concise intermediate messages")
			("remove-basis,b", "also remove standard data sorts and functions if unused")
			;

		po::options_description hidden("Hidden options");
		hidden.add_options()
			("FILES", po::value< vector<string> >(), "files")
			;

		po::options_description cmdline_options;
		cmdline_options.add(desc).add(hidden);

		po::options_description visible("Allowed options");
		visible.add(desc);

		po::positional_options_description p;
		p.add("FILES", -1);

		po::variables_map vm;
		store(po::command_line_parser(argc, argv).
			options(cmdline_options).positional(p).run(), vm);

		if ( vm.count("help") > 0 )
		{
			cerr << "Usage: "<< argv[0] << " [OPTION]... [INFILE [OUTFILE]] \n";
			cerr << "Remove unused parts from the data specification of the LPE in INFILE, and write" << endl;
			cerr << "the result to OUTFILE. If OUTFILE is not present, stdout is used. If INFILE is" << endl;
			cerr << "not present, stdin is used." << endl;
			cerr << endl;
			cerr << desc;
			return 0;
		}

		if ( vm.count("version") > 0 )
		{
			cerr << "Version " << VERSION << " (revision " << REVISION << ")" << endl;
			return 0;
		}

		bool verbose = false;
		if ( vm.count("verbose") > 0 )
		{
			verbose = true;
			gsSetVerboseMsg();
		}

		bool quiet = false;
		if ( vm.count("quiet") > 0 )
		{
			if ( verbose )
			{
				cerr << "conflicting options used (-q/--quiet and -v/--verbose)" << endl;
				return 1;
			}
			quiet = true;
			gsSetQuietMsg();
		}

		if ( vm.count("FILES") > 0 )
		{
			filenames = vm["FILES"].as< vector<string> >();
		}

		ATerm Spec;
		gsVerboseMsg("reading input LPE...\n");
		if ( filenames.size() == 0 )
		{
			Spec = readSpec(stdin);
		} else if ( filenames.size() > 2 )
		{
			cerr << "too many arguments" << endl;
			return 1;
		} else {
			Spec = readSpec(filenames[0]);
		}

		if ( (Spec == NULL) || (ATgetType(Spec) != AT_APPL) || !gsIsSpecV1((ATermAppl) Spec) )
		{
			cerr << "input is not a valid LPE" << endl;
			return 1;
		}

		if ( verbose )
		{
			specification s(Spec);
			gsVerboseMsg("input LPE information\n");
			gsVerboseMsg("number of sorts: %i\n",s.data().sorts().size());
			gsVerboseMsg("number of constructors: %i\n",s.data().constructors().size());
			gsVerboseMsg("number of mappings: %i\n",s.data().mappings().size());
			gsVerboseMsg("number of equations: %i\n",s.data().equations().size());
		}

		gsVerboseMsg("removing unused data...\n");
		Spec = (ATerm) removeUnusedData((ATermAppl) Spec, vm.count("remove-basis") == 0);

		if ( verbose )
		{
			specification s(Spec);
			gsVerboseMsg("output LPE information\n");
			gsVerboseMsg("number of sorts: %i\n",s.data().sorts().size());
			gsVerboseMsg("number of constructors: %i\n",s.data().constructors().size());
			gsVerboseMsg("number of mappings: %i\n",s.data().mappings().size());
			gsVerboseMsg("number of equations: %i\n",s.data().equations().size());
		}

		gsVerboseMsg("writing output LPE...\n");
		if( filenames.size() == 2 )
		{
			writeSpec(filenames[1],Spec);
		} else {
			writeSpec(stdout,Spec);
		}

	} catch(exception& e)
	{
		cerr << e.what() << "\n";
		return 1;
	}    
	
	return 0;
}
