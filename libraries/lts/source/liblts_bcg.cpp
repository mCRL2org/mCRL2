// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_bcg.cpp

#include <string>
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/lts/lts.h"
#include "mcrl2/exception.h"

using namespace mcrl2::core;

#ifdef USE_BCG

// BCG library initialisation
bool initialise() {
  BCG_INIT();

  return true;
}

static bool initialised = initialise();

using namespace std;

namespace mcrl2
{
namespace lts
{
namespace detail
{

void read_from_bcg(lts &l, string const& filename)
{
  string::size_type pos = filename.rfind('.');
  if ( (pos == string::npos) || (filename.substr(pos+1) != "bcg") )
  {
    throw mcrl2::runtime_error("cannot open BCG file without '.bcg' extension");
  }

  BCG_TYPE_OBJECT_TRANSITION bcg_graph;

  BCG_OT_READ_BCG_SURVIVE(BCG_TRUE);
  BCG_OT_READ_BCG_BEGIN(
      const_cast< char* >(filename.c_str()),
      &bcg_graph,
      0
      );
  BCG_OT_READ_BCG_SURVIVE(BCG_FALSE);

  if ( bcg_graph == NULL )
  {
    throw mcrl2::runtime_error("could not open BCG file '" + filename + "' for reading");
  }

  unsigned int n;

  n = BCG_OT_NB_STATES(bcg_graph);
  for (unsigned int i=0; i<n; i++)
  {
    l.add_state();
  }
  l.set_initial_state(BCG_OT_INITIAL_STATE(bcg_graph));

  n = BCG_OT_NB_LABELS(bcg_graph);
  for (unsigned int i=0; i<n; i++)
  {
    l.add_label((ATerm) ATmakeAppl0(ATmakeAFun(BCG_OT_LABEL_STRING(bcg_graph,i),0,ATtrue)),!strcmp(BCG_OT_LABEL_STRING(bcg_graph,i),"i"));
  }

  unsigned int from,label,to;
  BCG_OT_ITERATE_PLN(bcg_graph,from,label,to)
  {
   l.add_transition(transition(from,label,to));
  }
  BCG_OT_END_ITERATE;

  l.set_creator("");
  l.set_type(lts_bcg);

}

void write_to_bcg(const lts &l, string const& filename)
{
  BCG_IO_WRITE_BCG_SURVIVE(BCG_TRUE);
  BCG_TYPE_BOOLEAN b = BCG_IO_WRITE_BCG_BEGIN(
      const_cast< char* >(filename.c_str()),
      l.initial_state(),
      1, // XXX add check to see if this might be 2?
      (l.get_creator() != "")?const_cast< char* >(l.get_creator().c_str()):NULL,
      0
      );
  BCG_IO_WRITE_BCG_SURVIVE(BCG_FALSE);

  if ( b == BCG_TRUE )
  {
    throw mcrl2::runtime_error("could not open BCG file '" +filename + "' for writing\n");
  }

  char *buf = NULL;
  unsigned int buf_size = 0;
  bool warn_non_i = true;
  bool warn_i = true;
 
  for (transition_const_range r=l.get_transitions(); !r.empty(); r.advance_begin(1))
  { transition t=r.front();
    string label_str = l.label_value_str(t.label());
    if ( l.is_tau(t.label()) )
    {
      if ( warn_non_i && (label_str != "i") )
      {
        gsWarningMsg("LTS contains silent steps that are not labelled 'i'; saving as BCG means they are replaced by 'i'.\n");
        warn_non_i = false;
      }
      label_str = "i";
    } else if ( warn_i && (label_str == "i") )
    {
      gsWarningMsg("LTS contains label 'i' without being marked as silent step; saving as BCG means it is assumed to be a silent step.\n");
      warn_i = false;
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
      if ( buf == NULL )
      {
        throw mcrl2::runtime_error("insufficient memory to write LTS to BCG\n");
      }
    }
    strcpy(buf,label_str.c_str());
    BCG_IO_WRITE_BCG_EDGE(t.from(),buf,t.to());
  }

  BCG_IO_WRITE_BCG_END();

}

}
}
}

#endif
