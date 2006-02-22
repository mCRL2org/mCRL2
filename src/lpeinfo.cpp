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

//C++
#include <stdio.h>

//Boost
#include <boost/program_options.hpp>

//mCRL2
#include "atermpp/aterm.h"
#include "lpe/specification.h"

using namespace std;
using namespace lpe;
using namespace atermpp;

namespace po = boost::program_options;

#define p_version "lpeinfo 0.5"

class InfoObj
{
private:
  specification     p_spec;
  bool              p_pars;
  bool              p_debug;
  bool              p_verbose;
  string            p_filename;
  

public:
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

  // Displays the in information about an LPE
  //
  int display()
  {
    LPE lpe = p_spec.lpe();
    
    if (!p_pars) {
      if (p_filename.size() != 0){
        cout << "Output of                   : " << p_filename << endl << endl;
      }
      cout << "Number of summands          : " << lpe.summands().size() <<endl;
      cout << "Number of free variables    : " << p_spec.initial_free_variables().size() + lpe.free_variables().size() <<endl;
      cout << "Number of process parameters: " << lpe.process_parameters().size() <<endl; 
      cout << "Number of actions           : " << lpe.actions().size() << endl;
    };
    if (p_pars) {
      for (data_assignment_list::iterator i = p_spec.initial_assignments().begin(); i != p_spec.initial_assignments().end(); ++i)
      {
        cout << i->lhs().pp() << ": " << i->lhs().type().pp() << " = " <<i->rhs().pp() << endl;
      }
    }
    return 0;
  }

  // Reads an LPE from stdin
  // returns true if succeeds
  //  
  bool inline readStream()
  {
    ATermAppl z = (ATermAppl) ATreadFromFile(stdin);
    if (z == NULL){
      cout << "Could not read LPE from stdin"<< endl;
      return false;
    };
    if (!gsIsSpecV1(z)){
      cout << "Stdin does not contain an LPE" << endl;
      return false;
    }
    p_spec = specification(z);
    return true;
  }

  // Loads an LPE from file
  // returns true if succeeds
  //    
  bool inline loadFile(string filename)
  {
    p_filename = filename;
    if (!p_spec.load(filename))
    {
      cerr << "error: could not read input file '" << filename << "'" << endl;
      return false;
    } 
    return true;
  }

  // Sets verbose option
  // Note: Has to be set
  //
  void inline setPars(bool b)
  {
    p_pars = b;
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
   
  InfoObj obj;
  
  string filename;

  try {
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

    if (vm.count("INFILE")){
      filename = vm["INFILE"].as< string >();
    }

    if (vm.count("pars"))
		  {obj.setPars(true);} else
		  {obj.setPars(false);}

    if(filename.size() != 0 ){
      if (filename == ">"){
        if (obj.readStream()){obj.display();};
      }  
      else{
        if (obj.loadFile(filename)){obj.display() ;};
      }
    } else {
      if (obj.readStream()){obj.display() ;};
    }
  
  }
  catch(exception& e){
    cout << e.what() << "\n";
    return 1;
  }    
  return 0;
}
