// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/abstract.h
/// \brief The PBES abstract algorithm.

#ifndef MCRL2_PBES_ABSTRACT_H
#define MCRL2_PBES_ABSTRACT_H

#include <iostream>
#include "mcrl2/core/algorithm.h"
#include "mcrl2/data/find.h"
#include "mcrl2/pbes/pbes_expression_builder.h"
#include "mcrl2/pbes/pbes.h"

// TODO: isolate pbes2bes_variable_map from pbes2bes_finite
#include "mcrl2/pbes/pbes2bes_finite_algorithm.h" // for pbes2bes_variable_map

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  /// Visitor that ...
  struct pbes_abstract_builder: public pbes_expression_builder<pbes_expression>
  {
    std::vector<data::variable_list> m_quantifier_stack;
    const std::vector<data::variable> m_selected_variables;
  
    pbes_abstract_builder(const std::vector<data::variable>& selected_variables)
    : m_selected_variables(selected_variables)
    {}
  
    /// \brief Returns true if the m_quantifier_stack contains a given data variable
    bool is_bound(const data::variable& v) const
    {
      for (std::vector<data::variable_list>::const_iterator i = m_quantifier_stack.begin(); i != m_quantifier_stack.end(); ++i)
      {
        for (data::variable_list::iterator j = i->begin(); j != i->end(); ++j)
        {
          if (*j == v)
          {
            return true;
          }
        }
      }
      return false;
    }
  
    /// \brief Adds variables to the quantifier stack, and adds replacements for the name clashes to replacements.
    /// \param variables A sequence of data variables
    /// \return The number of replacements that were added.
    void push_variables(const data::variable_list& variables)
    {
      m_quantifier_stack.push_back(variables);
    }
  
    /// \brief Removes the last added variable list from the quantifier stack, and removes
    /// replacement_count replacements.
    /// \param replacement_count A positive integer
    void pop_variables()
    {
      m_quantifier_stack.pop_back();
    }
  
    /// \brief Visit data_expression node
    /// \param e A PBES expression
    /// \param d A data expression
    /// \return The result of visiting the node
    pbes_expression visit_data_expression(const pbes_expression& e, const data::data_expression& d)
    {
      std::set<data::variable> FV = data::find_free_variables(d);
      for (std::set<data::variable>::iterator i = FV.begin(); i != FV.end(); ++i)
      {
        if (!is_bound(*i))
        {
          std::clog << "Reducing data expression " << pp(e) << " to false." << std::endl;
          return pbes_expr::false_();
        }
      }
      return e;
    }
  
    /// \brief Visit forall node
    /// \param e A PBES expression
    /// \param variables A sequence of data variables
    /// \param expression A PBES expression
    /// \return The result of visiting the node
    pbes_expression visit_forall(const pbes_expression& /* e */, const data::variable_list& variables, const pbes_expression& expression)
    {
      push_variables(variables);
      pbes_expression new_expression = visit(expression);
      pop_variables();
      return pbes_expr::forall(variables, new_expression);
    }
  
    /// \brief Visit exists node
    /// \param e A PBES expression
    /// \param variables A sequence of data variables
    /// \param expression A PBES expression
    /// \return The result of visiting the node
    pbes_expression visit_exists(const pbes_expression& /* e */, const data::variable_list& variables, const pbes_expression& expression)
    {
      push_variables(variables);
      pbes_expression new_expression = visit(expression);
      pop_variables();
      return pbes_expr::exists(variables, new_expression);
    }
  };

} // namespace detail


  /// \brief Algorithm class for the abstract algorithm
  class pbes_abstract_algorithm: public core::algorithm
  {
    public:
      /// \brief Constructor.
      /// \param datar A data rewriter
      /// \param pbesr A PBES rewriter
      pbes_abstract_algorithm(unsigned int log_level = 0)
        : core::algorithm(log_level)
      {}

      /// \brief Runs the algorithm.
      /// \param p A PBES
      /// \param variable_map A map containing the parameters that should be expanded by the algorithm.
      void run(pbes<>& p,
               const pbes2bes_variable_map& variable_map
              )
      {
        for (atermpp::vector<pbes_equation>::iterator i = p.equations().begin(); i != p.equations().end(); ++i)
        {
          pbes2bes_variable_map::const_iterator j = variable_map.find(i->variable().name());
          if (j != variable_map.end())
          {
            detail::pbes_abstract_builder builder(j->second);
            i->formula() = builder.visit(i->formula());
          }
        }
      }
  };

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ABSTRACT_H
