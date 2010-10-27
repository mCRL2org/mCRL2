// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_aut.cpp

// #include <cstring>
#include <string>
// #include <cstdlib>
#include <sstream>
#include <assert.h>
#include <aterm2.h>
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/lts_aut.h"
#include "mcrl2/core/messaging.h"

static std::string c(const unsigned int n)  // Awkward trick, should be a better way to do this. Please replace....
{ std::stringstream out;
  out << n;
  return out.str();
}

using namespace mcrl2::core;
using namespace std;

namespace mcrl2
{
namespace lts
{
namespace detail
{

static void read_newline(istream &is,const unsigned int lineno=1)
{ 
  char ch;
  is.get(ch);

  // Skip over spaces
  while ( ch == ' ' )
  { 
    is.get(ch);  
  }

  // Windows systems typically have a carriage return before a newline.
  if ( ch == '\r' )
  {  
    is.get(ch);
  }

  if ( ch != '\n' )
  {
    if (lineno==1)
    {
      throw mcrl2::runtime_error("Expect a newline after the header des(...,...,...).");
    }
    else
    {
      throw mcrl2::runtime_error("Expect a newline after the transition at line " + c(lineno) + ".");
    }
  }
}

static void read_aut_header(
              istream &is, 
              unsigned int &initial_state, 
              unsigned int &num_transitions, 
              unsigned int &num_states)
{
  string s;
  is.width(3);
  is >> skipws >> s;

  if (s!="des")
  {
    throw mcrl2::runtime_error("Expect an .aut file to start with 'des'.");
  }  

  char ch;
  is >> skipws >> ch;
  
  if (ch != '(')
  {
    throw mcrl2::runtime_error("Expect an opening bracket '(' after 'des' in the first line of a .aut file.");
  }  

  is >> skipws >> initial_state;

  is >> skipws >> ch;
  if (ch != ',')
  {
    throw mcrl2::runtime_error("Expect a comma after the first number in the first line of a .aut file.");
  }  

  is >> skipws >> num_transitions;

  is >> skipws >> ch;
  if (ch != ',')
  {
    throw mcrl2::runtime_error("Expect a comma after the second number in the first line of a .aut file.");
  }  

  is >> skipws >> num_states;

  is >> ch;

  if (ch != ')')
  {
    throw mcrl2::runtime_error("Expect a closing bracket ')' after the third number in the first line of a .aut file.");
  }  

  read_newline(is);
}

static bool read_aut_transition(
              istream &is, 
              unsigned int &from, 
              string &label, 
              unsigned int &to,
              const unsigned int lineno)
{
  char ch;
  is >> skipws >> ch;
  if (is.eof())
  { return false;
  }
  if (ch != '(')
  { throw mcrl2::runtime_error("Expect opening bracket at line " + c(lineno) + ".");
  }  

  is >> skipws >> from;

  is >> skipws >> ch;
  if (ch != ',' )
  { throw mcrl2::runtime_error("Expect that the first number is followed by a comma at line " + c(lineno) + ".");
  }  

  is >> skipws >> ch;
  if (ch == '"')
  {
    label="";
    is >> ch;
    while ( (ch != '"') && !is.eof())
    { label.push_back(ch);
      is >> ch;
    }

    if ( ch != '"' )
    { throw mcrl2::runtime_error("Expect that the second item is a quoted label (using \") at line " + c(lineno) + ".");
    }
    is >> skipws >> ch;
  } 
  else 
  {
    label = ch;
    is >> ch;
    while ( (ch != ',') && !is.eof())
    { label.push_back(ch);
      is >> ch;
    }
  }

  if ( ch != ',' )
  { throw mcrl2::runtime_error("Expect a comma after the quoted label at line " + c(lineno) + ".");
  }  

  is >> skipws >> to;

  is >> ch;
  if ( ch != ')' )
  { throw mcrl2::runtime_error("Expect a closing bracket at the end of the transition at line " + c(lineno) + ".");
  }  

  read_newline(is,lineno);
  return true;
}

static void read_from_aut(lts_aut_t &l, istream &is)
{
  unsigned int line_no = 1;
  unsigned int initial_state=0, ntrans=0, nstate=0;

  read_aut_header(is,initial_state,ntrans,nstate);

  l.set_num_states(nstate,false);

  if (initial_state>= l.num_states() )
  {
    throw mcrl2::runtime_error("cannot parse AUT input (initial state index (" + c(initial_state) +
              ") is larger or equal to the number of states (" +
                  c(l.num_states()) + ") given in the header).");
  }

  l.set_initial_state(initial_state); 

  ATermIndexedSet labs = ATindexedSetCreate(100,50);
  while ( !is.eof() )
  {
    unsigned int from,to;
    string s;

    line_no++;

    if (!read_aut_transition(is,from,s,to,line_no))
    { break; // eof encountered
    }
    
    if ( from >= l.num_states() )
    {
      ATtableDestroy(labs);
      throw mcrl2::runtime_error("cannot parse AUT input (invalid transition at line " + 
              c(line_no) + "; state index (" + c(from) + ") higher than maximum (" +
                  c(l.num_states()) + ") given by header).");
    }
    if ( to >= l.num_states() )
    {
      ATtableDestroy(labs);
      throw mcrl2::runtime_error("cannot parse AUT input (invalid transition at line " + c(line_no) + 
              "; state index (" + c(to) + ") higher than maximum (" + c(l.num_states()) + ") given by header).");
    }

    int label;
    ATerm t = (ATerm) ATmakeAppl(ATmakeAFun(s.c_str(),0,ATtrue));
    if ( (label =  ATindexedSetGetIndex(labs,t)) < 0 )
    {
      ATbool b;
      label = ATindexedSetPut(labs,t,&b);
      l.add_label(string(ATwriteToString(t)),s=="tau");
    }

    l.add_transition(transition(from,(unsigned int) label,to));
  }
  ATtableDestroy(labs);
  if ( ntrans != l.num_transitions() )
  {
    throw mcrl2::runtime_error("number of transitions read (" + c(l.num_transitions()) + 
              ") does not correspond to the number of transition given in the header (" + c(ntrans) + ").");
  }
}

void lts_aut_t::load(const string &filename)
{
  if (filename=="")
  {
    read_from_aut(*this,cin);
  }
  else
  {
    ifstream is(filename.c_str());

    if ( !is.is_open() )
    {
      throw mcrl2::runtime_error("cannot open AUT file '" + filename + "' for reading.");
    }

    read_from_aut(*this,is);
    is.close();
  }
} 

void lts_aut_t::load(istream &is)
{
  read_from_aut(*this,is);
} 

static void write_to_aut(const lts_aut_t &l, ostream &os)
{
  os << "des (0," << l.num_transitions() << "," << l.num_states() << ")" << endl;

  for (transition_const_range t=l.get_transitions();  !t.empty(); t.advance_begin(1))
  {
    transition::size_type from = t.front().from();
    transition::size_type to = t.front().to();
    // AUT files need the initial state to be 0, so we will swap state 0 and
    // the initial state
    if ( from == 0 )
    {
      from = l.initial_state();
    } else if ( from == l.initial_state() )
    {
      from = 0;
    }
    if ( to == 0 )
    {
      to = l.initial_state();
    } else if ( to == l.initial_state() )
    {
      to = 0;
    }
    os << "(" << from << ",\""
       << detail::pp(l.label_value(t.front().label()))
       << "\"," << to << ")" << endl;
  }

}

void lts_aut_t::save(string const& filename) const
{
  if (filename=="")
  { 
    write_to_aut(*this,cout);
  }
  else
  {
    ofstream os(filename.c_str());
  
    if ( !os.is_open() )
    {
      throw mcrl2::runtime_error("cannot open AUT file '" + filename + "' for writing.");
      return;
    }

    write_to_aut(*this,os);
    os.close();
  }
} 



}
}
}
