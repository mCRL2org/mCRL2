#include <string>
#include <iostream>
#include <fstream>
#include <aterm2.h>
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "libprint.h"
#include "lts/liblts.h"
#include "lts/liblts_fsm.h"
#include "lpe/lpe.h"

#define ATisAppl(x) (ATgetType(x) == AT_APPL)
#define ATisList(x) (ATgetType(x) == AT_LIST)

namespace mcrl2
{
namespace lts
{

static bool write_lts_to_fsm(lts &l, std::ostream &os, lts_type type, ATermList params)
{
  // create arrays for fan in/out
  unsigned int *in  = (unsigned int*) malloc(l.num_states()*sizeof(unsigned int));
  unsigned int *out = (unsigned int*) malloc(l.num_states()*sizeof(unsigned int));
  if( (in==NULL) || (out==NULL) )
  {
    gsErrorMsg("malloc failed"); 
    exit(1);
  }

  // initialize arrays
  for(unsigned int i=0; i<l.num_states(); i++)
  {
    in[i]  = 0;
    out[i] = 0;
  }

  // compute fan in, fan out, and edges
  gsVerboseMsg("computing fan in/out...\n");
  unsigned int max_in = 0;
  unsigned int max_out = 0;
  for(unsigned int i=0; i<l.num_transitions(); i++)
  {
    if ( (++out[l.transition_from(i)]) > max_out )
    {
      max_out = out[l.transition_from(i)];
    }
    if ( (++in[l.transition_to(i)]) > max_in )
    {
      max_in = in[l.transition_to(i)];
    }
  }

  // determine number of state parameters
  unsigned int num_params;
  if ( (type != lts_none) && l.has_state_info() && (l.num_states() > 0) )
  {
    if ( type == lts_mcrl )
    {
      num_params = ATgetLength((ATermList) l.state_value(0));
    } else { // type == lts_mcrl2
      num_params = ATgetArity(ATgetAFun((ATermAppl) l.state_value(0)));
    }
  } else {
    num_params = 0;
  }

  // create set per parameter containing all used values
  ATermIndexedSet *set = (ATermIndexedSet*) malloc(num_params*sizeof(ATermIndexedSet));
  if ( set == NULL )
  {
    gsErrorMsg("malloc failed");
    exit(1);
  }
  if ( (type != lts_none) && l.has_state_info() )
  {
    for (unsigned int i=0; i<num_params; i++)
    {
      set[i] = ATindexedSetCreate(30,50);
    }
    for (unsigned int i=0; i<l.num_states(); i++)
    {
      ATermList state_pars;
      if ( type == lts_mcrl )
      {
        state_pars = (ATermList) l.state_value(i);
      } else { // type == mcrl2
        state_pars = ATgetArguments((ATermAppl) l.state_value(i));
      }
      
      assert( ((unsigned int) ATgetLength(state_pars)) >= num_params );
      if ( ((unsigned int) ATgetLength(state_pars)) < num_params )
      {
        gsErrorMsg("invalid state in LTS (it does not have as much parameters as the LPE)\n");
        exit(1);
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
      os << "unknown" << i << "(" << ATgetLength(vals) << ") unknown ";
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
        PrintPart_CXX(os,ATgetFirst(ATLgetFirst(params)),ppDefault);
        os << "(" << ATgetLength(vals) << ") ";
        PrintPart_CXX(os,ATgetFirst(ATgetNext(ATLgetFirst(params))),ppDefault);
        os << " ";
      }

      params = ATgetNext(params);
    }

    if ( type == lts_mcrl )
    {
      for(; !ATisEmpty(vals); vals=ATgetNext(vals))
      {
        os << " \"" << ATwriteToString(ATgetFirst(vals)) << "\"";
      }
    } else if ( type == lts_mcrl2 )
    {
      for(; !ATisEmpty(vals); vals=ATgetNext(vals))
      {
        os << " \"";
        PrintPart_CXX(os,ATgetFirst(vals),ppDefault);
        os << "\"";
      }
    }
    os << std::endl;;
  }

  // print additional "parameters"
  os << "fan_in(" << max_in+1 << ") Nat ";
  for (unsigned int i=0; i<=max_in; i++)
  {
    os << " \"" << i << "\"";
  }
  os << std::endl;

  os << "fan_out(" << max_out+1 << ") Nat ";
  for (unsigned int i=0; i<=max_out; i++)
  {
    os << " \"" << i << "\"";
  }
  os << std::endl;
 
  os << "node_nr(0)" << std::endl;


  // print states
  gsVerboseMsg("writing states...\n");
  os << "---" << std::endl;
  for(unsigned int i=0; i<l.num_states(); i++)
  {
    ATermList state_pars;
    if ( (type == lts_none) || !l.has_state_info() )
    {
      state_pars = ATmakeList0();
    } else {
      if ( type == lts_mcrl )
      {
        state_pars = (ATermList) l.state_value(i);
      } else { // type == lts_mcrl2
        state_pars = ATgetArguments((ATermAppl) l.state_value(i));
      }
    }

    for(unsigned int j=0; !ATisEmpty(state_pars); state_pars=ATgetNext(state_pars),j++)
    {
      ATerm val = ATgetFirst(state_pars);
      os << ATindexedSetGetIndex(set[j],val) << " ";
    }
    os << in[i] << " " << out[i] << " " << i+1 << std::endl;
  }
  

  // print transitions
  gsVerboseMsg("writing transitions...\n");
  os << "---" << std::endl;
  for (unsigned int i=0; i<l.num_transitions(); i++)
  {
    if ( type == lts_mcrl )
    {
      os << l.transition_from(i)+1 << " " << l.transition_to(i)+1 << " ";
      os << ATwriteToString(l.label_value(l.transition_label(i)));
      os << std::endl;
    } else if ( type == lts_mcrl2 )
    {
      os << l.transition_from(i)+1 << " " << l.transition_to(i)+1 << " \"";
      ATermAppl label = (ATermAppl) l.label_value(l.transition_label(i));
      if ( gsIsMultAct(label) )
      {
        PrintPart_CXX(os,(ATerm) label,ppDefault);
      } else {
        PrintPart_CXX(os,ATgetArgument(label,0),ppDefault);
      }
      os << "\"" << std::endl;
    } else { // type == lts_none
      os << l.transition_from(i)+1 << " " << l.transition_to(i)+1 << " \"" << l.transition_label(i) << "\"" << std::endl;
    }
  }

  // clean up
  if ( (type != lts_none) && l.has_state_info() )
  {
    for (unsigned int i=0; i<num_params; i++)
    {
      ATindexedSetDestroy(set[i]);
    }
  }
  free(set);
  free(out);
  free(in);

  return true;
}

static bool write_lts_to_fsm(lts &l, std::string &filename, lts_type type, ATermList params)
{
  std::ofstream os(filename.c_str());

  if ( !os.is_open() )
  {
    gsVerboseMsg("cannot open FSM file '%s' for writing\n",filename.c_str());
    return false;
  }

  return write_lts_to_fsm(l,os,type,params);
}

static ATermList get_lpe_params(ATerm lpe)
{
  ATermList params = NULL;

  if ( lpe != NULL )
  {
    params = ATmakeList0();

    if ( ATisAppl(lpe) && gsIsSpecV1((ATermAppl) lpe) )
    {
      ATermList pars = ATLgetArgument(ATAgetArgument((ATermAppl) lpe,5),1);
      for (; !ATisEmpty(pars); pars=ATgetNext(pars))
      {
        params = ATinsert(params,(ATerm) ATmakeList2(ATgetFirst(pars),(ATerm) gsGetSort(ATAgetFirst(pars))));
      }
      params = ATreverse(params);
    } else if ( ATisAppl(lpe) && !strcmp(ATgetName(ATgetAFun((ATermAppl) lpe)),"spec2gen") )
    {
      ATermList pars = ATLgetArgument(ATAgetArgument((ATermAppl) lpe,1),1);
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

static ATermList get_lpe_params(lpe::LPE &lpe)
{
  ATermList params = ATmakeList0();

  lpe::data_variable_list pars = lpe.process_parameters();
  lpe::data_variable_list::iterator pb = pars.begin();
  lpe::data_variable_list::iterator pe = pars.end();
  for (; pb != pe; pb++)
  {
    ATermAppl p = *pb;
    params = ATinsert(params,(ATerm) ATmakeList2((ATerm) p,(ATerm) gsGetSort(p)));
  }

  return ATreverse(params);
}

static lts_type get_lts_type(lts &l)
{
  if ( l.has_label_info() )
  {
    lts_type type;
    if ( l.num_labels() > 0 )
    {
      ATerm label = l.label_value(0);
      if ( ATisAppl(label) && (gsIsMultAct((ATermAppl) label) || is_timed_pair((ATermAppl) label)) )
      {
        type = lts_mcrl2;
      } else if ( ATisAppl(label) && (ATgetArity(ATgetAFun((ATermAppl) label)) == 0) )
      {
        type = lts_mcrl;
      } else {
        return lts_none;
      }
    } else if ( l.has_state_info() && (l.num_states() > 0) )
    {
      ATerm state = l.state_value(0);
      // XXX better checks!
      if ( ATisAppl(state) )
      {
        type = lts_mcrl2;
      } else if ( ATisList(state) )
      {
        type = lts_mcrl;
      } else {
        return lts_none;
      }
    } else {
      return lts_none;
    }

    if ( type == lts_mcrl )
    {
      for (unsigned int i=0; i<l.num_labels(); i++)
      {
        ATerm label = l.label_value(i);
        if ( !ATisAppl(label) || !(ATgetArity(ATgetAFun((ATermAppl) label)) == 0) )
        {
          return lts_none;
        }
      }
      if ( l.has_state_info() )
      {
        for (unsigned int i=0; i<l.num_states(); i++)
        {
          ATerm state = l.state_value(i);
          if ( !ATisList(state) ) // XXX
          {
            return lts_none;
          }
        }
      }
    } else { // type == lts_mcrl2
      for (unsigned int i=0; i<l.num_labels(); i++)
      {
        ATerm label = l.label_value(i);
        if ( !ATisAppl(label) || !(gsIsMultAct((ATermAppl) label) || is_timed_pair((ATermAppl) label)) )
        {
          return lts_none;
        }
      }
      if ( l.has_state_info() )
      {
        for (unsigned int i=0; i<l.num_states(); i++)
        {
          ATerm state = l.state_value(i);
          if ( !ATisAppl(state) ) // XXX
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

static lts_type get_lpe_type(ATerm lpe)
{
  if ( ATisAppl(lpe) && gsIsSpecV1((ATermAppl) lpe) )
  {
    gsVerboseMsg("detected mCRL2 LPE\n");
    return lts_mcrl2;
  } else if ( ATisAppl(lpe) && !strcmp(ATgetName(ATgetAFun((ATermAppl) lpe)),"spec2gen") )
  {
    gsVerboseMsg("detected mCRL LPE\n");
    return lts_mcrl;
  } else {
    assert(0);
    gsErrorMsg("invalid LPE supplied\n");
    return lts_none;
  }
}

static bool check_types(lts &l, ATerm lpe, lts_type *type)
{
  *type = get_lts_type(l);
  if ( (lpe == NULL) || (*type == lts_none) )
  {
    return true;
  } else {
    lts_type lpe_type = get_lpe_type(lpe);
    if ( *type == lpe_type )
    {
      return true;
    } else {
      gsVerboseMsg("supplied LPE cannot be used with LTS\n");
      return false;
    }
  }
}

static bool check_types(lts &l, lpe::LPE &/*lpe*/, lts_type *type)
{
  *type = lts_mcrl2;
  return (get_lts_type(l) == lts_mcrl2);
}

bool write_lts_to_fsm(lts &l, std::string &filename, ATerm lpe)
{
  lts_type type;
  return check_types(l,lpe,&type) && write_lts_to_fsm(l,filename,type,get_lpe_params(lpe));
}

bool write_lts_to_fsm(lts &l, std::string &filename, lpe::LPE &lpe)
{
  lts_type type;
  return check_types(l,lpe,&type) && write_lts_to_fsm(l,filename,type,get_lpe_params(lpe));
}

bool write_lts_to_fsm(lts &l, std::ostream &os, ATerm lpe)
{
  lts_type type;
  return check_types(l,lpe,&type) && write_lts_to_fsm(l,os,type,get_lpe_params(lpe));
}

bool write_lts_to_fsm(lts &l, std::ostream &os, lpe::LPE &lpe)
{
  lts_type type;
  return check_types(l,lpe,&type) && write_lts_to_fsm(l,os,type,get_lpe_params(lpe));
}

}
}
