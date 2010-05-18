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
#include "boost/cstdint.hpp"
#include "aterm2.h"
#include "svc/svc.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/data/data_specification.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data::detail;

#define ATisAppl(x) (ATgetType(x) == AT_APPL)
#define ATisList(x) (ATgetType(x) == AT_LIST)

using namespace std;

namespace mcrl2
{
namespace lts
{
namespace detail
{

void read_from_svc(lts &l, string const& filename, lts_type type)
{
  SVCfile f;
  SVCbool b;

  if ( SVCopen(&f,const_cast< char* >(filename.c_str()),SVCread,&b) )
  {
    throw mcrl2::runtime_error("cannot open SVC file '" + filename + "' for reading.");
  }

  l.set_creator(SVCgetCreator(&f));

  string svc_type = SVCgetType(&f);
  if ( type == lts_mcrl )
  {
    if ( svc_type != "generic" )
    {
      throw mcrl2::runtime_error("SVC file '" + filename + "' is not in the mCRL format.");
    }
    // state_info = (SVCgetIndexFlag(&f) == SVCfalse);
  } else if ( type == lts_mcrl2 )
  {
    if ( svc_type == "mCRL2" )
    {
      // state_info = false;
    } else if ( svc_type == "mCRL2+info" )
    {
      // state_info = true;
    } else {
      throw mcrl2::runtime_error("SVC file '" + filename + "' is not in the mCRL2 format.");
    }
  } else {
    if ( svc_type == "generic" )
    {
      throw mcrl2::runtime_error("SVC file '" + filename + "' is in the mCRL format.");
    } else if ( (svc_type == "mCRL2") || (svc_type == "mCRL2+info") )
    {
      throw mcrl2::runtime_error("SVC file '" + filename + "' is in the mCRL2 format.");
    }
    // state_info = (SVCgetIndexFlag(&f) == SVCfalse);
  }

  // label_info = true;

  if ( l.has_state_info() )
  {
    l.add_state(SVCstate2ATerm(&f,(SVCstateIndex) l.initial_state()));
  } else {
    l.add_state();
  }
  l.set_initial_state((unsigned int) SVCgetInitialState(&f));

  SVCstateIndex from, to;
  SVClabelIndex label;
  SVCparameterIndex param;
  while ( SVCgetNextTransition(&f,&from,&label,&to,&param) )
  {
    unsigned int max = (unsigned int) ((from > to)?from:to);
    for (unsigned int i=l.num_states(); i<=max; i++)
    {
      if ( l.has_state_info() )
      {
        l.add_state(SVCstate2ATerm(&f,(SVCstateIndex) i));
      } else {
        l.add_state();
      }
    }

    for (unsigned int i=l.num_labels(); i<=((unsigned int) label); i++)
    {
      if ( type == lts_mcrl )
      {
        ATermAppl lab = (ATermAppl) SVClabel2ATerm(&f,(SVClabelIndex) i);
        l.add_label((ATerm) lab,!strcmp(ATgetName(ATgetAFun(lab)),"tau"));
      } else if ( type == lts_mcrl2 )
      {
        ATermAppl lab = (ATermAppl) SVClabel2ATerm(&f,(SVClabelIndex) i);
        if ( !gsIsMultAct(lab) )
        {
          lab = ATAgetArgument(lab,0);
        }
        l.add_label((ATerm) lab,(ATisEmpty(ATLgetArgument(lab,0))==ATtrue)?true:false);
      } else {
        l.add_label(SVClabel2ATerm(&f,(SVClabelIndex) i));
      }
    }

    l.add_transition(transition((unsigned int) from,
                              (unsigned int) label,
                              (unsigned int) to));
  }

  SVCclose(&f);

  if ( type == lts_mcrl2 )
  {
    // Check to see if there is extra data at the end
    FILE *g = fopen(filename.c_str(),"rb");
    if ( (g == NULL) ||
         (fseek(g,-(12+sizeof(boost::uint64_t)),SEEK_END) != 0) )
    {
      gsErrorMsg("could not determine whether mCRL2 SVC has extra information; continuing without\n");
    } else {
      unsigned char buf[8+12];
      if ( fread(&buf,1,8+12,g) != 8+12 )
      {
        gsErrorMsg("could not determine whether mCRL2 SVC has extra information; continuing without\n");
      } else {
        if ( !strncmp(((char *) buf)+8,"   1STL2LRCm",12) )
        {
          ATerm data;
          boost::uint64_t position = 0;
          for (unsigned int i=0; i<8; i++)
          {
            position = position*0x100 + buf[7-i];
          }
          if ( (fseek(g,position,SEEK_SET) != 0 ) ||
               ((data = ATreadFromFile(g)) == NULL) )
          {
            gsErrorMsg("could not read extra information from mCRL2 LTS; continuing without\n");
          } else {
            gsVerboseMsg("read extra information from mCRL2 LTS\n");
            l.set_extra_data(data);
          }
        }
      }
    }
    if ( g != NULL )
    {
      fclose(g);
    }
  }

  l.set_type(type);

}

void write_to_svc(const lts& l1, string const& filename, lts_type type, lps::specification const& spec)
{
  lts l=l1; // Make a copy of the current lts, because below certain parts of the lts are
            // changed. This is highly undesirable, as lts's are large objects in memory.
            // It is a TODO to replace the code below, such that the lts l is not changed before
            // writing it to an .svc file.

  bool applied_conversion = false;

  if ( type == lts_mcrl )
  {
    if ( l.has_state_info() )
    {
      for (unsigned int i=0; i<l.num_states(); i++)
      {
        if ( !ATisList(l.state_value(i)) )
        { 
          gsWarningMsg("state values are not saved as they are not in mCRL format\n");
	  // state_info = false;
          break;
        }
      }
    }
    if ( !l.has_label_info() )
    {
     throw mcrl2::runtime_error("cannot save LTS in mCRL format; labels have no value.");
    }
    for (unsigned int i=0; i<l.num_labels(); i++)
    {
      if ( !ATisAppl(l.label_value(i)) || (ATgetArity(ATgetAFun((ATermAppl) l.label_value(i))) != 0) )
      {
        bool no_convert = true;
        if ( ATisAppl(l.label_value(i)) && (gsIsMultAct((ATermAppl) l.label_value(i)) || is_timed_pair((ATermAppl) l.label_value(i))) )
        {
          no_convert = false;
          l.set_label_value(i,(ATerm) ATmakeAppl0(ATmakeAFun(l.label_value_str(i).c_str(),0,ATtrue)));
          applied_conversion = true;
        }
        if ( no_convert )
        {
          throw mcrl2::runtime_error("cannot save LTS in mCRL format; label values are incompatible.");
        }
      }
    }
  } else if ( type == lts_mcrl2 )
  {
    if ( l.has_state_info() )
    {
      for (unsigned int i=0; i<l.num_states(); i++)
      {
        if ( !ATisAppl(l.state_value(i)) || strcmp(ATgetName(ATgetAFun((ATermAppl) l.state_value(i))),"STATE") )
        {
          gsWarningMsg("state values are not saved as they are not in mCRL2 format\n");
	  // state_info = false;
          break;
        }
      }
    }
    if ( !l.has_label_info() )
    {
      throw mcrl2::runtime_error("cannot save LTS in mCRL2 format; labels have no value.");
    }
    for (unsigned int i=0; i<l.num_labels(); i++)
    {
      if ( !ATisAppl(l.label_value(i)) || !(gsIsMultAct((ATermAppl) l.label_value(i)) || is_timed_pair((ATermAppl) l.label_value(i)) ) )
      {
        bool no_convert = true;
        if ( (&spec != &empty_specification()) )
        {
          stringstream ss(l.label_value_str(i));
          ATermAppl t = parse_mult_act(ss);
          if ( t == NULL )
          {
            gsVerboseMsg("cannot parse action as mCRL2\n");
          } 
          else 
          {
            t = type_check_mult_act(t,specification_to_aterm(spec));
            if ( t == NULL )
            {
              gsVerboseMsg("error type checking action\n");
            }
            else
            {
              no_convert = false;
              l.set_label_value(i,(ATerm) t);
              applied_conversion = true;
            }
          }
        }
        if ( no_convert )
        {
          throw mcrl2::runtime_error("cannot save LTS in mCRL2 format; label values are incompatible. Using the -l/--lps option might help.");
        }
      }
    }
  } else {
    gsVerboseMsg("saving SVC as type 'unknown'\n");
  }

  if ( l.has_state_info() && applied_conversion )
  {
    gsWarningMsg("state information will be lost due to conversion\n");
    // state_info = false;
    l.clear_states();
  }

  SVCfile f;
  SVCbool b = l.has_state_info() ? SVCfalse : SVCtrue;
  if ( SVCopen(&f,const_cast< char* >(filename.c_str()),SVCwrite,&b) )
  {
    throw mcrl2::runtime_error("cannot open SVC file '" + filename + "' for writing.");
  }

  if ( type == lts_mcrl )
  {
    SVCsetType(&f,const_cast < char* > ("generic"));
  } else if ( type == lts_mcrl2 )
  {
    if ( l.has_state_info() )
    {
      SVCsetType(&f,const_cast < char* > ("mCRL2+info"));
    } else {
      SVCsetType(&f,const_cast < char* > ("mCRL2"));
    }
  } else {
      SVCsetType(&f,const_cast < char* > ("unknown"));
  }

  if ( l.get_creator() == "" )
  {
    SVCsetCreator(&f,const_cast < char* > ("liblts (mCRL2)"));
  } else {
    SVCsetCreator(&f, const_cast< char* >(l.get_creator().c_str()));
  }

  SVCsetInitialState(&f,SVCnewState(&f, l.has_state_info() ? l.state_value(l.initial_state()) : (ATerm) ATmakeInt(l.initial_state()) ,&b));

  SVCparameterIndex param = SVCnewParameter(&f,(ATerm) ATmakeList0(),&b);
  
  for (transition_const_range t=l.get_transitions();  !t.empty(); t.advance_begin(1))
  {
    SVCstateIndex from = SVCnewState(&f, l.has_state_info() ? l.state_value(t.front().from()) : (ATerm) ATmakeInt(t.front().from()) ,&b);
    SVClabelIndex label = SVCnewLabel(&f, l.has_label_info() ? l.label_value(t.front().label()) : (ATerm) ATmakeInt(t.front().label()) ,&b);
    SVCstateIndex to = SVCnewState(&f, l.has_state_info() ? l.state_value(t.front().to()) : (ATerm) ATmakeInt(t.front().to()) ,&b);
    SVCputTransition(&f,from,label,to,param);
  }

  SVCclose(&f);

  if ( type == lts_mcrl2 && ((l.get_extra_data() != NULL) || applied_conversion) )
  {
    ATermAppl data_spec = NULL;
    ATermList params = NULL;
    ATermList act_spec = NULL;

    if ( applied_conversion )
    {
      data_spec = mcrl2::data::detail::data_specification_to_aterm_data_spec(spec.data());
      act_spec = spec.action_labels();
    } else {
      data_spec = ATAgetArgument((ATermAppl) l.get_extra_data(),0);
      if ( gsIsNil(data_spec) )
      {
        data_spec = NULL;
      }
      if ( l.has_state_info() && !gsIsNil(ATAgetArgument((ATermAppl) l.get_extra_data(),1)) )
      {
        params = ATLgetArgument(ATAgetArgument((ATermAppl) l.get_extra_data(),1),0);
      }
      act_spec = ATLgetArgument((ATermAppl) ATAgetArgument((ATermAppl) l.get_extra_data(),2), 0);
      if ( gsIsNil(data_spec) )
      {
        act_spec = ATmakeList0();
      }
    }

    add_extra_mcrl2_svc_data(filename,data_spec,params,act_spec);
  }

}

}
}
}
