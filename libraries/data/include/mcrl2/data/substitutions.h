// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/substitutions.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_SUBSTITUTIONS_H
#define MCRL2_DATA_SUBSTITUTIONS_H

#include <functional>
#include <iterator>
#include "mcrl2/data/find.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/is_simple_substitution.h"

namespace mcrl2 {

namespace data {

namespace detail
{
// The function below gets all free variables of the term t, which has
// the shape of an expression in internal format. The variables are added to result.
// This routine should be removed after internal and external format have merged.
inline void get_free_variables(const atermpp::aterm &t1, std::set < variable > &result)
{
  const atermpp::aterm_appl &t=static_cast<const atermpp::aterm_appl&>(t1);
  if (is_variable(t))
  {
    result.insert(variable(t));
  }
  else if (is_where_clause(t))
  {
    std::set < variable > free_variables_in_body;
    get_free_variables(t[0],free_variables_in_body);

    variable_list bound_vars;
    const assignment_list lv=assignment_list(t[1]);
    for(assignment_list :: const_iterator it=lv.begin() ; it!=lv.end(); ++it)
    {
      bound_vars.push_front(it->lhs());
      get_free_variables(it->rhs(),result);
    }
    for(std::set < variable > :: const_iterator i=free_variables_in_body.begin(); i!=free_variables_in_body.end(); ++i)
    {
      if (std::find(bound_vars.begin(),bound_vars.end(),*i)==bound_vars.end()) // not found, and hence variable *i is not bound.
      {
        result.insert(*i);
      }
    }
  }
  else if (is_abstraction(t))
  {
    std::set < variable > free_variables_in_body;
    get_free_variables(t[2],free_variables_in_body);
    const variable_list bound_vars=variable_list(t[1]);

    for(std::set < variable > :: const_iterator i=free_variables_in_body.begin(); i!=free_variables_in_body.end(); ++i)
    {
      if (std::find(bound_vars.begin(),bound_vars.end(),*i)==bound_vars.end()) // not found, and hence variable *i is not bound.
      {
        result.insert(*i);
      }
    }
  }
  else // Term has the shape #REWR#(t1,...,tn);
  {
    for(size_t i=0 ; i<t.size(); ++i)
    {
      if (!t[i].type_is_int())
      {
        get_free_variables(t[i],result);
      }
    }
  }
}
} // end namespace detail
/// \brief Returns true if FV(rhs) is included in {lhs}.
inline
bool is_simple_substitution(const data::variable& lhs, const data::data_expression& rhs)
{
  std::set<data::variable> v = data::find_free_variables(rhs);
  if (v.empty())
  {
    return true;
  }
  if (v.size() == 1)
  {
    return *(v.begin()) == lhs;
  }
  return false;
}

/// \brief Substitution that maps a sort expression to a sort expression.
struct sort_expression_assignment: public std::unary_function<sort_expression, sort_expression>
{
  typedef sort_expression variable_type;
  typedef sort_expression expression_type;

  sort_expression lhs;
  sort_expression rhs;

  sort_expression_assignment(const sort_expression& lhs_, const sort_expression& rhs_)
  : lhs(lhs_),
    rhs(rhs_)
  {}

  const sort_expression &operator()(const sort_expression& x)
  {
    if (x == lhs)
    {
      return rhs;
    }
    return x;
  }
};

/// \brief Substitution that maps a data expression to a data expression
struct data_expression_assignment: public std::unary_function<data_expression, data_expression>
{
  typedef data_expression variable_type;
  typedef data_expression expression_type;

  data_expression lhs;
  data_expression rhs;

  data_expression_assignment(const data_expression& lhs_, const data_expression& rhs_)
  : lhs(lhs_),
    rhs(rhs_)
  {}

  const data_expression &operator()(const data_expression& x)
  {
    if (x == lhs)
    {
      return rhs;
    }
    return x;
  }
};

/// \brief Substitution that maps data variables to data expressions. The substitution is stored as an
/// assignment_list.
struct assignment_sequence_substitution: public std::unary_function<variable, data_expression>
{
  typedef variable variable_type;
  typedef data_expression expression_type;

  const assignment_list& assignments;

  assignment_sequence_substitution(const assignment_list& assignments_)
    : assignments(assignments_)
  {}

  const data_expression& operator()(const variable& v) const
  {
    for (assignment_list::const_iterator i = assignments.begin(); i != assignments.end(); ++i)
    {
      if (i->lhs() == v)
      {
        return i->rhs();
      }
    }
    return v;
  }
};

template <>
inline
bool is_simple_substitution(const assignment_sequence_substitution& sigma)
{
  for (auto i = sigma.assignments.begin(); i != sigma.assignments.end(); ++i)
  {
    if (!is_simple_substitution(i->lhs(), i->rhs()))
    {
      return false;
    }
  }
  return true;
}

/// \brief Generic substitution function. The substitution is stored as a sequence
/// of variables and a sequence of expressions.
template <typename VariableContainer, typename ExpressionContainer>
struct sequence_sequence_substitution: public std::unary_function<typename VariableContainer::value_type, typename ExpressionContainer::value_type>
{
  /// \brief type used to represent variables
  typedef typename VariableContainer::value_type variable_type;

  /// \brief type used to represent expressions
  typedef typename ExpressionContainer::value_type  expression_type;

  const VariableContainer& variables;
  const ExpressionContainer& expressions;

  sequence_sequence_substitution(const VariableContainer& variables_, const ExpressionContainer& expressions_)
    : variables(variables_),
      expressions(expressions_)
  {
    assert(variables.size() == expressions.size());
  }

  expression_type operator()(const variable_type& v) const
  {
    typename VariableContainer::const_iterator i = variables.begin();
    typename ExpressionContainer::const_iterator j = expressions.begin();

    for (; i != variables.end(); ++i, ++j)
    {
      if (*i == v)
      {
        return *j;
      }
    }
    return expression_type(v);
  }

  template <typename Expression>
  expression_type operator()(const Expression&) const
  {
    throw std::runtime_error("data::sequence_sequence_substitution::operator(const Expression&) is a deprecated interface!");
    return data_expression();
  }

  std::string to_string() const
  {
    std::ostringstream out;
    out << "[";
    typename VariableContainer::const_iterator i = variables.begin();
    typename ExpressionContainer::const_iterator j = expressions.begin();
    for (; i != variables.end(); ++i, ++j)
    {
      out << (i == variables.begin() ? "" : "; ") << data::pp(*i) << " := " << data::pp(*j);
    }
    out << "]";
    return out.str();
  }
};

/// \brief Utility function for creating a sequence_sequence_substitution.
template <typename VariableContainer, typename ExpressionContainer>
sequence_sequence_substitution<VariableContainer, ExpressionContainer>
make_sequence_sequence_substitution(const VariableContainer& vc, const ExpressionContainer& ec)
{
  return sequence_sequence_substitution<VariableContainer, ExpressionContainer>(vc, ec);
}

template <typename VariableContainer, typename ExpressionContainer>
bool is_simple_substitution(const sequence_sequence_substitution<VariableContainer, ExpressionContainer>& sigma)
{
  auto i = sigma.variables.begin();
  auto j = sigma.expressions.begin();
  for (i = sigma.variables.begin(); i != sigma.variables.end(); ++i, ++j)
  {
    if (!is_simple_substitution(*i, *j))
    {
      return false;
    }
  }
  return true;
}

/// \brief Generic substitution function. The substitution is stored as a sequence
/// of pairs of variables and expressions.
template <typename Container>
struct pair_sequence_substitution: public std::unary_function<typename Container::value_type::first_type, typename Container::value_type::second_type>
{
  /// \brief type used to represent variables
  typedef typename Container::value_type::first_type variable_type;

  /// \brief type used to represent expressions
  typedef typename Container::value_type::second_type expression_type;

  const Container& container;

  pair_sequence_substitution(const Container& container_)
    : container(container_)
  {}

  const expression_type &operator()(const variable_type& v) const
  {
    for (typename Container::const_iterator i = container.begin(); i != container.end(); ++i)
    {
      if (i->first == v)
      {
        return i->second;
      }
    }
    return v;
  }

  template <typename Expression>
  expression_type operator()(const Expression&) const
  {
    throw std::runtime_error("data::pair_sequence_substitution::operator(const Expression&) is a deprecated interface!");
    return data_expression();
  }
};

/// \brief Utility function for creating a pair_sequence_substitution.
template <typename Container>
pair_sequence_substitution<Container>
make_pair_sequence_substitution(const Container& c)
{
  return pair_sequence_substitution<Container>(c);
}

template <typename Container>
bool is_simple_substitution(const pair_sequence_substitution<Container>& sigma)
{
  for (auto i = sigma.container.begin(); i != sigma.container.end(); ++i)
  {
    if (!is_simple_substitution(i->first, i->second))
    {
      return false;
    }
  }
  return true;
}

/// \brief Generic substitution function. The substitution is stored as a mapping
/// of variables to expressions.
template <typename AssociativeContainer>
struct map_substitution : public std::unary_function<typename AssociativeContainer::key_type, typename AssociativeContainer::mapped_type>
{
  typedef typename AssociativeContainer::key_type variable_type;
  typedef typename AssociativeContainer::mapped_type expression_type;

  const AssociativeContainer& m_map;

  map_substitution(const AssociativeContainer& m)
    : m_map(m)
  { }

  const expression_type operator()(const variable_type& v) const
  {
    typename AssociativeContainer::const_iterator i = m_map.find(v);
    if (i == m_map.end())
    {
      return v;
    }
    else
    {
      return i->second;
    }
    // N.B. This does not work!
    // return i == m_map.end() ? v : i->second;
  }

  template <typename Expression>
  expression_type operator()(const Expression&) const
  {
    throw std::runtime_error("data::map_substitution::operator(const Expression&) is a deprecated interface!");
    return data_expression();
  }

  std::string to_string() const
  {
    std::ostringstream out;
    out << "[";
    for (typename AssociativeContainer::const_iterator i = m_map.begin(); i != m_map.end(); ++i)
    {
      out << (i == m_map.begin() ? "" : "; ") << data::pp(i->first) << ":" << data::pp(i->first.sort()) << " := " << data::pp(i->second);
    }
    out << "]";
    return out.str();
  }
};

/// \brief Utility function for creating a map_substitution.
template <typename AssociativeContainer>
map_substitution<AssociativeContainer>
make_map_substitution(const AssociativeContainer& m)
{
  return map_substitution<AssociativeContainer>(m);
}

template <typename AssociativeContainer>
bool is_simple_substitution(const map_substitution<AssociativeContainer>& sigma)
{
  for (auto i = sigma.m_map.begin(); i != sigma.m_map.end(); ++i)
  {
    if (!is_simple_substitution(i->first, i->second))
    {
      return false;
    }
  }
  return true;
}

/// \brief Generic substitution function. The substitution is stored as a mapping
/// of variables to expressions. The substitution is mutable, meaning that substitutions
/// to variables can be added and removed as follows:
///\verbatim
/// mutable_map_substitution sigma;
/// mutable_map_substitution::variable_type v;
/// mutable_map_substitution::expression_type e;
/// sigma[v] = e; // add the assignment of e to v
/// sigma[v] = v; // remove the assignment to v
///\endverbatim
template <typename AssociativeContainer = std::map<variable,data_expression> >
class mutable_map_substitution : public std::unary_function<typename AssociativeContainer::key_type, typename AssociativeContainer::mapped_type>
{
protected:
  AssociativeContainer m_map;

public:
  typedef typename AssociativeContainer::key_type variable_type;
  typedef typename AssociativeContainer::mapped_type expression_type;
  typedef typename AssociativeContainer::const_iterator const_iterator;
  typedef typename AssociativeContainer::iterator iterator;

  /// \brief Friend functions that collect some details of a substitution,
  /// needed for rewriting in internal format, as well as alpha-conversion.
  template<typename AssociativeContainer1, typename UnaryOperator>
  friend mutable_map_substitution< typename std::map<typename AssociativeContainer1::key_type, atermpp::aterm_appl> >
  apply(const mutable_map_substitution< AssociativeContainer1 >& sigma, UnaryOperator f);

  template<typename AssociativeContainer1>
  friend std::set<core::identifier_string> get_identifiers(const mutable_map_substitution< AssociativeContainer1 >& sigma);


  /// \brief Wrapper class for internal storage and substitution updates using operator()
  struct assignment
  {
    typename AssociativeContainer::key_type m_variable;
    AssociativeContainer&                   m_map;

    /// \brief Constructor.
    ///
    /// \param[in] v a variable.
    /// \param[in] m a mapping of variables to expressions.
    assignment(typename AssociativeContainer::key_type v, AssociativeContainer& m) :
      m_variable(v), m_map(m)
    { }

    template <typename AssignableToExpression>
    void operator=(AssignableToExpression const& e)
    {
      mCRL2log(log::debug2, "substitutions") << "Setting " << data::pp(m_variable) << " := " << e << std::endl;
      if (e != m_variable)
      {
        m_map[m_variable] = e;
      }
      else
      {
        m_map.erase(m_variable);
      }
    }
  };

  mutable_map_substitution()
  {}

  mutable_map_substitution(const AssociativeContainer& m)
    : m_map(m)
  {}

  template <typename VariableContainer, typename ExpressionContainer>
  mutable_map_substitution(VariableContainer const& vc, ExpressionContainer const& ec)
  {
    BOOST_ASSERT(vc.size() == ec.size());

    typename ExpressionContainer::const_iterator j = ec.begin();
    for (typename VariableContainer::const_iterator i = vc.begin(); i != vc.end(); ++i, ++j)
    {
      m_map[*i] = *j;
    }
  }

  expression_type operator()(const variable_type& v) const
  {
    typename AssociativeContainer::const_iterator i = m_map.find(v);
    if (i == m_map.end())
    {
      return v;
    }
    else
    {
      return i->second;
    }
  }

  assignment operator[](variable_type const& v)
  {
    return assignment(v, this->m_map);
  }

  /// \brief Resets the substitution by letting every variable yield itself. Cf. clear() of a map.
  ///
  void clear()
  {
    m_map.clear();
  }

  template <typename Substitution>
  bool operator==(const Substitution&) const
  {
    return false;
  }

  mutable_map_substitution & operator=(const mutable_map_substitution& other)
  {
    m_map=other.m_map;
    return *this;
  }

  /// \brief Returns an iterator pointing to the beginning of the sequence of assignments
  /// TODO: should become protected
  const_iterator begin() const
  {
    return m_map.begin();
  }

  /// \brief Returns an iterator pointing past the end of the sequence of assignments
  /// TODO: should become protected
  const_iterator end() const
  {
    return m_map.end();
  }

  /// \brief Returns an iterator pointing to the beginning of the sequence of assignments
  /// TODO: should become protected
  iterator begin()
  {
    return this->m_map.begin();
  }

  /// \brief Returns an iterator pointing past the end of the sequence of assignments
  /// TODO: should become protected
  iterator end()
  {
    return this->m_map.end();
  }

  /// \brief Returns true if the substitution is empty
  bool empty()
  {
    return m_map.empty();
  }

  std::string to_string() const
  {
    std::stringstream result;
    result << "[";
    for (const_iterator i = begin(); i != end(); ++i)
    {
      result << (i == begin() ? "" : "; ") << data::pp(i->first) << ":" << data::pp(i->first.sort()) << " := " << data::pp(i->second);
    }
    result << "]";
    return result.str();
  }

  /// \brief Returns an iterator that references the expression associated with v or is equal to m_map.end()
  iterator find(variable_type const& v)
  {
    return this->m_map.find(v);
  }

  /// \brief Returns an iterator that references the expression associated with v or is equal to m_map.end()
  const_iterator find(variable_type const& v) const
  {
    return m_map.find(v);
  }
};

/// \brief Utility function for creating a mutable_map_substitution.
template <typename VariableContainer, typename ExpressionContainer, typename MapContainer>
mutable_map_substitution<MapContainer>
make_mutable_map_substitution(const VariableContainer& vc, const ExpressionContainer& ec)
{
  return mutable_map_substitution<MapContainer>(vc, ec);
}

template <typename VariableContainer, typename ExpressionContainer>
mutable_map_substitution<std::map<typename VariableContainer::value_type, typename ExpressionContainer::value_type> >
make_mutable_map_substitution(const VariableContainer& vc, const ExpressionContainer& ec)
{
  return mutable_map_substitution<std::map<typename VariableContainer::value_type, typename ExpressionContainer::value_type> >(vc, ec);
}

inline
std::set<data::variable> substitution_variables(const mutable_map_substitution<>& sigma)
{
  std::set<data::variable> result;
  for (auto i = sigma.begin(); i != sigma.end(); ++i)
  {
    data::find_free_variables(i->second, std::inserter(result, result.end()));
  }
  return result;
}

template <typename AssociativeContainer>
bool is_simple_substitution(const mutable_map_substitution<AssociativeContainer>& sigma)
{
  for (auto i = sigma.begin(); i != sigma.end(); ++i)
  {
    if (!is_simple_substitution(i->first, i->second))
    {
      return false;
    }
  }
  return true;
}

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
  template<typename VariableType1, typename ExpressionSequence1>
  friend std::set < variable > get_free_variables(const mutable_indexed_substitution<VariableType1, ExpressionSequence1 >& sigma);

  /// \brief Friend function that applies a function to all right hand sides of the substitution.
  template<typename VariableType1, typename ExpressionSequence1, typename UnaryOperator>
  friend mutable_indexed_substitution<VariableType1, std::vector<atermpp::aterm_appl> >
  apply(const mutable_indexed_substitution<VariableType1, ExpressionSequence1 >& sigma, UnaryOperator f);

  /// \brief Type of variables
  typedef VariableType variable_type;

  /// \brief Type of expressions
  typedef typename ExpressionSequence::value_type expression_type;

  /// \brief Default constructor
  mutable_indexed_substitution()
    : m_variables_in_rhs_set_is_defined(false)
  {}

  /// \brief Wrapper class for internal storage and substitution updates using operator()
  struct assignment
  {
    const variable_type  &m_variable;
    ExpressionSequence   &m_container;
    std::vector <size_t> &m_index_table;
    std::stack<size_t> &m_free_positions;
    bool m_variables_in_rhs_set_is_defined;
    std::set<variable> &m_variables_in_rhs;


    /// \brief Constructor.
    ///
    /// \param[in] v a variable.
    /// \param[in] c a container of expressions.
    /// \param[in] table a table of indices
    /// \param[in] fp a stack of free positions in \a table
    assignment(const variable_type &v, ExpressionSequence& c, std::vector <size_t> &table, std::stack<size_t> & fp,
               const bool b, std::set<variable> &vars) :
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
      mCRL2log(log::debug2, "substitutions") << "Setting " << data::pp(m_variable) << " := " << e << std::endl;
      assert(e.defined());

#ifndef MCRL2_USE_INDEX_TRAITS
      size_t i = m_variable.name().function().number();
#else
      size_t i = data::index_traits<data::variable>::index(m_variable);
#endif
      if (e != m_variable)
      {
        // Set a new variable;
        if (m_variables_in_rhs_set_is_defined)
        {
           detail::get_free_variables(e,m_variables_in_rhs);
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
  const expression_type &operator()(const variable_type& v) const
  {
#ifndef MCRL2_USE_INDEX_TRAITS
    const size_t i = v.name().function().number();
#else
    const size_t i = data::index_traits<data::variable>::index(v);
#endif
    if (i < m_index_table.size())
    {
      const size_t j = m_index_table[i];
      if (j!=size_t(-1))
      {
        // the variable has an assigned value.
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

  /// \brief Assignment operator
  mutable_indexed_substitution & operator=(const mutable_indexed_substitution& other)
  {
    m_container=other.m_container;
    m_index_table=other.m_index_table;
    m_free_positions=other.m_free_positions;
    m_variables_in_rhs_set_is_defined=other.m_variables_in_rhs_set_is_defined;
    m_variables_in_rhs=other.m_variables_in_rhs;
    return *this;
  }

  const std::set<variable>& variables_in_rhs()
  {
    if (!m_variables_in_rhs_set_is_defined)
    {
      for(std::vector<size_t> ::const_iterator i=m_index_table.begin(); i != m_index_table.end(); ++i)
      {
        if (*i != size_t(-1))
        {
          detail::get_free_variables(m_container[*i],m_variables_in_rhs);
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
  const expression_type &get(const size_t i) const
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
        result << core::identifier_string(static_cast<atermpp::function_symbol>(m_index_table[i]).name()) << " := " << data::pp(get(i));
      }
    }
    result << "]";
    return result.str();
  }

};


/// \brief Function to get all free variables in the substitution
///        The rhs' have the shape of an expression in internal rewriter format.
/// \deprecated
/* template<typename VariableType, typename ExpressionSequence>
std::set < variable > get_free_variables(const mutable_indexed_substitution<VariableType, ExpressionSequence >& sigma)
{
  std::set < variable > result;
  typedef typename ExpressionSequence::value_type expression_type;

  for(std::vector<size_t> ::const_iterator i=sigma.m_index_table.begin(); i != sigma.m_index_table.end(); ++i)
  {
    if (*i != size_t(-1))
    {
      detail::get_free_variables(sigma.m_container[*i],result);
    }
  }
  return result;
}
*/

/// \brief Function to get all identifiers in the substitution
/// \deprecated
/* template<typename VariableType, typename ExpressionSequence>
std::set<core::identifier_string> get_identifiers(const mutable_indexed_substitution<VariableType, ExpressionSequence >& sigma)
{
  typedef typename ExpressionSequence::value_type expression_type;

  std::set<core::identifier_string> result;
  for(size_t i = 0; i < sigma.size(); ++i)
  {
    if(sigma.get(i) != expression_type())
    {
      result.insert(core::identifier_string(static_cast<atermpp::function_symbol>(i).name()));
      find_all_if(sigma.get(i),core::is_identifier_string,std::inserter(result,result.begin()));
    }
  }
  return result;
} */

/// \deprecated
/// Provided for use with the rewriters in internal format
/* template<typename AssociativeContainer>
std::set<core::identifier_string> get_identifiers(const mutable_map_substitution< AssociativeContainer >& sigma)
{
  std::set<core::identifier_string> result;
  for(typename mutable_map_substitution< AssociativeContainer >::const_iterator i = sigma.begin(); i != sigma.end(); ++i)
  {
    result.insert(i->first.name());
    find_all_if(i->second,core::is_identifier_string,std::inserter(result,result.begin()));
  }
  return result;
} */

/// \deprecated
/// Provided for use with the rewriters in internal format
/// \brief Friend function that applies a function to all right hand sides of the substitution.
template<typename VariableType, typename ExpressionSequence, typename UnaryOperator>
mutable_indexed_substitution<VariableType, std::vector<atermpp::aterm_appl> >
apply(const mutable_indexed_substitution<VariableType, ExpressionSequence >& sigma, UnaryOperator f)
{
  mutable_indexed_substitution<VariableType, std::vector<atermpp::aterm_appl> > result;
  result.m_index_table=sigma.m_index_table;
  result.m_container.resize(sigma.m_container.size(),atermpp::aterm_appl());
  result.m_free_positions=sigma.m_free_positions;

  for(std::vector<size_t>::const_iterator i=sigma.m_index_table.begin(); i != sigma.m_index_table.end(); ++i)
  {
    if (*i != size_t(-1))
    {
      assert(*i<result.m_container.size());
      result.m_container[*i] = f(sigma.m_container[*i]);
    }
  }
  return result;
}

/// \deprecated
/// Provided for use with the rewriters in internal format
template<typename AssociativeContainer, typename UnaryOperator>
mutable_map_substitution< std::map<typename AssociativeContainer::key_type, atermpp::aterm_appl > >
apply(const mutable_map_substitution< AssociativeContainer >& sigma, UnaryOperator f)
{
  mutable_map_substitution< std::map<typename AssociativeContainer::key_type, atermpp::aterm_appl > > result;
  for(typename mutable_map_substitution< AssociativeContainer >::const_iterator i = sigma.begin(); i != sigma.end(); ++i)
  {
    result[i->first] = f(i->second);
  }
  return result;
}

/// \brief An adapter that makes an arbitrary substitution function mutable.
template <typename Substitution>
class mutable_substitution_composer: public std::unary_function<typename Substitution::variable_type, typename Substitution::expression_type>
{
  public:
    /// \brief type used to represent variables
    typedef typename Substitution::variable_type variable_type;

    /// \brief type used to represent expressions
    typedef typename Substitution::expression_type expression_type;

    /// \brief Wrapper class for internal storage and substitution updates using operator()
    typedef typename mutable_map_substitution<std::map<variable_type, expression_type> >::assignment assignment;

    /// \brief The type of the wrapped substitution
    typedef Substitution substitution_type;

  protected:
    /// \brief The wrapped substitution
    const Substitution& f_;

    /// \brief An additional mutable substitution
    mutable_map_substitution<std::map<variable_type, expression_type> > g_;

  public:
    /// \brief Constructor
    mutable_substitution_composer(const Substitution& f)
      : f_(f)
    {}

    /// \brief Apply on single single variable expression
    /// \param[in] v the variable for which to give the associated expression
    /// \return expression equivalent to <|s|>(<|e|>), or a reference to such an expression
    expression_type operator()(variable_type const& v) const
    {
      return data::replace_free_variables(f_(v), g_);
    }

    template <typename Expression>
    expression_type operator()(const Expression&) const
    {
      throw std::runtime_error("data::mutable_substitution_composer::operator(const Expression&) is a deprecated interface!");
      return data_expression();
    }

    assignment operator[](variable_type const& v)
    {
      return g_[v];
    }

    /// \brief Returns the wrapped substitution
    /// \return The wrapped substitution
    const substitution_type& substitution() const
    {
      return f_;
    }
};

/// \brief Specialization for mutable_map_substitution.
template <typename AssociativeContainer>
class mutable_substitution_composer<mutable_map_substitution<AssociativeContainer> >: public std::unary_function<typename AssociativeContainer::key_type, typename AssociativeContainer::mapped_type>
{
  public:
    /// \brief The type of the wrapped substitution
    typedef mutable_map_substitution<AssociativeContainer> substitution_type;

    /// \brief type used to represent variables
    typedef typename substitution_type::variable_type variable_type;

    /// \brief type used to represent expressions
    typedef typename substitution_type::expression_type expression_type;

    /// \brief Wrapper class for internal storage and substitution updates using operator()
    typedef typename substitution_type::assignment assignment;

  protected:
    /// \brief object on which substitution manipulations are performed
    mutable_map_substitution<AssociativeContainer>& g_;

  public:

    /// \brief Constructor with mutable substitution object
    /// \param[in,out] g underlying substitution object
    mutable_substitution_composer(mutable_map_substitution<AssociativeContainer>& g)
      : g_(g)
    {}

    /// \brief Apply on single single variable expression
    /// \param[in] v the variable for which to give the associated expression
    /// \return expression equivalent to <|s|>(<|e|>), or a reference to such an expression
    const expression_type operator()(variable_type const& v) const
    {
      return g_(v);
    }

    template <typename Expression>
    expression_type operator()(const Expression&) const
    {
      throw std::runtime_error("data::mutable_substitution_composer<mutable_map_substitution<AssociativeContainer> >::operator(const Expression&) is a deprecated interface!");
      return data_expression();
    }

    assignment operator[](variable_type const& v)
    {
      return g_[v];
    }

    /// \brief Returns the wrapped substitution
    /// \return The wrapped substitution
    const substitution_type& substitution() const
    {
      return g_;
    }
};

/// \brief Returns a string representation of the map, for example [a := 3, b := true].
/// \param sigma a substitution.
/// \return A string representation of the map.
template <typename Substitution>
std::string print_substitution(const Substitution& sigma)
{
  return sigma.to_string();
}

/// \brief Returns a string representation of the map, for example [a := 3, b := true].
/// \param[in] sigma a constant reference to an object of a mutable_substitution_composer instance
/// \return A string representation of the map.
template <typename Substitution>
std::string print_substitution(const mutable_substitution_composer<Substitution>& sigma)
{
  return print_substitution(sigma.substitution());
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SUBSTITUTIONS_H
