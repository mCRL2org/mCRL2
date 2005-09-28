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
string version = "Version 0.3";

int display(string filename, int opt)
{
  specification spec;
  if (!spec.load(filename))
  {
    cerr << "could not read " << filename << endl;
    return 1;
  }
  LPE lpe = spec.lpe();
    
  if (opt==0)
  {
    cout << "Number of summands          :" << lpe.summands().size() <<endl;
    cout << "Number of free variables    :" << spec.initial_free_variables().size() + lpe.free_variables().size() <<endl;
    cout << "Number of process variables :" << lpe.process_parameters().size() <<endl; 
    cout << "Number of actions           :" << lpe.actions().size() << endl;
  }

  if (opt==1)
  {
    for (data_variable_list::iterator i = lpe.process_parameters().begin(); i != lpe.process_parameters().end(); ++i)
    {
      cout << i->name() << " " << i->type().to_string() << endl;
    }
  }

  if (opt==2)
  {
    cout << lpe.process_parameters().size(); 
  }

  return 0;
}

template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
    copy(v.begin(), v.end(), ostream_iterator<T>(cout, " ")); 
    return os;
}

int main(int ac, char* av[])
{
      string filename;
      int opt = 0;

      try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("version,v", "get the version number of the current release of the mCRL2 tools")
	    ("pars","prints only the number of parameters")
            ("npars","prints the list of parameters")	
        ;
	
	po::options_description hidden("Hidden options");
	hidden.add_options()
	    ("input-file", po::value<string>(), "input file" )
	;
	
	po::options_description cmdline_options;
	cmdline_options.add(desc).add(hidden);
	
	po::options_description visible("Allowed options");
	visible.add(desc);
	
	po::positional_options_description p;
	p.add("input-file", -1);
	
	po::variables_map vm;
        po::store(po::command_line_parser(ac, av).
                  options(cmdline_options).positional(p).run(), vm);
        po::notify(vm);
        
	// If no arguments are ac==1, so print help
        if (vm.count("help") || ac == 1) {
            cout << "Usage: "<< av[0] << " [options] input-file\n";
            cout << desc;
            return 0;
        }
        
        if (vm.count("version")) {
	    cout << version << endl;
	    return 0;
	}

        if (vm.count("input-file"))
        {
          filename = vm["input-file"].as<string>();
	}

	if (vm.count("pars"))
		{opt = 1;}
	if (vm.count("npars"))
		{opt = 2;}
  display(filename, opt);       
    }
    catch(exception& e)
    {
        cout << e.what() << "\n";
        return 1;
    }    
    return 0;
}
