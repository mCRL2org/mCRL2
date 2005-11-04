// ======================================================================
//
// Copyright (c) 2004, 2005 TU/e
//
// ----------------------------------------------------------------------
//
// file          : lpeparelm 
// date          : 1-11-2005
// version       : 0.4
//
// author(s)     : Frank Stappers  <f.p.m.stappers@student.tue.nl>
//
// ======================================================================

#include "mcrl2/specification.h"
#include <boost/program_options.hpp>

using namespace std;
using namespace mcrl2;
using namespace atermpp;

namespace po = boost::program_options;
po::variables_map vm;

//Constanten
string version = "Version 0.4";
bool verbose    = false; 

//
// Returns a vector in which each element is a AtermsAppl (DataVarID)  
//
vector< aterm_appl > getDataVarIDs(aterm_appl input)
{
  vector< aterm_appl > dout;
  vector< aterm_appl > tmp;

  if (gsIsDataVarId(input)){
    dout.push_back(input);
  };
  aterm_list::iterator i = input.argument_list().begin();
  while (i!= aterm_appl(input).argument_list().end()){ 
    if(!gsIsDataVarId(aterm_appl(*i))){
      tmp  = getDataVarIDs(*i);
      if (tmp.size() == 1) {
        dout.push_back(*tmp.begin());
      }
    } else {
      dout.push_back(aterm_appl(*i));
      return dout;
    }
    i++;
  }
  return dout; 
}

//
// Builds and saves the LPE after applying the lpeparelm tool
//
void  rebuild_lpe(specification spec,string  outfile, set< data_variable > S ){

  LPE lpe = spec.lpe();
  summand_list rebuild_summandlist;
  data_variable_list rebuild_process_parameters;
  data_expression_list rebuild_data_expression_pars;

  //Remove process parameters in summands which are not used
  //
  summand_list summands = lpe.summands();
  for(summand_list::iterator i = summands.begin(); i != summands.end(); i++){
    data_assignment_list rebuild_assignments;
    for(data_assignment_list::iterator j = i->assignments().begin(); j != i->assignments().end();  j++){
      bool b = false;
      for(set<data_variable>::iterator k = S.begin(); k != S.end() ;k++){
        b = b || (*k == j->lhs());        
      }
      if (!b) {
        rebuild_assignments = push_front(rebuild_assignments , *j);
        rebuild_process_parameters = push_front(rebuild_process_parameters, j->lhs() );
      } 
    }  
    //Construct the new summation_variable_list
    //
    data_variable_list rebuild_summation_variables;
    for(data_variable_list::iterator j = i->summation_variables().begin(); j != i->summation_variables().end(); j++ ){
      bool b = false;
      for(set<data_variable>::iterator k = S.begin(); k != S.end(); k++){
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
   
    rebuild_summand = LPE_summand(reverse(rebuild_summation_variables), i->condition(),
      i->is_delta(), i->actions(), i-> time(), reverse(rebuild_assignments));  
    rebuild_summandlist = push_front(rebuild_summandlist, rebuild_summand);
  }
  
  //construct new specfication
  //
  //LPE(data_variable_list free_variables, data_variable_list process_parameters, 
  //  summand_list summands, action_list actions);
  LPE rebuild_lpe;
  rebuild_lpe = LPE(
    lpe.free_variables(), 
    reverse(rebuild_process_parameters), 
    reverse(rebuild_summandlist),
    lpe.actions()
  );

  data_variable_list               rebuild_initial_variables;
  data_expression_list             rebuild_initial_state;
  data_variable_list::iterator     rivi = spec.initial_variables().begin();
  data_expression_list::iterator   deli = spec.initial_state().begin();

  while (rivi != spec.initial_variables().end()){
    for(data_variable_list::iterator i = rebuild_process_parameters.begin(); i != rebuild_process_parameters.end(); i++){
      if (*i == *rivi){
        rebuild_initial_variables = push_front(rebuild_initial_variables, *rivi);
        rebuild_initial_state = push_front(rebuild_initial_state, *deli );
      }  
    }
    rivi++;
    deli++;
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
    spec.sorts(), 
    spec.constructors(),
    spec.mappings(), 
    spec.equations(), 
    spec.actions(), 
    rebuild_lpe,
    spec.initial_free_variables(), 
    reverse(rebuild_initial_variables),
    reverse(rebuild_initial_state)
  );
  
  if  (rebuild_spec.save(outfile)){
    cout << " Written output file: " << outfile << endl << endl;
  } else
  {
    cout << " \033[0;31mUnsuccefully\033[0m written to output file: " << outfile << endl;
  }
 
  return; 
}

//
// lpeparelm filter
//
void parelm(string filename, string outputfile ) 
{
  specification spec;
  if (!spec.load(filename))
  {
    cerr << " \033[0;31mCould not read " << filename << "\033[0m "<< endl;
    return;
  }
 
  cout << endl <<" Read from input file : " << filename << endl;
  LPE lpe = spec.lpe();

  set< data_variable > S;
  vector< aterm_appl > z;
  vector< aterm_appl > ta;

  // Searching for process parameters:
  // In a condition:     c_i(d,e_i) for some i \in I
  // In an argument:     f_i(d,e_i) for some i \in I
  // In a timecondition: t_i(d,e_i) for some i \in I
  //
  if(verbose){
    cout << "  Searching for used proces parameters within conditions, actions and time:" << endl;
  }
  int counter = 0;
  int n = lpe.summands().size();
  for(summand_list::iterator s_current = lpe.summands().begin(); s_current != lpe.summands().end(); ++s_current){ 
    
    if (verbose){
      cout << "    Summand :" << ++counter << "/" << n << endl;    
    }
    //Condition
    //
    z = getDataVarIDs(aterm_appl(s_current->condition()));
    for(vector< aterm_appl>::iterator i = z.begin(); i != z.end(); i++){
      S.insert(data_variable(*i));
    }
    //Time
    //
    if (s_current->has_time()){
      z = getDataVarIDs(aterm_appl(s_current->time()));
      for(vector< aterm_appl>::iterator i = z.begin(); i != z.end(); i++){
        S.insert(data_variable(*i));
      };
    }
    //Actions
    //
    for(action_list::iterator i = s_current->actions().begin(); i != s_current->actions().end(); i++){
      for(data_expression_list::iterator j = i->arguments().begin(); j != i->arguments().end(); j++){
        z = getDataVarIDs(aterm_appl(*j));
        for(vector< aterm_appl>::iterator k = z.begin(); k != z.end(); k++){
	        S.insert(*k);
	      };
      };  
    };
  }

  // Searching for process parameters: 
  // In an argument: g_i(d,e_i)_j for some i \in I and j \in {1, ... , m} where the j^th proces parameter is marked.   
  if(verbose){
    cout << "  Searching for infected process parameters within assignments:" << endl;
  }
  counter = 0;
  for(summand_list::iterator s_current = lpe.summands().begin(); s_current != lpe.summands().end(); ++s_current){   
    if (verbose){
      cout << "    Summand :" << ++counter << "/" << n << endl;    
    }  
    for(data_assignment_list::iterator di = s_current->assignments().begin(); di != s_current->assignments().end(); di++){
      ta = getDataVarIDs(aterm_appl(di->rhs()));
      for(vector< aterm_appl >::iterator ja = ta.begin(); ja!= ta.end(); ja++){
	      if (S.count(data_variable(*ja)) > 0 ){
          S.insert(di->lhs());
	      }
      };	
    }
  };

  // Needed all process parameters which are not marked have to be eliminated
  //
  set< data_variable > V;
  set< data_variable > R;
  for(data_variable_list::iterator di = lpe.process_parameters().begin(); di != lpe.process_parameters().end() ; di++){
    V.insert(*di);	  
  };
  set_difference(V.begin(), V.end(), S.begin(), S.end(), inserter(R, R.begin()));

  if (verbose) {
    cout << " Process parameters which are eliminated: " << endl << "    " ;
    for(set< data_variable >::iterator i = R.begin(); i != R.end(); i++){
     cout << "[" << i->name() << "]" << endl;
    }
  } else {  
    cout << "   Number of found proces parameters to eliminate: " << R.size() << endl;
  }
  // Rebuild LPE
  //  
  rebuild_lpe(spec, outputfile, R );

  return;
}

//
// Adding file extention lpeparelm
//
string addparelmstring(string filename)
{
  string token = "";
  string::size_type begIdx;
  string::size_type endIdx;

  // Find first position after last appearance
  //  of forward- or backslash

  endIdx = filename.find_last_of( "." );
  if ( endIdx == string::npos )
    endIdx = 0;
  else
    ++endIdx;

  begIdx = 0;
  token = filename.substr( begIdx, endIdx ).append("lpeparelm.lpe");
  
  return token;

}

//
// Interface
//
int main(int ac, char* av[])
{
   ATerm bot;
   ATinit(0,0,&bot);
   gsEnableConstructorFunctions();
   vector< string > filename;

   try {
     po::options_description desc;
     desc.add_options()
       ("help,h",      "display this help")
       ("version,v",   "display version information")
       ("monitor,m",   "display progress information")
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
     
     if (vm.count("help") || ac == 1) {
       cerr << "Usage: "<< av[0] << " [OPTION]... INFILE [OUTFILE] \n";
       cerr << "Remove inert parameters from the LPE in INFILE, and write the result" << endl;
       cerr << "to stdout." << endl;
       cerr << endl;
       cerr << desc;
       return 0;
     }
        
     if (vm.count("version")) {
	     cerr << version << endl;
	     return 0;
	   }

     if (vm.count("monitor")) {
       verbose = true;
	   }

     if (vm.count("INFILE")){
       filename = vm["INFILE"].as< vector<string> >();
	   }

     if (filename.size() > 2){
        cerr << "Specify only INPUT and/or OUTPUT file (Too many arguments)."<< endl;
     };
             

     if (filename.size() == 2){
       parelm(filename[0], filename[1]); 
     };
	
     if(filename.size() == 1){
      parelm(filename[0], addparelmstring(filename[0]));
        
    };
    
    }
    catch(exception& e){
      cerr << e.what() << "\n";
      return 1;
    }    
 
    return 0;
}
