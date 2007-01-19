///////////////////////////////////////////////////////////////////////////////
/// \file mucalculus.h
/// Contains data data structures for mu calculus.

#ifndef LPE_CALCULUS_H
#define LPE_CALCULUS_H

#include <iostream> // for debugging

#include <string>
#include <cassert>
#include "atermpp/aterm_traits.h"
#include "atermpp/atermpp.h"
#include "atermpp/aterm_string.h"
#include "lpe/detail/utility.h"
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
class state_formula: public aterm_appl
{
  public:
    state_formula()
    {}

    state_formula(ATermAppl t)
      : aterm_appl(aterm_appl(t))
    {
      assert(check_rule_StateFrm(m_term));
    }

    state_formula(aterm_appl t)
      : aterm_appl(t)
    {
      assert(check_rule_StateFrm(m_term));
    }

    // example: "X(d:D,e:E)"
    state_formula(std::string s)
    {
      std::pair<std::string, data_expression_list> p = parse_variable(s);
      m_term = reinterpret_cast<ATerm>(gsMakeStateVar(aterm_string(p.first), p.second));
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
      return gsIsStateTrue(*this);
    }     

    /// Returns true if the state formula equals 'false'.
    /// Note that the term will not be rewritten first.
    ///
    bool is_false() const
    {
      return gsIsStateFalse(*this);
    }

    /// Applies a substitution to this state_formula and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    state_formula substitute(Substitution f) const
    {
      return state_formula(f(aterm(*this)));
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
class action_formula: public aterm_appl
{
  public:
    action_formula()
    {}

    action_formula(ATermAppl t)
      : aterm_appl(aterm_appl(t))
    {
      assert(check_rule_ActFrm(m_term));
    }

    action_formula(aterm_appl t)
      : aterm_appl(t)
    {
      assert(check_rule_ActFrm(m_term));
    }

    /// Returns true if the action formula equals 'true'.
    /// Note that the term will not be rewritten first.
    ///
    bool is_true() const
    {
      return gsIsActTrue(*this);
    }     

    /// Returns true if the action formula equals 'false'.
    /// Note that the term will not be rewritten first.
    ///
    bool is_false() const
    {
      return gsIsActFalse(*this);
    }

    /// Applies a substitution to this action_formula and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    action_formula substitute(Substitution f) const
    {
      return action_formula(f(aterm(*this)));
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
class regular_formula: public aterm_appl
{
  public:
    regular_formula()
    {}

    regular_formula(ATermAppl t)
      : aterm_appl(aterm_appl(t))
    {
      assert(check_rule_RegFrm(m_term));
    }

    regular_formula(aterm_appl t)
      : aterm_appl(t)
    {
      assert(check_rule_RegFrm(m_term));
    }

    /// Applies a substitution to this regular_formula and returns the result.
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
/// \brief singly linked list of data expressions
///
typedef term_list<regular_formula> regular_formula_list;

} // namespace lpe

namespace atermpp
{
using lpe::state_formula;
using lpe::action_formula;
using lpe::regular_formula;

template<>
struct aterm_traits<state_formula>
{
  typedef ATermAppl aterm_type;
  static void protect(lpe::state_formula t)   { t.protect(); }
  static void unprotect(lpe::state_formula t) { t.unprotect(); }
  static void mark(lpe::state_formula t)      { t.mark(); }
  static ATerm term(lpe::state_formula t)     { return t.term(); }
  static ATerm* ptr(lpe::state_formula& t)    { return &t.term(); }
};

template<>
struct aterm_traits<action_formula>
{
  typedef ATermAppl aterm_type;
  static void protect(action_formula t)   { t.protect(); }
  static void unprotect(action_formula t) { t.unprotect(); }
  static void mark(action_formula t)      { t.mark(); }
  static ATerm term(action_formula t)     { return t.term(); }
  static ATerm* ptr(action_formula& t)    { return &t.term(); }
};

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

#endif // LPE_CALCULUS_H

