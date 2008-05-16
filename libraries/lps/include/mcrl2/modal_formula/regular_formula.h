// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/regular_formula.h
/// \brief Add your file description here.

#ifndef MCRL2_MODAL_REGULAR_FORMULA_H
#define MCRL2_MODAL_REGULAR_FORMULA_H

#include <iostream> // for debugging

#include <string>
#include <cassert>
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/soundness_checks.h"

namespace mcrl2 {

namespace modal {

using atermpp::aterm_appl;
using atermpp::aterm_list;
using atermpp::aterm;
using atermpp::term_list;

///////////////////////////////////////////////////////////////////////////////
// regular_formula
/// \brief regular formula expression.
///
class regular_formula: public aterm_appl
{
  public:
    regular_formula()
      : aterm_appl(mcrl2::core::detail::constructRegFrm())
    {}

    regular_formula(ATermAppl t)
      : aterm_appl(aterm_appl(t))
    {
      assert(mcrl2::core::detail::check_rule_RegFrm(m_term));
    }

    regular_formula(aterm_appl t)
      : aterm_appl(t)
    {
      assert(mcrl2::core::detail::check_rule_RegFrm(m_term));
    }

    /// Applies a substitution to this regular formula and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    regular_formula substitute(Substitution f) const
    {
      return regular_formula(f(aterm(*this)));
    }     
};

///////////////////////////////////////////////////////////////////////////////
// regular_formula_list
/// \brief singly linked list of regular expressions
///
typedef term_list<regular_formula> regular_formula_list;

} // namespace modal

} // namespace mcrl2

/// \internal
namespace atermpp
{
using mcrl2::modal::regular_formula;

template<>
struct aterm_traits<regular_formula>
{
  typedef ATermAppl aterm_type;
  static void protect(regular_formula t)   { t.protect(); }
  static void unprotect(regular_formula t) { t.unprotect(); }
  static void mark(regular_formula t)      { t.mark(); }
  static ATerm term(regular_formula t)     { return t.term(); }
  static ATerm* ptr(regular_formula& t)    { return &t.term(); }
};

} // namespace atermpp

#endif // MCRL2_MODAL_REGULAR_FORMULA_H
