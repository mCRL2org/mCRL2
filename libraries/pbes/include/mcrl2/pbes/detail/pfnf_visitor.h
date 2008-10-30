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

#include <numeric>
#include <utility>
#include <vector>
#include <stdexcept>
#include <boost/tuple/tuple.hpp>
#include "mcrl2/core/optimized_boolean_operators.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/pbes_expression_visitor.h"
#include "mcrl2/pbes/rewriter.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  template <typename Container>
  Container concat(const Container& x, const Container& y)
  {
    Container result = x;
    result.insert(result.end(), y.begin(), y.end());
    return result;
  }

  template <typename Term>
  struct pfnf_visitor: public pbes_expression_visitor<Term>
  {
    typedef pbes_expression_visitor<Term> super;
    typedef Term term_type;
    typedef core::term_traits<Term> tr;
    typedef typename tr::variable_type variable_type;
    typedef typename tr::variable_sequence_type variable_sequence_type;
    typedef typename tr::data_term_type data_term_type;
    typedef typename tr::propositional_variable_type propositional_variable_type;

    /// Represents a quantifier Qv:V. If the bool is true it is a forall, otherwise an exists.
    typedef std::pair<bool, variable_sequence_type> quantifier;

    /// Represents the implication g => ( X0(e0) /\ ... /\ Xk(ek) )
    typedef std::pair<term_type, std::vector<propositional_variable_type> > implication;

    /// Represents an expression in PFNF format.
    typedef boost::tuple<std::vector<quantifier>, pbes_expression, std::vector<implication> > expression;

    /// A stack containing expressions in PFNF format.
    std::vector<expression> expression_stack;

    /// A stack containing quantifier variables.
    std::vector<data::data_variable_list> quantifier_stack;

    /// Converts the top element of the expression stack to a pbes expression.
    pbes_expression evaluate() const
    {
      assert(!expression_stack.empty());
      const expression& expr = expression_stack.back();
      std::vector<quantifier> q = boost::get<0>(expr);
      pbes_expression h = boost::get<1>(expr);
      std::vector<implication> g = boost::get<2>(expr);
      pbes_expression result = h;
      for (std::vector<implication>::iterator i = g.begin(); i != g.end(); ++i)
      {
        pbes_expression x = std::accumulate(i->second.begin(), i->second.end(), tr::false_(), &core::optimized_or<Term>);
        result = core::optimized_and(result, core::optimized_imp(i->first, x));
      }
      for (std::vector<quantifier>::iterator i = q.begin(); i != q.end(); ++i)
      {
        result = i->first ? tr::forall(i->second, result) : tr::exists(i->second, result);
      }
      return result;
    }

    void print_expression(const expression& expr) const
    {
      std::vector<quantifier> q = boost::get<0>(expr);
      pbes_expression h = boost::get<1>(expr);
      std::vector<implication> g = boost::get<2>(expr);
      for (std::vector<quantifier>::iterator i = q.begin(); i != q.end(); ++i)
      {
        std::cout << (i->first ? "forall " : "exists ") << core::pp(i->second) << " ";
      }
      std::cout << (q.empty() ? "" : " . ") << core::pp(h) << "\n";
      for (std::vector<implication>::iterator i = g.begin(); i != g.end(); ++i)
      {
        std::cout << " /\\ " << core::pp(i->first) << " => ";
        if (i->second.empty())
        {
          std::cout << "true";
        }
        else
        {
          std::cout << "( ";
          for (std::vector<propositional_variable_type>::iterator j = i->second.begin(); j != i->second.end(); ++j)
          {
            if (j != i->second.begin())
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
    
    void print(std::string msg = "") const
    {
      std::cout << "--- " << msg << std::endl;
      for (std::vector<expression>::const_iterator i = expression_stack.begin(); i != expression_stack.end(); ++i)
      {
        print_expression(*i);
      }
      std::cout << "value = " << core::pp(evaluate()) << std::endl;
    }

    /// Visit data expression node.
    ///
    bool visit_data_expression(const term_type& e, const data_term_type& /* d */)
    {
      expression_stack.push_back(boost::make_tuple(std::vector<quantifier>(), e, std::vector<implication>()));
      return continue_recursion;
    }

    /// Visit true node.
    ///
    bool visit_true(const term_type& e)
    {
      expression_stack.push_back(boost::make_tuple(std::vector<quantifier>(), e, std::vector<implication>()));
print("true");
      return continue_recursion;
    }

    /// Visit false node.
    ///
    bool visit_false(const term_type& e)
    {
      expression_stack.push_back(boost::make_tuple(std::vector<quantifier>(), e, std::vector<implication>()));
print("false");
      return continue_recursion;
    }

    /// Visit not node.
    ///
    bool visit_not(const term_type& e, const term_type& /* arg */)
    {
      throw std::runtime_error("operation not should not occur");
      return continue_recursion;
    }

    /// Leave and node.
    ///
    void leave_and()
    {
      // join the two expressions on top of the stack
      expression right = expression_stack.back();
      expression_stack.pop_back();
      expression left  = expression_stack.back();
      expression_stack.pop_back();
      std::vector<quantifier> q = concat(boost::get<0>(left), boost::get<0>(right));
      pbes_expression h = core::optimized_and(boost::get<1>(left), boost::get<1>(right));
      std::vector<implication> g = concat(boost::get<2>(left), boost::get<2>(right));
      expression_stack.push_back(boost::make_tuple(q, h, g));
print("and");
    }

    /// Leave or node.
    ///
    void leave_or()
    {
      // join the two expressions on top of the stack
      expression right = expression_stack.back();
      expression_stack.pop_back();
      expression left  = expression_stack.back();
      expression_stack.pop_back();

      std::vector<quantifier> q = concat(boost::get<0>(left), boost::get<0>(right));
        
      pbes_expression h_phi = boost::get<1>(left);
      pbes_expression h_psi = boost::get<1>(right);

      pbes_expression h = core::optimized_or(h_phi, h_psi);

      pbes_expression not_h_phi = core::optimized_not(boost::get<1>(left));
      pbes_expression not_h_psi = core::optimized_not(boost::get<1>(right));

      const std::vector<implication>& q_phi = boost::get<2>(left);
      const std::vector<implication>& q_psi = boost::get<2>(right);

      std::vector<implication> g;

      // first conjunction
      for (std::vector<implication>::const_iterator i = q_phi.begin(); i != q_phi.end(); ++i)
      {
        g.push_back(implication(core::optimized_and(not_h_psi, i->first), i->second));
      }

      // second conjunction
      for (std::vector<implication>::const_iterator i = q_psi.begin(); i != q_psi.end(); ++i)
      {
        g.push_back(implication(core::optimized_and(not_h_phi, i->first), i->second));
      }

      // third conjunction
      for (std::vector<implication>::const_iterator i = q_phi.begin(); i != q_phi.end(); ++i)
      {
        for (std::vector<implication>::const_iterator k = q_psi.begin(); k != q_psi.end(); ++k)
        {
          g.push_back(implication(core::optimized_and(i->first, k->first), concat(i->second, k->second)));
        }
      }

      expression_stack.push_back(boost::make_tuple(q, h, g));
    }

    /// Visit imp node.
    ///
    bool visit_imp(const term_type& e, const term_type& /* left */, const term_type& /* right */)
    {
      throw std::runtime_error("operation imp should not occur");
      return continue_recursion;
    }

    /// Visit forall node.
    ///
    bool visit_forall(const term_type& e, const variable_sequence_type& variables, const term_type& /* expression */)
    {
      quantifier_stack.push_back(variables);
      return continue_recursion;
    }

    /// Leave forall node.
    ///
    void leave_forall()
    {
      // push the quantifier on the expression stack
      boost::get<0>(expression_stack.back()).push_back(std::make_pair(true, quantifier_stack.back()));
      quantifier_stack.pop_back();
print("forall");
    }

    /// Visit exists node.
    ///
    bool visit_exists(const term_type& e, const variable_sequence_type& variables, const term_type& /* expression */)
    {
      quantifier_stack.push_back(variables);
      return continue_recursion;
    }

    /// Leave exists node.
    ///
    void leave_exists()
    {
      // push the quantifier on the expression stack
      boost::get<0>(expression_stack.back()).push_back(std::make_pair(false, quantifier_stack.back()));
      quantifier_stack.pop_back();
print("exists");
    }

    /// Visit propositional variable node.
    ///
    bool visit_propositional_variable(const term_type& e, const propositional_variable_type& X)
    {
      // push the propositional variable on the expression stack
      std::vector<quantifier> q;
      pbes_expression h = tr::true_();
      std::vector<implication> g(1, implication(tr::true_(), std::vector<propositional_variable_type>(1, X)));
      expression_stack.push_back(boost::make_tuple(q, h, g));
print("propvar");
      return continue_recursion;
    }    
  };

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PFNF_VISITOR_H
