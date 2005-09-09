#include <iostream>
#include "atermpp/aterm.h"
#include "mcrl2/mcrl2_visitor.h"
#include "mcrl2/lpe.h"
#include <boost/program_options.hpp>

using namespace std;
using namespace mcrl2;
using namespace atermpp;

namespace po = boost::program_options;
po::variables_map vm;

//Constanten
string version = "Version 0.1";

int display(string filename, int opt)
{
  
  int outputvar;
  aterm_appl t = read_from_named_file(filename).to_appl();
  if (!t)
    cerr << "could not read file!" << endl;
  
  if (opt==0){
  //
  // Aantal summanden
  //
  LPE::summand_iterator sum_itb = LPE(t).summands_begin();
  LPE::summand_iterator sum_ite = LPE(t).summands_end(); 
  outputvar = distance(sum_itb, sum_ite);
  cout << "Number of LPE summands :" << outputvar <<endl;

  LPE::variable_iterator var_itfb = LPE(t).free_variables_begin();
  LPE::variable_iterator var_itfe = LPE(t).free_variables_end();
  outputvar = distance(var_itfb, var_itfe);
  cout << "Number of free LPE variables " << outputvar <<endl;

  LPE::variable_iterator var_itpb = LPE(t).process_parameters_begin();
  LPE::variable_iterator var_itpe = LPE(t).process_parameters_end();
  outputvar = distance(var_itpb, var_itpe);
  cout << "Number of LPE process variables " << outputvar <<endl; 

  //
  // Acties... ?
  //
  LPE::action_iterator act_itb = LPE(t).actions_begin();
  LPE::action_iterator act_ite = LPE(t).actions_end();
  
  //SEGMENTATION FAULT outputvar = distance(act_itb, act_ite);
  //Fout in lpe.h
  //cout << "Number of LPE actions " << outputvar << endl;
  
  }
  
  if (opt==1){
  LPE::variable_iterator var_itpb = LPE(t).process_parameters_begin();
  LPE::variable_iterator var_itpe = LPE(t).process_parameters_end();

  //Inspecteren van de iteratorlist
  string stringout_1;
  string stringout_2;
  LPE::variable_iterator s_current = var_itpb;
  while (s_current != var_itpe)
  {
     stringout_1 = DataVariable(*s_current).name();
     stringout_2 = DataVariable(*s_current).sort().to_string();
     
     cout << stringout_1 << " " << stringout_2 ;
     ++s_current;

  };	
  //Alt:
  //LPEInit lpe_init = LPE(t).lpe_init();
  //cout << lpe_init.to_string() << endl;
}
  if (opt==2){
    	LPE::variable_iterator var_itpb = LPE(t).process_parameters_begin();
  	LPE::variable_iterator var_itpe = LPE(t).process_parameters_end();
  	outputvar = distance(var_itpb, var_itpe);
  	cout << "Number of LPE process variables " << outputvar <<endl;
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
            ("input-file,i", po::value<string>(), "optional input file")
            ("version,v", "get the version number of the current release of the mCRL2 tools")
	    ("pars","prints only the number of parameters")
            ("npars","prints the list of parameters")	
        ;

        po::positional_options_description p;
        p.add("input-file", -1);
        
        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).
                  options(desc).positional(p).run(), vm);
        po::notify(vm);
    
        if (vm.count("help")) {
            cout << "Usage: options_description [options]\n";
            cout << desc;
            return 0;
        }
        
        if (vm.count("version")) {
	    cout << version << endl;
	}

        if (vm.count("input-file"))
        {
          filename = vm["input-file"].as<string>();
	  cout << filename << endl;
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
