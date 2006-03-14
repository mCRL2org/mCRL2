#include <string>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <svc/svc.h>
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "libprint.h"
#include "liblts.h"

#define ATisAppl(x) (ATgetType(x) == AT_APPL)
#define ATisList(x) (ATgetType(x) == AT_LIST)

namespace mcrl2
{
namespace lts
{

AFun timed_pair;
unsigned int timed_pair_set = 0;

#define isTimedPair(x) (ATisEqualAFun(ATgetAFun(x),timed_pair))

lts::lts()
{
  init();
}

lts::lts(std::string &filename, lts_type type)
{
  init();
  read_from(filename,type);
}

lts::lts(std::istream &is, lts_type type)
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

  free(states);
  free(state_values);
  free(labels);
  free(label_values);
  free(transitions);

  timed_pair--;
  if ( timed_pair_set == 0 )
  {
    ATunprotectAFun(timed_pair);
  }
}

void p_lts::init()
{
  if ( timed_pair_set == 0 )
  {
    timed_pair = ATmakeAFun("pair",2,ATfalse);
    ATprotectAFun(timed_pair);
  }
  timed_pair_set++;

  states_size = 0;
  nstates = 0;
  states = NULL;
  state_values = NULL;

  labels_size = 0;
  nlabels = 0;
  labels = NULL;
  label_values = NULL;

  transitions_size = 0;
  ntransitions = 0;
  transitions = NULL;
  
  type = lts_mcrl2;
  state_info = true;
  label_info = true;
}

void p_lts::clear()
{
  free(states);
  free(state_values);
  free(labels);
  free(label_values);
  free(transitions);

  init();
}

lts_type p_lts::detect_type(std::string &filename)
{
  std::ifstream is(filename.c_str(),std::ifstream::in|std::ifstream::binary);
  if ( !is.is_open() )
  {
    gsVerboseMsg("cannot open file '%s' for reading\n",filename.c_str());
    return lts_none;
  }

  lts_type t = detect_type(is);

  is.close();

  if ( t == lts_none )
  {
    SVCfile f;
    SVCbool b;

    char *s = strdup(filename.c_str());
    if ( SVCopen(&f,s,SVCread,&b) )
    {
      gsVerboseMsg("cannot detect file type of '%s'\n",filename.c_str());
      return lts_none;
    }
    free(s);

    std::string svc_type = SVCgetType(&f);
    
    if ( svc_type == "generic" )
    {
      gsVerboseMsg("detected mCRL input file\n");
      t = lts_mcrl;
    } else if ( (svc_type == "mCRL2") || (svc_type == "mCRL2+info") )
    {
      gsVerboseMsg("detected mCRL2 input file\n");
      t = lts_mcrl2;
    } else {
      gsVerboseMsg("detected SVC input file\n");
      t = lts_svc;
    }

    SVCclose(&f);
  }

  return t;
}

lts_type p_lts::detect_type(std::istream &is)
{
  std::streampos pos = is.tellg();
  char buf[5];
  std::streamsize r = is.readsome(buf,5);
  is.seekg(pos);

  if ( (r == 5) && !strncmp(buf,"des (",5) )
  {
    gsVerboseMsg("detected AUT input file\n");
    return lts_aut;
  }

  return lts_none;
}

bool p_lts::read_from_svc(std::string &filename, lts_type type)
{
  SVCfile f;
  SVCbool b;

  char *fn = strdup(filename.c_str());
  if ( SVCopen(&f,fn,SVCread,&b) )
  {
    gsVerboseMsg("cannot open SVC file '%s' for reading\n",filename.c_str());
    return false;
  }

  creator = SVCgetCreator(&f);
  
  std::string svc_type = SVCgetType(&f);
  if ( type == lts_mcrl )
  {
    if ( svc_type != "generic" )
    {
      gsVerboseMsg("SVC file '%s' is not in the mCRL format\n",filename.c_str());
      free(fn);
      return false;
    }
    state_info = !((bool) SVCgetIndexFlag(&f));
  } else if ( type == lts_mcrl2 )
  {
    if ( svc_type == "mCRL2" )
    {
      state_info = false;
    } else if ( svc_type == "mCRL2+info" )
    {
      state_info = true;
    } else {
      gsVerboseMsg("SVC file '%s' is not in the mCRL2 format\n",filename.c_str());
      free(fn);
      return false;
    }
  } else {
    if ( type == lts_mcrl )
    {
      gsVerboseMsg("SVC file '%s' is not in the mCRL format\n",filename.c_str());
      free(fn);
      return false;
    } else if ( type == lts_mcrl2 )
    {
      gsVerboseMsg("SVC file '%s' is not in the mCRL2 format\n",filename.c_str());
      free(fn);
      return false;
    }
    state_info = !((bool) SVCgetIndexFlag(&f));
  }

  free(fn);

  label_info = true;

  init_state = (unsigned int) SVCgetInitialState(&f);

  SVCstateIndex from, to;
  SVClabelIndex label;
  SVCparameterIndex param;
  while ( SVCgetNextTransition(&f,&from,&label,&to,&param) )
  {
    unsigned int max = (unsigned int) ((from > to)?from:to);
    for (unsigned int i=nstates; i<=max; i++)
    {
      if ( state_info )
      {
        p_add_state(SVCstate2ATerm(&f,(SVCstateIndex) i));
      } else {
        p_add_state();
      }
    }

    for (unsigned int i=nlabels; i<=((unsigned int) label); i++)
    {
      p_add_label(SVClabel2ATerm(&f,(SVClabelIndex) i));
    }

    p_add_transition((unsigned int) from,
                     (unsigned int) label,
                     (unsigned int) to);
  }

  SVCclose(&f);

  this->type = type;

  return true;
}

bool p_lts::read_from_aut(std::string &filename)
{
  std::ifstream is(filename.c_str());

  if ( !is.is_open() )
  {
    gsVerboseMsg("cannot open AUT file '%s' for reading\n",filename.c_str());
    return false;
  }

  bool r = read_from_aut(is);

  is.close();

  return r;
}

bool p_lts::read_from_aut(std::istream &is)
{
  unsigned int ntrans,nstate;
  char buf[1024];
  
  is.getline(buf,1024);
  sscanf(buf,"des (%u,%u,%u)",&init_state,&ntrans,&nstate);

  for (unsigned int i=0; i<nstate; i++)
  {
    p_add_state();
  }

  ATermIndexedSet labs = ATindexedSetCreate(100,50);
  while ( !is.eof() )
  {
    unsigned int from,to;
    char s[1024];

    is.getline(buf,1024);
    sscanf(buf,"(%u,\"%[^\"]\",%u)",&from,s,&to);
    int label;
    ATerm t = (ATerm) ATmakeAppl(ATmakeAFun(s,0,ATtrue));
    if ( (label =  ATindexedSetGetIndex(labs,t)) < 0 )
    {
      ATbool b;
      label = ATindexedSetPut(labs,t,&b);
      p_add_label(t);
    }
    p_add_transition(from,(unsigned int) label,to);
  }

  this->type = lts_aut;

  return true;
}

bool lts::read_from(std::string &filename, lts_type type)
{
  clear();
  if ( type == lts_none )
  {
    type = detect_type(filename);
    if ( type == lts_none )
    {
      return false;
    }
  }

  switch ( type )
  {
    case lts_mcrl2:
      return read_from_svc(filename,lts_mcrl2);
    case lts_aut:
      return read_from_aut(filename);
    case lts_mcrl:
      return read_from_svc(filename,lts_mcrl);
      break;
    case lts_svc:
      return read_from_svc(filename,lts_svc);
    default:
      assert(0);
      gsVerboseMsg("unknown source LTS type\n");
      return false;
  }
}

bool lts::read_from(std::istream &is, lts_type type)
{
  clear();
  if ( type == lts_none )
  {
    type = detect_type(is);
    if ( type == lts_none )
    {
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
    default:
      assert(0);
      gsVerboseMsg("unknown source LTS type\n");
      return false;
  }
}

bool p_lts::write_to_svc(std::string &filename, lts_type type)
{
  if ( type == lts_mcrl )
  {
    if ( state_info )
    {
      for (unsigned int i=0; i<nstates; i++)
      {
        if ( !ATisList(state_values[i]) )
        {
          gsVerboseMsg("cannot save LTS in mCRL format; state values are incompatible\n");
          return false;
        }
      }
    }
    if ( !label_info )
    {
      gsVerboseMsg("cannot save LTS in mCRL format; labels have no value\n");
      return false;
    }
    for (unsigned int i=0; i<nlabels; i++)
    {
      if ( !ATisAppl(label_values[i]) || (ATgetArity(ATgetAFun((ATermAppl) label_values[i])) != 0) )
      {
        gsVerboseMsg("cannot save LTS in mCRL format; label values are incompatible\n");
        return false;
      }
    }
  } else if ( type == lts_mcrl2 )
  {
    if ( state_info )
    {
      for (unsigned int i=0; i<nstates; i++)
      {
        if ( !ATisAppl(state_values[i]) ) // XXX check validity of data terms
        {
          gsVerboseMsg("cannot save LTS in mCRL2 format; state values are incompatible\n");
          return false;
        }
      }
    }
    if ( !label_info )
    {
      gsVerboseMsg("cannot save LTS in mCRL2 format; labels have no value\n");
      return false;
    }
    for (unsigned int i=0; i<nlabels; i++)
    {
      if ( !ATisAppl(label_values[i]) || !(gsIsMultAct((ATermAppl) label_values[i]) || isTimedPair((ATermAppl) label_values[i]) ) )
      {
        gsVerboseMsg("cannot save LTS in mCRL2 format; label values are incompatible\n");
        return false;
      }
    }
  } else {
    gsVerboseMsg("saving SVC as type 'unknown'\n");
  }

  SVCfile f;
  SVCbool b = state_info ? SVCfalse : SVCtrue;
  char *fn = strdup(filename.c_str());
  if ( SVCopen(&f,fn,SVCwrite,&b) )
  {
    gsVerboseMsg("cannot open SVC file '%s' for writing\n",fn);
    free(fn);
    return false;
  }
  free(fn);

  if ( type == lts_mcrl )
  {
    SVCsetType(&f,"generic");
  } else if ( type == lts_mcrl2 )
  {
    if ( state_info )
    {
      SVCsetType(&f,"mCRL2+info");
    } else {
      SVCsetType(&f,"mCRL2");
    }
  } else {
      SVCsetType(&f,"unknown");
  }

  if ( creator == "" )
  {
    SVCsetCreator(&f,"liblts (mCRL2)");
  } else {
    char *s = strdup(creator.c_str());
    SVCsetCreator(&f,s);
    free(s);
  }

  SVCsetInitialState(&f,SVCnewState(&f, state_info ? state_values[init_state] : (ATerm) ATmakeInt(init_state) ,&b));

  SVCparameterIndex param = SVCnewParameter(&f,(ATerm) ATmakeList0(),&b);
  for (unsigned int i=0; i<ntransitions; i++)
  {
    SVCstateIndex from = SVCnewState(&f, state_info ? state_values[transitions[i].from] : (ATerm) ATmakeInt(transitions[i].from) ,&b);
    SVClabelIndex label = SVCnewLabel(&f, label_info ? label_values[transitions[i].label] : (ATerm) ATmakeInt(transitions[i].label) ,&b);
    SVCstateIndex to = SVCnewState(&f, state_info ? state_values[transitions[i].to] : (ATerm) ATmakeInt(transitions[i].to) ,&b);
    SVCputTransition(&f,from,label,to,param);
  }

  SVCclose(&f);

  return true;
}

bool p_lts::write_to_aut(std::string &filename)
{
  std::ofstream os(filename.c_str());

  if ( !os.is_open() )
  {
    gsVerboseMsg("cannot open AUT file '%s' for writing\n",filename.c_str());
    return false;
  }

  write_to_aut(os);

  os.close();

  return true;
}

bool p_lts::write_to_aut(std::ostream &os)
{
  os << "des (" << init_state << "," << ntransitions << "," << nstates << ")" << std::endl;

  for (unsigned int i=0; i<ntransitions; i++)
  {
    os << "(" << transitions[i].from << ",";
    if ( label_info )
    {
      ATerm label = label_values[transitions[i].label];
      if ( ATisAppl(label) && gsIsMultAct((ATermAppl) label) )
      {
        os << "\"";
        PrintPart_CXX(os,label,ppDefault);
        os << "\"";
      } else if ( ATisAppl(label) && isTimedPair((ATermAppl) label) )
      {
        os << "\"";
        PrintPart_CXX(os,ATgetArgument((ATermAppl) label,0),ppDefault);
        os << "\"";
      } else {
        os << ATwriteToString(label);
      }
    } else {
      os << transitions[i].label;
    }
    os << "," << transitions[i].to << ")" << std::endl;
  }

  return true;
}

bool lts::write_to(std::string &filename, lts_type type)
{
  switch ( type )
  {
    case lts_mcrl2:
      return write_to_svc(filename,lts_mcrl2);
    case lts_aut:
      return write_to_aut(filename);
    case lts_mcrl:
      return write_to_svc(filename,lts_mcrl);
      break;
    case lts_svc:
      return write_to_svc(filename,lts_svc);
    default:
      assert(0);
      gsVerboseMsg("unknown target LTS type\n");
      return false;
  }
}

bool lts::write_to(std::ostream &os, lts_type type)
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

    assert(state_info == (value != NULL));

    states = (unsigned int *) realloc(states,new_states_size*sizeof(unsigned int));
    if ( state_info )
    {
      if ( state_values != NULL )
      {
        ATunprotectArray(state_values);
      }
      state_values = (ATerm *) realloc(state_values,new_states_size*sizeof(ATerm));
      for (unsigned int i=states_size; i<new_states_size; i++)
      {
        state_values[i] = NULL;
      }
      ATprotectArray(state_values,new_states_size);
    }
    states_size = new_states_size;
  }

  states[nstates] = nstates;
  if ( state_info )
  {
    state_values[nstates] = value;
  }

  return nstates++;
}

unsigned int lts::add_label(ATerm value)
{
  return p_add_label(value);
}

unsigned int p_lts::p_add_label(ATerm value)
{
  if ( nlabels == labels_size )
  {
    unsigned int new_labels_size = labels_size*2;
    if ( labels_size == 0 )
    {
      label_info = (value != NULL);
      new_labels_size = 128;
    }

    assert(label_info == (value != NULL));

    labels = (unsigned int *) realloc(labels,new_labels_size*sizeof(unsigned int));
    if ( label_info )
    {
      if ( label_values != NULL )
      {
        ATunprotectArray(label_values);
      }
      label_values = (ATerm *) realloc(label_values,new_labels_size*sizeof(ATerm));
      for (unsigned int i=labels_size; i<new_labels_size; i++)
      {
        label_values[i] = NULL;
      }
      ATprotectArray(label_values,new_labels_size);
    }
    labels_size = new_labels_size;
  }

  labels[nlabels] = nlabels;
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

    transitions = (transition *) realloc(transitions,new_transitions_size*sizeof(transition));
    transitions_size = new_transitions_size;
  }

  transitions[ntransitions].from = from;
  transitions[ntransitions].label = label;
  transitions[ntransitions].to = to;

  return ntransitions++;
}

void lts::set_state(unsigned int state, ATerm value)
{
  assert(state_info && (value != NULL));
  state_values[state] = value;
}

void lts::set_label(unsigned int label, ATerm value)
{
  assert(label_info && (value != NULL));
  label_values[label] = value;
}

ATerm lts::state_value(unsigned int state)
{
  assert(state_info && (state < nstates));
  return state_values[state];
}

ATerm lts::label_value(unsigned int label)
{
  assert(label_info && (label < nlabels));
  return label_values[label];
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

bool lts::has_creator()
{
  return creator.empty();
}

std::string lts::get_creator()
{
  return creator;
}

void lts::set_creator(std::string creator)
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
  return l->states[pos];
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
  return l->labels[pos];
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


}
}
