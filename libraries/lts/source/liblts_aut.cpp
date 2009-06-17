// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_aut.cpp

#include <string>
#include <cstdlib>
#include <fstream>
#include <assert.h>
#include <aterm2.h>
#include "mcrl2/lts/lts.h"
#include "mcrl2/core/messaging.h"

using namespace mcrl2::core;
using namespace std;

namespace mcrl2
{
namespace lts
{

bool p_lts::read_from_aut(string const& filename)
{
  ifstream is(filename.c_str());

  if ( !is.is_open() )
  {
    gsVerboseMsg("cannot open AUT file '%s' for reading\n",filename.c_str());
    return false;
  }

  bool r = read_from_aut(is);

  is.close();

  return r;
}

static bool read_aut_header(char *s, char **initial_state, char **transitions, char **states)
{
  while ( *s == ' ' )
    s++;

  if ( strncmp(s,"des",3) )
    return false;
  s += 3;

  while ( *s == ' ' )
    s++;

  if ( *s != '(' )
    return false;
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
    return false;
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
    return false;
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
    return false;
  s++;

  while ( *s == ' ' )
    s++;

  if ( *s == '\r' )
    s++;

  if ( *s != '\0' )
    return false;

  *end_initial_state = '\0';
  *end_transitions = '\0';
  *end_states = '\0';

  return true;
}

static bool read_aut_transition(char *s, char **from, char **label, char **to)
{
  while ( *s == ' ' )
    s++;

  if ( *s != '(' )
    return false;
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
    return false;
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
      return false;
    s++;
  } else {
    *label = s;
    while ( (*s != ',') && ( *s != '\0') )
      s++;
    end_label = s;
  }

  while ( *s == ' ' )
    s++;

  if ( *s != ',' )
    return false;
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
    return false;
  s++;

  while ( *s == ' ' )
    s++;

  if ( *s == '\r' )
    s++;

  if ( *s != '\0' )
    return false;

  *end_from = '\0';
  *end_label = '\0';
  *end_to = '\0';

  return true;
}

bool p_lts::read_from_aut(istream &is)
{
  unsigned int ntrans,nstate;
  #define READ_FROM_AUT_BUF_SIZE 8196
  char buf[READ_FROM_AUT_BUF_SIZE];
  char *s1,*s2,*s3;
  unsigned int line_no = 1;

  is.getline(buf,READ_FROM_AUT_BUF_SIZE);
  if ( read_aut_header(buf,&s1,&s2,&s3) )
  {
    init_state = strtoul(s1,NULL,10);
    ntrans = strtoul(s2,NULL,10);
    nstate = strtoul(s3,NULL,10);
  } else {
    gsErrorMsg("cannot parse AUT input (invalid header)\n");
    return false;
  }

  for (unsigned int i=0; i<nstate; i++)
  {
    p_add_state();
  }
  assert(nstate == nstates);

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
    if ( read_aut_transition((char *)buf,&s1,&s2,&s3) )
    {
      from = strtoul(s1,NULL,10);
      s = s2;
      to = strtoul(s3,NULL,10);
      if ( from >= nstates )
      {
        gsErrorMsg("cannot parse AUT input (invalid transition at line %d; state index (%u) higher than maximum (%u) given by header)\n",line_no,from,nstates);
        ATtableDestroy(labs);
        return false;
      }
      if ( to >= nstates )
      {
        gsErrorMsg("cannot parse AUT input (invalid transition at line %d; state index (%u) higher than maximum (%u) given by header)\n",line_no,to,nstates);
        ATtableDestroy(labs);
        return false;
      }
    } else {
      gsErrorMsg("cannot parse AUT input (invalid transition at line %d)\n",line_no);
      ATtableDestroy(labs);
      return false;
    }

    int label;
    ATerm t = (ATerm) ATmakeAppl(ATmakeAFun(s,0,ATtrue));
    if ( (label =  ATindexedSetGetIndex(labs,t)) < 0 )
    {
      ATbool b;
      label = ATindexedSetPut(labs,t,&b);
      p_add_label(t,!strcmp(s,"tau"));
    }

    p_add_transition(from,(unsigned int) label,to);
  }
  ATtableDestroy(labs);
  if ( ntrans != ntransitions )
  {
    gsErrorMsg("number of transitions read (%u) does not correspond to the number of transition given in the header (%u)\n",ntransitions,ntrans);
    return false;
  }

  this->type = lts_aut;

  return true;
}

bool p_lts::write_to_aut(string const& filename)
{
  ofstream os(filename.c_str());

  if ( !os.is_open() )
  {
    gsVerboseMsg("cannot open AUT file '%s' for writing\n",filename.c_str());
    return false;
  }

  write_to_aut(os);

  os.close();

  return true;
}

bool p_lts::write_to_aut(ostream &os)
{
  os << "des (0," << ntransitions << "," << nstates << ")" << endl;

  for (unsigned int i=0; i<ntransitions; i++)
  {
    unsigned int from = transitions[i].from;
    unsigned int to = transitions[i].to;
    // AUT files need the initial state to be 0, so we will swap state 0 and
    // the initial state
    if ( from == 0 )
    {
      from = init_state;
    } else if ( from == init_state )
    {
      from = 0;
    }
    if ( to == 0 )
    {
      to = init_state;
    } else if ( to == init_state )
    {
      to = 0;
    }
    os << "(" << from << ",\""
       << p_label_value_str(transitions[i].label)
       << "\"," << to << ")" << endl;
  }

  return true;
}

}
}
