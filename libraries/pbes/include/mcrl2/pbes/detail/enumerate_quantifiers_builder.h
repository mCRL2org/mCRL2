// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/enumerate_quantifiers_builder.h
/// \brief Simplifying rewriter for pbes expressions that eliminates quantifiers using enumeration.

#ifndef MCRL2_PBES_DETAIL_ENUMERATE_QUANTIFIERS_BUILDER_H
#define MCRL2_PBES_DETAIL_ENUMERATE_QUANTIFIERS_BUILDER_H

#include <numeric>
#include <set>
#include <utility>
#include <deque>
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>
#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/optimized_boolean_operators.h"
#include "mcrl2/core/sequence.h"
#include "mcrl2/core/detail/join.h"
#include "mcrl2/pbes/detail/simplify_rewrite_builder.h"
#include "gc.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {


  template <typename Sequence>
  bool empty_intersection(Sequence s1, Sequence s2)
  {
    for (typename Sequence::const_iterator i = s1.begin(); i != s1.end(); ++i)
    {
      if (std::find(s2.begin(), s2.end(), *i) != s2.end())
      {
        return false;
      }
    }
    return true;
  }

  /// Exception that is used as an early escape of the foreach_sequence algorithm.
  struct enumerate_quantifier_stop_early
  {};

  template <typename Term>
  struct enumerate_quantifiers_join_and
  {
    template <typename FwdIt>
    Term operator()(FwdIt first, FwdIt last) const
    {
      return std::accumulate(first, last, core::term_traits<Term>::true_(), &core::optimized_and<Term>);
    }
  };
    
  template <typename Term>
  struct enumerate_quantifiers_join_or
  {
    template <typename FwdIt>
    Term operator()(FwdIt first, FwdIt last) const
    {
      return std::accumulate(first, last, core::term_traits<Term>::false_(), &core::optimized_or<Term>);
    }
  };

  template <typename MapSubstitution>
  struct enumerate_quantifiers_sequence_assign
  {
    typedef typename MapSubstitution::variable_type variable_type;
    typedef typename MapSubstitution::term_type     term_type;

    MapSubstitution& sigma_;
    
    enumerate_quantifiers_sequence_assign(MapSubstitution& sigma)
      : sigma_(sigma)
    {}

    void operator()(variable_type v, term_type t)
    {
      sigma_[v] = t;
    }
  };

  /// This function object is passed to the foreach_sequence algorithm.
  /// It is invoked for every sequence of substitutions of the set Z in
  /// the algorithm.
  template <typename PbesTermSet, 
            typename PbesRewriter,
            typename PbesTerm,
            typename SubstitutionFunction,
            typename VariableSequence,
            typename StopCriterion
           >
  struct enumerate_quantifiers_sequence_action
  {
    PbesTermSet&         A_;
    PbesRewriter&        r_;
    const PbesTerm&      phi_;
    SubstitutionFunction sigma_;
    VariableSequence     v_;
    bool&                is_constant_;
    StopCriterion        stop_;
  
    enumerate_quantifiers_sequence_action(PbesTermSet& A,
                                 PbesRewriter &r,
                                 const PbesTerm& phi,
                                 SubstitutionFunction sigma,
                                 VariableSequence v,
                                 bool& is_constant,
                                 StopCriterion stop
                                )
      : A_(A), r_(r), phi_(phi), sigma_(sigma), v_(v), is_constant_(is_constant), stop_(stop)
    {}

    void operator()()
    {
      PbesTerm c = r_(phi_, sigma_);
#ifdef MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
  std::cout << "r(sigma(phi)) = " << core::pp(c) << " is " << (core::term_traits<PbesTerm>::is_constant(c) ? "" : "not ") << "constant" << std::endl;
#endif
      if (stop_(c))
      {
        throw enumerate_quantifier_stop_early();
      }
      else if (empty_intersection(v_, c.variables()))
      {
#ifdef MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
  std::cout << "<A.insert> " << core::pp(c) << std::endl;
#endif
        A_.insert(c);
      }
      else
      {
        is_constant_ = false;
      }
    } 
  };

  template <typename PbesTermSet, 
            typename PbesRewriter,
            typename PbesTerm,
            typename SubstitutionFunction,
            typename VariableSequence,
            typename StopCriterion
           >
  enumerate_quantifiers_sequence_action<PbesTermSet, PbesRewriter, PbesTerm, SubstitutionFunction, VariableSequence, StopCriterion>
  make_enumerate_quantifiers_sequence_action(PbesTermSet& A,
                                 PbesRewriter &r,
                                 const PbesTerm& phi,
                                 SubstitutionFunction sigma,
                                 VariableSequence v,
                                 bool& is_constant,
                                 StopCriterion stop
                                )
  {
    return enumerate_quantifiers_sequence_action<PbesTermSet, PbesRewriter, PbesTerm, SubstitutionFunction, VariableSequence, StopCriterion>(A, r, phi, sigma, v, is_constant, stop);
  }
 
  /// Eliminate quantifiers from the expression 'forall x.sigma(phi)'
  /// This algorithm is documented in the 'PBES implementation notes' document.
  template <typename DataVariableSequence,
            typename PbesTerm,
            typename MapSubstitution,
            typename DataEnumerator,
            typename PbesRewriter,
            typename StopCriterion,
            typename PbesTermJoinFunction
           >
    PbesTerm enumerate_quantifiers(DataVariableSequence x,
                                   const PbesTerm& phi,
                                   MapSubstitution& sigma,
                                   DataEnumerator& datae,
                                   PbesRewriter& pbesr,
                                   StopCriterion stop,
                                   PbesTerm stop_value,
                                   PbesTermJoinFunction join
                                  )
  {
#ifdef MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG2
std::cout << "x, phi, sigma = " << core::pp(x) << "\n" << core::pp(phi) << "\nstop = " << core::pp(stop_value) << std::endl;
std::cout << "--- sigma ---" << std::endl;
for (typename MapSubstitution::const_iterator i = sigma.begin(); i != sigma.end(); ++i)
{
  std::cout << core::pp(i->first) << " -> " << core::pp(i->second) << std::endl;
}
std::cout << "-------------" << std::endl;
#endif

    PbesTerm Rphi = pbesr(phi, sigma);
    if (core::term_traits<PbesTerm>::is_constant(Rphi))
    {
      return Rphi;
    }

    typedef typename DataEnumerator::variable_type variable_type;
    typedef typename DataEnumerator::term_type data_term_type;
    typedef PbesTerm pbes_term_type;
    typedef std::pair<variable_type, data_term_type> data_assignment;   

#ifdef MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
  std::cout << "x = " << core::pp(x) << std::endl;
  std::cout << "phi = " << core::pp(phi) << std::endl;
  std::cout << "stop_value = " << core::pp(stop_value) << std::endl;
#endif

    atermpp::set<pbes_term_type> A;
    std::vector<atermpp::vector<data_term_type> > D;

    // For an element (v, t, k) of todo, we have the invariant v == x[k].
    // The variable v is stored for efficiency reasons, it avoids the lookup x[k].
    std::deque<boost::tuple<variable_type, data_term_type, unsigned int> > todo;

    // initialize D and todo
    unsigned int j = 0;
    for (typename DataVariableSequence::const_iterator i = x.begin(); i != x.end(); ++i)
    {
      data_term_type t = core::term_traits<data_term_type>::variable2term(*i);
      D.push_back(atermpp::vector<data_term_type>(1, t));
#ifdef MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
  std::cout << "D[" << j << "] = {" << core::pp(t) << "}" << std::endl;
  std::cout << "todo = todo + (" << core::pp(*i) << ", " << j << ")" << std::endl;
#endif
      todo.push_back(boost::make_tuple(*i, t, j++));
    }

//std::cout << "collect 0 " << std::endl;
//AT_collect();

    try
    {
      while (!todo.empty())
      {
//std::cout << "collect 1 " << std::flush;
//AT_collect();
        boost::tuple<variable_type, data_term_type, unsigned int> front = todo.front();
//std::cout << "done" << std::endl;
        todo.pop_front();
        const variable_type& xk = boost::get<0>(front);
        const data_term_type& y = boost::get<1>(front);
        unsigned int k          = boost::get<2>(front);
#ifdef MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
  std::cout << "---\nchoose y = " << core::pp(y) << ", k = " << k << ", xk = " << core::pp(xk) << std::endl;
#endif
        bool is_constant = false;

        // save D[k] in variable Dk, as a preparation for the foreach_sequence algorithm
        atermpp::vector<data_term_type> Dk = D[k];
#ifdef MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG2
std::cout << "enumerating y: " << core::pp(y) << std::endl;
#endif
        atermpp::vector<data_term_type> z = datae.enumerate(y);
        for (typename atermpp::vector<data_term_type>::iterator i = z.begin(); i != z.end(); ++i)
        {
#ifdef MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
  std::cout << "e = " << core::pp(*i) << std::endl;
#endif
          sigma[xk] = *i;
          D[k].clear();
          D[k].push_back(*i);
          core::foreach_sequence(D,
                                 x.begin(),
                                 make_enumerate_quantifiers_sequence_action(A, pbesr, phi, sigma, i->variables(), is_constant, stop),
                                 enumerate_quantifiers_sequence_assign<MapSubstitution>(sigma)
                                );
#ifdef MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
  std::cout << "|A| = " << A.size() << std::endl;
  std::cout << "Z is " << (is_constant ? "" : "not ") << "constant" << std::endl;
#endif
          if (!is_constant)
          {
            Dk.push_back(*i);
            if (!core::term_traits<data_term_type>::is_constant(*i))
            {
#ifdef MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
  std::cout << "todo = todo + (" << core::pp(*i) << ", " << k << ")" << std::endl;
#endif
              todo.push_back(boost::make_tuple(xk, *i, k));
            }
          }
        }
 
        // restore D[k]
        D[k] = Dk;
      }
    }
    catch (enumerate_quantifier_stop_early /* a */)
    {
      return stop_value;
    }
  
    // remove the added substitutions from sigma
    for (typename DataVariableSequence::const_iterator i = x.begin(); i != x.end(); ++i)
    {
      sigma.erase(*i);
    }
#ifdef MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
  std::cout << "join(";
  for (typename atermpp::set<pbes_term_type>::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    std::cout << ", " << core::pp(*i);
  }
  std::cout << ") = -> " << core::pp(join(A.begin(), A.end())) << std::endl;
#endif
    return join(A.begin(), A.end());
  }

  // Simplifying PBES rewriter that eliminates quantifiers using enumeration.
  /// \param[in] SubstitutionFunction This must be a MapSubstitution.
  template <typename Term, typename DataRewriter, typename DataEnumerator, typename SubstitutionFunction>
  struct enumerate_quantifiers_builder: public simplify_rewrite_builder<Term, DataRewriter, SubstitutionFunction>
  {
    typedef simplify_rewrite_builder<Term, DataRewriter, SubstitutionFunction> super;
    typedef SubstitutionFunction                                               argument_type;
    typedef typename super::term_type                                          term_type;
    typedef typename core::term_traits<term_type>::data_term_type              data_term_type;             
    typedef typename core::term_traits<term_type>::data_term_sequence_type     data_term_sequence_type;    
    typedef typename core::term_traits<term_type>::variable_sequence_type variable_sequence_type;
    typedef typename core::term_traits<term_type>::propositional_variable_type propositional_variable_type;
    typedef core::term_traits<Term> tr;

    DataEnumerator& m_data_enumerator;
    
    /// Constructor.
    ///
    enumerate_quantifiers_builder(DataRewriter& r, DataEnumerator& enumerator)
      : super(r), m_data_enumerator(enumerator)
    { }
  
  
    /// Visit forall node.
    ///
    term_type visit_forall(const term_type& x, const variable_sequence_type& variables, const term_type& phi, SubstitutionFunction& sigma)
    {
      return detail::enumerate_quantifiers(variables,
                                           phi,
                                           sigma,
                                           m_data_enumerator,
                                           *this,
                                           tr::is_false,
                                           tr::false_(),
                                           enumerate_quantifiers_join_and<Term>()
                                          );
    }
  
    /// Visit exists node.
    ///
    term_type visit_exists(const term_type& x, const variable_sequence_type& variables, const term_type& phi, SubstitutionFunction& sigma)
    {
      return detail::enumerate_quantifiers(variables,
                                           phi,
                                           sigma,
                                           m_data_enumerator,
                                           *this,
                                           tr::is_true,
                                           tr::true_(),
                                           enumerate_quantifiers_join_or<Term>()
                                          );
    }
  };

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_ENUMERATE_QUANTIFIERS_BUILDER_H
