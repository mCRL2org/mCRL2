// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/old_rewriter.h
/// \brief Rewriter for pbes expressions.

#ifndef MCRL2_PBES_DETAIL_REWRITER_H
#define MCRL2_PBES_DETAIL_REWRITER_H

#include <set>
#include <utility>
#include <vector>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include "mcrl2/atermpp/deque.h"
#include "mcrl2/old_data/find.h"
#include "mcrl2/old_data/rewriter.h"
#include "mcrl2/old_data/enumerator.h"
#include "mcrl2/old_data/replace.h"
#include "mcrl2/pbes/pbes_expression_builder.h"

#include "mcrl2/atermpp/vector.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/core/sequence.h"
#include "mcrl2/old_data/enum.h"
#include "mcrl2/old_data/rewriter.h"
#include "mcrl2/old_data/data_specification.h"
#include "mcrl2/old_data/replace.h"
#include "mcrl2/utilities/aterm_ext.h"


namespace mcrl2 {

namespace old_data {

/// \cond INTERNAL_DOCS

namespace detail {

  /// Helper class for enumerate_data_variable_substitutions function. Every time operator()
  /// is called, the result of applying the given sequence of substitutions to the single
  /// term rewriter is appended to the result.
  struct enumerate_substitutions_helper
  {
    const single_term_rewriter& m_rewriter;
    const atermpp::vector<rewriter::substitution>& m_substitutions;
    atermpp::set<data_expression>& m_result;
  
    enumerate_substitutions_helper(const single_term_rewriter& rewriter,
                                   const atermpp::vector<rewriter::substitution>& substitutions,
                                   atermpp::set<data_expression>& result
                                  )
     : m_rewriter(rewriter),
       m_substitutions(substitutions),
       m_result(result)
    {}
    
    void operator()()
    {
      m_result.insert(m_rewriter(m_substitutions));
    }
  };
  
  /// This function generates all possible sequences of substitutions
  /// [d1 := d1_, ..., dn := dn_] where [first, last[ = [d1 ... dn].
  /// Each sequence of substitutions is stored in the output range [i, i+n[,
  /// and for each sequence the function f is called.
  template <typename Iter, typename Function>
  void enumerate_substitutions(atermpp::vector<rewriter::substitution>::iterator i, Iter first, Iter last, Function f)
  {
    if (first == last)
    {
      f();
    }
    else
    {
      for (atermpp::vector<rewriter::substitution>::iterator j = first->begin(); j != first->end(); ++j)
      {
        *i = *j;
        enumerate_substitutions(i+1, first + 1, last, f);
      }
    }
  }

} // namespace detail

/// \endcond

class enumerator
{
  protected:
    rewriter& m_rewriter;
    boost::shared_ptr<Enumerator> m_enumerator;

    typedef std::map<data_variable, atermpp::vector<rewriter::substitution> > substition_map;

    /// Caches the ranges of values of data variables of finite sort.
    substition_map m_finite_sort_substitutions;

    /// Returns all possible values of the data variable d, by means of a sequence of
    /// substitutions [d := d1, d := d2, ..., d := dn). For efficiency, such sequences
    /// are cached.
    atermpp::vector<rewriter::substitution>& enumerate_variable(data_variable d)
    {
      substition_map::iterator i = m_finite_sort_substitutions.find(d);
      if (i != m_finite_sort_substitutions.end())
      {
        return i->second;
      }
      atermpp::vector<data_expression> v = enumerate_finite_sort(d.sort());
      atermpp::vector<rewriter::substitution> w;
      for (std::vector<data_expression>::iterator i = v.begin(); i != v.end(); ++i)
      {
        w.push_back(rewriter::substitution(m_rewriter, d, *i));
      }
      m_finite_sort_substitutions[d] = w;
      return m_finite_sort_substitutions[d];
    }

  public:
    enumerator(rewriter& rewr, const data_specification& data_spec)
      : m_rewriter(rewr),
        m_enumerator(createEnumerator(data_spec, m_rewriter.m_rewriter.get()))
    {
    }
    
    /// Returns all possible values of the finite sort s.
    atermpp::vector<data_expression> enumerate_finite_sort(old_data::sort_expression s)
    {
      atermpp::vector<data_expression> result;
      data_variable dummy(core::identifier_string("dummy"), s);

      // find all elements of sort s by enumerating all valuations of dummy
      // that make the expression "true" true
      EnumeratorSolutions* sols = m_enumerator.get()->findSolutions(atermpp::make_list(dummy), m_rewriter.m_rewriter.get()->toRewriteFormat(data_expr::true_()));
      ATermList l; // variable to store a solution
      while (sols->next(&l)) // get next solution
      {
        // l is of the form [subst(x,expr)] where expr is in rewriter format
        atermpp::aterm_appl tmp = utilities::ATAgetFirst(l);
        data_expression d(m_rewriter.m_rewriter.get()->fromRewriteFormat(tmp(1)));
        result.push_back(d);
      }     
      return result;
    }

    /// Returns all expressions of the form rewrite(phi([d1 := d1_, ..., dn := dn_])),
    /// where d1 ... dn are data variables of the sequence [first, last[, and where
    /// d1_ ... dn_ are data expressions that vary over all possible values of the
    /// variables d1 ... dn. The variables d1 ... dn must have finite sorts.
    ///
    template <typename Iter>
    atermpp::set<data_expression> enumerate_expression_values(const data_expression& phi, Iter first, Iter last)
    {
      single_term_rewriter r(m_rewriter, phi);
//      boost::ptr_vector<atermpp::vector<rewriter::substitution> > substitution_sequences; // this fails miserably...
      std::vector<atermpp::vector<rewriter::substitution> > substitution_sequences;
      for (Iter i = first; i != last; ++i)
      {
//        substitution_sequences.push_back(&(enumerate_variable(*i)));
        substitution_sequences.push_back(enumerate_variable(*i));
      }
      atermpp::vector<rewriter::substitution> substitutions(std::distance(first, last));
      atermpp::set<data_expression> result;
      detail::enumerate_substitutions(substitutions.begin(),
                                      substitution_sequences.begin(),
                                      substitution_sequences.end(),
                                      detail::enumerate_substitutions_helper(r, substitutions, result)
                                     );
      return result;
    }
};

} // namespace data

namespace pbes_system {

namespace detail {

  struct enumerate_arguments_function
  {
    const pbes_expression& m_expr;
    const std::vector<old_data::data_variable>& m_src;
    const std::vector<old_data::data_expression>& m_dest;
    std::vector<pbes_expression>& m_expressions;
  
    enumerate_arguments_function(const pbes_expression& expr,
                                 const std::vector<old_data::data_variable>& src,
                                 const std::vector<old_data::data_expression>& dest,
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
  void enumerate_arguments(Iter first, Iter last, std::vector<old_data::data_expression>::iterator i, Function f)
  {
    if (first == last)
    {
      f();
    }
    else
    {
      for (std::vector<old_data::data_expression>::iterator j = first->begin(); j != first->end(); ++j)
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
    const old_data::data_specification& m_data;
  
    /// Cache for finite sorts
    std::map<old_data::sort_expression, bool> m_finite_sorts;
  
    /// Constructor.
    ///
    pbes_simplify_expression_builder(DataRewriter& r, const old_data::data_specification& data)
      : m_rewriter(r),
        m_data(data)
    { }
  
    /// Returns if the sort s is finite.
    /// For efficiency, the values of this function are cached.
    bool is_finite_sort(old_data::sort_expression s)
    {
      std::map<old_data::sort_expression, bool>::const_iterator i = m_finite_sorts.find(s);
      if (i != m_finite_sorts.end())
      {
        return i->second;
      }
      bool b = old_data::is_finite(m_data.constructors(), s);
      m_finite_sorts[s] = b;
      return m_finite_sorts[s];
    }
    
    /// Visit data expression node.
    ///
    pbes_expression visit_data_expression(const pbes_expression& /* e */, const old_data::data_expression& d)
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
    
    old_data::enumerator m_enumerator;
  
    /// Stores the quantifiers variables that are active in the current scope,
    /// but are not used (until the current node).
    std::set<old_data::data_variable> unused_quantifier_variables;
  
    /// Caches the ranges of values of finite sorts.
    std::map<old_data::sort_expression, std::vector<old_data::data_expression> > finite_sort_values;
  
    /// Constructor.
    ///
    pbes_rewrite_expression_builder(DataRewriter& r, const old_data::data_specification& data)
      : super(r, data),
        m_enumerator(r, data)
    { }
  
    /// Returns all possible values of the finite sort s.
    /// For efficiency, the values of this function are cached.
    std::vector<old_data::data_expression>& enumerate_values(old_data::sort_expression s)
    {
      std::map<old_data::sort_expression, std::vector<old_data::data_expression> >::iterator i = finite_sort_values.find(s);
      if (i != finite_sort_values.end())
      {
        return i->second;
      }
      std::vector<old_data::data_expression> v = m_enumerator.enumerate_finite_sort(s);
      finite_sort_values[s] = v;
      return finite_sort_values[s];
    }
  
    /// Adds the given variables to the set of unused quantifier variables.
    void push(old_data::data_variable_list variables)
    {
      unused_quantifier_variables.insert(variables.begin(), variables.end());
    }
  
    /// Removes the given variables from the set of unused quantifier variables.
    /// Returns the sublist of variables that does not appear in the set of
    /// unused quantifier variables.
    std::vector<old_data::data_variable> pop(old_data::data_variable_list variables)
    {
      std::vector<old_data::data_variable> result;
      for (old_data::data_variable_list::iterator i = variables.begin(); i != variables.end(); ++i)
      {
        std::set<old_data::data_variable>::iterator j = unused_quantifier_variables.find(*i);
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
    pbes_expression visit_data_expression(const pbes_expression& /* e */, const old_data::data_expression& d)
    {
      old_data::data_expression result = super::m_rewriter(d);
  
      // remove all data variables that are present in d from unused_quantifier_variables
      std::set<old_data::data_variable> v = find_all_data_variables(d);
      for (std::set<old_data::data_variable>::iterator i = v.begin(); i != v.end(); ++i)
      {
        unused_quantifier_variables.erase(*i);
      }
  
      return result;
    }
  
    /// Removes the data variables with finite sorts from variables.
    /// Returns the removed data variables.
    std::vector<old_data::data_variable> remove_finite_variables(std::vector<old_data::data_variable>& variables)
    {
      std::vector<old_data::data_variable> infinite;
      std::vector<old_data::data_variable> finite;
      for (std::vector<old_data::data_variable>::iterator i = variables.begin(); i != variables.end(); ++i)
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
    pbes_expression visit_forall(const pbes_expression& /* e */, const old_data::data_variable_list& variables, const pbes_expression& expr)
    {
      using namespace pbes_expr_optimized;
  
      push(variables);
      pbes_expression expr1 = super::visit(expr);
      std::vector<old_data::data_variable> variables1 = pop(variables); // the sublist of variables that is actually used
      std::vector<old_data::data_variable> finite_variables = remove_finite_variables(variables1);
  
      if (variables1.size() == variables.size())
      {
        return forall(variables, expr1);
      }
  
      std::vector<std::vector<old_data::data_expression> > finite_value_sequences;
      for (std::vector<old_data::data_variable>::iterator i = finite_variables.begin(); i != finite_variables.end(); ++i)
      {
        finite_value_sequences.push_back(enumerate_values(i->sort()));
      }
      std::vector<old_data::data_expression> finite_variables_replacements(finite_variables.size());
      std::vector<pbes_expression> v;
      enumerate_arguments(finite_value_sequences.begin(),
                          finite_value_sequences.end(),
                          finite_variables_replacements.begin(),
                          enumerate_arguments_function(expr1, finite_variables, finite_variables_replacements, v)
                         );
      return forall(old_data::data_variable_list(variables1.begin(), variables1.end()), join_and(v.begin(), v.end()));
    }
  
    /// Visit exists node.
    ///
    pbes_expression visit_exists(const pbes_expression& /* e */, const old_data::data_variable_list& variables, const pbes_expression& expr)
    {
      using namespace pbes_expr_optimized;
  
      push(variables);
      pbes_expression expr1 = super::visit(expr);
      std::vector<old_data::data_variable> variables1 = pop(variables);
      std::vector<old_data::data_variable> finite_variables = remove_finite_variables(variables1);
  
      if (variables1.size() == variables.size())
      {
        return exists(variables, expr1);
      }
  
      std::vector<std::vector<old_data::data_expression> > finite_value_sequences;
      for (std::vector<old_data::data_variable>::iterator i = finite_variables.begin(); i != finite_variables.end(); ++i)
      {
        finite_value_sequences.push_back(enumerate_values(i->sort()));
      }
      std::vector<old_data::data_expression> finite_variables_replacements(finite_variables.size());
      std::vector<pbes_expression> v;
      enumerate_arguments(finite_value_sequences.begin(),
                          finite_value_sequences.end(),
                          finite_variables_replacements.begin(),
                          enumerate_arguments_function(expr1, finite_variables, finite_variables_replacements, v)
                         );
      return exists(old_data::data_variable_list(variables1.begin(), variables1.end()), join_or(v.begin(), v.end()));
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
  class rewriter
  {
    private:
      pbes_rewrite_expression_builder<DataRewriter> m_builder;
  
    public:
      /// Constructor.
      ///
      rewriter(DataRewriter& r, const old_data::data_specification& data)
        : m_builder(r, data)
      { }
  
  		/// \brief Rewrites the pbes expression p.
  		///
  		pbes_expression operator()(const pbes_expression& p)
  		{
  		  return m_builder.visit(p);
  		}
  };

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_REWRITER_H
