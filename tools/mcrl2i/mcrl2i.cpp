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

#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/substitution.h"
#include "mcrl2/data/classic_enumerator.h"
#include "mcrl2/data/enumerator_factory.h"
#include "mcrl2/data/map_substitution_adapter.h"
#include "mcrl2/data/parser.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/detail/aterm_io.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/pbes/pbes.h"


using namespace std;
using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::data;
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

static data_expression parse_term(const string &term_string,
                                  const data_specification &spec,
                                  atermpp::set < variable > context_variables,
                                  const atermpp::set < variable > &local_variables = atermpp::set < variable >())
{ context_variables.insert(local_variables.begin(),local_variables.end());
  return parse_data_expression(term_string,context_variables.begin(),context_variables.end(),spec);
}

static void declare_variables(
                   const string &vars,
                   atermpp::set <variable> &context_variables,
                   data_specification &spec)
{ parse_variables(vars + ";",std::inserter(context_variables,context_variables.begin()),
                              context_variables.begin(), context_variables.end(),spec);
}

// Match beginning of string s with match. Return true
// iff it does. If a match is found, remove match from the beginning
// of s.
static bool match_and_remove(string &s, const string &match)
{ if (s.substr(0,match.size())==match)
  { s=s.substr(match.size());
    return true;
  }
  return false;
}

static const std::string help_text=
          "The following commands are available to manipulate mcrl2 data expressions. "
          "Essentially, there are commands to rewrite and type expressions, as well as generating "
          "the solutions for a boolean expression. The expressions can contain assigned or "
          "unassigned variables. Note that there are no bounds on the number of steps to evaluate "
          "or solve an expression, nor is the number of solutions bounded. Hence, the assign, eval "
          "solve commands can give rise to infinite loops.\n"
          "  h[elp]                         print this help message.\n"
          "  q[uit]                         quit.\n"
          "  t[ype] EXPRESSION              print type of EXPRESSION.\n"
          "  a[ssign] VAR=EXPRESSION        evaluate the expression and assign it to the variable.\n"
          "  e[val] EXPRESSION              rewrite EXPRESSION and print result.\n"
          "  v[ar] VARLIST                  declare variables in VARLIST.\n"
          "  r[ewriter] STRATEGY            use STRATEGY for rewriting.\n"
          "  s[solve] VARLIST. EXPRESSION   give all valuations of the variables in\n"
          "                                      VARLIST that satisfy EXPRESSION.\n"
          "VARLIST is of the form x,y,...: S; ... v,w,...: T.\n";


class mcrl2i_tool: public rewriter_tool<input_tool>
{
  public:
    /// Constructor.
    mcrl2i_tool()
      : rewriter_tool<input_tool>(
          TOOLNAME,
          AUTHORS,
          "Interpreter for the mCRL2 data language",
          "Evaluate mCRL2 data expressions via a text-based interface. "
          "If INFILE is present and if it contains an LPS or PBES, the data types of this specification may be used. "
          "If no input file is given, only the standard numeric datatypes are available. Stdin is ignored."
          + help_text
        )
    {}

    /// Runs the algorithm.
    bool run()
    {
      data_specification spec;
      if (!input_filename().empty())
      { try
        { // Try to read a linear specification
          mcrl2::lps::specification p;
          p.load(input_filename());
          spec=p.data();
        }
        catch (mcrl2::runtime_error e)
        { try
          { // Try to read a pbes.
            mcrl2::pbes_system::pbes <> p;
            p.load(input_filename());
            spec=p.data();
          }
          catch (mcrl2::runtime_error &e)
          { std::cout << "Could not read " << input_filename() << " as an LPS or a PBES. " << e.what() <<
                    "\nUsing standard data types only;\n";
            spec=data_specification();
          }
        }
      }

      // Import all standard data types should be available even if they are
      // not port of the loaded lps or pbes.
      spec.import_system_defined_sort(sort_real::real_());

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
            try
            { rewriter::strategy new_strategy = boost::lexical_cast< rewriter::strategy >(s);
              if (new_strategy!=m_rewrite_strategy)
              { m_rewrite_strategy=new_strategy;
                rewr=rewriter(spec,m_rewrite_strategy);
              }
            }
            catch (boost::bad_lexical_cast &e)
            { throw mcrl2::runtime_error("The string " + s + " does not describe a rewrite strategy.");
            }
          }
          else if (match_and_remove(s,"t ") || match_and_remove(s,"type "))
          { data_expression term = parse_term(s,spec,context_variables);
            cout << pp(term.sort()) << endl;
          }
          else if (match_and_remove(s,"v ") || match_and_remove(s,"var "))
          { declare_variables(s,context_variables,spec);
          }
          else if (match_and_remove(s,"e ") || match_and_remove(s,"eval "))
          { data_expression term = parse_term(s,spec,context_variables);
            cout << pp(rewr(term,make_map_substitution_adapter(assignments))) << "\n";
          }
          else if (match_and_remove(s,"s ") || match_and_remove(s,"solve "))
          {
            atermpp::set <variable> vars;
            string::size_type dotpos=s.find(".");
            if (dotpos==string::npos)
            { throw mcrl2::runtime_error("Expect a `.' in the input.");
            }
            parse_variables(s.substr(0,dotpos)+";",std::inserter(vars,vars.begin()),spec);
            data_expression term = parse_term(s.substr(dotpos+1),spec,context_variables,vars);
            if ( term.sort()!=sort_bool::bool_())
            { throw mcrl2::runtime_error("expression is not of sort Bool.");
            }
            for (classic_enumerator< > i =
                 e.make(data::convert < std::set <variable > >(vars),rewr,term);
                                                          i != classic_enumerator<>() ; ++i)
            {
              cout << "[";
              for ( atermpp::set< variable >::const_iterator v=vars.begin(); v!=vars.end() ; ++v )
              { cout << pp(*v) << " := " << pp((*i)(*v));
                if ( boost::next(v)!=vars.end() )
                { cout << ", ";
                }
              }
              cout << "] evaluates to "<< pp(rewr(term,*i)) << "\n";
            }
          }
          else if (match_and_remove(s,"a ") || match_and_remove(s,"assign "))
          {
            string::size_type assign_pos = s.find("=");
            if (assign_pos==string::npos)
            { throw mcrl2::runtime_error("Missing symbol = in assignment.");
            }
            string varname=trim_spaces(s.substr(0,assign_pos)).c_str();

            if (check_whether_variable_string_is_in_use(varname,context_variables))
            { throw mcrl2::runtime_error("Variable " + varname + " already in use.");
            }
            s = s.substr(assign_pos+1);
            data_expression term = parse_term(s,spec,context_variables);
            variable var(varname,term.sort());
            term = rewr(term,make_map_substitution_adapter(assignments));
            cout << pp(term) << "\n";
            assignments[var]=term;
            context_variables.insert(var);
          }
          else
          { throw mcrl2::runtime_error("unknown command (try 'h' for help).");
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
  MCRL2_ATERMPP_INIT(argc, argv)
  return mcrl2i_tool().execute(argc, argv);
}

