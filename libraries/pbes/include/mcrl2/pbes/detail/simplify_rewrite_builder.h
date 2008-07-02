// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/simplify_rewrite_builder.h
/// \brief Simplifying rewriter for pbes expressions.

#ifndef MCRL2_PBES_DETAIL_SIMPLIFY_REWRITE_BUILDER_H
#define MCRL2_PBES_DETAIL_SIMPLIFY_REWRITE_BUILDER_H

#include <set>
#include <utility>
#include "mcrl2/pbes/pbes_expression_builder.h"
#include "mcrl2/old_data/find.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  // Is called in the case rewriting is done with a substitution range.
  template <typename DataRewriter, typename SubstitutionRange>
  old_data::data_expression data_rewrite(DataRewriter rewr, old_data::data_expression d, const SubstitutionRange& sigma, bool& b)
  {
    old_data::data_expression result = rewr(d, sigma);
    std::set<old_data::data_variable> v = old_data::find_all_data_variables(result);
    b = !v.empty();
    return result;
  }

  // Is called in the case rewriting is done without a substitution range.
  template <typename DataRewriter>
  old_data::data_expression data_rewrite(DataRewriter rewr, old_data::data_expression d, const int&, bool&)
  {
    return rewr(d);
  }

  // Is called in the case rewriting is done with a substitution range.
  template <typename DataRewriter, typename SubstitutionRange>
  old_data::data_expression_list data_rewrite_list(DataRewriter rewr, old_data::data_expression_list v, const SubstitutionRange& sigma, bool& b)
  {
    std::vector<old_data::data_expression> w;
    b = false;
    for (old_data::data_expression_list::iterator i = v.begin(); i != v.end(); ++i)
    {
      old_data::data_expression d = rewr(*i, sigma);
      if (!b)
      {
        std::set<old_data::data_variable> v = old_data::find_all_data_variables(d);
        b = !v.empty();
      }
      w.push_back(d);
    }
    return old_data::data_expression_list(w.begin(), w.end());
  }

  // Is called in the case rewriting is done without a substitution range.
  template <typename DataRewriter>
  old_data::data_expression_list data_rewrite_list(DataRewriter rewr, old_data::data_expression_list v, const int&, bool&)
  {
    // TODO: there is probably a more efficient way to compute this
    std::vector<old_data::data_expression> w;
    for (old_data::data_expression_list::iterator i = v.begin(); i != v.end(); ++i)
    {
      w.push_back(rewr(*i));
    }
    return old_data::data_expression_list(w.begin(), w.end());
  }

  // Simplifying PBES rewriter.
  // If DataSubstitutionRange is of type int, then the rewriter is called without
  // a substitution range.
  template <typename DataRewriter, typename DataSubstitutionRange>
  struct simplify_rewrite_builder: public pbes_builder<std::pair<DataSubstitutionRange, bool> >
  {
    // argument type of visit functions
    typedef pbes_builder<std::pair<DataSubstitutionRange, bool> > super;   
    typedef typename super::argument_type argument_type;
      
    DataRewriter& m_data_rewriter;

    /// Constructor.
    ///
    simplify_rewrite_builder(DataRewriter& rewr)
      : m_data_rewriter(rewr)
    { }
  
    /// Visit data expression node.
    ///
    pbes_expression visit_data_expression(const pbes_expression& x, const old_data::data_expression& d, argument_type& arg)
    {
      old_data::data_expression result = data_rewrite(m_data_rewriter, d, arg.first, arg.second);
      return result;
    }
  
    /// Visit not node.
    ///
    pbes_expression visit_not(const pbes_expression& x, const pbes_expression& n, argument_type& arg)
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
    pbes_expression visit_and(const pbes_expression& x, const pbes_expression& left, const pbes_expression& right, argument_type& arg)
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
    pbes_expression visit_or(const pbes_expression& x, const pbes_expression& left, const pbes_expression& right, argument_type& arg)
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
    pbes_expression visit_imp(const pbes_expression& x, const pbes_expression& left, const pbes_expression& right, argument_type& arg)
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

    /// Visit forall node.
    ///
    pbes_expression visit_forall(const pbes_expression& x, const old_data::data_variable_list& variables, const pbes_expression& phi, argument_type& arg)
    {
      using namespace pbes_expr_optimized;
      return forall(variables, visit(phi, arg));
    }
  
    /// Visit exists node.
    ///
    pbes_expression visit_exists(const pbes_expression& x, const old_data::data_variable_list& variables, const pbes_expression& phi, argument_type& arg)
    {
      using namespace pbes_expr_optimized;
      return exists(variables, visit(phi, arg));
    }
  
    /// Visit propositional variable node.
    ///
    pbes_expression visit_propositional_variable(const pbes_expression& x, const propositional_variable_instantiation& v, argument_type& arg)
    {
      return propositional_variable_instantiation(v.name(), data_rewrite_list(m_data_rewriter, v.parameters(), arg.first, arg.second));
    }
  };

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_SIMPLIFY_REWRITE_BUILDER_H
