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
#include <vector>
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/parse.h"

using namespace mcrl2::core;

using namespace std;

namespace mcrl2
{
namespace lts
{

void lts_dot_t::save(const string& filename) const
{
  ofstream os(filename.c_str());
  if (!os.is_open())
  {
    throw mcrl2::runtime_error("cannot open DOT file '" + filename + "' for writing.");
  }
  save(os);
  os.close();
}

void lts_dot_t::save(std::ostream& os) const
{
  // Language definition seems to suggest that the name is optional, but tools seem to think otherwise
  os << "digraph G {" << endl;
  // os << "size=\"7,10.5\";" << endl;
  os << "center = TRUE;" << endl;
  os << "mclimit = 10.0;" << endl;
  os << "nodesep = 0.05;" << endl;
  os << "node [ width=0.25, height=0.25, label=\"\" ];" << endl;
  if (num_states() > 0)
  {
    if (has_state_info())
    {
      os << state_label(initial_state()).name();
    }
    else
    {
      os << "S" << initial_state();
    }

    os << " [ peripheries=2 ];" << endl;
    for (size_t i=0; i<num_states(); i++)
    {
      if (has_state_info())
      {
        const std::string& label = state_label(i).label();
        if (!label.empty())
        {
          os << state_label(i).name() << " [label=\"" << label << "\"];" << endl;
        }
      }
      else
      {
        os << "S" << i << endl;
      }
    }
  }
  const std::vector<transition> &trans=get_transitions();
  for (std::vector<transition>::const_iterator t=trans.begin(); t!=trans.end(); ++t)
  {
    if (has_state_info())
    {
      os << state_label(t->from()).name() << "->" << state_label(t->to()).name() << "[label=\"" <<
         mcrl2::lts::pp(action_label(t->label())) << "\"];" << endl;
    }
    else
    {
      os << "S" << t->from() << " -> " << "S" << t->to() << "[label=\"" <<
         mcrl2::lts::pp(action_label(t->label())) << "\"];" << endl;
    }
  }

  os << "}" << endl;
}

}
}
