// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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

namespace mcrl2::process
{

namespace detail {

struct find_subterm_traverser: public process_expression_traverser<find_subterm_traverser>
{
  using super = process_expression_traverser<find_subterm_traverser>;
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
  using super = process_expression_builder<replace_subterm_builder>;
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
      super::apply(result, x);
    }
    counter.decrease();
    return result;
  }

  template <class T>
  void apply(T& result, const action& x)
  {
    result = visit(x);
  }

  template <class T>
  void apply(T& result, const process_instance& x)
  {
    result = visit(x);
  }

  template <class T>
  void apply(T& result, const process_instance_assignment& x)
  {
    result = visit(x);
  }

  template <class T>
  void apply(T& result, const delta& x)
  {
    result = visit(x);
  }

  template <class T>
  void apply(T& result, const tau& x)
  {
    result = visit(x);
  }

  template <class T>
  void apply(T& result, const sum& x)
  {
    result = visit(x);
  }

  template <class T>
  void apply(T& result, const block& x)
  {
    result = visit(x);
  }

  template <class T>
  void apply(T& result, const hide& x)
  {
    result = visit(x);
  }

  template <class T>
  void apply(T& result, const rename& x)
  {
    result = visit(x);
  }

  template <class T>
  void apply(T& result, const comm& x)
  {
    result = visit(x);
  }

  template <class T>
  void apply(T& result, const allow& x)
  {
    result = visit(x);
  }

  template <class T>
  void apply(T& result, const sync& x)
  {
    result = visit(x);
  }

  template <class T>
  void apply(T& result, const at& x)
  {
    result = visit(x);
  }

  template <class T>
  void apply(T& result, const seq& x)
  {
    result = visit(x);
  }

  template <class T>
  void apply(T& result, const if_then& x)
  {
    result = visit(x);
  }

  template <class T>
  void apply(T& result, const if_then_else& x)
  {
    result = visit(x);
  }

  template <class T>
  void apply(T& result, const bounded_init& x)
  {
    result = visit(x);
  }

  template <class T>
  void apply(T& result, const merge& x)
  {
    result = visit(x);
  }

  template <class T>
  void apply(T& result, const left_merge& x)
  {
    result = visit(x);
  }

  template <class T>
  void apply(T& result, const choice& x)
  {
    result = visit(x);
  }

  template <class T>
  void apply(T& result, const stochastic_operator& x)
  {
    result = visit(x);
  }
};

} // namespace detail

/// \brief Replace the subterm at position (x, y) with a given term
inline
process_expression replace_subterm(const process_expression& expr, std::size_t x, std::size_t y, const process_expression& replacement)
{
  detail::replace_subterm_builder f(x, y, replacement);
  process_expression result;
  f.apply(result, expr);
  return result;
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

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_REPLACE_SUBTERM_H
