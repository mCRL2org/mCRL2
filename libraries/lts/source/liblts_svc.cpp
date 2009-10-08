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
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/lts/lts.h"
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

bool p_lts::read_from_svc(string const& filename, lts_type type)
{
  SVCfile f;
  SVCbool b;

  if ( SVCopen(&f,const_cast< char* >(filename.c_str()),SVCread,&b) )
  {
    gsVerboseMsg("cannot open SVC file '%s' for reading\n",filename.c_str());
    return false;
  }

  creator = SVCgetCreator(&f);

  string svc_type = SVCgetType(&f);
  if ( type == lts_mcrl )
  {
    if ( svc_type != "generic" )
    {
      gsVerboseMsg("SVC file '%s' is not in the mCRL format\n",filename.c_str());
      return false;
    }
    state_info = (SVCgetIndexFlag(&f) == SVCfalse);
  } else if ( type == lts_mcrl2 )
  {
    if ( svc_type == "mCRL2" )
    {
      state_info = false;
    } else if ( svc_type == "mCRL2+info" )
    {
      state_info = true;
    } else {
      gsVerboseMsg("SVC file '%s' is not in the mCRL2 format\n",filename.c_str());
      return false;
    }
  } else {
    if ( svc_type == "generic" )
    {
      gsVerboseMsg("SVC file '%s' is in the mCRL format\n",filename.c_str());
      return false;
    } else if ( (svc_type == "mCRL2") || (svc_type == "mCRL2+info") )
    {
      gsVerboseMsg("SVC file '%s' is in the mCRL2 format\n",filename.c_str());
      return false;
    }
    state_info = (SVCgetIndexFlag(&f) == SVCfalse);
  }

  label_info = true;

  init_state = (unsigned int) SVCgetInitialState(&f);
  if ( state_info )
  {
    p_add_state(SVCstate2ATerm(&f,(SVCstateIndex) init_state));
  } else {
    p_add_state();
  }

  SVCstateIndex from, to;
  SVClabelIndex label;
  SVCparameterIndex param;
  while ( SVCgetNextTransition(&f,&from,&label,&to,&param) )
  {
    unsigned int max = (unsigned int) ((from > to)?from:to);
    for (unsigned int i=nstates; i<=max; i++)
    {
      if ( state_info )
      {
        p_add_state(SVCstate2ATerm(&f,(SVCstateIndex) i));
      } else {
        p_add_state();
      }
    }

    for (unsigned int i=nlabels; i<=((unsigned int) label); i++)
    {
      if ( type == lts_mcrl )
      {
        ATermAppl lab = (ATermAppl) SVClabel2ATerm(&f,(SVClabelIndex) i);
        p_add_label((ATerm) lab,!strcmp(ATgetName(ATgetAFun(lab)),"tau"));
      } else if ( type == lts_mcrl2 )
      {
        ATermAppl lab = (ATermAppl) SVClabel2ATerm(&f,(SVClabelIndex) i);
        if ( !gsIsMultAct(lab) )
        {
          lab = ATAgetArgument(lab,0);
        }
        p_add_label((ATerm) lab,(ATisEmpty(ATLgetArgument(lab,0))==ATtrue)?true:false);
      } else {
        p_add_label(SVClabel2ATerm(&f,(SVClabelIndex) i));
      }
    }

    p_add_transition((unsigned int) from,
                     (unsigned int) label,
                     (unsigned int) to);
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
            extra_data = data;
          }
        }
      }
    }
    if ( g != NULL )
    {
      fclose(g);
    }
  }

  this->type = type;

  return true;
}

bool p_lts::write_to_svc(string const& filename, lts_type type, lps::specification const& spec)
{
  bool applied_conversion = false;

  if ( type == lts_mcrl )
  {
    if ( state_info )
    {
      for (unsigned int i=0; i<nstates; i++)
      {
        if ( !ATisList(state_values[i]) )
        { 
          gsWarningMsg("state values are not saved as they are not in mCRL format\n");
	  state_info = false;
          break;
        }
      }
    }
    if ( !label_info )
    {
      gsVerboseMsg("cannot save LTS in mCRL format; labels have no value\n");
      return false;
    }
    for (unsigned int i=0; i<nlabels; i++)
    {
      if ( !ATisAppl(label_values[i]) || (ATgetArity(ATgetAFun((ATermAppl) label_values[i])) != 0) )
      {
        bool no_convert = true;
        if ( ATisAppl(label_values[i]) && (gsIsMultAct((ATermAppl) label_values[i]) || is_timed_pair((ATermAppl) label_values[i])) )
        {
          no_convert = false;
          label_values[i] = (ATerm) ATmakeAppl0(ATmakeAFun(p_label_value_str(i).c_str(),0,ATtrue));
          applied_conversion = true;
        }
        if ( no_convert )
        {
          gsVerboseMsg("cannot save LTS in mCRL format; label values are incompatible\n");
          return false;
        }
      }
    }
  } else if ( type == lts_mcrl2 )
  {
    if ( state_info )
    {
      for (unsigned int i=0; i<nstates; i++)
      {
        if ( !ATisAppl(state_values[i]) || strcmp(ATgetName(ATgetAFun((ATermAppl) state_values[i])),"STATE") )
        {
          gsWarningMsg("state values are not saved as they are not in mCRL2 format\n");
	  state_info = false;
          break;
        }
      }
    }
    if ( !label_info )
    {
      gsVerboseMsg("cannot save LTS in mCRL2 format; labels have no value\n");
      return false;
    }
    for (unsigned int i=0; i<nlabels; i++)
    {
      if ( !ATisAppl(label_values[i]) || !(gsIsMultAct((ATermAppl) label_values[i]) || is_timed_pair((ATermAppl) label_values[i]) ) )
      {
        bool no_convert = true;
        if ( (&spec != &empty_specification()) )
        {
          stringstream ss(p_label_value_str(i));
          ATermAppl t = parse_mult_act(ss);
          if ( t == NULL )
          {
            gsVerboseMsg("cannot parse action as mCRL2\n");
          } else {
            lps::specification copy(spec);
            copy.data() = data::remove_all_system_defined(copy.data());
            t = type_check_mult_act(t,specification_to_aterm(copy));
            if ( t == NULL )
            {
              gsVerboseMsg("error type checking action\n");
            }
            else
            {
              no_convert = false;
              label_values[i] = (ATerm) t;
              applied_conversion = true;
            }
          }
        }
        if ( no_convert )
        {
          gsVerboseMsg("cannot save LTS in mCRL2 format; label values are incompatible\n");
          if ( (&spec == &empty_specification()) )
          {
            gsVerboseMsg("using the -l/--lps option might help\n");
          }
          return false;
        }
      }
    }
  } else {
    gsVerboseMsg("saving SVC as type 'unknown'\n");
  }

  if ( state_info && applied_conversion )
  {
    gsWarningMsg("state information will be lost due to conversion\n");
    state_info = false;
    free(state_values);
    state_values = NULL;
  }

  SVCfile f;
  SVCbool b = state_info ? SVCfalse : SVCtrue;
  if ( SVCopen(&f,const_cast< char* >(filename.c_str()),SVCwrite,&b) )
  {
    gsVerboseMsg("cannot open SVC file '%s' for writing\n",const_cast< char* >(filename.c_str()));
    return false;
  }

  if ( type == lts_mcrl )
  {
    SVCsetType(&f,const_cast < char* > ("generic"));
  } else if ( type == lts_mcrl2 )
  {
    if ( state_info )
    {
      SVCsetType(&f,const_cast < char* > ("mCRL2+info"));
    } else {
      SVCsetType(&f,const_cast < char* > ("mCRL2"));
    }
  } else {
      SVCsetType(&f,const_cast < char* > ("unknown"));
  }

  if ( creator == "" )
  {
    SVCsetCreator(&f,const_cast < char* > ("liblts (mCRL2)"));
  } else {
    SVCsetCreator(&f, const_cast< char* >(creator.c_str()));
  }

  SVCsetInitialState(&f,SVCnewState(&f, state_info ? state_values[init_state] : (ATerm) ATmakeInt(init_state) ,&b));

  SVCparameterIndex param = SVCnewParameter(&f,(ATerm) ATmakeList0(),&b);
  for (unsigned int i=0; i<ntransitions; i++)
  {
    SVCstateIndex from = SVCnewState(&f, state_info ? state_values[transitions[i].from] : (ATerm) ATmakeInt(transitions[i].from) ,&b);
    SVClabelIndex label = SVCnewLabel(&f, label_info ? label_values[transitions[i].label] : (ATerm) ATmakeInt(transitions[i].label) ,&b);
    SVCstateIndex to = SVCnewState(&f, state_info ? state_values[transitions[i].to] : (ATerm) ATmakeInt(transitions[i].to) ,&b);
    SVCputTransition(&f,from,label,to,param);
  }

  SVCclose(&f);

  if ( type == lts_mcrl2 && ((extra_data != NULL) || applied_conversion) )
  {
    ATermAppl data_spec = NULL;
    ATermList params = NULL;
    ATermList act_spec = NULL;

    if ( applied_conversion )
    {
      data_spec = mcrl2::data::detail::data_specification_to_aterm_data_spec(spec.data(), false);
      act_spec = spec.action_labels();
    } else {
      data_spec = ATAgetArgument((ATermAppl) extra_data,0);
      if ( gsIsNil(data_spec) )
      {
        data_spec = NULL;
      }
      if ( state_info && !gsIsNil(ATAgetArgument((ATermAppl) extra_data,1)) )
      {
        params = ATLgetArgument(ATAgetArgument((ATermAppl) extra_data,1),0);
      }
      act_spec = (ATermList) ATAgetArgument((ATermAppl) ATAgetArgument((ATermAppl) extra_data,2), 0);
      if ( gsIsNil(data_spec) )
      {
        act_spec = ATmakeList0();
      }
    }

    add_extra_mcrl2_svc_data(filename,data_spec,params,act_spec);
  }

  return true;
}

}
}
