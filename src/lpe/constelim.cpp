#include <iostream>
#include "atermpp/aterm.h"
#include "mcrl2/mcrl2_visitor.h"
#include "mcrl2/lpe.h"
#include <boost/program_options.hpp>
#include <vector>

using namespace std;
using namespace mcrl2;
using namespace atermpp;


namespace po = boost::program_options;
po::variables_map vm;

//Constanten
string version = "Version 0.1";

int const_main(string filename, int opt)
{
  vector<DataExpression>            iv;		//init vector
  vector<bool> 		                  fv;		//flag vector (C= True, V= False)
  vector< vector<DataExpression> >  sv; 	//(new) state vector
  vector< vector<bool> >            cv;		//change vector	
  vector<bool>                      pcv;  //partial change vector

  int                               nopp; //number of process parameters
  int                               nos;  //number of summands
  int                               noa;  //number of assignments in the init

  //Debug test vars
  int outputvar =  0;
  outputvar++;

  aterm_appl t = read_from_named_file(filename).to_appl();
  if (!t)
    cerr << "could not read file!" << endl;
  if (opt == 0)
  {
    //Get number of process parameters
    LPE::variable_iterator var_itpb = LPE(t).process_parameters_begin();
    LPE::variable_iterator var_itpe = LPE(t).process_parameters_end();
    nopp = distance(var_itpb, var_itpe);
    
    //Build "init vector" Step 1

    //#assignments == #process parameters 
    LPEInit::assignment_iterator var_isb = LPE::LPE(t).lpe_init().assignments_begin();
    LPEInit::assignment_iterator var_ise = LPE::LPE(t).lpe_init().assignments_end();
    noa = distance(var_isb, var_ise);
    //Vragen of onderstaande regel klopt
    if(nopp!=noa){cout << "Error: #assignments != #process parameters"<< endl; return 1;}; 
    
    //Get all right hand expressions from the init 
    for(LPEInit::assignment_iterator s_current =  var_isb; s_current != var_ise; s_current++){
      iv.push_back(LPEAssignment(*s_current).rhs());
    };	

    //Build "flag vector" Step 2
    for (int i=0; i < nopp; i++){
      fv.push_back(true);
    }

    //Begin Iteration
      //Build new state vector
      
      //Get number of summands
      LPE::summand_iterator sum_itb = LPE(t).summands_begin();
      LPE::summand_iterator sum_ite = LPE(t).summands_end(); 
      nos = distance(sum_itb, sum_ite);

      for(LPE::summand_iterator s_current =  sum_itb; s_current != sum_ite; ++s_current){
        //Summand Loop
        //Each pcv should be empty
        pcv.clear();
        //Fill the pcv
        //Number of partial pcv elements
        LPESummand::assignment_iterator var_ppcvb = LPESummand(*s_current).assignments_begin();
        LPESummand::assignment_iterator var_ppcve = LPESummand(*s_current).assignments_end();
        
        //Begin Debug
        outputvar = distance(var_ppcvb, var_ppcve);
        
        cout << outputvar << endl;
        
        DataExpression c_obj = LPESummand(*s_current).assignments_begin(); 
        cout << c_obj.to_string() << endl;
        //End Debug
        
        //Add pcv
      };	
      
      //Build change vector
      //Compare change vector with flag vector
    //End Iteration  
    //Subtitute all constant values

 
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
            cout << "Usage: constelim [options] input-file\n";
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
	  cout << filename << endl;
	}

        const_main(filename, opt);       
	
    }
    catch(exception& e)
    {
        cout << e.what() << "\n";
        return 1;
    }    
    return 0;
}
