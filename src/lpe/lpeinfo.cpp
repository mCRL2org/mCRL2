#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <iterator>
using namespace std;

#include "../mcrl2/lpe.h"

using namespace mcrl2;

po::variables_map vm;

static int program_options(int ac, char* av[])
{ 
  try 
  {
    po::options_description desc(
          "This program prints information about an .lpe file\n"
          "\n"
          "Usage: lpeinfo [options] file.lpe\n"
          "\n"
          "Allowed options");
    desc.add_options()
         ("help,h", "yields this message")
         ("version,v", 
          "get the version number of the current release of the mCRL2 tools")
         ("pars", "print the list of parameters")
         ("npars", "print only the number of parameters");
    
    po::positional_options_description p;
    p.add("input-file",0);

    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if (vm.count("help")) 
    { cout << desc << "\n";
      return 1;
    }

    if (vm.count("version")) 
    { cerr << "Version 0\n";
      return 1;
    }
     
  }

  catch(exception& e) 
  { cerr << "error: " << e.what() << "\n";
    return 1;
  }

  catch(...)
  { cerr << "Unknown exception\n";
    return 1;
  }
  
  return 0;
}

int main(int ac, char* av[])
{
  if (program_options(ac,av)==0)
  { cerr << "Normal program run\n";
    LPE lpe = LPE(read_from_named_file(vm["input-file"].as<string>()));

  };
}

