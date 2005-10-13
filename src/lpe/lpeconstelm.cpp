// TODO
// BRACKETS VERWIJDEREN IN BOOST OM DE OPTIES!!!!!
// GOED MAKEN VAN LAYOUT (UITLIJNING)

//C++
#include <iostream>
#include <vector>
#include <set>
#include <stdbool.h>
#include <string>
#include <iterator>
#include <fstream>

//Boost
#include <boost/program_options.hpp>

//mCRL2
#include <aterm2.h>
#include "atermpp/aterm.h"
#include "mcrl2/mcrl2_visitor.h"
#include "mcrl2/specification.h"
#include "mcrl2/predefined_symbols.h"
#include "mcrl2/sort.h"

#include "libgsrewrite.h"
#include "gsfunc.h"
#include "gslowlevel.h"

using namespace std;
using namespace mcrl2;
using namespace atermpp;

//Maximum number of proces parameters: 4294967295 [@AMD 1700+, Linux-FC4, 256 MB RAM]

using namespace std;
using namespace mcrl2;
using namespace atermpp;

namespace po = boost::program_options;
po::variables_map vm;

//Constanten
string version = "Version 0.2";

//Global variables
data_expression_list          sv; 	//state vector
data_expression_list          vinit; //init vector
data_expression_list          newstatevector; // newstate vector
int                           n;    //number of process parameters
int                           tab= 0;  

bool compare(data_expression x, data_expression y, data_equation_list equations)
{
  printf("Compare ");
  ATermAppl x1 = rewrite(x.to_ATermAppl(), gsMakeDataEqnSpec(equations.to_ATermList()));
  ATermAppl y1 = rewrite(y.to_ATermAppl(), gsMakeDataEqnSpec(equations.to_ATermList()));

  printf("- Done \n");  
  return atermpp::aterm(x1) == atermpp::aterm(y1);
}

bool eval_cond(data_expression datexpr, data_expression_list statevector, data_equation_list equations, set<int> S){
  printf("Eval_cond\n"  );
  bool b;
  
  set<int>::iterator i;
  data_expression_list::iterator j;

  data_expression_list conditionvector; 
  
  //
  //ORDE N^3
  //
  
  // TODO:
  //   Beter: end() -> begin() /\ push_front gebruiken
  //   Element_at = N
  //   Append = N
  //   begin-> end = N
  i = S.begin();
  while (i != S.end()) {

    conditionvector = append(conditionvector, element_at(statevector, *i ));

    i++;
  };
  
  //datexpr.substitute(datexpr.begin(), conditionvector.begin());
  
  // 
  // !!!!!!! data_expression(gsMakeOpIdFalse()) !!!!!! DIRECTE AANROEP UIT GSFUNC
  //
  b = compare(data_expression(gsMakeOpIdFalse()), datexpr, equations);

  b = true;
  
  printf("- Done \n");
  return b;
}

set< int > constelm(string filename, int option)
{
  specification spec;
  if (!spec.load(filename))
  {
    cerr << "could not read " << filename << endl;
  }
  LPE lpe = spec.lpe();

  n = lpe.process_parameters().size();
  vinit = spec.initial_state();
  newstatevector = vinit;
  
  set< int > V; 
  set< int > S;

  for(int j=0; j <= (n-1) ; j++){
    V.insert(j);
  };
  
  set< int > D;
  cout << lpe.summands().size() << endl;;
  
  while(S.size()!=V.size()) {
    sv = newstatevector;
    S = V;
    
    set< int > S_dummy;        
    
    for(summand_list::iterator s_current = lpe.summands().begin(); s_current != lpe.summands().end(); ++s_current){ 
      if (eval_cond(s_current->condition(), sv, spec.equations(), S)){
        data_expression_list nextstate = sv;
        data_assignment_list ass_nextstate = s_current->assignments();


        cout << "initvector "<< lpe.process_parameters().pp() << endl; 
        cout << "assignment "<< s_current->assignments().pp() << endl;

	      //
        // Doe hier nexstate kunstje
        //
        for (data_assignment_list::iterator i = ass_nextstate.begin(); i != ass_nextstate.end(); i++){
          nextstate = nextstate.substitute(*i);
          nextstate.pp();
	      };

        cout << sv.pp() << "  -  " << nextstate.pp() << endl;

        //assert(false);
        set< int >::iterator j = S.begin();
        while (j != S.end()){
          if (compare(element_at(sv, *j), element_at(nextstate, *j), spec.equations() ) ){
            S_dummy.insert(*j);
            newstatevector = replace(newstatevector, element_at(nextstate, *j), *j);
          };
          j++;
        };
      }; 
    }; 
    
    set_difference(S.begin(), S.end(), S_dummy.begin(), S_dummy.end(), inserter(V, V.begin()));
 
    ////
    // Als V <  S dan zijn er variabele process parameters gevonden
    ////

/*  Overbodig geworden door bijhouden van apparte "newstate vector"    

    if (S.size() != V.size()){
      //data_assignment nsv = vinit;
      set_difference(S.begin(), S.end(), S_dummy.begin(), S_dummy.end(), inserter(D, D.begin()));
      
      set< int >::iterator j = D.begin();
      while (j != D.end() ) {
        //sv = replace(sv, element_at(nextstate, *j), *j);
        j++;
      };      
    }; */
  }; 
  return S;
}

int main(int ac, char* av[])
{
      string filename;
      int opt = 0;

      try {
        po::options_description desc;
        desc.add_options()
            ("help,h",      "display this help")
            ("version,v",   "display version information")
            ("monitor,m",   "display progress information")
            ("nosingleton", "do not remove sorts consisting of a single element")
            ("nocondition", "do not use conditions during elimination (faster)")
        ;
	
	po::options_description hidden("Hidden options");
	hidden.add_options()
	    ("INFILE", po::value<string>(), "input file" )
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
        
        if (vm.count("help") || ac == 1) {
            cerr << "Usage: "<< av[0] << " [OPTION]... INFILE\n";
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
          opt = 1;
	      }

        if (vm.count("nosingleton")) {
          //cerr << "Active: no removal of process parameters which have sorts of cardinatilty one" << endl;
          opt = 2;
	      }

        if (vm.count("nocondition")) {
          //cerr << "Active: All conditions are true" << endl;
          opt = 3;
	      }

        if (vm.count("INFILE"))
        {
          filename = vm["INFILE"].as<string>();
	      }

        constelm(filename, opt);       
	
    }
    catch(exception& e)
    {
        cerr << e.what() << "\n";
        return 1;
    }    
    return 0;
}

