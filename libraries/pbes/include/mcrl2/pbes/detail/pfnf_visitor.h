// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pfnf_visitor.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_PFNF_VISITOR_H
#define MCRL2_PBES_DETAIL_PFNF_VISITOR_H

#include <algorithm>
#include <iterator>
#include <numeric>
#include <utility>
#include <vector>
#include <stdexcept>
#include <set>
#include <sstream>
#include <functional>
#include "mcrl2/atermpp/convert.h"
#include "mcrl2/core/optimized_boolean_operators.h"
#include "mcrl2/core/identifier_generator.h"
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/pbes_expression_visitor.h"
#include "mcrl2/pbes/pbes_expression.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

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

  struct pfnf_visitor: public pbes_expression_visitor<pbes_expression>
  {
    typedef pfnf_visitor self;
    typedef pbes_expression_visitor<pbes_expression> super;
    typedef core::term_traits<pbes_expression> tr;

    /// \brief Represents a quantifier Qv:V. If the bool is true it is a forall, otherwise an exists.
    typedef std::pair<bool, data::variable_list> quantifier;

    struct variable_variable_substitution: public std::unary_function<data::variable, data::variable>
    {
      atermpp::map<data::variable, data::variable> sigma;
        
      data::variable operator()(const data::variable& v) const
      {
        atermpp::map<data::variable, data::variable>::const_iterator i = sigma.find(v);
        if (i == sigma.end())
        {
          return v;
        }
        return i->second;
      }

      data::variable_list operator()(const data::variable_list& v) const
      {
        atermpp::vector<data::variable> result;
        for (data::variable_list::const_iterator i = v.begin(); i != v.end(); ++i)
        {
          result.push_back((*this)(*i));
        }
        return atermpp::convert<data::variable_list>(result);
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
    // N.B. implication derives from pbes_expression to prevent garbage collection issues; of course this is very ugly
    struct implication: public pbes_expression
    {
      std::vector<propositional_variable_instantiation> rhs;
        
      implication(const atermpp::aterm_appl& x, const std::vector<propositional_variable_instantiation>& rhs_)
        : pbes_expression(x),
          rhs(rhs_)
      {}

      implication(const atermpp::aterm_appl& x)
        : pbes_expression(x)
      {}
      
      // applies a substitution to variables
      void substitute(const variable_variable_substitution& sigma)
      {
        for (std::vector<propositional_variable_instantiation>::iterator i = rhs.begin(); i != rhs.end(); ++i)
        {
          *i = data::replace_free_variables(*i, variable_data_expression_substitution(sigma));
        }
        static_cast<pbes_expression&>(*this) = data::replace_free_variables(*this, variable_data_expression_substitution(sigma));
      }
    };

    // N.B. implication derives from pbes_expression to prevent garbage collection issues; of course this is very ugly
    struct expression: public pbes_expression
    {
      std::vector<quantifier> quantifiers;
      atermpp::vector<implication> implications;
        
      expression(const atermpp::aterm_appl& x, const std::vector<quantifier>& quantifiers_, const atermpp::vector<implication>& implications_)
        : pbes_expression(x),
          quantifiers(quantifiers_),
          implications(implications_)
      {}

      expression(const atermpp::aterm_appl& x)
        : pbes_expression(x)
      {}

      // applies a substitution to variables
      void substitute(const variable_variable_substitution& sigma)
      {
        for (std::vector<quantifier>::iterator i = quantifiers.begin(); i != quantifiers.end(); ++i)
        {
          i->second = sigma(i->second);
        }
        for (atermpp::vector<implication>::iterator i = implications.begin(); i != implications.end(); ++i)
        {
          i->substitute(sigma);
        }
        static_cast<pbes_expression&>(*this) = data::replace_free_variables(*this, variable_data_expression_substitution(sigma));
      }
    };

    // makes sure there are no name clashes between quantifier variables in left and right
    // TODO: the efficiency can be increased by maintaining some additional data structures
    void resolve_name_clashes(expression& left, expression& right)
    {
      std::set<data::variable> left_variables;
      std::set<data::variable> name_clashes;
      for (std::vector<quantifier>::const_iterator i = left.quantifiers.begin(); i != left.quantifiers.end(); ++i)
      {
        left_variables.insert(i->second.begin(), i->second.end());
      }
      for (std::vector<quantifier>::const_iterator j = right.quantifiers.begin(); j != right.quantifiers.end(); ++j)
      {
        std::set_intersection(left_variables.begin(), left_variables.end(), j->second.begin(), j->second.end(), std::inserter(name_clashes, name_clashes.end()));
      }
      if (!name_clashes.empty())
      {
        core::number_postfix_generator generator;
        for (std::set<data::variable>::const_iterator i = left_variables.begin(); i != left_variables.end(); ++i)
        {
          generator.add_to_context(std::string(i->name()));
        }
        variable_variable_substitution sigma;
        for (std::set<data::variable>::iterator i = name_clashes.begin(); i != name_clashes.end(); ++i)
        {
          sigma.sigma[*i] = data::variable(core::identifier_string(generator(std::string(i->name()))), i->sort());
        }
//std::cout << "LEFT\n"; print_expression(left);
//std::cout << "RIGHT\n"; print_expression(right);
        right.substitute(sigma);
//std::cout << "RIGHT\n"; print_expression(right);
      }
    }

    pbes_expression and_(const expression& left, const expression& right) const
    {
      return core::optimized_and(static_cast<const pbes_expression&>(left), static_cast<const pbes_expression&>(right));
    }

    pbes_expression or_(const expression& left, const expression& right) const
    {
      return core::optimized_or(static_cast<const pbes_expression&>(left), static_cast<const pbes_expression&>(right));
    }

    pbes_expression not_(const expression& x) const
    {
      return core::optimized_not(static_cast<const pbes_expression&>(x));
    }

    /// \brief A stack containing expressions in PFNF format.
    atermpp::vector<expression> expression_stack;

    /// \brief A stack containing quantifier variables.
    std::vector<data::variable_list> quantifier_stack;

    /// \brief Returns the top element of the expression stack converted to a pbes expression.
    /// \return The top element of the expression stack converted to a pbes expression.
    pbes_expression evaluate() const
    {
      assert(!expression_stack.empty());
      const expression& expr = expression_stack.back();
      const std::vector<quantifier>& q = expr.quantifiers;
      pbes_expression h = expr;
      const atermpp::vector<implication>& g = expr.implications;
      pbes_expression result = h;
      for (atermpp::vector<implication>::const_iterator i = g.begin(); i != g.end(); ++i)
      {
        pbes_expression x = std::accumulate(i->rhs.begin(), i->rhs.end(), tr::false_(), &core::optimized_or<pbes_expression>);
        pbes_expression y = *i;
        result = core::optimized_and(result, core::optimized_imp(y, x));
      }
      for (std::vector<quantifier>::const_iterator i = q.begin(); i != q.end(); ++i)
      {
        result = i->first ? tr::forall(i->second, result) : tr::exists(i->second, result);
      }
      return result;
    }

    /// \brief Prints an expression
    /// \param expr An expression
    void print_expression(const expression& expr) const
    {
      const std::vector<quantifier>& q = expr.quantifiers;
      pbes_expression h = expr;
      const atermpp::vector<implication>& g = expr.implications;
      for (std::vector<quantifier>::const_iterator i = q.begin(); i != q.end(); ++i)
      {
        std::cout << (i->first ? "forall " : "exists ") << core::pp(i->second) << " ";
      }
      std::cout << (q.empty() ? "" : " . ") << core::pp(h) << "\n";
      for (atermpp::vector<implication>::const_iterator i = g.begin(); i != g.end(); ++i)
      {
        std::cout << " /\\ " << core::pp(*i) << " => ";
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
            std::cout << core::pp(*j);
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
      for (atermpp::vector<expression>::const_iterator i = expression_stack.begin(); i != expression_stack.end(); ++i)
      {
        print_expression(*i);
      }
      std::cout << "value = " << core::pp(evaluate()) << std::endl;
    }

    /// \brief Visit data_expression node
    /// \param e A term
    /// \return The result of visiting the node
    bool visit_data_expression(const pbes_expression& e, const data::data_expression& /* d */)
    {
      expression_stack.push_back(expression(e));
      return super::continue_recursion;
    }

    /// \brief Visit true node
    /// \param e A term
    /// \return The result of visiting the node
    bool visit_true(const pbes_expression& e)
    {
      expression_stack.push_back(expression(e));
      return super::continue_recursion;
    }

    /// \brief Visit false node
    /// \param e A term
    /// \return The result of visiting the node
    bool visit_false(const pbes_expression& e)
    {
      expression_stack.push_back(expression(e));
      return super::continue_recursion;
    }

    /// \brief Visit not node
    /// \param e A term
    /// \return The result of visiting the node
    bool visit_not(const pbes_expression& /* e */, const pbes_expression& /* arg */)
    {
      throw std::runtime_error("operation not should not occur");
      return super::continue_recursion;
    }

    /// \brief Leave and node
    void leave_and()
    {
      // join the two expressions on top of the stack
      expression right = expression_stack.back();
      expression_stack.pop_back();
      expression left  = expression_stack.back();
      expression_stack.pop_back();
      resolve_name_clashes(left, right);
      std::vector<quantifier> q = concat(left.quantifiers, right.quantifiers);
      pbes_expression h = and_(left, right);
      atermpp::vector<implication> g = concat(left.implications, right.implications);
//std::cout << "AND RESULT\n"; print_expression(expression(h, q, g));
      expression_stack.push_back(expression(h, q, g));
    }

    /// \brief Leave or node
    void leave_or()
    {
      // join the two expressions on top of the stack
      expression right = expression_stack.back();
      expression_stack.pop_back();
      expression left  = expression_stack.back();
      expression_stack.pop_back();
      resolve_name_clashes(left, right);

      std::vector<quantifier> q = concat(left.quantifiers, right.quantifiers);

      pbes_expression h_phi = left;
      pbes_expression h_psi = right;
      pbes_expression h = or_(h_phi, h_psi);

      pbes_expression not_h_phi = not_(left);
      pbes_expression not_h_psi = not_(right);

      const atermpp::vector<implication>& q_phi = left.implications;
      const atermpp::vector<implication>& q_psi = right.implications;

      atermpp::vector<implication> g;

      // first conjunction
      for (atermpp::vector<implication>::const_iterator i = q_phi.begin(); i != q_phi.end(); ++i)
      {
        g.push_back(implication(and_(not_h_psi, *i), i->rhs));
      }

      // second conjunction
      for (atermpp::vector<implication>::const_iterator i = q_psi.begin(); i != q_psi.end(); ++i)
      {
        g.push_back(implication(and_(not_h_phi, *i), i->rhs));
      }

      // third conjunction
      for (atermpp::vector<implication>::const_iterator i = q_phi.begin(); i != q_phi.end(); ++i)
      {
        for (atermpp::vector<implication>::const_iterator k = q_psi.begin(); k != q_psi.end(); ++k)
        {
          g.push_back(implication(and_(*i, *k), concat(i->rhs, k->rhs)));
        }
      }
//std::cout << "OR RESULT\n"; print_expression(expression(h, q, g));
      expression_stack.push_back(expression(h, q, g));
    }

    /// \brief Visit imp node
    /// \param e A term
    /// \return The result of visiting the node
    bool visit_imp(const pbes_expression& /* e */, const pbes_expression& /* left */, const pbes_expression& /* right */)
    {
      throw std::runtime_error("operation imp should not occur");
      return super::continue_recursion;
    }

    /// \brief Visit forall node
    /// \param e A term
    /// \param variables A sequence of variables
    /// \return The result of visiting the node
    bool visit_forall(const pbes_expression& /* e */, const data::variable_list& variables, const pbes_expression& /* expression */)
    {
      quantifier_stack.push_back(variables);
      return super::continue_recursion;
    }

    /// \brief Leave forall node
    void leave_forall()
    {
      // push the quantifier on the expression stack
      expression_stack.back().quantifiers.push_back(std::make_pair(true, quantifier_stack.back()));
      quantifier_stack.pop_back();
    }

    /// \brief Visit exists node
    /// \param e A term
    /// \param variables A sequence of variables
    /// \return The result of visiting the node
    bool visit_exists(const pbes_expression& /* e */, const data::variable_list& variables, const pbes_expression& /* expression */)
    {
      quantifier_stack.push_back(variables);
      return super::continue_recursion;
    }

    /// \brief Leave exists node
    void leave_exists()
    {
      // push the quantifier on the expression stack
      expression_stack.back().quantifiers.push_back(std::make_pair(false, quantifier_stack.back()));
      quantifier_stack.pop_back();
    }

    /// \brief Visit propositional_variable node
    /// \param e A term
    /// \param X A propositional variable
    /// \return The result of visiting the node
    bool visit_propositional_variable(const pbes_expression& /* e */, const propositional_variable_instantiation& X)
    {
      // push the propositional variable on the expression stack
      std::vector<quantifier> q;
      pbes_expression h = tr::true_();
      atermpp::vector<implication> g(1, implication(tr::true_(), std::vector<propositional_variable_instantiation>(1, X)));
      expression_stack.push_back(expression(h, q, g));
      return super::continue_recursion;
    }
  };

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PFNF_VISITOR_H
