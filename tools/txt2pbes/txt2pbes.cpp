//  Copyright 2007 Aad Mathijssen and Simona Orzan. Distributed under
//  the Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./txt2pbes.cpp

#define NAME "txt2pbes" 
#define AUTHOR "Aad Mathijssen and Simona Orzan" 

// #define debug
 
//C++ 
#include <cstdio> 
#include <exception> 
#include <iostream> 
#include <fstream> 
#include <string> 
#include <utility> 
 
#include <sstream> 
 
//PBES-Framework 
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

//ATERM-specific 
#include "mcrl2/atermpp/substitute.h" 
#include "mcrl2/core/identifier_string.h" 
#include "mcrl2/atermpp/utility.h" 
#include "mcrl2/atermpp/indexed_set.h" 
#include "mcrl2/atermpp/table.h" 
#include "mcrl2/atermpp/vector.h" 
#include "mcrl2/atermpp/set.h" 
#include "gc.h" 

//MCRL2-specific 
#include "mcrl2/core/messaging.h" 
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h" // must come after mcrl2/core/messaging.h

// only for debug
//#include "mcrl2/core/libstruct_core.h"
 
//#include "pbes_simple.h"
#include "pbesparser.cpp"









using namespace std; 
using namespace mcrl2::utilities;
using namespace mcrl2::core; 
using namespace mcrl2::data; 
using namespace mcrl2::lps; 
using namespace mcrl2::pbes_system; 
using namespace mcrl2::pbes_system::pbes_expr;
 
//Type definitions ====================== 
 

struct t_tool_options
{
  std::string infilename;
  std::string outfilename;
  bool print_syntax;
  bool new_parser;

  t_tool_options() :
    infilename(""),
    outfilename(""),
    print_syntax(false),
    new_parser(false)
  {}
};



//=======================================
data_expression to_nat_expression(int n)
// constructs expressions 
// N ::= 0 | 1 | i | j | k | N + N
//=======================================
{
  
  switch (ps->op[n])
    {  
    case '0': 
      return (data_expr::nat(0));
    case '1': 
      return (data_expr::nat(1));
    case 'i': case 'j': case 'k':
      {
	string s("");s = s +  ps->op[n] + ":Nat";
	data_variable v = data_variable(s);
	data_expression dv = data_expression(v);
	return (dv);
      }
    case '+':
      return (data_expr::plus(to_nat_expression(ps->arg1[n]),
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
      return (data_expr::true_());
    case 'F': 
      return (data_expr::false_());
    case 'a': case 'b': case 'c':
      {
	string s("");s = s +  ps->op[n] + ":Nat";
	data_variable v = data_variable(s);
	data_expression dv = data_expression(v);
	return (dv);
      };
    case '<': 
      return (data_expr::less(to_nat_expression(ps->arg1[n]),
				   to_nat_expression(ps->arg2[n])));     
    case '=': 
      return(data_expr::equal_to(to_nat_expression(ps->arg1[n]),
				      to_nat_expression(ps->arg2[n])));
    case '!':
      return (data_expr::not_(to_bool_expression(ps->arg1[n])));
    case '&':
      return (data_expr::and_(to_bool_expression(ps->arg1[n]),
				   to_bool_expression(ps->arg2[n])));
    case '|':
      return (data_expr::or_(to_bool_expression(ps->arg1[n]),
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
      res = pbes_expr::true_(); break;
    case 'F':  
      res = pbes_expr::false_(); break;
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
      res = val(data_expr::less(to_nat_expression(ps->arg1[n]),
				     to_nat_expression(ps->arg2[n])));
      break;
    case '=': 
      res = val(data_expr::equal_to(to_nat_expression(ps->arg1[n]),
					 to_nat_expression(ps->arg2[n])));
      break;
      // pbes_expression EN,OF pbes_expression
    case '&':
      res = pbes_expr::and_(to_pbes_expression(ps->arg1[n]),
				 to_pbes_expression(ps->arg2[n]));
      break;
    case '|':
      res = pbes_expr::or_(to_pbes_expression(ps->arg1[n]),
				to_pbes_expression(ps->arg2[n]));
      break;
      // NEG pbes_expression 
      // in fact it only works for data_expressions under negation, 
      // since there is no pbes_expression constructor
      // for negation
    case '!':
      res = val(data_expr::not_(to_bool_expression(ps->arg1[n])));      
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
data_specification get_minimal_data_spec()
{
  std::stringstream ss;
  //  ss << "map i:Nat; b:Bool; init delta;";
  ss << "init delta@0;";
  ATermAppl r = parse_proc_spec(ss);
#ifdef debug
  cerr<<"typecheck\n";
#endif
  r = type_check_proc_spec(r);
#ifdef debug
  cerr<<"implement data spec\n";
#endif
  r = implement_data_proc_spec(r);
#ifdef debug
  cerr<<"linearise spec\n";
#endif
  r = linearise_std(r,t_lin_options());
#ifdef debug
  cerr<<"ready\n";
#endif

  specification spec(r);
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
  ps = new spbes;
  pos = 0; add(' ',0,0); pos++;  // convenient
  parsePBES(fileName);

       //    }
#ifdef debug  
  cerr << "txt parsed. Now making a pbes.\n";
#endif
  return pbes_simple_to_pbes();
}

void print_syntax(void)
{
  cerr <<
"SYNTAX of the simple PBES text language\n"
"(This is an extreme simplification of the official PBES grammar.)\n"
"\n"
"pbes      ::= pbes-eq | pbes-eq pbes\n"
"pbes-eq   ::= fixpoint pvar-decl '=' pbes-expr\n"
"fixpoint  ::= 'mu' | 'nu'\n"
"pvar-decl ::= pvar | pvar '(' pars ')'\n"
"pvar      ::= 'X' | 'Y' | 'Z'\n"
"\n"
"pbes-expr ::= boolvar | nat-expr '<' nat-expr | nat-expr '=' nat-expr\n"
"            | 'T' | 'F' | pvar-inst | '(' pbes-expr ')' | '!' pbes-expr\n"
"            | pbes-expr '&&' pbes-expr | pbes-expr '||' pbes-expr\n"
"\n"
"boolvar   ::= 'a' | 'b' | 'c'\n"
"natvar    ::= 'i' | 'j' | 'k'\n"				
"datavar   ::= boolvar | natvar\n"
"nat-expr  ::= '1' | natvar | nat-expr '+' nat-expr | '(' nat-expr ')'\n"
"pvar-inst ::= pvar '(' pars-inst ')'\n"
"pars      ::= datavar | pars ',' datavar\n"
"pars-inst ::= datavar | '!' boolvar\n"
"            | datavar ',' pars-inst | '!' boolvar ',' pars-inst\n"
"\n"
"EXAMPLES:\n"
"\n"
"  mu X(i) = (i<1) && X(i+1)\n"
"\n"
"  mu X(b,i)   = (Y(b, i, i+1+1) && b) || !b\n"
"  nu Y(b,i,j) = X(!b,i) && Y(b,i+1,j) && ((i+1) < j)\n";
}

//========================== 
t_tool_options parse_command_line(int ac, char **av)
//==========================
{
  interface_description clinterface(av[0], NAME, AUTHOR, "[OPTION]... [INFILE [OUTFILE]]\n"
    "Parse the textual description of a PBES from INFILE and write it to OUTFILE. "
    "If INFILE is not present, stdin is used. If OUTFILE is not present, stdout is used.\n\n"
    "A BNF syntax description of the current ad hoc parser can be displayed using the --syntax option.",
    "Currently the mCRL2 PBES parser that is accessible via the --new-parser option does not generate a valid PBES, because type-checking is not yet implemented."
  );

  clinterface.add_option("syntax", "display the BNF syntax description of the current ad hoc parser", 's');
  clinterface.add_option("new-parser", "use the mCRL2 PBES parser that adheres to the BNF syntax description at <http://mcrl2.org/wiki/index.php/PBES_syntax>", 'n');

  command_line_parser parser(clinterface, ac, av);

  t_tool_options tool_options;

  if (0 < parser.arguments.size()) {
    tool_options.infilename = parser.arguments[0];
  }
  if (1 < parser.arguments.size()) {
    tool_options.outfilename = parser.arguments[1];
  }
  if (2 < parser.arguments.size()) {
    parser.error("too many file arguments");
  }

  tool_options.print_syntax = 0 < parser.options.count("syntax");
  tool_options.new_parser   = 0 < parser.options.count("new-parser");

  return tool_options;
}



 
 
 
//MAIN =================================== 
int main(int argc, char** argv) 
{ 
  MCRL2_ATERM_INIT(argc, argv)
   
  try {
    t_tool_options tool_options = parse_command_line(argc, argv);

    if (tool_options.print_syntax) {
      print_syntax();
      return EXIT_SUCCESS;
    }
 
    if (tool_options.new_parser) {
      ATermAppl result = NULL;
      //parse specification
      if (tool_options.infilename == "") {
        //parse specification from stdin
        gsVerboseMsg("parsing input from stdin using the new parser...\n");
        result = parse_pbes_spec(cin);
      } else {
        //parse specification from infilename
        ifstream instream(tool_options.infilename.c_str(), ifstream::in|ifstream::binary);
        if (!instream.is_open()) {
          gsErrorMsg("cannot open input file '%s'\n", tool_options.infilename.c_str());
          return EXIT_FAILURE;
        }
        gsVerboseMsg("parsing input file '%s' using the new parser...\n", tool_options.infilename.c_str());
        result = parse_pbes_spec(instream);
        instream.close();
      }
      if (result == NULL) 
      {
        gsErrorMsg("parsing failed\n");
        return EXIT_FAILURE;
      }
      //type check the result
      gsVerboseMsg("type checking...\n");
      result = type_check_pbes_spec(result);
      if (result == NULL)
      {
        gsErrorMsg("type checking failed\n");
        return EXIT_FAILURE;
      }
      //implement standard data types and type constructors on the result
      gsVerboseMsg("implementing standard data types and type constructors...\n");
      result = implement_data_pbes_spec(result);
      if (result == NULL) 
      {
        gsErrorMsg("data implementation failed\n");
        return EXIT_FAILURE;
      }
      //store the result
      if (tool_options.outfilename == "") {
        gsVerboseMsg("saving result to stdout...\n");
        ATwriteToSAFFile((ATerm) result, stdout);
      } else {
        FILE *outstream = fopen(tool_options.outfilename.c_str(), "wb");
        if (outstream == NULL) {
          gsErrorMsg("cannot open output file '%s'\n", tool_options.outfilename.c_str());
          return EXIT_FAILURE;
        }
        gsVerboseMsg("saving result to '%s'...\n", tool_options.outfilename.c_str());
        ATwriteToSAFFile((ATerm) result, outstream);
        fclose(outstream);
      }
    } else {
      //!tool_options.new_parser
  
      cerr <<"Creating PBES ("<<tool_options.outfilename<< ") from text (" << tool_options.infilename <<") \n";
   
      //Create the pbes from the input text 
      pbes<> p = make_pbes(tool_options.infilename); 
      ATermAppl ap = p;
   
      if (tool_options.outfilename == "") {
        gsVerboseMsg("The resulting PBES is:\n");
        PrintPart_CXX(cout, (ATerm) ap, ppDefault);
        cout << endl;
      } else 
        if(!p.save(tool_options.outfilename, false)){
          gsErrorMsg("writing to %s failed\n",tool_options.outfilename.c_str());
          exit(1);
        }
      
      cerr <<"done\n";  
    }
    return EXIT_SUCCESS;
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_FAILURE;
} 
//======================================== 
 
 
 
 
