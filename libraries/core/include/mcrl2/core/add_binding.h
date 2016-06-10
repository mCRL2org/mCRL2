// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/add_binding.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_ADD_BINDING_H
#define MCRL2_CORE_ADD_BINDING_H

#include <set>
#include "mcrl2/atermpp/container_utility.h"

namespace mcrl2
{

namespace core
{

/// \brief Traverser that defines functions for maintaining bound variables.
template <template <class> class TraverserOrBuilder, typename Derived, typename Variable>
class add_binding : public TraverserOrBuilder<Derived>
{
  public:
    typedef TraverserOrBuilder<Derived> super;
    typedef Variable variable_type;

  protected:
    std::multiset<variable_type> m_bound_variables;

    /// \brief Add a variable to the multiset of bound variables.
    void increase_bind_count(const variable_type& var)
    {
      m_bound_variables.insert(var);
    }

    /// \brief Add a sequence of variables to the multiset of bound variables.
    template <typename Container>
    void increase_bind_count(const Container& variables, typename atermpp::enable_if_container<Container, variable_type>::type* = nullptr)
    {
      for (typename Container::const_iterator i = variables.begin(); i != variables.end(); ++i)
      {
        m_bound_variables.insert(*i);
      }
    }

    /// \brief Remove a variable from the multiset of bound variables.
    void decrease_bind_count(const variable_type& var)
    {
      m_bound_variables.erase(m_bound_variables.find(var));
    }

    /// \brief Remove a sequence of variables from the multiset of bound variables.
    template <typename Container>
    void decrease_bind_count(const Container& variables, typename atermpp::enable_if_container<Container, variable_type>::type* = nullptr)
    {
      for (typename Container::const_iterator i = variables.begin(); i != variables.end(); ++i)
      {
        m_bound_variables.erase(m_bound_variables.find(*i));
      }
    }

  public:
    /// \brief Returns true if the variable v is bound.
    bool is_bound(variable_type const& v) const
    {
      return m_bound_variables.find(v) != m_bound_variables.end();
    }

    /// \brief Returns the bound variables.
    const std::multiset<variable_type>& bound_variables() const
    {
      return m_bound_variables;
    }

    /// \brief Returns the bind count of the variable v.
    typename std::multiset<variable_type>::size_type bind_count(const variable_type& v)
    {
      return m_bound_variables.count(v);
    }
};

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_ADD_BINDING_H
