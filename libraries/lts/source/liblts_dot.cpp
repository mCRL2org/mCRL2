// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_dot.cpp

#include <ostream>
#include <fstream>
#include <string>
#include "mcrl2/core/struct.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/lts/liblts.h"

#ifdef __cplusplus
using namespace ::mcrl2::utilities;
using namespace mcrl2::core;
#endif

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
