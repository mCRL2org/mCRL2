// Author(s): Jeroen van der Wulp, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/substitutions/mutable_map_substitution.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_SUBSTITUTIONS_MUTABLE_MAP_SUBSTITUTION_H
#define MCRL2_DATA_SUBSTITUTIONS_MUTABLE_MAP_SUBSTITUTION_H

#include "mcrl2/data/is_simple_substitution.h"
#include "mcrl2/data/undefined.h"
#include "mcrl2/utilities/exception.h"
#include <functional>
#include <iostream>
#include <sstream>
#include <string>

namespace mcrl2 {

namespace data {

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
      mCRL2log(log::debug2, "substitutions") << "Setting " << m_variable << " := " << e << std::endl;
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
    assert(vc.size() == ec.size());

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

  mutable_map_substitution& operator=(const mutable_map_substitution& other)
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
      result << (i == begin() ? "" : "; ") << i->first << ":" << i->first.sort() << " := " << i->second;
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

template <typename AssociativeContainer>
std::ostream& operator<<(std::ostream& out, const mutable_map_substitution<AssociativeContainer>& sigma)
{
  return out << sigma.to_string();
}

std::set<data::variable> substitution_variables(const mutable_map_substitution<>& sigma);

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

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SUBSTITUTIONS_MUTABLE_MAP_SUBSTITUTION_H
