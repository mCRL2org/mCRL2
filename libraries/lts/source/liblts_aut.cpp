// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_aut.cpp

#include <cstring>
#include <string>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <assert.h>
#include <aterm2.h>
#include "mcrl2/lts/lts_io.h"
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

void read_from_aut(lts &l, string const& filename)
{
  ifstream is(filename.c_str());

  if ( !is.is_open() )
  {
    throw mcrl2::runtime_error("cannot open AUT file '" + filename + "' for reading.");
  }

  read_from_aut(l,is);
  is.close();
}

static void read_aut_header(char *s, char **initial_state, char **transitions, char **states)
{
  while ( *s == ' ' )
    s++;

  if ( strncmp(s,"des",3) )
  {
    throw mcrl2::runtime_error("Expext a .aut file to start with 'des'.");
  }  
  s += 3;

  while ( *s == ' ' )
    s++;

  if ( *s != '(' )
  {
    throw mcrl2::runtime_error("Expect an opening bracket '(' after 'des' in the first line of a .aut file.");
  }  
  s++;

  while ( *s == ' ' )
    s++;

  *initial_state = s;
  while ( (*s >= '0') && ( *s <= '9') )
    s++;
  char *end_initial_state = s;

  while ( *s == ' ' )
    s++;

  if ( *s != ',' )
  {
    throw mcrl2::runtime_error("Expect a comma after the first number in the first line of a .aut file.");
  }  
  s++;

  while ( *s == ' ' )
    s++;

  *transitions = s;
  while ( (*s >= '0') && ( *s <= '9') )
    s++;
  char *end_transitions = s;

  while ( *s == ' ' )
    s++;

  if ( *s != ',' )
  {
    throw mcrl2::runtime_error("Expect a comma after the second number in the first line of a .aut file.");
  }  
  s++;

  while ( *s == ' ' )
    s++;

  *states = s;
  while ( (*s >= '0') && ( *s <= '9') )
    s++;
  char *end_states = s;

  while ( *s == ' ' )
    s++;

  if ( *s != ')' )
  {
    throw mcrl2::runtime_error("Expect a closing bracket ')' after the third number in the first line of a .aut file.");
  }  
  s++;

  while ( *s == ' ' )
    s++;

  if ( *s == '\r' )
    s++;

  if ( *s != '\0' )
  {
    throw mcrl2::runtime_error("Expect a newline after the header des(...,...,...) in a .aut file.");
  }  

  *end_initial_state = '\0';
  *end_transitions = '\0';
  *end_states = '\0';
}

static void read_aut_transition(char *s, char **from, char **label, char **to, const unsigned int lineno)
{
  while ( *s == ' ' )
    s++;

  if ( *s != '(' )
  { throw mcrl2::runtime_error("Expect opening bracket at line " + c(lineno) + ".");
  }  
  s++;

  while ( *s == ' ' )
    s++;

  *from = s;
  while ( (*s >= '0') && ( *s <= '9') )
    s++;
  char *end_from = s;

  while ( *s == ' ' )
    s++;

  if ( *s != ',' )
  { throw mcrl2::runtime_error("Expect that the first number is followed by a comma at line " + c(lineno) + ".");
  }  
  s++;

  while ( *s == ' ' )
    s++;

  char *end_label;
  if ( *s == '"' )
  {
    s++;
    *label = s;
    while ( (*s != '"') && ( *s != '\0') )
      s++;
    end_label = s;
    if ( *s != '"' )
    { throw mcrl2::runtime_error("Expect that the second item is a quoted label (using \") at line " + c(lineno) + ".");
    }
    s++;
  } 
  else 
  {
    *label = s;
    while ( (*s != ',') && ( *s != '\0') )
      s++;
    end_label = s;
  }

  while ( *s == ' ' )
    s++;

  if ( *s != ',' )
  { throw mcrl2::runtime_error("Expect a comma after the quoted label at line " + c(lineno) + ".");
  }  
  s++;

  while ( *s == ' ' )
    s++;

  *to = s;
  while ( (*s >= '0') && ( *s <= '9') )
    s++;
  char *end_to = s;

  while ( *s == ' ' )
    s++;

  if ( *s != ')' )
  { throw mcrl2::runtime_error("Expect a closing bracket at the end of the transition at line " + c(lineno) + ".");
  }  
  s++;

  while ( *s == ' ' )
    s++;

  if ( *s == '\r' )
    s++;

  if ( *s != '\0' )
  { throw mcrl2::runtime_error("Expect a newline after the transition at line " + c(lineno) + ".");
  }  

  *end_from = '\0';
  *end_label = '\0';
  *end_to = '\0';

}

void read_from_aut(lts &l, istream &is)
{
  unsigned int ntrans,nstate;
  #define READ_FROM_AUT_BUF_SIZE 8196
  char buf[READ_FROM_AUT_BUF_SIZE];
  char *s1,*s2,*s3;
  unsigned int line_no = 1;

  is.getline(buf,READ_FROM_AUT_BUF_SIZE);
  read_aut_header(buf,&s1,&s2,&s3);
  ntrans = strtoul(s2,NULL,10);
  nstate = strtoul(s3,NULL,10);

  l.set_num_states(nstate,false);
  l.set_initial_state(strtoul(s1,NULL,10));
  assert(nstate == l.num_states());

  ATermIndexedSet labs = ATindexedSetCreate(100,50);
  while ( !is.eof() )
  {
    unsigned int from,to;
    const char *s;

    is.getline(buf,READ_FROM_AUT_BUF_SIZE);
    line_no++;
    if ( is.gcount() == 0 )
    {
      break;
    }
    read_aut_transition((char *)buf,&s1,&s2,&s3,line_no);
    
    from = strtoul(s1,NULL,10);
    s = s2;
    to = strtoul(s3,NULL,10);
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
    ATerm t = (ATerm) ATmakeAppl(ATmakeAFun(s,0,ATtrue));
    if ( (label =  ATindexedSetGetIndex(labs,t)) < 0 )
    {
      ATbool b;
      label = ATindexedSetPut(labs,t,&b);
      l.add_label(t,!strcmp(s,"tau"));
    }

    l.add_transition(transition(from,(unsigned int) label,to));
  }
  ATtableDestroy(labs);
  if ( ntrans != l.num_transitions() )
  {
    throw mcrl2::runtime_error("number of transitions read (" + c(l.num_transitions()) + 
              ") does not correspond to the number of transition given in the header (" + c(ntrans) + ").");
  }

  l.set_type(lts_aut);

}

void write_to_aut(const lts &l, string const& filename)
{
  ofstream os(filename.c_str());

  if ( !os.is_open() )
  {
    throw mcrl2::runtime_error("cannot open AUT file '" + filename + "' for writing.");
    return;
  }

  write_to_aut(l,os);
  os.close();
}

void write_to_aut(const lts &l, ostream &os)
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
       << l.label_value_str(t.front().label())
       << "\"," << to << ")" << endl;
  }

}

}
}
}
