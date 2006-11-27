#include <ostream>
#include <fstream>
#include <string>
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "libprint.h"
#include "lts/liblts.h"

#define ATisAppl(x) (ATgetType(x) == AT_APPL)
#define ATisList(x) (ATgetType(x) == AT_LIST)

using namespace std;

namespace mcrl2
{
namespace lts
{

bool p_lts::write_to_dot(string const& filename, lts_dot_options opts)
{
  ofstream os(filename.c_str());
  if ( !os.is_open() )
  {
    gsVerboseMsg("cannot open DOT file '%s' for writing\n",filename.c_str());
    return false;
  }

  bool r = write_to_dot(os,opts);

  os.close();

  return r;
}

bool p_lts::write_to_dot(ostream &os, lts_dot_options opts)
{
  os << "digraph \"" << *opts.name << "\" {" << endl;
  // os << "size=\"7,10.5\";" << endl;
  os << "center=TRUE;" << endl;
  os << "mclimit=10.0;" << endl;
  os << "nodesep=0.05;" << endl;
  os << "node[width=0.25,height=0.25,label=\"\"];" << endl;
  if ( nstates > 0 )
  {
    os << init_state << "[peripheries=2];" << endl;
    if ( opts.print_states && state_info )
    {
      for (unsigned int i=0; i<nstates; i++)
      {
        os << i << "[label=\"" << p_state_value_str(i) << "\"];" << endl;
      }
    }
  }
  for (unsigned int i=0; i<ntransitions; i++)
  {
    os << transitions[i].from << "->" << transitions[i].to << "[label=\"" << p_label_value_str(transitions[i].label) << "\"];" << endl;
  }

  os << "}" << endl;

  return true;
}

}
}
