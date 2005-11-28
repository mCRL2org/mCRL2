// ======================================================================
//
// Copyright (c) 2004, 2005 TU/e
//
// ----------------------------------------------------------------------
//
// file          : lpeparelm 
// date          : 15-11-2005
// version       : 0.5
//
// author(s)     : Frank Stappers  <f.p.m.stappers@student.tue.nl>
//
// ======================================================================

//C++
#include <stdio.h>

//Boost
#include <boost/program_options.hpp>

//mCRL2
#include "lpe/specification.h"
#include "atermpp/aterm.h"
#include "lpe/lpe_error.h"

using namespace std;
using namespace lpe;
using namespace atermpp;

using atermpp::aterm;
using atermpp::aterm_appl;
using atermpp::aterm_list;

namespace po = boost::program_options;
po::variables_map vm;


//Constanten
//Private:
  #define p_version "lpeparelm 0.5";
//Public:

class ParElmObj
{
private:
  string                      p_inputfile;
  string                      p_outputfile;
  vector< data_assignment >   p_initAssignments;
  set< data_variable >        p_S;                // <-inhert process parameters
  set< data_variable >        p_usedVars;
  bool                        p_verbose;
  bool                        p_debug;
  specification               p_spec;

  //Only used by getDataVarIDs  
  set< data_variable >        p_foundVariables;       
  
  void getDatVarRec(aterm_appl t)
  {
    if(gsIsDataVarId(t)){
      p_foundVariables.insert(t);
    };
    for(aterm_list::iterator i = t.argument_list().begin(); i!= t.argument_list().end();i++) {
      getDatVarRec((aterm_appl) *i);
    } 
  } 
  
  // Returns a vector in which each element is a AtermsAppl (DataVarID)  
  //
  set< data_variable > inline getDataVarIDs(aterm_appl input)
  {
    p_foundVariables.clear();
    getDatVarRec(input);
    return p_foundVariables;
  }
  
  // template for changing a vector into a list
  //  
  template <typename Term>
  inline
  term_list<Term> vectorToList(vector<Term> y)
  { 
    term_list<Term> result;
    for(typename vector<Term>::iterator i = y.begin(); i != y.end() ; i++)
      { 
        result = push_front(result,*i); 
      }
    return atermpp::reverse(result); 
  } 

  // template for changing a set into a list
  //  
  template <typename Term>
  inline
  term_list<Term> setToList(set<Term> y)
  { 
    term_list<Term> result;
    for(typename set<Term>::iterator i = y.begin(); i != y.end() ; i++)
      { 
        result = push_front(result,*i); 
      }
    return atermpp::reverse(result); 
  } 
  
public:

  // Sets verbose option
  // Note: Has to be set
  //
  void inline setVerbose(bool b)
  {
    p_verbose = b;
  }

  // Sets verbose option
  // Note: Has to be set
  //  
  void inline setDebug(bool b)
  {
    p_debug = b;
  }

  // Set output file
  //  
  void inline setSaveFile(string x)
  {
    p_outputfile = x;
  }

  // Loads an LPE from file
  // returns true if succeeds
  //  
  bool inline loadFile(string filename)
  {
    p_inputfile = filename;
    if (!p_spec.load(p_inputfile)){
      cerr << "error: could not read input file '" << filename << "'" << endl;
      return false;      
    };
    return true;   
  }

  // Reads an LPE from stdin
  // returns true if succeeds
  //  
  bool inline readStream()
  {
    ATermAppl z = (ATermAppl) ATreadFromFile(stdin);
    if (z == NULL){
      cout << "Could not read LPE from stdin"<< endl;
      return false;
    };
    if (!gsIsSpecV1(z)){
      cout << "Stdin does not contain an LPE" << endl;
      return false;
    }
    p_spec = specification(z);
    return true;
  }

  // Writes file to stdout
  //
  void writeStream(specification newSpec)
  {
    assert(gsIsSpecV1((ATermAppl) newSpec));
    ATwriteToBinaryFile(aterm(newSpec) , stdout);
  }

  // The lpeparelm filter
  //  
  void filter()
  {
    LPE                      lpe             = p_spec.lpe();

    vector< aterm_appl >     foundParameters;
    set< data_variable >     T;
    set< data_variable >     foundVariables;
    

    // Searching for process parameters:
    // In a condition:     c_i(d,e_i) for some i \in I
    // In an argument:     f_i(d,e_i) for some i \in I
    // In a timecondition: t_i(d,e_i) for some i \in I
    //
    if(p_verbose){
      cout << "  Searching for used process parameters:" << endl;
    }
    int counter = 0;
    int n = lpe.summands().size();
    for(summand_list::iterator currentSummand = lpe.summands().begin(); currentSummand != lpe.summands().end(); currentSummand++){ 
      
      if (p_verbose){
        cout << "    Summand :" << ++counter << "/" << n << endl;    
      }
      //Condition
      //
      foundVariables = getDataVarIDs(aterm_appl(currentSummand->condition()));
      //      cout << currentSummand->condition().pp()<< endl;
      //      cout << "\033[36m" << setToList(foundVariables).to_string() << "\033[0m" <<endl;
      for(set< data_variable>::iterator i = foundVariables.begin(); i != foundVariables.end(); i++){
         p_usedVars.insert(data_variable(*i));
      }

      //Time
      //
      if (currentSummand->has_time()){
        foundVariables = getDataVarIDs(aterm_appl(currentSummand->time()));
        //        cout << currentSummand->time().pp() << endl;        
        //        cout << "\033[39m" << setToList(foundVariables).to_string() << "\033[0m" <<endl; 
        for(set< data_variable >::iterator i = foundVariables.begin(); i != foundVariables.end(); i++){
           p_usedVars.insert(data_variable(*i));
        };
      }
     
      
      //Actions
      //
      for(action_list::iterator i = currentSummand->actions().begin(); i != currentSummand->actions().end(); i++){
        for(data_expression_list::iterator j = i->arguments().begin(); j != i->arguments().end(); j++){
          foundVariables = getDataVarIDs(aterm_appl(*j));
          //          cout << i->to_string() << endl;
          //          cout << "\033[31m" << setToList(foundVariables).to_string() << "\033[0m" <<endl;  
          for(set< data_variable >::iterator k = foundVariables.begin(); k != foundVariables.end(); k++){
  	         p_usedVars.insert(*k);
  	      };
        };  
      };
      //      cout << "\033[32m" << setToList(p_usedVars).to_string() << "\033[0m" <<endl;     
    }
  
    
    // Needed all process parameters which are not marked have to be eliminated
    //
    int cycle = 0;
    if(p_verbose){
      cout << "  Searching for dependent process parameters" << endl;
    } 
    bool reset = true;
    while (reset){
      if (p_verbose){
        cout << "  Cycle:"<< ++cycle << endl;
      }
      reset = false;
      counter = 0; 
      for(summand_list::iterator currentSummand = lpe.summands().begin(); currentSummand != lpe.summands().end(); currentSummand++){
        if (p_verbose){
          cout << "    Summand :" << ++counter << "/" << n << endl;    
        }
        for(data_assignment_list::iterator i = currentSummand->assignments().begin(); i !=  currentSummand->assignments().end() ;i++){
          if (p_usedVars.find(i->lhs()) != p_usedVars.end()){
            foundVariables = getDataVarIDs(aterm_appl(i->rhs()));
            //cout << i->rhs().pp() << endl;
            unsigned int  z = p_usedVars.size();
            for(set< data_variable >::iterator k = foundVariables.begin(); k != foundVariables.end(); k++){
  	          p_usedVars.insert(*k);
  	          //cout << k->pp() << endl;
  	        }
  	        //cout << z << "----" << p_usedVars.size() << endl;
            if (p_usedVars.size() != z){
              reset = true;
              //          cout << "\033[39m Match added: " << i->lhs().to_string() << "\033[0m" << endl; 
            };  
          }
        }
      }
      //cout << setToList(p_usedVars).to_string() << endl;
    }
  
    for(data_variable_list::iterator di = lpe.process_parameters().begin(); di != lpe.process_parameters().end() ; di++){
      T.insert(*di);	  
    };
    set_difference(T.begin(), T.end(),  p_usedVars.begin(),  p_usedVars.end(), inserter(p_S, p_S.begin()));
  
    //if (p_S.size() ==0){
    //  assert(test());
    //}

    if (p_verbose) {
      if (p_S.size() !=0){
        cout << " Removed process parameters: " << endl << "    " ;
        for(set< data_variable >::iterator i = p_S.begin(); i != (--p_S.end()); i++){
          cout << i->name() << ", ";
        }
        cout << (*(--p_S.end())).name() << endl;
      } else {
        cout << " No process parameters are removed. " << endl;
      }
    }// else {  
     // cout << " Number of removed process parameters : " << p_S.size() << endl;
    //}
  }
  
  void inline output()
  {
    LPE lpe = p_spec.lpe();
    summand_list rebuild_summandlist;
    data_variable_list rebuild_process_parameters;
    data_expression_list rebuild_data_expression_pars;
    
    for(data_variable_list::iterator i = lpe.process_parameters().begin() ; i != lpe.process_parameters().end() ; i++){
      if (p_usedVars.find(*i) != p_usedVars.end()){
        rebuild_process_parameters = push_front(rebuild_process_parameters, *i);
      }
    }
  
    //Remove process parameters in summands which are not used
    //
    summand_list summands = lpe.summands();
    for(summand_list::iterator i = summands.begin(); i != summands.end(); i++){
      data_assignment_list rebuild_assignments;
      for(data_assignment_list::iterator j = i->assignments().begin(); j != i->assignments().end();  j++){
        bool b = false;
        for(set<data_variable>::iterator k = p_S.begin(); k != p_S.end() ;k++){
          b = b || (*k == j->lhs());        
        }
        if (!b) {
          rebuild_assignments = push_front(rebuild_assignments , *j);
        } 
      }  
      //Construct the new summation_variable_list
      //
      data_variable_list rebuild_summation_variables;
      for(data_variable_list::iterator j = i->summation_variables().begin(); j != i->summation_variables().end(); j++ ){
        bool b = false;
        for(set<data_variable>::iterator k = p_S.begin(); k != p_S.end(); k++){
          b = b || (*k == *j);
        }
        if (!b){
          rebuild_summation_variables = push_front(rebuild_summation_variables, *j);
        }    
      }
      //LPE_summand(data_variable_list summation_variables, data_expression condition, 
      //          bool delta, action_list actions, data_expression time, 
      //          data_assignment_list assignments);
      LPE_summand rebuild_summand; 
     
      rebuild_summand = LPE_summand(atermpp::reverse(rebuild_summation_variables), i->condition(),
        i->is_delta(), i->actions(), i-> time(), atermpp::reverse(rebuild_assignments));  
      rebuild_summandlist = push_front(rebuild_summandlist, rebuild_summand);
    }
    
    //construct new specfication
    //
    //LPE(data_variable_list free_variables, data_variable_list process_parameters, 
    //  summand_list summands, action_list actions);
    LPE rebuild_lpe;
    rebuild_lpe = LPE(
      lpe.free_variables(), 
      atermpp::reverse(rebuild_process_parameters), 
      atermpp::reverse(rebuild_summandlist),
      lpe.actions()
    );
  
    data_variable_list               rebuild_initial_variables;
    data_expression_list             rebuild_initial_state;
    data_expression_list::iterator   j = p_spec.initial_state().begin();
  
    
    for(data_variable_list::iterator i = p_spec.initial_variables().begin() ; i != p_spec.initial_variables().end() ; i++){
      if (p_usedVars.find(*i) != p_usedVars.end()){
        rebuild_initial_variables = push_front(rebuild_initial_variables, *i);
        rebuild_initial_state = push_front(rebuild_initial_state, *j);
      }
      j++;
    }
    
    // Rebuild spec
    //
    //specification(sort_list sorts, function_list constructors, 
    //            function_list mappings, data_equation_list equations, 
    //            action_list actions, LPE lpe, 
    //            data_variable_list initial_free_variables, 
    //            data_variable_list initial_variables, 
    //            data_expression_list initial_state);
    //
    specification rebuild_spec;
    rebuild_spec = specification(
      p_spec.sorts(), 
      p_spec.constructors(),
      p_spec.mappings(), 
      p_spec.equations(), 
      p_spec.actions(), 
      rebuild_lpe,
      p_spec.initial_free_variables(), 
      atermpp::reverse(rebuild_initial_variables),
      atermpp::reverse(rebuild_initial_state)
    );
    
    assert(gsIsSpecV1((ATermAppl) rebuild_spec));
   
    if (p_outputfile.size() == 0){
      if(!p_verbose){
        assert(!p_verbose);
        writeStream(rebuild_spec);
      };
    } else {
      if(!rebuild_spec.save(p_outputfile)){
         cerr << "Unsuccessfully written outputfile: " << p_outputfile << endl;
      };
    } 
  }
    
  string inline getVersion()
  {
    return p_version;
  }
};

int main(int ac, char* av[])
  {
    ATerm bot;
    ATinit(0,0,&bot);
    gsEnableConstructorFunctions();
  
    vector< string > filename;
   
    ParElmObj obj;

    try {
      po::options_description desc;
      desc.add_options()
        ("help,h",      "display this help")
        ("verbose,v",   "turn on the display of short intermediate messages")
        ("debug,d",    "turn on the display of detailed intermediate messages")
        ("version",     "display version information")
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
    store(po::command_line_parser(ac, av).
    options(cmdline_options).positional(p).run(), vm);
     
    if (vm.count("help")) {
       cerr << "Usage: "<< av[0] << " [OPTION]... [INFILE [OUTFILE]] \n";
       cerr << "Remove inert parameters from the LPE in INFILE, and write the result" << endl;
       cerr << "to stdout." << endl;
       cerr << endl;
       cerr << desc;
      return 0;
    }
        
    if (vm.count("version")) {
	    cerr << obj.getVersion() << " (revision " << REVISION << ")" << endl;
	    return 0;
	  }

    if (vm.count("verbose")) {
      obj.setVerbose(true);
	  } else {
	    obj.setVerbose(false);
	  }
	  
	  if (vm.count("debug")) {
      obj.setDebug(true);
	  } else {
	    obj.setDebug(false);
	  }

    if (vm.count("INFILE")){
      filename = vm["INFILE"].as< vector<string> >();
	  }
	  
	  if (filename.size() == 0){
	    if (!obj.readStream()){return 1;}
	  }

    if (filename.size() > 2){
      cerr << "Specify only INPUT and/or OUTPUT file (Too many arguments)."<< endl;
      return 1;
    }
             
    if(filename.size() >= 1){
      if (filename[0] == ">"){
        if (!obj.readStream()){return 1;}
      }
      else{
        if(!obj.loadFile(filename[0])){return 1;};
      }
    } ; 
    if(filename.size() == 2){
      obj.setSaveFile(filename[1]);
    };

    obj.filter();
    obj.output(); 

  }
  catch(exception& e){
      cerr << e.what() << "\n";
      return 1;
  }    
  return 0;
}
