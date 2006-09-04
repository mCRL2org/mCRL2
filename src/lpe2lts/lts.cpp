#include <cstdio>
#include <cstring>
#include <aterm2.h>
#include "svc/svc.h" //XXX
#include "libstruct.h"
#include "libprint_c.h"
#include "lpe2lts.h"
#include "lts.h"

static lts_options lts_opts;
static ATermAppl term_nil;
static AFun afun_pair;
static unsigned long long initial_state;
static FILE *aut = NULL;
static SVCfile svcf;
static SVCfile *svc = &svcf;
static SVCparameterIndex svcparam = 0;

void open_lts(const char *filename, lts_options &opts)
{
  if ( term_nil == NULL )
  {
    term_nil = gsMakeNil();
    ATprotectAppl(&term_nil);
    afun_pair = ATmakeAFun("pair",2,ATfalse);
    ATprotectAFun(afun_pair);
  }
  lts_opts = opts;
  switch ( lts_opts.outformat )
  {
    case OF_AUT:
      gsVerboseMsg("writing state space in AUT format to '%s'.\n",filename);
      lts_opts.outinfo = false;
      if ( (aut = fopen(filename,"wb")) == NULL )
      {
        gsErrorMsg("cannot open '%s' for writing\n",filename);
        exit(1);
      }
      break;
    case OF_SVC:
      gsVerboseMsg("writing state space in SVC format to '%s'.\n",filename);
      {
        SVCbool b;
        char *t = strdup(filename);

        b = lts_opts.outinfo?SVCfalse:SVCtrue;
        SVCopen(svc,t,SVCwrite,&b); // XXX check result
        free(t);
        SVCsetCreator(svc,NAME);
        if (lts_opts.outinfo)
          SVCsetType(svc, "mCRL2+info");
        else
          SVCsetType(svc, "mCRL2");
        svcparam = SVCnewParameter(svc,(ATerm) ATmakeList0(),&b);
      }
      break;
    default:
      gsVerboseMsg("not saving state space.\n");
      break;
  }
}

void save_initial_state(unsigned long long idx, ATerm state)
{
  initial_state = idx;
  switch ( lts_opts.outformat )
  {
    case OF_AUT:
      fprintf(aut,"des (0,0,0)                   \n");
      break;
    case OF_SVC:
      {
        SVCbool b;
        if ( lts_opts.outinfo )
        {
          SVCsetInitialState(svc,SVCnewState(svc,(ATerm) lts_opts.nstate->makeStateVector(state),&b));
        } else {
          SVCsetInitialState(svc,SVCnewState(svc,(ATerm) ATmakeInt(initial_state),&b));
        }
      }
      break;
    default:
      break;
  }
}

void save_transition(unsigned long long idx_from, ATerm from, ATermAppl action, unsigned long long idx_to, ATerm to)
{
  switch ( lts_opts.outformat )
  {
    case OF_AUT:
      if ( idx_from == initial_state )
        idx_from = 0;
      if ( idx_to == initial_state )
        idx_to = 0;
      gsfprintf(aut,"(%llu,\"%P\",%llu)\n",idx_from,action,idx_to);
      fflush(aut);
      break;
    case OF_SVC:
      if ( lts_opts.outinfo )
      {
        SVCbool b;
        SVCputTransition(svc,
          SVCnewState(svc,(ATerm) lts_opts.nstate->makeStateVector(from),&b),
          SVCnewLabel(svc,(ATerm) ATmakeAppl2(afun_pair,(ATerm) action,(ATerm) term_nil),&b),
          SVCnewState(svc,(ATerm) lts_opts.nstate->makeStateVector(to),&b),
          svcparam);
      } else {
        SVCbool b;
        SVCputTransition(svc,
          SVCnewState(svc,(ATerm) ATmakeInt(idx_from),&b),
          SVCnewLabel(svc,(ATerm) ATmakeAppl2(afun_pair,(ATerm) action,(ATerm) term_nil),&b),
          SVCnewState(svc,(ATerm) ATmakeInt(idx_to),&b),
          svcparam);
      }
      break;
    default:
      break;
  }
}

void close_lts(unsigned long long num_states, unsigned long long num_trans)
{
  switch ( lts_opts.outformat )
  {
    case OF_AUT:
      rewind(aut);
      fprintf(aut,"des (0,%llu,%llu)",num_trans,num_states);
      fclose(aut);
      break;
    case OF_SVC:
      {
        int e = SVCclose(svc);
        if ( e )
        {
  	     printf("svcerror: %s\n",SVCerror(e)); 
        }
      }
      break;
    default:
      break;
  }
}
