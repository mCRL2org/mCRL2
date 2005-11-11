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

const string version = "lpeconstelm - version 0.5 ";

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
  bool                        p_nosingleton;
  bool                        p_alltrue;
  bool                        p_reachable; 
  string                      p_filenamein;
  
  specification               p_spec;

  inline ATermAppl rewrite(ATermAppl t)
  {
    ATermAppl result = gsRewriteTerm(t);
    return result;
  }

  inline void printNextState()
  {
    for(vector< data_assignment >::iterator i = p_nextState.begin(); i != p_nextState.end() ; i++ ){
      cout << "[" << i->pp() << "]";
    
    }
    cout << endl;
  }

  inline void calculateNextState(data_assignment_list assignments)
  {
    for(vector< data_assignment >::iterator i = p_currentState.begin(); i != p_currentState.end(); i++ ){
      int index = p_lookupIndex[i->lhs()];
      if (p_V.find(index) == p_V.end()){
      p_nextState[index] = 
           data_assignment( i->lhs(), 
                            rewrite(i->lhs().to_expr().substitute(assignments.begin(), assignments.end()).substitute(p_currentState.begin(), p_currentState.end()))
                          );
      } else {
        p_nextState[index] = *i;
      }    
    }
  }  
    

  
  inline bool inFreeVarList(data_expression dexpr)
  {
    return (p_freeVarSet.find(dexpr) != p_freeVarSet.end());
  }
  
  inline data_assignment newExpression(data_assignment ass)
  {
    char buffer [99];
    sprintf(buffer, "%s^%d", ass.lhs().name().c_str(), p_newVarCounter++);
    data_variable w(buffer, ass.lhs().type() );
    data_assignment a(ass.lhs() , w.to_expr());
    return a;
  }
  
  inline bool compare(data_expression x, data_expression y)
  {
    return (x==y);
  }
  
  inline bool conditionTest(data_expression x)
  {
   //cout << endl <<" cd " << data_expression(rewrite(x.substitute(p_currentState.begin(), p_currentState.end()))).pp() ;
   if (data_expression(rewrite(x.substitute(p_currentState.begin(), p_currentState.end()))).is_false())
     {
       return false;
     };
   return true;
  }

  inline bool cmpCurrToNext()
  {

    bool differs = false;
    for(vector< data_assignment>::iterator i= p_currentState.begin(); i != p_currentState.end() ;i++){
      int index = p_lookupIndex[i->lhs()]; 
      if (p_V.find(index) == p_V.end()) { //possible constant
        if (inFreeVarList(i->rhs())) { 
          if (p_variableList.find(p_nextState.at(index).rhs()) != p_variableList.end()){
            p_V.insert(p_lookupIndex[i->lhs()]); 
//            cout << "\033[34m OLD:    "<< i->pp() << endl;
//            cout << "\033[32m NEW:    "<< p_nextState.at(index).pp() << endl;
//            cout << "\033[0m";
          };
          p_newCurrentState.at(index) = p_nextState.at(index) ;
          p_currentState.at(index) = p_nextState.at(index);  
        } else {
          if (!inFreeVarList( p_nextState.at(index).rhs() )){
             if (!compare(i->rhs(), p_nextState.at(index).rhs())){
//                cout << "\033[34m OLD:    "<< i->pp() << endl;
//                cout << "\033[32m NEW:    "<< p_nextState.at(index).pp() << endl;
//                cout << "\033[0m";
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

  template <typename Term>
  inline
  term_list<Term> vectorToList(vector<Term> y)
  { 
    term_list<Term> result;
    for(typename vector<Term>::iterator i = y.begin(); i != y.end() ; i++)
      { 
        result = push_front(result,*i); 
      }
    return reverse(result); 
  } 

  template <typename Term>
  inline
  term_list<Term> setToList(set<Term> y)
  { 
    term_list<Term> result;
    for(typename set<Term>::iterator i = y.begin(); i != y.end() ; i++)
      { 
        result = push_front(result,*i); 
      }
    return reverse(result); 
  } 


//---------------------------------------------------------------
//---------------------   Debug begin  --------------------------
//---------------------------------------------------------------

  void inline printVar()
  {
    cout << " Variable indices : ";
    set< int >::iterator i = p_V.begin();
    int j = 0;
    while(i != p_V.end()){
      if (*i ==j){
        cout << j+1 << " ";
        i++;
       }
      j++;
    }
    cout << endl;
  }

  void inline printState()
  {
    for(set< int >::iterator i = p_S.begin(); i != p_S.end() ; i++ ){
      cout << "  [" << p_currentState[*i].pp() << "]" << endl;
      
    }
    cout << endl;
  }
  
  
  void inline printCurrentState()
  {
    for(vector< data_assignment >::iterator i = p_currentState.begin(); i != p_currentState.end() ; i++ ){
      cout << "[" << i->pp() << "]";
    
    }
    cout << endl;
  }
  
//---------------------------------------------------------------
//---------------------   Debug end  --------------------------
//---------------------------------------------------------------

public:

void inline outputConstelm()
  {
    LPE lpe = p_spec.lpe();
    summand_list rebuild_summandlist;
    //rebuild_summandlist = lpe.summands();

    //Remove the summands that are never visited
    //
    if (p_reachable){
      rebuild_summandlist = setToList(p_visitedSummands); 
    } else {
      rebuild_summandlist = lpe.summands();
    }

    if (p_verbose) {
      cout << "Number of written summands :"<<  rebuild_summandlist.size() << endl;
    }

    //Singleton sort process parameters
    //
    sort_list rebuild_sort = p_spec.sorts();
    // 2B implemented


    vector< data_assignment > constantPP;
    for(set< int >::iterator i = p_S.begin(); i != p_S.end(); i++){
      constantPP.push_back(p_initAssignments.at(*i));
    }
    
    vector< data_assignment > variablePP;
    for(set< int >::iterator i = p_V.begin(); i != p_V.end(); i++){
      variablePP.push_back(p_initAssignments.at(*i));
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
    for(summand_list::iterator j = rebuild_summandlist.begin(); j != rebuild_summandlist.end(); j++){
      data_assignment_list rebuild_sum_ass; 
      for(data_assignment_list::iterator k = j->assignments().begin(); k != j->assignments().end() ; k++){
        bool b = false;
        for(vector< data_assignment>::iterator l = constantPP.begin(); l != constantPP.end() ; l++ ){
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
      rebuild_condition = data_expression(rewrite(rebuild_condition.substitute(constantPP.begin(), constantPP.end())));

      //LPE_summand(data_variable_list summation_variables, data_expression condition, 
      //            bool delta, action_list actions, data_expression time, 
      //            data_assignment_list assignments);    
      tmp = LPE_summand(j->summation_variables(), rebuild_condition, 
        j->is_delta(), j->actions(), j->time(), 
	      reverse(rebuild_sum_ass));
        rebuild_summandlist_no_cp = push_front(rebuild_summandlist_no_cp, tmp); 
    }
      
    rebuild_summandlist = substitute(reverse(rebuild_summandlist_no_cp), constantPP.begin(), constantPP.end() );
  
    //construct new specfication
    //
    //LPE(data_variable_list free_variables, data_variable_list process_parameters, 
    //  summand_list summands, action_list actions);
    LPE rebuild_lpe;
    rebuild_lpe = LPE(
      lpe.free_variables(), 
      vectorToList(variablePPvar), 
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
      p_spec.sorts(), 
      p_spec.constructors(), 
      p_spec.mappings(), 
      p_spec.equations(), 
      p_spec.actions(), 
      rebuild_lpe, 
      p_spec.initial_free_variables(), 
      vectorToList(variablePPvar), 
      vectorToList(variablePPexpr)
    );
    
    assert(gsIsSpecV1((ATermAppl) rebuild_spec));
  
    if (p_outputfile.size() == 0){
      if(!p_verbose){
        assert(!p_verbose);
        writeStream(rebuild_spec);
      };
    } else {
      rebuild_spec.save(p_outputfile);
    }
}

  void inline setSaveFile(string x)
{
  p_outputfile = x;
}

  void inline printSet()
  {
    cout << "Constant indices: ";
    set< int >::iterator i = p_S.begin();
    int j = 0;
    while(i != p_S.end()){
      if (*i ==j){
        cout << j+1 << " ";
        i++;
       }
      j++;
    }
    cout << endl;
  }
  
  void inline loadFile(string filename)
  {
    p_filenamein = filename;
    p_spec.load(p_filenamein);   
  }

  bool inline readStream()
  {
    ATermAppl p_spec = (ATermAppl) ATreadFromFile(stdin);
    if (p_spec == NULL){
      cout << "Could not read LPE from stdin"<< endl;
      return false;
    };
    if (!gsIsSpecV1(p_spec)){
      cout << "Stdin does not contain an LPE" << endl;
      return false;
    }
    return true;
  }

  void writeStream(specification newSpec)
  {
    assert(gsIsSpecV1((ATermAppl) newSpec));
    ATwriteToBinaryFile(aterm(newSpec) , stdout);
  }

  void inline setVerbose(bool b)
  {
    p_verbose = b;
  }
  
  void inline setNoSingleton(bool b)
  {
    p_nosingleton = b;
  }
  
  void inline setAllTrue(bool b)
  {
    p_alltrue = b;
  }
  
  void inline setReachable(bool b)
  {
    p_reachable = b;
  }
  
  void printSetVar()
  {
    printState();
  }  

  void filter()
  {

    //---------------------------------------------------------------
    //---------------------   Init begin   --------------------------
    //---------------------------------------------------------------
  
    bool    same        ;
    int     counter  = 0;
    int     cycle    = 0;
    p_newVarCounter  = 0;
    LPE lpe          = p_spec.lpe();
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
    for (data_variable_list::iterator di = lpe.free_variables().begin(); di != lpe.free_variables().end(); di++){
      p_freeVarSet.insert(di->to_expr());
    } 

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
        cout << "Cycle:" << cycle++ << endl;
      }
      int summandnr = 0;
      for(summand_list::iterator currentSummand = lpe.summands().begin(); currentSummand != lpe.summands().end() ;currentSummand++ ){
        if ( (p_visitedSummands.find(*currentSummand) != p_visitedSummands.end()) || (conditionTest(currentSummand->condition()))) {
          if(p_verbose){
            cout << "Summand: "<< summandnr++ << endl;
          }
          p_visitedSummands.insert(*currentSummand); 
          calculateNextState(currentSummand->assignments());
          same = cmpCurrToNext() && same ;  
          //if (!same) {break;}                                           //Break reduces time to complete 
        }
      }
      p_currentState = p_newCurrentState;
    }
    
    //---------------------------------------------------------------
    //---------------------   Body end   ----------------------------
    //---------------------------------------------------------------



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
    if(p_freeVarSet.size() != 0){
      if (p_verbose){
        cout << "Free Variable checkup" << endl;
      }
      for(set< LPE_summand>::iterator i = p_visitedSummands.begin(); i != p_visitedSummands.end() ; i++){
        calculateNextState(i->assignments());
        cmpCurrToNext();
      }
      p_currentState = p_newCurrentState;
    }
    //---------------------------------------------------------------

    
    //---------------------------------------------------------------
    // Construct S    
    //
    set< int > S;
    int  n = lpe.process_parameters().size(); 
    for(int j=0; j < n ; j++){
      S.insert(j);
    };
    set_difference(S.begin(), S.end(), p_V.begin(), p_V.end(), inserter(p_S, p_S.begin()));
    
    if (p_verbose){
      printSet(); 
      printSetVar();  
    }
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
        ("version",     "display version information")
        ("monitor,m",   "display progress information")
        ("nosingleton", "do not remove sorts consisting of a single element")
        ("nocondition", "do not use conditions during elimination (faster)")
        ("noreachable", "does not remove summands which are not visited")
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
      obj.setVerbose(true);
	  } else {
	    obj.setVerbose(false);
	  }

    if (vm.count("nosingleton")) {
      obj.setNoSingleton(true);
	  } else {
	    obj.setNoSingleton(false);
	  }

    if (vm.count("nocondition")) {
      obj.setAllTrue(true);
	  } else {
	    obj.setAllTrue(false);
	  }

    if (vm.count("noreachable")) {
      obj.setReachable(false);
	  } else {
	    obj.setReachable(true);
	  }

    if (vm.count("INFILE")){
      filename = vm["INFILE"].as< vector<string> >();
	  }
	  
	  if (filename.size() == 0){
	    if (!obj.readStream()){return 0;}
	  }

    if (filename.size() > 2){
      cerr << "Specify only INPUT and/or OUTPUT file (Too many arguments)."<< endl;
    }
             
    if((filename.size() <= 2) && (filename.size() != 0)){
      obj.loadFile(filename[0]);
    } ; 
    if(filename.size() == 2){
      obj.setSaveFile(filename[1]);
    };
     
    obj.filter();
    obj.outputConstelm(); 
    
    gsRewriteFinalise();

    }
    catch(exception& e){
      cerr << e.what() << "\n";
      return 1;
    }    
    
    return 0;
}
