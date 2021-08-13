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

#ifndef __REWR_PROVER_H
#define __REWR_PROVER_H

#include "mcrl2/data/detail/prover/bdd_prover.h"

namespace mcrl2
{
namespace data
{
namespace detail
{

class RewriterProver: public Rewriter
{
  public:
    BDD_Prover prover_obj;

    typedef Rewriter::substitution_type substitution_type;

  public:
    RewriterProver(const data_specification& data_spec, mcrl2::data::rewriter::strategy strat, 
                   const used_data_equation_selector& equations_selector)
      : Rewriter(data_spec, equations_selector),
        prover_obj(data_spec, equations_selector, strat)
    {
    }
    
    virtual ~RewriterProver()
    {}

    rewrite_strategy getStrategy()
    {
      switch (prover_obj.rewriter_strategy())
      {
        case jitty:
          return jitty_prover;
#ifdef MCRL2_JITTYC_AVAILABLE
        case jitty_compiling:
          return jitty_compiling_prover;
#endif
        default:
          throw mcrl2::runtime_error("invalid rewrite strategy");
      }
    }


    data_expression rewrite(
         const data_expression& t,
         substitution_type& sigma)
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
        return prover_obj.get_bdd();
      // }
      // else
      //{
      //  return prover_obj.get_rewriter()->rewrite(t,sigma);
      //}
    }

  protected:

    // Protected copy constructor.
    RewriterProver(const RewriterProver& other)
      : Rewriter(other),
        prover_obj(other.prover_obj)
    {}
    
    std::shared_ptr<Rewriter> clone()
    {
      return std::shared_ptr<Rewriter>(new RewriterProver(*this));
    }
};

}
}
}

#endif
