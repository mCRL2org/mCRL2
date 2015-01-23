// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/detail/has_name_clashes.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_HAS_NAME_CLASHES_H
#define MCRL2_MODAL_FORMULA_HAS_NAME_CLASHES_H

#include <vector>
#include "mcrl2/modal_formula/traverser.h"
#include "mcrl2/utilities/exception.h"

namespace mcrl2
{

namespace state_formulas
{

namespace detail
{

/// \brief Traverser that checks for name clashes in nested mu's/nu's.
class state_formula_name_clash_checker: public state_formulas::state_formula_traverser<state_formula_name_clash_checker>
{
  public:
    typedef state_formulas::state_formula_traverser<state_formula_name_clash_checker> super;

    using super::apply;
    using super::enter;
    using super::leave;

    /// \brief The stack of names.
    std::vector<core::identifier_string> m_name_stack;

    /// \brief Pops the stack
    void pop()
    {
      m_name_stack.pop_back();
    }

    /// \brief Pushes name on the stack.
    void push(const core::identifier_string& name)
    {
      if (std::find(m_name_stack.begin(), m_name_stack.end(), name) != m_name_stack.end())
      {
        throw mcrl2::runtime_error("nested propositional variable " + std::string(name) + " clashes");
      }
      m_name_stack.push_back(name);
    }

    void enter(const mu& x)
    {
      push(x.name());
    }

    void leave(const mu&)
    {
      pop();
    }

    void enter(const nu& x)
    {
      push(x.name());
    }

    void leave(const nu&)
    {
      pop();
    }
};

} // namespace detail

/// \brief Throws a mcrl2::runtime_exception if the formula contains name clashes
inline
bool has_name_clashes(const state_formula& f)
{
  try
  {
    detail::state_formula_name_clash_checker checker;
    checker.apply(f);
  }
  catch (mcrl2::runtime_error)
  {
    return true;
  }
  return false;
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_HAS_NAME_CLASHES_H
