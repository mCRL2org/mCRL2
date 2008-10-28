// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/optimized_boolean_operators.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_OPTIMIZED_BOOLEAN_OPERATORS_H
#define MCRL2_CORE_OPTIMIZED_BOOLEAN_OPERATORS_H

#include "mcrl2/core/detail/optimized_logic_operators.h"
#include "mcrl2/core/term_traits.h"

namespace mcrl2 {

namespace core { 

  /// Operator not.
  ///
  /// \param arg A term.
  /// \return The application of not to the argument.
  template <typename Term>
  Term optimized_not(Term arg)
  {
    typedef typename core::term_traits<Term> tr;
    return detail::optimized_not(arg, tr::not_, tr::true_(), tr::is_true, tr::false_(), tr::is_false);
  }

  /// Operator and.
  ///
  /// \param left A term.
  /// \param right A term.
  /// \return The application of and to the arguments.
  template <typename Term>
  Term optimized_and(Term left, Term right)
  {
    typedef typename core::term_traits<Term> tr;
    return detail::optimized_and(left, right, tr::and_, tr::true_(), tr::is_true, tr::false_(), tr::is_false);
  }

  /// Operator or.
  ///
  /// \param left A term.
  /// \param right A term.
  /// \return The application of or to the arguments.
  template <typename Term>
  Term optimized_or(Term left, Term right)
  {
    typedef typename core::term_traits<Term> tr;
    return detail::optimized_or(left, right, tr::or_, tr::true_(), tr::is_true, tr::false_(), tr::is_false);
  }

  /// Implication.
  ///
  /// \param left A term.
  /// \param right A term.
  /// \return The application of implication to the arguments.
  template <typename Term>
  Term optimized_imp(Term left, Term right)
  {
    typedef typename core::term_traits<Term> tr;
    return detail::optimized_imp(left, right, tr::imp, tr::not_, tr::true_(), tr::is_true, tr::false_(), tr::is_false);
  }

  /// Universal quantification.
  ///
  /// \param v A sequence of variables.
  /// \param arg A term.
  /// \return The application of universal quantification to the arguments.
  template <typename Term, typename VariableSequence>
  Term optimized_forall(VariableSequence v, Term arg)
  {
    typedef typename core::term_traits<Term> tr;
    return detail::optimized_forall(v, arg, tr::forall, tr::true_(), tr::is_true, tr::false_(), tr::is_false);
  }

  /// Existential quantification.
  ///
  /// \param v A sequence of variables.
  /// \param arg A term.
  /// \return The application of existential quantification to the arguments.
  template <typename Term, typename VariableSequence>
  Term optimized_exists(VariableSequence v, Term arg)
  {
    typedef typename core::term_traits<Term> tr;
    return detail::optimized_exists(v, arg, tr::exists, tr::true_(), tr::is_true, tr::false_(), tr::is_false);
  }

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_OPTIMIZED_BOOLEAN_OPERATORS_H
