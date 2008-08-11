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

#include <set>
#include <utility>
#include "mcrl2/pbes/pbes_expression_builder.h"
#include "mcrl2/data/find.h"
#include "mcrl2/pbes/rewriter_substitution.h"
#include "mcrl2/atermpp/deque.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  /// Exception that is used as an early escape of the foreach_sequence algorithm.
  struct enumerate_quantifier_stop_early
  {
  };

  struct enumerate_quantifiers_join_and
  {
    template <typename FwdIt>
    pbes_expression operator()(FwdIt first, FwdIt last) const
    {
      return pbes_expr_optimized::join_and(first, last);
    }
  };
    
  struct enumerate_quantifiers_join_or
  {
    template <typename FwdIt>
    pbes_expression operator()(FwdIt first, FwdIt last) const
    {
      return pbes_expr_optimized::join_or(first, last);
    }
  };
    
  /// This function object is passed to the foreach_sequence algorithm, and
  /// processes a sequence of elements of D.
  template <typename PbesRewriter, typename DataSubstitutionRange>
  struct enumerate_quantifiers_helper
  {
    atermpp::set<pbes_expression>& A_;
    PbesRewriter &r_;
    const pbes_expression& phi_;
    const DataSubstitutionRange& sigma_;
    bool& is_constant_;
    pbes_expression stop_early_;
  
    enumerate_quantifiers_helper(atermpp::set<pbes_expression>& A,
                                 PbesRewriter &r,
                                 const pbes_expression& phi,
                                 DataSubstitutionRange& sigma,
                                 bool& is_constant,
                                 pbes_expression stop_early
                                )
      : A_(A), r_(r), phi_(phi), sigma_(sigma), is_constant_(is_constant), stop_early_(stop_early)
    {}
  
    void operator()() const
    {
      std::pair<pbes_expression, bool> c = r_(phi_, sigma_);
      if (c.first == stop_early_)
      {
        throw enumerate_quantifier_stop_early();
      }
      if (c.second)
      {
        is_constant_ = false;
      }
      else
      {
        A_.insert(c.first);
      }
    } 
  };
 
  /// Eliminate quantors from the expression 'forall x.sigma(phi)'
  template <typename DataSubstitutionRange,
            typename DataRewriter,
            typename DataEnumerator,
            typename PbesRewriter,
            typename JoinFunction
           >
  std::pair<pbes_expression, bool> enumerate_quantifiers(data::data_variable_list x,
                                                         const pbes_expression& phi,
                                                         DataSubstitutionRange& sigma,
                                                         DataRewriter& datar,
                                                         DataEnumerator& datae,
                                                         PbesRewriter& pbesr,
                                                         pbes_expression stop,
                                                         JoinFunction join
                                                        )
  {
    atermpp::set<pbes_expression> A;
    std::vector<atermpp::vector<pbes_rewriter_substitution> > D;
    atermpp::deque<std::pair<pbes_rewriter_substitution, unsigned int> > todo;

    // make a copy of x, to get random access
    std::vector<data::data_variable> x_(x.begin(), x.end());
  
    // initialize D and todo
    unsigned int j = 0;
    for (data::data_variable_list::iterator i = x.begin(); i != x.end(); ++i)
    {
      pbes_rewriter_substitution s(*i, datar);
      atermpp::vector<pbes_rewriter_substitution> d;
      d.push_back(s);
      D.push_back(d);
      todo.push_back(std::make_pair(s, j++));
    }
  
    // make room for adding new substitutions to sigma
    sigma.insert(sigma.end(), x.size(), pbes_rewriter_substitution());
  
    while (!todo.empty())
    {
      std::pair<pbes_rewriter_substitution, unsigned int> todo_front = todo.front();
      todo.pop_front();
      const pbes_rewriter_substitution& y = todo_front.first;
      unsigned int k = todo_front.second;
      bool is_constant = false;
  
      // save D[k] in variable Dk, as a preparation for the foreach_sequence algorithm
      atermpp::vector<pbes_rewriter_substitution> Dk = D[k];
      atermpp::vector<data::enumerator_expression> z = datae.enumerate(y);
      for (atermpp::vector<data::enumerator_expression>::iterator i = z.begin(); i != z.end(); ++i)
      {
        pbes_rewriter_substitution e(x_[k], *i, datar);
        try {
          D[k].clear();
          D[k].push_back(e);
          core::foreach_sequence(D, sigma.end() - x.size(), enumerate_quantifiers_helper<PbesRewriter, DataSubstitutionRange>(A, pbesr, phi, sigma, is_constant, stop));
          if (!is_constant)
          {
            Dk.push_back(e);
            if (!e.is_constant())
            {
              todo.push_back(std::make_pair(e, k));
            }
          }
        }
        catch (enumerate_quantifier_stop_early /* a */) {
          A.clear();
          A.insert(data::data_expr::false_());
          break;
        }
      }
      
      // restore D[k]
      D[k] = Dk;
    }
  
    sigma.erase(sigma.end() - x.size(), sigma.end());
    pbes_expression result = join(A.begin(), A.end());
    return std::pair<pbes_expression, bool>(result, false); // TODO: check 'false'
  }

  // Simplifying PBES rewriter that eliminates quantifiers using enumeration.
  template <typename DataRewriter, typename DataEnumerator>
  struct enumerate_quantifiers_builder: public simplify_rewrite_builder<DataRewriter, atermpp::vector<pbes_rewriter_substitution> >
  {
    // N.B. The substitution range for this rewriter has a fixed type!
    typedef atermpp::vector<pbes_rewriter_substitution> substitution_range;
    typedef simplify_rewrite_builder<DataRewriter, substitution_range> super;
    typedef typename super::argument_type argument_type;

    DataEnumerator& m_data_enumerator;
    
    /// Constructor.
    ///
    enumerate_quantifiers_builder(DataRewriter& r, DataEnumerator& enumerator)
      : super(r), m_data_enumerator(enumerator)
    { }
  
    /// Visit forall node.
    ///
    pbes_expression visit_forall(const pbes_expression& x, const data::data_variable_list& variables, const pbes_expression& phi, argument_type& arg)
    {
      return detail::enumerate_quantifiers(variables,
                                           phi,
                                           arg.first,
                                           super::m_data_rewriter,
                                           m_data_enumerator,
                                           *this,
                                           data::data_expr::false_(),
                                           enumerate_quantifiers_join_and()
                                          ).first;
    }
  
    /// Visit exists node.
    ///
    pbes_expression visit_exists(const pbes_expression& x, const data::data_variable_list& variables, const pbes_expression& phi, argument_type& arg)
    {
      return detail::enumerate_quantifiers(variables,
                                           phi,
                                           arg.first,
                                           super::m_data_rewriter,
                                           m_data_enumerator,
                                           *this,
                                           data::data_expr::true_(),
                                           enumerate_quantifiers_join_or()
                                          ).first;
    }

    /// Rewrites a pbes expression.
    ///
    template <typename DataSubstitutionRange>
    std::pair<pbes_expression, bool> operator()(const pbes_expression& x, DataSubstitutionRange& sigma)
    {
      bool b = false;
      argument_type a(sigma, b);
      pbes_expression result = visit(x, a);
      return std::make_pair(result, b);
    }
  };

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_ENUMERATE_QUANTIFIERS_BUILDER_H
