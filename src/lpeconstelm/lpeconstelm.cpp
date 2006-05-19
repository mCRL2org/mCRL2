// ======================================================================
//
// Copyright (c) 2004, 2005 TU/e
//
// ----------------------------------------------------------------------
//
// file          : lpeconstelm 
// date          : 16-11-2005
// version       : 0.5.2
//
// author(s)     : Frank Stappers  <f.p.m.stappers@student.tue.nl>
//
// ======================================================================

#include "mcrl2_revision.h"

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
#include "lpe/specification.h"

#include "librewrite_c.h"
#include "libstruct.h"
#include "liblowlevel.h"

// Squadt protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <sip/tool.h>
#endif

using namespace std;
using namespace lpe;
using namespace atermpp;

namespace po = boost::program_options;

/* Name of the file to read input from (or standard input: "-") */
std::vector < std::string > file_names;

#define VERSION "0.5.2"

class ConstelmObj
{
private:
  ATermTable                  safeguard;
  string                      p_inputfile;
  string                      p_outputfile;
  vector< data_assignment >   p_currentState;
  vector< data_assignment >   p_newCurrentState;
  vector< data_assignment >   p_nextState;
  vector< data_assignment >   p_initAssignments;
  map< data_variable, int  >  p_lookupIndex;
  map< int, data_variable >   p_lookupDataVarIndex;
  set< data_expression >      p_freeVarSet; 
  set< int >                  p_V; 
  set< int >                  p_S;
  set< LPE_summand >          p_visitedSummands;
  set< data_expression >      p_variableList; 
  int                         p_newVarCounter;
  bool                        p_verbose;
  bool                        p_debug;
  bool                        p_nosingleton;
  bool                        p_alltrue;
  bool                        p_reachable; 
  string                      p_filenamein;
  specification               p_spec;
  set< lpe::sort >            p_singletonSort;
  
  //Only used by getDataVarIDs  
  set< data_variable >        p_foundFreeVars;       

  //Only used by detectVar
  set< data_expression >      sum_vars;
 
public:
  ConstelmObj() {
    safeguard = ATtableCreate(10000,50);
  }
  ~ConstelmObj() {
    ATtableDestroy(safeguard);
  }
private:

  void getDatVarRec(aterm_appl t) {
    if(gsIsDataVarId(t) && (p_freeVarSet.find(data_variable(t).to_expr()) != p_freeVarSet.end())){
      p_foundFreeVars.insert(t);
    };
    for(aterm_list::iterator i = t.argument_list().begin(); i!= t.argument_list().end();i++) {
      getDatVarRec((aterm_appl) *i);
    } 
  } 
  
  // Returns a vector in which each element is a AtermsAppl (DataVarID)  
  //
  inline set< data_variable > getUsedFreeVars(aterm_appl input) {
    p_foundFreeVars.clear();
    getDatVarRec(input);
    return p_foundFreeVars;
  }
  
  // Rewrites an ATerm to a normal form
  //
  // pre : input is an AtermAppl
  // post: result is an ATermAppl in normal form
  inline ATermAppl rewrite(ATermAppl t) { 
    return gsRewriteTerm(t);
  }

  // Subsitutes a vectorlist of data assignements to a ATermAppl 
  //
  inline ATermAppl p_substitute(ATermAppl t, vector< data_assignment > &y ) { 
    for(vector< data_assignment >::iterator i = y.begin() ; i != y.end() ; i++){
      RWsetVariable(aterm(i->lhs()) ,gsToRewriteFormat(i->rhs()));
    }
    ATermAppl result = gsRewriteTerm(t);
    for(vector< data_assignment >::iterator i = y.begin() ; i != y.end() ; i++){
      RWclearVariable(aterm(i->lhs()));
    }
    return result;
  }
  
  // calculates a nextstate given the current 
  // stores the next state information in p_nextState 
  // 
  inline void calculateNextState(data_assignment_list assignments) {
    for(vector< data_assignment >::iterator i = p_currentState.begin(); i != p_currentState.end(); i++ ){
      int index = p_lookupIndex[i->lhs()];
      if (p_V.find(index) == p_V.end()){
        data_expression tmp = i->lhs().to_expr(); 
        for (data_assignment_list::iterator j = assignments.begin(); j != assignments.end() ; j++){
          if (j->lhs() == i->lhs()){
            tmp = j->rhs();
            break;
          }
        }
        p_nextState.at(index) = data_assignment(i->lhs(), p_substitute(tmp, p_currentState));
      } else {
        p_nextState.at(index) = *i;
      }
    }
  }
  
  // returns whether a expression occurs in the list of free variables
  //  
  inline bool inFreeVarList(data_expression dexpr) {
    return (p_freeVarSet.find(dexpr) != p_freeVarSet.end());
  }

  // Creates an unique expression:
  // these date_expressions are used to model that a process parameter has a 
  // value which is not constant
  //
  inline data_assignment newExpression(data_assignment ass) {
    char buffer [99];
    sprintf(buffer, "%s^%d", ass.lhs().name().c_str(), p_newVarCounter++);
    data_variable w(buffer, ass.lhs().type() );
    data_assignment a(ass.lhs() , w.to_expr());
    return a;
  }
  
  // returns whether two data_expressions are equal
  //
  inline bool compare(data_expression x, data_expression y) {
    return (x==y);
  }
  
  // returns whether the given data_expression is false
  //  
  inline bool conditionTest(data_expression x) {
    if (p_alltrue){return true;};
    //----------          Debug  
    //    cerr << "\033[33m " << x.pp() << endl;
    //    cerr << "\033[30m " << data_expression(rewrite(data_expression(p_substitute(x, p_currentState)))).pp() << endl;
    //    cerr << "\033[0m";
    //----------          Debug
    return (!(data_expression(rewrite(data_expression(p_substitute(x, p_currentState)))).is_false()));
  }

  // returns whether the currentState and NextState differ
  //
  inline bool cmpCurrToNext() {
    bool differs = false;
    for(vector< data_assignment>::iterator i= p_currentState.begin(); i != p_currentState.end() ;i++){
      int index = p_lookupIndex[i->lhs()]; 
      if (p_V.find(index) == p_V.end()) { 
        if (inFreeVarList(i->rhs())) { 
          if (!inFreeVarList( p_nextState.at(index).rhs() )){
	    ATtablePut(safeguard,aterm(p_nextState.at(index)),aterm(p_nextState.at(index)));
            p_newCurrentState.at(index) = p_nextState.at(index) ;
            p_currentState.at(index) = p_nextState.at(index);  
            if (p_variableList.find(p_nextState.at(index).rhs()) != p_variableList.end()){
              p_V.insert(p_lookupIndex[i->lhs()]); 
            //----------          Debug
            //            cerr << "\033[34m OLD:    "<< i->pp() << endl;
            //            cerr << "\033[32m NEW:    "<< p_nextState.at(index).pp() << endl;
            //            cerr << "\033[0m";
            //----------          Debug
            }
	  }
        } else {
          if (!inFreeVarList( p_nextState.at(index).rhs() )){
             if (!compare(i->rhs(), p_nextState.at(index).rhs())){
                //----------          Debug
                //
                //                cerr << "\033[34m OLD:    "<< i->pp() << endl;
                //                cerr << "\033[32m NEW:    "<< p_nextState.at(index).pp() << endl;
                //                cerr << "\033[0m";
                //----------          Debug
                p_newCurrentState.at(index) = newExpression(*i) ;
		p_currentState.at(index) = p_currentState.at(index);
	  	ATtablePut(safeguard,aterm(p_newCurrentState.at(index)),aterm(p_newCurrentState.at(index)));
                p_V.insert(index);
                p_variableList.insert(p_newCurrentState.at(index).rhs());
                differs = true;  
             }
           }
         }

      } 
    }
    return !differs;
  }

  // Remove detected constant parameters if they contain summation variables
  void detectVar(int n) {
   // every process parameter...
   for(int i = 0; i != n; i++ ){
     // ...that is found to be constant (i.e. that is not in the set of
     // variable parameters)...
     if ( p_V.find(i) == p_V.end() ) {
       // ...and contains a summation variable...
       data_expression t = p_currentState.at(i).rhs();
       if ( recDetectVar(t, sum_vars) || recDetectVar(t, p_variableList) ) {
         // ...is actually a variable parameter
         p_V.insert(i);
       }
     }
   }
  }

  // Return whether or not a summation variable occurs in a data term
  bool recDetectVar(data_expression t, set<data_expression> &S) {
     bool b = false;
     if( gsIsDataVarId(t) && (S.find(t) != S.end()) ){
       b = true;
     }
     if ( gsIsDataAppl(t) ) {
       for(aterm_list::iterator i = ((aterm_appl) t).argument_list().begin(); i!= ((aterm_appl) t).argument_list().end();i++) {
         b = b || recDetectVar((aterm_appl) *i, S);
       }
     }
     return b;
  }

  // template for changing a vector into a list
  //
  template <typename Term>
  inline term_list<Term> vectorToList(vector<Term> y) { 
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
  inline term_list<Term> setToList(set<Term> y) { 
    term_list<Term> result;
    for(typename set<Term>::iterator i = y.begin(); i != y.end() ; i++)
      { 
        result = push_front(result,*i); 
      }
    return atermpp::reverse(result); 
  } 
  
  // Find all sorts which have a singleton domain
  //
  void findSingleton() {

    map< lpe::sort, int >     p_countSort;
    //set< lpe::sort > result;
    for(sort_list::iterator i = p_spec.sorts().begin(); i != p_spec.sorts().end() ; i++){
      p_countSort[*i] = 0;
      p_singletonSort.insert(*i);
    }
    
    for(function_list::iterator i= p_spec.constructors().begin() ; i != p_spec.constructors().end() ; i++){
      p_countSort[i->result_type()]++;
    }

    unsigned int n = p_singletonSort.size()+1;
    while (n != p_singletonSort.size()){
      n = p_singletonSort.size();
      for(sort_list::iterator i = p_spec.sorts().begin(); i != p_spec.sorts().end() ; i++){
        int b = 1;
        //if p_countSort[*i] == 0 then there are sorts declared which are never used!!!!
        assert(p_countSort[*i] != 0);
        if (p_countSort[*i] == 1){
          for(function_list::iterator j = p_spec.constructors().begin() ; j != p_spec.constructors().end() ;j++){
            if (j->result_type() == *i){
              for(sort_list::iterator k = j->input_types().begin() ; k != j->input_types().end() ; k++ ){
                b = max(p_countSort[*k], b);
              }
            }
          }
        if (b!=1) {p_singletonSort.erase(*i); p_countSort[*i] = b;}        
        } else {
          p_singletonSort.erase(*i);
        }
        ;    
      }
    }
    //p_singletonSort = result;
    
    if (p_verbose){
      cerr << "lpeconstelm: Sorts which have singleton constructors:"<< endl;
      for(set<lpe::sort>::iterator i = p_singletonSort.begin(); i != p_singletonSort.end(); i++){
        cerr <<"lpeconstelm:   "<< i->pp() << endl;
      }
      if (p_singletonSort.empty()) {
        cerr <<"lpeconstelm:   []"<< endl;      
      }
    }
  } 

  //---------------------------------------------------------------
  //---------------------   Debug begin  --------------------------
  //---------------------------------------------------------------
  inline void printNextState() {
    for(vector< data_assignment >::iterator i = p_nextState.begin(); i != p_nextState.end() ; i++ ){
      cerr << "[" << i->pp() << "]";
    
    }
    cerr << endl;
  }

  inline void printVar() {
    cerr << "lpeconstelm: Variable indices : {";
    set< int >::iterator i = p_V.begin();
    int j = 0;
    while(i != p_V.end()){
      if (*i ==j){
        cerr << j+1 << " ";
        i++;
       }
      j++;
    }
    cerr << "}" << endl;
  }

  inline void printState() {
    if ( p_S.size() > 0 )
    {
      cerr << "lpeconstelm:   [ ";
      for(set< int >::iterator i = p_S.begin(); i != (--p_S.end()) ; i++ ){
        if (!p_nosingleton){
          cerr << p_currentState[*i].pp() << ", ";
        } else {
          cerr << p_currentState[*i].pp() << ": " << p_currentState[*i].lhs().type().pp() << ", ";
        }  
      }
      cerr << p_currentState[*(--p_S.end())].pp() << " ]" << endl;
    }
  }
  
  inline void printCurrentState() {
    for(vector< data_assignment >::iterator i = p_currentState.begin(); i != p_currentState.end() ; i++ ){
      cerr << "[" << i->pp() << "]";
    
    }
    cerr << endl;
  }
  //---------------------------------------------------------------
  //---------------------   Debug end  --------------------------
  //---------------------------------------------------------------
public:

  // sorts with singleton constructors are removed from p_S
  // pre:  p_S is calculated && p_initAssignments is set
  // post: p_S contains the indices without the process parameters which have a singleton constructor
  void removeSingleton(int n)
  {
    bool empty = true;
    sort_list rebuild_sort = p_spec.sorts();
    findSingleton();
    if(p_verbose){
    cerr <<
      "lpeconstelm: Constant process parameters which are not substituted and " << endl <<
      "lpeconstelm: removed [--nosingleton]:" << endl;
    }
    for(int i = 0; i < n; i++)
    {
      if ( (p_V.find(i) == p_V.end()) &&
           (p_singletonSort.find(p_initAssignments[i].lhs().type()) != p_singletonSort.end())
         )
      {
        p_V.insert(i);
        if (p_verbose){
          cerr << "lpeconstelm:   " << p_initAssignments[i].lhs().pp() << " : " << p_initAssignments[i].lhs().type().pp() << endl;
          empty = false;
        }
      }
    }
    if (empty){
      cerr << "lpeconstelm:   []" << endl;
    }
  }

  // Writes an LPD to a file or sdtout
  // Substituting occurences of constant parameters with their constant value
  // and removing the constant process parameters from the list of process. 
  // Constant parameters (stored in p_S)
  //
  inline void output() {
    lpe::LPE p_lpe = p_spec.lpe();
    summand_list rebuild_summandlist;

    //Remove the summands that are never visited
    //
    if (p_reachable){
      rebuild_summandlist = setToList(p_visitedSummands); 
    } else {
      rebuild_summandlist = p_lpe.summands();
    }

    if (p_verbose) {
      cerr << "lpeconstelm: Number of summands of old LPD: " << p_lpe.summands().size() << endl;
      cerr << "lpeconstelm: Number of summands of new LPD: " <<  rebuild_summandlist.size() << endl;
    }

    set< data_variable > constantVar;
    for(set< int >::iterator i = p_S.begin(); i != p_S.end(); i++){
      constantVar.insert(p_initAssignments.at(*i).lhs());
    }

    vector< data_assignment > constantPP;
    for(set< int >::iterator i = p_S.begin(); i != p_S.end(); i++){
      constantPP.push_back(p_currentState.at(*i));
    }

    vector< data_variable > variablePPvar;
    for(set< int >::iterator i = p_V.begin(); i != p_V.end(); i++){
      variablePPvar.push_back(p_initAssignments.at(*i).lhs());
    }

    vector< data_expression > variablePPexpr;
    for(set< int >::iterator i = p_V.begin(); i != p_V.end(); i++){
      variablePPexpr.push_back(p_initAssignments.at(*i).rhs());
    }    

    //Remove process parameters in in summand
    // 
      
          summand_list rebuild_summandlist_no_cp;
    for(summand_list::iterator currentSummand = rebuild_summandlist.begin(); currentSummand != rebuild_summandlist.end(); currentSummand++){

      //construct new LPD_summand
      //
      LPE_summand tmp;

      //Remove constant process parameters from the summands assignments 
      //
      data_assignment_list rebuildAssignments; 
      for(data_assignment_list::iterator currentAssignment = currentSummand->assignments().begin(); currentAssignment != currentSummand->assignments().end() ; currentAssignment++){
        if( constantVar.find(currentAssignment->lhs() ) == constantVar.end()){
          rebuildAssignments  = push_front(rebuildAssignments, data_assignment(currentAssignment->lhs(), data_expression(p_substitute(currentAssignment->rhs(), constantPP ))));
        }
      } 
      
      //Rebuild actions
      //

      action_list rebuild_actions;
      for(action_list::iterator i = currentSummand->actions().begin(); i != currentSummand->actions().end() ; i++){
        data_expression_list argumentList;
        for(data_expression_list::iterator j = (i->arguments().begin()); j != i->arguments().end(); j++){
          argumentList = push_front(argumentList, data_expression(p_substitute(*j, constantPP)));
        }
        rebuild_actions = push_front(rebuild_actions, action( i -> name(), atermpp::reverse(argumentList)));
      };
      
      //Rewrite condition
      //
      data_expression rebuild_condition = currentSummand->condition();
      rebuild_condition = data_expression(p_substitute(rebuild_condition, constantPP));

      data_expression rebuild_time = currentSummand->time();
      if ( currentSummand->has_time() )
      {
	      rebuild_time = data_expression(p_substitute(rebuild_time, constantPP));
      }

      //LPD_summand(data_variable_list summation_variables, data_expression condition, 
      //            bool delta, action_list actions, data_expression time, 
      //            data_assignment_list assignments);    
      tmp = LPE_summand(currentSummand->summation_variables(), rebuild_condition, 
        currentSummand->is_delta(), atermpp::reverse(rebuild_actions) , rebuild_time, 
              atermpp::reverse(rebuildAssignments));
        rebuild_summandlist_no_cp = push_front(rebuild_summandlist_no_cp, tmp); 
    }
    
     set< data_variable > usedFreeVars;
     set< data_variable > foundVars;
     for(summand_list::iterator currentSummand = rebuild_summandlist_no_cp.begin(); currentSummand != rebuild_summandlist_no_cp.end(); currentSummand++){ 
       for(data_assignment_list::iterator i = currentSummand->assignments().begin(); i !=  currentSummand->assignments().end() ;i++){
         foundVars = getUsedFreeVars(aterm_appl(i->rhs()));
         for(set< data_variable >::iterator k = foundVars.begin(); k != foundVars.end(); k++){
           usedFreeVars.insert(*k);
        }
       }
     }
  
    //construct new specfication
    //
    //LPE(data_variable_list free_variables, data_variable_list process_parameters, 
    //  summand_list summands, action_list actions);
    lpe::LPE rebuild_lpe;
    rebuild_lpe = lpe::LPE(
      setToList(usedFreeVars),
      vectorToList(variablePPvar), 
      rebuild_summandlist_no_cp,
      p_lpe.actions()
    );
     
     //cerr <<  p_spec.initial_free_variables() << endl;
     
    set< data_variable > initial_free_variables;
    usedFreeVars.empty();
    for(vector< data_expression >::iterator i = variablePPexpr.begin(); i != variablePPexpr.end(); i++){
         foundVars = getUsedFreeVars(aterm_appl(*i));
         for(set< data_variable >::iterator k = foundVars.begin(); k != foundVars.end(); k++){
           initial_free_variables.insert(*k); 
         }           
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
      setToList(initial_free_variables), 
      vectorToList(variablePPvar), 
      vectorToList(variablePPexpr)
    );
    
    assert(gsIsSpecV1((ATermAppl) rebuild_spec));

    //cerr << p_lpe.pp() << endl;
    if (p_outputfile.empty()){
      //if(!p_verbose){
      //  assert(!p_verbose);
        writeStream(rebuild_spec);
      //};
    }
    else {
      if(!rebuild_spec.save(p_outputfile)) {
         cerr << "lpeconstelm: Unsuccessfully written outputfile: " << p_outputfile << endl;
      }
    } 
  }
  
  // Set output file
  //
  inline void setSaveFile(string x) {
    p_outputfile = x;
  }

  // Print the set of constant process parameters
  //  
  inline void printSet() {
    cerr << "lpeconstelm: Constant indices: { ";
    set< int >::iterator i = p_S.begin();
    int j = 0;
    while(i != p_S.end()){
      if (*i ==j){
        cerr << j+1 << " ";
        i++;
       }
      j++;
    }
    cerr << "}"<< endl;
  }
  
  // Loads an LPD from file
  // returns true if succeeds
  //  
  inline bool loadFile(string filename) {
    p_filenamein = filename;
    if (!p_spec.load(p_filenamein))
    {
      cerr << "lpeconstelm: error: could not read input file '" << filename << "'" << endl;
      return false;
    } 
    //LPD x = p_spec.lpe(); 
    //cerr << x.pp() << endl;
    //p_spec.save("/scratch/dump.lpe");
    //assert(false);
    return true;
  }
  
  // Reads an LPD from stdin
  // returns true if succeeds
  //  
  inline bool readStream() {
    ATermAppl z = (ATermAppl) ATreadFromFile(stdin);
    if (z == NULL){
      cerr << "lpeconstelm: Could not read LPD from stdin"<< endl;
      return false;
    };
    if (!gsIsSpecV1(z)){
      cerr << "lpeconstelm: Stdin does not contain an LPD" << endl;
      return false;
    }
    p_spec = specification(z);
    //cerr << p_spec.lpe().pp() << endl;
    return true;
  }

  // Writes file to stdout
  //
  void writeStream(specification newSpec) {
    assert(gsIsSpecV1((ATermAppl) newSpec));
    ATwriteToBinaryFile(aterm(newSpec) , stdout);
  }

  // Sets verbose option
  // Note: Has to be set
  //
  inline void setVerbose(bool b) {
    p_verbose = b;
  }
  
  // Sets debug option
  // Note: Has to be set
  //
  inline void setDebug(bool b) {
    p_debug = b;
  }
  
  // Sets no singleton option
  // Note: Has to be set
  //  
  inline void setNoSingleton(bool b) {
    p_nosingleton = b;
  }

  // Sets all conditions to true
  // Note: Has to be set
  //  
  inline void setAllTrue(bool b) {
    p_alltrue = b;
  }
  
  // Sets the option if not inspected summands have to removed 
  // Note: Has to be set
  //  
  inline void setReachable(bool b) {
    p_reachable = b;
  }
  
  // Prints the data_variable which are constant
  //  
  void printSetVar() {
    printState();
  }  
  
  // The constelm filter
  //
  void filter() {

    //---------------------------------------------------------------
    //---------------------   Init begin   --------------------------
    //---------------------------------------------------------------
  
    bool    same        ;
    bool    foundFake = true;
    int     counter  = 0;
    int     cycle    = 0;
    p_newVarCounter  = 0;
    
    lpe::LPE p_lpe          = p_spec.lpe();
    gsRewriteInit(gsMakeDataEqnSpec(aterm_list(p_spec.equations())), GS_REWR_INNER); 

    for(data_assignment_list::iterator i = p_spec.initial_assignments().begin(); i != p_spec.initial_assignments().end() ; i++ ){
      p_lookupIndex[i->lhs()] = counter;
//      p_currentState.push_back(data_assignment(i->lhs(), data_expression(rewrite(i->rhs()))));
      data_assignment da(i->lhs(),data_expression(rewrite(i->rhs())));
      ATtablePut(safeguard,aterm(da),aterm(da));
      p_currentState.push_back(da);
      p_lookupDataVarIndex[counter] = i->lhs();
      counter++;
    }
    p_nextState       = p_currentState;
    p_newCurrentState = p_currentState;
    p_initAssignments = p_currentState;

    for (data_variable_list::iterator di = p_spec.initial_free_variables().begin(); di != p_spec.initial_free_variables().end(); di++){
      p_freeVarSet.insert(di->to_expr());
    }
    for (data_variable_list::iterator di = p_lpe.free_variables().begin(); di != p_lpe.free_variables().end(); di++){
      p_freeVarSet.insert(di->to_expr());
    } 
    
    // Make a set containing all summation variables (for detectVar)
    summand_list sums = p_spec.lpe().summands();
    summand_list::iterator sums_b = sums.begin();
    summand_list::iterator sums_e = sums.end();
    for (summand_list::iterator i = sums_b; i != sums_e; i++)
    {
//      sum_vars.insert(i->summation_variables().begin(),i->summation_variables().end());
      for (data_variable_list::iterator j = i->summation_variables().begin(); j != i->summation_variables().end(); j++)
      {
        sum_vars.insert(data_expression(*j));
      }
    }

    int n = p_spec.initial_assignments().size();

    while (foundFake){
      foundFake = false;
      //---------------------------------------------------------------
      //---------------------   Init end     --------------------------
      //---------------------------------------------------------------
  
      //---------------------------------------------------------------
      //---------------------   Body begin   --------------------------
      //---------------------------------------------------------------  
  
      same = false;  
      while (!same){
        same = true;
        if (p_verbose){
          //cerr << "Cycle:" << cycle++ << endl;
          cerr << "lpeconstelm: Cycle " << cycle++ << ": ";
        }
        //int summandnr = 1;
        for(summand_list::iterator currentSummand = p_lpe.summands().begin(); currentSummand != p_lpe.summands().end() ;currentSummand++ ){
          if ( (p_visitedSummands.find(*currentSummand) != p_visitedSummands.end()) || (conditionTest(currentSummand->condition()))) {
            if(p_verbose){
              //cerr << "  Summand: "<< summandnr++ << endl;
              cerr << ".";
            }
            p_visitedSummands.insert(*currentSummand); 
            //----------          Debug
            //          printCurrentState();
            calculateNextState(currentSummand->assignments());
            //----------          Debug  
            //          printCurrentState();
            same = cmpCurrToNext() && same ; 
            //ischanged = ischanged || !cmpCurrToNext();
            //if (!same) {break;}                                           //Break reduces time to complete; need to find out when to brake
          }
        }
        if (p_verbose) cerr << endl;
        p_currentState = p_newCurrentState;
      }

      //---------------------------------------------------------------
      //---------------------   Body end   ----------------------------
      //---------------------------------------------------------------

      // remove detected constants in case the value contains summation
      // variables or non constant parameter variables
      {
        int n = p_V.size();
        detectVar(p_lpe.process_parameters().size());
	int diff = p_V.size()-n;
        if ( p_verbose )
        {
          if ( diff == 1 )
          {
            cerr << "lpeconstelm: reset 1 parameter to variable because its value contained summation variables" << endl;
          } else if ( diff > 1 )
	  {
            cerr << "lpeconstelm: reset " << diff << " parameters to variable because their values contained summation variables" << endl;
          }
        }
      }

      //---------------------FeeVar aftercheck-------------------------
      //
      //                      |
      //                      |
      // Covers:              V
      //   proc: P(a,b)  = (_,a)+
      //                   (1,8)
      //                     
      //   init: P(_,_)
      // 
      //                                Each _ is a unique FreeVariable
      // The arrow is detected with the FeeVar aftercheck
    
      if(!p_freeVarSet.empty()){
        if (p_verbose){
          cerr << "lpeconstelm: Free Variable checkup:" << endl;
        }
      
        int n = p_V.size();
        for(set< LPE_summand>::iterator i = p_visitedSummands.begin(); i != p_visitedSummands.end() ; i++){
          calculateNextState(i->assignments());
          cmpCurrToNext();
        }

        p_currentState = p_newCurrentState;
        if (p_verbose){
          cerr << "lpeconstelm:   Detected "<<p_V.size() - n << " fake constant process parameters" <<endl;
          foundFake = ((p_V.size() - n) != 0);
        }         

      }
      
    }
    
    //---------------------------------------------------------------

    //Singleton sort process parameters
    //
    if(p_nosingleton){
       int n = p_V.size();
       removeSingleton(p_lpe.process_parameters().size());
       int diff = p_V.size()-n;
       if ( p_verbose )
       {
         if ( diff > 1 )
         {
           cerr << "lpeconstelm: " << diff << " constant parameters are not removed because their sorts contain only one element" << endl;
         } else if ( diff == 1 )
         {
           cerr << "lpeconstelm: 1 constant parameter is not removed because its sort contains only one element" << endl;
         }
       }
    }    
    
    //---------------------------------------------------------------
    // Construct S    
    //
    set< int > S;
    n = p_lpe.process_parameters().size(); 
    for(int j=0; j < n ; j++){
      S.insert(j);
    };
    set_difference(S.begin(), S.end(), p_V.begin(), p_V.end(), inserter(p_S, p_S.begin()));
    
    if (p_verbose){
      cerr << "lpeconstelm: Number of removed process parameters: "<< p_S.size() << endl ;//printSet(); 
      printSetVar();  
    }
  }
  
  // Gets the version of the tool
  //    
  inline string getVersion() {
    return (VERSION);
  }
};

void parse_command_line(int ac, char** av, ConstelmObj &constelm) {
  po::options_description description;

  description.add_options()
    ("no-singleton", "do not remove sorts consisting of a single element")
    ("no-condition", "all summand conditions are set true (faster)")
    ("no-reachable", "does not remove summands which are not visited")
    ("verbose,v",    "turn on the display of short intermediate messages")
    ("debug,d",      "turn on the display of detailed intermediate messages")
    ("version",      "display version information")
    ("help,h",       "display this help")
  ;
        
  po::options_description hidden("Hidden options");

  hidden.add_options()
     ("file_names", po::value< vector<string> >(), "input/output files")
  ;
        
  po::options_description cmdline_options;
  cmdline_options.add(description).add(hidden);
        
  po::options_description visible("Allowed options");
  visible.add(description);
        
  po::positional_options_description p;
  p.add("file_names", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(ac, av).options(cmdline_options).positional(p).run(), vm);
  po::notify(vm);
     
  if (vm.count("help")) {
    std::cerr << "Usage: "<< av[0] << " [OPTION]... [INFILE [OUTFILE]] \n"
              << "Removes constant process parameters from the LPD read from standard output or INFILE." << std::endl
              << "By default the result is written to standard output, and otherwise to OUTFILE." << endl
              << endl
              << description;

    exit (0);
  }
        
  if (vm.count("version")) {
    std::cerr << VERSION << " (revision " << REVISION << ")" << endl;

    exit (0);
  }

  constelm.setVerbose(0 < vm.count("verbose"));
  constelm.setDebug(0 < vm.count("debug"));
  constelm.setNoSingleton(0 < vm.count("no-singleton"));
  constelm.setAllTrue(0 < vm.count("no-condition"));
  constelm.setReachable(0 == vm.count("no-reachable"));

  if (vm.count("file_names")){
    file_names = vm["file_names"].as< std::vector< std::string > >();
  }

  if (file_names.size() == 0){
    /* Read from standard input */
    if (!constelm.readStream()) {
      exit (1);
    }
  }
  else if (2 < file_names.size()) {
    cerr << "lpeconstelm: Specify only INPUT and/or OUTPUT file (too many arguments)."<< endl;

    exit (0);
  }
  else {
    if (!constelm.loadFile(file_names[0])) {
      exit (1);
    }

    if (file_names.size() == 2) {
      constelm.setSaveFile(file_names[1]);
    }
  }
}

#ifdef ENABLE_SQUADT_CONNECTIVITY
/* Constants for identifiers of options and objects */
const unsigned int lpd_file_for_input  = 0;
const unsigned int lpd_file_for_output = 1;

/* Communicate the basic configuration display, and wait until the ok button was pressed */
sip::layout::tool_display::sptr set_basic_configuration_display(sip::tool::communicator& tc) {
  using namespace sip;
  using namespace sip::layout;
  using namespace sip::layout::elements;

  layout::tool_display::sptr display(new layout::tool_display);

  /* Create and add the top layout manager */
  layout::manager::aptr layout_manager = layout::horizontal_box::create();

  /* First column */
  layout::vertical_box* column = new layout::vertical_box();

  column->add(new checkbox("remove single element sorts", true), layout::left);
  column->add(new checkbox("remove summands that are not visited", true), layout::left);
  column->add(new checkbox("take summand conditions into account", true), layout::left);

  button* okay_button = new button("OK");

  column->add(okay_button, layout::right);

  /* Attach columns*/
  layout_manager->add(column, margins(0,5,0,5));

  display->set_top_manager(layout_manager);

  tc.send_display_layout(display);

  /* Wait until the ok button was pressed */
  okay_button->await_change();

  return (display);
}

/* Extracts the configuration from the currently set display layout */
void extract_configuration(sip::layout::tool_display& d, sip::configuration& c) {
  std::string input_file_name  = c.get_object(lpd_file_for_input)->get_location();

  /* Add output file to the configuration */
  c.add_output(lpd_file_for_output, "lpe", input_file_name + ".lpe");
}

/* Checks whether the configuration is complete and valid */
bool validate_configuration(sip::configuration& c) {
  bool valid  = true;

  /* Should contain a file name of an LPD that is to be read as input */
  valid &= c.object_exists(lpd_file_for_input);
  valid &= c.object_exists(lpd_file_for_output);

  if (valid) {
    std::string input_file_name  = c.get_object(lpd_file_for_input)->get_location();
    std::string output_file_name = c.get_object(lpd_file_for_output)->get_location();

    valid = input_file_name != output_file_name;
  }

  return (valid);
}

/*
 * Realises the configuration
 *
 * Precondition: the configuration is valid
 **/
void realise_configuration(sip::tool::communicator& tc, ConstelmObj& constelm, sip::configuration& c) {
  std::string input_file_name  = c.get_object(lpd_file_for_input)->get_location();
  std::string output_file_name = c.get_object(lpd_file_for_output)->get_location();

  if (!constelm.loadFile(input_file_name)) {
    tc.send_error_report("Error reading input!");

    exit(1);
  }

  constelm.setSaveFile(output_file_name);
}
#endif

int main(int ac, char** av) {
  ATerm       bottom;
  ConstelmObj constelm;

  ATinit(ac,av,&bottom);
  
  gsEnableConstructorFunctions();

#ifdef ENABLE_SQUADT_CONNECTIVITY
  sip::tool::communicator tc;

  /* Get tool capabilities in order to modify settings */
  sip::tool::capabilities& cp = tc.get_tool_capabilities();

  /* The tool has only one main input combination it takes an LPE and then behaves as a reporter */
  cp.add_input_combination(lpd_file_for_input, "Transformation", "lpe");

  /* On purpose we do not catch exceptions */
  if (tc.activate(ac,av)) {
    bool valid = false;

    std::string input_file_name;
    std::string output_file_name;

    /* Static configuration cycle (phase 1: obtain input combination) */
    while (!valid) {
      /* Wait for configuration data to be send (either a previous configuration, or only an input combination) */
      sip::configuration::ptr configuration = tc.await_configuration();

      /* Validate configuration specification, should contain a file name of an LPD that is to be read as input */
      valid  = configuration.get() != 0;
      valid &= configuration->object_exists(lpd_file_for_input);

      if (valid) {
        std::string input_file_name = configuration->get_object(lpd_file_for_input)->get_location();

        /* An object with the correct id exists, assume the URI is relative (i.e. a file name in the local file system) */
        constelm.loadFile(input_file_name);

        tc.set_configuration(configuration);
      }
      else {
        tc.send_error_report("Invalid input combination!");

        exit(1);
      }
    }

    /* Draw a configuration layout in the tool display */
    sip::layout::tool_display::sptr display = set_basic_configuration_display(tc);

    /* Clean the display */
    tc.clear_display();

    /* Extract configuration from the current state of the display and use it to update the configuration of tc */
    extract_configuration(*display, tc.get_configuration());
    
    /* Static configuration cycle (phase 2: gather user input) */
    if (!validate_configuration(tc.get_configuration())) {
      /* Wait for configuration data to be send (either a previous configuration, or only an input combination) */
      tc.send_error_report("Fatal error: the configuration is invalid");

      exit(1);
    }

    /* Realise the */
    realise_configuration(tc, constelm, tc.get_configuration());

    /* Send the controller the signal that we're ready to rumble (no further configuration necessary) */
    tc.send_accept_configuration();

    /* Wait for start message */
    tc.await_message(sip::send_signal_start);
  }
  else {
    parse_command_line(ac,av,constelm);
  }
#else
  parse_command_line(ac,av,constelm);
#endif

  constelm.filter();
  constelm.output(); 

  gsRewriteFinalise();

  return 0;
}
