#include <ostream>
#include <fstream>
#include <string>
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "libprint.h"
#include "lts/liblts.h"
#include "lts/liblts_dot.h"

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
        os << i << "[label=\"" << l.state_value_str(i) << "\"];" << endl;
      }
    }
  }
  for (unsigned int i=0; i<l.num_transitions(); i++)
  {
    os << l.transition_from(i) << "->" << l.transition_to(i) << "[label=\"" << l.label_value_str(l.transition_label(i)) << "\"];" << endl;
  }

  os << "}" << endl;

  return true;
}

}
}
