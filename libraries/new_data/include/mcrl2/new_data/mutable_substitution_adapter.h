// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/mutable_substitution_adapter.h
/// \brief An adapter that makes an arbitrary substitution function mutable.

#ifndef MCRL2_NEW_DATA_MUTABLE_SUBSTITUTION_ADAPTER_H
#define MCRL2_NEW_DATA_MUTABLE_SUBSTITUTION_ADAPTER_H

#include "mcrl2/new_data/substitution.h"

namespace mcrl2 {

namespace new_data {

  /// \brief An adapter that makes an arbitrary substitution function mutable.
  template <typename Substitution>
  class mutable_substitution_adapter
  {
    public:
      /// \brief type used to represent variables
      typedef typename Substitution::variable_type variable_type;

      /// \brief type used to represent expressions
      typedef typename Substitution::expression_type expression_type;

      /// \brief Wrapper class for internal storage and substitution updates using operator()
      typedef typename mutable_substitution<variable_type, expression_type>::assignment assignment;

      /// \brief The type of the wrapped substitution
      typedef Substitution substitution_type;

    protected:
      /// \brief The wrapped substitution
      const Substitution& f_;

      /// \brief An additional mutable substitution
      mutable_substitution<variable_type, expression_type> g_;

    public:
      /// \brief Constructor
      mutable_substitution_adapter(const Substitution& f)
        : f_(f)
      {}

      /// \brief Apply on single single variable expression
      /// \param[in] v the variable for which to give the associated expression
      /// \return expression equivalent to <|s|>(<|e|>), or a reference to such an expression
      expression_type operator()(variable_type const& v) const {
        return g_(f_(v));
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
      expression_type operator()(typename detail::expression_type_or_inaccessible< variable_type, expression_type >::type const& e) const {
        return g_(f_(e));
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
        return g_[v];
      }

      /// \brief Returns the wrapped substitution
      /// \return The wrapped substitution
      const substitution_type& substitution() const
      {
        return f_;
      }
  };

  /// \brief Specialization for mutable_substitution.
  template <typename Variable, typename Expression, template < class Substitution > class SubstitutionProcedure >
  class mutable_substitution_adapter<mutable_substitution<Variable, Expression, SubstitutionProcedure > >
  {
    public:
      /// \brief The type of the wrapped substitution
      typedef mutable_substitution<Variable, Expression, SubstitutionProcedure> substitution_type;

      /// \brief type used to represent variables
      typedef typename substitution_type::variable_type variable_type;

      /// \brief type used to represent expressions
      typedef typename substitution_type::expression_type expression_type;

      /// \brief Wrapper class for internal storage and substitution updates using operator()
      typedef typename substitution_type::assignment assignment;

    protected:
      mutable_substitution<Variable, Expression>& g_;

    public:
      mutable_substitution_adapter(mutable_substitution<Variable, Expression>& g)
        : g_(g)
      {}

      /// \brief Apply on single single variable expression
      /// \param[in] v the variable for which to give the associated expression
      /// \return expression equivalent to <|s|>(<|e|>), or a reference to such an expression
      expression_type operator()(variable_type const& v) const {
        return g_(v);
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
        return g_(e);
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
    /// \return A string representation of the map.
    template <typename Substitution>
    std::string to_string(const mutable_substitution_adapter<Substitution>& sigma)
    {
      return to_string(sigma.substitution());
    }

} // namespace new_data

} // namespace mcrl2

#endif // MCRL2_NEW_DATA_MUTABLE_SUBSTITUTION_ADAPTER_H
