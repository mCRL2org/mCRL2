// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts.cpp

#include <cstring>
#include "mcrl2/lps/specification.h"
#include "aterm2.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/print.h"
#include "mcrl2/lts/lps2lts_lts.h"
#include "mcrl2/lts/detail/lts_convert.h"
#include "mcrl2/lts/lts_io.h"


using namespace std;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::lts;
using namespace mcrl2::lts::detail;

namespace mcrl2
{
  namespace lts
  {
    #define NAME "lps2lts"

    void lps2lts_lts::open_lts(const char *filename, lps2lts_lts_options &opts)
    {
      lts_filename = std::string(filename);
      if ( term_nil == atermpp::aterm_appl() )
      {
        term_nil = gsMakeNil();
        term_nil.protect();
        afun_pair = atermpp::function_symbol("pair", 2, false);
        afun_pair.protect();
      }
      lts_opts = opts;
      switch ( lts_opts.outformat )
      {
        case lts_none:
          gsVerboseMsg("not saving state space.\n");
          break;
        case lts_aut:
          gsVerboseMsg("writing state space in AUT format to '%s'.\n",filename);
          lts_opts.outinfo = false;
          aut.open(filename);
          if ( !aut.is_open() )
          {
            gsErrorMsg("cannot open '%s' for writing\n",filename);
            exit(1);
          }
          break;
        /* case lts_lts:
          gsVerboseMsg("writing state space in mCRL2 format to '%s'.\n",filename);
          {
            SVCbool b;

            b = lts_opts.outinfo?SVCfalse:SVCtrue;
            SVCopen(svc,const_cast< char* > (filename),SVCwrite,&b); // XXX check result

            SVCsetCreator(svc,const_cast < char* > (NAME));
            if (lts_opts.outinfo)
              SVCsetType(svc, const_cast < char* > ("mCRL2+info"));
            else
              SVCsetType(svc, const_cast < char* > ("mCRL2"));
            svcparam = SVCnewParameter(svc,(ATerm) ATmakeList0(),&b);
          }
          break;
        */
        default:
          gsVerboseMsg("writing state space in %s format to '%s'.\n",
                      mcrl2::lts::detail::string_for_type(lts_opts.outformat).c_str(),filename);
          // generic_lts = new lts_extra();
          generic_lts.set_creator(lts_filename);
          // generic_lts.set_lts_terms(true);
          generic_lts.set_data(lts_opts.spec->data());
          generic_lts.set_process_parameters(lts_opts.spec->process().process_parameters());
          generic_lts.set_action_labels(lts_opts.spec->action_labels());
          aterm2state = ATindexedSetCreate(10000,50);
          aterm2label = ATindexedSetCreate(100,50);
          break;
      }
    }

    void lps2lts_lts::save_initial_state(size_t idx, ATerm state)
    {
      initial_state = idx;
      switch ( lts_opts.outformat )
      {
        case lts_none:
          break;
        case lts_aut:
          aut << "des (0,0,0)                                      " << endl;
          break;
        /* case lts_lts:
          {
            SVCbool b;
            if ( lts_opts.outinfo )
            {
              SVCsetInitialState(svc,SVCnewState(svc,(ATerm) lts_opts.nstate->makeStateVector(state),&b));
            } else {
              SVCsetInitialState(svc,SVCnewState(svc,(ATerm) ATmakeInt(initial_state),&b));
            }
          }
          break; */
        default:
          {
            ATbool is_new;
            const size_t t = ATindexedSetPut(aterm2state,state,&is_new);
            if ( is_new && lts_opts.outinfo)
            {
              const size_t u = generic_lts.add_state(state_label_lts(lts_opts.nstate->makeStateVector(state)));
              assert(u==t);
              static_cast <void>(u); // Avoid a warning when compiling in non debug mode.
            }
            assert(t>=0);
            // generic_lts.set_initial_state(ATgetInt((ATermInt) t));
            generic_lts.set_initial_state(t);
          }
          break;
      }
    }

    void lps2lts_lts::save_transition(size_t idx_from, ATerm from, ATermAppl action, size_t idx_to, ATerm to)
    {
      switch ( lts_opts.outformat )
      {
        case lts_none:
          break;
        case lts_aut:
          if ( idx_from == initial_state )
            idx_from = 0;
          if ( idx_to == initial_state )
            idx_to = 0;
          aut << "(" << idx_from << ",\"";
          PrintPart_CXX(aut,(ATerm) action,ppDefault);
          aut << "\"," << idx_to << ")" << endl;
          aut.flush();
          break;
        /* case lts_lts:   the lts type is not treated in a special way anymore.
          if ( lts_opts.outinfo )
          {
            SVCbool b;
            SVCputTransition(svc,
              SVCnewState(svc,(ATerm) lts_opts.nstate->makeStateVector(from),&b),
              SVCnewLabel(svc,(ATerm) ATmakeAppl2(AFun(afun_pair),(ATerm) action,(ATerm) static_cast<ATermAppl>(term_nil)),&b),
              SVCnewState(svc,(ATerm) lts_opts.nstate->makeStateVector(to),&b),
              svcparam);
          } else {
            SVCbool b;
            SVCputTransition(svc,
              SVCnewState(svc,(ATerm) ATmakeInt(idx_from),&b),
              SVCnewLabel(svc,(ATerm) ATmakeAppl2(AFun(afun_pair),(ATerm) action,(ATerm) static_cast<ATermAppl>(term_nil)),&b),
              SVCnewState(svc,(ATerm) ATmakeInt(idx_to),&b),
              svcparam);
          }
          break; */
        default:
          {
            ATbool is_new;
            const size_t from_state = ATindexedSetPut(aterm2state,from,&is_new);
            if ( is_new && lts_opts.outinfo )
            {
              const size_t t = generic_lts.add_state(state_label_lts(lts_opts.nstate->makeStateVector(from)));
              assert(t==from_state);
              static_cast <void>(t); // Avoid a warning when compiling in non debug mode.
            }
            const size_t to_state = ATindexedSetPut(aterm2state,to,&is_new);
            if ( is_new && lts_opts.outinfo )
            {
              const size_t t = generic_lts.add_state(state_label_lts(lts_opts.nstate->makeStateVector(to)));
              assert(t==to_state);
              static_cast <void>(t); // Avoid a warning when compiling in non debug mode.
            }
            const size_t label = ATindexedSetPut(aterm2label,(ATerm) action,&is_new);
            if ( is_new )
            {
              const size_t t = generic_lts.add_label((ATerm) action, ATisEmpty((ATermList) ATgetArgument(action,0)) == ATtrue);
              assert(t==label);
              static_cast <void>(t); // Avoid a warning when compiling in non debug mode.
            }
            generic_lts.add_transition(transition(from_state,label,to_state));
          }
          break;
      }
    }

    void lps2lts_lts::close_lts(size_t num_states, size_t num_trans)
    {
      switch ( lts_opts.outformat )
      {
        case lts_none:
          break;
        case lts_aut:
          aut.seekp(0);
          aut << "des (0," << num_trans << "," << num_states << ")";
          aut.close();
          break;
        /* case lts_lts:
          {
            int e = SVCclose(svc);
            if ( e )
            {
              gsErrorMsg("svcerror: %s\n",SVCerror(e));
            }
            add_extra_mcrl2_lts_data(
                         lts_filename,
                         mcrl2::data::detail::data_specification_to_aterm_data_spec(lts_opts.spec->data()),
                         lts_opts.spec->process().process_parameters(), lts_opts.spec->action_labels());
          } 
          break; */
        default:
          {
            generic_lts.set_creator(lts_filename);
            generic_lts.set_data(lts_opts.spec->data());
            generic_lts.set_process_parameters(lts_opts.spec->process().process_parameters());
            generic_lts.set_action_labels(lts_opts.spec->action_labels());            
            switch (lts_opts.outformat)
            {
              case lts_none: 
              {
                assert(0); break;
              }
              case lts_lts:
              {
                generic_lts.save(lts_filename); 
                break;
              }
              case lts_aut: 
              {
                assert(0); break;
              }
              case lts_fsm:
              {
                lts_fsm_t l;
                detail::lts_convert(generic_lts,l);
                l.save(lts_filename); 
                break;
              }
#ifdef USE_BCG
              case lts_bcg:
              {
                lts_bcg_t l;
                detail::lts_convert(generic_lts,l);
                l.save(lts_filename); 
                break;
              }
#endif
              case lts_dot:
              {
                lts_dot_t l;
                detail::lts_convert(generic_lts,l);
                l.save(lts_filename); 
                break;
              }
              default: assert(0); // lts_aut and lts_none cannot occur.
            }

            ATindexedSetDestroy(aterm2label);
            ATindexedSetDestroy(aterm2state);
            break;
          }
      }

      // Avoid static initialisation/descruction fiasco
      lts_opts.spec.reset();
    }

    void lps2lts_lts::remove_lts()
    {
      switch ( lts_opts.outformat )
      {
        case lts_aut:
          aut.close();
          break;
        /* case lts_lts:
          {
            int e = SVCclose(svc);
            if ( e )
            {
              gsErrorMsg("svcerror: %s\n",SVCerror(e));
            }
          }
          break; */
        case lts_none:
          break;
        default:
          ATindexedSetDestroy(aterm2label);
          ATindexedSetDestroy(aterm2state);
          break;
      }
      remove(lts_filename.c_str());
    }

  }
}


