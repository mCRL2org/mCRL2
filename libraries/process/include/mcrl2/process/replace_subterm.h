// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/replace_subterm.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_REPLACE_SUBTERM_H
#define MCRL2_PROCESS_REPLACE_SUBTERM_H

#include "mcrl2/process/builder.h"
#include "mcrl2/process/detail/position_count_traverser.h"

namespace mcrl2 {

namespace process {

namespace detail {

struct find_subterm_traverser: public process_expression_traverser<find_subterm_traverser>
{
  typedef process_expression_traverser<find_subterm_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  utilities::detail::position_counter counter;
  std::size_t xpos;
  std::size_t ypos; // depth
  process_expression result;

  find_subterm_traverser(std::size_t xpos_, std::size_t ypos_)
    : xpos(xpos_), ypos(ypos_)
  {}

  template <typename T>
  void visit(const T& x)
  {
    counter.increase();
    if (counter.at(xpos, ypos))
    {
      result = x;
    }
    super::apply(x);
    counter.decrease();
  }

  void apply(const action& x)
  {
    visit(x);
  }

  void apply(const process_instance& x)
  {
    visit(x);
  }

  void apply(const process_instance_assignment& x)
  {
    visit(x);
  }

  void apply(const delta& x)
  {
    visit(x);
  }

  void apply(const tau& x)
  {
    visit(x);
  }

  void apply(const sum& x)
  {
    visit(x);
  }

  void apply(const block& x)
  {
    visit(x);
  }

  void apply(const hide& x)
  {
    visit(x);
  }

  void apply(const rename& x)
  {
    visit(x);
  }

  void apply(const comm& x)
  {
    visit(x);
  }

  void apply(const allow& x)
  {
    visit(x);
  }

  void apply(const sync& x)
  {
    visit(x);
  }

  void apply(const at& x)
  {
    visit(x);
  }

  void apply(const seq& x)
  {
    visit(x);
  }

  void apply(const if_then& x)
  {
    visit(x);
  }

  void apply(const if_then_else& x)
  {
    visit(x);
  }

  void apply(const bounded_init& x)
  {
    visit(x);
  }

  void apply(const merge& x)
  {
    visit(x);
  }

  void apply(const left_merge& x)
  {
    visit(x);
  }

  void apply(const choice& x)
  {
    visit(x);
  }

  void apply(const stochastic_operator& x)
  {
    visit(x);
  }
};

struct replace_subterm_builder: public process_expression_builder<replace_subterm_builder>
{
  typedef process_expression_builder<replace_subterm_builder> super;
  using super::apply;
  using super::update;

  std::size_t xpos;
  std::size_t ypos; // depth
  process_expression replacement;
  bool ready = false;
  utilities::detail::position_counter counter;

  replace_subterm_builder(std::size_t xpos_, std::size_t ypos_, const process_expression& replacement_)
    : xpos(xpos_), ypos(ypos_), replacement(replacement_)
  {}

  template <typename T>
  process_expression visit(const T& x)
  {
    counter.increase();
    bool arrived = counter.at(xpos, ypos);
    process_expression result;
    if (counter.y > ypos)
    {
      result = x;
    }
    else if (arrived)
    {
      result = replacement;
    }
    else
    {
      result = super::apply(x);
    }
    counter.decrease();
    return result;
  }

  process_expression apply(const action& x)
  {
    return visit(x);
  }

  process_expression apply(const process_instance& x)
  {
    return visit(x);
  }

  process_expression apply(const process_instance_assignment& x)
  {
    return visit(x);
  }

  process_expression apply(const delta& x)
  {
    return visit(x);
  }

  process_expression apply(const tau& x)
  {
    return visit(x);
  }

  process_expression apply(const sum& x)
  {
    return visit(x);
  }

  process_expression apply(const block& x)
  {
    return visit(x);
  }

  process_expression apply(const hide& x)
  {
    return visit(x);
  }

  process_expression apply(const rename& x)
  {
    return visit(x);
  }

  process_expression apply(const comm& x)
  {
    return visit(x);
  }

  process_expression apply(const allow& x)
  {
    return visit(x);
  }

  process_expression apply(const sync& x)
  {
    return visit(x);
  }

  process_expression apply(const at& x)
  {
    return visit(x);
  }

  process_expression apply(const seq& x)
  {
    return visit(x);
  }

  process_expression apply(const if_then& x)
  {
    return visit(x);
  }

  process_expression apply(const if_then_else& x)
  {
    return visit(x);
  }

  process_expression apply(const bounded_init& x)
  {
    return visit(x);
  }

  process_expression apply(const merge& x)
  {
    return visit(x);
  }

  process_expression apply(const left_merge& x)
  {
    return visit(x);
  }

  process_expression apply(const choice& x)
  {
    return visit(x);
  }

  process_expression apply(const stochastic_operator& x)
  {
    return visit(x);
  }
};

} // namespace detail

/// \brief Replace the subterm at position (x, y) with a given term
inline
process_expression replace_subterm(const process_expression& expr, std::size_t x, std::size_t y, const process_expression& replacement)
{
  detail::replace_subterm_builder f(x, y, replacement);
  return f.apply(expr);
}

/// \brief Replace the subterm at position (x, y) with a given term
inline
process_specification replace_subterm(const process_specification& procspec, std::size_t x, std::size_t y, const process_expression& replacement)
{
  process_specification result = procspec;
  detail::replace_subterm_builder f(x, y, replacement);
  f.update(result);
  return result;
}

inline
process_expression find_subterm(const process_specification& procspec, std::size_t x, std::size_t y)
{
  detail::find_subterm_traverser f(x, y);
  f.apply(procspec);
  return f.result;
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_REPLACE_SUBTERM_H
