//  Copyright 2007 Simona Orzan. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./txt2pbes.cpp

//====================================================================== 
// 
//  
// 
//====================================================================== 
 
 
#define NAME "txt2pbes" 
#define VERSION "0.0.1" 
 


// #define debug
 
 
//C++ 
#include <cstdio> 
#include <exception> 
#include <iostream> 
#include <fstream> 
#include <string> 
#include <utility> 
 
#include <sstream> 
 
//Boost 
#include <boost/program_options.hpp> 
 
//MCRL-specific 
#include "mcrl2/core/messaging.h" 

//LPS-Framework 
#include "mcrl2/pbes/pbes.h" 
#include "mcrl2/pbes/pbes_expression.h" 
//#include "mcrl2/pbes/propositional_variable.h" 
#include "mcrl2/pbes/fixpoint_symbol.h" 
#include "mcrl2/pbes/utility.h" 
#include "mcrl2/data/data_expression.h" 
#include "mcrl2/data/data_operators.h" 
#include "mcrl2/data/sort_expression.h" 
 
#include "mcrl2/lps/detail/algorithms.h"


#include "mcrl2/core/detail/parse.h"
#include "mcrl2/core/detail/typecheck.h"
#include "mcrl2/core/detail/data_implementation.h"
#include "mcrl2/core/alpha.h"

// command-line options
#include <getopt.h>

//ATERM-specific 
#include "atermpp/substitute.h" 
#include "mcrl2/core/identifier_string.h" 
#include "atermpp/utility.h" 
#include "atermpp/indexed_set.h" 
#include "atermpp/table.h" 
#include "atermpp/vector.h" 
#include "atermpp/set.h" 
#include "gc.h" 


// only for debug
//#include "mcrl2/core/libstruct_core.h"
 
//#include "pbes_simple.h"
#include "pbesparser.cpp"









using namespace std; 
using namespace lps; 
using namespace mcrl2::utilities; 
using namespace pbes_expr;
 
namespace po = boost::program_options; 
 
//Type definitions ====================== 
 

string infilename; 
string outfilename; 

//t_pbes_simple ps;




//=======================================
data_expression to_nat_expression(int n)
// constructs expressions 
// N ::= 0 | 1 | i | j | k | N + N
//=======================================
{
  
  switch (ps->op[n])
    {  
    case '0': 
      return (lps::data_expr::nat(0));
    case '1': 
      return (lps::data_expr::nat(1));
    case 'i': case 'j': case 'k':
      {
	string s("");s = s +  ps->op[n] + ":Nat";
	data_variable v = data_variable(s);
	data_expression dv = data_expression(v);
	return (dv);
      }
    case '+':
      return (lps::data_expr::plus(to_nat_expression(ps->arg1[n]),
				   to_nat_expression(ps->arg2[n])));
      // default should be unreachable
    default: return (data_expr::nat(0));
    }
}




//=======================================
data_expression to_bool_expression(int n)
//=======================================
{ // parse an expression of boolean type, without quantifiers.
  // B = T | F | a | b | c | (N=N) | (N<N) | !B | B && B | B || B
 switch (ps->op[n])
    {  
    case 'T': 
      return (lps::data_expr::true_());
    case 'F': 
      return (lps::data_expr::false_());
    case 'a': case 'b': case 'c':
      {
	string s("");s = s +  ps->op[n] + ":Nat";
	data_variable v = data_variable(s);
	data_expression dv = data_expression(v);
	return (dv);
      };
    case '<': 
      return (lps::data_expr::less(to_nat_expression(ps->arg1[n]),
				   to_nat_expression(ps->arg2[n])));     
    case '=': 
      return(lps::data_expr::equal_to(to_nat_expression(ps->arg1[n]),
				      to_nat_expression(ps->arg2[n])));
    case '!':
      return (lps::data_expr::not_(to_bool_expression(ps->arg1[n])));
    case '&':
      return (lps::data_expr::and_(to_bool_expression(ps->arg1[n]),
				   to_bool_expression(ps->arg2[n])));
    case '|':
      return (lps::data_expr::or_(to_bool_expression(ps->arg1[n]),
				   to_bool_expression(ps->arg2[n])));
      // default should be unreachable
    default: return (data_expr::true_());
    }    
  }


data_expression get_data_expr(int n)
{
  if ((ps->op[n] == 'i') || (ps->op[n] == 'j') || (ps->op[n] == 'k') 
      || (ps->op[n] == '+') || (ps->op[n] == '0') || (ps->op[n] == '1'))
    return(to_nat_expression(n));
  else 
    return (to_bool_expression(n));
}


//=========================================
data_expression_list get_expr_list(int n)
//=========================================
{
  data_expression_list d;
  int en;
  while (n>0)    
    {
      if (ps->op[n] == ',')
	{en = ps->arg1[n]; n = ps->arg2[n];}
      else 
	{en = n; n = 0;}
      data_expression e = get_data_expr(en);
      d = push_back(d,e);
    }
#ifdef debug
  cerr<<"Expr_list: "<< pp(d).c_str() <<"\n";
#endif
  
  return d;
}





//========================================
pbes_expression to_pbes_expression(int n){
//========================================

  using namespace pbes_expr;
#ifdef debug
  cerr<<"to_pbes_expression, position "<< n <<"\n";
#endif
  pbes_expression res;
  switch (ps->op[n])
    {
      // TRUE, FALSE
    case 'T':  
      res = lps::pbes_expr::true_(); break;
    case 'F':  
      res = lps::pbes_expr::false_(); break;
      // IDPROP
    case 'a': case 'b': case 'c': 
      {
	string s("");s = s + ps->op[n] + ":Bool";
	data_variable v = data_variable(s);
	res = val(data_expression(v));
	break;
      }        
      // IDPRED [ expr_list ]
    case 'X': case 'Y': case 'Z': 
      {
	string s(""); s = s + ps->op[n];
	//	cerr<<"\n1\n";
	propositional_variable_instantiation propinst;
	//	cerr<<"\n2\n";
	
	if (ps->arg1[n]==0) // no parameters
	  {	  propinst = propositional_variable_instantiation(s);
	    //	    cerr<<"\n3\n";
}
	else
	  {
	  propinst = propositional_variable_instantiation
	    (s, get_expr_list(ps->arg1[n]));	  
	  if (!is_propositional_variable_instantiation(propinst))
	    gsVerboseMsg(" \n\n\nNEEEE\n\n\n");
	  //	cerr<<"\n4\n";

	  }
	//	cerr<<"\n5\n";

	res = pbes_expression(propinst);
	break;
      }      
      // nat_expr LESS,IS nat_expr
    case '<': 
      res = val(lps::data_expr::less(to_nat_expression(ps->arg1[n]),
				     to_nat_expression(ps->arg2[n])));
      break;
    case '=': 
      res = val(lps::data_expr::equal_to(to_nat_expression(ps->arg1[n]),
					 to_nat_expression(ps->arg2[n])));
      break;
      // pbes_expression EN,OF pbes_expression
    case '&':
      res = lps::pbes_expr::and_(to_pbes_expression(ps->arg1[n]),
				 to_pbes_expression(ps->arg2[n]));
      break;
    case '|':
      res = lps::pbes_expr::or_(to_pbes_expression(ps->arg1[n]),
				to_pbes_expression(ps->arg2[n]));
      break;
      // NEG pbes_expression 
      // in fact it only works for data_expressions under negation, 
      // since there is no pbes_expression constructor
      // for negation
    case '!':
      res = val(lps::data_expr::not_(to_bool_expression(ps->arg1[n])));      
      break;
    default: 
      gsErrorMsg("cannot parse pbes_expression"); exit(0);
    }
#ifdef debug
  cerr << "result: "<<pp(res).c_str()<<"\n";
#endif
  return res;
}



//======================================
data_variable_list get_var_list(int n)
//======================================
{
  data_variable_list d;
  int vn;
  while (n>0)    
    {
      if (ps->op[n] == ',')
	{vn = ps->arg1[n]; n = ps->arg2[n];}
      else 
	{vn = n; n = 0;}
      string s("");
      switch (ps->op[vn])
	{  
	case 'i': case 'j': case 'k':
	  {s = s +  ps->op[vn] + ":Nat";break;}
	case 'a': case 'b': case 'c':
	  {s = s +  ps->op[vn] + ":Bool";break;}
	default:
	  cerr << "Data variable name expected, got "<< ps->op[vn] <<"\n";
	};

      data_variable v = data_variable(s);
      d = push_back(d,v);
    }
#ifdef debug
  cerr<<"Var_list: "<< pp(d).c_str() <<"\n";
#endif
  
  return d;
}







//==============================
pbes_equation to_pbes_eq(int n){
//==============================

  fixpoint_symbol f;  
  propositional_variable prop;
  if (ps->op[ps->arg1[n]] == 'm')
    f = fixpoint_symbol(gsMakeMu()); // mu()
  else if (ps->op[ps->arg1[n]] == 'n')
    f = fixpoint_symbol(gsMakeNu()); //nu();
  else {
    gsErrorMsg("fixpoint expected\n");
    exit(0);     
  }
 
  string sop(""); sop=sop+ps->op[ps->arg1[ps->arg1[n]]]; // name
  data_variable_list dvl; // variable list
  dvl = get_var_list(ps->arg1[ps->arg1[ps->arg1[n]]]);
  if (dvl.empty())
    prop = propositional_variable(sop);
  else
    prop = propositional_variable(sop,dvl);

  pbes_expression rhs = to_pbes_expression(ps->arg2[n]);
  pbes_equation pbeq(f,prop,rhs);
  return pbeq;
}





//============================================================
data_expression_list standard_instance(data_variable_list dv){
//============================================================
  data_expression_list del;
  
  data_variable_list::iterator i = dv.begin();
  for ( ; i != dv.end(); i++){
    data_expression de;
    string iname(i->name());
    if ((iname == "a") || (iname == "b") || (iname == "c"))
      de = true_();
    else if ((iname == "i") || (iname == "j") || (iname == "k"))
      de = data_expr::nat(0);
    else {
      cerr << "standard_instance: Expected a,b,c,d,i,j,k\n";
      return del;
    }
    del = push_back(del,de);
  }
#ifdef debug
  cerr<<"Instantiated "<<pp(dv).c_str()<<" as "<<pp(del).c_str()<<"\n";
#endif
  return del;
}


// functie van Muck
// fills in Bool and Nat
lps::data_specification get_minimal_data_spec()
{
  std::stringstream ss;
  //  ss << "map i:Nat; b:Bool; init delta;";
  ss << "init delta@0;";
  ATermAppl r = parse_spec(ss);
#ifdef debug
  cerr<<"typecheck\n";
#endif
  r = type_check_spec(r);
#ifdef debug
  cerr<<"implement data spec\n";
#endif
  r = implement_data_spec(r);
#ifdef debug
  cerr<<"linearise spec\n";
#endif
  r = linearise_std(r,t_lin_options());
#ifdef debug
  cerr<<"ready\n";
#endif

  lps::specification spec(r);
  return spec.data();
}



//=========================
pbes<> pbes_simple_to_pbes(){
//=========================

  int i;
  pos = pos-1;
#ifdef debug
  cerr <<"\nps:\n";
  for (i=0;i<ps->nops;i++)
    cerr<<i<<" : "<<ps->op[i]<<" "<<ps->arg1[i]<<" "<<ps->arg2[i]<<"\n";
  cerr << "\nStarting parsing at position " << pos <<"\n";  
#endif
  
  atermpp::vector<pbes_equation> eqs; 
  for(i=0; i<ps->nops; i++)
    if (ps->op[i] == 'Q') // new equation
      {
#ifdef debug
	cerr<<"New equation starting at position "<< i <<"\n";
#endif
	pbes_equation pbeq = to_pbes_eq(i);
	eqs.push_back(pbeq);
      }


  data_specification data = get_minimal_data_spec();
  
  // create a standard instantiation for the variable
  // of the first equation
  propositional_variable pv = eqs.begin()->variable();
  propositional_variable_instantiation ppv = 
    propositional_variable_instantiation
    (pv.name(),standard_instance(pv.parameters()));
  
  pbes<> pb(data,eqs,ppv);
  return pb;
}
 

//====================================
pbes<> make_pbes(const string fileName){
//====================================

  //  pbes* newpbes = new pbes();
  /*
  string ext = fileName.substr( fileName.find_last_of( "." ) + 1, string::npos );
  string ext_lc = ext;

  for ( unsigned int i = 0 ; i < ext.size() ; ++i )
    {
    ext_lc[i] = tolower(ext_lc[i]);
  }
  
  if ( ext_lc != "txt" )
    {
    throw string( "Unknown file extension: " + ext );
  }
  else
    {
*/
  ps = new (struct spbes);
  pos = 0; add(' ',0,0); pos++;  // convenient
  parsePBES(fileName);

       //    }
#ifdef debug  
  cerr << "txt parsed. Now making a pbes.\n";
#endif
  return pbes_simple_to_pbes();
}

 



void print_help(void)
{
  fprintf(stderr,
    "\nUsage: txt2pbes [OPTION]... [INFILE [OUTFILE]]\n"
    "Parses the content of INFILE into a pbes specification.The result is written to OUTFILE\n"
    "If OUTFILE is not present, stdout is used. If INFILE is not present, stdin is\n"
    "used.\n"
    "  -s, --syntax          display the syntax (and examples) of the expected text input\n"    	  
    "  -h, --help            display this help message and terminate\n"
    "  -v, --verbose         display concise intermediate messages\n"
    "  -d, --debug           display detailed intermediate messages\n"
  );
}

void print_syntax(void)
{
  cerr<<"\nSYNTAX of the simple pbes text language\n";
  cerr<<"(This is an extreme simplification of the official PBES grammar.)\n\n";
  cerr<<"pbes        ::= pbes-eq pbes-eq .... \n"<<
"pbes-eq     ::= \"mu\" predvar \"=\" pbes-expr  | \"nu\" predvar \"=\" pbes-expr \n"
"predvar     ::= \"X\" | \"Y\" | \"Z\" | \"X\" \"(\" params \")\"	\
| \"Y\" \"(\" params \")\" | \"Z\" \"(\" params \")\"\n\n";
  
  cerr <<"pbes-expr   ::= \"T\" | \"F\" | boolvar | nat-expr \"<\" nat-expr \
| nat-expr \"=\" nat-expr						\
| predvar-inst \n"<<
"  | pbes-expr \"&&\" pbes-expr | pbes-expr \"||\" pbes-expr \
| \"!\" pbes-expr | \"(\" pbes-expr \")\"\n\n";
  
  cerr<<"boolvar     ::= \"a\" | \"b\" | \"c\"\n"<<
"natvar      ::= \"i\" | \"j\" | \"k\"\n"<<				
"nat-expr    ::= \"1\" | natvar | nat-expr \"+\" nat-expr | \"(\" nat-expr \")\"\n"<<
"predvar-inst::= \"X\" \"(\" params-inst \")\" | \"Y\" \"(\" params-inst \")\" \
| \"Z\" \"(\" params-inst \")\"\n"<<					
    "params	    ::=  boolvar | natvar | params \",\" boolvar | params \",\" natvar\n"<<
"params-inst ::= boolvar	| \"!\" boolvar	| nat-expr | boolvar \",\" params-inst \
| \"!\" boolvar \",\" params-inst | nat-expr\",\" params-inst\n\n";
  
  cerr<<"\nEXAMPLES:\n\n";
  cerr<<"mu X(i) = (i<1) && X(i+1)\n\n";
  cerr<<"mu X(b,i)   = (Y(b, i, i+1+1) && b) || !b\n";
  cerr<<"nu Y(b,i,j) = X(!b,i) && Y(b,i+1,j) && ((i+1) < j)\n\n";
}


void print_more_info(char *name)
{
  fprintf(stderr, "Use %s --help for options\n", name);
}



//========================== 
void parse_command_line(int argc, char **argv)
//==========================
{
#define SHORT_OPTIONS "hvds"
#define VERSION_OPTION CHAR_MAX + 1;
  struct option long_options[] = {
    { "syntax",   no_argument,        NULL,  's' },
    { "help",      no_argument,        NULL,  'h' },
     { "verbose",   no_argument,        NULL,  'v' },
    { "debug",     no_argument,        NULL,  'd' },
    { 0, 0, 0, 0 }
  };
  int option;
  //parse options
  while ((option = getopt_long(argc, argv, SHORT_OPTIONS, long_options, NULL)) != -1) {
    switch (option) {
    case 's': /* syntax */
      print_syntax();
      exit(0);
    case 'h': /* help */
      print_help();
      exit(0);
    case 'v': /* verbose */
      gsSetVerboseMsg();
      break;
    case 'd': /* debug */
      gsSetDebugMsg();
      break;
    default:
      print_more_info(argv[0]);
      exit(1);
    }
  }
 
  //check for wrong number of arguments
  int noargc; //non-option argument count
  noargc = argc - optind;
  if (noargc > 2) {
    fprintf(stderr, "%s: too many arguments\n", NAME);
    print_more_info(argv[0]);
    exit(1);
  } 
  else {
    //noargc >= 0 && noargc <= 2
    if (noargc > 0) {
      infilename = argv[optind];
    }
    if (noargc == 2) {
      outfilename = argv[optind + 1];
    }
  }
}



 
 
 
//MAIN =================================== 
int main(int argc, char** argv) 
{ 
  //Initialise ATerm library and lowlevel-functions 
  ATerm bottom; 
  ATinit(argc, argv, &bottom); 
  gsEnableConstructorFunctions(); 
   
  parse_command_line(argc, argv);

  cerr <<"Creating pbes ("<<outfilename<< ") from text (" << infilename <<") \n";

  //Create the pbes from the input text 
  pbes<> p = make_pbes(infilename); 
  ATermAppl ap = p;

  if (outfilename == "") {
    gsVerboseMsg("The resulting PBES is:\n");
    PrintPart_CXX(cout, (ATerm) ap, ppDefault);
    cout << endl;
  } else 
    if(!p.save(outfilename, false)){
      gsErrorMsg("writing to %s failed\n",outfilename.c_str());
      exit(1);
    }
  
  cerr <<"done\n";  

  return 0; 
} 
//======================================== 
 
 
 
 
