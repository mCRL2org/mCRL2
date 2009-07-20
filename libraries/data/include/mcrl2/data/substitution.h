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
#include "boost/type_traits/remove_reference.hpp"
#include "boost/iterator/filter_iterator.hpp"

#include "mcrl2/data/replace.h"

namespace mcrl2 {
  namespace data {

    // declare for use as template argument defaults
    class data_expression;
    class variable;

    /// \cond INTERNAL_DOCS
    namespace detail {
      template < class Derived >
      struct substitution_procedure {

         template < typename Substitution, typename Expression >
         static Expression apply(Substitution const& s, Expression const& e)
         {
           return Derived::generic_apply(s, e);
         }

         template < typename Substitution >
         static data_expression apply(Substitution const& s, typename Substitution::variable_type const& e,
                   typename boost::disable_if< typename boost::is_same< typename Substitution::variable_type, variable >::type >::type* = 0)
         {
           return s(e);
         }

         template < typename Substitution >
         static data_expression apply(Substitution const& s, data_expression const& e,
                   typename boost::enable_if< typename boost::is_same< typename Substitution::variable_type, variable >::type >::type* = 0)
         {
           return (e.is_variable()) ? s(variable(e)) : Derived::generic_apply(s, e);
         }
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
    struct textual_substitution : public detail::substitution_procedure< textual_substitution< Substitution > > {
      /// Function Arguments:
      ///  \param[in] s a substitution object that is only used for variable lookup
      ///  \param[in] e the expression on which to apply variable replacement according to the substitution
      /// \return the result of substitution
      template < typename Expression >
      static data_expression generic_apply(Substitution const& s, Expression const& e) {
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
    struct structural_substitution : public detail::substitution_procedure< structural_substitution< Substitution > > {
      /// Function Arguments:
      ///  \param[in] s a substitution object that is only used for variable lookup
      ///  \param[in] e the expression on which to apply variable replacement according to the substitution
      /// \return the result of substitution
      template < typename Expression >
      static data_expression generic_apply(Substitution const& s, Expression const& e) {
        return replace_free_variables(e, s);
      }
    };

    template < typename Derived,
               typename Variable = data::variable,
               typename Expression = data::data_expression,
               template < typename Substitution > class SubstitutionProcedure = structural_substitution >
    class substitution;

    /** \brief Generic substitution class (model of Substitution)
     *
     * Objects of this type represent mutable substitutions that can be applied
     * to expressions in order to generate new expressions. 
     *
     * \arg Derived represents a derived class (per CRTP)
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
    template < typename Derived, typename Variable, typename Expression, template < typename Substitution > class SubstitutionProcedure >
    class substitution : public std::unary_function< Expression, Expression > {

      public:

        /// \brief type used to represent variables
        typedef Variable                   variable_type;

        /// \brief type used to represent expressions
        typedef Expression                 expression_type;

      public:

        /// \brief Apply on single single variable expression
        /// \param[in] v the variable for which to give the associated expression
        /// \return expression equivalent to <|s|>(<|e|>), or a reference to such an expression
        expression_type operator()(variable_type const& v) const {
          return static_cast< Derived const& >(*this).apply(v);
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
        template < typename OtherExpression >
        expression_type operator()(OtherExpression const& e) const {
          return SubstitutionProcedure< Derived >::apply(static_cast< Derived const& >(*this), e);
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
        result << (i == sigma.begin() ? "" : "; ") << data::pp(i->first) << ":" << data::pp(i->first.sort()) << " := " << data::pp(i->second);
      }
      result << "]";
      return result.str();
    }

    /** \brief Function object for applying substitutions to an expression
     *
     * \arg[in] Expression the type of the resulting rexpression
     **/
    template < typename Expression >
    class apply_to_expression {

      private:

        Expression m_base_expression;
      
      public:

        apply_to_expression(Expression const& e) : m_base_expression(e)
        {}

        apply_to_expression()
        {}

        template < typename Substitution >
        Expression operator()(Substitution const& s) const
        {
          return s(m_base_expression);
        }
    };

  }
}

#endif
