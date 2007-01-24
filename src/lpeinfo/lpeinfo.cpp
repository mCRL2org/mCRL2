// ======================================================================
//
// Copyright (c) 2004, 2005 TU/e
//
// ----------------------------------------------------------------------
//
// file          : lpeinfo 
// date          : 18-11-2005
// version       : 0.5.1
//
// author(s)     : Frank Stappers  <f.p.m.stappers@student.tue.nl>
//
// ======================================================================

#ifdef BOOST_BUILD_PCH_ENABLED
# include <lpe/lpe.h>
#endif

// Squadt protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <utility/squadt_utility.h>
#endif

//C++
#include <exception>
#include <cstdio>

//Boost
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

//mCRL2
#include <atermpp/aterm.h>
#include <lpe/lpe.h>
#include <lpe/specification.h>
#include "liblowlevel.h"

using namespace std;
using namespace atermpp;

namespace po = boost::program_options;

#define VERSION "0.5"

/* Verbosity switch */
bool        verbose = false;

/* Name of the file to read input from (or standard input: "-") */
std::string file_name;

void parse_command_line(int ac, char** av) {
  po::options_description desc;

  desc.add_options()
      ("help,h",      "display this help")
      ("verbose,v",   "turn on the display of short intermediate messages")
      ("debug,d",    "turn on the display of detailed intermediate messages")
      ("version",     "display version information")
  ;
      
  po::options_description hidden("Hidden options");
  hidden.add_options()
      ("INFILE", po::value< string >(), "input file")
  ;
      
  po::options_description cmdline_options;
  cmdline_options.add(desc).add(hidden);
      
  po::options_description visible("Allowed options");
  visible.add(desc);
      
  po::positional_options_description p;
  p.add("INFILE", -1);
      
  po::variables_map vm;
  po::store(po::command_line_parser(ac, av).
    options(cmdline_options).positional(p).run(), vm);
  po::notify(vm);
      
  if (vm.count("help")) {
    cerr << "Usage: "<< av[0] << " [OPTION]... [INFILE]" << endl;
    cerr << "Print basic information on the LPE in INFILE." << endl;
    cerr << endl;
    cerr << desc;

    exit (0);
  }
      
  if (vm.count("version")) {
    cerr << "lpeinfo " << VERSION << " (revision " << REVISION << ")" << endl;

    exit (0);
  }

  if (vm.count("debug")) {
    gsSetDebugMsg();
  }

  if (vm.count("verbose")) {
    gsSetVerboseMsg();
  }

  verbose  = 0 < vm.count("verbose");
  file_name = (0 < vm.count("INFILE")) ? vm["INFILE"].as< string >() : "-";
}
        
// SQuADT protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY

class squadt_interactor : public squadt_tool_interface {

  private:

    static const char*  lpd_file_for_input;  ///< file containing an LPE that can be imported

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

void squadt_interactor::set_capabilities(sip::tool::capabilities& c) const {
  c.add_input_combination(lpd_file_for_input, sip::mime_type("lpe"), sip::tool::category::reporting);
}

void squadt_interactor::user_interactive_configuration(sip::configuration& c) {
}

bool squadt_interactor::check_configuration(sip::configuration const& c) const {
  bool result = true;

  result &= c.input_exists(lpd_file_for_input);

  return (result);
}

bool squadt_interactor::perform_task(sip::configuration& c) {
  bool result = true;

  lpe::specification lpe_specification;

  if (lpe_specification.load(c.get_input(lpd_file_for_input).get_location())) {
    using namespace sip;
    using namespace sip::layout;
    using namespace sip::layout::elements;
   
    lpe::LPE lpe = lpe_specification.lpe();
  
    /* Create and add the top layout manager */
    layout::manager::aptr top = layout::horizontal_box::create();
  
    /* First column */
    layout::vertical_box* left_column = new layout::vertical_box();
  
    layout::vertical_box::alignment a = layout::left;
  
    left_column->add(new label("Input read from:"), a);
    left_column->add(new label("Summands (#):"), a);
    left_column->add(new label("Free variables (#):"), a);
    left_column->add(new label("Process parameters (#):"), a);
    left_column->add(new label("Action labels (#):"), a);
  
    /* Second column */
    layout::vertical_box* right_column = new layout::vertical_box();
  
    right_column->add(new label(file_name), a);
    right_column->add(new label(boost::lexical_cast < std::string > (lpe.summands().size())), a);
    right_column->add(new label(boost::lexical_cast < std::string > ((lpe_specification.initial_free_variables().size() + lpe.free_variables().size()))), a);
    right_column->add(new label(boost::lexical_cast < std::string > (lpe.process_parameters().size())), a);
    right_column->add(new label(boost::lexical_cast < std::string > (lpe_specification.action_labels().size())), a);
  
    /* Attach columns*/
    top->add(left_column, margins(0,5,0,5));
    top->add(right_column, margins(0,5,0,20));
  
    send_display_layout(top);
  }
  else {
    send_error("Failure reading input from file: `" + file_name + "'\n");

    result = false;
  }

  return (result);
}
#endif

int main(int argc, char** argv) {
  ATerm bottom;

  ATinit(argc,argv,&bottom);

  gsEnableConstructorFunctions();

#ifdef ENABLE_SQUADT_CONNECTIVITY
  squadt_interactor c;

  if (!c.try_interaction(argc, argv)) {
#endif
    parse_command_line(argc,argv);

    lpe::specification lpe_specification;
 
    if (lpe_specification.load(file_name)) {
      lpe::LPE lpe = lpe_specification.lpe();
    
      cout << "Input read from " << ((file_name == "-") ? "standard input" : file_name) << endl << endl;
     
      cout << "Number of summands          : " << lpe.summands().size() << endl;
      cout << "Number of free variables    : " << lpe_specification.initial_free_variables().size() + lpe.free_variables().size() << endl;
      cout << "Number of process parameters: " << lpe.process_parameters().size() << endl; 
      cout << "Number of action labels     : " << lpe_specification.action_labels().size() << endl;
    }
    else {
      std::cerr << "Error: Unable to load LPE from `" + file_name + "'\n";
    }
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif

  return 0;
}
