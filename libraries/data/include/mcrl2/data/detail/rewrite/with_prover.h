// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/rewrite/with_prover.h
/// \brief Rewriting combined with semantic simplification using a prover

#ifndef MCRL2_DATA_DETAIL_REWR_PROVER_H
#define MCRL2_DATA_DETAIL_REWR_PROVER_H

#include "mcrl2/data/detail/prover/bdd_prover.h"

namespace mcrl2::data::detail
{

class RewriterProver: public Rewriter
{
  public:
    BDD_Prover prover_obj;

    using substitution_type = Rewriter::substitution_type;

  public:
    RewriterProver(const data_specification& data_spec, mcrl2::data::rewriter::strategy strat, 
                   const used_data_equation_selector& equations_selector)
      : Rewriter(data_spec, equations_selector),
        prover_obj(data_spec, equations_selector, strat)
    {
      thread_initialise();
    }

    ~RewriterProver() override = default;

    rewrite_strategy getStrategy() override
    {
      switch (prover_obj.rewriter_strategy())
      {
        case jitty:
          return jitty_prover;
#ifdef MCRL2_ENABLE_JITTYC
        case jitty_compiling:
          return jitty_compiling_prover;
#endif
        default:
          throw mcrl2::runtime_error("invalid rewrite strategy");
      }
    }

    void rewrite(data_expression& result, const data_expression& t, substitution_type& sigma) override
    {
      // The prover rewriter should also work on terms with other types than Bool. 
      // Up till 14/5/2020 this was not the case. The old code is left here, to
      // restore it easily, in case it turns out that the jittyp rewriter cannot
      // fruitfully be applied on expressions, other than those of type bool. 
      //
      // if (mcrl2::data::data_expression(t).sort() == mcrl2::data::sort_bool::bool_())
      // {
        prover_obj.set_substitution(sigma);
        prover_obj.set_formula(t);
        result=prover_obj.get_bdd();
        return;
      // }
      // else
      //{
      //  return prover_obj.get_rewriter()->rewrite(t,sigma);
      //}
    }

    data_expression rewrite(const data_expression& t, substitution_type& sigma) override
    {
      data_expression result;
      rewrite(result,t,sigma);
      return result;
    }

    void thread_initialise() override
    {
      Rewriter::thread_initialise();
      prover_obj.thread_initialise();
    }

  protected:

    // Protected copy constructor.
    RewriterProver(const RewriterProver& other) = delete;

    // Copy constructor intended for cloning. 
    RewriterProver(const RewriterProver& rewr, 
                   BDD_Prover prover_obj_)
      : Rewriter(rewr),
        prover_obj(prover_obj_)
    {
      thread_initialise();
    }

    std::shared_ptr<Rewriter> clone() override
    {
      return std::shared_ptr<Rewriter>(new RewriterProver(*this,prover_obj.clone()));
    }
};

}

#endif
