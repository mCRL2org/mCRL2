#include <ostream>
#include <fstream>
#include <string>
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "libprint.h"
#include "liblts.h"
#include "liblts_dot.h"

#define ATisAppl(x) (ATgetType(x) == AT_APPL)
#define ATisList(x) (ATgetType(x) == AT_LIST)

namespace mcrl2
{
namespace lts
{
  
bool write_lts_to_dot(lts &l, std::string &filename, std::string &name, bool print_states)
{
  std::ofstream os(filename.c_str());
  if ( !os.is_open() )
  {
    gsVerboseMsg("cannot open DOT file '%s' for writing\n",filename.c_str());
    return false;
  }

  bool r = write_lts_to_dot(l,os,name,print_states);

  os.close();

  return r;
}

using namespace std;
bool write_lts_to_dot(lts &l, ostream &os, string &name, bool print_states)
{
  os << "digraph \"" << name << "\" {" << endl;
  // os << "size=\"7,10.5\";" << endl;
  os << "center=TRUE;" << endl;
  os << "mclimit=10.0;" << endl;
  os << "nodesep=0.05;" << endl;
  os << "node[width=0.25,height=0.25,label=\"\"];" << endl;
  if ( l.num_states() > 0 )
  {
    os << l.initial_state() << "[peripheries=2];" << endl;
    if ( print_states && l.has_state_info() )
    {
      for (unsigned int i=0; i<l.num_states(); i++)
      {
        ATerm state = l.state_value(i);
        if ( ATisAppl(state) ) // XXX better check for mCRL2
        {
          os << i << "[label=\"(";
          ATermList args = ATgetArguments((ATermAppl) state);
          for (; !ATisEmpty(args); args=ATgetNext(args))
          {
            PrintPart_CXX(os,ATgetFirst(args),ppDefault);
            if ( !ATisEmpty(ATgetNext(args)) )
              os << ",";
          }
          os << ")\"];" << endl;
        } else if ( ATisList(state) )
        {
          os << i << "[label=\"[";
          ATermList args = (ATermList) state;
          for (; !ATisEmpty(args); args=ATgetNext(args))
          {
            os << ATwriteToString(ATgetFirst(args));
            if ( !ATisEmpty(ATgetNext(args)) )
              os << ",";
          }
          os << "]\"];" << endl;
        } else {
          os << ATwriteToString(state);
        }
      }
    }
  }
  for (unsigned int i=0; i<l.num_transitions(); i++)
  {
    os << l.transition_from(i) << "->" << l.transition_to(i) << "[label=\"";
    if ( l.has_label_info() )
    {
      ATerm label = l.label_value(l.transition_label(i));
      if ( ATisAppl(label) )
      {
        ATermAppl t = (ATermAppl) label;
        if ( ATgetArity(ATgetAFun(t)) == 0 )
        {
          os << ATgetName(ATgetAFun(t));
        } else if ( gsIsMultAct(t) || is_timed_pair(t) )
        {
          if ( !gsIsMultAct(t) ) // for backwards compatibility with untimed svc version
          {
            t = ATAgetArgument(t,0);
          }
          if ( ATisEmpty(ATLgetArgument(t,0)) )
          {
            os << "tau";
          } else {
            PrintPart_CXX(os,(ATerm) t,ppDefault);
          }
        } else {
          os << ATwriteToString(label);
        }
      } else {
        os << ATwriteToString(label);
      }
    } else {
      os << l.transition_label(i);
    }
    os << "\"];" << endl;
  }

  os << "}" << endl;

  return true;
}

}
}
