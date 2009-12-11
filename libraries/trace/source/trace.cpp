// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file trace.cpp
/// \brief This file contains the implementation of the trace class

#include <aterm2.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/print.h"
#include "mcrl2/trace.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::trace;



#define TRACE_MCRL2_MARKER "mCRL2Trace"
#define TRACE_MCRL2_MARKER_SIZE 10
#define TRACE_MCRL2_VERSION "\x01\x00"
#define TRACE_MCRL2_VERSION_SIZE 2

#define INIT_BUF_SIZE 64

using namespace std;

AFun trace_pair;
int trace_pair_set = 0;

static bool isTimedMAct(ATermAppl t)
{
  return ATisEqualAFun(ATgetAFun(t),trace_pair);
}

static ATermAppl makeTimedMAct(ATermAppl act, ATermAppl time)
{
  return ATmakeAppl2(trace_pair,(ATerm) act,(ATerm) time);
}

void Trace::init()
{
  if ( trace_pair_set == 0 )
  {
    trace_pair = ATmakeAFun("pair",2,ATfalse);
    ATprotectAFun(trace_pair);
  }
  trace_pair_set++;

  states = (ATermAppl *) malloc(INIT_BUF_SIZE*sizeof(ATermAppl));
  actions = (ATermAppl *) malloc(INIT_BUF_SIZE*sizeof(ATermAppl));
  times = (ATermAppl *) malloc(INIT_BUF_SIZE*sizeof(ATermAppl));
  buf_size = INIT_BUF_SIZE;
  len = 0;
  pos = 0;
  for (unsigned int i=0; i<buf_size; i++)
  {
    states[i] = NULL;
    actions[i] = NULL;
    times[i] = NULL;
  }
  ATprotectArray((ATerm *) states,buf_size);
  ATprotectArray((ATerm *) actions,buf_size);
  ATprotectArray((ATerm *) times,buf_size);

  // XXX set times[0] to gsMakeTime(0.0)?
}

void Trace::cleanup()
{
  ATunprotectArray((ATerm *) times);
  ATunprotectArray((ATerm *) actions);
  ATunprotectArray((ATerm *) states);
  free(times);
  free(actions);
  free(states);

  trace_pair_set--;
  if ( trace_pair_set == 0 )
  {
    ATunprotectAFun(trace_pair);
  }
}


Trace::Trace()
{
  init();
}

Trace::Trace(istream &is, TraceFormat tf)
{
  init();
  try
  {
    load(is,tf);
  } catch ( ... ) {
    cleanup();
    throw;
  }
}

Trace::Trace(string const& filename, TraceFormat tf)
{
  init();
  try
  {
    load(filename,tf);
  } catch ( ... ) {
    cleanup();
    throw;
  }
}

Trace::~Trace()
{
  cleanup();
}

void Trace::resetPosition()
{
  pos = 0;
}

void Trace::setPosition(unsigned int pos)
{
  if ( pos <= len )
  {
    this->pos = pos;
  }
}

unsigned int Trace::getPosition()
{
  return pos;
}

unsigned int Trace::getLength()
{
  return len;
}

ATermAppl Trace::currentState()
{
  return states[pos];
}

ATermAppl Trace::currentTime()
{
  return times[pos];
}

ATermAppl Trace::nextAction()
{
  ATermAppl act = actions[pos];

  if ( pos < len )
  {
    pos++;
  }

  return act;
}

void Trace::truncate()
{
  len = pos;
  actions[pos] = NULL;
}


void Trace::addAction(ATermAppl action, ATermAppl time)
{
  actions[pos] = action;
  pos++;
  len = pos;
  if ( len == buf_size )
  {
    ATunprotectArray((ATerm *) states);
    ATunprotectArray((ATerm *) actions);
    ATunprotectArray((ATerm *) times);
    states = (ATermAppl *) realloc(states,buf_size*2*sizeof(ATermAppl));
    actions = (ATermAppl *) realloc(actions,buf_size*2*sizeof(ATermAppl));
    times = (ATermAppl *) realloc(times,buf_size*2*sizeof(ATermAppl));
    for (unsigned int i=buf_size; i<buf_size*2; i++)
    {
      states[i] = NULL;
      actions[i] = NULL;
      times[i] = NULL;
    }
    buf_size = buf_size * 2;
    ATprotectArray((ATerm *) states,buf_size);
    ATprotectArray((ATerm *) actions,buf_size);
    ATprotectArray((ATerm *) times,buf_size);
  } else {
    states[pos] = NULL;
    actions[pos] = NULL;
  }
  times[pos] = time;
}

bool Trace::setState(ATermAppl state)
{
  if ( states[pos] == NULL )
  {
    states[pos] = state;
    return true;
  } else {
    return false;
  }
}

bool Trace::canSetState()
{
  return (states[pos] == NULL);
}


TraceFormat Trace::detectFormat(istream &is)
{
  char buf[TRACE_MCRL2_MARKER_SIZE];
  TraceFormat fmt = tfPlain;

  is.read(buf,TRACE_MCRL2_MARKER_SIZE);
  if ( is.bad() )
  {
    throw runtime_error("could not read from stream");
  }
  is.clear();

  if ( (is.gcount() == TRACE_MCRL2_MARKER_SIZE) && !strncmp(buf,TRACE_MCRL2_MARKER,TRACE_MCRL2_MARKER_SIZE) )
  {
    fmt = tfMcrl2;
  }

  is.seekg(-is.gcount(),ios::cur);
  if ( is.fail() )
  {
    throw runtime_error("could set position in stream");
  }

  return fmt;
}

ATerm readATerm(istream &is)
{
#define RAT_INIT_BUF_SIZE (64*1024)
  unsigned int buf_size = RAT_INIT_BUF_SIZE;
  char *buf = NULL;
  std::streamsize len = 0;

  while ( !is.eof() )
  {
    char *new_buf = (char *) realloc(buf,buf_size*sizeof(char));
    if ( new_buf == NULL )
    {
      free(buf);
      throw runtime_error("not enough memory to read ATerm");
    }
    buf = new_buf;

    is.read(buf+len,buf_size-len);
    if ( is.bad() )
    {
      free(buf);
      throw runtime_error("could not read ATerm from stream");
    }

    len+=is.gcount();
    buf_size = buf_size * 2;
  }
  is.clear();

  ATerm t = ATreadFromBinaryString((unsigned char *) buf, static_cast< int >(len));
  if ( t == NULL )
  {
    throw runtime_error("failed to read ATerm from stream");
  }

  free(buf);

  return t;
}

void Trace::loadMcrl2(istream &is)
{
  char buf[TRACE_MCRL2_MARKER_SIZE+TRACE_MCRL2_VERSION_SIZE];
  is.read(buf,TRACE_MCRL2_MARKER_SIZE+TRACE_MCRL2_VERSION_SIZE);
  if ( is.bad() || strncmp(buf,TRACE_MCRL2_MARKER,TRACE_MCRL2_MARKER_SIZE) )
  {
    throw runtime_error("stream does not contain an mCRL2 trace");
  }
  is.clear();

  resetPosition();
  truncate();

  ATermList trace = (ATermList) readATerm(is);
  assert( ATgetType(trace) == AT_LIST );

  for (; !ATisEmpty(trace); trace=ATgetNext(trace))
  {
    ATermAppl e = (ATermAppl) ATgetFirst(trace);

    if ( gsIsMultAct(e) ) // To be compatible with old untimed version
    {
      addAction(e);
    } else if ( isTimedMAct(e) )
    {
      if ( gsIsNil(ATAgetArgument(e,1)) )
      {
        addAction(ATAgetArgument(e,0));
      } else {
        addAction(ATAgetArgument(e,0),ATAgetArgument(e,1));
      }
    } else {
      setState(e);
    }
  }

  resetPosition();
}

void Trace::loadPlain(istream &is)
{
#define MAX_LINE_SIZE 1024
  char buf[MAX_LINE_SIZE];
  resetPosition();
  truncate();

  while ( !is.eof() )
  {
    is.getline(buf,MAX_LINE_SIZE);
    if ( is.bad() )
    {
      throw runtime_error("error while reading from stream");
    }
    if ( (strlen(buf) > 0) && (buf[strlen(buf)-1] == '\r') )
    { // remove CR
      buf[strlen(buf)-1] = '\0';
    }

    if ( is.gcount() > 0 )
    {
      // XXX need to parse trace
      addAction(ATmakeAppl0(ATmakeAFun(buf,0,ATfalse)));
    }
  }
  is.clear();

  resetPosition();
}

void Trace::load(istream &is, TraceFormat tf)
{
  try
  {

    if ( tf == tfUnknown )
    {
      tf = detectFormat(is);
    }

    switch ( tf )
    {
    case tfMcrl2:
      loadMcrl2(is);
      break;
    case tfPlain:
      loadPlain(is);
      break;
    default:
      break;
    }

  } catch ( runtime_error err )
  {
    string s;
    s = "error loading trace (";
    s += err.what();
    s += ")";
    throw runtime_error(s);
  }
}

void Trace::load(string const& filename, TraceFormat tf)
{
  ifstream is(filename.c_str(),ifstream::binary|ifstream::in);

  if ( !is.is_open() )
  {
    throw runtime_error("error loading trace (could not open file)");
  }

  try
  {
    load(is, tf);
  } catch (...)
  {
    is.close();
    throw;
  }

  is.close();
}

void Trace::saveMcrl2(ostream &os)
{
  ATermList trace = ATmakeList0();

  bool error_shown = false;
  unsigned int i=len+1;
  while ( i > 0 )
  {
    i--;
    if ( actions[i] != NULL )
    {
      if ( !gsIsMultAct(actions[i]) && !error_shown )
      {
        gsErrorMsg("saving trace that is not in mCRL2 format to a mCRL2 trace format\n");
        error_shown = true;
      }
      trace = ATinsert(trace,(ATerm) makeTimedMAct(actions[i], (times[i]==NULL)?gsMakeNil():times[i]) );
    }
    if ( states[i] != NULL )
    {
      trace = ATinsert(trace,(ATerm) states[i]);
    }
  }

  // write marker
  os << TRACE_MCRL2_MARKER;
  os.write(TRACE_MCRL2_VERSION,TRACE_MCRL2_VERSION_SIZE);
  if ( os.bad() )
  {
    throw runtime_error("could not write to stream");
  }

  // write trace
  int len;
  const char *bs = (const char *) ATwriteToBinaryString((ATerm) trace,&len); //XXX no error handling?
  os.write(bs,len);
  if ( os.bad() )
  {
    throw runtime_error("could not write to stream");
  }
}

void Trace::savePlain(ostream &os)
{
  for (unsigned int i=0; i<len; i++)
  {
    if ( gsIsMultAct(actions[i]) )
    {
      PrintPart_CXX(os,(ATerm) actions[i],ppDefault);
    } else {
      os << ATwriteToString((ATerm) actions[i]);
    }
    os << endl;
    if ( os.bad() )
    {
      throw runtime_error("could not write to stream");
    }
  }
}

void Trace::save(ostream &os, TraceFormat tf)
{
  try
  {
    switch ( tf )
    {
    case tfMcrl2:
      saveMcrl2(os);
      break;
    case tfPlain:
      savePlain(os);
      break;
    default:
      break;
    }
  } catch (runtime_error err)
  {
    string s;
    s = "error saving trace (";
    s += err.what();
    s += ")";
    throw runtime_error(s);
  }
}

void Trace::save(string const& filename, TraceFormat tf)
{
  ofstream os(filename.c_str(),ofstream::binary|ofstream::out|ofstream::trunc);
  if ( !os.is_open() )
  {
    throw runtime_error("error saving trace (could not open file)");
  }

  try
  {
    save(os, tf);
  } catch (...)
  {
    os.close();
    throw;
  }

  os.close();
}
