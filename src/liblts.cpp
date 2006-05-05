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
#include "setup.h"

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

lts::lts()
{
  init();
}

lts::lts(string &filename, lts_type type)
{
  init();
  read_from(filename,type);
}

lts::lts(istream &is, lts_type type)
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
  free(taus);
  free(labels);
  free(label_values);
  free(transitions);
}

void p_lts::init()
{
  states_size = 0;
  nstates = 0;
  states = NULL;
  state_values = NULL;

  labels_size = 0;
  nlabels = 0;
  labels = NULL;
  taus = NULL;
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
  free(taus);
  free(labels);
  free(label_values);
  free(transitions);

  init();
}

lts_type p_lts::detect_type(string &filename)
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
      if ( valid && is_valid )
      {
        return s;
      } else {
        valid = false;
        return "";
      }
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

bool p_lts::read_from_svc(string &filename, lts_type type)
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
  
  string svc_type = SVCgetType(&f);
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
    if ( svc_type == "generic" )
    {
      gsVerboseMsg("SVC file '%s' is in the mCRL format\n",filename.c_str());
      free(fn);
      return false;
    } else if ( (svc_type == "mCRL2") || (svc_type == "mCRL2+info") )
    {
      gsVerboseMsg("SVC file '%s' is in the mCRL2 format\n",filename.c_str());
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
      if ( type == lts_mcrl )
      {
        ATermAppl lab = (ATermAppl) SVClabel2ATerm(&f,(SVClabelIndex) i);
        p_add_label((ATerm) lab,!strcmp(ATgetName(ATgetAFun(lab)),"tau"));
      } else if ( type == lts_mcrl2 )
      {
        ATermAppl lab = (ATermAppl) SVClabel2ATerm(&f,(SVClabelIndex) i);
        if ( !gsIsMultAct(lab) )
        {
          lab = ATAgetArgument(lab,0);
        }
        p_add_label((ATerm) lab,(ATisEmpty(ATLgetArgument(lab,0))==ATtrue)?true:false);
      } else {
        p_add_label(SVClabel2ATerm(&f,(SVClabelIndex) i));
      }
    }

    p_add_transition((unsigned int) from,
                     (unsigned int) label,
                     (unsigned int) to);
  }

  SVCclose(&f);

  this->type = type;

  return true;
}

bool p_lts::read_from_aut(string &filename)
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

bool p_lts::read_from_aut(istream &is)
{
  unsigned int ntrans,nstate;
  char buf[1024];
  
  is.getline(buf,1024);
  sscanf(buf," des (%u,%u,%u)",&init_state,&ntrans,&nstate);

  for (unsigned int i=0; i<nstate; i++)
  {
    p_add_state();
  }
  assert(nstate == nstates);

  ATermIndexedSet labs = ATindexedSetCreate(100,50);
  while ( !is.eof() )
  {
    unsigned int from,to;
    char s[1024];

    is.getline(buf,1024);
    if ( is.gcount() == 0 )
    {
      break;
    }
    sscanf(buf,"(%u,\"%[^\"]\",%u)",&from,s,&to);

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
  assert(ntrans == ntransitions);

  this->type = lts_aut;

  return true;
}

#ifdef MCRL2_BCG
bool p_lts::read_from_bcg(string &filename)
{
  string::size_type pos = filename.rfind('.');
  if ( (pos == string::npos) || (filename.substr(pos+1) != "bcg") )
  {
    gsVerboseMsg("cannot open BCG file without '.bcg' extension\n");
    return false;
  }

  BCG_TYPE_OBJECT_TRANSITION bcg_graph;

  BCG_OT_READ_BCG_SURVIVE(BCG_TRUE);
  char *s = strdup(filename.c_str());
  BCG_OT_READ_BCG_BEGIN(
      s,
      &bcg_graph,
      0
      );
  free(s);
  BCG_OT_READ_BCG_SURVIVE(BCG_FALSE);

  if ( bcg_graph == NULL )
  {
    gsVerboseMsg("could not open BCG file '%s' for reading\n",filename.c_str());
    return false;
  }

  unsigned int n;

  n = BCG_OT_NB_STATES(bcg_graph);
  for (unsigned int i=0; i<n; i++)
  {
    p_add_state();
  }
  init_state = BCG_OT_INITIAL_STATE(bcg_graph);

  n = BCG_OT_NB_LABELS(bcg_graph);
  for (unsigned int i=0; i<n; i++)
  {
    p_add_label((ATerm) ATmakeAppl0(ATmakeAFun(BCG_OT_LABEL_STRING(bcg_graph,i),0,ATtrue)),!strcmp(BCG_OT_LABEL_STRING(bcg_graph,i),"i"));
  }

  unsigned int from,label,to;
  BCG_OT_ITERATE_PLN(bcg_graph,from,label,to)
  {
   p_add_transition(from,label,to); 
  }
  BCG_OT_END_ITERATE;

  creator = "";
  type = lts_bcg;

  return true;
}
#endif

bool lts::read_from(string &filename, lts_type type)
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

bool lts::read_from(istream &is, lts_type type)
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

bool p_lts::write_to_svc(string &filename, lts_type type)
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
      if ( !ATisAppl(label_values[i]) || !(gsIsMultAct((ATermAppl) label_values[i]) || is_timed_pair((ATermAppl) label_values[i]) ) )
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

bool p_lts::write_to_aut(string &filename)
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
  os << "des (" << init_state << "," << ntransitions << "," << nstates << ")" << endl;

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
      } else if ( ATisAppl(label) && is_timed_pair((ATermAppl) label) )
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
    os << "," << transitions[i].to << ")" << endl;
  }

  return true;
}

#ifdef MCRL2_BCG
bool p_lts::write_to_bcg(string &filename)
{
  BCG_IO_WRITE_BCG_SURVIVE(BCG_TRUE);
  char *s = strdup(filename.c_str());
  char *t = strdup(creator.c_str());
  BCG_TYPE_BOOLEAN b = BCG_IO_WRITE_BCG_BEGIN(
      s,
      init_state,
      1, // XXX add check to see if this might be 2?
      (creator != "")?t:NULL,
      0
      );
  free(t);
  free(s);
  BCG_IO_WRITE_BCG_SURVIVE(BCG_FALSE);

  if ( b == BCG_TRUE )
  {
    gsVerboseMsg("could not open BCG file '%s' for writing\n",filename.c_str());
    return false;
  }

  char *buf = NULL;
  unsigned int buf_size = 0;
  for (unsigned int i=0; i<ntransitions; i++)
  {
    string label_str;
    if ( label_info )
    {
      ATerm label = label_values[transitions[i].label];
      if ( ATisAppl(label) && (gsIsMultAct((ATermAppl) label) || is_timed_pair((ATermAppl) label)) )
      {
        if ( !gsIsMultAct((ATermAppl) label) )
        {
          label = ATgetArgument((ATermAppl) label,0);
        }
        label_str = PrintPart_CXX(label,ppDefault);
      } else {
        label_str = ATwriteToString(label);
      }
    } else {
      label_str = transitions[i].label;
    }
    if ( label_str.size() > buf_size )
    {
      if ( buf_size == 0 )
      {
        buf_size = 128;
      }
      while ( label_str.size() > buf_size )
      {
        buf_size = 2 * buf_size;
      }
      buf = (char *) realloc(buf,buf_size);
    }
    strcpy(buf,label_str.c_str());
    BCG_IO_WRITE_BCG_EDGE(transitions[i].from,buf,transitions[i].to);
  }

  BCG_IO_WRITE_BCG_END();

  return true;
}
#endif

bool lts::write_to(string &filename, lts_type type)
{
  switch ( type )
  {
    case lts_aut:
      return write_to_aut(filename);
    case lts_mcrl:
      return write_to_svc(filename,lts_mcrl);
      break;
    case lts_mcrl2:
      return write_to_svc(filename,lts_mcrl2);
    case lts_svc:
      return write_to_svc(filename,lts_svc);
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

bool lts::write_to(ostream &os, lts_type type)
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

    assert(label_info == (value != NULL));

    labels = (unsigned int *) realloc(labels,new_labels_size*sizeof(unsigned int));
    taus = (bool *) realloc(taus,new_labels_size*sizeof(bool));
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
