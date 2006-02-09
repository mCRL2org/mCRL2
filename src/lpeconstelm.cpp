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

using namespace std;
using namespace lpe;
using namespace atermpp;

namespace po = boost::program_options;
po::variables_map vm;

//Constanten
//Private:
  #define p_version "lpeconstelm 0.5.2";
//Public:

class ConstelmObj
{
private:
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
  set<data_variable>          sum_vars;
  
  void getDatVarRec(aterm_appl t)
  {
    if(gsIsDataVarId(t) && (p_freeVarSet.find(data_variable(t).to_expr()) != p_freeVarSet.end())){
      p_foundFreeVars.insert(t);
    };
    for(aterm_list::iterator i = t.argument_list().begin(); i!= t.argument_list().end();i++) {
      getDatVarRec((aterm_appl) *i);
    } 
  } 
  
  // Returns a vector in which each element is a AtermsAppl (DataVarID)  
  //
  set< data_variable > inline getUsedFreeVars(aterm_appl input)
  {
    p_foundFreeVars.clear();
    getDatVarRec(input);
    return p_foundFreeVars;
  }
  
  // Rewrites an ATerm to a normal form
  //
  // pre : input is an AtermAppl
  // post: result is an ATermAppl in normal form
  inline ATermAppl rewrite(ATermAppl t)
  { 
    return gsRewriteTerm(t);
  }

  // Subsitutes a vectorlist of data assignements to a ATermAppl 
  //
  inline ATermAppl p_substitute(ATermAppl t, vector< data_assignment > &y )
  { 
    for(vector< data_assignment >::iterator i = y.begin() ; i != y.end() ; i++){
      RWsetVariable(aterm(i->lhs()) , aterm(i->rhs()));
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
  inline void calculateNextState(data_assignment_list assignments)
  {
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
  inline bool inFreeVarList(data_expression dexpr)
  {
    return (p_freeVarSet.find(dexpr) != p_freeVarSet.end());
  }

  // Creates an unique expression:
  // these date_expressions are used to model that a process parameter has a 
  // value which is not constant
  //
  inline data_assignment newExpression(data_assignment ass)
  {
    char buffer [99];
    sprintf(buffer, "%s^%d", ass.lhs().name().c_str(), p_newVarCounter++);
    data_variable w(buffer, ass.lhs().type() );
    data_assignment a(ass.lhs() , w.to_expr());
    return a;
  }
  
  // returns whether two data_expressions are equal
  //
  inline bool compare(data_expression x, data_expression y)
  {
    return (x==y);
  }
  
  // returns whether the given data_expression is false
  //  
  inline bool conditionTest(data_expression x)
  {
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
  inline bool cmpCurrToNext()
  {
    bool differs = false;
    for(vector< data_assignment>::iterator i= p_currentState.begin(); i != p_currentState.end() ;i++){
      int index = p_lookupIndex[i->lhs()]; 
      if (p_V.find(index) == p_V.end()) { 
        if (inFreeVarList(i->rhs())) { 
          if (p_variableList.find(p_nextState.at(index).rhs()) != p_variableList.end()){
            p_V.insert(p_lookupIndex[i->lhs()]); 
            //----------          Debug
            //            cerr << "\033[34m OLD:    "<< i->pp() << endl;
            //            cerr << "\033[32m NEW:    "<< p_nextState.at(index).pp() << endl;
            //            cerr << "\033[0m";
            //----------          Debug
          };
          p_newCurrentState.at(index) = p_nextState.at(index) ;
          p_currentState.at(index) = p_nextState.at(index);  
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
  void detectVar(int n)
  {
   // every process parameter...
   for(int i = 0; i != n; i++ ){
     // ...that is found to be constant (i.e. that is not in the set of
     // variable parameters)...
     if ( p_V.find(i) == p_V.end() ) {
       // ...and contains a summation variable...
       data_expression t = p_currentState.at(i).rhs();
       if ( recDetectVar(t, sum_vars) ) {
         // ...is actually a variable parameter
         p_V.insert(i);
       }
     }
   }
  }

  // Return whether or not a summation variable occurs in a data term
  bool recDetectVar(data_expression t, set<data_variable> &S)
  {
     bool b = false;
     if( gsIsDataVarId(t) && (S.find(data_variable((ATermAppl) t)) != S.end()) ){
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
  
  // Find all sorts which have a singleton domain
  //
  void findSingleton()
  {

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
  inline void printNextState()
  {
    for(vector< data_assignment >::iterator i = p_nextState.begin(); i != p_nextState.end() ; i++ ){
      cerr << "[" << i->pp() << "]";
    
    }
    cerr << endl;
  }

  void inline printVar()
  {
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

  void inline printState()
  {
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
  
  void inline printCurrentState()
  {
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

  // Writes an LPE to a file or sdtout
  // Substituting occurences of constant parameters with their constant value
  // and removing the constant process parameters from the list of process. 
  // Constant parameters (stored in p_S)
  //
  void inline output()
  {
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
      cerr << "lpeconstelm: Number of summands of old LPE: " << p_lpe.summands().size() << endl;
      cerr << "lpeconstelm: Number of summands of new LPE: " <<  rebuild_summandlist.size() << endl;
    }

    set< data_variable > constantVar;
    for(set< int >::iterator i = p_S.begin(); i != p_S.end(); i++){
      constantVar.insert(p_initAssignments.at(*i).lhs());
    }

    vector< data_assignment > constantPP;
    for(set< int >::iterator i = p_S.begin(); i != p_S.end(); i++){
      constantPP.push_back(p_currentState.at(*i));
    }
    
    vector< data_assignment > variablePP;
    for(set< int >::iterator i = p_V.begin(); i != p_V.end(); i++){
      variablePP.push_back(p_currentState.at(*i));
    }
    
    vector< data_variable > variablePPvar;
    for(set< int >::iterator i = p_V.begin(); i != p_V.end(); i++){
      variablePPvar.push_back(p_currentState.at(*i).lhs());
    }

    vector< data_expression > variablePPexpr;
    for(set< int >::iterator i = p_V.begin(); i != p_V.end(); i++){
      variablePPexpr.push_back(p_initAssignments.at(*i).rhs());
    }    

    //Remove process parameters in in summand
    // 
      
          summand_list rebuild_summandlist_no_cp;
    for(summand_list::iterator currentSummand = rebuild_summandlist.begin(); currentSummand != rebuild_summandlist.end(); currentSummand++){

      //construct new LPE_summand
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

      //LPE_summand(data_variable_list summation_variables, data_expression condition, 
      //            bool delta, action_list actions, data_expression time, 
      //            data_assignment_list assignments);    
      tmp = LPE_summand(currentSummand->summation_variables(), rebuild_condition, 
        currentSummand->is_delta(), atermpp::reverse(rebuild_actions) , currentSummand->time(), 
              atermpp::reverse(rebuildAssignments));
        rebuild_summandlist_no_cp = push_front(rebuild_summandlist_no_cp, tmp); 
    }
    
    summand_list rebuild_summandlist2;
    for(summand_list::iterator i = rebuild_summandlist_no_cp.begin() ; i != rebuild_summandlist_no_cp.end() ; i++){
      rebuild_summandlist2 = push_front(rebuild_summandlist2, LPE_summand(p_substitute(*i, constantPP ))); 
    }
    
     set< data_variable > usedFreeVars;
     set< data_variable > foundVars;
     for(summand_list::iterator currentSummand = rebuild_summandlist2.begin(); currentSummand != rebuild_summandlist2.end(); currentSummand++){ 
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
      rebuild_summandlist2,
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
    
    if (p_outputfile.size() == 0){
      //if(!p_verbose){
      //  assert(!p_verbose);
        writeStream(rebuild_spec);
      //};
    } else {
      if(!rebuild_spec.save(p_outputfile)){
         cerr << "lpeconstelm: Unsuccessfully written outputfile: " << p_outputfile << endl;
      };
    } 
  }
  
  // Set output file
  //
  void inline setSaveFile(string x)
  {
    p_outputfile = x;
  }

  // Print the set of constant process parameters
  //  
  void inline printSet()
  {
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
  
  // Loads an LPE from file
  // returns true if succeeds
  //  
  bool inline loadFile(string filename)
  {
    p_filenamein = filename;
    if (!p_spec.load(p_filenamein))
    {
      cerr << "lpeconstelm: error: could not read input file '" << filename << "'" << endl;
      return false;
    } 
    //LPE x = p_spec.lpe(); 
    //cerr << x.pp() << endl;
    //p_spec.save("/scratch/dump.lpe");
    //assert(false);
    return true;
  }
  
  // Reads an LPE from stdin
  // returns true if succeeds
  //  
  bool inline readStream()
  {
    ATermAppl z = (ATermAppl) ATreadFromFile(stdin);
    if (z == NULL){
      cerr << "lpeconstelm: Could not read LPE from stdin"<< endl;
      return false;
    };
    if (!gsIsSpecV1(z)){
      cerr << "lpeconstelm: Stdin does not contain an LPE" << endl;
      return false;
    }
    p_spec = specification(z);
    //cerr << p_spec.lpe().pp() << endl;
    return true;
  }

  // Writes file to stdout
  //
  void writeStream(specification newSpec)
  {
    assert(gsIsSpecV1((ATermAppl) newSpec));
    ATwriteToBinaryFile(aterm(newSpec) , stdout);
  }

  // Sets verbose option
  // Note: Has to be set
  //
  void inline setVerbose(bool b)
  {
    p_verbose = b;
  }
  
  // Sets debug option
  // Note: Has to be set
  //
  void inline setDebug(bool b)
  {
    p_debug = b;
  }
  
  // Sets no singleton option
  // Note: Has to be set
  //  
  void inline setNoSingleton(bool b)
  {
    p_nosingleton = b;
  }

  // Sets all conditions to true
  // Note: Has to be set
  //  
  void inline setAllTrue(bool b)
  {
    p_alltrue = b;
  }
  
  // Sets the option if not inspected summands have to removed 
  // Note: Has to be set
  //  
  void inline setReachable(bool b)
  {
    p_reachable = b;
  }
  
  // Prints the data_variable which are constant
  //  
  void printSetVar()
  {
    printState();
  }  
  
  // The constelm filter
  //
  void filter()
  {

    //---------------------------------------------------------------
    //---------------------   Init begin   --------------------------
    //---------------------------------------------------------------
  
    bool    same        ;
    bool    foundFake = true;
    int     counter  = 0;
    int     cycle    = 0;
    p_newVarCounter  = 0;
    
    lpe::LPE p_lpe          = p_spec.lpe();
    gsRewriteInit(gsMakeDataEqnSpec(aterm_list(p_spec.equations())), GS_REWR_INNER3); 

    for(data_assignment_list::iterator i = p_spec.initial_assignments().begin(); i != p_spec.initial_assignments().end() ; i++ ){
      p_lookupIndex[i->lhs()] = counter;
      p_currentState.push_back(data_assignment(i->lhs(), data_expression(rewrite(i->rhs()))));
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
      sum_vars.insert(i->summation_variables().begin(),i->summation_variables().end());
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
      // variables
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
   
    ConstelmObj obj;

    try {
      po::options_description desc;
      desc.add_options()
        ("help,h",      "display this help")
        ("verbose,v",   "turn on the display of short intermediate messages")
        ("debug,d",    "turn on the display of detailed intermediate messages")
        ("version",     "display version information")
        ("no-singleton", "do not remove sorts consisting of a single element")
        ("no-condition", "all summand conditions are set true (faster)")
        ("no-reachable", "does not remove summands which are not visited")
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
      cerr << "Remove constant process parameters from the LPE in INFILE, and write the result" << endl;
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

    if (vm.count("no-singleton")) {
      obj.setNoSingleton(true);
          } else {
            obj.setNoSingleton(false);
          }

    if (vm.count("no-condition")) {
      obj.setAllTrue(true);
          } else {
            obj.setAllTrue(false);
          }

    if (vm.count("no-reachable")) {
      obj.setReachable(false);
          } else {
            obj.setReachable(true);
          }

    if (vm.count("INFILE")){
      filename = vm["INFILE"].as< vector<string> >();
          }

    //printf("%d",filename.size());           
          if (filename.size() == 0){
            if (!obj.readStream()){return 1;}
          }

    if (filename.size() > 2){
      cerr << "lpeconstelm: Specify only INPUT and/or OUTPUT file (Too many arguments)."<< endl;
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
    
    gsRewriteFinalise();

    }
    catch(exception& e){
      cerr << "lpeconstelm: " << e.what() << "\n";
      return 1;
    }    
    
    return 0;
}
