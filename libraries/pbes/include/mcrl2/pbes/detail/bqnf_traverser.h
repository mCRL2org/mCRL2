// Author(s): Gijs Kant
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/bqnf_traverser.h
/// \brief Traverser class for PBESs in Bounded Quantifier Normal Form (BQNF):
/// BQNF :== forall d: D . b => BQNF  |  exists d: D . b && BQNF  |  CONJ
/// CONJ :== And_{k: K} f_k && And_{i: I} forall v: D_I . g_i => DISJ^i
/// DISJ^i :== Or_{l: L_i} f_{il} || Or_{j: J_i} exists w: D_{ij} . g_{ij} && X_{ij}(e_{ij})
#ifndef MCRL2_PBES_DETAIL_BQNF_TRAVERSER_H
#define MCRL2_PBES_DETAIL_BQNF_TRAVERSER_H

#include <stack>
#include "mcrl2/pbes/traverser.h"
#include "mcrl2/pbes/pbes_functions.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// \cond INTERNAL_DOCS
/// \brief Visitor for checking if a pbes object is in BQNF.
struct bqnf_traverser: public pbes_expression_traverser<bqnf_traverser>
{
  typedef pbes_expression_traverser<bqnf_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  enum expression_mode {
    BOUNDED_FORALL, BOUNDED_EXISTS,
    CONJUNCTIVE, UNIVERSAL,
    DISJUNCTIVE, EXISTENTIAL,
    UNDETERMINED
  };

  bool result;
  std::stack<expression_mode> mode_stack;

  bqnf_traverser()
    : result(true)
  {}

  void enter(const pbes_system::forall& x)
  {
    expression_mode mode = mode_stack.top();
    bool simple_body = is_simple_expression(x.body());
    if (!simple_body)
    {
      switch(mode)
      {
      case UNDETERMINED:
      case BOUNDED_EXISTS:
        mode = BOUNDED_FORALL;
      case BOUNDED_FORALL:
        break;
      case CONJUNCTIVE:
        mode = UNIVERSAL;
      case UNIVERSAL:
        break;
      case DISJUNCTIVE:
      case EXISTENTIAL:
        //std::clog << "forall in mode " << mode << std::endl;
        result = false;
        break;
      default:
        break;
      }
    }
    mode_stack.push(mode);
  }

  void leave(const pbes_system::forall& /*x*/)
  {
    mode_stack.pop();
  }

  void enter(const pbes_system::exists& x)
  {
    expression_mode mode = mode_stack.top();
    bool simple_body = is_simple_expression(x.body());
    if (!simple_body)
    {
      switch(mode)
      {
      case UNDETERMINED:
      case BOUNDED_FORALL:
        mode = BOUNDED_EXISTS;
      case BOUNDED_EXISTS:
        break;
      case DISJUNCTIVE:
      case CONJUNCTIVE:
      case UNIVERSAL:
        mode = EXISTENTIAL;
      case EXISTENTIAL:
        break;
      default:
        break;
      }
    }
    mode_stack.push(mode);
  }

  void leave(const pbes_system::exists& /*x*/)
  {
    mode_stack.pop();
  }

  void enter(const pbes_system::and_& x)
  {
    expression_mode mode = mode_stack.top();
    bool is_simple = is_simple_expression(x);
    if (!is_simple)
    {
      switch(mode)
      {
      case UNDETERMINED:
      case BOUNDED_FORALL:
        mode = CONJUNCTIVE;
      case CONJUNCTIVE:
        break;
      case BOUNDED_EXISTS:
      {
        size_t count = 0;
        for(const pbes_expression& conjunct: split_conjuncts(x))
        {
          if (!is_simple_expression(conjunct))
          {
            count++;
          }
        }
        if (count > 1)
        {
          mode = CONJUNCTIVE;
        }
        break;
      }
      case UNIVERSAL:
      case DISJUNCTIVE:
      case EXISTENTIAL:
      {
        size_t count = 0;
        for(const pbes_expression& conjunct : split_conjuncts(x))
        {
          if (!is_simple_expression(conjunct))
          {
            count++;
            if (count > 1 || !is_propositional_variable_instantiation(conjunct))
            {
              //std::clog << "and in mode " << mode << std::endl;
              result = false;
            }
          }
        }
        break;
      }
      default:
        break;
      }
    }
    mode_stack.push(mode);
  }

  void leave(const pbes_system::and_& /*x*/)
  {
    mode_stack.pop();
  }

  void enter(const pbes_system::or_& x)
  {
    expression_mode mode = mode_stack.top();
    bool is_simple = is_simple_expression(x);
    if (!is_simple)
    {
      switch(mode)
      {
      case UNDETERMINED:
      case BOUNDED_EXISTS:
        mode = DISJUNCTIVE;
      case DISJUNCTIVE:
        break;
      case BOUNDED_FORALL:
      case CONJUNCTIVE:
      case UNIVERSAL:
      {
        size_t count = 0;
        for(const pbes_expression& disjunct: split_disjuncts(x))
        {
          if (!is_simple_expression(disjunct))
          {
            count++;
          }
        }
        if (count > 1)
        {
          mode = DISJUNCTIVE;
        }
        break;
      }
      case EXISTENTIAL:
        //std::clog << "or in mode " << mode << std::endl;
        result = false;
        break;
      default:
        break;
      }
    }
    mode_stack.push(mode);
  }

  void leave(const pbes_system::or_& /*x*/)
  {
    mode_stack.pop();
  }

  void enter(const pbes_equation& /*x*/)
  {
    mode_stack.push(UNDETERMINED);
  }

  void leave(const pbes_equation& /*x*/)
  {
    mode_stack.pop();
  }

};
/// \endcond

/// \brief Determines if an expression is a BQNF expression.
/// \param x a PBES object
/// \return true if x is a BQNF expression.
template <typename T>
bool is_bqnf(const T& x)
{
  bqnf_traverser f;
  f.apply(x);
  return f.result;
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_BQNF_TRAVERSER_H
