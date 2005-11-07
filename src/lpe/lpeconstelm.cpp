//C++
#include <iostream>
#include <vector>
#include <set>
#include <stdbool.h>
#include <string>
#include <iterator>
#include <fstream>
#include <stdio.h>

//Boost
#include <boost/program_options.hpp>

//mCRL2
#include "atermpp/aterm.h"
#include "mcrl2/specification.h"

#include "libgsrewrite.h"
#include "gsfunc.h"
#include "gslowlevel.h"

using namespace std;
using namespace mcrl2;
using namespace atermpp;

namespace po = boost::program_options;
po::variables_map vm;

//Constanten
string version = "Version 0.5";
bool verbose    = false; 
bool alltrue    = false;
bool reachable  = false;
bool nosingleton = false;

vector< data_expression >             nextState;
vector< data_expression >             currentState;
vector< data_expression >             newCurrentState;
set< int >                            V;                //Set of indices of process parameters which are variable
vector< LPE_summand>                  visitedSummands;
bool                                  same;             //Flag for detecting change;
vector< data_expression >             freeVarList;
map< data_variable, int  >            lookupIndex;
map< int, data_variable >             lookupDataVarIndex;
vector< data_assignment >             assignmentVector;
int                                   numOfnewVars;                          

//Begin Debug print 
void pe(vector< data_expression > x)
{
  for(vector< data_expression >::iterator i = x.begin(); i != x.end(); i++){
    cout << "[" << i->pp() << "]" ;
  }
  cout << endl;
}

void pa(vector< data_assignment > x)
{
  for(vector< data_assignment >::iterator i = x.begin(); i != x.end(); i++){
    cout << "[" << i->pp() << "]" ;
  }
  cout << endl;
}

void print_set(set< int > S)
{
  cout << " Set : ";
  
  set< int >::iterator i = S.begin();
  int j = 0;
  while(i != S.end()){
    if (*i ==j){
      cout << j << " ";
      i++;
    }
  j++;
  }
  cout << endl;
}

//End Debug print


ATermAppl rewrite(ATermAppl t)
{
  //gsEnableConstructorFunctions();
  ATermAppl result = gsRewriteTerm(t);
  return result;
}

bool compare(data_expression x, data_expression y)
{
    return x==y;
}

// calculates NextState
//
vector< data_expression > calculateNextState(vector< data_expression > oldVector, data_assignment_list assignments, set< int > V){
  vector< data_expression > newVector = oldVector;
  vector< data_assignment > tmp_ass;

  for(vector< data_expression >::iterator i = oldVector.begin(); i != oldVector.end() ; i++ ){
    tmp_ass.push_back(data_assignment(lookupDataVarIndex[i-oldVector.begin()], *i ));
  }
  
   //pa(tmp_ass); 
  
  for(data_assignment_list::iterator j = assignments.begin(); j != assignments.end(); j++ ){
    if (V.find(lookupIndex[j->lhs()]) == V.end()){
      newVector[lookupIndex[j->lhs()]] = j->rhs().substitute(tmp_ass.begin(), tmp_ass.end()); 
    }
  }
  return newVector;
};

// free Variable check
//
bool inFreeVarList(data_expression item){
  bool b = false;
  for(vector< data_expression >::iterator i = freeVarList.begin(); i != freeVarList.end() ; i++ ){
    b = (item == *i) || b;
  }
  return b;
}

data_expression calcCondition(vector< data_expression > currentState, data_expression conditionExpression )
{
  for(vector< data_expression >::iterator i = currentState.begin(); i != currentState.end()  ;i++){
    conditionExpression = conditionExpression.substitute( data_assignment(lookupDataVarIndex[i - currentState.begin()], *i ));
  } 
  return rewrite(conditionExpression);
}


void print_const(specification spec , set< int > S)
{  
  //if (verbose)
  {
  LPE lpe = spec.lpe();
  set< int >::iterator i;
  data_assignment_list sub = spec.initial_assignments();
  cout << " The constant process parameters " << endl << "   ";
  if (!S.empty()){
    i = S.begin(); 
    int k = 0;
    for(data_assignment_list::iterator j = sub.begin() ;j != sub.end() ;j++){
      if (*i ==k){
        cout << "[" << j->pp() << "]" ;
        i++;
      };
      k++;
    }
    cout << endl;
  } else
  {cout << "[]" << endl;} 
  }
}

data_expression newExpression(data_assignment ass)
{
  char buffer [99];
  sprintf(buffer, "%s^%d", ass.lhs().name().c_str(), numOfnewVars);
  numOfnewVars++;
  data_variable w(buffer, ass.lhs().type() );
  //data_assignment a(datavar , w.to_expr() );
  return w.to_expr();
}

summand_list rebuild_summands(vector< LPE_summand > sum_true) 
{
  summand_list sout;
  for(vector<LPE_summand>::iterator i = sum_true.begin(); i!= sum_true.end(); i++){
    sout = push_front(sout, *i);  
  }
  sout = reverse(sout);
  return sout;
}

void  rebuild_lpe(specification spec,string  outfile, set< int > S, bool single, vector< LPE_summand > sum_true){

  LPE lpe = spec.lpe();
  summand_list rebuild_summandlist;
  data_equation_list rebuild_equations =  spec.equations();

  //Remove the summands that are never visited
  //
  if (reachable){
    rebuild_summandlist = rebuild_summands(sum_true); 
  } else {
    rebuild_summandlist = lpe.summands();
  }

  if (verbose) {
    cout << "  Number of summand "<<  sum_true.size() << endl;
  }

  //Singleton sort process parameters
  //
  sort_list rebuild_sort = spec.sorts();
  // 2B implemented

  //Detect which process parameters are (not) constant
  //
  vector< data_assignment>  const_parameters;
  data_variable_list rebuild_process_parameters;
  data_expression_list rebuild_data_expression_pars;
  set< int >::iterator i;
  data_assignment_list sub = spec.initial_assignments();
  if (!S.empty()){
    i = S.begin(); 
    int k = 0;
    bool b = false;
    for(data_assignment_list::iterator j = sub.begin() ;j != sub.end() ;j++){
      if (*i==k){ b= true;
        const_parameters.push_back(*j);
        i++;
      };
      if (!b) {
        rebuild_process_parameters = push_front(rebuild_process_parameters, j->lhs()); 
        rebuild_data_expression_pars = push_front(rebuild_data_expression_pars, j->rhs());      
      }
      k++;
    }
    rebuild_process_parameters = reverse(rebuild_process_parameters);
    rebuild_data_expression_pars = reverse( rebuild_data_expression_pars);
  } else {
    rebuild_process_parameters = lpe.process_parameters();  
    rebuild_data_expression_pars = spec.initial_state();
  }
   
  //Remove process parameters in in summand
  // 
  summand_list rebuild_summandlist_no_cp;
  for(summand_list::iterator j = rebuild_summandlist.begin(); j != rebuild_summandlist.end(); j++){
    data_assignment_list rebuild_sum_ass; 
    for(data_assignment_list::iterator k = j->assignments().begin(); k != j->assignments().end() ; k++){
      bool b = false;
      for(vector< data_assignment>::iterator l = const_parameters.begin(); l != const_parameters.end() ; l++ ){
        b = b || (k->lhs() == l->lhs() );
      }
      if (!b){
        rebuild_sum_ass = push_front(rebuild_sum_ass, *k);
      }
	  }  
    //construct new LPE_summand
    //
    LPE_summand tmp;
    
    //Rewrite condition
    data_expression rebuild_condition = j->condition();
    rebuild_condition = rebuild_condition.substitute(const_parameters.begin(), const_parameters.end());
    rebuild_condition = data_expression(rewrite(rebuild_condition)) ;

    //LPE_summand(data_variable_list summation_variables, data_expression condition, 
    //            bool delta, action_list actions, data_expression time, 
    //            data_assignment_list assignments);    
    tmp = LPE_summand(j->summation_variables(), rebuild_condition, 
	    j->is_delta(), j->actions(), j->time(), 
	    reverse(rebuild_sum_ass));
    rebuild_summandlist_no_cp = push_front(rebuild_summandlist_no_cp, tmp); 
  }
      
  rebuild_summandlist = substitute( reverse(rebuild_summandlist_no_cp), const_parameters.begin(), const_parameters.end() );
  
  //construct new specfication
  //
  //LPE(data_variable_list free_variables, data_variable_list process_parameters, 
  //  summand_list summands, action_list actions);
  LPE rebuild_lpe;
  rebuild_lpe = LPE(
    lpe.free_variables(), 
    rebuild_process_parameters, 
    rebuild_summandlist,
    lpe.actions()
  );

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
    rebuild_equations, 
    spec.actions(), 
    rebuild_lpe, 
    spec.initial_free_variables(), 
    rebuild_process_parameters,
    rebuild_data_expression_pars  
  );
  
  if  (rebuild_spec.save(outfile)){
    cout << " Written output file: " << outfile << endl << endl;
  } else
  {
    cout << " Unsuccessfully written to output file '" << outfile << "'" << endl;
  }

  return;
}


void constelm(string filename, string outfile, int option)
{
  ATerm bot;
  ATinit(0,0,&bot);
  gsEnableConstructorFunctions();
  int cycle =0 ; 
  
  specification spec;
  if (!spec.load(filename))
  {
    cerr << "error: could not read input file '" << filename << "'" << endl;
    return;
  }

  LPE lpe = spec.lpe(); 
  data_equation_list equations = spec.equations();
  gsRewriteInit(gsMakeDataEqnSpec(aterm_list(equations)), GS_REWR_INNER3); 


  // Create assignmentVector
  int counter = 0;
  for(data_assignment_list::iterator i = spec.initial_assignments().begin(); i != spec.initial_assignments().end() ; i++ ){
    assignmentVector.push_back(*i);
    lookupIndex[i->lhs()] = counter;
    //lookupExpression[]
    currentState.push_back(i->rhs());
    //overbodig
    lookupDataVarIndex[counter] = i->lhs();
    //
    counter++;
  }

  // Create freevariable list
  for (data_variable_list::iterator di = spec.initial_free_variables().begin(); di != spec.initial_free_variables().end(); di++){
    freeVarList.push_back(di->to_expr());
  }
  for (data_variable_list::iterator di = lpe.free_variables().begin(); di != lpe.free_variables().end(); di++){
    freeVarList.push_back(di->to_expr());
  } 

//----------- 
same = false;
while (!same){
  same = true;
  cout << "Cycle:" << cycle++ << endl;
  int summandnr = 0;
  for(summand_list::iterator currentSummand = lpe.summands().begin(); currentSummand != lpe.summands().end() ;currentSummand++ ){
    if (!calcCondition(currentState, currentSummand->condition()).is_false()){
      //Add currentSummand to list of visitedSummands
      visitedSummands.push_back(*currentSummand); 

      int counter = 0;

      cout << "Summand: "<< summandnr++ << endl;
      //Calculate nextState

      nextState = calculateNextState(currentState, currentSummand->assignments(), V);
      vector< data_expression >::iterator j = nextState.begin();

      //pe(nextState);

      newCurrentState.clear();
      for(vector< data_expression >::iterator i = currentState.begin() ; i != currentState.end() ; i++ ){
        if (inFreeVarList(*i)){
          newCurrentState.push_back(*j);
          //cout << i->pp() << "in free var " <<endl;
        } else {
        if (inFreeVarList(*j)){
            //skip
            newCurrentState.push_back(*i);
        } else {  
          //cout << "cmp " << i->pp()  << " --- " << j->pp() << endl;
          if (compare(*i,*j) ){
            newCurrentState.push_back(*i);
          } else {
            if (V.find(counter) == V.end()){
              V.insert(counter);
              newCurrentState.push_back(newExpression( assignmentVector[counter] )); 
              same = false;
            }
          }
        }
        j++;
        counter++;
      }
      }
      currentState = newCurrentState;
    }
  }
  }

  set< int > S;
  set< int > R;

  int  n = lpe.process_parameters().size(); 
  for(int j=0; j < n ; j++){
    S.insert(j);
  };
 
  set_difference(S.begin(), S.end(), V.begin(), V.end(), inserter(R, R.begin()));
  print_set(R);
  print_const(spec, R);
  
  bool b = false;
  rebuild_lpe(spec, outfile, R , b, visitedSummands);

  return;
}


string addconstelm(string filename)
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
  token = filename.substr( begIdx, endIdx ).append("lpeconstelm.lpe");
  
  return token;

}


int main(int ac, char* av[])
{
   ATerm bot;
   ATinit(0,0,&bot);
   gsEnableConstructorFunctions();

  
   vector< string > filename;
   int opt = 0;

   try {
     po::options_description desc;
     desc.add_options()
       ("help,h",      "display this help")
       ("version,v",   "display version information")
       ("monitor,m",   "display progress information")
       ("nosingleton", "do not remove sorts consisting of a single element")
       ("nocondition", "do not use conditions during elimination (faster)")
       ("reachable,r", "elimantes summands which cannot be reached")
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
       cerr << "Remove constant process parameters from the LPE in INFILE, and write the result" << endl;
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
       //cerr << "Displaying progress" << endl;
       verbose = true;
	   }

     if (vm.count("nosingleton")) {
       //cerr << "Active: no removal of process parameters which have sorts of cardinatilty one" << endl;
       nosingleton = true;
	   }

     if (vm.count("nocondition")) {
       //cerr << "Active: All conditions are true" << endl;
       alltrue = true;
	   }

     if (vm.count("reachable")) {
       reachable = true;
	}

     if (vm.count("INFILE")){
       filename = vm["INFILE"].as< vector<string> >();
	   }

     if (filename.size() > 2){
        cerr << "Specify only INPUT and/or OUTPUT file (Too many arguments)."<< endl;
     };
             

     if (filename.size() == 2){
       constelm(filename[0], filename[1], opt); 
     };
	
     if(filename.size() == 1){
      constelm(filename[0], addconstelm(filename[0]) , opt);
        
    };

    }
    catch(exception& e){
      cerr << e.what() << "\n";
      return 1;
    }    
    
    return 0;
}


