// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_fsm.cpp

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <aterm2.h>
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/core/print.h"
#include "mcrl2/lts/lts.h"
#include "mcrl2/lps/specification.h"
#include "liblts_fsmparser.h"

#include "workarounds.h" // DECL_A

using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data::detail;
using namespace mcrl2;

#define ATisAppl(x) (ATgetType(x) == AT_APPL)
#define ATisList(x) (ATgetType(x) == AT_LIST)

namespace mcrl2
{
namespace lts
{

static ATerm parse_mcrl2_action(ATerm label, lps::specification &spec)
{
  std::stringstream ss(ATgetName(ATgetAFun((ATermAppl) label)));

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
  }

  return (ATerm) t;
}

static ATerm parse_mcrl2_state(ATerm state, lps::specification &spec)
{
  unsigned int len = ATgetLength((ATermList) state);
  DECL_A(state_args,ATerm,len);

  for (unsigned int i=0; !ATisEmpty((ATermList) state); state=(ATerm) ATgetNext((ATermList) state),++i)
  {
    ATermAppl val = ATAgetFirst((ATermList) state);
    ATermAppl expr = ATAgetArgument(val,0);
    ATermAppl sort = ATAgetArgument(ATAgetArgument(val,1),1);

    // typechecking and data implementation of terms needs an lps
    // before data implementation
    lps::specification copy(spec);
    copy.data() = data::remove_all_system_defined(copy.data());
    ATermAppl spec_for_type_check(specification_to_aterm(copy));

    std::stringstream sort_ss(ATgetName(ATgetAFun(sort)));
    sort = parse_sort_expr(sort_ss);
    if ( sort == NULL )
    {
      gsVerboseMsg("error parsing state argument sort\n");
      return NULL;
    }
    sort = type_check_sort_expr(sort,spec_for_type_check);
    if ( sort == NULL )
    {
      gsVerboseMsg("error type checking state argument sort\n");
      return NULL;
    }

    std::stringstream expr_ss(ATgetName(ATgetAFun(expr)));
    expr = parse_data_expr(expr_ss);
    if ( expr == NULL )
    {
      gsVerboseMsg("error parsing state argument\n");
      return NULL;
    }
    expr = type_check_data_expr(expr,sort,spec_for_type_check);
    if ( expr == NULL )
    {
      gsVerboseMsg("error type checking state argument\n");
      return NULL;
    }

    state_args[i] = (ATerm) expr;
  }

  ATerm r = (ATerm) ATmakeApplArray(ATmakeAFun("STATE",len,ATfalse),state_args);
  FREE_A(state_args);
  return r;
}

bool p_lts::read_from_fsm(std::istream &is, lts_type type, lps::specification *spec)
{
  if ( parse_fsm(is,*lts_object) )
  {
    if ( type == lts_mcrl2 )
    {
      for (unsigned int i=0; i<nlabels; i++)
      {
        label_values[i] = parse_mcrl2_action(label_values[i],*spec);
        if ( label_values[i] == NULL )
        {
          return false;
        }
      }
      if ( nstates > 0 && ATgetLength((ATermList) state_values[0]) == 0 )
      {
        p_remove_state_values();
      } else {
        for (unsigned int i=0; i<nstates; i++)
        {
          state_values[i] = parse_mcrl2_state(state_values[i],*spec);
          if ( state_values[i] == NULL )
          {
            return false;
          }
        }
      }
      data::variable_list process_parameters(spec->process().process_parameters());
      extra_data = (ATerm) ATmakeAppl3(ATmakeAFun("mCRL2LTS1",3,ATfalse),
              (ATerm)(ATermAppl) mcrl2::data::detail::data_specification_to_aterm_data_spec(spec->data()),
              (ATerm) ATmakeAppl1(ATmakeAFun("ParamSpec",1,ATfalse),(ATerm) static_cast< ATermList >(atermpp::term_list< data::variable >(process_parameters.begin(), process_parameters.end()))),
              ATgetArgument(specification_to_aterm(*spec),1));
      this->type = lts_mcrl2;
    } else if ( type == lts_mcrl ) {
      if ( nstates > 0 && ATgetLength((ATermList) state_values[0]) == 0 )
      {
        p_remove_state_values();
      } else {
        for (unsigned int i=0; i<nstates; i++)
        {
          ATermList m = ATmakeList0();
          for (ATermList l=ATreverse((ATermList) state_values[i]); !ATisEmpty(l); l=ATgetNext(l))
          {
            ATerm a = ATmake(ATgetName(ATgetAFun(ATAgetArgument(ATAgetFirst(l),0))));
            m = ATinsert(m,a);
          }
          state_values[i] = (ATerm) m;
        }
      }
      this->type = lts_mcrl;
    } else {
      this->type = lts_fsm;
    }
    return true;
  } else {
    return false;
  }
}

bool p_lts::read_from_fsm(std::string const &filename, lts_type type, lps::specification *spec)
{
  std::ifstream is(filename.c_str());

  if ( !is.is_open() )
  {
    gsVerboseMsg("cannot open FSM file '%s' for reading\n",filename.c_str());
    return false;
  }

  return read_from_fsm(is,type,spec);
}

bool p_lts::write_to_fsm(std::ostream &os, lts_type type, ATermList params)
{
  // determine number of state parameters
  unsigned int num_params;
  if ( (type != lts_none) && state_info && (nstates > 0) )
  {
    if ( (type == lts_mcrl) || (type == lts_fsm) || (type == lts_dot) )
    {
      num_params = ATgetLength((ATermList) state_values[0]);
    } else { // type == lts_mcrl2
      num_params = ATgetArity(ATgetAFun((ATermAppl) state_values[0]));
    }
  } else {
    num_params = 0;
    gsWarningMsg("parameter names are unknown (use LTS with extra information or supply the source LPS)\n");
  }

  // create set per parameter containing all used values
  ATermIndexedSet *set = (ATermIndexedSet*) malloc(num_params*sizeof(ATermIndexedSet));
  if ( set == NULL )
  {
    throw mcrl2::runtime_error("malloc failed");
  }
  if ( (type != lts_none) && state_info )
  {
    for (unsigned int i=0; i<num_params; i++)
    {
      set[i] = ATindexedSetCreate(30,50);
    }
    for (unsigned int i=0; i<nstates; i++)
    {
      ATermList state_pars;
      if ( (type == lts_mcrl) || (type == lts_fsm) || (type == lts_dot) )
      {
        state_pars = (ATermList) state_values[i];
      } else { // type == mcrl2
        state_pars = ATgetArguments((ATermAppl) state_values[i]);
      }

      assert( ((unsigned int) ATgetLength(state_pars)) >= num_params );
      if ( ((unsigned int) ATgetLength(state_pars)) < num_params )
      {
       throw mcrl2::runtime_error("invalid state in LTS (it does not have as much parameters as the LPS)");
      }

      for (unsigned int j=0; !ATisEmpty(state_pars); state_pars=ATgetNext(state_pars),j++)
      {
        ATbool is_new;
        ATindexedSetPut(set[j],ATgetFirst(state_pars),&is_new);
      }
    }
  }

  // print parameters with used values
  gsVerboseMsg("writing parameter table...\n");
  for(unsigned i=0; i<num_params; i++)
  {
    ATermList vals = ATindexedSetElements(set[i]);

    if ( (params == NULL) || ATisEmpty(params) )
    {
      if ( type == lts_fsm || type == lts_dot )
      {
        ATermAppl type = (ATermAppl) ATgetArgument(ATAgetFirst(vals),1);
        os << ATgetName(ATgetAFun(ATgetArgument(type,0))) << "(" << ATgetLength(vals) << ") " << ATgetName(ATgetAFun(ATgetArgument(type,1))) << " ";
      } else {
        os << "unknown" << i << "(" << ATgetLength(vals) << ") unknown ";
      }
    } else {
      if ( type == lts_mcrl )
      {
        std::string s;
        s = ATwriteToString(ATgetFirst(ATLgetFirst(params)));
        s = s.substr(1,s.size()-3);
        os << s << "(" << ATgetLength(vals) << ") ";
        s = ATwriteToString(ATgetFirst(ATgetNext(ATLgetFirst(params))));
        s = s.substr(1,s.size()-2);
        os << s << " ";
      } else { // type == lts_mcrl2
        ATermAppl param = ATAgetFirst(params);
        assert(gsIsDataVarId(param));
        PrintPart_CXX(os,ATgetArgument(param, 0),ppDefault);
        os << "(" << ATgetLength(vals) << ") ";
        PrintPart_CXX(os,ATgetArgument(param, 1),ppDefault);
        os << " ";
      }

      params = ATgetNext(params);
    }

    if ( type == lts_mcrl2 )
    {
      for(; !ATisEmpty(vals); vals=ATgetNext(vals))
      {
        os << " \"";
        PrintPart_CXX(os,ATgetFirst(vals),ppDefault);
        os << "\"";
      }
    } else if ( type == lts_fsm || type == lts_dot )
    {
      for(; !ATisEmpty(vals); vals=ATgetNext(vals))
      {
        os << " " << ATwriteToString(ATgetArgument(ATAgetFirst(vals),0));
      }
    } else {
      for(; !ATisEmpty(vals); vals=ATgetNext(vals))
      {
        os << " \"" << ATwriteToString(ATgetFirst(vals)) << "\"";
      }
    }
    os << std::endl;;
  }

  // print states
  gsVerboseMsg("writing states...\n");
  os << "---" << std::endl;
  for(unsigned int i=0; i<nstates; i++)
  {
    unsigned int idx = i;
    // make sure initial state is first
    if ( i == 0 )
    {
      idx = init_state;
    } else if ( i == init_state )
    {
      idx = 0;
    }
    ATermList state_pars;
    if ( (type == lts_none) || !state_info )
    {
      state_pars = ATmakeList0();
    } else {
      if ( (type == lts_mcrl) || (type == lts_fsm) || (type == lts_dot) )
      {
        state_pars = (ATermList) state_values[idx];
      } else { // type == lts_mcrl2
        state_pars = ATgetArguments((ATermAppl) state_values[idx]);
      }
    }

    for(unsigned int j=0; !ATisEmpty(state_pars); state_pars=ATgetNext(state_pars),j++)
    {
      ATerm val = ATgetFirst(state_pars);
      if (j > 0) {
        os << " ";
      }
      os << ATindexedSetGetIndex(set[j],val);
    }
    os << std::endl;
  }


  // print transitions
  gsVerboseMsg("writing transitions...\n");
  os << "---" << std::endl;
  for (unsigned int i=0; i<ntransitions; i++)
  {
    unsigned int from = transitions[i].from;
    // correct state numbering
    if ( from == 0 )
    {
      from = init_state;
    } else if ( from == init_state )
    {
      from = 0;
    }
    unsigned int to = transitions[i].to;
    if ( to == 0 )
    {
      to = init_state;
    } else if ( to == init_state )
    {
      to = 0;
    }
    // correct state numbering
    os << from+1 << " " << to+1 << " \"";
    os << p_label_value_str(transitions[i].label);
    os << "\"" << std::endl;
  }

  // clean up
  if ( (type != lts_none) && state_info )
  {
    for (unsigned int i=0; i<num_params; i++)
    {
      ATindexedSetDestroy(set[i]);
    }
  }
  free(set);

  return true;
}

bool p_lts::write_to_fsm(std::string const& filename, lts_type type, ATermList params)
{
  std::ofstream os(filename.c_str());

  if ( !os.is_open() )
  {
    gsVerboseMsg("cannot open FSM file '%s' for writing\n",filename.c_str());
    return false;
  }

  return write_to_fsm(os,type,params);
}

static ATermList get_lps_params(ATerm lps)
{
  ATermList params = NULL;

  if ( lps != NULL )
  {
    if ( ATisAppl(lps) && gsIsLinProcSpec((ATermAppl) lps) )
    {
      params = ATLgetArgument(ATAgetArgument((ATermAppl) lps,3),0);
    } else if ( ATisAppl(lps) && !strcmp(ATgetName(ATgetAFun((ATermAppl) lps)),"spec2gen") )
    {
      ATermList pars = ATLgetArgument(ATAgetArgument((ATermAppl) lps,1),1);
      params = ATmakeList0();
      for (; !ATisEmpty(pars); pars=ATgetNext(pars))
      {
        params = ATinsert(params,(ATerm) ATmakeList2(ATgetArgument(ATAgetFirst(pars),0),ATgetArgument(ATAgetFirst(pars),1)));
      }
      params = ATreverse(params);
    } else {
      return false;
    }
  }

  return params;
}

static ATermList get_lps_params(lps::specification &lps)
{
  data::variable_list process_parameters(lps.process().process_parameters());
  return atermpp::term_list< data::variable >(process_parameters.begin(), process_parameters.end());
}

static bool isATermString(ATerm a)
{
  return ATisAppl(a) && (ATgetArity(ATgetAFun((ATermAppl) a)) == 0);
}

static bool ismCRL2Action(ATerm a)
{
  return ATisAppl(a) && (gsIsMultAct((ATermAppl) a) || is_timed_pair((ATermAppl) a));
}

static bool ismCRL2State(ATerm a)
{
  return ATisAppl(a) && !strcmp(ATgetName(ATgetAFun((ATermAppl) a)),"STATE");
}

static bool ismuCRLState(ATerm a)
{
  if ( ATisList(a) )
  {
    ATermList l = (ATermList) a;

    for (; !ATisEmpty(l); l=ATgetNext(l))
    {
      if ( !ATisAppl(ATgetFirst(l)) )
      {
        return false;
      }
    }

    return true;
  }

  return false;
}

static bool isFSMState(ATerm a)
{
  if ( ATisList(a) )
  {
    ATermList l = (ATermList) a;

    for (; !ATisEmpty(l); l=ATgetNext(l))
    {
      if ( ATisAppl(ATgetFirst(l)) )
      {
        ATermAppl first = (ATermAppl) ATgetFirst(l);
        AFun f = ATgetAFun(first);

        if ( (ATgetArity(f) == 2) && !strcmp(ATgetName(f),"Value") && isATermString(ATgetArgument(first,0)) && ATisAppl(ATgetArgument(first,1)) )
        {
          ATermAppl type = (ATermAppl) ATgetArgument(first,1);
          AFun g = ATgetAFun(type);

          if ( !((ATgetArity(g) == 2) && !strcmp(ATgetName(g),"Type") && isATermString(ATgetArgument(type,0)) && isATermString(ATgetArgument(type,1))) )
          {
            return false;
          }
        } else {
          return false;
        }
      } else {
        return false;
      }
    }
    return true;
  }

  return false;
}

lts_type p_lts::fsm_get_lts_type()
{
  if ( label_info )
  {
    lts_type type = lts_fsm;
    if ( nlabels > 0 )
    {
      ATerm label = label_values[0];
      if ( ismCRL2Action(label) )
      {
        type = lts_mcrl2;
      } else if ( !isATermString(label) )
      {
        return lts_none;
      }
    }
    if ( (type == lts_fsm) && state_info && (nstates > 0) )
    {
      ATerm state = state_values[0];
      // XXX better checks!
      if ( ismCRL2State(state) )
      {
        type = lts_mcrl2;
      } else if ( isFSMState(state) ) // first check FSM as FSM fits muCRL
      {
        type = lts_fsm;
      } else if ( ismuCRLState(state) )
      {
        type = lts_mcrl;
      } else {
        return lts_none;
      }
    }

    if ( type == lts_mcrl )
    {
      for (unsigned int i=0; i<nlabels; i++)
      {
        if ( !isATermString(label_values[i]) )
        {
          return lts_none;
        }
      }
      if ( state_info )
      {
        for (unsigned int i=0; i<nstates; i++)
        {
          if ( !ismuCRLState(state_values[i]) )
          {
            return lts_none;
          }
        }
      }
    } else if ( type == lts_mcrl2 )
    {
      for (unsigned int i=0; i<nlabels; i++)
      {
        if ( !ismCRL2Action(label_values[i]) )
        {
          return lts_none;
        }
      }
      if ( state_info )
      {
        for (unsigned int i=0; i<nstates; i++)
        {
          if ( !ismCRL2State(state_values[i]) )
          {
            return lts_none;
          }
        }
      }
    } else { // type == lts_fsm
      for (unsigned int i=0; i<nlabels; i++)
      {
        if ( !isATermString(label_values[i]) )
        {
          return lts_none;
        }
      }
      if ( state_info )
      {
        for (unsigned int i=0; i<nstates; i++)
        {
          if ( !isFSMState(state_values[i]) )
          {
            return lts_none;
          }
        }
      }
    }

    return type;
  } else {
    return lts_none;
  }
}

static lts_type get_lps_type(ATerm lps)
{
  if ( lps == NULL )
  {
    return lts_none;
  } else if ( ATisAppl(lps) && gsIsLinProcSpec((ATermAppl) lps) )
  {
    gsVerboseMsg("detected mCRL2 LPS\n");
    return lts_mcrl2;
  } else if ( ATisAppl(lps) && !strcmp(ATgetName(ATgetAFun((ATermAppl) lps)),"spec2gen") )
  {
    gsVerboseMsg("detected mCRL LPS\n");
    return lts_mcrl;
  } else {
    assert(0);
    gsErrorMsg("invalid LPS supplied\n");
    return lts_none;
  }
}

static bool check_type(lts_type type, ATerm lps)
{
  if ( (lps == NULL) || (type == lts_fsm) )
  {
    return true;
  } else {
    lts_type lps_type = get_lps_type(lps);
    if ( type == lps_type )
    {
      return true;
    } else {
      return false;
    }
  }
}

static bool check_type(lts_type type, lps::specification &/*spec*/)
{
  return (type == lts_mcrl2);
}

bool p_lts::read_from_fsm(std::string const& filename, ATerm lps)
{
  lts_type tmp = get_lps_type(lps);
  if ( tmp == lts_mcrl2 )
  {
    lps::specification spec(lps);
    return read_from_fsm(filename,tmp,&spec);
  } else {
    return read_from_fsm(filename,tmp);
  }
}

bool p_lts::read_from_fsm(std::string const& filename, lps::specification &spec)
{
  return read_from_fsm(filename,lts_mcrl2,&spec);
}

bool p_lts::read_from_fsm(std::istream &is, ATerm lps)
{
  lts_type tmp = get_lps_type(lps);
  if ( tmp == lts_mcrl2 )
  {
    lps::specification spec(lps);
    return read_from_fsm(is,tmp,&spec);
  } else {
    return read_from_fsm(is,tmp);
  }
}

bool p_lts::read_from_fsm(std::istream &is, lps::specification &spec)
{
  return read_from_fsm(is,lts_mcrl2,&spec);
}

bool p_lts::write_to_fsm(std::string const& filename, ATerm lps)
{
  lts_type tmp = fsm_get_lts_type();
  if ( (lps != NULL) && !check_type(tmp,lps) )
  {
    gsWarningMsg("supplied LPS cannot be used with LTS; ignoring it\n");
    return write_to_fsm(filename,tmp,NULL);
  } else {
    return write_to_fsm(filename,tmp,get_lps_params(lps));
  }
}

bool p_lts::write_to_fsm(std::string const& filename, lps::specification &spec)
{
  lts_type tmp = fsm_get_lts_type();
  if ( !check_type(tmp, spec) )
  {
    gsWarningMsg("supplied LPS cannot be used with LTS; ignoring it\n");
    return write_to_fsm(filename,tmp,NULL);
  } else {
    return write_to_fsm(filename,tmp,get_lps_params(spec));
  }
}

bool p_lts::write_to_fsm(std::ostream &os, ATerm lps)
{
  lts_type tmp = fsm_get_lts_type();
  if ( (lps != NULL) && !check_type(tmp,lps) )
  {
    gsWarningMsg("supplied LPS cannot be used with LTS; ignoring it\n");
    return write_to_fsm(os,tmp,NULL);
  } else {
    return write_to_fsm(os,tmp,get_lps_params(lps));
  }
}

bool p_lts::write_to_fsm(std::ostream &os, lps::specification &spec)
{
  lts_type tmp = fsm_get_lts_type();
  if ( !check_type(tmp, spec) )
  {
    gsWarningMsg("supplied LPS cannot be used with LTS; ignoring it\n");
    return write_to_fsm(os,tmp,NULL);
  } else {
    return write_to_fsm(os,tmp,get_lps_params(spec));
  }
}

}
}
