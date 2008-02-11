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
#include <vector>
#include "boost/ptr_container/ptr_vector.hpp"
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
/// are stored in the output range [i, i+n[, and for each such range
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
    using namespace pbes_expr;
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
    using namespace pbes_expr;
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
    using namespace pbes_expr;
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
    using namespace pbes_expr;

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
  data::enumerator m_enumerator;

  /// Stores the quantifiers variables that are active in the current scope,
  /// but are not used (until the current node).
  std::set<data::data_variable> unused_quantifier_variables;

  /// Caches the ranges of values of finite sorts.
  std::map<data::sort_expression, std::vector<data::data_expression> > finite_sort_values;

  /// Constructor.
  ///
  pbes_rewrite_expression_builder(DataRewriter& r, const data::data_specification& data)
    : pbes_simplify_expression_builder<DataRewriter>(r, data),
      m_enumerator(data)
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
    // g++ 3.4 wants to have the fully qualified name ...
    data::data_expression result = pbes_simplify_expression_builder<DataRewriter>::m_rewriter(d);

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
      if (pbes_simplify_expression_builder<DataRewriter>::is_finite_sort(i->sort()))
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
    using namespace pbes_expr;

    push(variables);
    pbes_expression expr1 = pbes_simplify_expression_builder<DataRewriter>::visit(expr);
    std::vector<data::data_variable> variables1 = pop(variables); // the sublist of variables that is actually used
    std::vector<data::data_variable> finite_variables = remove_finite_variables(variables1);

    if (variables1.size() == variables.size())
    {
      return forall(variables, expr1);
    }

    boost::ptr_vector<std::vector<data::data_expression> > finite_value_sequences;
    for (std::vector<data::data_variable>::iterator i = finite_variables.begin(); i != finite_variables.end(); ++i)
    {
      finite_value_sequences.push_back(&(enumerate_values(i->sort())));
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
    using namespace pbes_expr;

    push(variables);
    pbes_expression expr1 = pbes_simplify_expression_builder<DataRewriter>::visit(expr);
    std::vector<data::data_variable> variables1 = pop(variables);
    std::vector<data::data_variable> finite_variables = remove_finite_variables(variables1);

    if (variables1.size() == variables.size())
    {
      return exists(variables, expr1);
    }

    boost::ptr_vector<std::vector<data::data_expression> > finite_value_sequences;
    for (std::vector<data::data_variable>::iterator i = finite_variables.begin(); i != finite_variables.end(); ++i)
    {
      finite_value_sequences.push_back(&(enumerate_values(i->sort())));
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
    using namespace pbes_expr;
    return propositional_variable_instantiation(v.name(), atermpp::apply(v.parameters(), pbes_simplify_expression_builder<DataRewriter>::m_rewriter));
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

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITER_H
