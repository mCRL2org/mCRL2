// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriter.h
/// \brief Rewriters for pbes expressions.

#ifndef MCRL2_PBES_REWRITER_H
#define MCRL2_PBES_REWRITER_H

#include <set>
#include <vector>
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/data_variable_replace.h"
#include "mcrl2/pbes/pbes_expression_builder.h"

namespace mcrl2 {

namespace pbes_system {

template <class DataRewriter>
struct pbes_rewrite_expression_builder: public pbes_expression_builder
{
  DataRewriter& m_rewriter;
  data::enumerator m_enumerator;
  const data::data_specification& m_data;

  /// Stores the quantifiers variables that are active in the current scope,
  /// but are not used (until the current node).
  std::set<data::data_variable> unused_quantifier_variables;

  /// Constructor.
  ///
  pbes_rewrite_expression_builder(DataRewriter& r, const data::data_specification& data)
    : m_rewriter(r),
      m_enumerator(data),
      m_data(data)
  { }

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

  bool is_true(const pbes_expression& p) const
  {
    return pbes_expr::is_true(p) || data::data_expr::is_true(p);
  }

  bool is_false(const pbes_expression& p) const
  {
    return pbes_expr::is_false(p) || data::data_expr::is_false(p);
  }

  /// Visit data expression node.
  ///
  pbes_expression visit_data_expression(const pbes_expression& /* e */, const data::data_expression& d)
  {
    data::data_expression result = m_rewriter(d);

    // remove all data variables that are present in d from unused_quantifier_variables
    std::set<data::data_variable> v = find_variables(d);
    for (std::set<data::data_variable>::iterator i = v.begin(); i != v.end(); ++i)
    {
      unused_quantifier_variables.erase(*i);
    }

    return result;
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
      return right;
    }
    if (is_true(right))
    {
      return visit(left);
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
    if (is_true(left) || is_true(right))
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
      return visit(pbes_expr_unoptimized::not_(left));
    }
    return pbes_expression(); // continue recursion
  }

  /// Removes the data variables with finite sorts from variables.
  /// Returns the removed data variables.
  std::vector<data::data_variable> remove_finite_variables(std::vector<data::data_variable>& variables)
  {
    std::vector<data::data_variable> infinite;
    std::vector<data::data_variable> finite;
    for (std::vector<data::data_variable>::iterator i = variables.begin(); i != variables.end(); ++i)
    {
      if (is_finite_sort(i->sort()))
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
    pbes_expression expr1 = visit(expr);
    std::vector<data::data_variable> variables1 = pop(variables);
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
    pbes_expression expr1 = visit(expr);
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
    return propositional_variable_instantiation(v.name(), atermpp::apply(v.parameters(), m_rewriter));
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
