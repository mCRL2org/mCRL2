/// Author(s): F.P.M. (Frank) Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./lpsconstelm.cpp

#include "boost.hpp" // precompiled headers

#define NAME "lpsconstelm"
#define AUTHOR "Frank Stappers"

//LPS framework
#include "mcrl2/lps/specification.h"

//C/C++
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <iterator>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <fstream>

//Rewriter
#include "mcrl2/data/rewrite.h"

//Aterm
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/map.h"
#include "mcrl2/atermpp/vector.h"

#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/command_line_rewriting.h"
#include "mcrl2/utilities/command_line_messaging.h"

using namespace atermpp;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;
using namespace mcrl2::lps;

template <class T>
inline std::string to_string (const T& t)
{
  std::stringstream ss;
  ss << t;
  return ss.str();
}

// Squadt protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/mcrl2_squadt_interface.h>
#endif

class lpsConstElm
#ifdef ENABLE_SQUADT_CONNECTIVITY
: public mcrl2::utilities::squadt::mcrl2_tool_interface
#endif
                  {

  private:
    ATermTable                            safeguard;
    std::string                           p_outputfile;
    atermpp::vector< mcrl2::data::data_assignment >   p_currentState;
    atermpp::vector< mcrl2::data::data_assignment >   p_newCurrentState;
    atermpp::vector< mcrl2::data::data_assignment >   p_nextState;
    atermpp::vector< mcrl2::data::data_assignment >   p_initAssignments;
    atermpp::map< mcrl2::data::data_variable, int  >  p_lookupIndex;
    atermpp::map< int, mcrl2::data::data_variable >   p_lookupDataVarIndex;
    atermpp::set< mcrl2::data::data_expression >      p_freeVarSet;
    atermpp::set< int >                       p_V;
    atermpp::set< int >                       p_S;
    atermpp::set< mcrl2::lps::summand >          p_visitedSummands;
    atermpp::set< mcrl2::data::data_expression >      p_variableList;
    atermpp::map< int, atermpp::vector< std::pair< mcrl2::data::data_assignment, mcrl2::data::data_assignment > > > p_changed_assignements_per_cycle;
    int                                   p_newVarCounter;
    bool                                  p_nosingleton;
    bool                                  p_alltrue;
    bool                                  p_reachable;
    bool                                  p_show;
    std::string                           p_logfile;
    std::string                           p_logstring;
    mcrl2::lps::specification                    p_spec;
    atermpp::set< sort_expression >       p_singletonSort;
    Rewriter*                             rewr;
    //rewrite strategy used by the rewriter.
    RewriteStrategy p_strategy;

    atermpp::map< mcrl2::data::data_variable, atermpp::vector < mcrl2::data::data_expression > > p_parameter_trace ;
    int p_cycle;

    //Only used by getDataVarIDs
    atermpp::set< mcrl2::data::data_variable >        p_foundFreeVars;

    //Only used by detectVar
    atermpp::set< mcrl2::data::data_expression >      sum_vars;

  public:

    lpsConstElm();
    ~lpsConstElm();

  private:

    bool parse_command_line(int argc, char** argv);
    void getDatVarRec(aterm_appl t);
    atermpp::set< mcrl2::data::data_variable > getUsedFreeVars(aterm_appl input);
    ATermAppl rewrite(ATermAppl t);
    ATermAppl p_substitute(ATermAppl t, atermpp::vector< mcrl2::data::data_assignment > &y );
    void calculateNextState(mcrl2::data::data_assignment_list assignments);
    bool inFreeVarList(mcrl2::data::data_expression dexpr);
    mcrl2::data::data_assignment newExpression(mcrl2::data::data_assignment ass);
    bool compare(mcrl2::data::data_expression x, mcrl2::data::data_expression y);
    bool cmpCurrToNext();
    bool conditionTest(mcrl2::data::data_expression x);
    void detectVar(int n);
    bool recDetectVarList(mcrl2::data::data_expression_list l, atermpp::set<data_expression> &S);
    bool recDetectVar(mcrl2::data::data_expression t, atermpp::set<mcrl2::data::data_expression> &S);
    bool DetectVariableProcessParamtersInDataAppl (aterm_appl t);
    template <typename Term>
    atermpp::term_list<Term> vectorToList(atermpp::vector<Term> y);
    template <typename Term>
    atermpp::term_list<Term> setToList(atermpp::set<Term> y);
    void findSingleton();

    void printNextState();
    void printVar();
    void printState();
    void printCurrentState();

  public:

    bool execute(int argc, char** argv);
    void removeSingleton(int n);
    bool output();
    void loadFile(std::string const& x);
    void setSaveFile(std::string const& x);
    void printSet();
    void setVerbose(bool b);
    void setDebug(bool b);
    void setNoSingleton(bool b);
    void setAllTrue(bool b);
    void setRewriteStrategy(RewriteStrategy s);
    void setReachable(bool b);
    void setShow(bool b);
    void printSetVar();
    bool filter();

#ifdef ENABLE_SQUADT_CONNECTIVITY
    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities& c) const {
      c.add_input_configuration("lps_in", tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::transformation);
    }

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration&);

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const& c) const {
      return c.input_exists("lps_in") && c.output_exists("lps_out");
    }

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration&);
#endif
};

#ifdef ENABLE_SQUADT_CONNECTIVITY
char const* option_remove_single_element_sorts = "remove_single_element_sorts";
char const* option_remove_unvisited_summands   = "remove_unvisited_summands";
char const* option_ignore_summand_conditions   = "ignore_summand_conditions";
char const* option_rewrite_strategy            = "rewrite_strategy";

void lpsConstElm::user_interactive_configuration(tipi::configuration& c) {
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;

  /* Set defaults where the supplied configuration does not have values */
  if (!c.output_exists("lps_out")) {
    /* Add output file to the configuration */
    c.add_output("lps_out", tipi::mime_type("lps", tipi::mime_type::application), c.get_output_name(".lps"));
  }

  if (!c.option_exists(option_remove_single_element_sorts)) {
    c.add_option(option_remove_single_element_sorts, false).
        set_argument_value< 0 >(true, false);
  }
  if (!c.option_exists(option_remove_unvisited_summands)) {
    c.add_option(option_remove_unvisited_summands, false).
        set_argument_value< 0 >(true, false);
  }
  if (!c.option_exists(option_ignore_summand_conditions)) {
    c.add_option(option_ignore_summand_conditions, false).
        set_argument_value< 0 >(true, false);
  }

  /* Create display */
  tipi::tool_display d;

  tipi::layout::vertical_box& m = d.create< vertical_box >();

  mcrl2::utilities::squadt::radio_button_helper < RewriteStrategy > strategy_selector(d);

  m.append(d.create< label >().set_text("Rewrite strategy")).
    append(d.create< horizontal_box >().
                append(strategy_selector.associate(GS_REWR_INNER, "Inner")).
#ifdef MCRL2_INNERC_AVAILABLE
                append(strategy_selector.associate(GS_REWR_INNERC, "Innerc")).
#endif
#ifdef MCRL2_JITTYC_AVAILABLE
                append(strategy_selector.associate(GS_REWR_JITTY, "Jitty", true)).
                append(strategy_selector.associate(GS_REWR_JITTYC, "Jittyc")));
#else
                append(strategy_selector.associate(GS_REWR_JITTY, "Jitty", true)));
#endif

  checkbox& remove_single_element_sorts = d.create< checkbox >().
                        set_status(c.get_option_argument< bool >(option_remove_single_element_sorts));
  checkbox& remove_unvisited_summands   = d.create< checkbox >().
                        set_status(c.get_option_argument< bool >(option_remove_unvisited_summands));
  checkbox& ignore_summand_conditions   = d.create< checkbox >().
                        set_status(c.get_option_argument< bool >(option_ignore_summand_conditions));

  m.append(d.create< vertical_box >().set_default_alignment(layout::left).
      append(remove_single_element_sorts.set_label("remove single element sorts")).
      append(remove_unvisited_summands.set_label("remove summands that are not visited")).
      append(ignore_summand_conditions.set_label("take summand conditions into account")),
    margins(0,5,0,5));

  if (c.option_exists(option_rewrite_strategy)) {
    strategy_selector.set_selection(c.get_option_argument< RewriteStrategy >(option_rewrite_strategy, 0));
  }

  button& okay_button = d.create< button >().set_label("OK");

  m.append(d.create< label >().set_text(" ")).
    append(okay_button, layout::right);

  send_display_layout(d.manager(m));

  /* Wait until the ok button was pressed */
  okay_button.await_change();

  /* Update configuration */
  c.get_option(option_remove_single_element_sorts).
      set_argument_value< 0 >(remove_single_element_sorts.get_status());
  c.get_option(option_remove_unvisited_summands).
      set_argument_value< 0 >(remove_unvisited_summands.get_status());
  c.get_option(option_ignore_summand_conditions).
      set_argument_value< 0 >(ignore_summand_conditions.get_status());
  c.get_option(option_rewrite_strategy).
      set_argument_value< 0 >(strategy_selector.get_selection());

  send_clear_display();
}

bool lpsConstElm::perform_task(tipi::configuration& c) {
  /* Set with options from the current configuration object */
  setNoSingleton(c.option_exists(option_remove_single_element_sorts));
  setReachable(c.option_exists(option_remove_unvisited_summands));
  setAllTrue(c.option_exists(option_remove_unvisited_summands));
  setRewriteStrategy(c.get_option_argument< RewriteStrategy >(option_rewrite_strategy, 0));

  send_hide_display();

  loadFile(c.get_input("lps_in").location());
  setSaveFile(c.get_output("lps_out").location());

  filter();
  output();

  return true;
}
#endif

lpsConstElm::lpsConstElm() : safeguard(ATtableCreate(10000,50)), rewr(NULL) {
  p_spec.protect();
}

lpsConstElm::~lpsConstElm() {
  p_spec.unprotect();
  delete rewr;
  ATtableDestroy(safeguard);
}

void lpsConstElm::getDatVarRec(aterm_appl t) {
  if( gsIsDataVarId(t) )
    {
      if ( !p_freeVarSet.empty() &&  (p_freeVarSet.find(data_variable(t)) != p_freeVarSet.end())){
      p_foundFreeVars.insert(t);
    }
  };

  if(gsIsDataAppl(t) )
  {
    getDatVarRec( ATAgetArgument( t, 0 ) );

    ATermList z = (ATermList) ATgetArgument(t,1);

    while (!ATisEmpty(z))
    {
      getDatVarRec( (aterm_appl) ATgetFirst(z) );
      z = ATgetNext(z);
    }
  }
}


bool lpsConstElm::DetectVariableProcessParamtersInDataAppl (aterm_appl t) {
// Returns true if a variable process parameter is detected
// Returns false all process parameters are constant
  if( gsIsDataVarId(t) )
    {
      return (p_variableList.find(data_variable(t)) != p_variableList.end());
    }

  if(gsIsDataAppl(t) )
  {
    bool returnvalue = DetectVariableProcessParamtersInDataAppl( ATAgetArgument( t, 0 ) );

    ATermList z = (ATermList) ATgetArgument(t,1);

    while (!ATisEmpty(z) && !returnvalue )
    {
      returnvalue = returnvalue || DetectVariableProcessParamtersInDataAppl( (aterm_appl) ATgetFirst(z) );
      z = ATgetNext(z);
    }

    return returnvalue;
  }

  return false;
}

// Returns a vector in which each element is a AtermsAppl (DataVarID)
//
inline atermpp::set< mcrl2::data::data_variable > lpsConstElm::getUsedFreeVars(aterm_appl input) {
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
inline ATermAppl lpsConstElm::p_substitute(ATermAppl t, atermpp::vector< mcrl2::data::data_assignment > &y ) {
  for(atermpp::vector< mcrl2::data::data_assignment >::iterator i = y.begin() ; i != y.end() ; i++){
    rewr->setSubstitution(i->lhs() ,i->rhs());
  }
  ATermAppl result = rewr->rewrite(t);
  for(atermpp::vector< mcrl2::data::data_assignment >::iterator i = y.begin() ; i != y.end() ; i++){
    rewr->clearSubstitution(i->lhs());
  }
  return result;
}
// calculates a nextstate given the current
// stores the next state information in p_nextState
//
inline void lpsConstElm::calculateNextState(data_assignment_list assignments) {
  for(atermpp::vector< mcrl2::data::data_assignment >::iterator i = p_currentState.begin(); i != p_currentState.end(); i++ ){
    int index = p_lookupIndex[i->lhs()];
    if (p_V.find(index) == p_V.end()){
      mcrl2::data::data_expression tmp = i->lhs();
      for (mcrl2::data::data_assignment_list::iterator j = assignments.begin(); j != assignments.end() ; j++){
        if (j->lhs() == i->lhs()){
          tmp = j->rhs();
          break;
        }
      }
      p_nextState.at(index) = mcrl2::data::data_assignment(i->lhs(), p_substitute(tmp, p_currentState));
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
inline mcrl2::data::data_assignment lpsConstElm::newExpression(mcrl2::data::data_assignment ass) {
  std::ostringstream s;
  s << ass.lhs().name() << "^" << p_newVarCounter++;
  data_variable w(s.str() , ass.lhs().sort() );
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
  return (p_alltrue || (!(data_expr::is_false(data_expression(rewrite(data_expression(p_substitute(x, p_currentState))))))));
}

// returns whether the currentState and NextState differ
//
inline bool lpsConstElm::cmpCurrToNext() {
  bool differs = false;
  for(atermpp::vector< mcrl2::data::data_assignment>::iterator i= p_currentState.begin(); i != p_currentState.end() ;i++){
    int index = p_lookupIndex[i->lhs()];
    if (p_V.find(index) == p_V.end()) {
      if (inFreeVarList(i->rhs())) {
        if (!inFreeVarList( p_nextState.at(index).rhs() )){
          ATtablePut(safeguard,aterm(p_nextState.at(index)),aterm(p_nextState.at(index)));
          p_newCurrentState.at(index) = p_nextState.at(index) ;
          p_currentState.at(index) = p_nextState.at(index);

          if (DetectVariableProcessParamtersInDataAppl( (ATermAppl) p_nextState.at(index).rhs()))
          {
            p_V.insert(p_lookupIndex[i->lhs()]);

            if (p_show)
            {
              p_changed_assignements_per_cycle[p_cycle].push_back(std::make_pair(*i, p_nextState.at(index)));
            }

            //----------          Debug
            // gsVerboseMsg("\033[34m OLD:    %s\n", pp(*i).c_str());
            // gsVerboseMsg("\033[32m NEW:    %s\033[0m\n", pp(p_nextState.at(index)).c_str());
            //----------          Debug
          }
        }
      } else {
        if (!inFreeVarList( p_nextState.at(index).rhs() )){
           if (!compare(i->rhs(), p_nextState.at(index).rhs())){
              if (p_show)
              {
                p_changed_assignements_per_cycle[p_cycle].push_back(std::make_pair(*i, p_nextState.at(index)));
              }
              //----------          Debug
              // gsVerboseMsg("\033[34m OLD:    %s\n", pp(*i).c_str());
              // gsVerboseMsg("\033[32m NEW:    %s\033[0m\n", pp(p_nextState.at(index)).c_str());
              // gsVerboseMsg("\033[33m Different  \033[0m\n");
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
   // ...that is marked  as a constant (i.e. that is not in the set of
   // variable parameters)...
   if ( p_V.find(i) == p_V.end() ) {
     // ...and contains a summation variable...
     data_expression t = p_currentState.at(i).rhs();
     if ( recDetectVar(t, sum_vars) || recDetectVar(t, p_variableList) ) {
       // ...is actually a non constant parameter
       p_V.insert(i);
     }
   }
 }
}

// Return whether or not a summation variable occurs in a data term list
bool lpsConstElm::recDetectVarList(mcrl2::data::data_expression_list l, atermpp::set<data_expression> &S) {
  //gsVerboseMsg("list: %s\n", l.to_string().c_str());
  bool b = false;
  for(data_expression_list::iterator i = l.begin(); i != l.end() && !(b); ++i) {
    b = b || recDetectVar(aterm_appl(*i), S);
  }
  return b;
}

// Return whether or not a summation variable occurs in a data term
bool lpsConstElm::recDetectVar(mcrl2::data::data_expression t, atermpp::set<data_expression> &S) {
   //gsVerboseMsg("expr: %s\n", t.to_string().c_str());
   bool b = false;
   if( gsIsDataVarId(t) && (S.find(t) != S.end()) ){
     b = true;
   }
   if ( gsIsDataAppl(t) ) {
     b = b || recDetectVar(data_expression(t.argument(0)), S);
     b = b || recDetectVarList(data_expression_list(t.argument(1)), S);
   }
   return b;
}

// template for changing a vector into a list
//
template <typename Term>
inline atermpp::term_list<Term> lpsConstElm::vectorToList(atermpp::vector<Term> y) {
  term_list<Term> result;
  for(typename atermpp::vector<Term>::iterator i = y.begin(); i != y.end() ; i++)
    {
      result = push_front(result,*i);
    }
  return atermpp::reverse(result);
}

// template for changing a set into a list
//
template <typename Term>
inline term_list<Term> lpsConstElm::setToList(atermpp::set<Term> y) {
  term_list<Term> result;
  for(typename atermpp::set<Term>::iterator i = y.begin(); i != y.end() ; i++)
    {
      result = push_front(result,*i);
    }
  return atermpp::reverse(result);
}

// Find all sorts which have a singleton domain
//
void lpsConstElm::findSingleton() {

  atermpp::map< sort_expression, int >     p_countSort;
  //set< sort_expression > result;
  for(mcrl2::data::sort_expression_list::iterator i = p_spec.data().sorts().begin(); i != p_spec.data().sorts().end() ; i++){
    p_countSort[*i] = 0;
    p_singletonSort.insert(*i);
  }

  for(mcrl2::data::data_operation_list::iterator i= p_spec.data().constructors().begin() ; i != p_spec.data().constructors().end() ; i++){
    p_countSort[result_sort(i->sort())]++;
  }

  unsigned int n = p_singletonSort.size()+1;
  while (n != p_singletonSort.size()){
    n = p_singletonSort.size();
    for(sort_expression_list::iterator i = p_spec.data().sorts().begin(); i != p_spec.data().sorts().end() ; i++){
      int b = 1;
      //if p_countSort[*i] == 0 then there are sorts declared which are never used!!!!
//      assert(p_countSort[*i] != 0);

      if (p_countSort[*i] == 1){
        for(data_operation_list::iterator j = p_spec.data().constructors().begin() ; j != p_spec.data().constructors().end() ;j++){
          if (result_sort(j->sort()) == *i){
            sort_expression_list sorts = domain_sorts(j->sort());
            for(sort_expression_list::iterator k = sorts.begin() ; k != sorts.end() ; k++ ){
              b = (std::max)(p_countSort[*k], b);
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

  if (gsVerbose){
    gsVerboseMsg("lpsconstelm: Sorts which have singleton constructors:\n");
    for(atermpp::set<sort_expression>::iterator i = p_singletonSort.begin(); i != p_singletonSort.end(); i++){
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
  for(atermpp::vector< mcrl2::data::data_assignment >::iterator i = p_nextState.begin(); i != p_nextState.end() ; i++ ){
    result << "[" << pp(*i) << "]";
  }
  gsVerboseMsg("%s\n", result.str().c_str());
}

inline void lpsConstElm::printVar() {
// (JK: 16/1/2006: This function is never called)
  std::ostringstream result;
  result << "lpsconstelm: Variable indices : {";
  atermpp::set< int >::iterator i = p_V.begin();
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
  if ( !p_S.empty() )
  {
    result << "lpsconstelm: [ ";
    for(std::set< int >::iterator i = p_S.begin(); i != p_S.end() ; i++ ){
      if (i != p_S.begin())
      {
        result << ", ";
      }
      if (!p_nosingleton){
        result << pp(p_currentState[*i]) ;
      } else {
        result << pp(p_currentState[*i]) << pp(p_currentState[*i].lhs().sort()) ;
      }
    }
    result << " ]";
  }
  gsVerboseMsg("%s\n", result.str().c_str());
}

inline void lpsConstElm::printCurrentState() {
// (JK: 16/1/2006: This function is never called)
  std::ostringstream result;
  for(atermpp::vector< mcrl2::data::data_assignment >::iterator i = p_currentState.begin(); i != p_currentState.end() ; i++ ){
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
  sort_expression_list rebuild_sort = p_spec.data().sorts();
  findSingleton();
  gsVerboseMsg("lpsconstelm: Constant process parameters which are not substituted and removed [--no-singleton]:\n");
  for(int i = 0; i < n; i++)
  {
    if ( (p_V.find(i) == p_V.end()) &&
         (p_singletonSort.find(p_initAssignments[i].lhs().sort()) != p_singletonSort.end())
       )
    {
      p_V.insert(i);
      if (gsVerbose){
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
inline bool lpsConstElm::output() {
  linear_process p_process = p_spec.process();
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

  atermpp::set< mcrl2::data::data_variable > constantVar;
  for(atermpp::set< int >::iterator i = p_S.begin(); i != p_S.end(); i++){
    constantVar.insert(p_initAssignments.at(*i).lhs());
  }

  atermpp::vector< mcrl2::data::data_assignment > constantPP;
  for(std::set< int >::iterator i = p_S.begin(); i != p_S.end(); i++){
    constantPP.push_back(p_currentState.at(*i));
  }

  atermpp::vector< mcrl2::data::data_variable > variablePPvar;
  for(atermpp::set< int >::iterator i = p_V.begin(); i != p_V.end(); i++){
    variablePPvar.push_back(p_initAssignments.at(*i).lhs());
  }

  atermpp::vector< mcrl2::data::data_expression > variablePPexpr;
  for(atermpp::set< int >::iterator i = p_V.begin(); i != p_V.end(); i++){
    variablePPexpr.push_back(p_initAssignments.at(*i).rhs());
  }

  if (p_show)
  {
    p_logstring.append( "\"Cycle\"");
    for (int i = 0; i != p_cycle; ++i)
    {
      p_logstring.append( "\t\"" + to_string( i ) + "\"" ) ;
    }
    p_logstring.append( "\n" );

    for ( atermpp::map< mcrl2::data::data_variable, atermpp::vector< mcrl2::data::data_expression > >::iterator i = p_parameter_trace.begin()
        ; i != p_parameter_trace.end()
        ; ++i  )
    {
      p_logstring.append( "\"" + pp(i->first) + "\"" ) ;

      int count = 0;
      bool changed = false;

      for ( atermpp::vector< mcrl2::data::data_expression >::iterator j = i->second.begin()
          ; j != i->second.end()
          ; ++j )
      {
        bool printed = false;
        atermpp::map< int, atermpp::vector< std::pair< mcrl2::data::data_assignment, mcrl2::data::data_assignment > > >::iterator it_capc = p_changed_assignements_per_cycle.find(count) ;

        if ( it_capc != p_changed_assignements_per_cycle.end() )
        {
          if ( !(it_capc->second.empty()) )
          {

            for ( atermpp::vector< std::pair< mcrl2::data::data_assignment, mcrl2::data::data_assignment > >::iterator  it_ca  = it_capc->second.begin()
                  ; it_ca != it_capc->second.end()
                  ; ++it_ca )
            {
              if ( i->first == it_ca->first.lhs() )
              {
                p_logstring.append( it_ca->first.rhs() == it_ca->second.rhs()
                    ? "\t\""+ pp(it_ca->second.rhs()) + "\""
                    : "\t\"["+ pp(it_ca->first.rhs()) + " ? "+ pp(it_ca->second.rhs()) + "\"]" );
                changed = printed = true;
              }
            }
          }
        }
        if (!printed)
        {
          if (changed)
          {
            p_logstring.append( "\t\"Not Constant\"");
          } else {
            p_logstring.append( "\t\"" + pp(*j) + "\"" );
          }

        }
        ++count;
      }
      p_logstring.append( "\n" );
    }

   //
   // Write to file
   //

   if (p_show)
   {
     FILE * pFile;

     pFile = fopen (p_logfile.c_str() ,"w");

     if (pFile == NULL)
     {
       gsVerboseMsg("lpsconstelm: Cannot write to the CSV log-file: %s. Writing of the log-file is omitted.\n", p_logfile.c_str() );
     } else {
       fprintf (pFile, "%s", p_logstring.c_str() );
       fclose (pFile);
     }
    }

  }
  //Remove process parameters in in summand
  //
  summand_list rebuild_summandlist_no_cp;

  for(summand_list::iterator currentSummand = rebuild_summandlist.begin(); currentSummand != rebuild_summandlist.end(); currentSummand++){

    //construct new LPS_summand
    //
    summand tmp;

    //Remove constant process parameters from the summands assignments
    //
    mcrl2::data::data_assignment_list rebuildAssignments;
    for(mcrl2::data::data_assignment_list::iterator currentAssignment = currentSummand->assignments().begin(); currentAssignment != currentSummand->assignments().end() ; currentAssignment++){
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
      rebuild_actions = push_front(rebuild_actions, action( i -> label(), atermpp::reverse(argumentList)));
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

  //Move the 'constant' free variables to the set of usedFreeVars
  mcrl2::data::data_variable_list lps_init_free_vars = p_spec.initial_process().free_variables();
  atermpp::set <mcrl2::data::data_variable> init_free_vars;
  for(mcrl2::data::data_variable_list::iterator i = lps_init_free_vars.begin(); i != lps_init_free_vars.end(); ++i)
  {
    init_free_vars.insert(*i);
  }

  // atermpp::set< mcrl2::data::data_variable > usedFreeVars = (atermpp::set) p_process.find_free_variables();
  // begin fix
  std::set< mcrl2::data::data_variable > tmp_free_vars = p_process.find_free_variables();
  atermpp::set< mcrl2::data::data_variable > usedFreeVars;
  for(std::set< mcrl2::data::data_variable >::iterator i = tmp_free_vars.begin(); i != tmp_free_vars.end() ;++i )
  {
    usedFreeVars.insert( *i );
  }
  // end fix

  for(atermpp::vector< mcrl2::data::data_assignment >::iterator i = constantPP.begin(); i != constantPP.end(); i++)
  {
    if(is_data_variable(i->rhs()) && !init_free_vars.empty() && init_free_vars.find(i->rhs()) != init_free_vars.end() )
    {
      usedFreeVars.insert(i->rhs());
    }
  }

  //exit(0);

  //construct new specfication
  //
  //linear_process(data_variable_list free_variables, data_variable_list process_parameters,
  //  summand_list summands);
  linear_process rebuild_process;
  rebuild_process = linear_process(
    setToList(usedFreeVars),
    vectorToList(variablePPvar),
    atermpp::reverse(rebuild_summandlist_no_cp)
  );

  //gsDebugMsg("%s\n", p_spec.initial_process().free_variables().to_string().c_str());

  atermpp::set< mcrl2::data::data_variable > foundVars;
  atermpp::set< mcrl2::data::data_variable > initial_free_variables;
  usedFreeVars.empty();

  for(atermpp::vector< mcrl2::data::data_expression >::iterator i = variablePPexpr.begin(); i != variablePPexpr.end(); i++){
     foundVars = getUsedFreeVars(aterm_appl(*i));
     for(atermpp::set< mcrl2::data::data_variable >::iterator k = foundVars.begin(); k != foundVars.end(); k++){
       initial_free_variables.insert(*k);
     }
  }

  // Rebuild spec
  //
  //specification(sort_expression_list sorts, data_operation_list constructors,
  //            data_operation_list mappings, data_equation_list equations,
  //            action_label_list action_labels, LPS lps,
  //            data_variable_list initial_free_variables,
  //            data_variable_list initial_variables,
  //            data_expression_list initial_state);
  //
//data_assignment_expressions(data_assignment_list());
//data_assignment_list xyz = make_assignment_list(data_variable_list(), data_expression_list());
    specification rebuild_spec = specification(
    p_spec.data(),
    p_spec.action_labels(),
    rebuild_process,
    process_initializer(setToList(initial_free_variables),
                        make_assignment_list(vectorToList(variablePPvar),
                                             vectorToList(variablePPexpr)
                                            )
                       )
  );

  gsVerboseMsg("lpsconstelm: Number of process parameters in the old LPS: %d\n", p_process.process_parameters().size());
  gsVerboseMsg("lpsconstelm: Number of process parameters in the new LPS: %d\n", rebuild_process.process_parameters().size());

  if (constantPP.begin() != constantPP.end())
  {
    gsVerboseMsg("lpsconstelm: ===== Replacements =====\n");
    for(atermpp::vector< mcrl2::data::data_assignment >::iterator i = constantPP.begin(); i != constantPP.end(); ++i)
    {
      gsVerboseMsg("lpsconstelm: constant process parameter \"%s\" is replaced by \"%s\"\n", pp(i->lhs()).c_str(), pp(i->rhs()).c_str());
    }
  }
  assert(gsIsLinProcSpec((ATermAppl) rebuild_spec));

  rebuild_spec.save(p_outputfile);

  return true;
}

// Load file
//
inline void lpsConstElm::loadFile(std::string const& x) {
  p_spec.load(x);
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

// Sets no singleton option
// Note: Has to be set
//
inline void lpsConstElm::setNoSingleton(bool b) {
  p_nosingleton = b;
}

inline void lpsConstElm::setRewriteStrategy(RewriteStrategy s) {
  p_strategy = s;
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

// If p_show is true, a change trace for each parameter parameter is given
// Note: Has to be set
//
inline void lpsConstElm::setShow(bool b) {
  p_show = b;
}

// Prints the data_variable which are constant
//
void lpsConstElm::printSetVar() {
  printState();
}

// The constelm filter
//
bool lpsConstElm::filter() {

  //---------------------------------------------------------------
  //---------------------   Init begin   --------------------------
  //---------------------------------------------------------------

  bool    same        ;
  bool    foundFake = true;
  int     counter  = 0;
  p_cycle = 0;
  p_newVarCounter  = 0;

  linear_process p_process = p_spec.process();
  rewr           = createRewriter(p_spec.data(), p_strategy);

  data_assignment_list initial_assignments = p_spec.initial_process().assignments();

  if (p_show)
  {

    for( data_assignment_list::iterator i = initial_assignments.begin()
       ; i != initial_assignments.end()
       ; ++i
       )
    {
      p_parameter_trace[i->lhs()].push_back( i->rhs());
    }
  }


  for(mcrl2::data::data_assignment_list::iterator i = initial_assignments.begin(); i != initial_assignments.end() ; i++ ){
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
      gsVerboseMsg("lpsconstelm: Cycle %d\n", p_cycle++);
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

      if (p_show)
      {
         for( atermpp::vector< mcrl2::data::data_assignment >::iterator i = p_currentState.begin()
            ; i != p_currentState.end()
            ; ++i
            )
        {
          p_parameter_trace[i->lhs()].push_back( i->rhs());
        }
      }

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
        gsVerboseMsg("lpsconstelm: 1 parameter is marked constant. The parameter contains is a sum variable. Therefore the parameter is not substituted.\n");
      } else if ( diff > 1 )
      {
        gsVerboseMsg("lpsconstelm: %d parameters are marked constant. The parameters contain sum variables. Therefore the parameters are not substituted.\n", diff);
      }
    }

    //---------------------FreeVar aftercheck-------------------------
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
    // The arrow is shows the detection of a constant variable in the FreeVar aftercheck

    if(!p_freeVarSet.empty()){
      gsVerboseMsg("lpsconstelm: Free Variable checkup:\n");

      int n = p_V.size();
      for(atermpp::set< summand>::iterator i = p_visitedSummands.begin(); i != p_visitedSummands.end() ; i++){
        calculateNextState(i->assignments());
        cmpCurrToNext();
      }

      p_currentState = p_newCurrentState;
      if (gsVerbose){
        gsVerboseMsg("lpsconstelm: Detected %d fake constant process parameters\n", p_V.size() - n);
        foundFake = ((p_V.size() - n) != 0);
      }

    }

    p_cycle++;

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

  if (gsVerbose){
    gsVerboseMsg("lpsconstelm: Number of removed process parameters: %d\n", p_S.size());
    printSetVar();
  }

  return true;
}

bool lpsConstElm::parse_command_line(int ac, char** av) {
  interface_description clinterface(av[0], NAME, AUTHOR,
    "remove constant parameters from an LPS",
    "[OPTION]... [INFILE [OUTFILE]]\n",
    "Remove constant process parameters from the LPS in INFILE and write the result "
    "to OUTFILE. If OUTFILE is not present, stdout is used. If INFILE is not present, "
    "stdin is used.");

  clinterface.add_option("no-singleton", "do not process parameters of which the sort contains a single element");
  clinterface.add_option("no-condition", "treat all summand conditions as true (faster)");
  clinterface.add_option("no-reachable", "do not remove summands that are not visited");
  clinterface.add_option("csv", make_mandatory_argument("NAME"), "stores the value changes of the process parameters in the CSV file NAME", 'c');
  clinterface.add_rewriting_options();

  command_line_parser parser(clinterface, ac, av);

  if (parser.continue_execution()) {
    setNoSingleton(0 < parser.options.count("no-singleton"));
    setAllTrue(0 < parser.options.count("no-condition"));
    setReachable(0 == parser.options.count("no-reachable"));

    p_strategy = parser.option_argument_as< RewriteStrategy >("rewriter");

    if(parser.options.count("csv"))
    {
      setShow(true);

      p_logfile = parser.option_argument("csv");

      gsDebugMsg( "Output file %s", p_logfile.c_str() );
    } else {
      setShow(false);
    }

    std::string name_for_input;
    std::string name_for_output;

    if (2 < parser.arguments.size()) {
      parser.error("too many file arguments");
    }
    else {
      if (0 < parser.arguments.size()) {
        name_for_input = parser.arguments[0];
      }
      if (1 < parser.arguments.size()) {
        name_for_output = parser.arguments[1];
      }
    }
    loadFile(name_for_input);
    setSaveFile(name_for_output);
  }

  return parser.continue_execution();
}

bool lpsConstElm::execute(int argc, char** argv) {
#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (mcrl2::utilities::squadt::free_activation(*this, argc, argv)) {
    return true;
  }
#endif

  if (parse_command_line(argc, argv)) {
    bool b1 = filter();
    bool b2 = output();
    return (b1 && b2);
  }

  return true;
}

int main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  try {
    lpsConstElm tool;

    tool.execute(argc, argv);
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
