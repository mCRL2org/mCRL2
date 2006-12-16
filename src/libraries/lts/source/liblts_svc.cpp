#include <string>
#include <svc/svc.h>
#include "libprint_c.h"
#include "libstruct.h"
#include "lts/liblts.h"
#include "lpe/specification.h"
#include "libparse.h"
#include "typecheck.h"
#include "dataimpl.h"

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

  char *fn = strdup(filename.c_str());
  if ( SVCopen(&f,fn,SVCread,&b) )
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
      free(fn);
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
      free(fn);
      return false;
    }
  } else {
    if ( svc_type == "generic" )
    {
      gsVerboseMsg("SVC file '%s' is in the mCRL format\n",filename.c_str());
      free(fn);
      return false;
    } else if ( (svc_type == "mCRL2") || (svc_type == "mCRL2+info") )
    {
      gsVerboseMsg("SVC file '%s' is in the mCRL2 format\n",filename.c_str());
      free(fn);
      return false;
    }
    state_info = (SVCgetIndexFlag(&f) == SVCfalse);
  }

  free(fn);

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

  this->type = type;

  return true;
}

bool p_lts::write_to_svc(string const& filename, lts_type type, lpe::specification *spec)
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
          gsWarningMsg("state values are not saved as they are not the in mCRL format\n");
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
        if ( !ATisAppl(state_values[i]) ) // XXX check validity of data terms
        {
          gsWarningMsg("state values are not saved as they are not the in mCRL2 format\n");
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
        if ( (spec != NULL) )
        {
          stringstream ss(p_label_value_str(i));
          ATermAppl t = parse_mult_act(ss);
          if ( t == NULL )
          {
            gsVerboseMsg("cannot parse action as mCRL2\n");
          } else {
            t = type_check_mult_act(t,*spec);
            if ( t == NULL )
            {
              gsVerboseMsg("error type checking action\n");
            } else {
              t = implement_data_mult_act(t,*spec);
              if ( t == NULL )
              {
                gsVerboseMsg("error implementing data of action\n");
              } else {
                no_convert = false;
                label_values[i] = (ATerm) t;
                applied_conversion = true;
              }
            }
          }
        }
        if ( no_convert )
        {
          gsVerboseMsg("cannot save LTS in mCRL2 format; label values are incompatible\n");
          if ( spec == NULL )
          {
            gsVerboseMsg("using the -l/--lpe option might help\n");
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
  char *fn = strdup(filename.c_str());
  if ( SVCopen(&f,fn,SVCwrite,&b) )
  {
    gsVerboseMsg("cannot open SVC file '%s' for writing\n",fn);
    free(fn);
    return false;
  }
  free(fn);

  if ( type == lts_mcrl )
  {
    SVCsetType(&f,"generic");
  } else if ( type == lts_mcrl2 )
  {
    if ( state_info )
    {
      SVCsetType(&f,"mCRL2+info");
    } else {
      SVCsetType(&f,"mCRL2");
    }
  } else {
      SVCsetType(&f,"unknown");
  }

  if ( creator == "" )
  {
    SVCsetCreator(&f,"liblts (mCRL2)");
  } else {
    char *s = strdup(creator.c_str());
    SVCsetCreator(&f,s);
    free(s);
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

  return true;
}

}
}
