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
#include "mcrl2_revision.h"

#include "atermpp/aterm.h"
#include "lpe/specification.h"
#include "lpe/lpe_error.h"

// Squadt protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <sip/tool.h>
#endif

using namespace std;
using namespace lpe;
using namespace atermpp;

using atermpp::aterm;
using atermpp::aterm_appl;
using atermpp::aterm_list;

const char* version = "0.5.1";

class ParElmObj {
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
  inline set< data_variable > getDataVarIDs(aterm_appl input)
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
  inline void setVerbose(bool b)
  {
    p_verbose = b;
  }

  // Sets verbose option
  // Note: Has to be set
  //  
  inline void setDebug(bool b)
  {
    p_debug = b;
  }

  // Set output file
  //  
  inline void setSaveFile(string x)
  {
    p_outputfile = x;
  }

  // Loads an LPE from file
  // returns true if succeeds
  //  
  inline bool loadFile(string filename)
  {
    p_inputfile = filename;
    if (!p_spec.load(p_inputfile)){
      cerr << "lpeparelm: error: could not read input file '" << filename << "'" << endl;
      return false;      
    };
    return true;   
  }

  // Reads an LPE from stdin
  // returns true if succeeds
  //  
  inline bool readStream()
  {
    ATermAppl z = (ATermAppl) ATreadFromFile(stdin);
    if (z == NULL){
      cerr << "lpeparelm: Could not read LPE from stdin"<< endl;
      return false;
    };
    if (!gsIsSpecV1(z)){
      cerr << "lpeparelm: Stdin does not contain an LPE" << endl;
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
      cerr << "lpeparelm: Searching for used process parameters: ";
    }
    //int counter = 0;
    //int n = lpe.summands().size();
    for(summand_list::iterator currentSummand = lpe.summands().begin(); currentSummand != lpe.summands().end(); currentSummand++){ 
      
      if (p_verbose){
        //cerr << "    Summand :" << ++counter << "/" << n << endl;    
        cerr << ".";
      }
      //Condition
      //
      foundVariables = getDataVarIDs(aterm_appl(currentSummand->condition()));
      //      cerr << currentSummand->condition().pp()<< endl;
      //      cerr << "\033[36m" << setToList(foundVariables).to_string() << "\033[0m" <<endl;
      for(set< data_variable>::iterator i = foundVariables.begin(); i != foundVariables.end(); i++){
         p_usedVars.insert(data_variable(*i));
      }

      //Time
      //
      if (currentSummand->has_time()){
        foundVariables = getDataVarIDs(aterm_appl(currentSummand->time()));
        //        cerr << currentSummand->time().pp() << endl;        
        //        cerr << "\033[39m" << setToList(foundVariables).to_string() << "\033[0m" <<endl; 
        for(set< data_variable >::iterator i = foundVariables.begin(); i != foundVariables.end(); i++){
           p_usedVars.insert(data_variable(*i));
        };
      }
     
      
      //Actions
      //
      for(action_list::iterator i = currentSummand->actions().begin(); i != currentSummand->actions().end(); i++){
        for(data_expression_list::iterator j = i->arguments().begin(); j != i->arguments().end(); j++){
          foundVariables = getDataVarIDs(aterm_appl(*j));
          //          cerr << i->to_string() << endl;
          //          cerr << "\033[31m" << setToList(foundVariables).to_string() << "\033[0m" <<endl;  
          for(set< data_variable >::iterator k = foundVariables.begin(); k != foundVariables.end(); k++){
  	         p_usedVars.insert(*k);
  	      };
        };  
      };
      //      cerr << "\033[32m" << setToList(p_usedVars).to_string() << "\033[0m" <<endl;     
    }
    if (p_verbose) cerr << endl;
    
    // Needed all process parameters which are not marked have to be eliminated
    //
    int cycle = 0;
    if(p_verbose){
      cerr << "lpeparelm: Searching for dependent process parameters" << endl;
    } 
    bool reset = true;
    while (reset){
      if (p_verbose){
        cerr << "lpeparelm:   Cycle "<< ++cycle << ": ";
      }
      reset = false;
      //counter = 0; 
      for(summand_list::iterator currentSummand = lpe.summands().begin(); currentSummand != lpe.summands().end(); currentSummand++){
        if (p_verbose){
          //cerr << "    Summand :" << ++counter << "/" << n << endl;    
          cerr << ".";
        }
        for(data_assignment_list::iterator i = currentSummand->assignments().begin(); i !=  currentSummand->assignments().end() ;i++){
          if (p_usedVars.find(i->lhs()) != p_usedVars.end()){
            foundVariables = getDataVarIDs(aterm_appl(i->rhs()));
            //cerr << i->rhs().pp() << endl;
            unsigned int  z = p_usedVars.size();
            for(set< data_variable >::iterator k = foundVariables.begin(); k != foundVariables.end(); k++){
  	          p_usedVars.insert(*k);
  	          //cerr << k->pp() << endl;
  	        }
  	        //cerr << z << "----" << p_usedVars.size() << endl;
            if (p_usedVars.size() != z){
              reset = true;
              //          cerr << "\033[39m Match added: " << i->lhs().to_string() << "\033[0m" << endl; 
            };  
          }
        }
      }
      if (p_verbose) cerr << endl;
      //cerr << setToList(p_usedVars).to_string() << endl;
    }
  
    for(data_variable_list::iterator di = lpe.process_parameters().begin(); di != lpe.process_parameters().end() ; di++){
      T.insert(*di);	  
    };
    set_difference(T.begin(), T.end(),  p_usedVars.begin(),  p_usedVars.end(), inserter(p_S, p_S.begin()));
  
    //if (p_S.size() ==0){
    //  assert(test());
    //}

    if (p_verbose) {
      cerr << "lpeparelm: Number of removed process parameters: " << p_S.size() << endl;
      if (p_S.size() !=0){
	cerr << "lpeparelm:   [ ";
        for(set< data_variable >::iterator i = p_S.begin(); i != (--p_S.end()); i++){
          cerr << i->name() << ", ";
        }
        cerr << (*(--p_S.end())).name() << " ]" << endl;
      }
    }// else {  
     // cerr << " Number of removed process parameters : " << p_S.size() << endl;
    //}
  }
  
  inline void output()
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
      //if(!p_verbose){
      //  assert(!p_verbose);
        writeStream(rebuild_spec);
      //};
    } else {
      if(!rebuild_spec.save(p_outputfile)){
         cerr << "lpeparelm: Unsuccessfully written outputfile: " << p_outputfile << endl;
      };
    } 
  }
    
  inline string getVersion()
  {
    return (version);
  }
};

void parse_command_line(int ac, char** av, ParElmObj& parelm) {
  namespace po = boost::program_options;

  po::options_description description;

  /* Name of the file to read input from (or standard input: "-") */
  std::vector < std::string > file_names;

  description.add_options()
    ("verbose,v", "turn on the display of short intermediate messages")
    ("debug,d",   "turn on the display of detailed intermediate messages")
    ("version",   "display version information")
    ("help,h",    "display this help")
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
              << "Removes unused parameters from the LPD read from standard input or INFILE." << std::endl
              << "By default the result is written to standard output, and otherwise to OUTFILE." << std::endl
              << std::endl << description;

    exit (0);
  }
        
  if (vm.count("version")) {
    std::cerr << version << " (revision " << REVISION << ")" << endl;

    exit (0);
  }

  parelm.setVerbose(vm.count("verbose"));
  parelm.setDebug(vm.count("debug"));

  if (vm.count("file_names")){
    file_names = vm["file_names"].as< std::vector< std::string > >();
  }

  if (file_names.size() == 0){
    /* Read from standard input */
    if (!parelm.readStream()) {
      exit (1);
    }
  }
  else if (2 < file_names.size()) {
    cerr << "lpeparelm: Specify only INPUT and/or OUTPUT file (too many arguments)."<< endl;

    exit (0);
  }
  else {
    if (!parelm.loadFile(file_names[0])) {
      exit (1);
    }

    if (file_names.size() == 2) {
      parelm.setSaveFile(file_names[1]);
    }
  }
}

#ifdef ENABLE_SQUADT_CONNECTIVITY
/* Constants for identifiers of options and objects */
const unsigned int lpd_file_for_input  = 0;
const unsigned int lpd_file_for_output = 1;

void realise_configuration(sip::tool::communicator& tc, ParElmObj& constelm, sip::configuration& c) {
  std::string input_file_name  = c.get_object(lpd_file_for_input)->get_location();
  std::string output_file_name = c.get_object(lpd_file_for_output)->get_location();

  if (!constelm.loadFile(input_file_name)) {
    tc.send_error_report("Error reading input!");

    exit(1);
  }

  constelm.setSaveFile(output_file_name);

  constelm.setVerbose(true);
}
#endif

int main(int ac, char** av) {
  ATerm     bottom;
  ParElmObj parelm;

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

    /* Static configuration cycle (phase 1: obtain input combination) */
    while (!valid) {
      /* Wait for configuration data to be send (either a previous configuration, or only an input combination) */
      sip::configuration::sptr configuration = tc.await_configuration();

      /* Validate configuration specification, should contain a file name of an LPD that is to be read as input */
      valid  = configuration.get() != 0;
      valid &= configuration->object_exists(lpd_file_for_input);

      if (valid) {
        std::string input_file_name = configuration->get_object(lpd_file_for_input)->get_location();

        /* Add output file to the configuration */
        configuration->add_output(lpd_file_for_output, "lpe", input_file_name + ".lpe");

        /* An object with the correct id exists, assume the URI is relative (i.e. a file name in the local file system) */
        tc.set_configuration(configuration);
      }
      else {
        tc.send_error_report("Invalid input combination!");

        exit(1);
      }
    }

    realise_configuration(tc, parelm, tc.get_configuration());

    /* Send the controller the signal that we're ready to rumble (no further configuration necessary) */
    tc.send_accept_configuration();

    /* Wait for start message */
    tc.await_message(sip::message_signal_start);
  }
  else {
    parse_command_line(ac,av,parelm);
  }
#else
  parse_command_line(ac,av,parelm);
#endif

  parelm.filter();
  parelm.output(); 

#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (tc.is_active()) {
    sip::report report;

    report.set_comment("done with state space generation");

    tc.send_report(report);

    tc.send_signal_done();

    gsRewriteFinalise();

    tc.await_message(sip::message_request_termination);
  }
  else {
    gsRewriteFinalise();
  } 
#else
  gsRewriteFinalise();
#endif

  return (0);
}
