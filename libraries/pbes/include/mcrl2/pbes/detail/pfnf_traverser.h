// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pfnf_traverser.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_PFNF_TRAVERSER_H
#define MCRL2_PBES_DETAIL_PFNF_TRAVERSER_H

#include <algorithm>
#include <iterator>
#include <numeric>
#include <utility>
#include <vector>
#include <stdexcept>
#include <set>
#include <sstream>
#include <functional>
#include "mcrl2/utilities/optimized_boolean_operators.h"
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/replace.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/traverser.h"

#ifdef MCRL2_PFNF_VISITOR_DEBUG
#include "mcrl2/data/print.h"
#endif

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

/// \brief Represents a quantifier Qv:V. If the bool is true it is a forall, otherwise an exists.
typedef std::pair<bool, data::variable_list> pfnf_traverser_quantifier;

struct variable_variable_substitution: public std::unary_function<data::variable, data::variable>
{
  std::map<data::variable, data::variable> sigma;

  data::variable operator()(const data::variable& v) const
  {
    std::map<data::variable, data::variable>::const_iterator i = sigma.find(v);
    if (i == sigma.end())
    {
      return v;
    }
    return i->second;
  }

  data::variable_list operator()(const data::variable_list& v) const
  {
    std::vector<data::variable> result;
    for (data::variable_list::const_iterator i = v.begin(); i != v.end(); ++i)
    {
      result.push_back((*this)(*i));
    }
    return data::variable_list(result.begin(),result.end());
  }

  std::string to_string() const
  {
    std::ostringstream out;
    out << "[";
    for (std::map<data::variable, data::variable>::const_iterator i = sigma.begin(); i != sigma.end(); ++i)
    {
      if (i != sigma.begin())
      {
        out << ", ";
      }
      out << data::pp(i->first) << " := " << data::pp(i->second);
    }
    out << "]";
    return out.str();
  }
};

struct variable_data_expression_substitution: public std::unary_function<data::variable, data::data_expression>
{
  typedef data::variable variable_type;
  typedef data::data_expression expression_type;

  const variable_variable_substitution& sigma;

  variable_data_expression_substitution(const variable_variable_substitution& sigma_)
    : sigma(sigma_)
  {}

  data::data_expression operator()(const data::variable& v) const
  {
    return sigma(v);
  }
};

/// \brief Represents the implication g => ( X0(e0) \/ ... \/ Xk(ek) )
struct pfnf_traverser_implication
{
  pbes_expression g;
  std::vector<propositional_variable_instantiation> rhs;

  pfnf_traverser_implication(const atermpp::aterm_appl& g_, const std::vector<propositional_variable_instantiation>& rhs_)
    : g(g_),
      rhs(rhs_)
  {}

  pfnf_traverser_implication(const atermpp::aterm_appl& x)
    : g(x)
  {}

  // applies a substitution to variables
  void substitute(const variable_variable_substitution& sigma)
  {
    for (std::vector<propositional_variable_instantiation>::iterator i = rhs.begin(); i != rhs.end(); ++i)
    {
      *i = pbes_system::replace_free_variables(*i, variable_data_expression_substitution(sigma));
    }
    g = pbes_system::replace_free_variables(g, variable_data_expression_substitution(sigma));
  }

};

struct pfnf_traverser_expression
{
	pbes_expression expr;
  std::vector<pfnf_traverser_quantifier> quantifiers;
  std::vector<pfnf_traverser_implication> implications;

  pfnf_traverser_expression(const atermpp::aterm_appl& x, const std::vector<pfnf_traverser_quantifier>& quantifiers_, const std::vector<pfnf_traverser_implication>& implications_)
    : expr(x),
      quantifiers(quantifiers_),
      implications(implications_)
  {}

  pfnf_traverser_expression(const atermpp::aterm_appl& x)
    : expr(x)
  {}

  // applies a substitution to variables
  void substitute(const variable_variable_substitution& sigma)
  {
    for (std::vector<pfnf_traverser_quantifier>::iterator i = quantifiers.begin(); i != quantifiers.end(); ++i)
    {
      i->second = sigma(i->second);
    }
    for (std::vector<pfnf_traverser_implication>::iterator i = implications.begin(); i != implications.end(); ++i)
    {
      i->substitute(sigma);
    }
    expr = pbes_system::replace_free_variables(expr, variable_data_expression_substitution(sigma));
  }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

/// \brief Concatenates two containers
/// \param x A container
/// \param y A container
/// \return The concatenation of x and y
template <typename Container>
Container concat(const Container& x, const Container& y)
{
  Container result = x;
  result.insert(result.end(), y.begin(), y.end());
  return result;
}

/// \brief Applies the PFNF rewriter to a PBES expression.
struct pfnf_traverser: public pbes_expression_traverser<pfnf_traverser>
{
  typedef pbes_expression_traverser<pfnf_traverser> super;
  using super::enter;
  using super::leave;
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  // makes sure there are no name clashes between quantifier variables in left and right
  // TODO: the efficiency can be increased by maintaining some additional data structures
  void resolve_name_clashes(pfnf_traverser_expression& left, pfnf_traverser_expression& right)
  {
    std::set<data::variable> left_variables;
    std::set<data::variable> right_variables;
    std::set<data::variable> name_clashes;
    for (std::vector<pfnf_traverser_quantifier>::const_iterator i = left.quantifiers.begin(); i != left.quantifiers.end(); ++i)
    {
      left_variables.insert(i->second.begin(), i->second.end());
    }
    for (std::vector<pfnf_traverser_quantifier>::const_iterator j = right.quantifiers.begin(); j != right.quantifiers.end(); ++j)
    {
      const data::variable_list& rv = j->second;
      for (data::variable_list::const_iterator k = rv.begin(); k != rv.end(); ++k)
      {
        right_variables.insert(*k);
        if (left_variables.find(*k) != left_variables.end())
        {
          name_clashes.insert(*k);
        }
      }
    }

#ifdef MCRL2_PFNF_VISITOR_DEBUG
std::cout << "NAME CLASHES: " << core::detail::print_set(name_clashes) << std::endl;
#endif

    if (!name_clashes.empty())
    {
      data::set_identifier_generator generator;
      for (std::set<data::variable>::const_iterator i = left_variables.begin(); i != left_variables.end(); ++i)
      {
        generator.add_identifier(i->name());
      }
      for (std::set<data::variable>::const_iterator i = right_variables.begin(); i != right_variables.end(); ++i)
      {
        generator.add_identifier(i->name());
      }
      variable_variable_substitution sigma;
      for (std::set<data::variable>::iterator i = name_clashes.begin(); i != name_clashes.end(); ++i)
      {
        sigma.sigma[*i] = data::variable(generator(std::string(i->name())), i->sort());
      }
#ifdef MCRL2_PFNF_VISITOR_DEBUG
std::cout << "LEFT\n"; print_expression(left);
std::cout << "RIGHT BEFORE\n"; print_expression(right);
std::cout << "SIGMA = " << sigma.to_string() << std::endl;
#endif
      right.substitute(sigma);
#ifdef MCRL2_PFNF_VISITOR_DEBUG
std::cout << "RIGHT AFTER\n"; print_expression(right);
#endif
    }
  }

  pbes_expression make_and(const pfnf_traverser_expression& left, const pfnf_traverser_expression& right) const
  {
    return utilities::optimized_and(left.expr, right.expr);
  }

  pbes_expression make_or(const pfnf_traverser_expression& left, const pfnf_traverser_expression& right) const
  {
    return utilities::optimized_or(left.expr, right.expr);
  }

  pbes_expression make_not(const pfnf_traverser_expression& x) const
  {
    return utilities::optimized_not(x.expr);
  }

  /// \brief A stack containing expressions in PFNF format.
  std::vector<pfnf_traverser_expression> expression_stack;

  /// \brief A stack containing quantifier variables.
  std::vector<data::variable_list> quantifier_stack;

  /// \brief Returns the top element of the expression stack converted to a pbes expression.
  /// \return The top element of the expression stack converted to a pbes expression.
  pbes_expression evaluate() const
  {
    assert(!expression_stack.empty());
    const pfnf_traverser_expression& expr = expression_stack.back();
    const std::vector<pfnf_traverser_quantifier>& q = expr.quantifiers;
    pbes_expression h = expr.expr;
    const std::vector<pfnf_traverser_implication>& g = expr.implications;
    pbes_expression result = h;
    const pbes_expression F = false_();
    for (std::vector<pfnf_traverser_implication>::const_iterator i = g.begin(); i != g.end(); ++i)
    {
      pbes_expression x = std::accumulate(i->rhs.begin(), i->rhs.end(), F, &utilities::optimized_or<pbes_expression>);
      result = utilities::optimized_and(result, utilities::optimized_imp(i->g, x));
    }
    for (std::vector<pfnf_traverser_quantifier>::const_iterator i = q.begin(); i != q.end(); ++i)
    {
      if (i->first)
      {
        result = forall(i->second, result);
      }
      else
      {
        result = exists(i->second, result);
      }
    }
    return result;
  }

  /// \brief Prints an expression
  /// \param expr An expression
  void print_expression(const pfnf_traverser_expression& expr) const
  {
    const std::vector<pfnf_traverser_quantifier>& q = expr.quantifiers;
    pbes_expression h = expr.expr;
    const std::vector<pfnf_traverser_implication>& g = expr.implications;
    for (std::vector<pfnf_traverser_quantifier>::const_iterator i = q.begin(); i != q.end(); ++i)
    {
      std::cout << (i->first ? "forall " : "exists ") << data::pp(i->second) << " ";
    }
    std::cout << (q.empty() ? "" : " . ") << pbes_system::pp(h) << "\n";
    for (std::vector<pfnf_traverser_implication>::const_iterator i = g.begin(); i != g.end(); ++i)
    {
      std::cout << " /\\ " << pbes_system::pp(i->g) << " => ";
      if (i->rhs.empty())
      {
        std::cout << "true";
      }
      else
      {
        std::cout << "( ";
        for (std::vector<propositional_variable_instantiation>::const_iterator j = i->rhs.begin(); j != i->rhs.end(); ++j)
        {
          if (j != i->rhs.begin())
          {
            std::cout << " \\/ ";
          }
          std::cout << pbes_system::pp(*j);
        }
        std::cout << " )";
        std::cout << std::endl;
      }
    }
    std::cout << std::endl;
  }

  /// \brief Prints the expression stack
  /// \param msg A string
  void print(std::string msg = "") const
  {
    std::cout << "--- " << msg << std::endl;
    for (std::vector<pfnf_traverser_expression>::const_iterator i = expression_stack.begin(); i != expression_stack.end(); ++i)
    {
      print_expression(*i);
    }
    std::cout << "value = " << pbes_system::pp(evaluate()) << std::endl;
  }

  void enter(const data::data_expression& x)
  {
    expression_stack.push_back(pfnf_traverser_expression(x));
  }

  void enter(const true_& x)
  {
    expression_stack.push_back(pfnf_traverser_expression(x));
  }

  void enter(const false_& x)
  {
    expression_stack.push_back(pfnf_traverser_expression(x));
  }

  void enter(const not_&)
  {
    throw std::runtime_error("operation not should not occur");
  }

  void leave(const and_&)
  {
    // join the two expressions on top of the stack
    pfnf_traverser_expression right = expression_stack.back();
    expression_stack.pop_back();
    pfnf_traverser_expression left  = expression_stack.back();
    expression_stack.pop_back();
    resolve_name_clashes(left, right);
    std::vector<pfnf_traverser_quantifier> q = concat(left.quantifiers, right.quantifiers);
    pbes_expression h = make_and(left, right);
    std::vector<pfnf_traverser_implication> g = concat(left.implications, right.implications);
//std::cout << "AND RESULT\n"; print_expression(pfnf_traverser_expression(h, q, g));
    expression_stack.push_back(pfnf_traverser_expression(h, q, g));
  }

  void leave(const or_&)
  {
    // join the two expressions on top of the stack
    pfnf_traverser_expression right = expression_stack.back();
    expression_stack.pop_back();
    pfnf_traverser_expression left  = expression_stack.back();
    expression_stack.pop_back();
    resolve_name_clashes(left, right);

    std::vector<pfnf_traverser_quantifier> q = concat(left.quantifiers, right.quantifiers);

    pbes_expression h_phi = left.expr;
    pbes_expression h_psi = right.expr;
    pbes_expression h = make_or(h_phi, h_psi);

    pbes_expression not_h_phi = make_not(left.expr);
    pbes_expression not_h_psi = make_not(right.expr);

    const std::vector<pfnf_traverser_implication>& q_phi = left.implications;
    const std::vector<pfnf_traverser_implication>& q_psi = right.implications;

    std::vector<pfnf_traverser_implication> g;

    // first conjunction
    for (std::vector<pfnf_traverser_implication>::const_iterator i = q_phi.begin(); i != q_phi.end(); ++i)
    {
      g.push_back(pfnf_traverser_implication(make_and(not_h_psi, i->g), i->rhs));
    }

    // second conjunction
    for (std::vector<pfnf_traverser_implication>::const_iterator i = q_psi.begin(); i != q_psi.end(); ++i)
    {
      g.push_back(pfnf_traverser_implication(make_and(not_h_phi, i->g), i->rhs));
    }

    // third conjunction
    for (std::vector<pfnf_traverser_implication>::const_iterator i = q_phi.begin(); i != q_phi.end(); ++i)
    {
      for (std::vector<pfnf_traverser_implication>::const_iterator k = q_psi.begin(); k != q_psi.end(); ++k)
      {
        g.push_back(pfnf_traverser_implication(make_and(i->g, k->g), concat(i->rhs, k->rhs)));
      }
    }
//std::cout << "OR RESULT\n"; print_expression(pfnf_traverser_expression(h, q, g));
    expression_stack.push_back(pfnf_traverser_expression(h, q, g));
  }

  void enter(const imp& /*x*/)
  {
    throw std::runtime_error("operation imp should not occur");
  }

  void enter(const forall& x)
  {
    quantifier_stack.push_back(x.variables());
  }

  void leave(const forall&)
  {
    // push the quantifier on the expression stack
    expression_stack.back().quantifiers.push_back(std::make_pair(true, quantifier_stack.back()));
    quantifier_stack.pop_back();
  }

  void enter(const exists& x)
  {
    quantifier_stack.push_back(x.variables());
  }

  void leave(const exists&)
  {
    // push the quantifier on the expression stack
    expression_stack.back().quantifiers.push_back(std::make_pair(false, quantifier_stack.back()));
    quantifier_stack.pop_back();
  }

  void enter(const propositional_variable_instantiation& x)
  {
    // push the propositional variable on the expression stack
    std::vector<pfnf_traverser_quantifier> q;
    pbes_expression h = true_();
    std::vector<pfnf_traverser_implication> g(1, pfnf_traverser_implication(true_(), std::vector<propositional_variable_instantiation>(1, x)));
    expression_stack.push_back(pfnf_traverser_expression(h, q, g));
  }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PFNF_TRAVERSER_H
