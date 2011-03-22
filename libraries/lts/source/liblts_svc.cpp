// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_svc.cpp

#include <string>
#include <sstream>
// #include "boost/cstdint.hpp"
#include "aterm2.h"
#include "svc/svc.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/lps/typecheck.h"
#include "mcrl2/lps/multi_action.h"
#include "mcrl2/lts/lts_svc.h"


// using namespace mcrl2::core;
// using namespace mcrl2::core::detail;
// using namespace mcrl2::data::detail;

using namespace std;
using namespace mcrl2::lts;
using namespace mcrl2::lts::detail;


static void read_from_svc(lts_svc_t& l, string const& filename)
{
  SVCfile f;
  bool b;

  if (SVCopen(&f,const_cast< char* >(filename.c_str()),SVCread,&b))
  {
    throw mcrl2::runtime_error("cannot open SVC file '" + filename + "' for reading.");
  }

  bool svc_file_has_state_info = !SVCgetIndexFlag(&f);

  assert(SVCgetInitialState(&f)==0);
  if (svc_file_has_state_info)
  {
    l.add_state(state_label_svc((ATermAppl)SVCstate2ATerm(&f,(SVCstateIndex) SVCgetInitialState(&f))));
  }
  else
  {
    l.add_state();
  }
  l.set_initial_state((size_t) SVCgetInitialState(&f));


  SVCstateIndex from, to;
  SVClabelIndex label;
  SVCparameterIndex param;
  while (SVCgetNextTransition(&f,&from,&label,&to,&param))
  {
    size_t max = (size_t)((from > to)?from:to);
    for (size_t i=l.num_states(); i<=max; i++)
    {
      if (svc_file_has_state_info)
      {
        l.add_state(state_label_svc((ATermAppl)SVCstate2ATerm(&f,(SVCstateIndex) i)));
      }
      else
      {
        l.add_state();
      }
    }

    for (size_t i=l.num_action_labels(); i<=(size_t)label; i++)
    {
      l.add_action(SVClabel2ATerm(&f,(SVClabelIndex) i));
    }

    l.add_transition(transition((size_t)from,
                                (size_t)label,
                                (size_t)to));
  }

  SVCclose(&f);
}

static void write_to_svc(const lts_svc_t& l, const string& filename)
{
  SVCfile f;
  bool b = !l.has_state_info();
  if (SVCopen(&f,const_cast< char* >(filename.c_str()),SVCwrite,&b))
  {
    throw mcrl2::runtime_error("cannot open SVC file '" + filename + "' for writing.");
  }

  if (l.has_state_info())
  {
    SVCsetType(&f,const_cast < char* >("SVC+info"));
  }
  else
  {
    SVCsetType(&f,const_cast < char* >("SVC"));
  }

  SVCsetCreator(&f,const_cast < char* >("liblts (mCRL2)"));

  assert(l.initial_state()< ((size_t)1 << (sizeof(int)*8-1)));
  SVCsetInitialState(&f,SVCnewState(&f, l.has_state_info() ?
                                    (ATerm)l.state_label(l.initial_state()) : (ATerm) ATmakeInt((int)l.initial_state()) ,&b));

  SVCparameterIndex param = SVCnewParameter(&f,(ATerm) ATmakeList0(),&b);

  for (transition_const_range t=l.get_transitions();  !t.empty(); t.advance_begin(1))
  {
    assert(t.front().from() < ((size_t)1 << (sizeof(int)*8-1)));
    SVCstateIndex from = SVCnewState(&f, l.has_state_info() ?
                                     (ATerm)l.state_label(t.front().from()) : (ATerm) ATmakeInt((int)t.front().from()) ,&b);
    SVClabelIndex label = SVCnewLabel(&f, (ATerm)l.action_label(t.front().label()),&b);
    assert(t.front().to() < ((size_t)1 << (sizeof(int)*8-1)));
    SVCstateIndex to = SVCnewState(&f, l.has_state_info() ? (ATerm)l.state_label(t.front().to()) :
                                   (ATerm) ATmakeInt((int)t.front().to()) ,&b);
    SVCputTransition(&f,from,label,to,param);
  }

  SVCclose(&f);
}

namespace mcrl2
{
namespace lts
{

void lts_svc_t::load(const std::string& filename)
{
  if (filename=="")
  {
    throw mcrl2::runtime_error("Cannot read svc/lts file " + filename + " from stdin");
  }
  else
  {
    read_from_svc(*this,filename);
  }

}

void lts_svc_t::save(const std::string& filename) const
{
  if (filename=="")
  {
    throw mcrl2::runtime_error("Cannot write svc file " + filename + " to stdout");
  }
  else
  {
    write_to_svc(*this,filename);
  }

}
}
}
