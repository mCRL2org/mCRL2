// ======================================================================
//
// Copyright (c) 2004, 2005 TU/e
//
// ----------------------------------------------------------------------
//
// file          : lpsconstelm 
// date          : 16-11-2005
// version       : 0.5.2
//
// author(s)     : Frank Stappers  <f.p.m.stappers@student.tue.nl>
//
// ======================================================================

//LPS framework
#ifdef BOOST_BUILD_PCH_ENABLED
# ifdef ENABLE_SQUADT_CONNECTIVITY
#  include <utilities/mcrl2_squadt.h>
#  include "lps/specification.h"
# else
#  include "specification.h"
# endif
#else
# include "lps/specification.h"
#endif

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

using namespace lps;
using namespace atermpp;

const char* version = "0.5.2";

class lpsConstElm {
  private:
    ATermTable                            safeguard;
    std::string                           p_inputfile;
    std::string                           p_outputfile;
    std::vector< lps::data_assignment >   p_currentState;
    std::vector< lps::data_assignment >   p_newCurrentState;
    std::vector< lps::data_assignment >   p_nextState;
    std::vector< lps::data_assignment >   p_initAssignments;
    std::map< lps::data_variable, int  >  p_lookupIndex;
    std::map< int, lps::data_variable >   p_lookupDataVarIndex;
    std::set< lps::data_expression >      p_freeVarSet; 
    std::set< int >                       p_V; 
    std::set< int >                       p_S;
    std::set< lps::summand >          p_visitedSummands;
    std::set< lps::data_expression >      p_variableList; 
    int                                   p_newVarCounter;
    bool                                  p_verbose;
    bool                                  p_nosingleton;
    bool                                  p_alltrue;
    bool                                  p_reachable; 
    std::string                           p_filenamein;
    lps::specification                    p_spec;
    std::set< lps::sort >                 p_singletonSort;
    Rewriter*                             rewr;
    
    //Only used by getDataVarIDs  
    std::set< lps::data_variable >        p_foundFreeVars;       
 
    //Only used by detectVar
    std::set< lps::data_expression >      sum_vars;
 
  public:

    lpsConstElm();

    ~lpsConstElm();

  private:

    void getDatVarRec(aterm_appl t);
    std::set< lps::data_variable > getUsedFreeVars(aterm_appl input);
    ATermAppl rewrite(ATermAppl t);
    ATermAppl p_substitute(ATermAppl t, std::vector< lps::data_assignment > &y );
    void calculateNextState(lps::data_assignment_list assignments);
    bool inFreeVarList(lps::data_expression dexpr);
    lps::data_assignment newExpression(lps::data_assignment ass);
    bool compare(lps::data_expression x, lps::data_expression y);
    bool cmpCurrToNext();
    bool conditionTest(lps::data_expression x);
    void detectVar(int n);
    bool recDetectVarList(lps::data_expression_list l, std::set<data_expression> &S); 
    bool recDetectVar(lps::data_expression t, std::set<lps::data_expression> &S);
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
    void writeStream(lps::specification newSpec);
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
// Squadt protocol interface and utility pseudo-library
#include <utilities/mcrl2_squadt.h>

class squadt_interactor : public mcrl2_squadt::tool_interface {

  private:

    static const char*  lps_file_for_input;  ///< file containing an LPS that can be imported
    static const char*  lps_file_for_output; ///< file used to write the output to

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

const char* squadt_interactor::lps_file_for_input  = "lps_in";
const char* squadt_interactor::lps_file_for_output = "lps_out";

const char* squadt_interactor::option_remove_single_element_sorts = "remove_single_element_sorts";
const char* squadt_interactor::option_remove_unvisited_summands   = "remove_unvisited_summands";
const char* squadt_interactor::option_ignore_summand_conditions   = "ignore_summand_conditions";

void squadt_interactor::set_capabilities(sip::tool::capabilities& c) const {
  c.add_input_combination(lps_file_for_input, sip::mime_type("lps", sip::mime_type::application), sip::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(sip::configuration& c) {
  using namespace sip;
  using namespace sip::layout;
  using namespace sip::layout::elements;

  /* Set defaults where the supplied configuration does not have values */
  if (!c.output_exists(lps_file_for_output)) {
    /* Add output file to the configuration */
    c.add_output(lps_file_for_output, sip::mime_type("lps", sip::mime_type::application), c.get_output_name(".lps"));
  }

  if (!c.option_exists(option_remove_single_element_sorts)) {
    c.add_option(option_remove_single_element_sorts, false).
        set_argument_value< 0, sip::datatype::boolean >(true, false);
  }
  if (!c.option_exists(option_remove_unvisited_summands)) {
    c.add_option(option_remove_unvisited_summands, false).
        set_argument_value< 0, sip::datatype::boolean >(true, false);
  }
  if (!c.option_exists(option_ignore_summand_conditions)) {
    c.add_option(option_ignore_summand_conditions, false).
        set_argument_value< 0, sip::datatype::boolean >(true, false);
  }

  /* Create and add the top layout manager */
  layout::manager::aptr top(layout::horizontal_box::create());

  /* First column */
  layout::vertical_box* column = new layout::vertical_box();

  checkbox* remove_single_element_sorts = new checkbox("remove single element sorts",
        c.get_option_argument< bool >(option_remove_single_element_sorts));
  checkbox* remove_unvisited_summands   = new checkbox("remove summands that are not visited",
        c.get_option_argument< bool >(option_remove_unvisited_summands));
  checkbox* ignore_summand_conditions   = new checkbox("take summand conditions into account",
        c.get_option_argument< bool >(option_ignore_summand_conditions));

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

  /* Update configuration */
  c.get_option(option_remove_single_element_sorts).
      set_argument_value< 0, sip::datatype::boolean >(remove_single_element_sorts->get_status());
  c.get_option(option_remove_unvisited_summands).
      set_argument_value< 0, sip::datatype::boolean >(remove_unvisited_summands->get_status());
  c.get_option(option_ignore_summand_conditions).
      set_argument_value< 0, sip::datatype::boolean >(ignore_summand_conditions->get_status());
}

bool squadt_interactor::check_configuration(sip::configuration const& c) const {
  bool result = true;

  result &= c.input_exists(lps_file_for_input);
  result &= c.output_exists(lps_file_for_output);

  return (result);
}

bool squadt_interactor::perform_task(sip::configuration& c) {
  lpsConstElm constelm;

  /* Set with options from the current configuration object */
  constelm.setNoSingleton(c.option_exists(option_remove_single_element_sorts));
  constelm.setReachable(c.option_exists(option_remove_unvisited_summands));
  constelm.setAllTrue(c.option_exists(option_remove_unvisited_summands));

  send_hide_display();

  if (constelm.loadFile(c.get_input(lps_file_for_input).get_location())) {
    constelm.setSaveFile(c.get_output(lps_file_for_output).get_location());

    constelm.filter();
    constelm.output(); 

    return (true);
  }
  else {
    send_error("Could not read `" + c.get_input(lps_file_for_input).get_location() + "', corruption or incorrect format?\n");
  }

  return (false);
}
#endif

lpsConstElm::lpsConstElm() {
  safeguard = ATtableCreate(10000,50);
  rewr = NULL;
}
lpsConstElm::~lpsConstElm() {
  delete rewr;
  ATtableDestroy(safeguard);
}

void lpsConstElm::getDatVarRec(aterm_appl t) {
  if(gsIsDataVarId(t) && (p_freeVarSet.find(data_variable(t)) != p_freeVarSet.end())){
    p_foundFreeVars.insert(t);
  };

  for(aterm_appl::iterator i = t.begin(); i!= t.end();i++) {
    getDatVarRec(aterm_appl(*i));
  } 
} 

// Returns a vector in which each element is a AtermsAppl (DataVarID)  
//
inline std::set< lps::data_variable > lpsConstElm::getUsedFreeVars(aterm_appl input) {
  p_foundFreeVars.clear();
  getDatVarRec(input);
  return p_foundFreeVars;
}

// Rewrites an ATerm to a normal form
//
// pre : input is an AtermAppl
// post: result is an ATermAppl in normal form
inline ATermAppl lpsConstElm::rewrite(ATermAppl t) { 
  return rewr->rewrite(t);
}

// Subsitutes a vectorlist of data assignements to a ATermAppl 
//
inline ATermAppl lpsConstElm::p_substitute(ATermAppl t, std::vector< lps::data_assignment > &y ) {
  for(std::vector< lps::data_assignment >::iterator i = y.begin() ; i != y.end() ; i++){
    rewr->setSubstitution(i->lhs() ,rewr->toRewriteFormat(i->rhs()));
  }
  ATermAppl result = rewr->rewrite(t);
  for(std::vector< lps::data_assignment >::iterator i = y.begin() ; i != y.end() ; i++){
    rewr->clearSubstitution(i->lhs());
  }
  return result;
}
// calculates a nextstate given the current 
// stores the next state information in p_nextState 
// 
inline void lpsConstElm::calculateNextState(data_assignment_list assignments) {
  for(std::vector< lps::data_assignment >::iterator i = p_currentState.begin(); i != p_currentState.end(); i++ ){
    int index = p_lookupIndex[i->lhs()];
    if (p_V.find(index) == p_V.end()){
      lps::data_expression tmp = i->lhs(); 
      for (lps::data_assignment_list::iterator j = assignments.begin(); j != assignments.end() ; j++){
        if (j->lhs() == i->lhs()){
          tmp = j->rhs();
          break;
        }
      }
      p_nextState.at(index) = lps::data_assignment(i->lhs(), p_substitute(tmp, p_currentState));
    } else {
      p_nextState.at(index) = *i;
    }
  }
}

// returns whether a expression occurs in the list of free variables
//  
inline bool lpsConstElm::inFreeVarList(data_expression dexpr) {
  return (p_freeVarSet.find(dexpr) != p_freeVarSet.end());
}

// Creates an unique expression:
// these date_expressions are used to model that a process parameter has a 
// value which is not constant
//
inline lps::data_assignment lpsConstElm::newExpression(lps::data_assignment ass) {
  char buffer [99];
  sprintf(buffer, "%s^%d", std::string(ass.lhs().name()).c_str(), p_newVarCounter++);
  data_variable w(buffer, ass.lhs().sort() );
  data_assignment a(ass.lhs() , w);
  return a;
}

// returns whether two data_expressions are equal
//
inline bool lpsConstElm::compare(data_expression x, data_expression y) {
  return (x==y);
}

// returns whether the given data_expression is false
//  
inline bool lpsConstElm::conditionTest(data_expression x) {
  if (p_alltrue){return true;};
  //----------          Debug  
  //gsDebugMsg("\033[33m %s\n", pp(x).c_str());
  //gsDebugMsg("\033[30m %s\033[0m\n", pp(data_expression(rewrite(data_expression(p_substitute(x, p_currentState))))).c_str());
  //----------          Debug
  return (!(data_expression(rewrite(data_expression(p_substitute(x, p_currentState)))).is_false()));
}

// returns whether the currentState and NextState differ
//
inline bool lpsConstElm::cmpCurrToNext() {
  bool differs = false;
  for(std::vector< lps::data_assignment>::iterator i= p_currentState.begin(); i != p_currentState.end() ;i++){
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
            //gsDebugMsg("\033[34m OLD:    %s\n", pp(*i).c_str());
            //gsDebugMsg("\033[32m NEW:    %s\033[0m\n", pp(p_nextState.at(index)).c_str());
            //----------          Debug
          }
        }
      } else {
        if (!inFreeVarList( p_nextState.at(index).rhs() )){
           if (!compare(i->rhs(), p_nextState.at(index).rhs())){
              //----------          Debug
              //gsDebugMsg("\033[34m OLD:    %s\n", pp(*i).c_str());
              //gsDebugMsg("\033[32m NEW:    %s\033[0m\n", pp(p_nextState.at(index)).c_str());
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
void lpsConstElm::detectVar(int n) {
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

// Return whether or not a summation variable occurs in a data term list
bool lpsConstElm::recDetectVarList(lps::data_expression_list l, std::set<data_expression> &S) {
  gsVerboseMsg("list: %s\n", l.to_string().c_str());
  bool b = false;
  for(data_expression_list::iterator i = l.begin(); i != l.end() && !(b); ++i) {
    b = b || recDetectVar(aterm_appl(*i), S);
  }
  return b;
}

// Return whether or not a summation variable occurs in a data term
bool lpsConstElm::recDetectVar(lps::data_expression t, std::set<data_expression> &S) {
  gsVerboseMsg("expr: %s\n", t.to_string().c_str());
   bool b = false;
   if( gsIsDataVarId(t) && (S.find(t) != S.end()) ){
     b = true;
   }
   if ( gsIsDataApplProd(t) ) {
     b = b || recDetectVar(data_expression(t.argument(0)), S);
     b = b || recDetectVarList(data_expression_list(t.argument(1)), S);
   }
   return b;
}

// template for changing a vector into a list
//
template <typename Term>
inline atermpp::term_list<Term> lpsConstElm::vectorToList(std::vector<Term> y) { 
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
inline term_list<Term> lpsConstElm::setToList(std::set<Term> y) { 
  term_list<Term> result;
  for(typename std::set<Term>::iterator i = y.begin(); i != y.end() ; i++)
    { 
      result = push_front(result,*i); 
    }
  return atermpp::reverse(result); 
} 

// Find all sorts which have a singleton domain
//
void lpsConstElm::findSingleton() {

  std::map< lps::sort, int >     p_countSort;
  //set< lps::sort > result;
  for(lps::sort_list::iterator i = p_spec.data().sorts().begin(); i != p_spec.data().sorts().end() ; i++){
    p_countSort[*i] = 0;
    p_singletonSort.insert(*i);
  }
  
  for(lps::function_list::iterator i= p_spec.data().constructors().begin() ; i != p_spec.data().constructors().end() ; i++){
    p_countSort[i->range_sort()]++;
  }

  unsigned int n = p_singletonSort.size()+1;
  while (n != p_singletonSort.size()){
    n = p_singletonSort.size();
    for(sort_list::iterator i = p_spec.data().sorts().begin(); i != p_spec.data().sorts().end() ; i++){
      int b = 1;
      //if p_countSort[*i] == 0 then there are sorts declared which are never used!!!!
//      assert(p_countSort[*i] != 0);

      if (p_countSort[*i] == 1){
        for(function_list::iterator j = p_spec.data().constructors().begin() ; j != p_spec.data().constructors().end() ;j++){
          if (j->range_sort() == *i){
            sort_list sorts = j->domain_sorts();
            for(sort_list::iterator k = sorts.begin() ; k != sorts.end() ; k++ ){
              b = std::max(p_countSort[*k], b);
            }
          }
        }
        if (b!=1) {
          p_singletonSort.erase(*i); p_countSort[*i] = b;
        }
      } else {
        p_singletonSort.erase(*i);
      }
    }
  }
  //p_singletonSort = result;
  
  if (p_verbose){
    gsVerboseMsg("lpsconstelm: Sorts which have singleton constructors:\n");
    for(std::set<lps::sort>::iterator i = p_singletonSort.begin(); i != p_singletonSort.end(); i++){
      gsVerboseMsg("lpsconstelm:   %s\n", pp(*i).c_str());
    }
    if (p_singletonSort.empty()) {
      gsVerboseMsg("lpsconstelm:   []\n");
    }
  }
} 

//---------------------------------------------------------------
//---------------------   Debug begin  --------------------------
//---------------------------------------------------------------
inline void lpsConstElm::printNextState() {
// (JK: 16/1/2006: This function is never called)  
  std::ostringstream result;
  for(std::vector< lps::data_assignment >::iterator i = p_nextState.begin(); i != p_nextState.end() ; i++ ){
    result << "[" << pp(*i) << "]";
  }
  gsVerboseMsg("%s\n", result.str().c_str());
}

inline void lpsConstElm::printVar() {
// (JK: 16/1/2006: This function is never called)  
  std::ostringstream result;
  result << "lpsconstelm: Variable indices : {";
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

inline void lpsConstElm::printState() {
  std::ostringstream result;
  if ( p_S.size() > 0 )
  {
    result << "lpsconstelm:   [ ";
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

inline void lpsConstElm::printCurrentState() {
// (JK: 16/1/2006: This function is never called)  
  std::ostringstream result;
  for(std::vector< lps::data_assignment >::iterator i = p_currentState.begin(); i != p_currentState.end() ; i++ ){
    result << "[" << pp(*i) << "]";
  }
  gsVerboseMsg("%s\n", result.str().c_str());
}
//---------------------------------------------------------------
//---------------------   Debug end  --------------------------
//---------------------------------------------------------------
// sorts with singleton constructors are removed from p_S
// pre:  p_S is calculated && p_initAssignments is set
// post: p_S contains the indices without the process parameters which have a singleton constructor
void lpsConstElm::removeSingleton(int n)
{
  bool empty = true;
  sort_list rebuild_sort = p_spec.data().sorts();
  findSingleton();
  gsVerboseMsg("lpsconstelm: Constant process parameters which are not substituted and removed [--no-singleton]:\n");
  for(int i = 0; i < n; i++)
  {
    if ( (p_V.find(i) == p_V.end()) &&
         (p_singletonSort.find(p_initAssignments[i].lhs().sort()) != p_singletonSort.end())
       )
    {
      p_V.insert(i);
      if (p_verbose){
        gsVerboseMsg("lpsconstelm:   %s : %s\n", pp(p_initAssignments[i].lhs()).c_str(), pp(p_initAssignments[i].lhs().sort()).c_str());
        empty = false;
      }
    }
  }
  if (empty){
    gsVerboseMsg("lpsconstelm:   []\n");
  }
}

// Writes an LPS to a file or sdtout
// Substituting occurences of constant parameters with their constant value
// and removing the constant process parameters from the list of process. 
// Constant parameters (stored in p_S)
//
inline void lpsConstElm::output() {
  lps::linear_process p_process = p_spec.process();
  summand_list rebuild_summandlist;

  //Remove the summands that are never visited
  //
  if (p_reachable){
    rebuild_summandlist = setToList(p_visitedSummands); 
  } else {
    rebuild_summandlist = p_process.summands();
  }

  gsVerboseMsg("lpsconstelm: Number of summands of old LPS: %d\n", p_process.summands().size());
  gsVerboseMsg("lpsconstelm: Number of summands of new LPS: %d\n", rebuild_summandlist.size());

  std::set< lps::data_variable > constantVar;
  for(std::set< int >::iterator i = p_S.begin(); i != p_S.end(); i++){
    constantVar.insert(p_initAssignments.at(*i).lhs());
  }

  std::vector< lps::data_assignment > constantPP;
  for(std::set< int >::iterator i = p_S.begin(); i != p_S.end(); i++){
    constantPP.push_back(p_currentState.at(*i));
  }

  std::vector< lps::data_variable > variablePPvar;
  for(std::set< int >::iterator i = p_V.begin(); i != p_V.end(); i++){
    variablePPvar.push_back(p_initAssignments.at(*i).lhs());
  }

  std::vector< lps::data_expression > variablePPexpr;
  for(std::set< int >::iterator i = p_V.begin(); i != p_V.end(); i++){
    variablePPexpr.push_back(p_initAssignments.at(*i).rhs());
  }    

  //Remove process parameters in in summand
  // 
  lps::summand_list rebuild_summandlist_no_cp;

  for(lps::summand_list::iterator currentSummand = rebuild_summandlist.begin(); currentSummand != rebuild_summandlist.end(); currentSummand++){

    //construct new LPS_summand
    //
    lps::summand tmp;

    //Remove constant process parameters from the summands assignments 
    //
    lps::data_assignment_list rebuildAssignments; 
    for(lps::data_assignment_list::iterator currentAssignment = currentSummand->assignments().begin(); currentAssignment != currentSummand->assignments().end() ; currentAssignment++){
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

    //LPS_summand(data_variable_list summation_variables, data_expression condition, 
    //            bool delta, action_list actions, data_expression time, 
    //            data_assignment_list assignments);    
    tmp = summand(currentSummand->summation_variables(), rebuild_condition, 
      currentSummand->is_delta(), atermpp::reverse(rebuild_actions) , rebuild_time, 
            atermpp::reverse(rebuildAssignments));
      rebuild_summandlist_no_cp = push_front(rebuild_summandlist_no_cp, tmp); 
  }
  
   std::set< lps::data_variable > usedFreeVars;
   std::set< lps::data_variable > foundVars;
   for(lps::summand_list::iterator currentSummand = rebuild_summandlist_no_cp.begin(); currentSummand != rebuild_summandlist_no_cp.end(); currentSummand++){ 
     for(lps::data_assignment_list::iterator i = currentSummand->assignments().begin(); i !=  currentSummand->assignments().end() ;i++){
       foundVars = getUsedFreeVars(aterm_appl(i->rhs()));
       for(std::set< lps::data_variable >::iterator k = foundVars.begin(); k != foundVars.end(); k++){
         usedFreeVars.insert(*k);
      }
     }
   }

  //construct new specfication
  //
  //linear_process(data_variable_list free_variables, data_variable_list process_parameters, 
  //  summand_list summands);
  lps::linear_process rebuild_process;
  rebuild_process = lps::linear_process(
    setToList(usedFreeVars),
    vectorToList(variablePPvar), 
    atermpp::reverse(rebuild_summandlist_no_cp)
  );
   
   //gsDebugMsg("%s\n", p_spec.initial_process().free_variables().to_string().c_str());
   
  std::set< lps::data_variable > initial_free_variables;
  usedFreeVars.empty();
  for(std::vector< lps::data_expression >::iterator i = variablePPexpr.begin(); i != variablePPexpr.end(); i++){
       foundVars = getUsedFreeVars(aterm_appl(*i));
       for(std::set< lps::data_variable >::iterator k = foundVars.begin(); k != foundVars.end(); k++){
         initial_free_variables.insert(*k); 
       }           
  }

  // Rebuild spec
  //
  //specification(sort_list sorts, function_list constructors, 
  //            function_list mappings, data_equation_list equations, 
  //            action_label_list action_labels, LPS lps, 
  //            data_variable_list initial_free_variables, 
  //            data_variable_list initial_variables, 
  //            data_expression_list initial_state);
  //
//data_assignment_expressions(data_assignment_list());
//data_assignment_list xyz = make_assignment_list(data_variable_list(), data_expression_list());
  lps::specification rebuild_spec = lps::specification(
    p_spec.data(),
    p_spec.action_labels(), 
    rebuild_process,
    process_initializer(setToList(initial_free_variables), 
                        make_assignment_list(vectorToList(variablePPvar), 
                                             vectorToList(variablePPexpr)
                                            )
                       )
  );
  assert(gsIsSpecV1((ATermAppl) rebuild_spec));

  //gsDebugMsg("%s\n", pp(p_process).c_str());
  if (p_outputfile.empty()){
    //if(!p_verbose){
    //  assert(!p_verbose);
      writeStream(rebuild_spec);
    //};
  }
  else {
    if(!rebuild_spec.save(p_outputfile)) {
       gsErrorMsg("lpsconstelm: Unsuccessfully written outputfile: %s\n", p_outputfile.c_str());
    }
  } 
}

// Set output file
//
inline void lpsConstElm::setSaveFile(std::string const& x) {
  p_outputfile = x;
}

// Print the set of constant process parameters
//  
inline void lpsConstElm::printSet() {
  std::ostringstream result;
  result << "lpsconstelm: Constant indices: { ";
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

// Loads an LPS from file
// returns true if succeeds
//  
inline bool lpsConstElm::loadFile(std::string const& filename) {
  p_filenamein = filename;
  if (!p_spec.load(p_filenamein))
  {
    gsErrorMsg("lpsconstelm: error: could not read input file '%s'\n", filename.c_str());
    return false;
  } 
  //LPS x = p_spec.process(); 
  //gsDebugMsg("%s\n", pp(x).c_str());
  //p_spec.save("/scratch/dump.lps");
  //assert(false);
  return true;
}

// Reads an LPS from stdin
// returns true if succeeds
//  
inline bool lpsConstElm::readStream() {
  ATermAppl z = (ATermAppl) ATreadFromFile(stdin);
  if (z == NULL){
    gsErrorMsg("lpsconstelm: Could not read LPS from stdin\n");
    return false;
  };
  if (!gsIsSpecV1(z)){
    gsErrorMsg("lpsconstelm: Stdin does not contain an LPS\n");
    return false;
  }
  p_spec = specification(z);
  //gsDebugMsg("%s\n", pp(p_spec.process()).c_str());
  return true;
}

// Writes file to stdout
//
void lpsConstElm::writeStream(lps::specification newSpec) {
  assert(gsIsSpecV1((ATermAppl) newSpec));
  ATwriteToBinaryFile(aterm(newSpec) , stdout);
}

// Sets verbose option
// Note: Has to be set
//
inline void lpsConstElm::setVerbose(bool b) {
  if (b)
  {
    gsSetVerboseMsg();
  }
  p_verbose = b;
}

// Sets debug option
// Note: Has to be set
//
inline void lpsConstElm::setDebug(bool b) {
  setVerbose(b);
  if (b)
  {
    gsSetDebugMsg();
  }
}

// Sets no singleton option
// Note: Has to be set
//  
inline void lpsConstElm::setNoSingleton(bool b) {
  p_nosingleton = b;
}

// Sets all conditions to true
// Note: Has to be set
//  
inline void lpsConstElm::setAllTrue(bool b) {
  p_alltrue = b;
}

// Sets the option if not inspected summands have to removed 
// Note: Has to be set
//  
inline void lpsConstElm::setReachable(bool b) {
  p_reachable = b;
}

// Prints the data_variable which are constant
//  
void lpsConstElm::printSetVar() {
  printState();
}  

// The constelm filter
//
void lpsConstElm::filter() {

  //---------------------------------------------------------------
  //---------------------   Init begin   --------------------------
  //---------------------------------------------------------------

  bool    same        ;
  bool    foundFake = true;
  int     counter  = 0;
  int     cycle    = 0;
  p_newVarCounter  = 0;
  
  lps::linear_process p_process = p_spec.process();
  rewr           = createRewriter(p_spec.data()); 

  data_assignment_list initial_assignments = p_spec.initial_process().assignments();
  for(lps::data_assignment_list::iterator i = initial_assignments.begin(); i != initial_assignments.end() ; i++ ){
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

  data_variable_list initial_free_variables = p_spec.initial_process().free_variables();
  for (data_variable_list::iterator di = initial_free_variables.begin(); di != initial_free_variables.end(); di++){
    p_freeVarSet.insert(*di);
  }
  for (data_variable_list::iterator di = p_process.free_variables().begin(); di != p_process.free_variables().end(); di++){
    p_freeVarSet.insert(*di);
  } 
  
  // Make a set containing all summation variables (for detectVar)
  summand_list sums = p_spec.process().summands();
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

  int n = p_spec.initial_process().assignments().size();

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
      gsVerboseMsg("lpsconstelm: Cycle %d: \n", cycle++);
      //int summandnr = 1;
      for(summand_list::iterator currentSummand = p_process.summands().begin(); currentSummand != p_process.summands().end() ;currentSummand++ ){
        if ( (p_visitedSummands.find(*currentSummand) != p_visitedSummands.end()) || (conditionTest(currentSummand->condition()))) {
          //gsDebugMsg(  "Summand: %d\n", summandnr++);
          p_visitedSummands.insert(*currentSummand); 
          //----------          Debug
          // printCurrentState();
          calculateNextState(currentSummand->assignments());
          //----------          Debug  
          // printCurrentState(); 
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
      detectVar(p_process.process_parameters().size());
      int diff = p_V.size()-n;
      if ( diff == 1 )
      {
        gsVerboseMsg("lpsconstelm: reset 1 parameter to variable because its value contained summation variables\n");
      } else if ( diff > 1 )
      {
        gsVerboseMsg("lpsconstelm: reset %d parameters to variable because their values contained summation variables\n", diff);
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
      gsVerboseMsg("lpsconstelm: Free Variable checkup:\n");
    
      int n = p_V.size();
      for(std::set< summand>::iterator i = p_visitedSummands.begin(); i != p_visitedSummands.end() ; i++){
        calculateNextState(i->assignments());
        cmpCurrToNext();
      }

      p_currentState = p_newCurrentState;
      if (p_verbose){
        gsVerboseMsg("lpsconstelm:   Detected %d fake constant process parameters\n", p_V.size() - n);
        foundFake = ((p_V.size() - n) != 0);
      }         

    }
    
  }
  
  //---------------------------------------------------------------

  //Singleton sort process parameters
  //
  if(p_nosingleton){
     int n = p_V.size();
     removeSingleton(p_process.process_parameters().size());
     int diff = p_V.size()-n;
     if ( diff > 1 )
     {
       gsVerboseMsg("lpsconstelm: %d constant parameters are not removed because their sorts contain only one element\n", diff);
     } else if ( diff == 1 )
     {
       gsVerboseMsg("lpsconstelm: 1 constant parameter is not removed because its sort contains only one element\n");
     }
  }    
  
  //---------------------------------------------------------------
  // Construct S    
  //
  std::set< int > S;
  n = p_process.process_parameters().size(); 
  for(int j=0; j < n ; j++){
    S.insert(j);
  };
  set_difference(S.begin(), S.end(), p_V.begin(), p_V.end(), inserter(p_S, p_S.begin()));
  
  if (p_verbose){
    gsVerboseMsg("lpsconstelm: Number of removed process parameters: %d\n", p_S.size());
    printSetVar();  
  }
}

// Gets the version of the tool
//    
inline std::string lpsConstElm::getVersion() {
  return (version);
}

void parse_command_line(int ac, char** av, lpsConstElm& constelm) {
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
              << "Removes constant process parameters from the LPS read from standard input or INFILE." << std::endl
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
    std::cerr << "lpsconstelm: Specify only INPUT and/or OUTPUT file (too many arguments)."<< std::endl;

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
  if (!mcrl2_squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
#endif
    lpsConstElm constelm;

    parse_command_line(argc,argv,constelm);

    constelm.filter();
    constelm.output(); 
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif

  return 0;
}
