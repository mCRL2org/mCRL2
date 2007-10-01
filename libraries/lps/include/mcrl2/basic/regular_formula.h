// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/basic/regular_formula.h
/// \brief Add your file description here.

#ifndef MCRL2_BASIC_REGULAR_FORMULA_H
#define MCRL2_BASIC_REGULAR_FORMULA_H

#include <iostream> // for debugging

#include <string>
#include <cassert>
#include "atermpp/aterm_traits.h"
#include "atermpp/atermpp.h"
#include "mcrl2/basic/detail/constructors.h"
#include "mcrl2/basic/detail/soundness_checks.h"

namespace lps {

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
      : aterm_appl(detail::constructRegFrm())
    {}

    regular_formula(ATermAppl t)
      : aterm_appl(aterm_appl(t))
    {
      assert(detail::check_rule_RegFrm(m_term));
    }

    regular_formula(aterm_appl t)
      : aterm_appl(t)
    {
      assert(detail::check_rule_RegFrm(m_term));
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

} // namespace lps

/// \internal
namespace atermpp
{
using lps::regular_formula;

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

#endif // MCRL2_BASIC_REGULAR_FORMULA_H
