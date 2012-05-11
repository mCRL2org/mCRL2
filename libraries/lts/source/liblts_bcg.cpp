// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_bcg.cpp

#ifdef USE_BCG

#include <string>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/lts/lts_bcg.h"
#include "mcrl2/exception.h"
#include "bcg_user.h"

using namespace mcrl2::lts;
using namespace std;
using namespace mcrl2::log;

// BCG library initialisation
static bool initialise()
{
  BCG_INIT();
  return true;
}

static bool initialised = initialise();


static void read_from_bcg(lts_bcg_t& l, const string& filename)
{
  BCG_TYPE_OBJECT_TRANSITION bcg_graph;

  BCG_OT_READ_BCG_SURVIVE(BCG_TRUE);
  BCG_OT_READ_BCG_BEGIN(
    const_cast< char* >(filename.c_str()),
    &bcg_graph,
    0
  );
  BCG_OT_READ_BCG_SURVIVE(BCG_FALSE);

  if (bcg_graph == NULL)
  {
    throw mcrl2::runtime_error("could not open BCG file '" + filename + "' for reading");
  }

  size_t n;

  n = BCG_OT_NB_STATES(bcg_graph);
  for (size_t i=0; i<n; i++)
  {
    l.add_state();
  }
  l.set_initial_state(BCG_OT_INITIAL_STATE(bcg_graph));

  n = BCG_OT_NB_LABELS(bcg_graph);
  for (size_t i=0; i<n; i++)
  {
    const std::string s=BCG_OT_LABEL_STRING(bcg_graph,i);
    l.add_action(s,s=="i");
  }

  size_t from,label,to;
  BCG_OT_ITERATE_PLN(bcg_graph,from,label,to)
  {
    l.add_transition(transition(from,label,to));
  }
  BCG_OT_END_ITERATE;

}

static void write_to_bcg(const lts_bcg_t& l, const string& filename)
{
  BCG_IO_WRITE_BCG_SURVIVE(BCG_TRUE);
  BCG_TYPE_BOOLEAN b = BCG_IO_WRITE_BCG_BEGIN(
                         const_cast< char* >(filename.c_str()),
                         l.initial_state(),
                         1,     // XXX add check to see if this might be 2?
                         NULL,  // There is no creator.
                         0
                       );
  BCG_IO_WRITE_BCG_SURVIVE(BCG_FALSE);

  if (b == BCG_TRUE)
  {
    throw mcrl2::runtime_error("could not open BCG file '" +filename + "' for writing\n");
  }

  char* buf = NULL;
  size_t buf_size = 0;
  bool warn_non_i = true;
  bool warn_i = true;

  const std::vector<transition> &trans=l.get_transitions();
  for (std::vector<transition>::const_iterator r=trans.begin(); r!=trans.end(); ++r)
  {
    string label_str = l.action_label(r->label());
    if (l.is_tau(r->label()))
    {
      if (warn_non_i && (label_str != "i"))
      {
        mCRL2log(warning) << "LTS contains silent steps that are not labelled 'i'; saving as BCG means they are replaced by 'i'." << std::endl;
        warn_non_i = false;
      }
      label_str = "i";
    }
    else if (warn_i && (label_str == "i"))
    {
      mCRL2log(warning) << "LTS contains label 'i' without being marked as silent step; saving as BCG means it is assumed to be a silent step." << std::endl;
      warn_i = false;
    }
    if (label_str.size() > buf_size)
    {
      if (buf_size == 0)
      {
        buf_size = 128;
      }
      while (label_str.size() > buf_size)
      {
        buf_size = 2 * buf_size;
      }
      buf = (char*) realloc(buf,buf_size);
      if (buf == NULL)
      {
        throw mcrl2::runtime_error("insufficient memory to write LTS to BCG\n");
      }
    }
    strcpy(buf,label_str.c_str());
    BCG_IO_WRITE_BCG_EDGE(r->from(),buf,r->to());
  }

  free(buf);

  BCG_IO_WRITE_BCG_END();

}

namespace mcrl2
{
namespace lts
{

void lts_bcg_t::load(const string& filename)
{
  if (filename=="")
  {
    throw mcrl2::runtime_error("Cannot read .bcg file from stdin.");
  }
  else
  {
    read_from_bcg(*this,filename);
  }
}

void lts_bcg_t::save(string const& filename) const
{
  if (filename=="")
  {
    throw mcrl2::runtime_error("Cannot write .bcg file to stdout.");
  }
  else
  {
    write_to_bcg(*this,filename);
  }
}

}
}

#endif
