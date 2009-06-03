// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/substitution.h
/// \brief Template class for substitution

#ifndef _MCRL2_DATA_SUBSTITUTION__HPP_
#define _MCRL2_DATA_SUBSTITUTION__HPP_

#include <sstream>

#include "boost/type_traits/remove_cv.hpp"
#include "boost/type_traits/is_same.hpp"

#include "mcrl2/atermpp/map.h"
#include "mcrl2/data/replace.h"

namespace mcrl2 {
  namespace data {

    // declare for use as template argument defaults
    class data_expression;
    class variable;

    /// \cond INTERNAL_DOCS
    namespace detail {
      class inaccessible {

        inaccessible() { }
      };

      template < bool Condition, typename Result >
      struct choose_expression_type_or_inaccessible {
        typedef inaccessible type;
      };

      template < typename Result >
      struct choose_expression_type_or_inaccessible< false, Result > {
        typedef Result type;
      };

      template < typename Variable, typename Expression >
      struct expression_type_or_inaccessible {

        typedef typename choose_expression_type_or_inaccessible< boost::is_same<
            typename boost::remove_cv< Variable >::type,
            typename boost::remove_cv< Expression >::type >::value, Expression >::type type;
      };
    }
    /// \endcond

    /** \brief Procedure to be used with substitution to apply immediate textual substitution an expression
     *
     * The structure of expressions is completely ignored when doing
     * replacements. Pay careful attention to the fact that using this
     * procedure may result in terms that are not valid data expressions.
     *
     * The replacements are given by an object of type Substitution.
     *
     * Type parameters:
     *  \arg Substitution a model of Substitution
     *
     * Examples: 
     *  - [x := true, y := false] applied to x && y results in true && false
     *  - [x := y, y := false] applied to x && y results in y && false
     *  - [x := true, y := false] applied to lambda x:Bool. x && y results in lambda true:Bool. true && false
     **/
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

    /** \brief Procedure to be used with substitution to apply immediate structural substitution an expression
     *
     * Structural substitution takes variable binders into account i.e. the
     * structure of the expression . The procedure is matches capture-avoiding
     * substitution if the set of replacements only consists of closed
     * expressions, or if it does contain open expressions but the structure
     * of the operand is such that replacements do not introduce variables
     * that get bound.
     *
     * The replacements are given by an object of type Substitution. When
     * replacements cause a variable to be captured an assertion will be triggered.
     *
     * Type parameters:
     *  \arg Substitution a model of Substitution
     *
     *  take lambda x:Bool. x && y and [x := true, y != false] the result is lambda true:Bool. true && false
     *
     * Examples: 
     *  - [x := true, y := false] applied to x && y results in true && false
     *  - [x := y, y := false] applied to x && y results in y && false
     *  - [x := y, y := false] applied to lambda x:Bool. x && y results in lambda x:Bool. x && false
     *  - [y := x] applied to lambda x:Bool. x && y results in lambda x:Bool. x && x (invalid input: assertion)
     **/
    template < typename Substitution >
    struct structural_substitution {
      /// Function Arguments:
      ///  \param[in] s a substitution object that is only used for variable lookup
      ///  \param[in] e the expression on which to apply variable replacement according to the substitution
      /// \return the result of substitution
      static data_expression apply(Substitution const& s, typename Substitution::expression_type const& e) {
        return replace_free_variables(e, s);
      }
    };

    template < typename Variable = data::variable,
               typename Expression = data::data_expression,
               template < typename Substitution > class SubstitutionProcedure = structural_substitution >
    class substitution;

    template < typename Variable = data::variable,
               typename Expression = data::data_expression,
               template < typename Substitution > class SubstitutionProcedure = structural_substitution >
    class mutable_substitution;

    /** \brief Generic substitution class (model of Substitution)
     *
     * Objects of this type represent mutable substitutions that can be applied
     * to expressions in order to generate new expressions. 
     *
     * \arg Variable type used to represent variables
     * \arg Expression type used to represent expressions
     * \arg SubstitutionProcedure procedure parametrised with a model of Substitution that is used for applying a substitution
     *
     * Model of Substitution.
     *
     * \note the default substitution procedure is structural that does take
     * variable binders into account but does not avoid capture. In special
     * cases one would typically a need full-blown capture avoiding
     * substitution procedure.
     **/
    template < typename Variable, typename Expression, template < typename Substitution > class SubstitutionProcedure >
    class substitution : public std::unary_function< Expression, Expression > {

      public:

        /// \brief type used to represent variables
        typedef Variable                                                            variable_type;

        /// \brief type used to represent expressions
        typedef Expression                                                          expression_type;

        /// \brief Iterator type for constant element access
        typedef typename atermpp::map< variable_type, expression_type >::const_iterator const_iterator;

        /// \brief Iterator type for non-constant element access
        typedef typename atermpp::map< variable_type, expression_type >::iterator       iterator;

      protected:

        /// \brief a mapping from variables to expressions
        atermpp::map< variable_type, expression_type > m_map;

      public:

        /// \brief Returns an iterator pointing to the beginning of the sequence of assignments
        const_iterator begin() const {
          return m_map.begin();
        }

        /// \brief Returns an iterator pointing past the end of the sequence of assignments
        const_iterator end() const {
          return m_map.end();
        }

        /// \brief Returns an iterator that references the expression associated with v or is equal to m_map.end()
        iterator find(variable_type const& v) const {
          return m_map.find(v);
        }

        /// \brief Apply on single single variable expression
        /// \param[in] v the variable for which to give the associated expression
        /// \return expression equivalent to <|s|>(<|e|>), or a reference to such an expression
        expression_type operator()(variable_type const& v) const {
          typename atermpp::map< variable_type, expression_type >::const_iterator i = m_map.find(v);

          if (i == m_map.end()) {
            return expression_type(v);
          }

          return i->second;
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
         * \note This overload is only available if Expression is not equal to Variable (modulo const-volatile qualifiers)
         **/
        expression_type operator()(typename detail::expression_type_or_inaccessible< Variable, Expression >::type const& e) const {
          return SubstitutionProcedure< substitution >::apply(*this, e);
        }

        /** \brief Comparison operation between substitutions
         *
         * \param[in] other object of another substitution object
         * \return whether the substitution expressed by this object is equivalent to <|other|>
         **/
        bool operator==(substitution const& other) const {
          const_iterator i = m_map.begin();
          const_iterator j = other.m_map.begin();

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

    /** \brief Generic substitution class (model of Substitution)
     *
     * Objects of this type represent mutable substitutions that can be applied
     * to expressions in order to generate new expressions. 
     *
     * \arg Variable type used to represent variables
     * \arg Expression type used to represent expressions
     * \arg SubstitutionProcedure procedure parametrised with a model of Substitution that is used for applying a substitution
     *
     * Model of MutableSubstitution.
     *
     * \note the default substitution procedure is term substitution that does
     * not take into account binders. In special cases one would typically a
     * need full-blown capture avoiding substitution procedure.
     **/
    template < typename Variable, typename Expression, template < typename Substitution > class SubstitutionProcedure >
    class mutable_substitution : public substitution< Variable, Expression, SubstitutionProcedure > {

      public:

        /// \brief type used to represent variables
        typedef Variable    variable_type;

        /// \brief type used to represent expressions
        typedef Expression  expression_type;

        /// \brief constant iterator type for individual assignments
        typedef typename substitution< Variable, Expression, SubstitutionProcedure >::const_iterator const_iterator;

        /// \brief iterator type for individual assignments
        typedef typename substitution< Variable, Expression, SubstitutionProcedure >::iterator       iterator;

        /// \brief Wrapper class for internal storage and substitution updates using operator()
        class assignment {

          private:

            variable_type                                   m_variable;
            atermpp::map< variable_type, expression_type >& m_map;

          public:

            /// \brief Constructor.
            ///
            /// \param[in] v a variable.
            /// \param[in] m a mapping of variables to expressions.
            assignment(variable_type v, atermpp::map< variable_type, expression_type >& m) :
                m_variable(v), m_map(m)
            {
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

      public:

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

        using substitution< Variable, Expression, SubstitutionProcedure >::begin;
        using substitution< Variable, Expression, SubstitutionProcedure >::end;
        using substitution< Variable, Expression, SubstitutionProcedure >::find;

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
        
        /// \brief Returns true if the sequence of assignments is empty
        bool empty() const {
          return this->m_map.empty();
        }
    };

    /// \brief Returns a string representation of the map, for example [a := 3, b := true].
    /// \param sigma a substitution.
    /// \return A string representation of the map.
    template <typename Substitution>
    std::string to_string(const Substitution& sigma)
    {
      std::stringstream result;
      result << "[";
      for (typename Substitution::const_iterator i = sigma.begin(); i != sigma.end(); ++i)
      {
        result << (i == sigma.begin() ? "" : "; ") << core::pp(i->first) << ":" << core::pp(i->first.sort()) << " := " << core::pp(i->second);
      }
      result << "]";
      return result.str();
    }

  }
}

#endif
