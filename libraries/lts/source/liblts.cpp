// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts.cpp

#include <string>
#include <set>
#include <stack>
#include <bitset>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstdlib>
#include <algorithm>
// #include <boost/bind.hpp>
#include "aterm2.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/lts/lts_utilities.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/data/data_specification.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;

#ifdef USE_BCG
#include <bcg_user.h>
#endif

#define ATisAppl(x) (ATgetType(x) == AT_APPL)
#define ATisList(x) (ATgetType(x) == AT_LIST)

using namespace std;

namespace mcrl2
{
namespace lts
{
namespace detail
{
lps::specification const& empty_specification() {
  static lps::specification dummy;

  return dummy;
}
}

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

static bool compare_transitions_slt(const transition t1, const transition t2) 
{
  if (t1.from() != t2.from()) 
  {
    return t1.from() < t2.from();
  } 
  else if (t1.label() != t2.label()) 
  {
    return t1.label() < t2.label();
  } 
  else  
  {
    return t1.to() < t2.to();
  }
}

static bool compare_transitions_lts(const transition t1, const transition t2) 
{
  if (t1.label() != t2.label()) 
  {
    return t1.label() < t2.label();
  } 
  else if (t1.to() != t2.to()) 
  {
    return t1.to() < t2.to();
  } 
  else 
  {
    return t1.from() < t2.from();
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

lts_extra::lts_extra(lps::specification const& spec)
{
  type = le_mcrl2;
  content.mcrl2_spec = lps::specification_to_aterm(spec);
}

lts_extra::lts_extra(detail::lts_dot_options opts)
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

lps::specification lts_extra::get_mcrl2_spec()
{
  assert( type == le_mcrl2 );
  return lps::specification(content.mcrl2_spec);
}

detail::lts_dot_options lts_extra::get_dot_options()
{
  assert( type == le_dot );
  return content.dot_options;
}

lts_extra lts_no_extra = lts_extra();


void lts::init()
{ 
  nstates = 0;
  nlabels = 0;

  extra_data = NULL;
  ATprotect(&extra_data);

  this->type = lts_none;
}

lts::lts() 
{ 
  init();
}

lts::lts(string &filename, lts_type type,lts_extra extra) 
{
  init();
  detail::read_from(*this,filename,type);
}

lts::lts(istream &is, lts_type type,lts_extra extra) 
{
  init();
  detail::read_from(*this,is,type,extra);
}

lts::lts(const std::string &s) 
{
  init();
  std::istringstream is(s);
  detail::read_from(*this,is,lts_aut);
}

lts::~lts()
{ 
  ATunprotect(&extra_data);
}

lts::lts(lts const &l) 
{
  init_state = l.init_state;
  nstates = l.nstates;
  nlabels = l.nlabels;

  transitions = l.transitions;

  type = l.type;

  extra_data = l.extra_data;
  ATprotect(&extra_data);

  state_values=l.state_values;

  taus = l.taus;

  label_values = l.label_values;

  creator = l.creator;
}

void lts::swap(lts &l) 
{ 
  { const unsigned int aux=init_state; init_state=l.init_state;   l.init_state=aux; }
  { const unsigned int aux=nstates;    nstates=l.nstates;         l.nstates=aux; }
  { const unsigned int aux=nlabels;    nlabels=l.nlabels;         l.nlabels=aux; }

  transitions.swap(l.transitions);

  { const lts_type aux=type;           type=l.type;               l.type=aux; }

  { const ATerm aux=extra_data;        extra_data=l.extra_data;   l.extra_data=aux; }

  state_values.swap(l.state_values);
  taus.swap(l.taus);

  label_values.swap(l.label_values);

  creator.swap(l.creator);
}

void lts::clear()
{
  clear_states();
  clear_labels();
  clear_transitions();
  clear_type();
  extra_data = NULL;

}

  void lts::clear_states()
  {
    remove_state_values();
    nstates = 0;
  }


// Merges an LTS L with this LTS (say K) and stores the resulting LTS
// (say M) in this LTS datastructure, effectively replacing K.
// Conceptually, we just take the union of the sets of states and the
// sets of transitions of K and L:
//   States_M      = States_K + States_L
//   Transitions_M = Transitions_K + Transitions_L
// where + denotes set union.
// However, this assumes that States_K and States_L are disjoint,
// which is generally not the case. More specifically we have:
//   States_K = { 0, ..., N_K - 1 }   and
//   States_L = { 0, ..., N_L - 1 }
// for some N_K, N_L > 0.
// Therefore, state i of L will be numbered |N_K| + i in the resulting
// LTS M and state i of K will be numbered i in M. This yields:
//   States_M = { 0, ..., N_K + N_L - 1 }.
void merge(lts &l1, const lts &l2)
{ 
  const unsigned int old_nstates=l1.num_states();
  l1.set_num_states(l1.num_states() + l2.num_states());
  

  // The resulting LTS will have state information only if BOTH LTSs
  // currently have state information.
  if ( l1.has_state_info() && l2.has_state_info() )
  {
    for (unsigned int i=0; i<l2.num_states(); ++i)
    {
      l1.add_state(l2.state_value(i));
    }
  }
  else
  {
    // remove state information from this LTS, if any
    l1.remove_state_values();
  }

  unsigned new_nlabels = 0;
  if (l1.has_label_info() && l2.has_label_info())
  {
    // Before we can set the label data in the realloc'ed transitions
    // array, we first have to collect the labels of both LTSs in an
    // indexed set.
    ATermIndexedSet labs = ATindexedSetCreate(l1.num_labels() + l2.num_labels(),75);
    ATbool b;

    // Add the labels of this LTS and count the number of labels that
    // the resulting LTS will contain
    for (unsigned int i = 0; i < l1.num_labels(); ++i)
    {
      ATindexedSetPut(labs,l1.label_value(i),&b);
      if ( b )
      {
        ++new_nlabels;
      }
    }
    // Same for LTS l2
    for (unsigned int i=0; i<l2.num_labels(); ++i)
    {
      ATindexedSetPut(labs,l2.label_value(i),&b);
      if ( b )
      {
        ++new_nlabels;
      }
    }

    // Update the tau-information
    std::vector<bool> new_taus(new_nlabels,false);
    for (unsigned int i = 0; i < l1.num_labels(); ++i)
    { 
      assert(ATindexedSetGetIndex(labs,l1.label_value(i))<(int)new_taus.size());
      new_taus[ATindexedSetGetIndex(labs,l1.label_value(i))] = l1.is_tau(i);
    }
    for (unsigned int i = 0; i < l2.num_labels(); ++i)
    { assert(ATindexedSetGetIndex(labs,l2.label_value(i))<(int)new_taus.size());
      new_taus[ATindexedSetGetIndex(labs,l2.label_value(i))] = l2.is_tau(i);
    }
    
    // Store the label values contained in the indexed set
    l1.clear_labels(); 

    for (unsigned int i = 0; i < new_nlabels; ++i)
    {
      l1.add_label(ATindexedSetGetElem(labs,i),new_taus[i]);
    }

    // Update the label numbers of all transitions of this LTS to
    // the new indices as given by the indexed set.
    
    for (transition_range r = l1.get_transitions(); !r.empty(); r.advance_begin(1))
    { r.front().set_label(
        ATindexedSetGetIndex(labs,l1.label_value(r.front().label())));
    }
    // Now add the transition labels of LTS l2

    // Now add the source and target states of the transitions of LTS l2.
    // The labels will be added below, depending on whether there is label
    // information in both LTSs.
    for (transition_const_range r = l2.get_transitions(); !r.empty(); r.advance_begin(1))
    { const transition transition_to_add=r.front();
      l1.add_transition(transition(transition_to_add.from()+old_nstates,
                                ATindexedSetGetIndex(labs,l2.label_value(transition_to_add.label())),
                                transition_to_add.to()+old_nstates));
    }


    ATindexedSetDestroy(labs);
  }
  else
  {
    // One of the LTSs does not have label info, so the resulting LTS
    // will not have label info either. Moreover, we consider the sets
    // of labels of the LTSs to be disjoint
    const unsigned int old_nlabels=l1.num_labels();

    // Remove label info from this LTS, if any
    if ( l1.has_label_info() )
    {
      l1.set_num_labels(old_nlabels,false);
    }
    // Now add the source and target states of the transitions of LTS l2.
    // The labels will be added below, depending on whether there is label
    // information in both LTSs.
    
    // Add taus from LTS l2
    for (unsigned int i = 0; i < l2.num_labels(); ++i)
    {
      l1.add_label(l2.is_tau(i));
    }

    for (transition_const_range r = l2.get_transitions(); !r.empty(); r.advance_begin(1))
    { const transition transition_to_add=r.front();
      l1.add_transition(transition(transition_to_add.from()+old_nstates,
                                   transition_to_add.label()+old_nlabels,
                                   transition_to_add.to()+old_nstates));
    }

  }

  // Update the fields that have not been updated yet
}

  void lts::clear_type()
  {
    type = lts_none;
  }

namespace detail
{

lts_type detect_type(string const& filename)
{
  ifstream is(filename.c_str(),ifstream::in|ifstream::binary);
  if ( !is.is_open() )
  {
    throw mcrl2::runtime_error("Cannot open file '" + filename + "' for reading.");
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
    std::streamsize buffer_size;
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
      if ( static_cast< std::streamsize >(pos/8) == buffer_size )
      {
        input->read((char *) buffer,56);
        if ( input->eof() )
        {
          input->clear();
        }
        buffer_size = input->gcount();
        pos = 0;
      }
      if ( static_cast< std::streamsize >(pos/8) == buffer_size )
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

lts_type detect_type(istream &is)
{
  if ( is == cin ) // XXX better test to see if is is seekable?
  {
    throw mcrl2::runtime_error("Type detection does not work on stdin.");
    return lts_none;
  }

  streampos init_pos = is.tellg();
  char buf[32]; is.read(buf,32);
  if ( is.eof() ) is.clear();
  std::streamsize r = is.gcount();
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
      { if (core::gsVerbose)
        { std::cerr << "detected AUT input file\n";
        }
        return lts_aut;
      }
    }
  }

  // detect lts_dot
  if ( r >= 7 )
  {
    // we assume that "digraph" is completely in buf in case this is a dot file
    int i = 0;
    // skip any spaces or tabs
    while ( (i < r) && ((buf[i] == ' ') || (buf[i] == '\t')) )
    {
      i++;
    }
    // at least need to start with digraph
    if ( (i+7 <= r) && !memcmp(buf+i,"digraph",7) )
    {
      i = i + 7;
      if (core::gsVerbose)
      { std::cerr << "Detected DOT input file.\n";
      }
      return lts_dot;
    }
  }

  // detect lts_svc, lts_mcrl and lts_mcrl2
  if ( r >= 18 )
  {
    svc_buffer sbuf((unsigned char *) buf, static_cast< unsigned int >(r));
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
                      if (core::gsVerbose)
                      { std::cerr << "Detected mCRL input file.\n";
                      }
                      return lts_mcrl;
                    } else if ( (s == "mCRL2") || (s == "mCRL2+info") )
                    { if (core::gsVerbose)
                      { std::cerr << "Detected mCRL2 input file.\n";
                      }
                      return lts_mcrl2;
                    } else {
                      if (core::gsVerbose)
                      { std::cerr << "Detected SVC input file\n";
                      }
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

#ifdef USE_BCG
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
        if (core::gsVerbose)
        { std::cerr << "Detected BCG input file.\n";
        }
        return lts_bcg;
      }
    }
  }
#endif

  return lts_none;
}

void read_from(lts &l,string const& filename, lts_type type, lts_extra extra)
{ 
  if ( type == lts_none )
  {
    type = detect_type(filename);
    if ( type == lts_none && (type = guess_format(filename)) == lts_none )
    {
      mcrl2::runtime_error("Could not determine type of input file '" + filename + "'.");
    }
  }

  switch ( type )
  {
    case lts_aut:
      read_from_aut(l,filename);
      return;
    case lts_mcrl:
      read_from_svc(l,filename,lts_mcrl);
      return;
    case lts_mcrl2:
      read_from_svc(l,filename,lts_mcrl2);
      return;
    case lts_svc:
      read_from_svc(l,filename,lts_svc);
      return;
    case lts_fsm:
      switch ( extra.get_type() )
      {
        case le_mcrl1:
          read_from_fsm(l,filename,extra.get_mcrl1_spec());
          return;
        case le_mcrl2:
          read_from_fsm(l,filename,extra.get_mcrl2_spec());
          return;
        default:
          read_from_fsm(l,filename);
          return;
      }
    case lts_dot:
      read_from_dot(l,filename);
      return;
#ifdef USE_BCG
    case lts_bcg:
      read_from_bcg(l,filename);
      return;
#endif
    default:
      throw runtime_error ("Unknown source LTS type");
  }
}

void read_from(lts &l,istream &is, lts_type type, lts_extra extra)
{
  if ( type == lts_none )
  {
    type = detect_type(is);
    if ( type == lts_none )
    { throw mcrl2::runtime_error("Could not determine type of input stream.");
    }
  }

  switch ( type )
  {
    case lts_aut:
       read_from_aut(l,is);
       return; 
    case lts_mcrl:
    case lts_mcrl2:
    case lts_svc:
      throw mcrl2::runtime_error("Cannot read SVC based files from streams");
    case lts_fsm:
      switch ( extra.get_type() )
      {
        case le_mcrl1:
           read_from_fsm(l,is,extra.get_mcrl1_spec());
           return; 
        case le_mcrl2:
           read_from_fsm(l,is,extra.get_mcrl2_spec());
           return; 
        default:
           read_from_fsm(l,is);
           return; 
      }
    case lts_dot:
       read_from_dot(l,is);
       return; 
#ifdef USE_BCG
    case lts_bcg:
      throw mcrl2::runtime_error("Cannot read BCG files from streams.");
#endif
    default:
      throw mcrl2::runtime_error("Unknown source LTS type.");
  }
}
} // namespace detail

void lts::write_to(string const& filename, lts_type type, lts_extra extra) const
{
  switch ( type )
  {
    case lts_aut:
      detail::write_to_aut(*this,filename);
      return;
    case lts_mcrl:
      detail::write_to_svc(*this,filename,lts_mcrl);
      return;
    case lts_mcrl2:
      if ( extra.get_type() == le_mcrl2 )
      {
         detail::write_to_svc(*this,filename,lts_mcrl2,extra.get_mcrl2_spec());
      } 
      else 
      {
         detail::write_to_svc(*this,filename,lts_mcrl2);
      }
      return;
    case lts_svc:
      detail::write_to_svc(*this,filename,lts_svc);
      return;
    case lts_fsm:
      switch ( extra.get_type() )
      {
        case le_mcrl1:
          detail::write_to_fsm(*this,filename,extra.get_mcrl1_spec());
          return;
        case le_mcrl2:
          detail::write_to_fsm(*this,filename,extra.get_mcrl2_spec());
          return;
        default:
          if ( this->type == lts_mcrl2 && extra_data != NULL )
          {
            detail::write_to_fsm(*this,filename,lts_mcrl2,ATLgetArgument(ATAgetArgument((ATermAppl) extra_data,1),0));
          } 
          else 
          {
            detail::write_to_fsm(*this,filename);
          }
          return;
      }
    case lts_dot:
      if ( extra.get_type() == le_dot )
      {
         detail::write_to_dot(*this,filename,extra.get_dot_options());
      } 
      else 
      {
        detail::lts_dot_options opts;
        string s("unknown");
        opts.name = &s;
        opts.print_states = false;
        detail::write_to_dot(*this,filename,opts);
      }
      return;
#ifdef USE_BCG
    case lts_bcg:
      detail::write_to_bcg(*this,filename);
      return;
#endif
    default:
      throw mcrl2::runtime_error("Unknown target LTS type.");
  }
}

void lts::write_to(ostream &os, lts_type type, lts_extra extra) const
{
  switch ( type )
  {
    case lts_aut:
       detail::write_to_aut(*this,os);
       return;
    case lts_mcrl:
    case lts_mcrl2:
    case lts_svc:
      throw mcrl2::runtime_error("Cannot write SVC based files to streams.");
    case lts_fsm:
      switch ( extra.get_type() )
      {
        case le_mcrl1:
           detail::write_to_fsm(*this, os,extra.get_mcrl1_spec());
           return;
        case le_mcrl2:
           detail::write_to_fsm(*this,os,extra.get_mcrl2_spec());
           return;
        default:
          if ( this->type == lts_mcrl2 && extra_data != NULL && !gsIsNil(ATAgetArgument((ATermAppl) extra_data,1)) )
          {
             detail::write_to_fsm(*this,os,lts_mcrl2,ATLgetArgument(ATAgetArgument((ATermAppl) extra_data,1),0));
          } 
          else 
          {
             detail::write_to_fsm(*this,os);
          }
          return;
      }
    case lts_dot:
      if ( extra.get_type() == le_dot )
      {
         detail::write_to_dot(*this,os,extra.get_dot_options());
      } 
      else 
      {
        detail::lts_dot_options opts;
        string s("unknown");
        opts.name = &s;
        opts.print_states = false;
        detail::write_to_dot(*this,os,opts);
      }
      return;
#ifdef USE_BCG
    case lts_bcg:
      throw mcrl2::runtime_error("Cannot write BCG files to streams.");
#endif
    default:
      throw mcrl2::runtime_error("Unknown target LTS type.");
  }
}

void lts::sort_transitions(transition_sort_style ts) 
{
  switch (ts)
  {
    case lbl_tgt_src:
      sort(transitions.begin(),transitions.end(),compare_transitions_lts);
      break;
    case src_lbl_tgt:
    default:
      sort(transitions.begin(),transitions.end(),compare_transitions_slt);
      break;
  }
}

unsigned int* lts::get_transition_indices() {
  unsigned int *A = (unsigned int*)malloc((nstates+1)*sizeof(unsigned int));
  if (A == NULL) 
  {
    throw mcrl2::runtime_error("Out of memory.");
  }
  unsigned int t = 0;
  A[0] = 0;
  for (unsigned int s = 1; s <= nstates; ++s) {
    while (t < num_transitions() && transitions[t].from() == s-1) {
      ++t;
    }
    A[s] = t;
  }
  return A;
}

unsigned int** lts::get_transition_pre_table()
{
  unsigned int **A = (unsigned int**)malloc(nlabels*sizeof(unsigned int*));
  if (A == NULL)
  {
    throw mcrl2::runtime_error("Out of memory.");
  }

  unsigned int t = 0;
  unsigned int s, l;
  for (l = 0; l < nlabels; ++l)
  {
    A[l] = (unsigned int*)malloc((nstates+1)*sizeof(unsigned int));
    if (A[l] == NULL)
    {
      throw mcrl2::runtime_error("Out of memory.");
    }

    A[l][0] = t;
    for (s = 1; s <= nstates; ++s)
    {
      while (t < num_transitions() && transitions[t].label() == l
          && transitions[t].to() == s-1)
      {
        ++t;
      }
      A[l][s] = t;
    }
  }
  return A;
}

string lts::state_value_str(unsigned int state) const
{
  assert(state < nstates);
  string s;
  if ( has_state_info() )
  {
    ATerm value = state_values[state];
    if ( ATisAppl(value) && !strcmp(ATgetName(ATgetAFun((ATermAppl) value)),"STATE") )
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

ATerm lts::label_value(unsigned int label) const
{
  assert(label < label_values.size());
  return label_values[label];
}

string lts::label_value_str(unsigned int label) const
{
  assert(label < nlabels);
  string s;
  if ( has_label_info() )
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

lts_type lts::get_type() const
{
  return type;
}

bool lts::has_state_parameters() const
{
  return has_state_info() && ( ( type == lts_mcrl2 ) ||
                         ( type == lts_mcrl ) ||
                         ( type == lts_dot ) ||
                         ( type == lts_fsm ) );
}

unsigned int lts::num_state_parameters() const
{
  if ( type == lts_mcrl2 )
  {
    if ( is_timed_pair((ATermAppl) state_values[0]) )
    {
      return ATgetArity(ATgetAFun(ATAgetArgument((ATermAppl) state_values[0],0)));
    } else {
      return ATgetArity(ATgetAFun((ATermAppl) state_values[0]));
    }
  } else if ( type == lts_mcrl || type == lts_fsm || type == lts_dot )
  {
    return ATgetLength((ATermList) state_values[0]);
  }

  assert(0);
  return 0;
}

ATerm lts::state_parameter_name(unsigned int idx) const
{
  if ( type == lts_mcrl2 )
  {
    if ( extra_data != NULL && !gsIsNil(ATAgetArgument((ATermAppl) extra_data,1)) )
    {
      return ATelementAt(ATLgetArgument(ATAgetArgument((ATermAppl) extra_data,1),0),idx);
    } else {
      char s[2+sizeof(unsigned int)*3];
      sprintf(s,"p%u",idx);
      return (ATerm) gsMakeDataVarId(gsString2ATermAppl(s),(ATermAppl) state_parameter_sort(idx));
    }
  } else if ( type == lts_mcrl )
  {
    char s[2+sizeof(unsigned int)*3];
    sprintf(s,"p%u",idx);
    return (ATerm) ATmakeAppl0(ATmakeAFun(s,0,ATtrue));
  } else if ( type == lts_fsm || type == lts_dot )
  {
    return ATgetArgument(ATAgetArgument(ATAelementAt((ATermList) state_values[0],idx),1),0);
  }

  assert(0);
  return NULL;
}

std::string lts::state_parameter_name_str(unsigned int idx) const
{
  if ( type == lts_mcrl2 || type == lts_mcrl )
  {
    if ( extra_data != NULL && !gsIsNil(ATAgetArgument((ATermAppl) extra_data,1)) )
    {
      return ATgetName(ATgetAFun(ATAgetArgument(ATAelementAt(ATLgetArgument(ATAgetArgument((ATermAppl) extra_data,1),0),idx),0)));
    } else {
      char s[2+sizeof(unsigned int)*3];
      sprintf(s,"p%u",idx);
      return s;
    }
  } else if ( type == lts_fsm || type == lts_dot )
  {
    return ATgetName(ATgetAFun((ATermAppl) state_parameter_name(idx)));
  }

  assert(0);
  return "";
}

ATerm lts::state_parameter_sort(unsigned int idx) const
{
  if ( type == lts_mcrl2 )
  {
    return (ATerm) static_cast<ATermAppl>(data::data_expression(get_state_parameter_value(0,idx)).sort());
  } else if ( type == lts_mcrl )
  {
    char s[2+sizeof(unsigned int)*3];
    sprintf(s,"D%u",idx);
    return (ATerm) ATmakeAppl0(ATmakeAFun(s,0,ATtrue));
  } else if ( type == lts_fsm || type == lts_dot )
  {
    return ATgetArgument(ATAgetArgument(ATAelementAt((ATermList) state_values[0],idx),1),1);
  }

  assert(0);
  return NULL;
}

std::string lts::state_parameter_sort_str(unsigned int idx) const
{
  if ( type == lts_mcrl2 )
  {
    return PrintPart_CXX(state_parameter_sort(idx),ppDefault);
  } else if ( type == lts_mcrl )
  {
    char s[2+sizeof(unsigned int)*3];
    sprintf(s,"D%u",idx);
    return s;
  } else if ( type == lts_fsm || type == lts_dot )
  {
    return ATgetName(ATgetAFun((ATermAppl) state_parameter_sort(idx)));
  }

  assert(0);
  return "";
}

ATerm lts::get_state_parameter_value(unsigned int state, unsigned int idx) const
{
  if ( type == lts_mcrl2 )
  {
    return ATgetArgument((ATermAppl) state_values[state],idx);
  } else if ( type == lts_mcrl )
  {
    return ATelementAt((ATermList) state_values[state],idx);
  } else if ( type == lts_fsm || type == lts_dot )
  {
    return ATgetArgument(ATAelementAt((ATermList) state_values[state],idx),0);
  }

  assert(0);
  return NULL;
}

std::string lts::get_state_parameter_value_str(unsigned int state, unsigned int idx) const
{
  if ( type == lts_mcrl2 )
  {
    return PrintPart_CXX(get_state_parameter_value(state,idx),ppDefault);
  } else if ( type == lts_mcrl )
  {
    return ATwriteToString(get_state_parameter_value(state,idx));
  } else if ( type == lts_fsm || type == lts_dot )
  {
    std::string s = ATwriteToString(get_state_parameter_value(state,idx));
    return s.substr(1,s.size()-2);
  }

  assert(0);
  return "";
}

atermpp::set<ATerm> lts::get_label_values() const
{
  atermpp::set<ATerm> r;

  for (unsigned int i=0; i<nlabels; i++)
  {
    r.insert(label_values[i]);
  }

  return r;
}

atermpp::set<ATerm> lts::get_state_values() const
{
  atermpp::set<ATerm> r;

  for (unsigned int i=0; i<nstates; i++)
  {
    r.insert(state_values[i]);
  }

  return r;
}

atermpp::set<ATerm> lts::get_state_parameter_values(unsigned int idx)
{
  atermpp::set<ATerm> r;

  for (unsigned int i=0; i<nstates; i++)
  {
    r.insert(get_state_parameter_value(i,idx));
  }

  return r;
}

std::string lts::pretty_print_label_value(ATerm value)
{
  if ( type == lts_mcrl2 )
  {
    return PrintPart_CXX(value,ppDefault);
  } else if ( type == lts_mcrl || type == lts_fsm || type == lts_dot )
  {
    std::string s = ATwriteToString(value);
    return s.substr(1,s.size()-2);
  }

  return ATwriteToString(value);
}

std::string lts::pretty_print_state_value(ATerm value) const
{
  if ( type == lts_mcrl2 )
  {
    std::string s = "[";
    for (unsigned int i=0; i<ATgetArity(ATgetAFun((ATermAppl) value)); i++)
    {
      if ( i > 0 )
      {
        s += ",";
      }
      s += PrintPart_CXX(ATgetArgument(value,i),ppDefault);
    }
    return s+"]";
  }

  return ATwriteToString(value);
}

std::string lts::pretty_print_state_parameter_value(ATerm value)
{
  if ( type == lts_mcrl2 )
  {
    return PrintPart_CXX(value,ppDefault);
  } else if ( type == lts_fsm || type == lts_dot )
  {
    std::string s = ATwriteToString(value);
    return s.substr(1,s.size()-2);
  }

  return ATwriteToString(value);
}

ATerm lts::get_extra_data()
{
  return extra_data;
}

void lts::set_extra_data(ATerm data)
{
  extra_data = data;
}

bool lts::has_data_specification()
{
  return extra_data != NULL &&
         ATisAppl(extra_data) &&
         !strcmp("mCRL2LTS1",ATgetName(ATgetAFun((ATermAppl) extra_data))) &&
         !gsIsNil(ATAgetArgument((ATermAppl) extra_data,0));
}

data::data_specification lts::get_data_specification()
{
  assert(has_data_specification());

  return data::data_specification(ATAgetArgument((ATermAppl) extra_data,0));
}

void lts::set_data_specification(data::data_specification const& spec)
{
  assert( type == lts_mcrl2 );

  if ( extra_data == NULL )
  {
    extra_data = (ATerm) ATmakeAppl3(ATmakeAFun("mCRL2LTS1",3,ATfalse),(ATerm)(ATermAppl) mcrl2::data::detail::data_specification_to_aterm_data_spec(spec), (ATerm) gsMakeNil(), (ATerm) gsMakeNil());
  } else {
    extra_data = (ATerm) ATsetArgument((ATermAppl) extra_data,(ATerm)(ATermAppl) mcrl2::data::detail::data_specification_to_aterm_data_spec(spec),0);
  }
}

void lts::remove_state_values()
{
  state_values=atermpp::vector<ATerm>();
  
  if ( type == lts_mcrl2 && extra_data != NULL )
  {
    extra_data = (ATerm) ATsetArgument((ATermAppl) extra_data,(ATerm) gsMakeNil(),1);
  }
}

bool reachability_check(lts &l, bool remove_unreachable)
{
  // First calculate which states can be reached, and store this in the array visited.
  const outgoing_transitions_per_state_t out_trans=transitions_per_outgoing_state(l.get_transitions());

  std::vector < bool > visited(l.num_states(),false);
  visited[l.initial_state()]=true;
  std::stack<unsigned int> todo;
  todo.push(l.initial_state());

  while (!todo.empty())
  {
    unsigned int state_to_consider=todo.top();
    todo.pop();
    for (outgoing_transitions_per_state_t::const_iterator i=out_trans.lower_bound(state_to_consider); 
               i!=out_trans.upper_bound(state_to_consider); ++i) 
    { 
      assert(from(i)<l.num_states() && to(i)<l.num_states());
      if ( visited[from(i)] && !visited[to(i)])
      { 
        visited[to(i)]=true;
        todo.push(to(i));
      }
    }
  }
  
  // Property: in_visited(s) == true: state s is reachable from the initial state

  // check to see if all states are reachable from the initial state, i.e. 
  // whether all bits are set.
  bool all_reachable = find(visited.begin(),visited.end(),false)==visited.end();

  if ( !all_reachable && remove_unreachable )
  {
    // Remove all unreachable states, transitions from such states and labels
    // that are only used in these transitions.

    std::map < unsigned int , unsigned > state_map;
    std::map < unsigned int , unsigned > label_map;
    
    lts new_lts; 

    unsigned int new_nstates = 0;
    for (unsigned int i=0; i<l.num_states(); i++)
    {
      if ( visited[i] )
      {
        state_map[i] = new_nstates;
        new_lts.add_state(l.has_state_info()?l.state_value(i):NULL);
        new_nstates++;
      }
    }

    for (transition_const_range r=l.get_transitions(); !r.empty(); r.advance_begin(1) )
    { const transition t=r.front();
      if ( visited[t.from()] )
      { 
        label_map[t.label()] = 1;
      }
    }

    unsigned int new_nlabels = 0;
    for (unsigned int i=0; i<l.num_labels(); i++)
    {
      if (label_map.count(i)>0)   // Label i is used.
      { 
        label_map[i] = new_nlabels;
        new_lts.add_label(l.has_label_info()?l.label_value(i):NULL,l.is_tau(i));
        new_nlabels++;
      }
    }

    for (transition_const_range r=l.get_transitions(); !r.empty(); r.advance_begin(1) )
    { 
      const transition t=r.front();
      if (visited[t.from()])
      { new_lts.add_transition(transition(state_map[t.from()],label_map[t.label()],state_map[t.to()]));
      }
    }

    new_lts.set_initial_state(state_map[l.initial_state()]);
    l.swap(new_lts);
  }

  return all_reachable;
}

namespace detail
{
lts_type guess_format(string const& s) {
  string::size_type pos = s.find_last_of('.');

  if ( pos != string::npos )
  {
    string ext = s.substr(pos+1);

    if ( ext == "aut" )
    {
      if (core::gsVerbose)
      { std::cerr << "Detected Aldebaran extension.\n";
      }
      return lts_aut;
    } else if ( ext == "lts" )
    {
      if (core::gsVerbose)
      { std::cerr << "Detected mCRL2 extension.\n";
      }
      return lts_mcrl2;
    } else if ( ext == "svc" )
    {
      if (core::gsVerbose)
      { std::cerr << "Detected SVC extension; assuming mCRL format.\n";
      }
      return lts_mcrl;
    } else if ( ext == "fsm" )
    {
      if (core::gsVerbose)
      { std::cerr << "Detected Finite State Machine extension.\n";
      }
      return lts_fsm;
    } else if ( ext == "dot" )
    {
      if (core::gsVerbose)
      { std::cerr << "Detected GraphViz extension.\n";
      }
      return lts_dot;
#ifdef USE_BCG
    } else if ( ext == "bcg" )
    {
      if (core::gsVerbose)
      { std::cerr << "Detected Binary Coded Graph extension.\n";
      }
      return lts_bcg;
#endif
    }
  }

  return lts_none;
}

static std::string type_strings[] = { "unknown", "mcrl2", "aut", "mcrl", "svc", "fsm", "dot", "bcg" };

static std::string type_desc_strings[] = { "unknown LTS format",
                                           "mCRL2 LTS format",
                                           "Aldebaran format (CADP)",
                                           "mCRL SVC format",
                                           "(generic) SVC format",
                                           "Finite State Machine format",
                                           "GraphViz format",
                                           "Binary Coded Graph format (CADP)" };

static std::string extension_strings[] = { "", "lts", "aut", "svc", "svc", "fsm", "dot", "bcg" };

static std::string mime_type_strings[] = { "", "application/lts", "text/aut", "application/svc+mcrl", "application/svc", "text/fsm", "text/dot", "application/bcg" };

lts_type parse_format(std::string const& s) {
  if ( s == "aut" )
  {
    return lts_aut;
  } else if ( s == "mcrl" || s == "svc+mcrl")
  {
    return lts_mcrl;
  } else if ( s == "mcrl2" || s == "lts")
  {
    return lts_mcrl2;
  } else if ( s == "svc" )
  {
    return lts_svc;
  } else if ( s == "fsm" )
  {
    return lts_fsm;
  } else if ( s == "dot" )
  {
    return lts_dot;
#ifdef USE_BCG
  } else if ( s == "bcg" )
  {
    return lts_bcg;
#endif
  }

  return lts_none;
}

std::string string_for_type(const lts_type type) {
  return (type_strings[type]);
}

std::string extension_for_type(const lts_type type) {
  return (extension_strings[type]);
}

std::string mime_type_for_type(const lts_type type) {
  return (mime_type_strings[type]);
}


void add_extra_mcrl2_svc_data(std::string const &filename, ATermAppl data_spec, ATermList params, ATermList act_labels)
{
  FILE *f = fopen(filename.c_str(),"ab");
  if ( f == NULL )
  {
    throw mcrl2::runtime_error("Could not open file '" + filename + "' to add extra LTS information.");
    return;
  }

  ATerm arg1 = (ATerm) ((data_spec == NULL)?gsMakeNil():data_spec);
  ATerm arg2 = (ATerm) ((params == NULL)?gsMakeNil():ATmakeAppl1(ATmakeAFun("ParamSpec",1,ATfalse),(ATerm) params));
  ATerm arg3 = (ATerm) ((ATisEmpty(act_labels))?gsMakeNil():core::detail::gsMakeActSpec(act_labels));
  ATerm data = (ATerm) ATmakeAppl3(ATmakeAFun("mCRL2LTS1",3,ATfalse),arg1,arg2,arg3);

  long position;
  if ( (position = ftell(f)) == -1 )
  {
    fclose(f);
    throw mcrl2::runtime_error("Could not determine file size of '" + filename + 
                          "'; not adding extra information.");
    return;
  }

  if ( ATwriteToBinaryFile(data,f) == ATfalse )
  {
    fclose(f);
    throw mcrl2::runtime_error("Error writing extra LTS information to '" + filename + 
               "', file could be corrupted.");
    return;
  }

  unsigned char buf[8+12+1] = "XXXXXXXX   1STL2LRCm";
  for (unsigned int i=0; i<8; i++)
  {
    buf[i] = position % 0x100;
    position /= 0x100;
  }
  if ( fwrite(buf,1,8+12,f) != 8+12 )
  {
    fclose(f);
    throw mcrl2::runtime_error("error writing extra LTS information to '" + filename + 
                     "', file could be corrupted.");
    return;
  }

  fclose(f);
}

static const std::set<lts_type> &initialise_supported_lts_formats()
{
  static std::set<lts_type> s;
  for (unsigned int i = lts_type_min; i<1+(unsigned int)lts_type_max; ++i)
  {
    if ( lts_none != (lts_type) i )
    {
      s.insert((lts_type) i);
    }
  }
  return s;
}
const std::set<lts_type> &supported_lts_formats()
{
  static const std::set<lts_type> &s = initialise_supported_lts_formats();
  return s;
}

std::string supported_lts_formats_text(lts_type default_format, const std::set<lts_type> &supported)
{
  vector<lts_type> types(supported.begin(),supported.end());
  std::sort(types.begin(),types.end(),boost::bind(lts_named_cmp<lts_type>,type_strings,_1,_2));

  string r;
  for (vector<lts_type>::iterator i=types.begin(); i!=types.end(); i++)
  {
    r += "  '" + type_strings[*i] + "' for the " + type_desc_strings[*i];

    if ( *i == default_format )
    {
      r += " (default)";
    }


    if ( i+2 == types.end() )
    {
      r += ", or\n";
    } else if ( i+1 != types.end() )
    {
      r += ",\n";
    }
  }

  return r;
}

std::string supported_lts_formats_text(const std::set<lts_type> &supported)
{
  return supported_lts_formats_text(lts_none,supported);
}

std::string lts_extensions_as_string(const std::string &sep, const std::set<lts_type> &supported)
{
  vector<lts_type> types(supported.begin(),supported.end());
  std::sort(types.begin(),types.end(),boost::bind(lts_named_cmp<lts_type>,extension_strings,_1,_2));

  string r, prev;
  bool first = true;
  for (vector<lts_type>::iterator i=types.begin(); i!=types.end(); i++)
  {
    if ( extension_strings[*i] == prev ) // avoid mentioning extensions more than once
    {
      continue;
    }
    if ( first )
    {
      first = false;
    } else {
      r += sep;
    }
    r += "*." + extension_strings[*i];
    prev = extension_strings[*i];
  }

  return r;
}

std::string lts_extensions_as_string(const std::set<lts_type> &supported)
{
  return lts_extensions_as_string(",",supported);
}
} // namespace detail

ATermList sorted_insert(ATermList l,ATermAppl t)
{ 
  if (l==ATempty)
  { return ATinsert(l,(ATerm)t);
  }

  if (ATgetName(ATgetAFun(ATgetArgument(t,0)))<
         ATgetName(ATgetAFun(ATgetArgument(ATgetFirst(l),0))))
  { return ATinsert(l,(ATerm)t);
  }
  if ((ATgetName(ATgetAFun(ATgetArgument(t,0)))==
         ATgetName(ATgetAFun(ATgetArgument(ATgetFirst(l),0))))
       &&
      (ATgetArgument(t,1)< ATgetArgument(ATgetFirst(l),1)))
  { return ATinsert(l,(ATerm)t);
  }
  return ATinsert(sorted_insert(ATgetNext(l),t),ATgetFirst(l));
}

ATerm sort_multi_action(ATerm ma)
{ ATermList tl=ATLgetArgument((ATermAppl)(ma),0); //get the multi_action_list
  ATermList result=ATempty;
  for( ; tl!=ATempty ; tl=ATgetNext(tl))
  { result=sorted_insert(result,ATAgetFirst(tl));
  }
  return (ATerm)gsMakeMultAct(result);
}


bool lts::hide_actions(const std::vector<std::string> &tau_actions)
{ 
  if (tau_actions.size()==0) return true; // Nothing needs to be hidden.

  for(unsigned int i=0; i< num_labels(); ++i)
  { 
    string s=label_value_str(i);
    stringstream ss(s); 
    ATermAppl t=parse_mult_act(ss);
    
    if ( t == NULL )
    {
       std::cerr << "Cannot reconstruct multi action " << s << " (parsing)\n";
       return false;
    }

    ATermList new_multi_action=ATempty;
    for(ATermList mas=ATLgetArgument(t,0); mas!=ATempty; mas=ATgetNext(mas))
    { ATermAppl multiaction=ATAgetFirst(mas);
      
      if (std::find(tau_actions.begin(),tau_actions.end(),
                 string(ATgetName(ATgetAFun(ATgetArgument(multiaction,0)))))==tau_actions.end())  // this action must not be hidden.
      { new_multi_action=ATinsert(new_multi_action,(ATerm)multiaction);
      }
    }
    new_multi_action=ATreverse(new_multi_action);
    set_label_value(i, (ATerm)gsMakeMultAct(new_multi_action),new_multi_action==ATempty); // indicate that label i is now a tau label
  }

  // sort the multi-actions
  for(unsigned int i=0; i<num_labels(); ++i)
  { set_label_value(i, sort_multi_action(label_value(i)),is_tau(i));
  }

  // Now the labels have been adapted to the hiding operator. Check now whether labels
  // did become equal.

  map < unsigned int, unsigned int> map_multiaction_indices;
  for(unsigned int i=0; i<num_labels(); ++i)
  { for (unsigned int j=0; j!=i; ++j)
    { if (label_value(i)==label_value(j))  
      { assert(map_multiaction_indices.count(i)==0);
        map_multiaction_indices.insert(pair<unsigned int, unsigned int>(i,j));
        break;
      }
    }
  }

  // If labels became equal, take care they get equal numbers in the transition
  // system, because all behavioural reduction algorithms only compare the labels.
  if (!map_multiaction_indices.empty())
  { 
    for (transition_range r=get_transitions(); !r.empty(); r.advance_begin(1))
    { 
      transition &t=r.front();
      if (map_multiaction_indices.count(t.label())>0)
      { t.set_label(map_multiaction_indices[t.label()]);
      }
    }
  }
  return true;
}

}
}
