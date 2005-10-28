// TODO
// BRACKETS VERWIJDEREN IN BOOST OM DE OPTIES!!!!!
// GOED MAKEN VAN LAYOUT (UITLIJNING)
// ON WINDOWS ansi.sys DRIVER NEEDS TO BE INSTALLED TO DISPLAY COLOR
// data_expression(_list), data_variable(_list), data_assignment(_list) and other ATerms should be protected or reside as local variables 

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

//Constanten
string version = "Version 0.4";
bool verbose    = false; 
bool alltrue    = false;
bool reachable  = false;
bool nosingleton = false;

const int opt_none       = 0;
const int opt_left       = 1;
const int opt_right      = 2;
const int opt_both       = 3;


ATermAppl rew2(ATermAppl t, ATermAppl rewrite_terms)
{
  gsEnableConstructorFunctions();
  ATermAppl result = gsRewriteTerm(t);
  return result;
}

string findfile(string path)
{
  string token = "";
  string::size_type begIdx;
  string::size_type endIdx;

  // Find first position after last appearance
  //  of forward- or backslash

  begIdx = path.find_last_of( "\\/" );
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

  endIdx = path.find_last_of( "\\/" );
  if ( endIdx == string::npos )
    endIdx = 0;
  else
    ++endIdx;

  begIdx = 0;
  token = path.substr( begIdx, endIdx );
  
  return token;
}


string addconstelm(string filename)
{
  string token = "";
  string::size_type begIdx;
  string::size_type endIdx;

  // Find first position after last appearance
  //  of forward- or backslash

  endIdx = filename.find_last_of( "." );
  if ( endIdx == string::npos )
    endIdx = 0;
  else
    ++endIdx;

  begIdx = 0;
  token = filename.substr( begIdx, endIdx ).append("lpeconstelm.lpe");
  
  return token;

}


void print_set(set< int > S)
{
  cout << "\033[0;1m Set : \033[m";
  
  set< int >::iterator i = S.begin();
  int j = 0;
  while(i != S.end()){
    if (*i ==j){
      cout << j << " ";
      i++;
    }
  j++;
  }
  cout << endl;
}

void print_statevector(vector< data_assignment > sv  )
{
  for(vector< data_assignment >::iterator i =sv.begin();i != sv.end();i++){
    cout <<"[" << i->pp() <<"]";
  }
  cout << endl;
}


bool compare(data_expression x, data_expression y, data_equation_list equations, int option)
{
  if (option == opt_none ){
    return x==y;
  };
 
 if (option == opt_left){
    ATermAppl x1 = rew2(aterm_appl(x) , gsMakeDataEqnSpec(aterm_list(equations) ));
    ATermAppl y1 = aterm_appl(y) ;
    return atermpp::aterm(x1) == atermpp::aterm(y1);     
  };
 
 if (option == opt_right){
    ATermAppl x1 = aterm_appl(x);
    ATermAppl y1 = rew2(aterm_appl(y) , gsMakeDataEqnSpec(aterm_list(equations) ));
    return atermpp::aterm(x1) == atermpp::aterm(y1);     
  };
 
 if (option == opt_both){
    ATermAppl x1 = rew2(aterm_appl(x), gsMakeDataEqnSpec(aterm_list(equations) ));
    ATermAppl y1 = rew2(aterm_appl(y), gsMakeDataEqnSpec(aterm_list(equations) ));
    return atermpp::aterm(x1) == atermpp::aterm(y1);     
  };

  cout << "\033[0;31mError in Rewrite\033[0m" << endl;
  ATermAppl x1 = rew2(aterm_appl(x), gsMakeDataEqnSpec(aterm_list(equations) ));
  ATermAppl y1 = rew2(aterm_appl(y), gsMakeDataEqnSpec(aterm_list(equations) ));
  return atermpp::aterm(x1) == atermpp::aterm(y1);  

}
 
vector< data_assignment > nextstate(vector< data_assignment > currentstate, vector< data_assignment > assignments, data_equation_list equations, LPE lpe)
{
  vector< data_assignment > out;
  vector< data_assignment > out1;
  vector< data_assignment > out2;
  data_expression z;

 // return currentstate;

  //Speedup
  data_variable d;
  vector< data_assignment >::iterator i = currentstate.begin();
  while(i != currentstate.end() ){
    d = i->lhs();
    out.push_back( data_assignment(d, data_expression(aterm_appl(d) )));
    i++;
  
  }
  //End speedup
 
  //data_variable d;
  i = out.begin();
  while(i != out.end() ){
    vector< data_assignment >::iterator j = assignments.begin();
    d = i->lhs();
    z = d.to_expr();
    while( j != assignments.end() ){
      z = z.substitute( *j );    
      j++;
    } 
    out1.push_back( data_assignment(d , z));
    i++;
  } 
  
  i = out1.begin();
  while(i != out1.end() ){
    vector< data_assignment >::iterator j = currentstate.begin();
    z = i->rhs();
    while( j != currentstate.end() ){
      z = z.substitute( *j );    
      j++;
    } 
    out2.push_back(data_assignment(i->lhs(),data_expression( rew2(aterm_appl(z), gsMakeDataEqnSpec(aterm_list(equations))))));
    i++;
  }

 return out2; 
}
 

vector< data_expression > rhsl(vector < data_assignment > x)
{
  vector< data_expression > y;
  vector< data_assignment >::iterator i = x.begin();
  while (i != x.end()) {
    y.push_back(i->rhs());
    i++;
  };
  return y;
}


vector< data_variable > lhsl(vector < data_assignment > x)
{
  vector< data_variable > y;
  vector< data_assignment >::iterator i = x.begin();
  while (i != x.end()) {
    y.push_back(i->lhs());
    i++;
  };
  return y;
}



bool eval_cond(data_expression datexpr, vector< data_assignment > statevector, data_equation_list equations, set<int> S){

  bool b;

 // return true;
  
  if (alltrue){
    return true;
  };

  set<int>::iterator i;
  data_assignment_list conditionvector; 
  
  //Speedup
  vector< data_variable >    sv1 = lhsl(statevector);
  vector< data_expression >  sv2 = rhsl(statevector);

  i = S.begin();
  int z = 0;
  while (i != S.end()) {
    if(z == *i) {
      conditionvector = push_front(conditionvector, data_assignment(  sv1[*i] , sv2[*i] ));
      i++;
    }
    z++; 
  };
  
  data_assignment_list::iterator j = conditionvector.begin();
  while (j != conditionvector.end() ){;
    datexpr = datexpr.substitute(*j);
    j++;
  };

  // 
  // !!!!!!! data_expression(gsMakeOpIdFalse()) !!!!!! DIRECTE AANROEP UIT GSFUNC
  //

  b = !compare(data_expression(gsMakeOpIdFalse()), datexpr, equations, opt_right);
  return b;
}


void  rebuild_lpe(specification spec,string  outfile, set< int > S, bool single){

  LPE lpe = spec.lpe();
  
  vector< data_assignment > result;
  set< int >::iterator i;
  data_assignment_list sub = spec.initial_assignments();
  if (!S.empty()){
    i = S.begin(); 
    int k = 0;
    for(data_assignment_list::iterator j = sub.begin() ;j != sub.end() ;j++){
      if (*i==k){
        result.push_back(*j);
        i++;
      };
      k++;
    }
  }
  if (!result.empty()){
    lpe = lpe.substitute(result.begin(), result.end());
  }
  
  cout << lpe.pp() << endl;
  
  if  (spec.save(outfile)){
    cout << " Written output file: " << outfile << endl << endl;
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
  data_assignment_list sub = spec.initial_assignments();
  cout << "\033[0;1m The constant process parameters \033[0m" << endl;
  if (!S.empty()){
    i = S.begin(); 
    int k = 0;
    for(data_assignment_list::iterator j = sub.begin() ;j != sub.end() ;j++){
      if (*i==k){
      // result.push_back(*j);
        cout << "[" << j->pp() << "]" ;
        i++;
      };
      k++;
    }
    cout << endl;
  } else
  {cout << "[]" << endl;}
  
  return;
}
 
data_assignment make_var(data_variable datavar, int n){
  char buffer [99];
  sprintf(buffer, "%s^%d",datavar.name().c_str(), n);
  
  data_variable w(buffer, datavar.type() );
  data_assignment a(datavar , w.to_expr() );
  return a;
} 

void constelm(string filename, string outfile, int option)
{
  data_expression_list          vinit; //init vector
  vector< data_assignment >newstatevector; // newstate vector
  vector< data_assignment >tvector;
  vector< data_assignment >sv;
  vector< data_assignment >ainit; 
  data_variable_list            freevars;
  int                           n;    //number of process parameters
  int                           newdatvar = 0 ;
  set < data_expression >          listofnonconst;
  vector< summand_list::iterator > sum_true;
  
  // Load LPE input file
  specification spec;
  if (!spec.load(filename))
  {
    cerr << "could not read " << filename << endl;
    return;
  }
 
  cout << endl <<" Read from input file : " << filename << endl;
  LPE lpe = spec.lpe();


  //spec.m_sorts = spec.m_sorts;
  //
  // Determine the inital processes
  // 
  n = lpe.process_parameters().size();
  
  for(data_assignment_list::iterator i = spec.initial_assignments().begin(); i != spec.initial_assignments().end() ; i++ ){
    sv.push_back(*i);
  }
  //sv = spec.initial_assignments();
  
  ainit = sv;
  newstatevector = sv;  

  data_equation_list equations = spec.equations();
  
  for (data_variable_list::iterator di = spec.initial_free_variables().begin(); di != spec.initial_free_variables().end(); di++){
    push_front(freevars, *di);
  }
  for (data_variable_list::iterator di = lpe.free_variables().begin(); di != lpe.free_variables().end(); di++){
    push_front(freevars, *di);
  }  
  //freevars = concat(spec.initial_free_variables(), lpe.free_variables());
  
  gsRewriteInit(gsMakeDataEqnSpec(aterm_list(equations)), GS_REWR_INNER3); 
  
  set< int > V; 
  set< int > S;

  for(int j=0; j <= (n-1) ; j++){
    V.insert(j);
  };
  
  set< int > D;

  ////
  // Als V <  S dan zijn er variabele process parameters gevonden
  ////
  if (verbose){
    cout << endl << " Output of: "<< filename <<endl << endl;
  };

  int count = 1; 
  while(S.size()!=V.size()) {
    sv = newstatevector;

    S = V;
    V.clear();
    set< int > S_dummy;  
    sum_true.clear();      
    
    if (verbose)
    {
      print_set(S);
      cout << "\033[0;1m Iteration           : \033[m" << count++ << endl; 
      cout << "\033[0;1m Current statevector : \033[m"; print_statevector(sv);
      cout << "\033[0;1m Resulting Nextstates: \033[m" << endl;
    };
    
    for(summand_list::iterator s_current = lpe.summands().begin(); s_current != lpe.summands().end(); ++s_current){ 
      if (eval_cond(s_current->condition(), sv, equations , S)) {
        //assert(false);
        sum_true.push_back(s_current);
        
        vector< data_assignment > ass_nextstate;
	for(data_assignment_list::iterator i = s_current->assignments().begin(); i != s_current->assignments().end() ; i++ ){
	  ass_nextstate.push_back(*i);
	 // cout << i -> pp() << "--------" << endl;
	}

	tvector = nextstate(sv, ass_nextstate, equations, lpe );

        vector< data_expression > rhstv = rhsl(tvector);
        vector< data_variable > lhstv = lhsl(tvector);
        vector< data_expression > rhsnsv = rhsl(newstatevector);      
      
        set< int >::iterator j = S.begin();
        
       	
        while (j != S.end()){
	  data_expression rhs_tv_j  = rhstv[*j];
	  data_expression rhs_nsv_j = rhsnsv[*j];
	  bool skip = false; 

	   //Begin freevar treatment  
          data_variable_list::iterator f = freevars.begin();
	  while (f != freevars.end()){

            data_expression foe = f->to_expr(); 
	        if ( compare(rhs_tv_j  , foe, equations, opt_right ) || 
	             compare(rhs_nsv_j , foe ,equations, opt_right ) ){
	            skip = true;
	            if (compare(rhs_nsv_j , foe , spec.equations(), opt_right )){ 
	              newstatevector[*j] = tvector[*j];
              };    
              if (0 != listofnonconst.count(rhs_tv_j) )
	      {
                S_dummy.insert(*j);
              }
	          };
	          f++;
	        };
	        // End Freevar treatment 
	        
	        if (!skip){ 
	          if ((rhsl(ainit)[*j]) != rhs_tv_j ){
                    S_dummy.insert(*j);
                    data_assignment newass = make_var(lhstv[*j], newdatvar++);
                    newstatevector[*j] = newass;
                    listofnonconst.insert(newass.rhs());
                    
                  };
	        };
          j++;
        };

        if(verbose)	
	      {
	      cout << "     ";
              for(vector<data_assignment>::iterator i = newstatevector.begin();i != newstatevector.end() ;i++){
                cout << "[" << i->pp() << "]" ;
	      }	      
  	      cout << endl; 
        };
      };
         
    }; 
    
    set_difference(S.begin(), S.end(), S_dummy.begin(), S_dummy.end(), inserter(V, V.begin()));
    if (verbose){
      cout << endl;
    };
  }; 
    

  if (reachable){
   // rebuild_summands(lpe,sum_true); 
  }

  if (verbose){
    print_const(spec , S);
   // cout << sum_true.size() << endl;
  }
  rebuild_lpe(spec, outfile, S, nosingleton); 

  // Finalise
  gsRewriteFinalise();
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
       ("reachable,r", "elimantes summands which cannot be reached")
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
       verbose = true;
	   }

     if (vm.count("nosingleton")) {
       //cerr << "Active: no removal of process parameters which have sorts of cardinatilty one" << endl;
       nosingleton = true;
	   }

     if (vm.count("nocondition")) {
       //cerr << "Active: All conditions are true" << endl;
       alltrue = true;
	   }

     if (vm.count("reachable")) {
       reachable = true;
	}

     if (vm.count("INFILE")){
       filename = vm["INFILE"].as< vector<string> >();
	   }

     if (filename.size() > 2){
        cerr << "Specify only INPUT and/or OUTPUT file (Too many arguments)."<< endl;
     };
             

     if (filename.size() == 2){
       constelm(filename[0], filename[1], opt); 
     };
	
     if(filename.size() == 1){
      constelm(filename[0], addconstelm(filename[0]) , opt);
        
    };

    }
    catch(exception& e){
      cerr << e.what() << "\n";
      return 1;
    }    
    
    return 0;
}

