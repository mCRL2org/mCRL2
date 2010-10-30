// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_lts.cpp

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
#include "mcrl2/data/data_specification.h"
#include "mcrl2/lps/typecheck.h"
#include "mcrl2/lps/multi_action.h"
#include "mcrl2/lts/lts_lts.h"


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

void read_from_lts(lts_lts_t &l, string const& filename, lts_type type)
{
  SVCfile f;
  SVCbool b;
  bool svc_file_has_state_info = false;

  if ( SVCopen(&f,const_cast< char* >(filename.c_str()),SVCread,&b) )
  {
    throw mcrl2::runtime_error("cannot open SVC file '" + filename + "' for reading.");
  }

  l.set_creator(SVCgetCreator(&f));

  string svc_type = SVCgetType(&f);
  if ( type == lts_lts )
  {
    if ( svc_type == "mCRL2" )
    {
      svc_file_has_state_info = false; // redundant.
    } 
    else if ( svc_type == "mCRL2+info" )
    {
      svc_file_has_state_info = true;
    } 
    else 
    {
      throw mcrl2::runtime_error("SVC file '" + filename + "' is not in the mCRL2 format.");
    }
  } 
  else 
  {
    if ( svc_type == "generic" )
    {
      throw mcrl2::runtime_error("SVC file '" + filename + "' is in the mCRL format.");
    } else if ( (svc_type == "mCRL2") || (svc_type == "mCRL2+info") )
    {
      throw mcrl2::runtime_error("SVC file '" + filename + "' is in the mCRL2 format.");
    }
    svc_file_has_state_info = (SVCgetIndexFlag(&f) == SVCfalse);
  }

  assert(SVCgetInitialState(&f)==0);
  if ( svc_file_has_state_info )
  {
    using namespace mcrl2::data;
    using namespace mcrl2::lts::detail;
    ATermAppl state_label=(ATermAppl)SVCstate2ATerm(&f,(SVCstateIndex) SVCgetInitialState(&f));
    std::vector < data_expression > state_element_vector;
    const unsigned int arity=ATgetArity(ATgetAFun(state_label));
    state_element_vector.reserve(arity);
    for(unsigned int i=0; i<arity; ++i)
    {
      state_element_vector.push_back(data_expression(ATgetArgument(state_label,i)));
    }
    l.add_state(state_label_lts(state_element_vector.begin(),state_element_vector.end()));
  }
  else
  {
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
      if ( svc_file_has_state_info )
      {
        using namespace mcrl2::data;
        using namespace mcrl2::lts::detail;
        ATermAppl state_label=(ATermAppl)SVCstate2ATerm(&f,(SVCstateIndex) i);
        std::vector < data_expression > state_element_vector;
        const unsigned int arity=ATgetArity(ATgetAFun(state_label));
        state_element_vector.reserve(arity);
        for(unsigned int j=0; j<arity; ++j)
        {
          state_element_vector.push_back(data_expression(ATgetArgument(state_label,j)));
        }
        l.add_state(state_label_lts(state_element_vector.begin(),state_element_vector.end()));
      } 
      else 
      {
        l.add_state();
      }
    }

    for (unsigned int i=l.num_action_labels(); i<=((unsigned int) label); i++)
    {
      if ( type == lts_lts )
      {
        ATermAppl lab = (ATermAppl) SVClabel2ATerm(&f,(SVClabelIndex) i);
        if ( !gsIsMultAct(lab) )
        {
          lab = ATAgetArgument(lab,0);
        }
        l.add_label((ATerm) lab,(ATisEmpty(ATLgetArgument(lab,0))==ATtrue)?true:false);
      } 
      else 
      {
        l.add_label(SVClabel2ATerm(&f,(SVClabelIndex) i));
      }
    }

    l.add_transition(transition((unsigned int) from,
                                (unsigned int) label,
                                (unsigned int) to));
  }

  SVCclose(&f);

  if ( type == lts_lts )
  {
    // Check to see if there is extra data at the end
    FILE *g = fopen(filename.c_str(),"rb");
    if ( (g == NULL) ||
         (fseek(g,-(12+sizeof(boost::uint64_t)),SEEK_END) != 0) )
    {
      gsErrorMsg("could not determine whether mCRL2 SVC has extra information; continuing without\n");
    } 
    else 
    {
      unsigned char buf[8+12];
      if ( fread(&buf,1,8+12,g) != 8+12 )
      {
        gsErrorMsg("could not determine whether mCRL2 SVC has extra information; continuing without\n");
      } 
      else 
      {
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
          } 
          else 
          {
            gsVerboseMsg("read extra information from mCRL2 LTS\n");
            data::data_specification data_spec(ATgetArgument(data,0));
            data_spec.declare_data_specification_to_be_type_checked(); // We can assume that this data spec is well typed.
            l.set_data(data::data_specification(data_spec));
            if (!gsIsNil((ATermAppl)ATgetArgument(data,1)))
            {
             // The parameters below have the structure "ParamSpec(variable list);
             l.set_process_parameters(data::variable_list(ATgetArgument(ATgetArgument(data,1),0)));
            }
            if (!gsIsNil((ATermAppl)ATgetArgument(data,1)))
            {
              // The parameters below have the structure "ActSpec(variable list);
              l.set_action_labels(lps::action_label_list(ATgetArgument(ATgetArgument(data,2),0)));
            }
          }
        }
      }
    }
    if ( g != NULL )
    {
      fclose(g);
    }
  }
}

static void write_to_lts(const lts_lts_t& l, string const& filename, lts_type type)
{
  /* if (!l.has_label_info())
  { 
    throw mcrl2::runtime_error("Cannot save .lts file, because there are no transition labels");
  } */

  SVCfile f;
  SVCbool b = l.has_state_info() ? SVCfalse : SVCtrue;
  if ( SVCopen(&f,const_cast< char* >(filename.c_str()),SVCwrite,&b) )
  {
    throw mcrl2::runtime_error("cannot open SVC file '" + filename + "' for writing.");
  }

  if ( type == lts_lts )
  {
    if ( l.has_state_info() )
    {
      SVCsetType(&f,const_cast < char* > ("mCRL2+info"));
    } 
    else 
    {
      SVCsetType(&f,const_cast < char* > ("mCRL2"));
    }
  } 
  else 
  {
      SVCsetType(&f,const_cast < char* > ("unknown"));
  }

  if ( l.creator() == "" )
  {
    SVCsetCreator(&f,const_cast < char* > ("liblts (mCRL2)"));
  } 
  else 
  {
    SVCsetCreator(&f, const_cast< char* >(l.creator().c_str()));
  }

  SVCsetInitialState(&f,SVCnewState(&f, l.has_state_info() ? (ATerm)l.state_value(l.initial_state()).aterm() : (ATerm) ATmakeInt(l.initial_state()) ,&b));

  SVCparameterIndex param = SVCnewParameter(&f,(ATerm) ATmakeList0(),&b);
  
  for (transition_const_range t=l.get_transitions();  !t.empty(); t.advance_begin(1))
  {
    SVCstateIndex from = SVCnewState(&f, l.has_state_info() ? (ATerm)l.state_value(t.front().from()).aterm() : (ATerm) ATmakeInt(t.front().from()) ,&b);
    // SVClabelIndex label = SVCnewLabel(&f, l.has_label_info() ? (ATerm)l.label_value(t.front().label()).aterm() : (ATerm) ATmakeInt(t.front().label()) ,&b);
    SVClabelIndex label = SVCnewLabel(&f, (ATerm)l.label_value(t.front().label()).aterm(), &b);
    SVCstateIndex to = SVCnewState(&f, l.has_state_info() ? (ATerm)l.state_value(t.front().to()).aterm() : (ATerm) ATmakeInt(t.front().to()) ,&b);
    SVCputTransition(&f,from,label,to,param);
  }

  SVCclose(&f);


  ATermAppl  data_spec = mcrl2::data::detail::data_specification_to_aterm_data_spec(l.data());
  ATermList params = l.process_parameters(); 
  ATermList act_spec = l.action_labels(); 
  add_extra_mcrl2_lts_data(filename,data_spec,params,act_spec);
}
} // namespace detail

void lts_lts_t::save(const std::string &filename) const
{
  if (filename=="")
  { 
    throw mcrl2::runtime_error("Cannot write svc/lts file " + filename + " to stdout");
  }
  else
  {
	  mcrl2::lts::detail::write_to_lts(*this,filename,detail::guess_format(filename));
  }

}
void lts_lts_t::load(const std::string &filename)
{
 if (filename=="")
 { 
   throw mcrl2::runtime_error("Cannot read svc/lts file " + filename + " from stdin");
 }
 else
 {
	 mcrl2::lts::detail::read_from_lts(*this,filename,detail::detect_type(filename));
 }

}

}
}
