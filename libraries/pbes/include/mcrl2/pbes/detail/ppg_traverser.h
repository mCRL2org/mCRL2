// Author(s): Gijs Kant
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/ppg_traverser.h
/// \brief Traverser class for Parameterised Parity Games (PPG), PBES expressions of the form:
/// PPG :== /\_{i: I} f_i && /\_{j: J} forall v: D_j . ( g_j => X_j(e_j) )
///       | \/_{i: I} f_i || \/_{j: J} exists v: D_j . ( g_j && X_j(e_j) ).
#ifndef MCRL2_PBES_DETAIL_PPG_TRAVERSER_H
#define MCRL2_PBES_DETAIL_PPG_TRAVERSER_H

#include <stack>
#include "mcrl2/pbes/traverser.h"
#include "mcrl2/pbes/pbes_functions.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// \cond INTERNAL_DOCS
/// \brief Visitor for checking if a pbes object is a PPG.
struct ppg_traverser: public pbes_expression_traverser<ppg_traverser>
{
  typedef pbes_expression_traverser<ppg_traverser> super;
  using super::enter;
  using super::leave;
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  enum expression_mode {
    CONJUNCTIVE, UNIVERSAL,
    DISJUNCTIVE, EXISTENTIAL,
    UNDETERMINED
  };

  std::string print_mode(expression_mode mode)
  {
    switch(mode)
    {
    case CONJUNCTIVE:
      return "Conjunctive";
      break;
    case UNIVERSAL:
      return "Universal";
      break;
    case DISJUNCTIVE:
      return "Disjunctive";
      break;
    case EXISTENTIAL:
      return "Existential";
      break;
    case UNDETERMINED:
      return "Undetermined";
      break;
    default:
      throw(std::runtime_error("Unknown mode!"));
    }
  }

  bool result;
  std::stack<expression_mode> mode_stack;

  ppg_traverser()
    : result(true)
  {}

  void enter(const pbes_system::forall& x)
  {
    expression_mode mode = mode_stack.top();
    bool simple_body = is_simple_expression(x.body());
    if (!simple_body)
    {
      //std::clog << "Note: 'forall' in mode " << print_mode(mode) << std::endl;
      switch(mode)
      {
      case UNDETERMINED:
      case CONJUNCTIVE:
        mode = UNIVERSAL;
      case UNIVERSAL:
        break;
      case DISJUNCTIVE:
      case EXISTENTIAL:
        result = false;
        break;
      default:
        break;
      }
    }
    mode_stack.push(mode);
  }

  void leave(const pbes_system::forall& x)
  {
    mode_stack.pop();
  }

  void enter(const pbes_system::exists& x)
  {
    expression_mode mode = mode_stack.top();
    bool simple_body = is_simple_expression(x.body());
    if (!simple_body)
    {
      //std::clog << "Note: 'exists' in mode " << print_mode(mode) << std::endl;
      switch(mode)
      {
      case UNDETERMINED:
      case DISJUNCTIVE:
        mode = EXISTENTIAL;
      case EXISTENTIAL:
        break;
      case CONJUNCTIVE:
      case UNIVERSAL:
        //std::clog << "Invalid: 'exists' in mode " << print_mode(mode) << std::endl;
        result = false;
        break;
      default:
        break;
      }
    }
    mode_stack.push(mode);
  }

  void leave(const pbes_system::exists& x)
  {
    mode_stack.pop();
  }

  void enter(const pbes_system::and_& x)
  {
    expression_mode mode = mode_stack.top();
    bool is_simple = is_simple_expression(x);
    if (!is_simple)
    {
      //std::clog << "Note: 'and' in mode " << print_mode(mode) << std::endl;
      switch(mode)
      {
      case UNDETERMINED:
        mode = CONJUNCTIVE;
        //std::clog << "Note: to mode " << print_mode(mode) << std::endl;
      case CONJUNCTIVE:
        break;
      case UNIVERSAL:
        //std::clog << "and: " << core::pp(x) << std::endl;
        //std::clog << "Invalid: 'and' in mode " << print_mode(mode) << std::endl;
        result = false;
        break;
      case EXISTENTIAL:
      case DISJUNCTIVE:
      {
        size_t count = 0;
        atermpp::vector<pbes_expression> conjuncts = pbes_expr::split_conjuncts(x);
        for(atermpp::vector<pbes_expression>::iterator it = conjuncts.begin(); it != conjuncts.end(); ++it)
        {
          if (!is_simple_expression(*it))
          {
            //std::clog << "and: " << core::pp(*it) << std::endl;
            count++;
            if (count > 1 || !is_propositional_variable_instantiation(*it))
            {
              //std::clog << "Invalid: 'and' in mode " << print_mode(mode) << std::endl;
              result = false;
              break;
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

  void leave(const pbes_system::and_& x)
  {
    mode_stack.pop();
  }

  void enter(const pbes_system::or_& x)
  {
    expression_mode mode = mode_stack.top();
    bool is_simple = is_simple_expression(x);
    if (!is_simple)
    {
      //std::clog << "Note: 'or' in mode " << print_mode(mode) << std::endl;
      switch(mode)
      {
      case UNDETERMINED:
        mode = DISJUNCTIVE;
      case DISJUNCTIVE:
        break;
      case EXISTENTIAL:
        //std::clog << "Invalid: 'or' in mode " << print_mode(mode) << std::endl;
        result = false;
        break;
      case UNIVERSAL:
      case CONJUNCTIVE:
      {
        size_t count = 0;
        atermpp::vector<pbes_expression>  disjuncts = pbes_expr::split_disjuncts(x);
        for(atermpp::vector<pbes_expression>::iterator it = disjuncts.begin(); it != disjuncts.end(); ++it)
        {
          if (!is_simple_expression(*it))
          {
            count++;
            if (count > 1 || !is_propositional_variable_instantiation(*it))
            {
              ///std::clog << "Invalid: 'or' in mode " << print_mode(mode) << std::endl;
              result = false;
              break;
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

  void leave(const pbes_system::or_& x)
  {
    mode_stack.pop();
  }

  void enter(const pbes_equation& x)
  {
    //std::clog << "Equation " << x.variable().name() << std::endl;
    mode_stack.push(UNDETERMINED);
  }

  void leave(const pbes_equation& x)
  {
    mode_stack.pop();
  }

};
/// \endcond

/// \brief Determines if an expression is a PPG expression.
/// \param x a PBES object
/// \return true if x is a PPG expression.
template <typename T>
bool is_ppg(const T& x)
{
  ppg_traverser f;
  f(x);
  return f.result;
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PPG_TRAVERSER_H
