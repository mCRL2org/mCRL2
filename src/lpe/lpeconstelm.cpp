// TODO
// BRACKETS VERWIJDEREN IN BOOST OM DE OPTIES!!!!!
// GOED MAKEN VAN LAYOUT (UITLIJNING)
// ON WINDOWS ansi.sys DRIVER NEEDS TO BE INSTALLED TO DISPLAY COLOR

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
data_expression_list          vinit; //init vector
data_assignment_list          newstatevector; // newstate vector
data_assignment_list          tvector;
data_assignment_list          sv;
data_assignment_list          ainit;
data_variable_list            freevars;
int                           n;    //number of process parameters


string findfile(string path)
{
  string token = "";
  string::size_type begIdx;
  string::size_type endIdx;

  // Find first position after last appearance
  //  of forward- or backslash

  begIdx = path.find_last_of( "\\\/" );
  if ( begIdx == string::npos )
    begIdx = 0;
  else
    ++begIdx;

  endIdx = path.size();
  token = path.substr( begIdx, endIdx );
  
  return token;

}

string findpath(string path)
{
  string token = "";
  string::size_type begIdx;
  string::size_type endIdx;

  // Find first position after last appearance
  //  of forward- or backslash

  endIdx = path.find_last_of( "\\\/" );
  if ( endIdx == string::npos )
    endIdx = 0;
  else
    ++endIdx;

  begIdx = 0;
  token = path.substr( begIdx, endIdx );
  
  return token;

}

bool compare(data_expression x, data_expression y, data_equation_list equations)
{
  return x==y;
  ATermAppl x1 = rewrite(x.to_ATermAppl(), gsMakeDataEqnSpec(equations.to_ATermList()));
  ATermAppl y1 = rewrite(y.to_ATermAppl(), gsMakeDataEqnSpec(equations.to_ATermList()));
  return atermpp::aterm(x1) == atermpp::aterm(y1);
}

data_assignment_list nextstate(data_assignment_list currentstate, data_assignment_list assignments, data_equation_list equations, LPE lpe)
{
  data_assignment_list out;
  data_assignment_list out1;
  data_assignment_list out2;
  data_expression z;
  
  data_assignment_list::iterator i = currentstate.begin();
  while(i != currentstate.end() ){
    z = i->lhs().to_expr();
    //cout << z.s << endl;
    out = append(out, data_assignment(i->lhs(), z ) );
    i++;
  }
  
  i = out.begin();
  while(i != out.end() ){
    data_assignment_list::iterator j = assignments.begin();
    z = i->lhs().to_expr();
    while( j != assignments.end() ){
      z = z.substitute( *j );    
      j++;
    } 
    out1 = append(out1, data_assignment(i->lhs(), z));
    i++;
  }

  i = out1.begin();
  while(i != out1.end() ){
    data_assignment_list::iterator j = currentstate.begin();
    z = i->rhs();
    while( j != currentstate.end() ){
      z = z.substitute( *j );    
      j++;
    } 
    out2 = append(out2, data_assignment(i->lhs(),data_expression( rewrite(z.to_ATermAppl(), gsMakeDataEqnSpec(equations.to_ATermList())))));
    i++;
  }
 
 return out2; 
}

data_expression_list rhsl(data_assignment_list x)
{
  data_expression_list y;
  data_assignment_list::iterator i = x.begin();
  while (i != x.end()) {
    y = append(y, i->rhs() );
    i++;
  };

  return y;
}

data_variable_list lhsl(data_assignment_list x)
{
  data_variable_list y;
  data_assignment_list::iterator i = x.begin();
  while (i != x.end()) {
    y = append(y, i->lhs() );
    i++;
  };

  return y;
}

bool eval_cond(data_expression datexpr, data_assignment_list statevector, data_equation_list equations, set<int> S){

  bool b;
  set<int>::iterator i;
  data_assignment_list conditionvector; 
  
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
    conditionvector = append(conditionvector, data_assignment(  element_at(lhsl(statevector), *i) , element_at( rhsl(statevector), *i  )));
    i++;
  };

  data_assignment_list::iterator j = conditionvector.begin();
  while (j != conditionvector.end() ){;
    datexpr = datexpr.substitute(*j);
    j++;
  };

  // 
  // !!!!!!! data_expression(gsMakeOpIdFalse()) !!!!!! DIRECTE AANROEP UIT GSFUNC
  //

  b = !compare(data_expression(gsMakeOpIdFalse()), datexpr, equations);

  return b;
}

void  save_const(specification spec,string  outfile, set< int > S){

  LPE lpe = spec.lpe();

  set< int >::iterator i;
  data_assignment_list sub = spec.init_assignments();
  data_assignment_list result;
  
  i = S.begin();
  while (i != S.end()) {
    result = append(result, data_assignment(  element_at(lhsl(sub), *i) , element_at( rhsl(sub), *i  )));
    i++;
  };
  
  data_assignment_list::iterator j = result.begin();
  while(j != result.end() ){
    lpe = lpe.substitute(*j);
    j++;
  }

 // spec2 = specification()

  
  if  (spec.save(outfile)){
    cout << " Written output file: " << outfile << endl;
  } else
  {
    cout << " \033[0;31mUnsuccefully\033[0m written to output file: " << outfile << endl;
  }

  return;
}

void print_const(specification spec , set< int > S)
{  

  LPE lpe = spec.lpe();

  set< int >::iterator i;
  data_assignment_list sub = spec.init_assignments();
  data_assignment_list result;
  
  i = S.begin();
  while (i != S.end()) {
    result = append(result, data_assignment(  element_at(lhsl(sub), *i) , element_at( rhsl(sub), *i  )));
    i++;
  };
  
  { 
    cout << "\033[0;1;37m Constant Process parameters : \033[m" << endl;
    cout << "     "<< result.pp() << endl <<endl;
  };
  
  return;
}

void constelm(string filename, string outfile, int option)
{
  // Load LPE input file
  specification spec;
  if (!spec.load(filename))
  {
    cerr << "could not read " << filename << endl;
    return;
  }
 
  cout << "Read from input file : " << filename << endl;
  LPE lpe = spec.lpe();


  int bla = 0;   

  //cout << outfile << endl; 

 // assert(false);
  //
  // Determine the inital processes
  // 
  n = lpe.process_parameters().size();
  sv = spec.init_assignments();
  ainit = spec.init_assignments();
  newstatevector = sv;  
  
  freevars = concat(spec.initial_free_variables(), lpe.free_variables());
  
  cout << freevars.size() << endl; 
  cout << freevars.pp() << endl;
 

  set< int > V; 
  set< int > S;

  for(int j=0; j <= (n-1) ; j++){
    V.insert(j);
  };
  
  set< int > D;
  //cout << lpe.summands().size() << endl;;

  ////
  // Als V <  S dan zijn er variabele process parameters gevonden
  ////

  cout << endl << " Output of: "<< filename <<endl << endl;

  int count = 1; 
  while(S.size()!=V.size()) {
    sv = newstatevector;
    S = V;
    V.clear();
    set< int > S_dummy;        
    
    {
      cout << "\033[0;1;37m Iteration           : \033[m" << count++ << endl; 
      cout << "\033[0;1;37m Current statevector : \033[m" << sv.pp() << endl;
      cout << "\033[0;1;37m Resulting Nextstates: \033[m" << endl;
    };
    
    for(summand_list::iterator s_current = lpe.summands().begin(); s_current != lpe.summands().end(); ++s_current){ 
      if (eval_cond(s_current->condition(), sv, spec.equations(), S)) {
        
        cout << sv.pp() << endl;
        cout << s_current->condition().pp() << endl;   

        data_assignment_list ass_nextstate = s_current->assignments();
        tvector = nextstate(sv, ass_nextstate, spec.equations(), lpe );
      
        set< int >::iterator j = S.begin();
        
        while (j != S.end()){
          
         

	        bool skip = false; 
	        //Begin freevar hack  
	    	
          data_variable_list::iterator f = freevars.begin();
          
	        while (f != freevars.end()){
          //bla++;
          //cout << bla << "--" << f->pp() << endl; 
	        if ( compare(element_at(rhsl(tvector), *j) , f->to_expr(), spec.equations() ) || 
	          compare(element_at(rhsl(ainit), *j) , f->to_expr(), spec.equations() ) ){
	            skip = true;
            //  cout << bla << "skipped" << endl;
	            if (compare(element_at(rhsl(ainit), *j) , f->to_expr(), spec.equations() )){ 
	              ainit = replace(ainit, element_at(tvector, *j), *j);
	          //    cout << bla << "if" << endl;
                };    
	          };
            //cout << bla << "no freevar" << endl;
	          f++;
	        };
	        // End Freevar hack 


	        
	        if (!skip){ 
	          if (element_at(rhsl(ainit), *j) != (element_at(rhsl(tvector), *j)  ) ){
              S_dummy.insert(*j);
              newstatevector = replace(newstatevector, element_at(tvector, *j), *j);
            };
	        };
          j++;
        };
        //Debug print
	
	      {
    	    cout << "     " << newstatevector.pp() << endl; 
        };
      };
         
    }; 
    
    set_difference(S.begin(), S.end(), S_dummy.begin(), S_dummy.end(), inserter(V, V.begin()));

    cout << endl;
  }; 
  
  print_const(spec , S);
 // save_const(spec, outfile, S);

  return;
}

int main(int ac, char* av[])
{
   ATerm bot;
   ATinit(0,0,&bot);
   gsEnableConstructorFunctions();
  
      vector< string > filename;
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
     
        if (vm.count("help") || ac == 1) {
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
          filename = vm["INFILE"].as< vector<string> >();
                 //<< vm["input-file"].as< vector<string> >() << "\n";
	      }
        //set< int > S = constelm(filename, opt);

        if (filename.size() > 2)
        {
          cerr << "Specify only INPUT and/or OUTPUT file (Too many arguments)."<< endl;
        };
             

        if (filename.size() == 2)
        {
          constelm(filename[0], filename[1], opt); 
        };
	
        if(filename.size() == 1)
        {
          string temp = "constelm_";
          temp = findpath(filename[0]).append(temp.append(findfile(filename[0])));
          constelm(filename[0], temp , opt);
        };

    }
    catch(exception& e)
    {
        cerr << e.what() << "\n";
        return 1;
    }    
    return 0;
}

