// Author(s): Muck van Weerdenburg; adapted by Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2i.cpp


#include "boost.hpp" // precompiled headers

#define TOOLNAME "mcrl2i"
#define AUTHORS "Muck van Weerdenburg; Jan Friso Groote"

#include <iostream>
#include <sstream>
#include <string>
#include <cctype>

#include "boost/utility.hpp"
#include "boost/lexical_cast.hpp"

#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include "mcrl2/new_data/rewriter.h"
#include "mcrl2/new_data/substitution.h"
#include "mcrl2/new_data/classic_enumerator.h"
#include "mcrl2/new_data/enumerator_factory.h"
#include "mcrl2/new_data/map_substitution_adapter.h"
#include "mcrl2/new_data/parser.h"
#include "mcrl2/new_data/detail/data_specification_compatibility.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/detail/aterm_io.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/new_data/data_specification.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/pbes/pbes.h"


using namespace std;
using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::new_data;
using mcrl2::core::gsDebug;
using namespace mcrl2::utilities::tools;
using mcrl2::utilities::tools::rewriter_tool;


static bool check_whether_variable_string_is_in_use(
                   const std::string &s,
                   const atermpp::set < variable > &varset)
{ for(atermpp::set < variable >::const_iterator i=varset.begin();
          i!=varset.end(); ++i)
  { if (i->name()==s)
    { return true;
    }
  }
  return false;
}

static string trim_spaces(const string &str)
{
  // function from http://www.codeproject.com/vcpp/stl/stdstringtrim.asp
  string s(str);
  string::size_type pos = s.find_last_not_of(' ');
  if(pos != string::npos)
  {
    s.erase(pos + 1);
    pos = s.find_first_not_of(' ');
    if (pos != string::npos)
      s.erase(0, pos);
  } else
    s.erase(s.begin(), s.end());

  return s;
}

/// Read the variable declaration in the string decl, and add the variable to varset.
/// This function throws a mcrl2::runtime error if parsing of a variable declaration fails.
static void parse_var_decl(string decl, 
                           atermpp::set < variable > &varset, 
                           data_specification &spec)
{
    string::size_type semi_pos = decl.find(':');
    if ( semi_pos == string::npos )
    { throw mcrl2::runtime_error("invalid type declaration:" + decl);
    }

    stringstream ss(decl.substr(semi_pos+1));

    sort_expression sort(type_check_sort_expr(parse_sort_expr(ss),
                              new_data::detail::data_specification_to_aterm_data_spec(spec)));

    if (gsDebug)
    { cerr << "sort parsed and type checked: " << pp(sort) << "\n";
    }

    string names = decl.substr(0,semi_pos) + ',';
    string::size_type i;
    while ( ( i = names.find(',') ) != string::npos )
    {
      varset.insert(variable(trim_spaces(names.substr(0,i)), sort));
      names = names.substr(i+1);
    }
}

static void parse_var_decl_list(
                           string decl_list, 
                           atermpp::set < variable > &varset, 
                           data_specification &spec)
{
  decl_list += ';';
  string::size_type pos;
  while ( (pos = decl_list.find(';')) != string::npos )
  {
    string decl(decl_list.substr(0,pos));
    decl_list = decl_list.substr(pos+1);
    parse_var_decl(decl,varset,spec);
  }
}

static atermpp::set < variable > 
              parse_varlist_from_string(string &s, data_specification &spec)
{
  atermpp::set < variable > variable_set;
  string::size_type start = s.find('<');
  if ( start == string::npos )
  { throw mcrl2::runtime_error("opening bracket for variable list not found");
  }
  string::size_type end = s.find('>',start);
  if ( end == string::npos )
  { throw mcrl2::runtime_error("closing bracket for variable list not found");
  }
  string varlist = s.substr(start+1,end-start-1);
  s = s.substr(end+1);

  parse_var_decl_list(varlist,variable_set,spec) ;

  return variable_set;
}

static data_expression parse_term(string &term_string, 
                                  const data_specification &spec, 
                                  atermpp::set < variable > context_variables,
                                  const atermpp::set < variable > &local_variables = atermpp::set < variable >())
{
  context_variables.insert(local_variables.begin(),local_variables.end());
//  return type_check_data_expr(parse_data_expression(term_string),NULL,new_data::detail::data_specification_to_aterm_data_spec(spec),context_variables);
//  variable context is ignored, current objective is to get the tool to compile
  return parse_data_expression(term_string);
}

static void declare_variables(
                   string &vars, 
                   atermpp::set <variable> &context_variables, 
                   data_specification &spec)
{
  atermpp::set <variable >  varset; 
  parse_var_decl_list(vars,varset,spec);

  context_variables.insert(varset.begin(),varset.end());
}

// Match beginning of string s with match. Return true
// iff it does. If a match is found, remove match from the beginning
// of s.
static bool match_and_remove(string &s, const string &match)
{ if (s.substr(0,match.size())==match) 
  { s.substr(match.size());
    return true;
  }
  return false;
}

static const std::string help_text=      
          "At the prompt any mCRL2 data expression can be given. This term will be "
          "rewritten to normal form and printed. Also, one can assign values to "
          "variables. These variables can then be used in expressions. The prompt accepts "
          "the following commands (where VARLIST is of the form x,y,...: S; ... v,w,...: T):\n"
          "  h[elp]                         print this help message\n"
          "  q[uit]                         quit\n"
          "  t[ype] EXPRESSION              print type of EXPRESSION\n"
          "  a[ssign] VAR=EXPRESSION        assign the value of the expression to the variable\n"
          "  v[ar] VARLIST                  declare variables in VARLIST\n"
          "  r[ewriter] STRATEGY            use STRATEGY for rewriting\n"
          "  s[solve] <VARLIST> EXPRESSION  give all valuations of the variables in\n"
          "                                      VARLIST that satisfy EXPRESSION\n";


class mcrl2i_tool: public rewriter_tool<input_tool> 
{
  protected:
    typedef rewriter_tool<input_tool> super;

  /// Parse the non-default options.
    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
    }

  public:
    /// Constructor.
    mcrl2i_tool()
      : super(
          TOOLNAME,
          AUTHORS,
          "interpreter for the mCRL2 data language",
          "Evaluate mCRL2 data expressions via a text-based interface. "
          "If INFILE is present and if it contains an LPS or PBES, the data types of this specification may be used." 
          + help_text
        )
    {}

    /// Runs the algorithm.
    bool run()
    {
      if (core::gsVerbose)
      {
        std::cerr << "parameters of mcrl2i:" << std::endl;
        std::cerr << "  input file:         " << m_input_filename << std::endl;
        std::cerr << "  rewrite strategy:   " << m_rewrite_strategy << std::endl;
      }

      data_specification spec; 
      try 
      { // Try to read a linear specification
        mcrl2::lps::specification p;
        p.load(m_input_filename);
        spec=p.data();
      }
      catch (mcrl2::runtime_error e)
      { try 
        { // Try to read a pbes.
          mcrl2::pbes_system::pbes <> p;
          p.load(m_input_filename);
          spec=p.data();
        }
        catch (mcrl2::runtime_error &e)
        { std::cout << "Could not read " << m_input_filename << " as an LPS or a PBES. " << e.what() <<
                  "\nUsing standard data types only;\n";
          spec=data_specification();
        }
      }
      
      std::cout << "mCRL2 interpreter (type h for help)" << std::endl;

      rewriter rewr(spec,m_rewrite_strategy);
      enumerator_factory < classic_enumerator<> > e(spec,rewr); 
      atermpp::set < variable > context_variables;
      atermpp::map < variable, data_expression > assignments;

      bool done = false;
      while ( !done )
      {
        try
        {
          string s;
          (cout << "? ").flush();
          getline(cin, s);
          if ( (s.length() > 0) && (s[s.length()-1] == '\r') )
          { // remove CR
            s.resize(s.length()-1);
          }

          if (cin.eof())
          { cout << endl;
            done = true;
          }

          if (match_and_remove(s,"q") || match_and_remove(s,"quit"))
          { if ( cin.eof() ) cout << endl;
            done = true;
          } 
          else if (match_and_remove(s,"h") || match_and_remove(s,"help"))
          { cout << help_text;
          } 
          else if (match_and_remove(s,"r ") || match_and_remove(s,"rewriter "))
          { 
            rewriter::strategy new_strategy = boost::lexical_cast< rewriter::strategy >(s);
            if (new_strategy!=m_rewrite_strategy)
            { m_rewrite_strategy=new_strategy;
              rewr=rewriter(spec,m_rewrite_strategy);
            }
          } 
          else if (match_and_remove(s,"t ") || match_and_remove(s,"type "))
          { data_expression term = parse_term(s,spec,context_variables);
            cout << pp(term.sort()) << endl; 
          }
          else if (match_and_remove(s,"v ") || match_and_remove(s,"var "))
          { declare_variables(s,context_variables,spec);
          } 
          else if (match_and_remove(s,"s ") || match_and_remove(s,"solve "))
          {
            atermpp::set <variable> vars = parse_varlist_from_string(s,spec);
            data_expression term = parse_term(s,spec,context_variables,vars);
            if ( term.sort()!=sort_bool_::bool_())
            { throw mcrl2::runtime_error("expression is not of sort Bool\n");
            }
            // static EnumeratorSolutions *sols = NULL;
            // clear_rewr_substs(vars);
            for (classic_enumerator< > i =
                 e.make(new_data::convert < std::set <variable > >(vars),rewr,term);
                                                          i != classic_enumerator<>() ; ++i) 
            { 
              cout << "[";
              for ( atermpp::set< variable >::const_iterator v=vars.begin(); v!=vars.end() ; ++v ) 
              { cout << pp(*v) << " := " << (*i)(*v); 
                if ( boost::next(v)!=vars.end() )
                { cout << ", ";
                }
              }
              cout << "] gives "<< rewr(term,*i) << "]\n";
            }
          }
          else if (match_and_remove(s,"a ") || match_and_remove(s,"assign "))
          {
            string::size_type assign_pos = s.find("=");
            if (assign_pos==string::npos)
            { throw mcrl2::runtime_error("Missing symbol = in assignment");
            }
            string varname=trim_spaces(s.substr(0,assign_pos)).c_str();

            if (check_whether_variable_string_is_in_use(varname,context_variables))
            { throw mcrl2::runtime_error("Variable " + varname + " already in use");
            }
            s = s.substr(assign_pos+1);
            data_expression term = parse_term(s,spec,context_variables);
            variable var(varname,term.sort());
            term = rewr(term,make_map_substitution_adapter(assignments));
            assignments[var]=term;
            context_variables.insert(var);
          }
          else 
          { throw mcrl2::runtime_error("unknown command (try ':h' for help)");
          }
        }
        catch (mcrl2::runtime_error &e)  // Catch errors in the input.
        { cout << e.what() << endl;
        }
      }

      return true;
    }
};

int main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv)
  return mcrl2i_tool().execute(argc, argv);
}

