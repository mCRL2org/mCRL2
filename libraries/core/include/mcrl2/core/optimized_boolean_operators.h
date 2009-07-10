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

  /// \brief Make a negation
  /// \param arg A term
  /// \return The application of not to the argument.
  template <typename Term>
  inline
  Term optimized_not(Term arg)
  {
    typedef typename core::term_traits<Term> tr;
    return detail::optimized_not(arg, tr::not_, tr::true_(), tr::is_true, tr::false_(), tr::is_false);
  }

  /// \brief Make a conjunction
  /// \param p A term
  /// \param q A term
  /// \return The application of and to the arguments.
  template <typename Term>
  inline
  Term optimized_and(Term p, Term q)
  {
    typedef typename core::term_traits<Term> tr;
    return detail::optimized_and(p, q, tr::and_, tr::true_(), tr::is_true, tr::false_(), tr::is_false);
  }

  /// \brief Make a disjunction
  /// \param p A term
  /// \param q A term
  /// \return The application of or to the arguments.
  template <typename Term>
  inline
  Term optimized_or(Term p, Term q)
  {
    typedef typename core::term_traits<Term> tr;
    return detail::optimized_or(p, q, tr::or_, tr::true_(), tr::is_true, tr::false_(), tr::is_false);
  }

  /// \brief Make an implication
  /// \param p A term
  /// \param q A term
  /// \return The application of implication to the arguments.
  template <typename Term>
  inline
  Term optimized_imp(Term p, Term q)
  {
    typedef typename core::term_traits<Term> tr;
    return detail::optimized_imp(p, q, tr::imp, tr::not_, tr::true_(), tr::is_true, tr::false_(), tr::is_false);
  }

  /// \brief Make a universal quantification
  /// \param l A sequence of variables
  /// \param p A term
  /// \return The application of universal quantification to the arguments.
  template <typename Term, typename VariableSequence>
  inline
  Term optimized_forall(VariableSequence l, Term p)
  {
    typedef typename core::term_traits<Term> tr;
    return detail::optimized_forall(l, p, tr::forall, tr::true_(), tr::is_true, tr::false_(), tr::is_false);
  }

  /// \brief Make an existential quantification
  /// \param l A sequence of variables
  /// \param p A term
  /// \return The application of existential quantification to the arguments.
  template <typename Term, typename VariableSequence>
  inline
  Term optimized_exists(VariableSequence l, Term p)
  {
    typedef typename core::term_traits<Term> tr;
    return detail::optimized_exists(l, p, tr::exists, tr::true_(), tr::is_true, tr::false_(), tr::is_false);
  }

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_OPTIMIZED_BOOLEAN_OPERATORS_H
