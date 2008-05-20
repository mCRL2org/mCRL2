// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts.cpp

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstdlib>
#include "mcrl2/core/struct.h"
#include "mcrl2/lts/liblts.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/lps/specification.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;

#ifdef MCRL2_BCG
#include <bcg_user.h>
#endif

#define ATisAppl(x) (ATgetType(x) == AT_APPL)
#define ATisList(x) (ATgetType(x) == AT_LIST)

using namespace std;

namespace mcrl2
{
namespace lts
{

AFun timed_pair;
bool timed_pair_not_set = true;
static void set_timed_pair()
{
  if ( timed_pair_not_set )
  {
    timed_pair = ATmakeAFun("pair",2,ATfalse);
    ATprotectAFun(timed_pair);
    timed_pair_not_set = false;
  }
}
bool is_timed_pair(ATermAppl t)
{
  set_timed_pair();
  return ATisEqualAFun(ATgetAFun(t),timed_pair);
}
ATermAppl make_timed_pair(ATermAppl action, ATermAppl time)
{
  set_timed_pair();
  if ( time == NULL)
  {
    time = gsMakeNil();
  }
  return ATmakeAppl2(timed_pair,(ATerm) action,(ATerm) time);
}

static int compare_transitions(const void *t1, const void *t2) {
  if (((transition*)t1)->from != ((transition*)t2)->from) {
    return int(((transition*)t1)->from) - int(((transition*)t2)->from);
  } else if (((transition*)t1)->label != ((transition*)t2)->label) {
    return int(((transition*)t1)->label) - int(((transition*)t2)->label);
  } else  {
    return int(((transition*)t1)->to) - int(((transition*)t2)->to);
  } 
}


lts_extra::lts_extra()
{
  type = le_nothing;
}

lts_extra::lts_extra(ATerm t)
{
  type = le_mcrl1;
  content.mcrl1_spec = t;
}

lts_extra::lts_extra(lps::specification *spec)
{
  type = le_mcrl2;
  content.mcrl2_spec = spec;
}

lts_extra::lts_extra(lts_dot_options opts)
{
  type = le_dot;
  content.dot_options = opts;
}

lts_extra_type lts_extra::get_type()
{
  return type;
}

ATerm lts_extra::get_mcrl1_spec()
{
  assert( type == le_mcrl1 );
  return content.mcrl1_spec;
}

lps::specification *lts_extra::get_mcrl2_spec()
{
  assert( type == le_mcrl2 );
  return content.mcrl2_spec;
}

lts_dot_options lts_extra::get_dot_options()
{
  assert( type == le_dot );
  return content.dot_options;
}

lts_extra lts_no_extra = lts_extra();

p_lts::p_lts(lts *l)
{
  lts_object = l;
}

lts::lts(bool state_info, bool label_info) : p_lts(this)
{
  init(state_info,label_info);
}

lts::lts(string &filename, lts_type type) : p_lts(this)
{
  init();
  read_from(filename,type);
}

lts::lts(istream &is, lts_type type) : p_lts(this)
{
  init();
  read_from(is,type);
}

lts::~lts()
{
  if ( state_values != NULL )
  {
    ATunprotectArray(state_values);
  }
  if ( label_values != NULL )
  {
    ATunprotectArray(label_values);
  }

  free(state_values);
  free(taus);
  free(label_values);
  free(transitions);
}

void lts::reset(bool state_info, bool label_info)
{
  clear(state_info,label_info);
}

void p_lts::init(bool state_info, bool label_info)
{
  states_size = 0;
  nstates = 0;
  state_values = NULL;

  labels_size = 0;
  nlabels = 0;
  taus = NULL;
  label_values = NULL;

  transitions_size = 0;
  ntransitions = 0;
  transitions = NULL;
  
  this->type = lts_none;
  this->state_info = state_info;
  this->label_info = label_info;
}

void p_lts::clear(bool state_info, bool label_info)
{
  if ( state_values != NULL )
  {
    ATunprotectArray(state_values);
    free(state_values);
  }
  free(taus);
  if ( label_values != NULL )
  {
    ATunprotectArray(label_values);
    free(label_values);
  }
  free(transitions);

  init(state_info,label_info);
}

lts_type p_lts::detect_type(string const& filename)
{
  ifstream is(filename.c_str(),ifstream::in|ifstream::binary);
  if ( !is.is_open() )
  {
    gsVerboseMsg("cannot open file '%s' for reading\n",filename.c_str());
    return lts_none;
  }

  lts_type t = detect_type(is);

  is.close();

  return t;
}

typedef struct {
  unsigned int flag;
  unsigned int size;
  unsigned int value;
} svc_int;

class svc_buffer
{
  private:
    unsigned char buffer[56];
    unsigned int buffer_size;
    unsigned int pos;
    unsigned int count;
    istream *input;
    bool valid;

  public:
    svc_buffer(istream *is)
    {
      input = is;
      buffer_size = 0;
      pos = 0;
      count = 0;
      valid = true;
    }

    svc_buffer(unsigned char *buf, unsigned int size)
    {
      input = NULL;
      set_buffer(buf,size);
    }

    void set_input(istream *is)
    {
      input = is;
    }

    void set_buffer(unsigned char *buf, unsigned int size)
    {
      if ( size > 56 )
      {
        size = 56;
      }
      memcpy(buffer,buf,size);
      buffer_size = size;
      pos = 0;
      count = 0;
      valid = true;
    }

    void reset_buffer()
    {
      buffer_size = 0;
      pos = 0;
      count = 0;
      valid = true;
    }

    unsigned int get_count()
    {
      return count;
    }

    bool is_valid()
    {
      return valid;
    }

    unsigned int get_bit()
    {
      if ( pos/8 == buffer_size )
      {
        input->read((char *) buffer,56);
        if ( input->eof() )
        {
          input->clear();
        }
        buffer_size = input->gcount();
        pos = 0;
      }
      if ( pos/8 == buffer_size )
      {
        valid = false;
        return 0;
      } else {
        unsigned int r = ( buffer[pos/8] >> (7 - (pos%8)) ) & 1;
        pos++;
        count++;
        return r;
      }
    }

    unsigned char get_byte()
    {
      unsigned char r = 0;
      for (unsigned int i=0; i<8; i++)
      {
        r = r*2 + get_bit();
      }
      return r;
    }

    char get_char()
    {
      char r = 0;
      for (unsigned int i=0; i<7; i++)
      {
        r = r*2 + get_bit();
      }
      return r;
    }

    svc_int get_int()
    {
      svc_int r;
      r.flag = get_bit();
      r.size = get_bit();
      r.size = r.size*2 + get_bit();
      r.value = 0;
      for (unsigned int i=0; i<=r.size; i++)
      {
        r.value = r.value*256 + get_byte();
      }
      return r;
    }

    string get_string()
    {
      string s;
      bool is_valid = false;
      while ( true )
      {
        char c = get_char();
        if ( c == 0 )
        {
          is_valid = true;
          break;
        } else if ( (c < ' ') || (c > '~') )
        {
          break;
        } else {
          s += c;
        }
      }

      if (! (valid && is_valid) ) {
        valid = false;

        s.clear();
      }

      return s;
    }
};

lts_type p_lts::detect_type(istream &is)
{
  if ( is == cin ) // XXX better test to see if is is seekable?
  {
    gsVerboseMsg("type detection does not work on stdin\n");
    return lts_none;
  }

  streampos init_pos = is.tellg();
  char buf[32]; is.read(buf,32);
  if ( is.eof() ) is.clear();
  streamsize r = is.gcount();
  is.seekg(init_pos);

  // detect lts_aut
  if ( r >= 3 )
  {
    // we assume that "des" is completely in buf in case this is a aut file
    int i = 0;
    // skip any spaces or tabs
    while ( (i < r) && ((buf[i] == ' ') || (buf[i] == '\t')) )
    {
      i++;
    }
    // at least need to start with des
    if ( (i+3 <= r) && !memcmp(buf+i,"des",3) )
    {
      i = i + 3;
      // skip any spaces or tabs
      while ( (i < r) && ((buf[i] == ' ') || (buf[i] == '\t')) )
      {
        i++;
      }
      // if we are not at the end of the buffer, then we expect a opening
      // parenthesis
      if ( (i >= r) || (buf[i] == '(') ) 
      {
        gsVerboseMsg("detected AUT input file\n");
        return lts_aut;
      }
    }
  }
  
  // detect lts_svc, lts_mcrl and lts_mcrl2
  if ( r >= 18 )
  {
    svc_buffer sbuf((unsigned char *) buf,r);
    sbuf.get_bit(); // indexed flag
    svc_int header_pos = sbuf.get_int(); // header pos
    if ( header_pos.flag == 0 )
    {
      if ( sbuf.get_int().flag == 0 ) // body pos
      {
        if ( sbuf.get_int().flag == 0 ) // trailer pos
        {
          svc_int version_pos = sbuf.get_int(); // version pos
          if ( (version_pos.flag == 0) &&
               (version_pos.value >= (sbuf.get_count()+7)/8) &&
               (header_pos.value >= (sbuf.get_count()+7)/8) )
          {
            is.seekg(init_pos+((streampos) version_pos.value));
            if ( is.fail() )
            {
              is.seekg(init_pos);
            } else {
              sbuf.reset_buffer();
              sbuf.set_input(&is);
              sbuf.get_string();
              if ( sbuf.is_valid() )
              {
                is.seekg(init_pos+((streampos) header_pos.value));
                if ( is.fail() )
                {
                  is.seekg(init_pos);
                } else {
                  sbuf.reset_buffer();
                  string s;
                  for (unsigned int i=0; i<4; i++)
                  {
                    s = sbuf.get_string();
                    if ( !sbuf.is_valid() )
                    {
                      break;
                    }
                  }

                  is.seekg(init_pos);
                  if ( sbuf.is_valid() )
                  {
                    if ( s == "generic" )
                    {
                      gsVerboseMsg("detected mCRL input file\n");
                      return lts_mcrl;
                    } else if ( (s == "mCRL2") || (s == "mCRL2+info") )
                    {
                      gsVerboseMsg("detected mCRL2 input file\n");
                      return lts_mcrl2;
                    } else {
                      gsVerboseMsg("detected SVC input file\n");
                      return lts_svc;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

#ifdef MCRL2_BCG
  // detect lts_bcg
  if ( r >= 2 )
  {
    if ( (buf[0] == 0x01) && (buf[1] == 0x00) )
    {
      const unsigned int offsets[] = { 0x3, 0xb, 0x13, 0x1b, 0x23, 0x2b, 0x33, 0x3b, 0x43, 0x4b };
      const unsigned int num_offsets = 10;

      is.seekg(0,istream::end);
      streampos size = is.tellg();

      bool valid = true;
      for (unsigned int i=0; i<num_offsets; i++)
      {
        if ( offsets[i] >= size )
        {
          valid = false;
          break;
        }
        
        unsigned int pointer = 0; is.read((char *) &pointer,4);
        if ( pointer >= size )
        {
          valid = false;
          break;
        }
      }

      is.seekg(init_pos);

      if ( valid )
      {
        gsVerboseMsg("detected BCG input file\n");
        return lts_bcg;
      }
    }
  }
#endif

  return lts_none;
}

bool lts::read_from(string const& filename, lts_type type, lts_extra extra)
{
  clear();
  if ( type == lts_none )
  {
    type = detect_type(filename);
    if ( type == lts_none )
    {
      gsVerboseMsg("could not determine type of input file '%s'\n",filename.c_str());
      return false;
    }
  }

  switch ( type )
  {
    case lts_aut:
      return read_from_aut(filename);
    case lts_mcrl:
      return read_from_svc(filename,lts_mcrl);
      break;
    case lts_mcrl2:
      return read_from_svc(filename,lts_mcrl2);
    case lts_svc:
      return read_from_svc(filename,lts_svc);
    case lts_fsm:
      switch ( extra.get_type() )
      {
        case le_mcrl1:
          return read_from_fsm(filename,extra.get_mcrl1_spec());
        case le_mcrl2:
          return read_from_fsm(filename,*extra.get_mcrl2_spec());
        default:
          return read_from_fsm(filename);
      }
    case lts_dot:
      gsVerboseMsg("cannot read dot files\n");
      return false;
#ifdef MCRL2_BCG
    case lts_bcg:
      return read_from_bcg(filename);
#endif
    default:
      assert(0);
      gsVerboseMsg("unknown source LTS type\n");
      return false;
  }
}

bool lts::read_from(istream &is, lts_type type, lts_extra extra)
{
  clear();
  if ( type == lts_none )
  {
    type = detect_type(is);
    if ( type == lts_none )
    {
      gsVerboseMsg("could not determine type of input stream\n");
      return false;
    }
  }

  switch ( type )
  {
    case lts_aut:
      return read_from_aut(is);
    case lts_mcrl:
    case lts_mcrl2:
    case lts_svc:
      gsVerboseMsg("cannot read SVC based files from streams\n");
      return false;
    case lts_fsm:
      switch ( extra.get_type() )
      {
        case le_mcrl1:
          return read_from_fsm(is,extra.get_mcrl1_spec());
        case le_mcrl2:
          return read_from_fsm(is,*extra.get_mcrl2_spec());
        default:
          return read_from_fsm(is);
      }
    case lts_dot:
      gsVerboseMsg("cannot read dot files\n");
      return false;
#ifdef MCRL2_BCG
    case lts_bcg:
      gsVerboseMsg("cannot read BCG files from streams\n");
      return false;
#endif
    default:
      assert(0);
      gsVerboseMsg("unknown source LTS type\n");
      return false;
  }
}

bool lts::write_to(string const& filename, lts_type type, lts_extra extra)
{
  switch ( type )
  {
    case lts_aut:
      return write_to_aut(filename);
    case lts_mcrl:
      return write_to_svc(filename,lts_mcrl);
      break;
    case lts_mcrl2:
      if ( extra.get_type() == le_mcrl2 )
      {
        return write_to_svc(filename,lts_mcrl2,extra.get_mcrl2_spec());
      } else {
        return write_to_svc(filename,lts_mcrl2);
      }
    case lts_svc:
      return write_to_svc(filename,lts_svc);
    case lts_fsm:
      switch ( extra.get_type() )
      {
        case le_mcrl1:
          return write_to_fsm(filename,extra.get_mcrl1_spec());
        case le_mcrl2:
          return write_to_fsm(filename,*extra.get_mcrl2_spec());
        default:
          return write_to_fsm(filename);
      }
    case lts_dot:
      if ( extra.get_type() == le_dot )
      {
        return write_to_dot(filename,extra.get_dot_options());
      } else {
        lts_dot_options opts;
        string s("unknown");
        opts.name = &s;
        opts.print_states = false;
        return write_to_dot(filename,opts);
      }
#ifdef MCRL2_BCG
    case lts_bcg:
      return write_to_bcg(filename);
#endif
    default:
      assert(0);
      gsVerboseMsg("unknown target LTS type\n");
      return false;
  }
}

bool lts::write_to(ostream &os, lts_type type, lts_extra extra)
{
  switch ( type )
  {
    case lts_aut:
      return write_to_aut(os);
    case lts_mcrl:
    case lts_mcrl2:
    case lts_svc:
      gsVerboseMsg("cannot write SVC based files to streams\n");
      return false;
    case lts_fsm:
      switch ( extra.get_type() )
      {
        case le_mcrl1:
          return write_to_fsm(os,extra.get_mcrl1_spec());
        case le_mcrl2:
          return write_to_fsm(os,*extra.get_mcrl2_spec());
        default:
          return write_to_fsm(os);
      }
    case lts_dot:
      if ( extra.get_type() == le_dot )
      {
        return write_to_dot(os,extra.get_dot_options());
      } else {
        lts_dot_options opts;
        string s("unknown");
        opts.name = &s;
        opts.print_states = false;
        return write_to_dot(os,opts);
      }
#ifdef MCRL2_BCG
    case lts_bcg:
      gsVerboseMsg("cannot write BCG files to streams\n");
      return false;
#endif
    default:
      assert(0);
      gsVerboseMsg("unknown target LTS type\n");
      return false;
  }
}

unsigned int lts::num_states()
{
  return nstates;
}

unsigned int lts::num_labels()
{
  return nlabels;
}

unsigned int lts::num_transitions()
{
  return ntransitions;
}

unsigned int lts::initial_state()
{
  return init_state;
}

void lts::set_initial_state(unsigned int state)
{
  assert( state < nstates );
  if ( state < nstates )
  {
    init_state = state;
  }
}

unsigned int lts::add_state(ATerm value)
{
  return p_add_state(value);
}

unsigned int p_lts::p_add_state(ATerm value)
{
  if ( nstates == states_size )
  {
    unsigned int new_states_size = states_size*2;
    if ( states_size == 0 )
    {
      state_info = (value != NULL);
      new_states_size = 128;
    }
    if ( states_size > (50*1025*1025)/sizeof(unsigned int) )
    {
      new_states_size = states_size + (50*1025*1025)/sizeof(unsigned int);
    }

    assert(state_info == (value != NULL));

    if ( state_info )
    {
      if ( state_values != NULL )
      {
        ATunprotectArray(state_values);
      }
      state_values = (ATerm *) realloc(state_values,new_states_size*sizeof(ATerm));
      if ( state_values == NULL )
      {
        gsErrorMsg("insufficient memory to store LTS\n");
        exit(1);
      }
      for (unsigned int i=states_size; i<new_states_size; i++)
      {
        state_values[i] = NULL;
      }
      ATprotectArray(state_values,new_states_size);
    }
    states_size = new_states_size;
  }

  if ( state_info )
  {
    state_values[nstates] = value;
  }

  return nstates++;
}

unsigned int lts::add_label(bool is_tau)
{
  return p_add_label(NULL, is_tau);
}

unsigned int lts::add_label(ATerm value, bool is_tau)
{
  return p_add_label(value, is_tau);
}

unsigned int p_lts::p_add_label(ATerm value, bool is_tau)
{
  if ( nlabels == labels_size )
  {
    unsigned int new_labels_size = labels_size*2;
    if ( labels_size == 0 )
    {
      label_info = (value != NULL);
      new_labels_size = 128;
    }
    if ( labels_size > (50*1025*1025)/sizeof(unsigned int) )
    {
      new_labels_size = labels_size + (50*1025*1025)/sizeof(unsigned int);
    }

    assert(label_info == (value != NULL));

    taus = (bool *) realloc(taus,new_labels_size*sizeof(bool));
    if ( taus == NULL )
    {
      gsErrorMsg("insufficient memory to store LTS\n");
      exit(1);
    }
    if ( label_info )
    {
      if ( label_values != NULL )
      {
        ATunprotectArray(label_values);
      }
      label_values = (ATerm *) realloc(label_values,new_labels_size*sizeof(ATerm));
      if ( label_values == NULL )
      {
        gsErrorMsg("insufficient memory to store LTS\n");
        exit(1);
      }
      for (unsigned int i=labels_size; i<new_labels_size; i++)
      {
        label_values[i] = NULL;
      }
      ATprotectArray(label_values,new_labels_size);
    }
    labels_size = new_labels_size;
  }

  taus[nlabels] = is_tau;
  if ( label_info )
  {
    label_values[nlabels] = value;
  }

  return nlabels++;
}

unsigned int lts::add_transition(unsigned int from,
                                 unsigned int label,
                                 unsigned int to)
{
  return p_add_transition(from,label,to);
}

unsigned int p_lts::p_add_transition(unsigned int from,
                                 unsigned int label,
                                 unsigned int to)
{
  if ( ntransitions == transitions_size )
  {
    unsigned int new_transitions_size = transitions_size*2;
    if ( transitions_size == 0 )
    {
      new_transitions_size = 128;
    }
    if ( transitions_size > (50*1025*1025)/sizeof(transition) )
    {
      new_transitions_size = transitions_size + (50*1025*1025)/sizeof(transition);
    }

    transitions = (transition *) realloc(transitions,new_transitions_size*sizeof(transition));
    if ( transitions == NULL )
    {
      gsErrorMsg("insufficient memory to store LTS\n");
      exit(1);
    }
    transitions_size = new_transitions_size;
  }

  transitions[ntransitions].from = from;
  transitions[ntransitions].label = label;
  transitions[ntransitions].to = to;

  return ntransitions++;
}

void p_lts::p_sort_transitions() {
  qsort(transitions,ntransitions,sizeof(transition),compare_transitions);
}

unsigned int* p_lts::p_get_transition_indices() {
  unsigned int *A = (unsigned int*)malloc((nstates+1)*sizeof(unsigned int));
  if (A == NULL) {
    gsErrorMsg("out of memory\n");
    exit(1);
  }
  unsigned int t = 0;
  A[0] = 0;
  for (unsigned int s = 1; s <= nstates; ++s) {
    while (t < ntransitions && transitions[t].from == s-1) {
      ++t;
    }
    A[s] = t;
  }
  return A;
}

void lts::set_state(unsigned int state, ATerm value)
{
  assert(state_info && (value != NULL));
  state_values[state] = value;
}

void lts::set_label(unsigned int label, ATerm value, bool is_tau)
{
  assert(label_info && (value != NULL));
  label_values[label] = value;
  taus[label] = is_tau;
}

ATerm lts::state_value(unsigned int state)
{
  assert(state_info && (state < nstates));
  return state_values[state];
}

string p_lts::p_state_value_str(unsigned int state)
{
  assert(state < nstates);
  string s;
  if ( state_info )
  {
    ATerm value = state_values[state];
    if ( ATisAppl(value) ) // XXX better check for mCRL2
    {
      s = "(";
      ATermList args = ATgetArguments((ATermAppl) value);
      for (; !ATisEmpty(args); args=ATgetNext(args))
      {
        s += PrintPart_CXX(ATgetFirst(args),ppDefault);
        if ( !ATisEmpty(ATgetNext(args)) )
          s += ",";
      }
      s += ")";
    } else if ( ATisList(value) )
    {
      s = "[";
      ATermList args = (ATermList) value;
      for (; !ATisEmpty(args); args=ATgetNext(args))
      {
        s += ATwriteToString(ATgetFirst(args));
        if ( !ATisEmpty(ATgetNext(args)) )
          s += ",";
      }
      s += "]";
    } else {
      s = ATwriteToString(value);
    }
  } else {
    stringstream ss;
    ss << state;
    s = ss.str();
  }
  return s;
}

string lts::state_value_str(unsigned int state)
{
  return p_state_value_str(state);
}

ATerm lts::label_value(unsigned int label)
{
  assert(label_info && (label < nlabels));
  return label_values[label];
}

string p_lts::p_label_value_str(unsigned int label)
{
  assert(label < nlabels);
  string s;
  if ( label_info )
  {
    ATerm value = label_values[label];
    if ( ATisAppl(value) && gsIsMultAct((ATermAppl) value) )
    {
      s = PrintPart_CXX(value,ppDefault);
    } else if ( ATisAppl(value) && is_timed_pair((ATermAppl) value) )
    {
      s = PrintPart_CXX(ATgetArgument((ATermAppl) value,0),ppDefault);
    } else {
      s = ATwriteToString(value);
      if ( ATisAppl(value) && gsIsString((ATermAppl) value) )
      {
        s = s.substr(1,s.length()-2);
      }
    }
  } else {
    stringstream ss;
    ss << label;
    s = ss.str();
  }
  return s;
}

string lts::label_value_str(unsigned int label)
{
  return p_label_value_str(label);
}

unsigned int lts::transition_from(unsigned int transition)
{
  assert(transition < ntransitions);
  return transitions[transition].from;
}

unsigned int lts::transition_label(unsigned int transition)
{
  assert(transition < ntransitions);
  return transitions[transition].label;
}

unsigned int lts::transition_to(unsigned int transition)
{
  assert(transition < ntransitions);
  return transitions[transition].to;
}

state_iterator lts::get_states()
{
  return state_iterator(this);
}

label_iterator lts::get_labels()
{
  return label_iterator(this);
}

transition_iterator lts::get_transitions()
{
  return transition_iterator(this);
}

bool lts::is_tau(unsigned int label)
{
  assert(label < nlabels);
  return taus[label];
}

void lts::set_tau(unsigned int label, bool is_tau)
{
  assert(label < nlabels);
  taus[label] = is_tau;
}

bool lts::has_creator()
{
  return !creator.empty();
}

string lts::get_creator()
{
  return creator;
}

void lts::set_creator(string creator)
{
  this->creator = creator;
}

lts_type lts::get_type()
{
  return type;
}

bool lts::has_state_info()
{
  return state_info;
}

bool lts::has_label_info()
{
  return label_info;
}

void lts::remove_state_values() {
  state_info = false;
  if ( state_values != NULL )
  {
    ATunprotectArray(state_values);
    free(state_values);
    state_values = NULL;
  }
}

void lts::sort_transitions() {
  p_sort_transitions();
}

unsigned int* lts::get_transition_indices() {
  return p_get_transition_indices();
}

bool lts::reachability_check(bool remove_unreachable)
{
  // We use two algorithms here. One is O(nstates*ntransitions) and needs
  // nstates bits of memory. The other is O(ntransitions), but needs an
  // additional 2*nstates bytes of memory and requires the transitions to be
  // sorted (or more precisely: grouped on the source state).
  //
  // We first allocate memory needed by both algorithms. Then we try to allocate
  // memory for the faster one. If this, or the following test for sortedness,
  // fails, then we just use the slower algorithm.

  // bit array to represent the set of visited states
  #define visited_bpi (8*sizeof(unsigned int)) // bits per (unsigned) int
  unsigned int visited_size = ((nstates-1)/visited_bpi)+1;
                                // nstates/visited_bpi rounded upwards
  unsigned int *visited = (unsigned int *) malloc(visited_size*sizeof(unsigned int));
  #define add_visited(s) (visited[s/visited_bpi] |= 1 << (s % visited_bpi))
  #define in_visited(s) (visited[s/visited_bpi] & (1 << (s % visited_bpi)))
  if ( visited == NULL )
  {
    gsErrorMsg("cannot allocate enough memory for reachability check");
    exit(1);
  }

  // We try to allocate the memory for the faster algorithm. For ease we
  // allocate both needed arrays at once.
  // Note that we also just (todo_stack != NULL) as a general check to see
  // which algorithm we will use.
  unsigned int *todo_stack = (unsigned int *) malloc(2*nstates*sizeof(unsigned int));
  unsigned int *state2trans = &todo_stack[nstates];
  if ( todo_stack != NULL ) // Do we have enough memory for fast algorithm?
  {
    // XXX sort transitions array? Allows us to always use the fast algorithm
    //     (if sufficient memory). However, we must make sure that the
    //     algorithm is O(ntransitions*log(ntransitions)) in the worst case;
    //     otherwise the other reachability algorithm might be faster than the
    //     sorting itself. Also the memory usage should be minimal. (Heapsort
    //     seems a obvious choice.) A downside is that you change the order of
    //     the transitions, which might be undesirable.
    //     (NB: Technically we only need to group the transitions on the from
    //     field)

    // We check whether or not the transitions are grouped on from field (i.e.
    // the source state). While doing this we also store the location of these
    // groups in state2trans (i.e. state2trans[s] will be an index to the
    // beginning of the block of transitions from state s).

    // empty visited states set
    for (unsigned int i=0; i<visited_size; i++)
    {
      visited[i] = 0;
    }
    // initialise state2trans; we use ntransitions as default because we know
    // it exists and no actual transitions have such a high index
    for (unsigned int i=0; i<nstates; i++)
    {
      state2trans[i] = ntransitions;
    }

    unsigned int current_state = nstates;
    bool is_sorted = true;
    // (We just a ghost transitions[-1] with transitions[-1].from == nstates)
    // inv: !is_sorted || ( P(i) && transitions[i-1].from == current_state )
    //       where P(j) = for all states s that occur as source in
    //                    transitions[-1..j) we have that all transitions in
    //                    this part of the array with s as source are stored
    //                    consecutively starting at state2trans[i]
    //                    and for all other states s we have that
    //                    state2trans[i] == ntransitions
    for (unsigned int i=0; i<ntransitions; i++)
    {
      if ( transitions[i].from != current_state )
      {
        current_state = transitions[i].from;
        if ( in_visited(current_state) )
        {
          // We already saw this state are source in another block; the
          // transitions are not sorted.
          is_sorted = false;
          break;
        }
        add_visited(current_state); // remember we saw this state
        state2trans[current_state] = i;
      }
    }
    // post: !is_sorted || P(ntransitions)

    if ( !is_sorted )
    {
      // transitions are not sorted; set todo_stack to NULL such that we use
      // the slower algorithm
      free(todo_stack);
      todo_stack = NULL;
    }
  }

  // empty visited states set
  for (unsigned int i=0; i<visited_size; i++)
  {
    visited[i] = 0;
  }

  // choose between algorithms and execute choice; afterwards in_visited(s)
  // must must hold for all states s reachable from the initial state
  if ( todo_stack == NULL )
  {
    gsDebugMsg("checking reachability with incremental algorithm\n");
    // We're doing the slower algorithm: just loop over all transitions and add
    // target states from transitions that have a source that we have already
    // reached.
    
    add_visited(init_state);

    bool notdone = true;
    while ( notdone )
    {
      notdone = false;
      for (unsigned int i=0; i<ntransitions; i++)
      {
        if ( in_visited(transitions[i].from) && !in_visited(transitions[i].to) )
        {
          add_visited(transitions[i].to);
          notdone = true;
        }
      }
    }

  } else {
    gsDebugMsg("checking reachability with todo list\n");
    // We're doing the faster algorithm: we know that all transitions are
    // grouped on source state with state2trans[s] being the beginning of such
    // a group for source s. We keep a todo list in todo_stack of states that
    // are reachable but of which the outgoing transitions have not yet been
    // investigated.

    // the nitial state is always reachable and will be the initial contents of
    // the todo list
    add_visited(init_state);
    todo_stack[0] = init_state;
    unsigned int todo_stack_num = 1; // number of elements on the stack 

    while ( todo_stack_num > 0 )
    {
      unsigned int current_state = todo_stack[--todo_stack_num]; // top of stack
      unsigned int i = state2trans[current_state]; // index of first transition
                                                   // with current_state as
                                                   // source

      // iterate over all transitions with current_state as source
      while ( (i < ntransitions) && (transitions[i].from == current_state) )
      {
        if ( !in_visited(transitions[i].to) )
        {
          // we haven't seen transitions[i].to before; add it to the todo list
          add_visited(transitions[i].to);
          todo_stack[todo_stack_num++] = transitions[i].to;
        }
        i++;
      }
    }

    // clean up memory needed specifically for this algorithm
    free(todo_stack);
  }
  // in_visited(s) == state s is reachable from the initial state

  bool r = true; // return value
  // check to see if all states are reachable from the initial state
  for (unsigned int i=0; i<visited_size-1; i++) // quickly check the elements
                                                // of the visited array of
                                                // which all bits are used
  {
    if ( visited[i] != (~0U) ) // all bits should be set
    {
      r = false;
      break;
    }
  }
  // the last element of the visited array needs special care as not all bits
  // are necessarily used
  if ( r && (visited_size > 0) )
  {
    // the states in the last element have a index in
    // [(visited_size-1)*visited_bpi..nstates)
    for (unsigned int i=(visited_size-1)*visited_bpi; i<nstates; i++)
    {
      if ( !in_visited(i) )
      {
        r = false;
        break;
      }
    }
  }

  if ( !r && remove_unreachable )
  {
    // Remove all unreachable states, transitions from such states and labels
    // that are only used in these transitions.
    unsigned int *state_map = (unsigned int *) malloc(nstates*sizeof(unsigned int));
    unsigned int *label_map = (unsigned int *) malloc(nlabels*sizeof(unsigned int));
    if ( (state_map == NULL) || (label_map == NULL) )
    {
      gsErrorMsg("not enough memory to remove unreachable states\n");
      free(state_map);
      free(label_map);
      exit(1);
    }

    unsigned int new_nstates = 0;
    for (unsigned int i=0; i<nstates; i++)
    {
      if ( in_visited(i) )
      {
        state_map[i] = new_nstates;
        if ( state_info )
        {
          state_values[new_nstates] = state_values[i];
        }
        new_nstates++;
      }
    }

    for (unsigned int i=0; i<nlabels; i++)
    {
      label_map[i] = 0;
    }

    unsigned int new_ntransitions = 0;
    for (unsigned int i=0; i<ntransitions; i++)
    {
      if ( in_visited(transitions[i].from) )
      {
        label_map[transitions[i].label] = 1;
        transitions[new_ntransitions].from = state_map[transitions[i].from];
        transitions[new_ntransitions].label = transitions[i].label;
        transitions[new_ntransitions].to = state_map[transitions[i].to];
        new_ntransitions++;
      }
    }

    unsigned int new_nlabels = 0;
    for (unsigned int i=0; i<nlabels; i++)
    {
      if ( label_map[i] )
      {
        label_map[i] = new_nlabels;
        taus[new_nlabels] = taus[i];
        if ( label_info )
        {
          label_values[new_nlabels] = label_values[i];
        }
        new_nlabels++;
      }
    }
    for (unsigned int i=0; i<new_ntransitions; i++)
    {
      transitions[i].label = label_map[transitions[i].label];
    }

    init_state = state_map[init_state];
    nstates = new_nstates;
    ntransitions = new_ntransitions;
    nlabels = new_nlabels;

    // XXX realloc tables?

    free(label_map);
    free(state_map);
  }

  free(visited);
  return r;
}

state_iterator::state_iterator(lts *l)
{
  this->l = l;
  pos = 0;
  max = l->nstates;
}

bool state_iterator::more()
{
  return (pos < l->nstates);
}

unsigned int state_iterator::operator *()
{
  return pos;
}

void state_iterator::operator ++()
{
  pos++;
}


label_iterator::label_iterator(lts *l)
{
  this->l = l;
  pos = 0;
  max = l->nlabels;
}

bool label_iterator::more()
{
  return (pos < l->nlabels);
}

unsigned int label_iterator::operator *()
{
  return pos;
}

void label_iterator::operator ++()
{
  pos++;
}


transition_iterator::transition_iterator(lts *l)
{
  this->l = l;
  pos = 0;
  max = l->ntransitions;
}

bool transition_iterator::more()
{
  return (pos < l->ntransitions);
}

unsigned int transition_iterator::from()
{
  return l->transitions[pos].from;
}

unsigned int transition_iterator::label()
{
  return l->transitions[pos].label;
}

unsigned int transition_iterator::to()
{
  return l->transitions[pos].to;
}

void transition_iterator::operator ++()
{
  pos++;
}

lts_type lts::guess_format(string const& s) {
  string::size_type pos = s.find_last_of('.');
  
  if ( pos != string::npos )
  {
    string ext = s.substr(pos+1);

    if ( ext == "aut" )
    {
      gsVerboseMsg("detected AUT extension\n");
      return lts_aut;
    } else if ( ext == "svc" )
    {
      gsVerboseMsg("detected SVC extension; assuming mCRL2 format\n");
      return lts_mcrl2;
    } else if ( ext == "fsm" )
    {
      gsVerboseMsg("detected FSM extension\n");
      return lts_fsm;
    } else if ( ext == "dot" )
    {
      gsVerboseMsg("detected dot extension\n");
      return lts_dot;
#ifdef MCRL2_BCG
    } else if ( ext == "bcg" )
    {
      gsVerboseMsg("detected BCG extension\n");
      return lts_bcg;
#endif
    }
  }

  return lts_none;
}

lts_type lts::parse_format(char const* s) {
  if ( !strcmp(s,"aut") )
  {
    return lts_aut;
  } else if ( !strcmp(s,"mcrl") || !strcmp(s,"svc+mcrl"))
  {
    return lts_mcrl;
  } else if ( !strcmp(s,"mcrl2") || !strcmp(s,"svc+mcrl2"))
  {
    return lts_mcrl2;
  } else if ( !strcmp(s,"svc") )
  {
    return lts_svc;
  } else if ( !strcmp(s,"fsm") )
  {
    return lts_fsm;
  } else if ( !strcmp(s,"dot") )
  {
    return lts_dot;
#ifdef MCRL2_BCG
  } else if ( !strcmp(s,"bcg") )
  {
    return lts_bcg;
#endif
  }

  return lts_none;
}

char const* p_lts::type_strings[]      = { "unknown", "mCRL2", "AUT", "mCRL", "SVC", "FSM", "dot", "BCG" };
                                                                                                      
char const* p_lts::extension_strings[] = { "",        "svc",   "aut", "svc",  "svc", "fsm", "dot", "bcg" };

char const* lts::string_for_type(const lts_type type) {
  return (type_strings[type]);
}

char const* lts::extension_for_type(const lts_type type) {
  return (extension_strings[type]);
}

lts_equivalence lts::parse_equivalence(char const* s)
{
  if ( !strcmp(s,"strong") )
  {
    return lts_eq_strong;
  } else if ( !strcmp(s,"branch") )
  {
    return lts_eq_branch;
  } else if ( !strcmp(s,"trace") )
  {
    return lts_eq_trace;
  } else if ( !strcmp(s,"wtrace") )
  {
    return lts_eq_weak_trace;
  } else if ( !strcmp(s,"isomorph") )
  {
    return lts_eq_isomorph;
  } else {
    return lts_eq_none;
  }
}

char const* p_lts::equivalence_strings[]      = { "unknown", "trace", "strong", "branch", "wtrace", "isomorph" };
                                                                                                      
char const* p_lts::equivalence_desc_strings[] = { "unknown equivalence", "trace equivalence", "strong bisimilarity", "weak trace equivalence", "branching bisimilarity", "isomorphism" };

char const* lts::string_for_equivalence(const lts_equivalence s)
{
  return equivalence_strings[s];
}

char const* lts::name_of_equivalence(const lts_equivalence s)
{
  return equivalence_desc_strings[s];
}

}
}
