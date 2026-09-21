// Author(s): Gijs Kant
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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

#include "mcrl2/pbes/pbes_functions.h"





namespace mcrl2::pbes_system::detail {

/// \cond INTERNAL_DOCS
/// \brief Visitor for checking if a pbes object is in BQNF.
struct bqnf_traverser: public pbes_expression_traverser<bqnf_traverser>
{
  using super = pbes_expression_traverser<bqnf_traverser>;
  using super::enter;
  using super::leave;
  using super::apply;

  enum class expression_mode {
    BOUNDED_FORALL, BOUNDED_EXISTS,
    CONJUNCTIVE, UNIVERSAL,
    DISJUNCTIVE, EXISTENTIAL,
    UNDETERMINED
  };

  bool result = true;
  std::stack<expression_mode> mode_stack;

  void enter(const pbes_system::forall& x)
  {
    expression_mode mode = mode_stack.top();
    bool simple_body = is_simple_expression(x.body(), false);
    if (!simple_body)
    {
      switch(mode)
      {
      case expression_mode::UNDETERMINED:
      case expression_mode::BOUNDED_EXISTS:
        mode = expression_mode::BOUNDED_FORALL;
      case expression_mode::BOUNDED_FORALL:
        break;
      case expression_mode::CONJUNCTIVE:
        mode = expression_mode::UNIVERSAL;
      case expression_mode::UNIVERSAL:
        break;
      case expression_mode::DISJUNCTIVE:
      case expression_mode::EXISTENTIAL:
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
    bool simple_body = is_simple_expression(x.body(), false);
    if (!simple_body)
    {
      switch(mode)
      {
      case expression_mode::UNDETERMINED:
      case expression_mode::BOUNDED_FORALL:
        mode = expression_mode::BOUNDED_EXISTS;
      case expression_mode::BOUNDED_EXISTS:
        break;
      case expression_mode::DISJUNCTIVE:
      case expression_mode::CONJUNCTIVE:
      case expression_mode::UNIVERSAL:
        mode = expression_mode::EXISTENTIAL;
      case expression_mode::EXISTENTIAL:
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
    bool is_simple = is_simple_expression(x, false);
    if (!is_simple)
    {
      switch(mode)
      {
      case expression_mode::UNDETERMINED:
      case expression_mode::BOUNDED_FORALL:
        mode = expression_mode::CONJUNCTIVE;
      case expression_mode::CONJUNCTIVE:
        break;
      case expression_mode::BOUNDED_EXISTS:
      {
        std::size_t count = 0;
        for(const pbes_expression& conjunct: split_conjuncts(x))
        {
          if (!is_simple_expression(conjunct, false))
          {
            count++;
          }
        }
        if (count > 1)
        {
          mode = expression_mode::CONJUNCTIVE;
        }
        break;
      }
      case expression_mode::UNIVERSAL:
      case expression_mode::DISJUNCTIVE:
      case expression_mode::EXISTENTIAL:
      {
        std::size_t count = 0;
        for(const pbes_expression& conjunct : split_conjuncts(x))
        {
          if (!is_simple_expression(conjunct, false))
          {
            count++;
            if (count > 1 || !is_propositional_variable_instantiation(conjunct))
            {
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
    bool is_simple = is_simple_expression(x, false);
    if (!is_simple)
    {
      switch(mode)
      {
      case expression_mode::UNDETERMINED:
      case expression_mode::BOUNDED_EXISTS:
        mode = expression_mode::DISJUNCTIVE;
      case expression_mode::DISJUNCTIVE:
        break;
      case expression_mode::BOUNDED_FORALL:
      case expression_mode::CONJUNCTIVE:
      case expression_mode::UNIVERSAL:
      {
        std::size_t count = 0;
        for(const pbes_expression& disjunct: split_disjuncts(x))
        {
          if (!is_simple_expression(disjunct, false))
          {
            count++;
          }
        }
        if (count > 1)
        {
          mode = expression_mode::DISJUNCTIVE;
        }
        break;
      }
      case expression_mode::EXISTENTIAL:
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
    mode_stack.push(expression_mode::UNDETERMINED);
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

} // namespace mcrl2::pbes_system::detail





#endif // MCRL2_PBES_DETAIL_BQNF_TRAVERSER_H
