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
//#include <aterm2.h>
//#include "atermpp/aterm.h"
//#include "mcrl2/mcrl2_visitor.h"
//#include "mcrl2/specification.h"
//#include "mcrl2/predefined_symbols.h"
//#include "mcrl2/sort.h"

#include "libgsrewrite.h"
#include "gsfunc.h"
#include "gslowlevel.h"

using namespace std;
//using namespace mcrl2;
//using namespace atermpp;

namespace po = boost::program_options;
po::variables_map vm;

//Constanten
string version = "Version 0.4";
bool verbose    = false; 
bool alltrue    = false;
bool reachable  = false;
bool nosingleton = false;

const int opt_none        = 0;
const int opt_left       = 1;
const int opt_right      = 2;
const int opt_both       = 3;

/*
ATermAppl rew2(ATermAppl t, ATermAppl rewrite_terms)
{
  gsEnableConstructorFunctions();
  ATermAppl result = gsRewriteTerm(t);
  return result;
}
*/
/*
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
*/
/*
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
*/
/*
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
*/
/*
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
*/

/*
bool compare(data_expression x, data_expression y, data_equation_list equations, int option)
{
  if (option == opt_none ){
    return x==y;
  };
 
 if (option == opt_left){
    ATermAppl x1 = rew2(x.to_ATermAppl(), gsMakeDataEqnSpec(equations.to_ATermList()));
    ATermAppl y1 = y.to_ATermAppl() ;
    return atermpp::aterm(x1) == atermpp::aterm(y1);     
  };
 
 if (option == opt_right){
    ATermAppl x1 = x.to_ATermAppl() ;
    ATermAppl y1 = rew2(y.to_ATermAppl(), gsMakeDataEqnSpec(equations.to_ATermList()));
    return atermpp::aterm(x1) == atermpp::aterm(y1);     
  };
 
 if (option == opt_both){
    ATermAppl x1 = rew2(x.to_ATermAppl(), gsMakeDataEqnSpec(equations.to_ATermList()));
    ATermAppl y1 = rew2(y.to_ATermAppl(), gsMakeDataEqnSpec(equations.to_ATermList()));
    return atermpp::aterm(x1) == atermpp::aterm(y1);     
  };

  cout << "\033[0;31mError in Rewrite\033[0m" << endl;
  ATermAppl x1 = rew2(x.to_ATermAppl(), gsMakeDataEqnSpec(equations.to_ATermList()));
  ATermAppl y1 = rew2(y.to_ATermAppl(), gsMakeDataEqnSpec(equations.to_ATermList()));
  return atermpp::aterm(x1) == atermpp::aterm(y1);  
 
}
*/ 
/*
data_assignment_list nextstate(data_assignment_list currentstate, data_assignment_list assignments, data_equation_list equations, LPE lpe)
{
  data_assignment_list out;
  data_assignment_list out1;
  data_assignment_list out2;
  data_expression z;

  //Speedup
  data_variable d;
  data_assignment_list::iterator i = currentstate.begin();
  while(i != currentstate.end() ){
    d = i->lhs();
    out = push_front(out, data_assignment(d, d.to_expr() ));
    i++;
  
  }
  //Keep in mind list is reversed -- Does not match set S!!!!
  out = reverse(out);
  //End speedup
 
  //data_variable d;
  i = out.begin();
  while(i != out.end() ){
    data_assignment_list::iterator j = assignments.begin();
    d = i->lhs();
    z = d.to_expr();
    while( j != assignments.end() ){
      z = z.substitute( *j );    
      j++;
    } 
    out1 = push_front(out1, data_assignment(d , z));
    i++;
  } 
  out1 = reverse(out1);
  
  i = out1.begin();
  while(i != out1.end() ){
    data_assignment_list::iterator j = currentstate.begin();
    z = i->rhs();
    while( j != currentstate.end() ){
      z = z.substitute( *j );    
      j++;
    } 
    out2 = push_front(out2, data_assignment(i->lhs(),data_expression( rew2(z.to_ATermAppl(), gsMakeDataEqnSpec(equations.to_ATermList())))));
    i++;
  }
  out2 = reverse(out2);

 return out2; 
}
*/ 
/*
data_expression_list rhsl(data_assignment_list x)
{
  data_expression_list y;
  data_assignment_list::iterator i = x.begin();
  while (i != x.end()) {
    y = push_front(y, i->rhs() );
    i++;
  };
  y = reverse(y);  
  return y;
}
*/

/*
data_variable_list lhsl(data_assignment_list x)
{
  data_variable_list y;
  data_assignment_list::iterator i = x.begin();
  while (i != x.end()) {
    y = push_front(y, i->lhs() );
    i++;
  };
  y = reverse(y); 
  return y;
}
*/

/*
bool eval_cond(data_expression datexpr, data_assignment_list statevector, data_equation_list equations, set<int> S){

  bool b;
  
  if (alltrue){
    return true;
  };

  set<int>::iterator i;
  data_assignment_list conditionvector; 
  
  //Speedup
  data_variable_list    sv1 = lhsl(statevector);
  data_expression_list  sv2 = rhsl(statevector);
  data_variable_list::iterator isv1 = sv1.begin();
  data_expression_list::iterator isv2 = sv2.begin();

  i = S.begin();
  int z = 0;
  while (i != S.end()) {
    if(z == *i) {
      conditionvector = push_front(conditionvector, data_assignment(  *isv1 , *isv2 ));
      i++;
    }
    isv1++; 
    isv2++;
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
*/
/*
void  rebuild_lpe(specification spec,string  outfile, set< int > S, bool single){

  LPE lpe = spec.lpe();

  set< int >::iterator i;
  data_assignment_list sub = spec.init_assignments();
  data_assignment_list result;
  
  i = S.begin();
  while (i != S.end()) {
    if (!single){ //&& element_at(lhsl(sub), *i).type().size() == 1){
      result = append(result, data_assignment(  element_at(lhsl(sub), *i) , element_at( rhsl(sub), *i  )));
     };
    i++;
  };
  
  data_assignment_list::iterator j = result.begin();
  while(j != result.end() ){
    lpe = lpe.substitute(*j);
    j++;
  }

  //spec2 = specification(spec.sorts(), spec.constructors(), spec.mappings(), spec.equations(), spec.initial_state(), spec.init_variables(), spec.init_assignments, spec.lpe().to_ATermAppl() );

  
  if  (spec.save(outfile)){
    cout << " Written output file: " << outfile << endl << endl;
  } else
  {
    cout << " \033[0;31mUnsuccefully\033[0m written to output file: " << outfile << endl;
  }

  return;
}
*/
/*
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
    cout << "\033[0;1m Constant Process parameters : \033[m" << endl;
    cout << "     "<< result.pp() << endl <<endl;
  };
 
  return;
}
*/ 
/*data_assignment make_var(data_variable datavar, int n){
  char buffer [99];
  sprintf(buffer, "%s^%d",datavar.name().c_str(), n);
  
  data_variable w(buffer, datavar.type() );
  data_assignment a(datavar , w.to_expr() );
  return a;
} */
/*
void constelm(string filename, string outfile, int option)
{
  data_expression_list          vinit; //init vector
  data_assignment_list          newstatevector; // newstate vector
  data_assignment_list          tvector;
  data_assignment_list          sv;
  data_assignment_list          ainit; 
  data_variable_list            freevars;
  int                           n;    //number of process parameters
  int                           newdatvar = 0 ;
  data_expression_list          listofnonconst;
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
  sv = spec.init_assignments();
  ainit = spec.init_assignments();
  newstatevector = sv;  

  data_equation_list equations = spec.equations();
  
  freevars = concat(spec.initial_free_variables(), lpe.free_variables());
  
  gsRewriteInit(gsMakeDataEqnSpec(equations.to_ATermList()), GS_REWR_INNER3); 
  
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
      cout << "\033[0;1m Current statevector : \033[m" << sv.pp() << endl;
      cout << "\033[0;1m Resulting Nextstates: \033[m" << endl;
    };
    
    for(summand_list::iterator s_current = lpe.summands().begin(); s_current != lpe.summands().end(); ++s_current){ 
      if (eval_cond(s_current->condition(), sv, equations , S)) {
        
        sum_true.push_back(s_current);
        

        data_assignment_list ass_nextstate = s_current->assignments();
        tvector = nextstate(sv, ass_nextstate, equations, lpe );

        data_expression_list rhstv = rhsl(tvector);
        data_variable_list lhstv = lhsl(tvector);
        data_expression_list rhsnsv = rhsl(newstatevector);      
      
        set< int >::iterator j = S.begin();
       
        while (j != S.end()){
          data_expression rhs_tv_j = element_at(rhstv, *j);
          data_expression rhs_nsv_j = element_at(rhsnsv, *j);
	  bool skip = false; 

	   //Begin freevar treatment  
          data_variable_list::iterator f = freevars.begin();
	  while (f != freevars.end()){

            data_expression foe = f->to_expr(); 
	        if ( compare(rhs_tv_j  , foe, equations, opt_right ) || 
	             compare(rhs_nsv_j , foe ,equations, opt_right ) ){
	            skip = true;
	            if (compare(rhs_nsv_j , foe , spec.equations(), opt_right )){ 
	              newstatevector = replace(newstatevector, element_at(tvector, *j), *j);
              };    
              if (-1 != index_of(listofnonconst, rhs_tv_j, 0) ) {
                S_dummy.insert(*j);
              }
	          };
	          f++;
	        };
	        // End Freevar treatment 
	        
	        if (!skip){ 
	          if (element_at(rhsl(ainit), *j) != rhs_tv_j ){
                    S_dummy.insert(*j);
                    data_assignment newass = make_var(element_at(lhstv, *j), newdatvar++);
                    newstatevector = replace(newstatevector, newass, *j);
                    listofnonconst = push_front(listofnonconst, newass.rhs());
                    
                  };
	        };
          j++;
        };

        if(verbose)	
	      {
  	      cout << "     " << newstatevector.pp() << endl; 
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
    //cout << sum_true.size() << endl;
  }
  rebuild_lpe(spec, outfile, S, nosingleton); 

  // Finalise
  gsRewriteFinalise();
  return;
}
*/
int main(int ac, char* av[])
{
/*   ATerm bot;
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
 */   
    return 0;
}

