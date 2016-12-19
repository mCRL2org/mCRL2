// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/substitutions.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_SUBSTITUTIONS_H
#define MCRL2_PBES_SUBSTITUTIONS_H

#include <utility>
#include "mcrl2/data/replace.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/pbes/replace.h"

namespace mcrl2 {

namespace pbes_system {

/** \brief Substitution function for propositional variables
 *
 * Model of Substitution.
 *
 **/
class propositional_variable_substitution: public std::unary_function<propositional_variable_instantiation, pbes_expression>
{
  public:
    // maps X to (phi, d), where X(d) is the propositional variable corresponding to X
    typedef std::map<core::identifier_string, std::pair<pbes_expression, data::variable_list> > map_type;

    typedef map_type::iterator iterator;
    typedef map_type::const_iterator const_iterator;

  protected:
    map_type m_map;

  public:
    /// \brief type used to represent variables
    typedef propositional_variable_instantiation variable_type;

    /// \brief type used to represent expressions
    typedef pbes_expression expression_type;

    /// \brief Apply this substitution to a single variable expression.
    /// \param[in] v The variable for which to give the associated expression.
    /// \return expression equivalent to s(e), or a reference to such an expression.
    pbes_expression operator()(const variable_type& v) const
    {
      map_type::const_iterator i = m_map.find(v.name());
      if (i == m_map.end())
      {
        return v;
      }
      pbes_expression phi = i->second.first;
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
      phi = pbes_system::replace_variables_capture_avoiding(phi, sigma, data::substitution_variables(sigma));
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
        void operator=(const pbes_expression& e)
        {
          m_map[m_variable.name()] = std::make_pair(e, m_variable.parameters());
        }
    };

    propositional_variable_substitution()
    {}

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
      return assignment(v, this->m_map);
    }

    /// \brief Constructor. Initializes the substitution with the assignment X := phi.
    propositional_variable_substitution(const propositional_variable& X, const pbes_expression& phi)
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
      return this->m_map.find(atermpp::deprecated_cast<core::identifier_string>(v));
    }

    /// \brief Removes the substitution to the propositional variable v.
    map_type::size_type erase(const propositional_variable& v)
    {
      return m_map.erase(atermpp::deprecated_cast<core::identifier_string>(v));
    }

    /// \brief Returns an iterator that references the expression associated with v or is equal to m_map.end()
    const_iterator find(variable_type const& v) const
    {
      return m_map.find(atermpp::deprecated_cast<core::identifier_string>(v));
    }

    /// \brief Returns true if the sequence of assignments is empty
    bool empty() const
    {
      return this->m_map.empty();
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_SUBSTITUTIONS_H
