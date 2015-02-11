// Author(s): Muck van Weerdenburg; adapted by Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2i.cpp


#define TOOLNAME "mcrl2i"
#define AUTHORS "Muck van Weerdenburg; Jan Friso Groote"

#include <iostream>
#include <sstream>
#include <string>
#include <cctype>

#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/pbes/io.h"

using namespace std;
using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::utilities::tools;
using mcrl2::data::tools::rewriter_tool;


static bool check_whether_variable_string_is_in_use(
  const std::string& s,
  const std::set < variable > &varset)
{
  for (std::set < variable >::const_iterator i=varset.begin();
       i!=varset.end(); ++i)
  {
    if (i->name().function().name()==s)
    {
      return true;
    }
  }
  return false;
}

static string trim_spaces(const string& str)
{
  // function from http://www.codeproject.com/vcpp/stl/stdstringtrim.asp
  string s(str);
  string::size_type pos = s.find_last_not_of(' ');
  if (pos != string::npos)
  {
    s.erase(pos + 1);
    pos = s.find_first_not_of(' ');
    if (pos != string::npos)
    {
      s.erase(0, pos);
    }
  }
  else
  {
    s.erase(s.begin(), s.end());
  }

  return s;
}

static data_expression parse_term(const string& term_string,
                                  const data_specification& spec,
                                  std::set < variable > context_variables,
                                  const std::set < variable > &local_variables = std::set < variable >())
{
  context_variables.insert(local_variables.begin(),local_variables.end());
  return parse_data_expression(term_string,context_variables.begin(),context_variables.end(),spec);
}

static void declare_variables(
  const string& vars,
  std::set <variable> &context_variables,
  data_specification& spec)
{
  parse_variables(vars + ";",std::inserter(context_variables,context_variables.begin()),
                  context_variables.begin(), context_variables.end(),spec);
}

// Match beginning of string s with match. Return true
// iff it does. If a match is found, remove match from the beginning
// of s.
static bool match_and_remove(string& s, const string& match)
{
  if (s.substr(0,match.size())==match)
  {
    s=s.substr(match.size());
    return true;
  }
  return false;
}

static bool add_context_sorts(const std::set<sort_expression> &new_sorts,
                                    std::set<sort_expression> &context_sorts,
                                    data_specification &spec)
{
  // Check whether new rewrite rules are required, and if so, reinitialise the rewriter with them.
  bool reinitialise_rewriter=false;
  for(auto i=new_sorts.begin(); i!=new_sorts.end(); ++i)
  {
     if (context_sorts.insert(*i).second)
     {
       // The sort was not yet present in the context sorts
       reinitialise_rewriter=true;
       spec.add_context_sort(*i);
     }
  }
  return reinitialise_rewriter;
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
      std::set < variable > context_variables;
      std::set < sort_expression > context_sorts;
      bool need_to_rebuild_rewriter=true;

      data_specification spec;
      if (!input_filename().empty())
      {
        try
        {
          // Try to read a linear specification
          mcrl2::lps::specification p;
          load_lps(p, input_filename());
          spec = p.data();
          context_variables = p.global_variables();
        }
        catch (mcrl2::runtime_error)
        {
          try
          {
            // Try to read a pbes.
            mcrl2::pbes_system::pbes p;
            load_pbes(p, input_filename());
            spec=p.data();
            context_variables = p.global_variables();
          }
          catch (mcrl2::runtime_error& e)
          {
            std::cout << "Could not read " << input_filename() << " as an LPS or a PBES. " << e.what() <<
                      "\nUsing standard data types only;\n";
            spec=data_specification();
            context_variables.clear();
          }
        }
      }


      std::cout << "mCRL2 interpreter (type h for help)" << std::endl;

      rewriter rewr; 

      mutable_map_substitution < std::map < variable, data_expression > > assignments;

      bool done = false;
      while (!done)
      {
        try
        {
          string s;
          (cout << "? ").flush();
          getline(cin, s);
          if ((s.length() > 0) && (s[s.length()-1] == '\r'))
          {
            // remove CR
            s.resize(s.length()-1);
          }

          if (cin.eof())
          {
            cout << endl;
            done = true;
          }

          if (match_and_remove(s,"q") || match_and_remove(s,"quit"))
          {
            if (cin.eof())
            {
              cout << endl;
            }
            done = true;
          }
          else if (match_and_remove(s,"h") || match_and_remove(s,"help"))
          {
            cout << help_text;
          }
          else if (match_and_remove(s,"r ") || match_and_remove(s,"rewriter "))
          {
            rewriter::strategy new_strategy = parse_rewrite_strategy(s);
            if (new_strategy!=m_rewrite_strategy)
            {
              m_rewrite_strategy=new_strategy;
              need_to_rebuild_rewriter=true;
            }
          }
          else if (match_and_remove(s,"t ") || match_and_remove(s,"type "))
          {
            data_expression term = parse_term(s,spec,context_variables);
            cout << data::pp(term.sort()) << endl;
          }
          else if (match_and_remove(s,"v ") || match_and_remove(s,"var "))
          {
            declare_variables(s,context_variables,spec);
          }
          else if (match_and_remove(s,"e ") || match_and_remove(s,"eval "))
          {
            data_expression term = parse_term(s,spec,context_variables);
            std::set<sort_expression> all_sorts=find_sort_expressions(term);
            need_to_rebuild_rewriter=need_to_rebuild_rewriter||add_context_sorts(all_sorts,context_sorts,spec);
            if (need_to_rebuild_rewriter)
            {
              rewr=rewriter(spec,m_rewrite_strategy);
              need_to_rebuild_rewriter=false;
            }
            cout << data::pp(rewr(term,assignments)) << "\n";
          }
          else if (match_and_remove(s,"s ") || match_and_remove(s,"solve "))
          {
            std::set <variable> vars;
            string::size_type dotpos=s.find(".");
            if (dotpos==string::npos)
            {
              throw mcrl2::runtime_error("Expect a `.' in the input.");
            }
            parse_variables(s.substr(0,dotpos)+";",std::inserter(vars,vars.begin()),spec);
            std::set<sort_expression> all_sorts=find_sort_expressions(vars);
            data_expression term = parse_term(s.substr(dotpos+1),spec,context_variables,vars);
            if (term.sort()!=sort_bool::bool_())
            {
              throw mcrl2::runtime_error("expression is not of sort Bool.");
            }
            find_sort_expressions(term,std::inserter(all_sorts,all_sorts.end()));

            need_to_rebuild_rewriter=need_to_rebuild_rewriter||add_context_sorts(all_sorts,context_sorts,spec);
            if (need_to_rebuild_rewriter)
            {
              rewr=rewriter(spec,m_rewrite_strategy);
              need_to_rebuild_rewriter=false;
            }

            term=rewr(term);
            typedef enumerator_algorithm_with_iterator<rewriter> enumerator_type;
            enumerator_type enumerator(rewr, spec, rewr, 10000);

            data::mutable_indexed_substitution<> sigma;
            std::deque<enumerator_list_element_with_substitution<> >
                 enumerator_deque(1, enumerator_list_element_with_substitution<>(variable_list(vars.begin(), vars.end()), term));
            for (enumerator_type::iterator i = enumerator.begin(sigma, enumerator_deque); i != enumerator.end(); ++i)
            {
              i->add_assignments(vars,sigma,rewr);

              cout << "[";
              for (std::set< variable >::const_iterator v=vars.begin(); v!=vars.end() ; ++v)
              {
                if (v!=vars.begin())
                {
                  cout << ", ";
                }
             
                cout << data::pp(*v) << " := " << data::pp(sigma(*v));
              }
              cout << "] evaluates to "<< data::pp(rewr(term,sigma)) << "\n";
            }
          }
          else if (match_and_remove(s,"a ") || match_and_remove(s,"assign "))
          {
            string::size_type assign_pos = s.find("=");
            if (assign_pos==string::npos)
            {
              throw mcrl2::runtime_error("Missing symbol = in assignment.");
            }
            string varname=trim_spaces(s.substr(0,assign_pos)).c_str();

            if (check_whether_variable_string_is_in_use(varname,context_variables))
            {
              throw mcrl2::runtime_error("Variable " + varname + " already in use.");
            }
            s = s.substr(assign_pos+1);
            data_expression term = parse_term(s,spec,context_variables);
            variable var(varname,term.sort());

            std::set<sort_expression> all_sorts=find_sort_expressions(term);
            need_to_rebuild_rewriter=need_to_rebuild_rewriter||add_context_sorts(all_sorts,context_sorts,spec);
            if (need_to_rebuild_rewriter)
            {
              rewr=rewriter(spec,m_rewrite_strategy);
              need_to_rebuild_rewriter=false;
            }
            term = rewr(term,assignments);
            cout << data::pp(term) << "\n";
            assignments[var]=term;
            context_variables.insert(var);
          }
          else
          {
            throw mcrl2::runtime_error("unknown command (try 'h' for help).");
          }
        }
        catch (mcrl2::runtime_error& e)  // Catch errors in the input.
        {
          cout << e.what() << endl;
        }
      }

      return true;
    }
};

int main(int argc, char** argv)
{
  return mcrl2i_tool().execute(argc, argv);
}

