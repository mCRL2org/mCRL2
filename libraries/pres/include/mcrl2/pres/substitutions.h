// Author(s): Jan Friso Groote. Based on pbes/substitutions.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/substitutions.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_SUBSTITUTIONS_H
#define MCRL2_PRES_SUBSTITUTIONS_H

#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/pres/replace.h"



namespace mcrl2::pres_system {

/** \brief Substitution function for propositional variables
 *
 * Model of Substitution.
 *
 **/
class propositional_variable_substitution
{
  public:
    // maps X to (phi, d), where X(d) is the propositional variable corresponding to X
    using map_type = std::map<core::identifier_string, std::pair<pres_expression, data::variable_list>>;

    using iterator = map_type::iterator;
    using const_iterator = map_type::const_iterator;

  protected:
    map_type m_map;

  public:
    /// \brief type used to represent variables
    using variable_type = propositional_variable_instantiation;

    /// \brief type used to represent expressions
    using expression_type = pres_expression;

    /// \brief Apply this substitution to a single variable expression.
    /// \param[in] v The variable for which to give the associated expression.
    /// \return expression equivalent to s(e), or a reference to such an expression.
    pres_expression operator()(const variable_type& v) const
    {
      auto i = m_map.find(v.name());
      if (i == m_map.end())
      {
        return v;
      }
      pres_expression phi = i->second.first;
      const data::variable_list& d = i->second.second;
      const data::data_expression_list& e = v.parameters();
      data::mutable_map_substitution<> sigma;
      auto j = d.begin();
      auto k = e.begin();
      for (; j != d.end(); ++j, ++k)
      {
        sigma[*j] = *k;
      }

      // return phi[d := e]
      phi = pres_system::replace_variables_capture_avoiding(phi, sigma);
      return phi;
    }

    /// \brief Wrapper class for internal storage and substitution updates using operator()
    class assignment
    {
      private:
        const propositional_variable& m_variable;
        map_type&                     m_map;

      public:

        /// \brief Constructor.
        ///
        /// \param[in] v a variable.
        /// \param[in] m a mapping of variables to expressions.
        assignment(const propositional_variable& v, map_type& m) :
          m_variable(v), m_map(m)
        { }

        /** \brief Assigns expression on the right-hand side
         * \param[in] e the expression to associate to the variable for the owning substitution object
         * \code
         *  template< typename E, typename V >
         *  void example(V const& v, E const& e) {
         *    substitution< E, V > s;         // substitution
         *
         *    s[v] = e;
         *
         *    assert(s(v) == e);
         * \endcode
         **/
        void operator=(const pres_expression& e)
        {
          m_map[m_variable.name()] = std::make_pair(e, m_variable.parameters());
        }
    };

    propositional_variable_substitution() = default;

    propositional_variable_substitution(const map_type& other)
      : m_map(other)
    {
    }

    /** \brief Update substitution for a single variable
     *
     * \param[in] v the variable for which to update the value
     *
     * \code
     *  template< typename E, typename V >
     *  void example(V const& v, E const& e) {
     *    substitution< E, V > s;         // substitution
     *
     *    std::cout << s(x) << std::endl; // prints x
     *
     *    s[v] = e;
     *
     *    std::cout << s(x) << std::endl; // prints e
     *  }
     * \endcode
     *
     * \return expression assignment for variable v, effect
     **/
    assignment operator[](const propositional_variable& v)
    {
      return { v, this->m_map };
    }

    /// \brief Constructor. Initializes the substitution with the assignment X := phi.
    propositional_variable_substitution(const propositional_variable& X, const pres_expression& phi)
    {
      (*this)[X] = phi;
    }

    /// \brief Returns an iterator pointing to the beginning of the sequence of assignments
    const_iterator begin() const
    {
      return m_map.begin();
    }

    /// \brief Returns an iterator pointing past the end of the sequence of assignments
    const_iterator end() const
    {
      return m_map.end();
    }

    /// \brief Returns an iterator pointing to the beginning of the sequence of assignments
    iterator begin()
    {
      return this->m_map.begin();
    }

    /// \brief Returns an iterator pointing past the end of the sequence of assignments
    iterator end()
    {
      return this->m_map.end();
    }

    /// \brief Returns an iterator that references the expression associated with v or is equal to m_map.end()
    iterator find(const variable_type &v)
    {
      return this->m_map.find(static_cast<core::identifier_string>(v));
    }

    /// \brief Removes the substitution to the propositional variable v.
    map_type::size_type erase(const propositional_variable& v)
    {
      return m_map.erase(static_cast<core::identifier_string>(v));
    }

    /// \brief Returns an iterator that references the expression associated with v or is equal to m_map.end()
    const_iterator find(variable_type const& v) const
    {
      return m_map.find(static_cast<core::identifier_string>(v));
    }

    /// \brief Returns true if the sequence of assignments is empty
    bool empty() const
    {
      return this->m_map.empty();
    }
};

} // namespace mcrl2::pres_system



#endif // MCRL2_PRES_SUBSTITUTIONS_H
