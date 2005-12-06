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

#define TRACE_V1_MARKER "mCRL2TraceV1"
#define TRACE_V1_MARKER_SIZE 12

#define INIT_BUF_SIZE 64

using namespace std;

void Trace::init()
{
	states = (ATermAppl *) malloc(INIT_BUF_SIZE*sizeof(ATermAppl));
	actions = (ATermAppl *) malloc(INIT_BUF_SIZE*sizeof(ATermAppl));
	buf_size = INIT_BUF_SIZE;
	len = 0;
	pos = 0;
	for (unsigned int i=0; i<buf_size; i++)
	{
		states[i] = NULL;
		actions[i] = NULL;
	}
	ATprotectArray((ATerm *) states,buf_size);
	ATprotectArray((ATerm *) actions,buf_size);
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
	ATunprotectArray((ATerm *) actions);
	ATunprotectArray((ATerm *) states);
	free(actions);
	free(states);
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


void Trace::addAction(ATermAppl action)
{
	actions[pos] = action;
	pos++;
	len = pos;
	if ( len == buf_size )
	{
		ATunprotectArray((ATerm *) states);
		ATunprotectArray((ATerm *) actions);
		states = (ATermAppl *) realloc(states,buf_size*2*sizeof(ATermAppl));
		actions = (ATermAppl *) realloc(actions,buf_size*2*sizeof(ATermAppl));
		for (unsigned int i=buf_size; i<buf_size*2; i++)
		{
			states[i] = NULL;
			actions[i] = NULL;
		}
		buf_size = buf_size * 2;
		ATprotectArray((ATerm *) states,buf_size);
		ATprotectArray((ATerm *) actions,buf_size);
	} else {
		states[pos] = NULL;
		actions[pos] = NULL;
	}
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

TraceFormat Trace::detectFormat(istream &is)
{
	char buf[TRACE_V1_MARKER_SIZE];
	TraceFormat fmt = tfPlain;

	is.read(buf,TRACE_V1_MARKER_SIZE);

	if ( (is.gcount() == TRACE_V1_MARKER_SIZE) && !strncmp(buf,TRACE_V1_MARKER,TRACE_V1_MARKER_SIZE) )
	{
		fmt = tfVer1;
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

void Trace::loadVer1(istream &is)
{
	char buf[TRACE_V1_MARKER_SIZE];
	is.read(buf,TRACE_V1_MARKER_SIZE);

	assert(!strncmp(buf,TRACE_V1_MARKER,TRACE_V1_MARKER_SIZE));

	resetPosition();
	truncate();

	ATermList trace = (ATermList) readATerm(is); //XXX

	for (; !ATisEmpty(trace); trace=ATgetNext(trace))
	{
		ATermAppl e = (ATermAppl) ATgetFirst(trace);

		if ( gsIsMultAct(e) )
		{
			addAction(e);
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
		case tfVer1:
			loadVer1(is);
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

void Trace::saveVer1(ostream &os)
{
	ATermList trace = ATmakeList0();

	unsigned int i=len+1;
	while ( i > 0 )
	{
		i--;
		if ( actions[i] != NULL )
		{
			trace = ATinsert(trace,(ATerm) actions[i]);
		}
		if ( states[i] != NULL )
		{
			trace = ATinsert(trace,(ATerm) states[i]);
		}
	}

	os << TRACE_V1_MARKER;
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
			PrintPart_CXX(os,(ATerm) actions[i],ppAdvanced);
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
		case tfVer1:
			saveVer1(os);
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
