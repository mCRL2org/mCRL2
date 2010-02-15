// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/substitution.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_SUBSTITUTION_H
#define MCRL2_PBES_SUBSTITUTION_H

#include "mcrl2/data/substitution.h"
#include "mcrl2/data/sequence_substitution.h"
#include "mcrl2/pbes/substitute.h"
#include "mcrl2/pbes/detail/propositional_variable_substituter.h"

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
      typedef atermpp::map<core::identifier_string, std::pair<pbes_expression, data::variable_list> > map_type;

      typedef map_type::iterator iterator;
      typedef map_type::const_iterator const_iterator;

    protected:
      map_type m_map;     
      
    public:
      /// \brief type used to represent variables
      typedef propositional_variable_instantiation variable_type;

      /// \brief type used to represent expressions
      typedef pbes_expression expression_type;

      /// \brief Apply on single single variable expression
      /// \param[in] v the variable for which to give the associated expression
      /// \return expression equivalent to <|s|>(<|e|>), or a reference to such an expression
      pbes_expression operator()(const propositional_variable_instantiation& v) const
      {
        map_type::const_iterator i = m_map.find(v.name());
        if (i == m_map.end())
        {
          return v;
        }
        pbes_expression phi = i->second.first;
        const data::variable_list& d = i->second.second;
        const data::data_expression_list& e = v.parameters();

        // return phi[d := e]
        pbes_system::substitute_gcc_workaround(phi, data::make_double_sequence_substitution_adaptor(d, e));
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
      assignment operator[](const propositional_variable& v) {
        return assignment(v, this->m_map);
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
      iterator find(variable_type const& v)
      {
        return this->m_map.find(v);
      }

      /// \brief Removes the substitution to the propositional variable v.
      map_type::size_type erase(const propositional_variable& v)
      {
        return m_map.erase(v);
      }

      /// \brief Returns an iterator that references the expression associated with v or is equal to m_map.end()
      const_iterator find(variable_type const& v) const
      {
        return m_map.find(v);
      }

      /// \brief Returns true if the sequence of assignments is empty
      bool empty() const
      {
        return this->m_map.empty();
      }
  };     

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_SUBSTITUTION_H
