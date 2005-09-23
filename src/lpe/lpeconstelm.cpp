#include <iostream>
#include <vector>
#include <boost/program_options.hpp>
#include <stdbool.h>
#include <aterm2.h>

#include "atermpp/aterm.h"
#include "mcrl2/mcrl2_visitor.h"
#include "mcrl2/lpe.h"
#include "libgsrewrite.h"
#include "gsfunc.h"
#include "gslowlevel.h"

using namespace std;
using namespace mcrl2;
using namespace atermpp;

namespace po = boost::program_options;
po::variables_map vm;

//Constanten
string version = "Version 0.1";

//Global Vars
vector<LPEAssignment>             iv;		//init vector
vector<bool> 		                  fv;		//flag vector (C= True, V= False)
vector< vector<LPEAssignment> >   sv; 	//(new) state vector
vector< vector<bool> >            cv;		//change vector	

vector< DataVariable >            lofv; //list of free variables
//ATermAppl                         rwcon; //rewritten condition 

//debug vars
int                               noi = 0 ;  //number of iterations                               

bool substitute(LPEAssignment x){
  cout << x.lhs().name() << " by value " << x.rhs().to_string()<< endl;
  return true;
}


bool cex(LPEAssignment init, LPEAssignment state){
//
// Compares if two given assignments are equal
// if the right hand side of "state" is a don't care 
// they the given assignments are equal 
//
  if (ATisEqual(init.rhs().term(), state.rhs().term())){
    return true;
  }; 
  
  for (unsigned int i=0; i < lofv.size() ;i++){
    if (ATisEqual(state.rhs().term(),lofv[i].term())) {
      return true;
    };
  };

  return false;  
}

int eval_datexp(DataExpression datexpr, int opt)
{
  if (opt==3) {return(true);};

  for (unsigned int i=0; i < iv.size(); i++){
    if (fv[i]) {
      datexpr = datexpr.replace(iv[i].lhs(), iv[i].rhs());
    }
  }

  /**
    *   Rewrite dataxpr.term to eval
    **/
  
  ATerm rwcon = (ATerm) gsRewriteTerm(datexpr.term().appl());
  
  ATerm t = (ATerm) gsMakeDataExprTrue();
  ATerm f = (ATerm) gsMakeDataExprFalse();


 
  if (ATisEqual(rwcon,t)){ 
    return 0;
  } else {
    if (ATisEqual(rwcon,f)){ 
      return 1;
    } else {
      return 2;
    }; 
  } 
  
}

int const_main(string filename, int opt)
{

  //vector<bool>                      pcv;  //partial change vector
  vector<string>                    sovp; //set of variable process parameters
  int                               nopp; //number of process parameters
  int                               nos;  //number of summands
  int                               noa;  //number of assignments in the init
  

  // Init vars
/** 
  **/ 
 
  int outputvar =  0;
  outputvar++;
  gsEnableConstructorFunctions();
  
/**
  **/
  
  aterm_appl t = read_from_named_file(filename).to_appl();
  if (!t)
    cerr << "could not read "<< vm["input-file"].as<string>() << endl;
  if ((opt == 0) || (opt == 1) || (opt == 2) || (opt == 3))
  {
    //make list of free variables
    LPEInit::variable_iterator fv_1b = LPE::LPE(t).lpe_init().free_variables_begin();
    LPEInit::variable_iterator fv_1e = LPE::LPE(t).lpe_init().free_variables_end();
    for(LPEInit::variable_iterator s_current =  fv_1b; s_current != fv_1e; s_current++){
      lofv.push_back(*s_current);
    };  
    //concat list of free variables
    LPE::variable_iterator fv_2b = LPE::LPE(t).free_variables_begin();
    LPE::variable_iterator fv_2e = LPE::LPE(t).free_variables_end();
    for(LPEInit::variable_iterator s_current =  fv_2b; s_current != fv_2e; s_current++){
      lofv.push_back(*s_current);
    };  

    //print list of free vars
    for (unsigned int i=0; i < lofv.size(); i++){
      cout << DataVariable(lofv[i]).name() << " ";
    }
    cout << endl;
    
    //Define rewrite rules on conditons
    gsRewriteInit(ATAgetArgument(t.appl(),3),GS_REWR_INNER3);    

    //Get number of process parameters
    LPE::variable_iterator var_itpb = LPE(t).process_parameters_begin();
    LPE::variable_iterator var_itpe = LPE(t).process_parameters_end();
    nopp = distance(var_itpb, var_itpe);
    
    //Build "init vector" Step 1

    //#assignments == #process parameters 
    LPEInit::assignment_iterator var_isb = LPE::LPE(t).lpe_init().assignments_begin();
    LPEInit::assignment_iterator var_ise = LPE::LPE(t).lpe_init().assignments_end();
    noa = distance(var_isb, var_ise);
    
    if(nopp!=noa){cout << "Error: #assignments != #process parameters"<< endl; return 1;}; 
    
    //Get all assingments from the init 
    for(LPEInit::assignment_iterator s_current =  var_isb; s_current != var_ise; s_current++){
      iv.push_back(*s_current);
    };	

    //Build "flag vector" Step 2
    for (int i=0; i < nopp; i++){
      fv.push_back(true);
    }
    
    bool iteration = true;
    //Begin Iteration
    while(iteration==true){
      //Build new state vector and change vector
      
      //Get number of summands
      LPE::summand_iterator sum_itb = LPE(t).summands_begin();
      LPE::summand_iterator sum_ite = LPE(t).summands_end(); 
      nos = distance(sum_itb, sum_ite);

      for(LPE::summand_iterator s_current =  sum_itb; s_current != sum_ite; ++s_current){
        //Summand Loop
        //Each pcv should be empty
        //pcv.clear();
        
        //Number of partial pcv elements
        LPESummand::assignment_iterator var_ppcvb = LPESummand(*s_current).assignments_begin();
        LPESummand::assignment_iterator var_ppcve = LPESummand(*s_current).assignments_end();
        
        int c = eval_datexp(LPESummand(*s_current).condition() , opt);

        if ((c == 0) or (c== 2)){
          //Summand is rewritten
          //Create a new state vector
          sv.push_back(iv);          
          for (LPESummand::assignment_iterator c_obj = LPESummand(*s_current).assignments_begin(); c_obj != var_ppcve; ++c_obj){
            for (unsigned int i=0; i < sv[sv.size()-1].size(); i++){
              if (ATisEqual(sv[sv.size()-1][i].lhs().term(), LPEAssignment(*c_obj).lhs().term())){
                
                // Copy rhs of c_obj to sv
                // sv[sv.end()][i].rhs() = LPEAssignment(*c_obj).rhs();
                // Fill in the values
                //
                // No Code
                //
                // Rewrite the rhs of the current sv element
                
                // Due missing above implementation the algorithm is not working proberly
                cout << cex(sv[sv.size()-1][i],LPEAssignment(*c_obj)) << endl; 
                // Algorithm however detect possible changes between init vector and statevector 
                // Only new assignments connot be made 
                //
              }
              ;
            };
          };
          //New state vector is computed
           
          //Create a new change vector from the flag vector
          cv.push_back(fv);
          for (unsigned int i=0; i < cv[cv.size()-1].size(); i++) {
            if (cv[cv.size()-1][i]){
              //compare if state vector elements are equal to init vector elements
              cv[cv.size()-1][i] = cv[cv.size()-1][i] && cex(iv[i], sv[sv.size()-1][i]); 
            };
          }
          //change vector is created
        };
        if (c == 1) {
            //skip;          
        };
        
      }; //end summand loop	
  
      //
      //flattened the change vector and flag vector
      //
      vector<bool>          fcv = fv;      //create flattened change vector 
      vector<LPEAssignment> fsv = iv; //create flattened init vector
 
      for (unsigned int i=0; i < cv[0].size(); i++){ 
        //Check only those vectors of which the flagvector elements are true/Constant
        if (fv[i]) {
          for(unsigned int j=0; j < cv.size(); j++){ 
            if (!cv[j][i]) { 
              fcv[i] = false;
              fsv[i] = sv[j][i];
            }
          }
        };
      }

      if (opt==2)
      {
        for (unsigned int i=0; i < cv.size(); i++) {
          for (unsigned int j=0; j < cv[i].size(); j++) {
            cout << cv[i][j]; 
          }
          cout << endl;
        }
        cout << endl;
      }
     

      // change vector and state vector are flattened
     

      //Actual compare flattened change vector with flag vector
      if (fcv.size()!= fv.size()) 
        {cout << "fch != fv" << endl; return 1;}

      //Display change vector
      if (opt==2)
      {
        for (unsigned int i=0; i < fv.size(); i++){
          cout << fv[i];
        };
        cout << " -- ";
        for (unsigned int i=0; i < fcv.size(); i++){
          cout << fcv[i];
        };
      cout << endl;
      }       
      
      //While condition :)
      bool b = true;
      for (unsigned int i=0; i < fcv.size(); i++){
        b = b && (fcv[i]==fv[i]);  
      }
      //If all elements equal -> No more iteration, set iteration false
      //If a element differs -> Iteration needed, set iteration true
      iteration = !b ;
      if (iteration){
        fv = fcv; //flag vector is replaced by the flattend change vector
        iv = fsv; //init vector is replaced by the flattend state vector
      }
    noi++;
    }
  //End Iteration 
  } 
  //Subtitute all constant values
  for (unsigned int i  = 0; i != fv.size(); i++){
    if (fv[i]){ 
      substitute(iv[i]);
    };
  };
   
  //cout << "noi " << noi <<endl;
  cerr << "einde" << endl;
  
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
            ("version,v", "gets the version number of the current release of this mCRL2 tool")
            ("monitor,m", "displays progressing information")
            ("nosingleton", "displays progressing information")
            ("nocondition", "Saves computing time. No check if conditions are rewritten to false")
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

        if (vm.count("monitor")) {
          cout << "Displaying progress" << endl;
          opt = 1;
	      }

        if (vm.count("nosingleton")) {
          cout << "Active: no removal of process parameters which have sorts of cardinatilty one" << endl;
          opt = 2;
	      }

        if (vm.count("nocondition")) {
          cout << "Active: All conditions are true" << endl;
          opt = 3;
	      }

        if (vm.count("input-file"))
        {
          filename = vm["input-file"].as<string>();
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
