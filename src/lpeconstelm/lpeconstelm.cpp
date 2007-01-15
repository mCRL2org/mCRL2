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

//Lowlevel
#include <libprint_c.h>
#include "libstruct.h"
#include "liblowlevel.h"

//Rewriter
#include "librewrite.h"

//Aterm
#include "atermpp/aterm.h"

//LPE framework
#include "lpe/specification.h"

// Squadt protocol interface and utility pseudo-library
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <utility/squadt_utility.h>
#endif

using namespace lpe;
using namespace atermpp;

const char* version = "0.5.2";

class lpeConstElm {
  private:
    ATermTable                            safeguard;
    std::string                           p_inputfile;
    std::string                           p_outputfile;
    std::vector< lpe::data_assignment >   p_currentState;
    std::vector< lpe::data_assignment >   p_newCurrentState;
    std::vector< lpe::data_assignment >   p_nextState;
    std::vector< lpe::data_assignment >   p_initAssignments;
    std::map< lpe::data_variable, int  >  p_lookupIndex;
    std::map< int, lpe::data_variable >   p_lookupDataVarIndex;
    std::set< lpe::data_expression >      p_freeVarSet; 
    std::set< int >                       p_V; 
    std::set< int >                       p_S;
    std::set< lpe::LPE_summand >          p_visitedSummands;
    std::set< lpe::data_expression >      p_variableList; 
    int                                   p_newVarCounter;
    bool                                  p_verbose;
    bool                                  p_debug;
    bool                                  p_nosingleton;
    bool                                  p_alltrue;
    bool                                  p_reachable; 
    std::string                           p_filenamein;
    lpe::specification                    p_spec;
    std::set< lpe::sort >                 p_singletonSort;
    Rewriter*                             rewr;
    
    //Only used by getDataVarIDs  
    std::set< lpe::data_variable >        p_foundFreeVars;       
 
    //Only used by detectVar
    std::set< lpe::data_expression >      sum_vars;
 
  public:

    lpeConstElm();

    ~lpeConstElm();

  private:

    void getDatVarRec(aterm_appl t);
    std::set< lpe::data_variable > getUsedFreeVars(aterm_appl input);
    ATermAppl rewrite(ATermAppl t);
    ATermAppl p_substitute(ATermAppl t, std::vector< lpe::data_assignment > &y );
    void calculateNextState(lpe::data_assignment_list assignments);
    bool inFreeVarList(lpe::data_expression dexpr);
    lpe::data_assignment newExpression(lpe::data_assignment ass);
    bool compare(lpe::data_expression x, lpe::data_expression y);
    bool cmpCurrToNext();
    bool conditionTest(lpe::data_expression x);
    void detectVar(int n);
    bool recDetectVar(lpe::data_expression t, std::set<lpe::data_expression> &S);
    template <typename Term>
    atermpp::term_list<Term> vectorToList(std::vector<Term> y);
    template <typename Term>
    atermpp::term_list<Term> setToList(std::set<Term> y);
    void findSingleton();

    void printNextState();
    void printVar();
    void printState();
    void printCurrentState();

  public:

    void removeSingleton(int n);
    void output();
    void setSaveFile(std::string const& x);
    void printSet();
    bool loadFile(std::string const& filename);
    bool readStream();
    void writeStream(lpe::specification newSpec);
    void setVerbose(bool b);
    void setDebug(bool b);
    void setNoSingleton(bool b);
    void setAllTrue(bool b);
    void setReachable(bool b);
    void printSetVar();
    std::string getVersion();
    void filter();
};

// Squadt protocol interface and utility pseudo-library
#ifdef ENABLE_SQUADT_CONNECTIVITY

class squadt_interactor : public squadt_tool_interface {

  private:

    static const char*  lpd_file_for_input;  ///< file containing an LPE that can be imported
    static const char*  lpd_file_for_output; ///< file used to write the output to

    static const char*  option_remove_single_element_sorts;
    static const char*  option_remove_unvisited_summands;
    static const char*  option_ignore_summand_conditions;

  public:

    /** \brief configures tool capabilities */
    void set_capabilities(sip::tool::capabilities&) const;

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(sip::configuration&);

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(sip::configuration const&) const;

    /** \brief performs the task specified by a configuration */
    bool perform_task(sip::configuration&);
};

const char* squadt_interactor::lpd_file_for_input  = "lpd_in";
const char* squadt_interactor::lpd_file_for_output = "lpd_out";

const char* squadt_interactor::option_remove_single_element_sorts = "remove_single_element_sorts";
const char* squadt_interactor::option_remove_unvisited_summands   = "remove_unvisited_summands";
const char* squadt_interactor::option_ignore_summand_conditions   = "ignore_summand_conditions";

void squadt_interactor::set_capabilities(sip::tool::capabilities& c) const {
  c.add_input_combination(lpd_file_for_input, sip::mime_type("lpe"), sip::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(sip::configuration& c) {
  using namespace sip;
  using namespace sip::layout;
  using namespace sip::layout::elements;

  /* Create and add the top layout manager */
  layout::manager::aptr top = layout::horizontal_box::create();

  /* First column */
  layout::vertical_box* column = new layout::vertical_box();

  checkbox* remove_single_element_sorts = new checkbox("remove single element sorts", true);
  checkbox* remove_unvisited_summands   = new checkbox("remove summands that are not visited", true);
  checkbox* ignore_summand_conditions   = new checkbox("take summand conditions into account", true);

  column->add(remove_single_element_sorts, layout::left);
  column->add(remove_unvisited_summands, layout::left);
  column->add(ignore_summand_conditions, layout::left);

  button* okay_button = new button("OK");

  column->add(okay_button, layout::right);

  /* Attach columns*/
  top->add(column, margins(0,5,0,5));

  send_display_layout(top);

  /* Wait until the ok button was pressed */
  okay_button->await_change();

  if (c.is_fresh()) {
    if (!c.output_exists(lpd_file_for_output)) {
      /* Add output file to the configuration */
      c.add_output(lpd_file_for_output, sip::mime_type("lpe"), c.get_output_name(".lpe"));
    }

    /* Values for the options */
    if (remove_single_element_sorts->get_status()) {
      c.add_option(option_remove_single_element_sorts);
    }
    if (remove_unvisited_summands->get_status()) {
      c.add_option(option_remove_unvisited_summands);
    }
    if (ignore_summand_conditions->get_status()) {
      c.add_option(option_ignore_summand_conditions);
    }
  }
}

bool squadt_interactor::check_configuration(sip::configuration const& c) const {
  bool result = true;

  result &= c.input_exists(lpd_file_for_input);
  result &= c.output_exists(lpd_file_for_output);

  return (result);
}

bool squadt_interactor::perform_task(sip::configuration& c) {
  lpeConstElm constelm;

  /* Set with options from the current configuration object */
  constelm.setNoSingleton(c.option_exists(option_remove_single_element_sorts));
  constelm.setReachable(c.option_exists(option_remove_unvisited_summands));
  constelm.setAllTrue(c.option_exists(option_remove_unvisited_summands));

  send_hide_display();

  if (constelm.loadFile(c.get_input(lpd_file_for_input).get_location())) {
    constelm.setSaveFile(c.get_output(lpd_file_for_output).get_location());

    constelm.filter();
    constelm.output(); 

    return (true);
  }
  else {
    send_error("Could not read `" + c.get_input(lpd_file_for_input).get_location() + "', corruption or incorrect format?\n");
  }

  return (false);
}
#endif

lpeConstElm::lpeConstElm() {
  safeguard = ATtableCreate(10000,50);
  rewr = NULL;
}
lpeConstElm::~lpeConstElm() {
  delete rewr;
  ATtableDestroy(safeguard);
}

void lpeConstElm::getDatVarRec(aterm_appl t) {
  if(gsIsDataVarId(t) && (p_freeVarSet.find(data_variable(t)) != p_freeVarSet.end())){
    p_foundFreeVars.insert(t);
  };

  for(aterm_appl::iterator i = t.begin(); i!= t.end();i++) {
    getDatVarRec(aterm_appl(*i));
  } 
} 

// Returns a vector in which each element is a AtermsAppl (DataVarID)  
//
inline std::set< lpe::data_variable > lpeConstElm::getUsedFreeVars(aterm_appl input) {
  p_foundFreeVars.clear();
  getDatVarRec(input);
  return p_foundFreeVars;
}

// Rewrites an ATerm to a normal form
//
// pre : input is an AtermAppl
// post: result is an ATermAppl in normal form
inline ATermAppl lpeConstElm::rewrite(ATermAppl t) { 
  return rewr->rewrite(t);
}

// Subsitutes a vectorlist of data assignements to a ATermAppl 
//
inline ATermAppl lpeConstElm::p_substitute(ATermAppl t, std::vector< lpe::data_assignment > &y ) {
  for(std::vector< lpe::data_assignment >::iterator i = y.begin() ; i != y.end() ; i++){
    rewr->setSubstitution(i->lhs() ,rewr->toRewriteFormat(i->rhs()));
  }
  ATermAppl result = rewr->rewrite(t);
  for(std::vector< lpe::data_assignment >::iterator i = y.begin() ; i != y.end() ; i++){
    rewr->clearSubstitution(i->lhs());
  }
  return result;
}
// calculates a nextstate given the current 
// stores the next state information in p_nextState 
// 
inline void lpeConstElm::calculateNextState(data_assignment_list assignments) {
  for(std::vector< lpe::data_assignment >::iterator i = p_currentState.begin(); i != p_currentState.end(); i++ ){
    int index = p_lookupIndex[i->lhs()];
    if (p_V.find(index) == p_V.end()){
      lpe::data_expression tmp = i->lhs(); 
      for (lpe::data_assignment_list::iterator j = assignments.begin(); j != assignments.end() ; j++){
        if (j->lhs() == i->lhs()){
          tmp = j->rhs();
          break;
        }
      }
      p_nextState.at(index) = lpe::data_assignment(i->lhs(), p_substitute(tmp, p_currentState));
    } else {
      p_nextState.at(index) = *i;
    }
  }
}

// returns whether a expression occurs in the list of free variables
//  
inline bool lpeConstElm::inFreeVarList(data_expression dexpr) {
  return (p_freeVarSet.find(dexpr) != p_freeVarSet.end());
}

// Creates an unique expression:
// these date_expressions are used to model that a process parameter has a 
// value which is not constant
//
inline lpe::data_assignment lpeConstElm::newExpression(lpe::data_assignment ass) {
  char buffer [99];
  sprintf(buffer, "%s^%d", ass.lhs().unquoted_name().c_str(), p_newVarCounter++);
  data_variable w(buffer, ass.lhs().sort() );
  data_assignment a(ass.lhs() , w);
  return a;
}

// returns whether two data_expressions are equal
//
inline bool lpeConstElm::compare(data_expression x, data_expression y) {
  return (x==y);
}

// returns whether the given data_expression is false
//  
inline bool lpeConstElm::conditionTest(data_expression x) {
  if (p_alltrue){return true;};
  //----------          Debug  
  gsDebugMsg("\033[33m %s\n", pp(x).c_str());
  gsDebugMsg("\033[30m %s\033[0m\n", pp(data_expression(rewrite(data_expression(p_substitute(x, p_currentState))))).c_str());
  //----------          Debug
  return (!(data_expression(rewrite(data_expression(p_substitute(x, p_currentState)))).is_false()));
}

// returns whether the currentState and NextState differ
//
inline bool lpeConstElm::cmpCurrToNext() {
  bool differs = false;
  for(std::vector< lpe::data_assignment>::iterator i= p_currentState.begin(); i != p_currentState.end() ;i++){
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
            gsDebugMsg("\033[34m OLD:    %s\n", pp(*i).c_str());
            gsDebugMsg("\033[32m NEW:    %s\033[0m\n", pp(p_nextState.at(index)).c_str());
            //----------          Debug
          }
        }
      } else {
        if (!inFreeVarList( p_nextState.at(index).rhs() )){
           if (!compare(i->rhs(), p_nextState.at(index).rhs())){
              //----------          Debug
              gsDebugMsg("\033[34m OLD:    %s\n", pp(*i).c_str());
              gsDebugMsg("\033[32m NEW:    %s\033[0m\n", pp(p_nextState.at(index)).c_str());
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
void lpeConstElm::detectVar(int n) {
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
bool lpeConstElm::recDetectVar(lpe::data_expression t, std::set<data_expression> &S) {
   bool b = false;
   if( gsIsDataVarId(t) && (S.find(t) != S.end()) ){
     b = true;
   }
   if ( gsIsDataAppl(t) ) {
     for(aterm_appl::iterator i = aterm_appl(t).begin(); i!= aterm_appl(t).end();i++) {
       b = b || recDetectVar(aterm_appl(*i), S);
     }
   }
   return b;
}

// template for changing a vector into a list
//
template <typename Term>
inline atermpp::term_list<Term> lpeConstElm::vectorToList(std::vector<Term> y) { 
  term_list<Term> result;
  for(typename std::vector<Term>::iterator i = y.begin(); i != y.end() ; i++)
    { 
      result = push_front(result,*i); 
    }
  return atermpp::reverse(result); 
} 

// template for changing a set into a list
//  
template <typename Term>
inline term_list<Term> lpeConstElm::setToList(std::set<Term> y) { 
  term_list<Term> result;
  for(typename std::set<Term>::iterator i = y.begin(); i != y.end() ; i++)
    { 
      result = push_front(result,*i); 
    }
  return atermpp::reverse(result); 
} 

// Find all sorts which have a singleton domain
//
void lpeConstElm::findSingleton() {

  std::map< lpe::sort, int >     p_countSort;
  //set< lpe::sort > result;
  for(lpe::sort_list::iterator i = p_spec.data().sorts().begin(); i != p_spec.data().sorts().end() ; i++){
    p_countSort[*i] = 0;
    p_singletonSort.insert(*i);
  }
  
  for(lpe::function_list::iterator i= p_spec.data().constructors().begin() ; i != p_spec.data().constructors().end() ; i++){
    p_countSort[i->range_sort()]++;
  }

  unsigned int n = p_singletonSort.size()+1;
  while (n != p_singletonSort.size()){
    n = p_singletonSort.size();
    for(sort_list::iterator i = p_spec.data().sorts().begin(); i != p_spec.data().sorts().end() ; i++){
      int b = 1;
      //if p_countSort[*i] == 0 then there are sorts declared which are never used!!!!
        assert(p_countSort[*i] != 0);

      if (p_countSort[*i] == 1){
        for(function_list::iterator j = p_spec.data().constructors().begin() ; j != p_spec.data().constructors().end() ;j++){
          if (j->range_sort() == *i){
            sort_list sorts = j->domain_sorts();
            for(sort_list::iterator k = sorts.begin() ; k != sorts.end() ; k++ ){
              b = std::max(p_countSort[*k], b);
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
    gsVerboseMsg("lpeconstelm: Sorts which have singleton constructors:\n");
    for(std::set<lpe::sort>::iterator i = p_singletonSort.begin(); i != p_singletonSort.end(); i++){
      gsVerboseMsg("lpeconstelm:   %s\n", pp(*i).c_str());
    }
    if (p_singletonSort.empty()) {
      gsVerboseMsg("lpeconstelm:   []\n");
    }
  }
} 

//---------------------------------------------------------------
//---------------------   Debug begin  --------------------------
//---------------------------------------------------------------
inline void lpeConstElm::printNextState() {
  std::ostringstream result;
  if (p_verbose)
  {
    for(std::vector< lpe::data_assignment >::iterator i = p_nextState.begin(); i != p_nextState.end() ; i++ ){
      result << "[" << pp(*i) << "]";
    }
    gsVerboseMsg("%s\n", result.str().c_str());
  }
}

inline void lpeConstElm::printVar() {
  std::ostringstream result;
  if (p_verbose)
  {
    result << "lpeconstelm: Variable indices : {";
    std::set< int >::iterator i = p_V.begin();
    int j = 0;
    while(i != p_V.end()){
      if (*i ==j){
        result << j+1 << " ";
        i++;
      }
      j++;
    }
    result << "}";
    gsVerboseMsg("%s\n", result.str().c_str());
  }
}

inline void lpeConstElm::printState() {
  std::ostringstream result;
  if (p_verbose)
  {
    if ( p_S.size() > 0 )
    {
      result << "lpeconstelm:   [ ";
      for(std::set< int >::iterator i = p_S.begin(); i != (--p_S.end()) ; i++ ){
        if (!p_nosingleton){
          result << pp(p_currentState[*i]);
        } else {
          result << pp(p_currentState[*i]) << pp(p_currentState[*i].lhs().sort());
        }  
      }
      result << pp(p_currentState[*(--p_S.end())]) << " ]";
    }
    gsVerboseMsg("%s\n", result.str().c_str());
  }
}

inline void lpeConstElm::printCurrentState() {
  std::ostringstream result;
  if (p_verbose)
  {
    for(std::vector< lpe::data_assignment >::iterator i = p_currentState.begin(); i != p_currentState.end() ; i++ ){
      result << "[" << pp(*i) << "]";
    }
    gsVerboseMsg("%s\n", result.str().c_str());
  }
}
//---------------------------------------------------------------
//---------------------   Debug end  --------------------------
//---------------------------------------------------------------
// sorts with singleton constructors are removed from p_S
// pre:  p_S is calculated && p_initAssignments is set
// post: p_S contains the indices without the process parameters which have a singleton constructor
void lpeConstElm::removeSingleton(int n)
{
  bool empty = true;
  sort_list rebuild_sort = p_spec.data().sorts();
  findSingleton();
  gsVerboseMsg("lpeconstelm: Constant process parameters which are not substituted and removed [--no-singleton]:\n");
  for(int i = 0; i < n; i++)
  {
    if ( (p_V.find(i) == p_V.end()) &&
         (p_singletonSort.find(p_initAssignments[i].lhs().sort()) != p_singletonSort.end())
       )
    {
      p_V.insert(i);
      if (p_verbose){
        gsVerboseMsg("lpeconstelm:   %s : %s\n", pp(p_initAssignments[i].lhs()).c_str(), pp(p_initAssignments[i].lhs().sort()).c_str());
        empty = false;
      }
    }
  }
  if (empty){
    gsVerboseMsg("lpeconstelm:   []\n");
  }
}

// Writes an LPD to a file or sdtout
// Substituting occurences of constant parameters with their constant value
// and removing the constant process parameters from the list of process. 
// Constant parameters (stored in p_S)
//
inline void lpeConstElm::output() {
  lpe::LPE p_lpe = p_spec.lpe();
  summand_list rebuild_summandlist;

  //Remove the summands that are never visited
  //
  if (p_reachable){
    rebuild_summandlist = setToList(p_visitedSummands); 
  } else {
    rebuild_summandlist = p_lpe.summands();
  }

  gsVerboseMsg("lpeconstelm: Number of summands of old LPD: %d\n", p_lpe.summands().size());
  gsVerboseMsg("lpeconstelm: Number of summands of new LPD: %d\n", rebuild_summandlist.size());

  std::set< lpe::data_variable > constantVar;
  for(std::set< int >::iterator i = p_S.begin(); i != p_S.end(); i++){
    constantVar.insert(p_initAssignments.at(*i).lhs());
  }

  std::vector< lpe::data_assignment > constantPP;
  for(std::set< int >::iterator i = p_S.begin(); i != p_S.end(); i++){
    constantPP.push_back(p_currentState.at(*i));
  }

  std::vector< lpe::data_variable > variablePPvar;
  for(std::set< int >::iterator i = p_V.begin(); i != p_V.end(); i++){
    variablePPvar.push_back(p_initAssignments.at(*i).lhs());
  }

  std::vector< lpe::data_expression > variablePPexpr;
  for(std::set< int >::iterator i = p_V.begin(); i != p_V.end(); i++){
    variablePPexpr.push_back(p_initAssignments.at(*i).rhs());
  }    

  //Remove process parameters in in summand
  // 
  lpe::summand_list rebuild_summandlist_no_cp;

  for(lpe::summand_list::iterator currentSummand = rebuild_summandlist.begin(); currentSummand != rebuild_summandlist.end(); currentSummand++){

    //construct new LPD_summand
    //
    lpe::LPE_summand tmp;

    //Remove constant process parameters from the summands assignments 
    //
    lpe::data_assignment_list rebuildAssignments; 
    for(lpe::data_assignment_list::iterator currentAssignment = currentSummand->assignments().begin(); currentAssignment != currentSummand->assignments().end() ; currentAssignment++){
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
  
   std::set< lpe::data_variable > usedFreeVars;
   std::set< lpe::data_variable > foundVars;
   for(lpe::summand_list::iterator currentSummand = rebuild_summandlist_no_cp.begin(); currentSummand != rebuild_summandlist_no_cp.end(); currentSummand++){ 
     for(lpe::data_assignment_list::iterator i = currentSummand->assignments().begin(); i !=  currentSummand->assignments().end() ;i++){
       foundVars = getUsedFreeVars(aterm_appl(i->rhs()));
       for(std::set< lpe::data_variable >::iterator k = foundVars.begin(); k != foundVars.end(); k++){
         usedFreeVars.insert(*k);
      }
     }
   }

  //construct new specfication
  //
  //LPE(data_variable_list free_variables, data_variable_list process_parameters, 
  //  summand_list summands);
  lpe::LPE rebuild_lpe;
  rebuild_lpe = lpe::LPE(
    setToList(usedFreeVars),
    vectorToList(variablePPvar), 
    atermpp::reverse(rebuild_summandlist_no_cp)
  );
   
   //gsDebugMsg("%s\n", p_spec.initial_free_variables().to_string().c_str());
   
  std::set< lpe::data_variable > initial_free_variables;
  usedFreeVars.empty();
  for(std::vector< lpe::data_expression >::iterator i = variablePPexpr.begin(); i != variablePPexpr.end(); i++){
       foundVars = getUsedFreeVars(aterm_appl(*i));
       for(std::set< lpe::data_variable >::iterator k = foundVars.begin(); k != foundVars.end(); k++){
         initial_free_variables.insert(*k); 
       }           
  }

  // Rebuild spec
  //
  //specification(sort_list sorts, function_list constructors, 
  //            function_list mappings, data_equation_list equations, 
  //            action_label_list action_labels, LPE lpe, 
  //            data_variable_list initial_free_variables, 
  //            data_variable_list initial_variables, 
  //            data_expression_list initial_state);
  //
  specification rebuild_spec;
  rebuild_spec = specification(
    p_spec.data(),
    p_spec.action_labels(), 
    rebuild_lpe, 
    setToList(initial_free_variables), 
    vectorToList(variablePPvar), 
    vectorToList(variablePPexpr)
  );
  
  assert(gsIsSpecV1((ATermAppl) rebuild_spec));

  //gsDebugMsg("%s\n", pp(p_lpe).c_str());
  if (p_outputfile.empty()){
    //if(!p_verbose){
    //  assert(!p_verbose);
      writeStream(rebuild_spec);
    //};
  }
  else {
    if(!rebuild_spec.save(p_outputfile)) {
       gsErrorMsg("lpeconstelm: Unsuccessfully written outputfile: %s\n", p_outputfile.c_str());
    }
  } 
}

// Set output file
//
inline void lpeConstElm::setSaveFile(std::string const& x) {
  p_outputfile = x;
}

// Print the set of constant process parameters
//  
inline void lpeConstElm::printSet() {
  std::ostringstream result;
  if (p_verbose)
  {
    result << "lpeconstelm: Constant indices: { ";
    std::set< int >::iterator i = p_S.begin();
    int j = 0;
    while(i != p_S.end()){
      if (*i ==j){
        result << j+1;
        i++;
      }
      j++;
    }
    result << " }";
    gsVerboseMsg("%s\n", result.str().c_str());
  }
}

// Loads an LPD from file
// returns true if succeeds
//  
inline bool lpeConstElm::loadFile(std::string const& filename) {
  p_filenamein = filename;
  if (!p_spec.load(p_filenamein))
  {
    gsErrorMsg("lpeconstelm: error: could not read input file '%s'\n", filename.c_str());
    return false;
  } 
  //LPD x = p_spec.lpe(); 
  //gsDebugMsg("%s\n", pp(x).c_str());
  //p_spec.save("/scratch/dump.lpe");
  //assert(false);
  return true;
}

// Reads an LPD from stdin
// returns true if succeeds
//  
inline bool lpeConstElm::readStream() {
  ATermAppl z = (ATermAppl) ATreadFromFile(stdin);
  if (z == NULL){
    gsErrorMsg("lpeconstelm: Could not read LPD from stdin\n");
    return false;
  };
  if (!gsIsSpecV1(z)){
    gsErrorMsg("lpeconstelm: Stdin does not contain an LPD\n");
    return false;
  }
  p_spec = specification(z);
  //gsDebugMsg("%s\n", pp(p_spec.lpe()).c_str());
  return true;
}

// Writes file to stdout
//
void lpeConstElm::writeStream(lpe::specification newSpec) {
  assert(gsIsSpecV1((ATermAppl) newSpec));
  ATwriteToBinaryFile(aterm(newSpec) , stdout);
}

// Sets verbose option
// Note: Has to be set
//
inline void lpeConstElm::setVerbose(bool b) {
  gsSetVerboseMsg();
  p_verbose = b;
}

// Sets debug option
// Note: Has to be set
//
inline void lpeConstElm::setDebug(bool b) {
  setVerbose(b);
  gsSetDebugMsg();
  p_debug = b;
}

// Sets no singleton option
// Note: Has to be set
//  
inline void lpeConstElm::setNoSingleton(bool b) {
  p_nosingleton = b;
}

// Sets all conditions to true
// Note: Has to be set
//  
inline void lpeConstElm::setAllTrue(bool b) {
  p_alltrue = b;
}

// Sets the option if not inspected summands have to removed 
// Note: Has to be set
//  
inline void lpeConstElm::setReachable(bool b) {
  p_reachable = b;
}

// Prints the data_variable which are constant
//  
void lpeConstElm::printSetVar() {
  printState();
}  

// The constelm filter
//
void lpeConstElm::filter() {

  //---------------------------------------------------------------
  //---------------------   Init begin   --------------------------
  //---------------------------------------------------------------

  bool    same        ;
  bool    foundFake = true;
  int     counter  = 0;
  int     cycle    = 0;
  p_newVarCounter  = 0;
  
  lpe::LPE p_lpe = p_spec.lpe();
  rewr           = createRewriter(p_spec.data()); 

  for(lpe::data_assignment_list::iterator i = p_spec.initial_assignments().begin(); i != p_spec.initial_assignments().end() ; i++ ){
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
    p_freeVarSet.insert(*di);
  }
  for (data_variable_list::iterator di = p_lpe.free_variables().begin(); di != p_lpe.free_variables().end(); di++){
    p_freeVarSet.insert(*di);
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
      sum_vars.insert(*j);
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
      gsVerboseMsg("lpeconstelm: Cycle %d: \n", cycle++);
      //int summandnr = 1;
      for(summand_list::iterator currentSummand = p_lpe.summands().begin(); currentSummand != p_lpe.summands().end() ;currentSummand++ ){
        if ( (p_visitedSummands.find(*currentSummand) != p_visitedSummands.end()) || (conditionTest(currentSummand->condition()))) {
          //gsDebugMsg(  "Summand: %d\n", summandnr++);
          p_visitedSummands.insert(*currentSummand); 
          //----------          Debug
          if (p_debug) { printCurrentState(); }
          calculateNextState(currentSummand->assignments());
          //----------          Debug  
          if (p_debug) { printCurrentState(); }
          same = cmpCurrToNext() && same ; 
          //ischanged = ischanged || !cmpCurrToNext();
          //if (!same) {break;}                                           //Break reduces time to complete; need to find out when to brake
        }
      }
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
      if ( diff == 1 )
      {
        gsVerboseMsg("lpeconstelm: reset 1 parameter to variable because its value contained summation variables\n");
      } else if ( diff > 1 )
      {
        gsVerboseMsg("lpeconstelm: reset %d parameters to variable because their values contained summation variables\n", diff);
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
      gsVerboseMsg("lpeconstelm: Free Variable checkup:\n");
    
      int n = p_V.size();
      for(std::set< LPE_summand>::iterator i = p_visitedSummands.begin(); i != p_visitedSummands.end() ; i++){
        calculateNextState(i->assignments());
        cmpCurrToNext();
      }

      p_currentState = p_newCurrentState;
      if (p_verbose){
        gsVerboseMsg("lpeconstelm:   Detected %d fake constant process parameters\n", p_V.size() - n);
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
     if ( diff > 1 )
     {
       gsVerboseMsg("lpeconstelm: %d constant parameters are not removed because their sorts contain only one element\n", diff);
     } else if ( diff == 1 )
     {
       gsVerboseMsg("lpeconstelm: 1 constant parameter is not removed because its sort contains only one element\n");
     }
  }    
  
  //---------------------------------------------------------------
  // Construct S    
  //
  std::set< int > S;
  n = p_lpe.process_parameters().size(); 
  for(int j=0; j < n ; j++){
    S.insert(j);
  };
  set_difference(S.begin(), S.end(), p_V.begin(), p_V.end(), inserter(p_S, p_S.begin()));
  
  if (p_verbose){
    gsVerboseMsg("lpeconstelm: Number of removed process parameters: %d\n", p_S.size());
    printSetVar();  
  }
}

// Gets the version of the tool
//    
inline std::string lpeConstElm::getVersion() {
  return (version);
}

void parse_command_line(int ac, char** av, lpeConstElm& constelm) {
  namespace po = boost::program_options;

  po::options_description description;

  /* Name of the file to read input from (or standard input: "-") */
  std::vector < std::string > file_names;

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
     ("file_names", po::value< std::vector< std::string > >(), "input/output files")
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
              << "Removes constant process parameters from the LPD read from standard input or INFILE." << std::endl
              << "By default the result is written to standard output, and otherwise to OUTFILE." << std::endl
              << std::endl << description;

    exit (0);
  }
        
  if (vm.count("version")) {
    std::cerr << version << " (revision " << REVISION << ")" << std::endl;

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
    std::cerr << "lpeconstelm: Specify only INPUT and/or OUTPUT file (too many arguments)."<< std::endl;

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

int main(int argc, char** argv) {
  ATerm       bottom;

  ATinit(argc,argv,&bottom);
  
  gsEnableConstructorFunctions();

#ifdef ENABLE_SQUADT_CONNECTIVITY
  squadt_interactor c;

  if (!c.try_interaction(argc, argv)) {
#endif
    lpeConstElm constelm;

    parse_command_line(argc,argv,constelm);

    constelm.filter();
    constelm.output(); 
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif

  return 0;
}
