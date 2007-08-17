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
 

 #define debug
 
 
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
#include "print/messaging.h" 

//LPS-Framework 
#include "mcrl2/pbes/pbes.h" 
#include "mcrl2/pbes/pbes_expression.h" 
//#include "mcrl2/pbes/propositional_variable.h" 
#include "mcrl2/pbes/fixpoint_symbol.h" 
#include "mcrl2/pbes/utility.h" 
#include "mcrl2/data/data_expression.h" 
#include "mcrl2/data/data_operators.h" 
#include "mcrl2/data/sort.h" 
 
#include "mcrl2/lps/detail/algorithms.h"

//ATERM-specific 
#include "atermpp/substitute.h" 
#include "mcrl2/basic/identifier_string.h" 
#include "atermpp/utility.h" 
#include "atermpp/indexed_set.h" 
#include "atermpp/table.h" 
#include "atermpp/vector.h" 
#include "atermpp/set.h" 
#include "gc.h" 
 
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
// E ::= 0 | 1 | i | j | k | E + E
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





data_expression to_bool_expression(int n)
{ // parse a boolean expression
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
  cout<<"Expr_list: "<< pp(d).c_str() <<"\n";
#endif
  
  return d;
}





//========================================
pbes_expression to_pbes_expression(int n){
//========================================

  using namespace pbes_expr;
#ifdef debug
  cout<<"to_pbes_expression, position "<< n <<"\n";
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
	//	cout<<"\n1\n";
	propositional_variable_instantiation propinst;
	//	cout<<"\n2\n";
	
	if (ps->arg1[n]==0) // no parameters
	  {	  propinst = propositional_variable_instantiation(s);
	    //	    cout<<"\n3\n";
}
	else
	  {
	  propinst = propositional_variable_instantiation
	    (s, get_expr_list(ps->arg1[n]));	  
	  //	cout<<"\n4\n";

	  }
	//	cout<<"\n5\n";

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
      // since there is no pbes_expr constructor
      // for negation
    case '!':
      res = val(lps::data_expr::not_(to_nat_expression(ps->arg1[n])));      
      break;
    default: 
      gsErrorMsg("cannot parse pbes_expression"); exit(0);
    }
#ifdef debug
  cout << "result: "<<pp(res).c_str()<<"\n";
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
	  cout << "Data variable name expected, got "<< ps->op[vn] <<"\n";
	};

      data_variable v = data_variable(s);
      d = push_back(d,v);
    }
#ifdef debug
  cout<<"Var_list: "<< pp(d).c_str() <<"\n";
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
      cout << "standard_instance: Expected a,b,c,d,i,j,k\n";
      return del;
    }
    del = push_back(del,de);
  }
#ifdef debug
  cout<<"Instantiated "<<pp(dv).c_str()<<" as "<<pp(del).c_str()<<"\n";
#endif
  return del;
}


// functie van Muck
// fills in Bool and Nat
lps::data_specification get_minimal_data_spec()
{
  std::stringstream ss;
  ss << "map i: Nat; b:Bool; init delta;";
  
  ATermAppl r = lps::detail::parse_specification(ss);
  r = lps::detail::type_check_specification(r);
  r = lps::detail::implement_data_specification(r);
  
  lps::specification spec(r);
  return spec.data();
}



//=========================
pbes pbes_simple_to_pbes(){
//=========================

  int i;
  pos = pos-1;
#ifdef debug
  cout <<"\nps:\n";
  for (i=0;i<ps->nops;i++)
    cout<<i<<" : "<<ps->op[i]<<" "<<ps->arg1[i]<<" "<<ps->arg2[i]<<"\n";
  cout << "\nStarting parsing at position " << pos <<"\n";  
#endif
  
  equation_system eqs = equation_system(); 
  for(i=0; i<ps->nops; i++)
    if (ps->op[i] == 'Q') // new equation
      {
#ifdef debug
	cout<<"New equation starting at position "<< i <<"\n";
#endif
	pbes_equation pbeq = to_pbes_eq(i);
	eqs = eqs + pbeq;
      }


  data_specification data = get_minimal_data_spec();
  
  // create a standard instantiation for the variable
  // of the first equation
  propositional_variable pv = eqs.begin()->variable();
  propositional_variable_instantiation ppv = 
    propositional_variable_instantiation
    (pv.name(),standard_instance(pv.parameters()));
  
  pbes pb(data,eqs,ppv);
  return pb;
}
 

//====================================
pbes make_pbes(const string fileName){
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
  cout << "txt parsed. Now making a pbes.\n";
#endif
  return pbes_simple_to_pbes();
}

 
 
//========================
void parse_command_line(int argc, char** argv){
//========================

  po::options_description desc; 
  desc.add_options() 
    ("verbose,v",	"turn on the display of short intermediate messages") 
    ("debug,d",		"turn on the display of detailed intermediate messages") 
    ("version",		"display version information") 
    ("syntax,s", "display the syntax and examples of the input txt")
    ("help,h",		"display this help") 
    ; 
   
  po::options_description hidden("Hidden options"); 
  hidden.add_options() 
    ("INFILE",              po::value<string>(), "input file") 
    ; 
   
  po::options_description cmdline_options; 
  cmdline_options.add(desc).add(hidden);  
   
  po::positional_options_description p; 
  p.add("INFILE", -1); 
   
  po::variables_map vm; 
  po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm); 
  po::notify(vm); 
   
  if (vm.count("help")) { 
    cerr << "Usage: " << argv[0] << " [OPTION]... INFILE" << endl; 
    cerr << "Parses the content of INFILE into a pbes specification.The result is written to INFILE.pbes" << endl; 
    cerr << endl; 
    cerr << desc <<endl; 
    exit(0); 
  } 
   
  if (vm.count("version")) { 
    cerr << NAME << " " << VERSION  << endl; 
    exit(0); 
  } 
   
  if (vm.count("debug")) {
    gsSetDebugMsg(); 
    pbesdebug = 1;
  }

  if (vm.count("verbose")) 
    gsSetVerboseMsg(); 
   
  if (vm.count("syntax")){
    cerr<<"\nSYNTAX of the simple pbes text language\n";
    cerr<<"(This is an extreme simplification of the official PBES grammar.)\n\n";
    cerr<<"pbes        ::= pbes-eq pbes-eq .... \n\
\n\
pbes-eq     ::= \"mu\" predvar \"=\" pbes-expr  | \"nu\" predvar \"=\" pbes-expr \n\
\n\
predvar	    ::= \"X\" | \"Y\" | \"Z\" | \"X\" \"(\" params \")\" \
| \"Y\" \"(\" params \")\" | \"Z\" \"(\" params \")\"\n\n";

cerr <<"pbes-expr   ::= \"T\" | \"F\" | boolvar | nat-expr \"<\" nat-expr \
| nat-expr \"=\" nat-expr \
| predvar-inst \n\
                | pbes-expr \"&&\" pbes-expr | pbes-expr \"||\" pbes-expr \
| \"!\" pbes-expr | \"(\" pbes-expr \")\"\n\n";

    cerr<<"boolvar     ::= \"a\" | \"b\" | \"c\"\n\n\
natvar      ::= \"i\" | \"j\" | \"k\"\n\n\	
nat-expr    ::= \"1\" | natvar | nat-expr \"+\" nat-expr | \"(\" nat-expr \")\"\n\n\
predvar-inst::= \"X\" \"(\" params-inst \")\" | \"Y\" \"(\" params-inst \")\" \  
| \"Z\" \"(\" params-inst \")\"\n\n\
params	    ::=  boolvar | natvar | params \",\" boolvar | params \",\" natvar\n\n\
params-inst ::= boolvar	| \"!\" boolvar	| nat-expr | boolvar \",\" params-inst \
| \"!\" boolvar \",\" params-inst | nat-expr\",\" params-inst\n\n";

    cerr<<"\nEXAMPLES:\n\n";
    cerr<<"mu X(b,i) = b && X(T,i+1)\n\n";
}

   
  infilename = (0 < vm.count("INFILE")) ? vm["INFILE"].as<string>() : "-"; 

}


 
 
 
 
//MAIN =================================== 
int main(int argc, char** argv) 
{ 
  //Initialise ATerm library and lowlevel-functions 
  ATerm bottom; 
  ATinit(argc, argv, &bottom); 
  gsEnableConstructorFunctions(); 
   
  parse_command_line(argc, argv);

  // infilename is now filled in
  if (infilename != "-")
    {
      outfilename = infilename + ".pbes";

      gsVerboseMsg("Creating pbes ( '%s' ) from text ( '%s' )",outfilename,infilename);
      //Create the pbes from the input text 
      pbes p = make_pbes(infilename); 

      if(!p.save(outfilename, false))
	gsErrorMsg("writing failed\n");

      gsVerboseMsg("done");

    }

  return 0; 
} 
//======================================== 
 
 
 
 
