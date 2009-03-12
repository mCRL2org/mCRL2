// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/substitution.h
/// \brief Template class for substitution

#ifndef _MCRL2_NEW_DATA_SUBSTITUTION__HPP_
#define _MCRL2_NEW_DATA_SUBSTITUTION__HPP_

#include <map>
#include <set>
#include <stack>
#include <vector>
#include <memory>
#include <functional>

#include "boost/assert.hpp"
#include "boost/utility/enable_if.hpp"
#include "boost/type_traits/add_cv.hpp"
#include "boost/type_traits/add_pointer.hpp"
#include "boost/type_traits/is_reference.hpp"
#include "boost/type_traits/is_same.hpp"
#include "boost/range/iterator_range.hpp"
#include "boost/iterator/indirect_iterator.hpp"

#include "mcrl2/new_data/expression_traits.h"
#include "mcrl2/new_data/replace.h"

namespace mcrl2 {
  namespace new_data {

    // declare for use as template argument defaults
    class data_expression;
    class variable;

    /// \brief Procedure to be used with substitution to apply textual substitution an expression
    ///
    /// Type parameters:
    ///  \arg Substitution a model of Substitution
    template < typename Substitution >
    struct textual_substitution {
      /// Function Arguments:
      ///  \param[in] s a substitution object that is only used for variable lookup
      ///  \param[in] e the expression on which to apply variable replacement according to the substitution
      /// \return the result of substitution
      static data_expression apply(Substitution const& s, typename Substitution::expression_type const& e) {
        return replace_variables(e, s);
      }
    };

    template < typename Variable = new_data::variable,
               typename Expression = new_data::data_expression,
               template < typename Substitution > class SubstitutionProcedure = textual_substitution >
    class mutable_map_substitution;

    /** \brief Generic substitution class (model of Substitution)
     *
     * Objects of this type represent mutable substitutions that can be applied
     * to expressions in order to generate new expressions. 
     *
     * \arg Variable type used to represent variables
     * \arg Expression type used to represent expressions
     * \arg SubstitutionProcedure procedure parametrised with a model of Substitution that is used for applying a substitution
     *
     * Model of ModifiableSubstitution.
     *
     * \note the default substitution procedure is term substitution that does
     * not take into account binders. In special cases one would typically a
     * need full-blown capture avoiding substitution procedure.
     **/
    template < typename Variable, typename Expression, template < typename Substitution > class SubstitutionProcedure >
    class mutable_map_substitution {

      public:

        /// \brief type used to represent variables
        typedef Variable                                                            variable_type;

        /// \brief type used to represent expressions
        typedef Expression                                                          expression_type;

        /// \brief Iterator type for constant element access
        typedef typename std::map< variable_type, expression_type >::const_iterator const_iterator;

        /// \brief Iterator type for non-constant element access
        typedef typename std::map< variable_type, expression_type >::iterator       iterator;

        /// \brief Wrapper class for internal storage and substitution updates using operator()
        class assignment {

          private:

            Variable                                    m_variable;

            std::map< variable_type, expression_type >& m_map;

          public:

            assignment(variable_type const& v, std::map< variable_type, expression_type >& m) : m_variable(v), m_map(m) {
            }

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
            void operator=(expression_type const& e) {
              m_map[m_variable] = e;
            }
        };

      private:

        std::map< variable_type, expression_type > m_map;

        /// \brief traits class instance for expression_type
        typedef typename mcrl2::core::term_traits< expression_type >               expression_traits;

        expression_type const& lookup(variable_type const& v) const {
          typename std::map< variable_type, expression_type >::const_iterator i = m_map.find(v);

          if (i == m_map.end()) {
            return v;
          }

          return i->second;
        }

      public:

        /// \brief Apply on single single variable expression
        /// \note This overload is only available if Expression is not equal to Variable
        /// \param[in] v the variable expression to which to apply substitution
        /// \return expression equivalent to <|s|>(<|e|>), or a reference to such an expression
        typename boost::disable_if<
          typename boost::is_same< typename boost::add_cv< Variable >::type,
            typename boost::add_cv< Expression > >::type, Expression >::type
                 operator()(Variable const& v) const {

          return lookup(v); // clone
        }

        /// \brief Returns an iterator pointing to the beginning of the sequence of assignments
        const_iterator begin() const {
          return m_map.begin();
        }

        /// \brief Returns an iterator pointing to the beginning of the sequence of assignments
        iterator begin() {
          return m_map.begin();
        }

        /// \brief Returns an iterator pointing past the end of the sequence of assignments
        const_iterator end() const {
          return m_map.end();
        }

        /// \brief Returns an iterator pointing past the end of the sequence of assignments
        iterator end() {
          return m_map.end();
        }

        /** \brief Apply substitution to an expression
         *
         * Substitution respects bound variables e.g. (lambda x.x)[x := 1]
         * yields (lambda x.x), but is not capture-avoiding e.g. (lambda x.x +
         * y)[y := x] yields (lambda x.x + x).
         *
         * \code
         *  template< typename E, typename V >
         *  void example() {
         *    V                    x("x");    // variable
         *    substitution< E, V > s;         // substitution
         *
         *    std::cout << s(x) << std::endl; // prints x, assuming that << is defined for E
         *  }
         * \endcode
         *
         * \param[in] e the expression to which to apply substitution
         * \return expression equivalent to <|s|>(<|e|>), or a reference to such an expression
         **/
        Expression operator()(Expression const& e) const {
          return SubstitutionProcedure< mutable_map_substitution >::apply(*this, e);
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
        assignment operator[](Variable const& v) {
          return assignment(v, m_map);
        }

        /** \brief Comparison operation between substitutions
         *
         * \param[in] other object of another substitution object
         * \return whether the substitution expressed by this object is equivalent to <|other|>
         **/
        bool operator==(mutable_map_substitution const& other) const {
          typename std::map< variable_type, expression_type >::const_iterator i = m_map.begin();
          typename std::map< variable_type, expression_type >::const_iterator j = other.m_map.begin();

          while (i != m_map.end() && j != other.m_map.end()) {
            expression_type const& ii(i->second);
            expression_type const& jj(j->second);

            if (ii == i->first) {
              ++i;
            }
            else if (jj == j->first) {
              ++j;
            }
            else if (ii == jj) {
              ++i;
              ++j;
            }
            else {
              return false;
            }
          }
          while (i != m_map.end()) {
            expression_type const& ii(i->second);

            if (ii != i->first) {
              return false;
            }

            ++i;
          }
          while (j != other.m_map.end()) {
            expression_type const& jj(j->second);

            if (jj == j->first) {
              return false;
            }

            ++j;
          }

          return true;
        }
    };

  }
}

#endif
