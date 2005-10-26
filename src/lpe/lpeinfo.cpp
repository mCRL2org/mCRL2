/* 

Version 0.3.1
*) Program exits after illigal input file
*) Added enumerated types for options

*/

#include <iostream>
#include "atermpp/aterm.h"
#include "mcrl2/specification.h"
#include <boost/program_options.hpp>

using namespace std;
using namespace mcrl2;
using namespace atermpp;

namespace po = boost::program_options;
po::variables_map vm;

//Constanten
string version = "Version 0.3.2";
enum {
  NO_OPTION = 0,
  PARS = 1,
  NPARS = 2
};


int display(string filename, int opt)
{
  specification spec;
  if (!spec.load(filename))
  {
    cerr << "Could not read input file " << filename << endl;
    return 1;
  }
  LPE lpe = spec.lpe();
    
  if (opt == NO_OPTION) {
    cout << "Output of                   : " << filename << endl << endl;
    cout << "Number of summands          : " << lpe.summands().size() <<endl;
    cout << "Number of free variables    : " << spec.initial_free_variables().size() + lpe.free_variables().size() <<endl;
    cout << "Number of process parameters: " << lpe.process_parameters().size() <<endl; 
    cout << "Number of actions           : " << lpe.actions().size() << endl;
  } else if (opt== PARS) {
    for (data_variable_list::iterator i = lpe.process_parameters().begin(); i != lpe.process_parameters().end(); ++i)
    {
      cout << i->pp() << ": " << i->type().pp() << endl;
    }
  } else if (opt== NPARS )
  {
    cout << lpe.process_parameters().size();
  }
  return 0;
}

int main(int ac, char* av[])
{

   ATerm bot;
   ATinit(0,0,&bot);
   gsEnableConstructorFunctions();

   vector< string > filename;
      int opt = NO_OPTION;

      try {
        po::options_description desc;
        desc.add_options()
            ("help,h",    "display this help")
            ("version,v", "display version information")
            ("pars",      "print process parameters")
	          ("npars",     "print the number of process parameters")
        ;
	
	po::options_description hidden("Hidden options");
	hidden.add_options()
       ("INFILE", po::value< vector<string> >(), "input file")
	;
	
	po::options_description cmdline_options;
	cmdline_options.add(desc).add(hidden);
	
	po::options_description visible("Allowed options");
	visible.add(desc);
	
	po::positional_options_description p;
	p.add("INFILE", -1);
	
	po::variables_map vm;
        po::store(po::command_line_parser(ac, av).
                  options(cmdline_options).positional(p).run(), vm);
        po::notify(vm);
        
	// If no arguments are ac==1, so print help
        if (vm.count("help") || ac == 1) {
            cerr << "Usage: "<< av[0] << " [OPTION]... INFILE" << endl;
            cerr << "Print basic information on the LPE in INFILE." << endl;
            cerr << endl;
            cerr << desc;
            return 0;
        }
        
        if (vm.count("version")) {
	    cout << version << endl;
	    return 0;
	}

        if (vm.count("INFILE"))
        {
       filename = vm["INFILE"].as< vector<string> >();
	}

	if (vm.count("pars"))
		{opt = PARS;}
	if (vm.count("npars"))
		{opt = NPARS;}

  vector< string >::iterator z;
  z = filename.begin();
  while (z != filename.end()){
    cout << endl;
    display(*z, opt);
    z++;       
    cout << endl;
  };

  }
    catch(exception& e)
    {
        cout << e.what() << "\n";
        return 1;
    }    
    return 0;
}
