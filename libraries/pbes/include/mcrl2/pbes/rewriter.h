// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriter.h
/// \brief Rewriter for pbes expressions.

#ifndef MCRL2_PBES_REWRITER_H
#define MCRL2_PBES_REWRITER_H

#include <set>
#include <utility>
#include <vector>
#include <boost/bind.hpp>
#include "mcrl2/atermpp/deque.h"
#include "mcrl2/core/print.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/pbes/pbes_expression_builder.h"

#include "mcrl2/core/data_implementation.h"
#include "mcrl2/pbes/utility.h"
#include "mcrl2/pbes/gauss.h"

namespace mcrl2 {

namespace pbes_system {

/// \cond INTERNAL_DOCS
namespace detail {

  struct pbes_rewriter_substitution: public data::rewriter::substitution, public data::enumerator_expression
  {
    pbes_rewriter_substitution()
    {}
   
    pbes_rewriter_substitution(const data::data_variable& variable, data::rewriter& datar)
      : data::rewriter::substitution(datar, variable, variable),
        data::enumerator_expression(ATermAppl(variable), make_list(variable))
    {}
    
    pbes_rewriter_substitution(const data::data_variable& variable,
                             const data::enumerator_expression& t,
                             data::rewriter& datar
                            )
      : data::rewriter::substitution(datar, variable, t.expression()),
        data::enumerator_expression(t)
    {}
    
    std::string to_string() const
    {
      using ::pp;
      return pp(expression()) + " " + pp(variables());
    }
  };
  
  /// Exception that is used as an early escape of the foreach_sequence algorithm.
  struct eliminate_quantor_stop_early
  {
  };

  struct my_join_and
  {
    template <typename FwdIt>
    pbes_expression operator()(FwdIt first, FwdIt last) const
    {
      return pbes_expr_optimized::join_and(first, last);
    }
  };
    
  struct my_join_or
  {
    template <typename FwdIt>
    pbes_expression operator()(FwdIt first, FwdIt last) const
    {
      return pbes_expr_optimized::join_or(first, last);
    }
  };
    
  /// This function object is passed to the foreach_sequence algorithm, and
  /// processes a sequence of elements of D.
  template <typename PbesRewriter, typename SubstitutionSequence>
  struct eliminate_quantors_helper
  {
    atermpp::set<pbes_expression>& A_;
    PbesRewriter &r_;
    const pbes_expression& phi_;
    const SubstitutionSequence& sigma_;
    bool& is_constant_;
    pbes_expression stop_early_;
  
    eliminate_quantors_helper(atermpp::set<pbes_expression>& A,
                              PbesRewriter &r,
                              const pbes_expression& phi,
                              SubstitutionSequence& sigma,
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
        throw eliminate_quantor_stop_early();
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
  template <typename SubstitutionSequence,
            typename DataRewriter,
            typename DataEnumerator,
            typename PbesRewriter,
            typename JoinFunction
           >
  std::pair<pbes_expression, bool> eliminate_quantors(data::data_variable_list x,
                                                      const pbes_expression& phi,
                                                      SubstitutionSequence& sigma,
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
          core::foreach_sequence(D, sigma.end() - x.size(), eliminate_quantors_helper<PbesRewriter, SubstitutionSequence>(A, pbesr, phi, sigma, is_constant, stop));
          if (!is_constant)
          {
            Dk.push_back(e);
            if (!e.is_constant())
            {
              todo.push_back(std::make_pair(e, k));
            }
          }
        }
        catch (eliminate_quantor_stop_early /* a */) {
          A.clear();
          A.insert(pbes_expr::false_());
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

} // namespace detail
/// \endcond

  template <typename DataRewriter>
  struct pbes_simplify_builder: public pbes_builder<std::pair<atermpp::vector<detail::pbes_rewriter_substitution>, bool> >
  {
    // argument type of visit functions
    typedef pbes_builder<std::pair<atermpp::vector<detail::pbes_rewriter_substitution>, bool> > super;   
    typedef typename super::argument_type argument_type;
      
    DataRewriter& m_rewriter;
  
    /// Constructor.
    ///
    pbes_simplify_builder(DataRewriter& rewr)
      : m_rewriter(rewr)
    { }
  
    /// Visit data expression node.
    ///
    pbes_expression visit_data_expression(pbes_expression x, const data::data_expression& d, argument_type& arg)
    {
      data::data_expression result = m_rewriter(d, arg.first);
      std::set<data::data_variable> v = data::find_all_data_variables(result);
      arg.second = !v.empty();
      return result;
    }
  
    /// Visit not node.
    ///
    pbes_expression visit_not(pbes_expression x, const pbes_expression& n, argument_type& arg)
    {
      using namespace pbes_expr_optimized;
      if (is_true(n))
      {
        arg.second = false;
        return false_();
      }
      if (is_false(n))
      {
        arg.second = false;
        return true_();
      }
      return pbes_expression(); // continue recursion
    }
  
    /// Visit and node.
    ///
    pbes_expression visit_and(pbes_expression x, const pbes_expression& left, const pbes_expression& right, argument_type& arg)
    {
      using namespace pbes_expr_optimized;
      if (is_true(left))
      {
        return super::visit(right, arg);
      }
      if (is_true(right))
      {
        return super::visit(left, arg);
      }
      if (is_false(left))
      {
        arg.second = false;
        return false_();
      }
      if (is_false(right))
      {
        arg.second = false;
        return false_();
      }
      if (left == right)
      {
        return super::visit(left, arg);
      }
      return pbes_expression(); // continue recursion
    }
  
    /// Visit or node.
    ///
    pbes_expression visit_or(pbes_expression x, const pbes_expression& left, const pbes_expression& right, argument_type& arg)
    {
      using namespace pbes_expr_optimized;
      if (is_true(left))
      {
        arg.second = false;
        return true_();
      }
      if (is_true(right))
      {
        arg.second = false;
        return true_();
      }
      if (is_false(left))
      {
        return super::visit(right, arg);
      }
      if (is_false(right))
      {
        return super::visit(left, arg);
      }
      if (left == right)
      {
        return super::visit(left, arg);
      }
      return pbes_expression(); // continue recursion
    }    
  
    /// Visit imp node.
    ///
    pbes_expression visit_imp(pbes_expression x, const pbes_expression& left, const pbes_expression& right, argument_type& arg)
    {
      using namespace pbes_expr_optimized;
  
      if (is_true(left))
      {
        return super::visit(right, arg);
      }
      if (is_false(left))
      {
        arg.second = false;
        return true_();
      }
      if (is_true(right))
      {
        arg.second = false;
        return true_();
      }
      if (left == right)
      {
        arg.second = false;
        return true_();
      }
      if (is_false(right))
      {
        return super::visit(not_(left), arg);
      }
      return pbes_expression(); // continue recursion
    }
  };

  template <typename DataRewriter, typename DataEnumerator>
  struct pbes_rewrite_builder: public pbes_simplify_builder<DataRewriter>
  {
    typedef pbes_simplify_builder<DataRewriter> super;
    typedef typename super::argument_type argument_type;

    DataEnumerator& m_enumerator;
    
    /// Constructor.
    ///
    pbes_rewrite_builder(DataRewriter& r, DataEnumerator& enumerator)
      : super(r), m_enumerator(enumerator)
    { }
  
    /// Visit forall node.
    ///
    pbes_expression visit_forall(pbes_expression x, const data::data_variable_list& variables, const pbes_expression& phi, argument_type& arg)
    {
      return detail::eliminate_quantors(variables,
                                        phi,
                                        arg.first,
                                        super::m_rewriter,
                                        m_enumerator,
                                        *this,
                                        pbes_expr::false_(),
                                        detail::my_join_and()
                                       ).first;
    }
  
    /// Visit exists node.
    ///
    pbes_expression visit_exists(pbes_expression x, const data::data_variable_list& variables, const pbes_expression& phi, argument_type& arg)
    {
      return detail::eliminate_quantors(variables,
                                        phi,
                                        arg.first,
                                        super::m_rewriter,
                                        m_enumerator,
                                        *this,
                                        pbes_expr::true_(),
                                        detail::my_join_or()
                                       ).first;
    }
  
    /// Visit propositional variable node.
    ///
    pbes_expression visit_propositional_variable(pbes_expression x, const propositional_variable_instantiation& v, argument_type& arg)
    {
      // data::data_expression_list params = atermpp::apply(v.parameters(), boost::bind<data::data_expression>(super::m_rewriter, _2)(arg.first));
      data::data_expression_list v1 = v.parameters();
      std::vector<data::data_expression> v2;
      for (data::data_expression_list::iterator i = v1.begin(); i != v1.end(); ++i)
      {
        v2.push_back(super::m_rewriter(*i, arg.first));
      }
      data::data_expression_list params(v2.begin(), v2.end());
      std::set<data::data_variable> w = data::find_all_data_variables(params);     
      arg.second = !w.empty();
      return propositional_variable_instantiation(v.name(), params);
    }

    /// Rewrites a pbes expression.
    ///
    pbes_expression operator()(const pbes_expression& x)
    {
      argument_type arg;
      return super::visit(x, arg);
    }

    /// Rewrites a pbes expression.
    ///
    template <typename SubstitutionSequence>
    std::pair<pbes_expression, bool> operator()(const pbes_expression& x, SubstitutionSequence& sigma)
    {
      bool b = false;
      argument_type a(sigma, b);
      pbes_expression result = visit(x, a);
      return std::make_pair(result, b);
    }
  };

  template <typename DataRewriter, typename DataEnumerator>
  struct pbes_rewriter
  {
    pbes_rewrite_builder<DataRewriter, DataEnumerator> m_builder;

    pbes_rewriter(DataRewriter& r, DataEnumerator& e)
      : m_builder(r, e)
    { }
   
    /// Rewrites a pbes expression.
    ///
    pbes_expression operator()(const pbes_expression& x)
    {
      typename pbes_rewrite_builder<DataRewriter, DataEnumerator>::argument_type arg;
      return m_builder.visit(x, arg);
    }

    /// Rewrites a pbes expression.
    /// \return (r, b), where r is the rewrite result, and b denotes whether or not r depends on sigma
    ///
    template <typename SubstitutionSequence>
    std::pair<pbes_expression, bool> operator()(const pbes_expression& x, SubstitutionSequence& sigma)
    {
      bool b = false;
      pbes_expression result = m_builder.visit(x, std::make_pair(sigma, b));
      return std::make_pair(result, b);
    }
  };

  class simplify_rewriter_jfg
  {
    data::rewriter datar;
    
    public:
      simplify_rewriter_jfg(const data::data_specification& data)
        : datar(data)
      { }
      
      pbes_expression operator()(pbes_expression p)
      {
        return pbes_expression_rewrite_and_simplify(p, datar.get_rewriter());
      }
  };
  
  class substitute_rewriter_jfg
  {
    data::rewriter& datar_;
    const data::data_specification& data_spec;
   
    public:
      substitute_rewriter_jfg(data::rewriter& datar, const data::data_specification& data)
        : datar_(datar), data_spec(data)
      { }
      
      pbes_expression operator()(pbes_expression p)
      {
        return pbes_expression_substitute_and_rewrite(p, data_spec, datar_.get_rewriter(), false);
      }   
  };
  
  class pbessolve_rewriter
  {
    data::rewriter& datar_;
    const data::data_specification& data_spec;
    int n;
    data_variable_list fv;
    boost::shared_ptr<BDD_Prover> prover;
   
    public:
      pbessolve_rewriter(data::rewriter& datar, const data::data_specification& data, RewriteStrategy rewrite_strategy, SMT_Solver_Type solver_type)
        : datar_(datar),
          data_spec(data),
          n(0),
          prover(new BDD_Prover(data_spec, rewrite_strategy, 0, false, solver_type, false))
      { }
  
      pbes_expression operator()(pbes_expression p)
      {
        return pbes_expression_simplify(p, &n, &fv, prover.get());
      }   
  };

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITER_H
