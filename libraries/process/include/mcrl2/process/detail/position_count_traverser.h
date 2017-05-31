// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/position_count_traverser.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_POSITION_COUNT_TRAVERSER_H
#define MCRL2_PROCESS_DETAIL_POSITION_COUNT_TRAVERSER_H

#include "mcrl2/process/traverser.h"
#include "mcrl2/utilities/detail/position_counter.h"

namespace mcrl2 {

namespace process {

namespace detail {

struct position_count_traverser: public process_expression_traverser<position_count_traverser>
{
  typedef process_expression_traverser<position_count_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  utilities::detail::position_counter counter;

  template <typename T>
  void visit(const T& x)
  {
    counter.increase();
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

inline
std::vector<std::size_t> position_counts(const process_specification& x)
{
  detail::position_count_traverser f;
  f.apply(x);
  return f.counter.ycounts;
}

} // namespace detail

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_POSITION_COUNT_TRAVERSER_H
