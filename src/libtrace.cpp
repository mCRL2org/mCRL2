#include <aterm2.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <ext/stdio_filebuf.h>
#include <string>
#include "libstruct.h"
#include "libprint.h"
#include "libprint_c.h"
#include "libtrace.h"


/* mCRL2 Trace Format
 *
 *   Bytes      Information
 *
 *    0 -  9    Marker "mCRL2Trace"
 *   10 - 11    Version number (little endian)
 *   12 -       Binary ATermList representing the trace
 *
 * The contents of the trace ATermList have the following form.
 *
 *   T   ::=  T'  |  State |> T'  |  State
 *   T'  ::=  pair(Action,Time) |> T  |  pair(Action,Time)  |  T''
 *   T'' ::=  Action |> T  |  Action
 *
 * This means that a trace is a sequence of Action/Time-pairs with at most one
 * State between every pair and at the beginning and end of the trace.
 * Actions are in the mCRL2 MultAct(<Action>*) format, Time is a mCRL2
 * <DataExpr> of the sort Time and States are ATermAppls with "STATE" as
 * function symbol and mCRL2 <DataExpr>s as * arguments.
 *
 * Note that T'' is only here for backwards compatibility with the previous
 * untimed version.
 */

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

Trace::Trace()
{
	init();
}

Trace::Trace(istream &is, TraceFormat tf)
{
	init();
	load(is,tf);
}

Trace::Trace(string &filename, TraceFormat tf)
{
	init();
	load(filename,tf);
}

Trace::~Trace()
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

void Trace::resetPosition()
{
	pos = 0;
}

void Trace::setPosition(unsigned int pos)
{
	if ( pos < len )
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
	if ( pos < len )
	{
		return actions[pos++];
	} else {
		return NULL;
	}
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


ATermAppl Trace::getAction()
{
	ATermAppl act = actions[pos];
	
	if ( pos < len )
	{
		pos++;
	}

	return act;
}

ATermAppl Trace::getState()
{
	return states[pos];
}

ATermAppl Trace::getTime()
{
	return times[pos];
}

TraceFormat Trace::detectFormat(istream &is)
{
	char buf[TRACE_MCRL2_MARKER_SIZE];
	TraceFormat fmt = tfPlain;

	is.read(buf,TRACE_MCRL2_MARKER_SIZE);

	if ( (is.gcount() == TRACE_MCRL2_MARKER_SIZE) && !strncmp(buf,TRACE_MCRL2_MARKER,TRACE_MCRL2_MARKER_SIZE) )
	{
		fmt = tfMcrl2;
	}

	is.seekg(-is.gcount(),ios::cur);

	return fmt;
}

ATerm readATerm(istream &is)
{
#define RAT_INIT_BUF_SIZE (64*1024)
	unsigned int buf_size = RAT_INIT_BUF_SIZE;
	char *buf = (char *) malloc(buf_size*sizeof(char));
	unsigned int len = 0;

	while ( !is.eof() )
	{
		buf_size = buf_size * 2;
		buf = (char *) realloc(buf,buf_size*sizeof(char));
		is.read(buf+len,RAT_INIT_BUF_SIZE);
		len+=is.gcount();

	}

	ATerm t = ATreadFromBinaryString(buf,len);

	free(buf);

	return t;
}

void Trace::loadMcrl2(istream &is)
{
	char buf[TRACE_MCRL2_MARKER_SIZE+TRACE_MCRL2_VERSION_SIZE];
	is.read(buf,TRACE_MCRL2_MARKER_SIZE+TRACE_MCRL2_VERSION_SIZE);

	assert(!strncmp(buf,TRACE_MCRL2_MARKER,TRACE_MCRL2_MARKER_SIZE));

	resetPosition();
	truncate();

	ATermList trace = (ATermList) readATerm(is); //XXX

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
		if  ( is.gcount() > 0 )
		{
			// XXX need to parse trace
			addAction(ATmakeAppl0(ATmakeAFun(buf,0,ATfalse)));
		}
	}

	resetPosition();
}

void Trace::load(istream &is, TraceFormat tf)
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
}

void Trace::load(string &filename, TraceFormat tf)
{
	ifstream is(filename.c_str(),ifstream::binary|ifstream::in);

	load(is, tf);

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

	os << TRACE_MCRL2_MARKER;
	os.write(TRACE_MCRL2_VERSION,TRACE_MCRL2_VERSION_SIZE);
	int len;
	char *bs = ATwriteToBinaryString((ATerm) trace,&len); //XXX
	os.write(bs,len);
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
	}
}

void Trace::save(ostream &os, TraceFormat tf)
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
}

void Trace::save(string &filename, TraceFormat tf)
{
	ofstream os(filename.c_str(),ofstream::binary|ofstream::out|ofstream::trunc);

	save(os, tf);

	os.close();
}
