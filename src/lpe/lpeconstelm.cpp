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
#include <boost/format.hpp>

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

//Customtypes
//typedef data_assignment_list            Tsv;
//typedef set < data_assignment_list >    Tnsv;

//Constanten
string version = "Version 0.2";

//Global variables
data_assignment_list          sv; 	//state vector
data_assignment_list          vinit; //init vector
set< data_assignment_list >   nsvs;  //set of newstate variables
int                           n;    //number of process parameters

bool eval_cond(data_expression datexpr, data_assignment_list statevector, data_equation_list equations, set<int> S){
  //trace
  bool b;
  
  set<int>::iterator i;
  data_assignment_list::iterator j;

  data_assignment_list conditionvector; 
  
  i = S.begin();
  while (i == S.end()) {
    //Consercie wordt niet ondersteunt van Assignement naar ATerm;
    //conditionvector = append(conditionvector, ( element_at(statevector, *i )));// << endl;
    i++;
  };
  
  // Substitutie van op data_expression wordt niet ondersteunt
  // datexpr = datexpr.substitute(conditionvector);
  
  // compare functie op Atermen???
  // isFalse is een aterm representatie voor false
  // b = compare(isFalse,  rewrite(datexpr.to_ATermAppl(), gsMakeDataEqnSpec(equations.to_ATermList() ) ) );
   
   // voor het compileren en testen voor t gemak maar b = true
   b = true;
   return b;
}

set< int > constelm(string filename, int option)
{
  //Read input file
  specification spec;
  if (!spec.load(filename))
  {
    cerr << "could not read " << filename << endl;
  }
  LPE lpe = spec.lpe();

//assert(false);
  n = lpe.process_parameters().size();
  vinit = spec.init_assignments();
  sv = vinit;
  
  set< int > V; 
  set< int > S;

  for(int j=0; j <= (n-1) ; j++){
    V.insert(j);
  };
  
  set< int > D;
  cout << lpe.summands().size() << endl;;

  while(S.size()!=V.size()) {
    //nsvs = emptyset; 
    nsvs.erase(nsvs.begin(),nsvs.end());
    S = V;
    
    set< int > S_dummy;        
    
    for(summand_list::iterator s_current = lpe.summands().begin(); s_current != lpe.summands().end(); ++s_current){ 
      if (eval_cond(s_current->condition(), sv, spec.equations(), S)){
        //data_assignment_list nextstate = s_current->assignments().substitute(sv);
	//nsvs.insert(nextstate);
	set< int >::iterator j = S.begin();
	while (j != S.end()){
          //Compare functie werkt niet
	  //if (compare(element_at(sv, *j), element_at(nextstate, *j) ) ){
            S_dummy.insert(*j);
	  //};
	  j++;
        };
      }; 
    };  
    
    //
    //S_dummy = Sort(S_dummy)
    
    set_difference(S.begin(), S.end(), S_dummy.begin(), S_dummy.end(), inserter(V, V.begin()));
    
    ////
    // Als V <  S dan zijn er variabele process parameters gevonden
    ////
    
    if (S.size() != V.size()){
      //data_assignment nsv = vinit;
      set_difference(S.begin(), S.end(), S_dummy.begin(), S_dummy.end(), inserter(D, D.begin()));
      
      set< int >::iterator j = D.begin();
      while (j != D.end() ) {
        //sv = replace(sv, element_at(nextstate, *j), *j);
        j++;
      };      
    };
  }; 
  return S;
}


int main(int ac, char* av[])
{
      string filename;
      int opt = 0;

      try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("version,v", "gets the version number of the current release of this mCRL2 tool")
            ("monitor,m", "displays progressing information")
            ("nosingleton", "displays progressing information")
            ("nocondition", "Saves computing time. No check if conditions are rewritten to false")
        ;
	
	po::options_description hidden("Hidden options");
	hidden.add_options()
	    ("input-file", po::value<string>(), "input file" )
	;
	
	po::options_description cmdline_options;
	cmdline_options.add(desc).add(hidden);
	
	po::options_description visible("Allowed options");
	visible.add(desc);
	
	po::positional_options_description p;
	p.add("input-file", -1);
	
	po::variables_map vm;
        po::store(po::command_line_parser(ac, av).
                  options(cmdline_options).positional(p).run(), vm);
        po::notify(vm);
        
        if (vm.count("help") || ac == 1) {
            cout << "Usage: "<< av[0] << " [options] input-file\n";
            cout << desc;
            return 0;
        }
        
        if (vm.count("version")) {
	        cout << version << endl;
	        return 0;
	      }

        if (vm.count("monitor")) {
          cout << "Displaying progress" << endl;
          opt = 1;
	      }

        if (vm.count("nosingleton")) {
          cout << "Active: no removal of process parameters which have sorts of cardinatilty one" << endl;
          opt = 2;
	      }

        if (vm.count("nocondition")) {
          cout << "Active: All conditions are true" << endl;
          opt = 3;
	      }

        if (vm.count("input-file"))
        {
          filename = vm["input-file"].as<string>();
	      }

        constelm(filename, opt);       
	
    }
    catch(exception& e)
    {
        cout << e.what() << "\n";
        return 1;
    }    
    return 0;
}

/*class Sum {
public:
    Sum(double d) : total(d) {}
    double operator()(double val) {
        total += val;
        return total;
    }
    double getTotal() {
        return total;
    }
private:
    double total;
}; */

  //Test vars
  /*
  cout << D.size() << endl;

   Sum summer(0.0);
   summer = for_each(V.begin(), V.end(), summer);
   cout << summer.getTotal() << endl;
  */

