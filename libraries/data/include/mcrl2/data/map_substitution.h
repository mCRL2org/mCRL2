// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/map_substitution.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_MAP_SUBSTITUTION_H__
#define MCRL2_DATA_MAP_SUBSTITUTION_H__

#include "mcrl2/data/substitution.h"

namespace mcrl2 {

namespace data {

  template < typename UniqueSortedPairAssociativeContainer,
               template < typename Substitution > class SubstitutionProcedure = structural_substitution >
  class map_substitution;

  template < typename UniqueSortedPairAssociativeContainer = atermpp::map< variable, data_expression >,
               template < typename Substitution > class SubstitutionProcedure = structural_substitution >
  struct mutable_map_substitution;

  /**
   * \brief Substitution that uses a Unique Sorted Pair Associative Container for storing assignments
   *
   * Instantiate types are models of the Map Substitution concept. Provided that
   * SubstitutionProcedure is a subtitution procedure and
   * UniqueSortedPairAssociativeContainer is a model of Unique Sorted Pair
   * Associative Container (STL concept). In the case that the
   * UniqueSortedPairAssociativeContainer type parameter has a const
   * qualifier, the instantiated type is also a model of the Mutable Subsitution concept.
   */
  template < typename UniqueSortedPairAssociativeContainer, template < typename Substitution > class SubstitutionProcedure >
  class map_substitution :
      public substitution< map_substitution< UniqueSortedPairAssociativeContainer, SubstitutionProcedure >,
        typename boost::remove_reference< UniqueSortedPairAssociativeContainer >::type::key_type,
        typename boost::remove_reference< UniqueSortedPairAssociativeContainer >::type::value_type::second_type, SubstitutionProcedure >
  {
      typedef substitution< map_substitution< UniqueSortedPairAssociativeContainer, SubstitutionProcedure >,
        typename boost::remove_reference< UniqueSortedPairAssociativeContainer >::type::key_type,
        typename boost::remove_reference< UniqueSortedPairAssociativeContainer >::type::value_type::second_type,
      										 SubstitutionProcedure > super;

      typedef typename boost::remove_cv<
               typename boost::remove_reference< UniqueSortedPairAssociativeContainer >::type >::type container_type;

    public:

      /// \brief type used to represent variables
      typedef typename super::variable_type                    variable_type;

      /// \brief type used to represent expressions
      typedef typename super::expression_type                  expression_type;

      /// \brief Iterator type for constant element access
      typedef typename container_type::const_iterator          const_iterator;

      /// \brief Iterator type for non-constant element access
      typedef typename container_type::iterator                iterator;

    protected:

      friend class substitution< map_substitution, variable_type, expression_type, SubstitutionProcedure >;

      /// \brief a mapping from variables to expressions
      UniqueSortedPairAssociativeContainer m_map;

    protected:

      map_substitution() {
      }

      /// \brief Apply on single single variable expression
      /// \param[in] v the variable for which to give the associated expression
      /// \return expression equivalent to <|s|>(<|e|>), or a reference to such an expression
      expression_type apply(variable_type const& v) const {
        const_iterator i = m_map.find(v);

        if (i == m_map.end()) {
          expression_type e = v;

          return e;
        }

        return i->second;
      }

    public:

      /// \brief Wrapper class for internal storage and substitution updates using operator()
      class assignment {

        private:

          typename container_type::key_type m_variable;
          container_type&                   m_map;

        public:

          /// \brief Constructor.
          ///
          /// \param[in] v a variable.
          /// \param[in] m a mapping of variables to expressions.
          assignment(typename container_type::key_type v, container_type& m) :
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
          template < typename AssignableToExpression >
          void operator=(AssignableToExpression const& e)
          {
            if (e != m_variable) {
              m_map[m_variable] = e;
            }
            else {
              m_map.erase(m_variable);
            }
          }
      };

      template <typename VariableContainer, typename ExpressionContainer >
      map_substitution(VariableContainer const& vc, ExpressionContainer const& ec) {
        BOOST_ASSERT(vc.size() == ec.size());

        typename ExpressionContainer::const_iterator j = ec.begin();
        for (typename VariableContainer::const_iterator i = vc.begin(); i != vc.end(); ++i, ++j)
        {
          m_map[*i] = *j;
        }
      }

      map_substitution(container_type& other) : m_map(other) {
      }

      map_substitution(const container_type& other) : m_map(other) {
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
      assignment operator[](variable_type const& v) {
        return assignment(v, this->m_map);
      }

      /// \brief Returns an iterator pointing to the beginning of the sequence of assignments
      const_iterator begin() const {
        return m_map.begin();
      }

      /// \brief Returns an iterator pointing past the end of the sequence of assignments
      const_iterator end() const {
        return m_map.end();
      }

      /// \brief Returns an iterator pointing to the beginning of the sequence of assignments
      iterator begin() {
        return this->m_map.begin();
      }

      /// \brief Returns an iterator pointing past the end of the sequence of assignments
      iterator end() {
        return this->m_map.end();
      }

      /// \brief Returns an iterator that references the expression associated with v or is equal to m_map.end()
      iterator find(variable_type const& v) {
        return this->m_map.find(v);
      }

      /// \brief Returns an iterator that references the expression associated with v or is equal to m_map.end()
      const_iterator find(variable_type const& v) const {
        return m_map.find(v);
      }

      /// \brief Returns true if the sequence of assignments is empty
      bool empty() const {
        return this->m_map.empty();
      }
  };

  /** \brief Generic substitution class (model of Mutable Substitution)
   *
   * Used to generate models of the Mutable Map Substitution concept.
   *
   * \see map_substitution
   **/
  template < typename UniqueSortedPairAssociativeContainer, template < typename Substitution > class SubstitutionProcedure >
  struct mutable_map_substitution : public map_substitution< UniqueSortedPairAssociativeContainer, SubstitutionProcedure > {
  };

  /// \brief Utility function for creating a map_substitution_adapter.
  template <typename VariableContainer, typename ExpressionContainer, typename MapContainer >
  map_substitution< MapContainer >
  make_map_substitution(const VariableContainer& vc, const ExpressionContainer& ec)
  {
    return map_substitution< MapContainer >(vc, ec);
  }

  template <typename VariableContainer, typename ExpressionContainer >
  map_substitution< std::map< typename VariableContainer::value_type, typename ExpressionContainer::value_type > >
  make_map_substitution(const VariableContainer& vc, const ExpressionContainer& ec)
  {
    return map_substitution< std::map< typename VariableContainer::value_type, typename ExpressionContainer::value_type > >(vc, ec);
  }

  /// \brief Utility function for creating a map_substitution_adapter.
  template <typename MapContainer>
  map_substitution<MapContainer const&> make_map_substitution_adapter(const MapContainer& m)
  {
    return map_substitution<MapContainer const&>(m);
  }

  /// \brief Utility function for creating a map_substitution_adapter.
  template <typename MapContainer>
  map_substitution<MapContainer&> make_mutable_map_substitution_adapter(MapContainer& m)
  {
    return map_substitution<MapContainer&>(m);
  }

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_MAP_SUBSTITUTION_ADAPTER_H
