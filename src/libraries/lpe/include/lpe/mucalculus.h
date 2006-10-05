///////////////////////////////////////////////////////////////////////////////
/// \file mucalculus.h
/// Contains data data structures for mu calculus.

#ifndef LPE_CALCULUS_H
#define LPE_CALCULUS_H

#include <iostream> // for debugging

#include <string>
#include <cassert>
#include "atermpp/atermpp.h"
#include "atermpp/aterm_string.h"
#include "lpe/detail/utility.h"
#include "lpe/aterm_wrapper.h"
#include "lpe/substitute.h"
#include "lpe/sort.h"
#include "lpe/action.h"
#include "lpe/data.h"
#include "libstruct.h"

namespace lpe {

using atermpp::aterm_appl;
using atermpp::aterm_list;
using atermpp::aterm_string;
using atermpp::aterm;
using atermpp::term_list;
using lpe::detail::parse_variable;

///////////////////////////////////////////////////////////////////////////////
// state_formula
/// \brief state formula expression.
///
class state_formula: public aterm_appl_wrapper
{
  public:
    state_formula()
    {}

    state_formula(ATermAppl t)
      : aterm_appl_wrapper(aterm_appl(t))
    {
      assert(gsIsStateFrm(t));
    }

    state_formula(aterm_appl t)
      : aterm_appl_wrapper(t)
    {
      assert(gsIsStateFrm(t));
    }

    // example: "X(d:D,e:E)"
    state_formula(std::string s)
    {
      std::pair<std::string, data_expression_list> p = parse_variable(s);
      m_term = gsMakeStateVar(aterm_string(p.first), p.second);
    }

    /// Returns true if every propositional variable occurring in the formula
    /// is bound by a 'nu' and every data variable occurring in the formula is
    /// bound by a 'forall'.
    /// NOT YET IMPLEMENTED
    ///
    bool is_closed() const
    {
      assert(false);
      return true;
    }     

    /// Returns true if the state formula equals 'true'.
    /// Note that the term will not be rewritten first.
    ///
    bool is_true() const
    {
      return gsIsStateTrue(m_term);
    }     

    /// Returns true if the state formula equals 'false'.
    /// Note that the term will not be rewritten first.
    ///
    bool is_false() const
    {
      return gsIsStateFalse(m_term);
    }

    /// Applies a substitution to this state_formula and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    state_formula substitute(Substitution f) const
    {
      return state_formula(f(aterm_appl(*this)));
    }     
};

///////////////////////////////////////////////////////////////////////////////
// state_formula_list
/// \brief singly linked list of data expressions
///
typedef term_list<state_formula> state_formula_list;

///////////////////////////////////////////////////////////////////////////////
// action_formula
/// \brief action formula expression.
///
class action_formula: public aterm_appl_wrapper
{
  public:
    action_formula()
    {}

    action_formula(ATermAppl t)
      : aterm_appl_wrapper(aterm_appl(t))
    {
      assert(gsIsActFrm(t));
    }

    action_formula(aterm_appl t)
      : aterm_appl_wrapper(t)
    {
      assert(gsIsActFrm(t));
    }

    /// Returns true if the action formula equals 'true'.
    /// Note that the term will not be rewritten first.
    ///
    bool is_true() const
    {
      return gsIsActTrue(m_term);
    }     

    /// Returns true if the action formula equals 'false'.
    /// Note that the term will not be rewritten first.
    ///
    bool is_false() const
    {
      return gsIsActFalse(m_term);
    }

    /// Applies a substitution to this action_formula and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    action_formula substitute(Substitution f) const
    {
      return action_formula(f(aterm_appl(*this)));
    }     
};

///////////////////////////////////////////////////////////////////////////////
// action_formula_list
/// \brief singly linked list of data expressions
///
typedef term_list<action_formula> action_formula_list;

///////////////////////////////////////////////////////////////////////////////
// regular_formula
/// \brief regular formula expression.
///
class regular_formula: public aterm_appl_wrapper
{
  public:
    regular_formula()
    {}

    regular_formula(ATermAppl t)
      : aterm_appl_wrapper(aterm_appl(t))
    {
      assert(gsIsRegFrm(t));
    }

    regular_formula(aterm_appl t)
      : aterm_appl_wrapper(t)
    {
      assert(gsIsRegFrm(t));
    }

    /// Applies a substitution to this regular_formula and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    regular_formula substitute(Substitution f) const
    {
      return regular_formula(f(aterm_appl(*this)));
    }     
};

///////////////////////////////////////////////////////////////////////////////
// regular_formula_list
/// \brief singly linked list of data expressions
///
typedef term_list<regular_formula> regular_formula_list;

} // namespace lpe

#endif // LPE_CALCULUS_H

