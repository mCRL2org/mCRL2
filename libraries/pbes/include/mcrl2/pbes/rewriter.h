// Author(s): Wieger Wesselink
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
#include "mcrl2/data/find.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/pbes/pbes_expression_builder.h"

namespace mcrl2 {

namespace pbes_system {

  struct enumerate_arguments_function
  {
    const pbes_expression& m_expr;
    const std::vector<data::data_variable>& m_src;
    const std::vector<data::data_expression>& m_dest;
    std::vector<pbes_expression>& m_expressions;
  
    enumerate_arguments_function(const pbes_expression& expr,
                                 const std::vector<data::data_variable>& src,
                                 const std::vector<data::data_expression>& dest,
                                 std::vector<pbes_expression>& expressions
                                )
      : m_expr(expr), m_src(src), m_dest(dest), m_expressions(expressions)
    {}
  
    void operator()()
    {
      m_expressions.push_back(data_variable_sequence_replace(m_expr, m_src, m_dest));
    }
  };
  
  /// This function generates all possible sequences of data expressions
  /// [x1, ..., xn] where n = distance(last, first), such that x1 is
  /// an element of *first, x2 an element of *(++first) etc. The sequences
  /// are stored in the output range [i, i+n), and for each such range
  /// the function f is called.
  template <typename Iter, typename Function>
  void enumerate_arguments(Iter first, Iter last, std::vector<data::data_expression>::iterator i, Function f)
  {
    if (first == last)
    {
      f();
    }
    else
    {
      for (std::vector<data::data_expression>::iterator j = first->begin(); j != first->end(); ++j)
      {
        *i = *j;
        enumerate_arguments(first + 1, last, i + 1, f);
      }
    }
  }

  template <class DataRewriter>
  struct pbes_simplify_expression_builder: public pbes_expression_builder
  {
    DataRewriter& m_rewriter;
    const data::data_specification& m_data;
  
    /// Cache for finite sorts
    std::map<data::sort_expression, bool> m_finite_sorts;
  
    /// Constructor.
    ///
    pbes_simplify_expression_builder(DataRewriter& r, const data::data_specification& data)
      : m_rewriter(r),
        m_data(data)
    { }
  
    /// Returns if the sort s is finite.
    /// For efficiency, the values of this function are cached.
    bool is_finite_sort(data::sort_expression s)
    {
      std::map<data::sort_expression, bool>::const_iterator i = m_finite_sorts.find(s);
      if (i != m_finite_sorts.end())
      {
        return i->second;
      }
      bool b = data::is_finite(m_data.constructors(), s);
      m_finite_sorts[s] = b;
      return m_finite_sorts[s];
    }
    
    /// Visit data expression node.
    ///
    pbes_expression visit_data_expression(const pbes_expression& /* e */, const data::data_expression& d)
    {
      return m_rewriter(d);
    }
  
    /// Visit not node.
    ///
    pbes_expression visit_not(const pbes_expression& /* e */, const pbes_expression& arg)
    {
      using namespace pbes_expr_optimized;
      if (is_true(arg))
      {
        return false_();
      }
      if (is_false(arg))
      {
        return true_();
      }
      return pbes_expression(); // continue recursion
    }
  
    /// Visit and node.
    ///
    pbes_expression visit_and(const pbes_expression& /* e */, const pbes_expression& left, const pbes_expression& right)
    {
      using namespace pbes_expr_optimized;
      if (is_true(left))
      {
        return visit(right);
      }
      if (is_true(right))
      {
        return visit(left);
      }
      if (is_false(left))
      {
        return false_();
      }
      if (is_false(right))
      {
        return false_();
      }
      if (left == right)
      {
        return visit(left);
      }
      return pbes_expression(); // continue recursion
    }
  
    /// Visit or node.
    ///
    pbes_expression visit_or(const pbes_expression& /* e */, const pbes_expression& left, const pbes_expression& right)
    {
      using namespace pbes_expr_optimized;
      if (is_true(left))
      {
        return true_();
      }
      if (is_true(right))
      {
        return true_();
      }
      if (is_false(left))
      {
        return visit(right);
      }
      if (is_false(right))
      {
        return visit(left);
      }
      if (left == right)
      {
        return visit(left);
      }
      return pbes_expression(); // continue recursion
    }    
  
    /// Visit imp node.
    ///
    pbes_expression visit_imp(const pbes_expression& /* e */, const pbes_expression& left, const pbes_expression& right)
    {
      using namespace pbes_expr_optimized;
  
      if (is_true(left))
      {
        return visit(right);
      }
      if (is_false(left))
      {
        return true_();
      }
      if (is_true(right))
      {
        return true_();
      }
      if (left == right)
      {
        return true_();
      }
      if (is_false(right))
      {
        return visit(not_(left));
      }
      return pbes_expression(); // continue recursion
    }
  };

  template <class DataRewriter>
  struct pbes_rewrite_expression_builder: public pbes_simplify_expression_builder<DataRewriter>
  {
    typedef pbes_simplify_expression_builder<DataRewriter> super;
    
    data::enumerator m_enumerator;
  
    /// Stores the quantifiers variables that are active in the current scope,
    /// but are not used (until the current node).
    std::set<data::data_variable> unused_quantifier_variables;
  
    /// Caches the ranges of values of finite sorts.
    std::map<data::sort_expression, std::vector<data::data_expression> > finite_sort_values;
  
    /// Constructor.
    ///
    pbes_rewrite_expression_builder(DataRewriter& r, const data::data_specification& data)
      : super(r, data),
        m_enumerator(r, data)
    { }
  
    /// Returns all possible values of the finite sort s.
    /// For efficiency, the values of this function are cached.
    std::vector<data::data_expression>& enumerate_values(data::sort_expression s)
    {
      std::map<data::sort_expression, std::vector<data::data_expression> >::iterator i = finite_sort_values.find(s);
      if (i != finite_sort_values.end())
      {
        return i->second;
      }
      std::vector<data::data_expression> v = m_enumerator.enumerate_finite_sort(s);
      finite_sort_values[s] = v;
      return finite_sort_values[s];
    }
  
    /// Adds the given variables to the set of unused quantifier variables.
    void push(data::data_variable_list variables)
    {
      unused_quantifier_variables.insert(variables.begin(), variables.end());
    }
  
    /// Removes the given variables from the set of unused quantifier variables.
    /// Returns the sublist of variables that does not appear in the set of
    /// unused quantifier variables.
    std::vector<data::data_variable> pop(data::data_variable_list variables)
    {
      std::vector<data::data_variable> result;
      for (data::data_variable_list::iterator i = variables.begin(); i != variables.end(); ++i)
      {
        std::set<data::data_variable>::iterator j = unused_quantifier_variables.find(*i);
        if (j == unused_quantifier_variables.end())
        {
          result.push_back(*i);
        }
        else
        {
          unused_quantifier_variables.erase(*i);
        }
      }
      return result;
    }
  
    /// Visit data expression node.
    ///
    pbes_expression visit_data_expression(const pbes_expression& /* e */, const data::data_expression& d)
    {
      data::data_expression result = super::m_rewriter(d);
  
      // remove all data variables that are present in d from unused_quantifier_variables
      std::set<data::data_variable> v = find_all_data_variables(d);
      for (std::set<data::data_variable>::iterator i = v.begin(); i != v.end(); ++i)
      {
        unused_quantifier_variables.erase(*i);
      }
  
      return result;
    }
  
    /// Removes the data variables with finite sorts from variables.
    /// Returns the removed data variables.
    std::vector<data::data_variable> remove_finite_variables(std::vector<data::data_variable>& variables)
    {
      std::vector<data::data_variable> infinite;
      std::vector<data::data_variable> finite;
      for (std::vector<data::data_variable>::iterator i = variables.begin(); i != variables.end(); ++i)
      {
        if (super::is_finite_sort(i->sort()))
        {
          finite.push_back(*i);
        }
        else
        {
          infinite.push_back(*i);
        }
      }
      std::swap(variables, infinite);
      return finite;
    }
  
    /// Visit forall node.
    ///
    pbes_expression visit_forall(const pbes_expression& /* e */, const data::data_variable_list& variables, const pbes_expression& expr)
    {
      using namespace pbes_expr_optimized;
  
      push(variables);
      pbes_expression expr1 = super::visit(expr);
      std::vector<data::data_variable> variables1 = pop(variables); // the sublist of variables that is actually used
      std::vector<data::data_variable> finite_variables = remove_finite_variables(variables1);
  
      if (variables1.size() == variables.size())
      {
        return forall(variables, expr1);
      }
  
      std::vector<std::vector<data::data_expression> > finite_value_sequences;
      for (std::vector<data::data_variable>::iterator i = finite_variables.begin(); i != finite_variables.end(); ++i)
      {
        finite_value_sequences.push_back(enumerate_values(i->sort()));
      }
      std::vector<data::data_expression> finite_variables_replacements(finite_variables.size());
      std::vector<pbes_expression> v;
      enumerate_arguments(finite_value_sequences.begin(),
                          finite_value_sequences.end(),
                          finite_variables_replacements.begin(),
                          enumerate_arguments_function(expr1, finite_variables, finite_variables_replacements, v)
                         );
      return forall(data::data_variable_list(variables1.begin(), variables1.end()), join_and(v.begin(), v.end()));
    }
  
    /// Visit exists node.
    ///
    pbes_expression visit_exists(const pbes_expression& /* e */, const data::data_variable_list& variables, const pbes_expression& expr)
    {
      using namespace pbes_expr_optimized;
  
      push(variables);
      pbes_expression expr1 = super::visit(expr);
      std::vector<data::data_variable> variables1 = pop(variables);
      std::vector<data::data_variable> finite_variables = remove_finite_variables(variables1);
  
      if (variables1.size() == variables.size())
      {
        return exists(variables, expr1);
      }
  
      std::vector<std::vector<data::data_expression> > finite_value_sequences;
      for (std::vector<data::data_variable>::iterator i = finite_variables.begin(); i != finite_variables.end(); ++i)
      {
        finite_value_sequences.push_back(enumerate_values(i->sort()));
      }
      std::vector<data::data_expression> finite_variables_replacements(finite_variables.size());
      std::vector<pbes_expression> v;
      enumerate_arguments(finite_value_sequences.begin(),
                          finite_value_sequences.end(),
                          finite_variables_replacements.begin(),
                          enumerate_arguments_function(expr1, finite_variables, finite_variables_replacements, v)
                         );
      return exists(data::data_variable_list(variables1.begin(), variables1.end()), join_or(v.begin(), v.end()));
    }
  
    /// Visit propositional variable node.
    ///
    pbes_expression visit_propositional_variable(const pbes_expression& /* e */, const propositional_variable_instantiation& v)
    {
      using namespace pbes_expr_optimized;
      return propositional_variable_instantiation(v.name(), atermpp::apply(v.parameters(), super::m_rewriter));
    }
  };

  /// \brief A rewriter class for pbes expressions.
  ///
  template <class DataRewriter>
  class simplify_rewriter
  {
    private:
      pbes_simplify_expression_builder<DataRewriter> m_builder;
  
    public:
      /// Constructor.
      ///
      simplify_rewriter(DataRewriter& r, const data::data_specification& data)
        : m_builder(r, data)
      { }
  
  		/// \brief Rewrites the pbes expression p.
  		///
  		pbes_expression operator()(const pbes_expression& p)
  		{
  		  pbes_expression result = m_builder.visit(p);
  		  return result;
  		}
  };
  
  /// \brief A rewriter class for pbes expressions.
  ///
  template <class DataRewriter>
  class rewriter
  {
    private:
      pbes_rewrite_expression_builder<DataRewriter> m_builder;
  
    public:
      /// Constructor.
      ///
      rewriter(DataRewriter& r, const data::data_specification& data)
        : m_builder(r, data)
      { }
  
  		/// \brief Rewrites the pbes expression p.
  		///
  		pbes_expression operator()(const pbes_expression& p)
  		{
  		  return m_builder.visit(p);
  		}
  };

  struct my_rewriter_substitution: public data::rewriter::substitution, public data::enumerator_expression
  {
    my_rewriter_substitution()
    {}
   
    my_rewriter_substitution(const data::data_variable& variable, data::rewriter& datar)
      : data::rewriter::substitution(datar, variable, variable),
        data::enumerator_expression(ATermAppl(variable), make_list(variable))
    {}
    
    my_rewriter_substitution(const data::data_variable& variable,
                             const data::enumerator_expression& t,
                             data::rewriter& datar
                            )
      : data::rewriter::substitution(datar, variable, t.expression()),
        data::enumerator_expression(t)
    {}
    
    std::string to_string() const
    {
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
    std::vector<atermpp::vector<my_rewriter_substitution> > D;
    atermpp::deque<std::pair<my_rewriter_substitution, unsigned int> > todo;

    // make a copy of x, to get random access
    std::vector<data::data_variable> x_(x.begin(), x.end());
  
    // initialize D and todo
    unsigned int j = 0;
    for (data::data_variable_list::iterator i = x.begin(); i != x.end(); ++i)
    {
      my_rewriter_substitution s(*i, datar);
      atermpp::vector<my_rewriter_substitution> d;
      d.push_back(s);
      D.push_back(d);
      todo.push_back(std::make_pair(s, j++));
    }
  
    // make room for adding new substitutions to sigma
    sigma.insert(sigma.end(), x.size(), my_rewriter_substitution());
  
    while (!todo.empty())
    {
      std::pair<my_rewriter_substitution, unsigned int> todo_front = todo.front();
      todo.pop_front();
      const my_rewriter_substitution& y = todo_front.first;
      unsigned int k = todo_front.second;
      bool is_constant = false;
  
      // save D[k] in variable Dk, as a preparation for the foreach_sequence algorithm
      atermpp::vector<my_rewriter_substitution> Dk = D[k];
      atermpp::vector<data::enumerator_expression> z = datae.enumerate(y);
      for (atermpp::vector<data::enumerator_expression>::iterator i = z.begin(); i != z.end(); ++i)
      {
        my_rewriter_substitution e(x_[k], *i, datar);
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

  template <typename DataRewriter>
  struct pbes_simplify_builder: public pbes_builder<std::pair<atermpp::vector<my_rewriter_substitution>, bool> >
  {
    // argument type of visit functions
    typedef pbes_builder<std::pair<atermpp::vector<my_rewriter_substitution>, bool> > super;   
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
      data::data_expression result = m_rewriter(d, arg.first.begin(), arg.first.end());
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
      return eliminate_quantors(variables,
                                phi,
                                arg.first,
                                super::m_rewriter,
                                m_enumerator,
                                *this,
                                pbes_expr::false_(),
                                my_join_and()
                               ).first;
    }
  
    /// Visit exists node.
    ///
    pbes_expression visit_exists(pbes_expression x, const data::data_variable_list& variables, const pbes_expression& phi, argument_type& arg)
    {
      return eliminate_quantors(variables,
                                phi,
                                arg.first,
                                super::m_rewriter,
                                m_enumerator,
                                *this,
                                pbes_expr::true_(),
                                my_join_or()
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
        v2.push_back(super::m_rewriter(*i, arg.first.begin(), arg.first.end()));
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
      typedef pbes_builder<std::pair<atermpp::vector<my_rewriter_substitution>, bool> > super;
      bool b = false;
      argument_type a(sigma, b);
      pbes_expression result = visit(x, a);
      return std::make_pair(result, b);
    }
  };

  template <typename DataRewriter, typename DataEnumerator>
  struct the_pbes_rewriter
  {
    pbes_rewrite_builder<DataRewriter, DataEnumerator> m_builder;

    the_pbes_rewriter(DataRewriter& r, DataEnumerator& e)
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

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITER_H
