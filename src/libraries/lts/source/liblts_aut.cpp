#include <string>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <assert.h>
#include <aterm2.h>
#include "libprint_c.h"
#include "lts/liblts.h"

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

bool p_lts::read_from_aut(istream &is)
{
  boost::regex regex_aut_header("^[[:space:]]*des[[:space:]]*\\([[:space:]]*([[:digit:]]+)[[:space:]]*,[[:space:]]*([[:digit:]]+)[[:space:]]*,[[:space:]]*([[:digit:]]+)[[:space:]]*\\)[[:space:]]*$");
  boost::regex regex_aut_transition("^[[:space:]]*\\([[:space:]]*([[:digit:]]+)[[:space:]]*,[[:space:]]*\"?([^\"]+)\"?[[:space:]]*,[[:space:]]*([[:digit:]]+)[[:space:]]*\\)[[:space:]]*$");

  unsigned int ntrans,nstate;
  #define READ_FROM_AUT_BUF_SIZE 8196
  char buf[READ_FROM_AUT_BUF_SIZE];
  boost::match_results<const char *> what;
  
  is.getline(buf,READ_FROM_AUT_BUF_SIZE);
  if ( regex_search((char *)buf,what,regex_aut_header) )
  {
    *(const_cast<char *>(what[1].second)) = '\0';
    *(const_cast<char *>(what[2].second)) = '\0';
    *(const_cast<char *>(what[3].second)) = '\0';
    init_state = boost::lexical_cast<unsigned int>(what[1].first);
    ntrans = boost::lexical_cast<unsigned int>(what[2]);
    nstate = boost::lexical_cast<unsigned int>(what[3]);
  } else { 
    gsErrorMsg("cannot parse AUT input! (invalid header)\n");
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
    if ( is.gcount() == 0 )
    {
      break;
    }
    if ( regex_search((char *)buf,what,regex_aut_transition) )
    {
      *(const_cast<char *>(what[1].second)) = '\0';
      *(const_cast<char *>(what[2].second)) = '\0';
      *(const_cast<char *>(what[3].second)) = '\0';
      from = boost::lexical_cast<unsigned int>(what[1].first);
      s = what[2].first;
      to = boost::lexical_cast<unsigned int>(what[3].first);
    } else {
      gsErrorMsg("cannot parse AUT input! (invalid transition)\n");
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
  assert(ntrans == ntransitions);
  ATtableDestroy(labs);

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
