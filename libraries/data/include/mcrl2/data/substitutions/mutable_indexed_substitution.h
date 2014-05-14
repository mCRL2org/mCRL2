// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/substitutions/mutable_indexed_substitution.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_SUBSTITUTIONS_MUTABLE_INDEXED_SUBSTITUTION_H
#define MCRL2_DATA_SUBSTITUTIONS_MUTABLE_INDEXED_SUBSTITUTION_H

#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include "mcrl2/data/is_simple_substitution.h"
#include "mcrl2/data/undefined.h"
#include "mcrl2/utilities/exception.h"

namespace mcrl2 {

namespace data {

/// \brief Generic substitution function.
/// \details This substitution assumes a function variable -> size_t, that, for
///          each variable gives a unique index. The substitutions are stored
///          internally as a vector, mapping size_t to expression.
///          Provided that, given a variable, its index can be computed in O(1)
///          time, insertion is O(1) amortized, and lookup is O(1).
///          Memory required is O(n) where n is the largest index used.
template <typename VariableType = data::variable, typename ExpressionSequence = std::vector<data_expression> >
class mutable_indexed_substitution : public std::unary_function<VariableType, typename ExpressionSequence::value_type>
{
protected:
  /// \brief Internal storage for substitutions.
  /// Required to be a container with random access through [] operator.
  ExpressionSequence m_container;
  std::vector <size_t> m_index_table;
  std::stack<size_t> m_free_positions;
  bool m_variables_in_rhs_set_is_defined;
  std::set<variable> m_variables_in_rhs;

public:

  /// \brief Friend function to get all identifiers in the substitution
  template<typename VariableType1, typename ExpressionSequence1>
  friend std::set<core::identifier_string> get_identifiers(const mutable_indexed_substitution< VariableType1, ExpressionSequence1 >& sigma);

  /// \brief Type of variables
  typedef VariableType variable_type;

  /// \brief Type of expressions
  typedef typename ExpressionSequence::value_type expression_type;

  /// \brief Default constructor
  mutable_indexed_substitution()
    : m_variables_in_rhs_set_is_defined(false)
  {
  }

  /// \brief Wrapper class for internal storage and substitution updates using operator()
  struct assignment
  {
    const variable_type& m_variable;
    ExpressionSequence& m_container;
    std::vector <size_t>& m_index_table;
    std::stack<size_t>& m_free_positions;
    bool m_variables_in_rhs_set_is_defined;
    std::set<variable>& m_variables_in_rhs;


    /// \brief Constructor.
    ///
    /// \param[in] v a variable.
    /// \param[in] c a container of expressions.
    /// \param[in] table a table of indices
    /// \param[in] fp a stack of free positions in \a table
    assignment(const variable_type& v, ExpressionSequence& c, std::vector <size_t>& table, std::stack<size_t>& fp,
               const bool b, std::set<variable>& vars) :
      m_variable(v),
      m_container(c),
      m_index_table(table),
      m_free_positions(fp),
      m_variables_in_rhs_set_is_defined(b),
      m_variables_in_rhs(vars)
    { }

    /// \brief Actual assignment
    template <typename AssignableToExpression>
    void operator=(AssignableToExpression const& e)
    {
      mCRL2log(log::debug2, "substitutions") << "Setting " << m_variable << " := " << e << std::endl;
      assert(e.defined());

      size_t i = core::index_traits<data::variable, data::variable_key_type, 2>::index(m_variable);

      if (e != m_variable)
      {
        // Set a new variable;
        if (m_variables_in_rhs_set_is_defined)
        {
           m_variables_in_rhs=find_free_variables(e);
        }

        // Resize container if needed
        if (i >= m_index_table.size())
        {
          m_index_table.resize(i+1, size_t(-1));
        }

        size_t j=m_index_table[i];
        assert(j==size_t(-1) || j<m_container.size());
        if (j==size_t(-1))
        {
          // The variable was not assigned.
          if (m_free_positions.empty())
          {
            m_index_table[i]=m_container.size();
            m_container.push_back(e);
          }
          else
          {
            j=m_free_positions.top();
            m_index_table[i]=j;
            m_container[j]=e;
            m_free_positions.pop();
          }
        }
        else
        {
          // The variable was already assigned. Replace the assignment.
          // Note that we do not remove the variables in the term that is replaced.
          m_container[j]=e;
        }
      }
      else
      {
        // Indicate that the current variable is free; postpone deleting the
        // actual value assigned to the variable.
        // Note that we do not remove variables in variables_in_rhs;
        if (i<m_index_table.size())
        {
          size_t j=m_index_table[i];
          if (j!=size_t(-1))
          {
            m_free_positions.push(j);
            m_index_table[i]=size_t(-1);
          }
        }
      }
    }
  };

  /// \brief Application operator; applies substitution to v.
  const expression_type& operator()(const variable_type& v) const
  {
    const size_t i = core::index_traits<data::variable, data::variable_key_type, 2>::index(v);
    if (i < m_index_table.size())
    {
      const size_t j = m_index_table[i];
      if (j!=size_t(-1))
      {
        // the variable has an assigned value.
        assert(j<m_container.size());
        return m_container[j];
      }
    }
    // no value assigned to v;
    return v;
  }

  /// \brief Index operator.
  assignment operator[](variable_type const& v)
  {
    return assignment(v, m_container,m_index_table,m_free_positions,m_variables_in_rhs_set_is_defined,m_variables_in_rhs);
  }

  /// \brief Clear substitutions.
  void clear()
  {
    m_index_table.clear();
    m_container.clear();
    m_free_positions=std::stack<size_t>();
    m_variables_in_rhs_set_is_defined=false;
    m_variables_in_rhs.clear();
  }

  /// \brief Compare substitutions
  template <typename Substitution>
  bool operator==(const Substitution&) const
  {
    return false;
  }

  /// \brief Provides a set of variables that occur in the right hand sides of the assignments.
  const std::set<variable>& variables_in_rhs()
  {
    if (!m_variables_in_rhs_set_is_defined)
    {
      for(std::vector<size_t> ::const_iterator i=m_index_table.begin(); i != m_index_table.end(); ++i)
      {
        if (*i != size_t(-1))
        {
          m_variables_in_rhs=find_free_variables(m_container[*i]);
        }
      }
      m_variables_in_rhs_set_is_defined=true;
    }
    return m_variables_in_rhs;
  }

  /// \brief Returns true if the substitution is empty
  bool empty()
  {
    assert(m_container.size()>=m_free_positions.size());
    return m_container.size()==m_free_positions.size();
  }

protected:
  /// \brief size of the wrapped container
  size_t size() const
  {
    return m_container.size();
  }

  /// \brief set position i of the wrapped container to e
  void set(const size_t i, const expression_type& e)
  {
    mCRL2log(log::debug2, "substitutions") << "Setting " << static_cast<atermpp::function_symbol>(i).name() << " := " << e << std::endl;
    m_container[i] = e;
  }

  /// \brief get the element at position i of the wrapped container
  const expression_type& get(const size_t i) const
  {
    assert(i < m_index_table.size());
    assert(m_index_table[i]!=size_t(-1));
    return m_container[m_index_table[i]];
  }

public:
  /// \brief string representation of the substitution
  std::string to_string() const
  {
    std::stringstream result;
    bool first = true;
    result << "[";
    for (size_t i = 0; i < m_index_table.size(); ++i)
    {
      if (m_index_table[i] != size_t(-1))
      {
        if (first)
        {
          first = false;
        }
        else
        {
          result << "; ";
        }
        result << core::identifier_string(static_cast<atermpp::function_symbol>(m_index_table[i]).name()) << " := " << get(i);
      }
    }
    result << "]";
    return result.str();
  }

};

template <typename VariableType, typename ExpressionSequence>
std::ostream& operator<<(std::ostream& out, const mutable_indexed_substitution<VariableType, ExpressionSequence>& sigma)
{
  return out << sigma.to_string();
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SUBSTITUTIONS_MUTABLE_INDEXED_SUBSTITUTION_H
