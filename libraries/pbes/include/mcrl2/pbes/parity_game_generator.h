// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/parity_game_generator.h
/// \brief A class for generating a parity game from a pbes.

#ifndef MCRL2_PBES_PARITY_GAME_GENERATOR_H
#define MCRL2_PBES_PARITY_GAME_GENERATOR_H

#include <iostream>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>
#include <utility>
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/map.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/selection.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/detail/bes_equation_limit.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"
#include "mcrl2/utilities/number_postfix_generator.h"

namespace mcrl2
{

namespace pbes_system
{

/// \brief Class for generating a BES from a PBES. This BES can be interpreted as
/// a graph corresponding to a parity game problem. The proposition variables
/// of the BES correspond to the vertices of the graph.
/// An interface to the graph is provided in which the vertices correspond to
/// integer values. The values are in the range [0, 1, ..., n], i.e. there are
/// no holes in the sequence.
/// Each vertex is labeled with a priority value, which is the
/// block nesting depth of the proposition variable in the BES.
class parity_game_generator
{
  protected:
    /// \brief The traits class of the expression type.
    typedef core::term_traits<pbes_expression> tr;

    /// \brief Substitution function type used by the PBES rewriter.
    typedef data::mutable_map_substitution< atermpp::map< data::variable, data::data_expression_with_variables > > substitution_function;

    /// \brief Mark whether initialization has been initialized.
    /// Needed to properly cope with virtual inheritance!
    bool m_initialized;

    /// \brief The PBES that is being solved.
    pbes<>& m_pbes;

    /// \brief Data rewriter.
    data::rewriter datar;

    /// \brief Data enumerator.
    data::data_enumerator datae;

    /// \brief Data rewriter that operates on data expressions with variables.
    data::rewriter_with_variables datarv;

    /// \brief PBES rewriter.
    pbes_system::enumerate_quantifiers_rewriter<pbes_system::pbes_expression, data::rewriter_with_variables, data::data_enumerator> R;

    /// \brief Maps propositional variables to corresponding PBES equations.
    std::map<core::identifier_string, atermpp::vector<pbes_equation>::const_iterator > m_pbes_equation_index;

    /// \brief Maps propositional variables to corresponding priorities.
    std::map<core::identifier_string, size_t> m_priorities;

    /// \brief Maps PBES closed expressions to corresponding BES variables.
    atermpp::map<pbes_expression, size_t> m_pbes_expression_index;

    /// \brief Contains intermediate results of the BES that is being generated.
    /// m_bes[i] represents a BES equation corresponding to BES variable i.
    /// m_bes[i].first is the right hand side of the BES equation
    /// m_bes[i].second is the block nesting depth of the corresponding PBES variable
    atermpp::vector<std::pair<pbes_expression, size_t> > m_bes;

    /// \brief Determines what kind of BES equations are generated for true and false.
    bool m_true_false_dependencies;

    /// \brief True if it is a min-parity game.
    bool m_is_min_parity;

    virtual
    std::string print(const pbes_expression& e)
    {
      return pbes_system::pp(e);
    }

    /// \brief Check whether e corresponds to true
    virtual
    bool is_true(const pbes_expression& e) const
    {
      return tr::is_true(e);
    }

    /// \brief Check whether e corresponds to false
    virtual
    bool is_false(const pbes_expression& e) const
    {
      return tr::is_false(e);
    }

    /// \brief Adds a BES equation for a given PBES expression, if it not already exists.
    /// \param t A PBES expression
    /// \param priority A positive integer
    /// \return The index of a BES equation corresponding to the given PBES expression.
    /// If no equation exists for the expression, a new one is added.
    virtual
    size_t add_bes_equation(pbes_expression t, size_t priority)
    {
      size_t result;

      mCRL2log(log::debug2, "parity_game_generator") << "Adding equation for " << t << std::endl;

      // TODO: can this insertion be done more efficiently?
      atermpp::map<pbes_expression, size_t>::iterator i = m_pbes_expression_index.find(t);
      if (i != m_pbes_expression_index.end())
      {
        result = i->second;
      }
      else
      {
        size_t p = m_pbes_expression_index.size();
        m_pbes_expression_index[t] = p;
        if (tr::is_prop_var(t))
        {
          priority = m_priorities[tr::name(t)];
        }
        m_bes.push_back(std::make_pair(t, priority));
        detail::check_bes_equation_limit(m_bes.size());
        mCRL2log(log::verbose) << print_equation_count(m_bes.size());
        result = p;
      }

      return result;
    }

    /// \brief Generates a substitution function for the pbesinst rewriter.
    /// \param v A sequence of data variables
    /// \param e A sequence of data expressions
    /// \return A substitution function.
    virtual
    substitution_function make_substitution(data::variable_list v, data::data_expression_list e)
    {
      assert(v.size() == e.size());
      substitution_function sigma;
      data::variable_list::iterator i = v.begin();
      data::data_expression_list::iterator j = e.begin();
      for (; i != v.end(); ++i, ++j)
      {
        sigma[*i] = *j;
      }
      return sigma;
    }

    virtual
    pbes_expression expand_rhs(const pbes_expression& psi)
    {
      // expand the right hand side if needed
      if (tr::is_prop_var(psi))
      {
        const pbes_equation& pbes_eqn = *m_pbes_equation_index[tr::name(psi)];
        substitution_function sigma = make_substitution(pbes_eqn.variable().parameters(), tr::param(psi));
        mCRL2log(log::debug2, "parity_game_generator") << "Expanding right hand side " << print(pbes_eqn.formula()) << " into " << std::flush;
        pbes_expression result(R(pbes_eqn.formula(), sigma));
        mCRL2log(log::debug2, "parity_game_generator") << print(result) << std::endl;
        return result;
      }
      return psi;
    }

    /// \brief Compute equation index map.
    virtual
    void compute_equation_index_map()
    {

      for (atermpp::vector<pbes_equation>::const_iterator i = m_pbes.equations().begin(); i != m_pbes.equations().end(); ++i)
      {
        m_pbes_equation_index[i->variable().name()] = i;
      }
    }

    /// \brief Compute priorities of PBES propositional variables.
    template <typename Container>
    void compute_priorities(const Container& equations)
    {

      fixpoint_symbol sigma = fixpoint_symbol::nu();
      size_t priority = 0;
      for (typename Container::const_iterator i = equations.begin(); i != equations.end(); ++i)
      {
        if (pbes_equation(*i).symbol() == sigma)
        {
          m_priorities[pbes_equation(*i).variable().name()] = priority;
        }
        else
        {
          sigma = pbes_equation(*i).symbol();
          m_priorities[pbes_equation(*i).variable().name()] = ++priority;
        }
      }

      // If it is a max-priority game, adjust the priorities
      if (!m_is_min_parity)
      {
        // Choose an even upperbound max_priority
        size_t max_priority = (priority % 2 == 0 ? priority : priority + 1);
        if (max_priority == 0)
        {
          max_priority = 2;
        }
        for (std::map<core::identifier_string, size_t>::iterator i = m_priorities.begin(); i != m_priorities.end(); ++i)
        {
          i->second = max_priority - i->second;
        }
        // Add BES equations for true and false with priorities 0 and 1.
        add_bes_equation(tr::true_(), max_priority);
        add_bes_equation(tr::false_(), max_priority - 1);
      }
      else
      {
        // Add BES equations for true and false with priorities 0 and 1.
        add_bes_equation(tr::true_(), 0);
        add_bes_equation(tr::false_(), 1);
      }
    }

    // prints the BES equation with left hand side 'index' and right hand side 'rhs'
    virtual
    std::string print_bes_equation(size_t index, const std::set<size_t>& rhs)
    {
      std::ostringstream out;
      const std::pair<pbes_expression, size_t>& eqn = m_bes[index];
      const size_t priority = eqn.second;
      out << (priority % 2 == 1 ? "mu Y" : "nu Y") << index << " = ";
      std::string op = (get_operation(index) == PGAME_AND ? " && " : " || ");
      for (std::set<size_t>::const_iterator i = rhs.begin(); i != rhs.end(); ++i)
      {
        out << (i == rhs.begin() ? "" : op) << "Y" << *i;
      }
      out <<  " (priority = " << priority << ")" << std::endl;
      return out.str();
    }

    /// \brief Prints a log message for every step-th equation
    virtual
    std::string print_equation_count(size_t size, size_t step = 1000) const
    {
      if (size > 0 && (size % step == 0 || (size < 1000 && size % 100 == 0)))
      {
        std::ostringstream out;
        out << "Generated " << size << " BES equations" << std::endl;
        return out.str();
      }
      return "";
    }

    virtual
    void initialize_generation()
    {
      if (m_initialized)
      {
        return;
      }
      else
      {
        // Nothing to be done for an empty PBES.
        if (m_pbes.equations().empty())
        {
          return;
        }

        // Normalize the pbes, since the parity game generator currently doesn't handle negation and implication.
        pbes_system::normalize(m_pbes);

        compute_equation_index_map();
        compute_priorities(m_pbes.equations());

        // Add a BES equation for the initial state.
        propositional_variable_instantiation phi = get_initial_state();
        add_bes_equation(phi, m_priorities[phi.name()]);

        m_initialized = true;
      }
    }

  public:
    /// \brief The operation type of the vertices.
    enum operation_type { PGAME_OR, PGAME_AND };

    /// \brief Constructor.
    /// \param p A PBES
    /// \param true_false_dependencies If true, nodes are generated for the values <tt>true</tt> and <tt>false</tt>.
    /// \param is_min_parity If true a min-parity game is produced, otherwise a max-parity game
    parity_game_generator(pbes<>& p, bool true_false_dependencies = false, bool is_min_parity = true, data::rewriter::strategy rewrite_strategy = data::jitty)
      :
      m_initialized(false),
      m_pbes(p),
      datar(p.data(), mcrl2::data::used_data_equation_selector(p.data(), pbes_system::find_function_symbols(p), p.global_variables()), rewrite_strategy),
      datae(p.data(), datar),
      datarv(datar),
      R(datarv, datae),
      m_true_false_dependencies(true_false_dependencies),
      m_is_min_parity(is_min_parity)
    {
      detail::instantiate_global_variables(p);
    }

    virtual ~parity_game_generator() {}

    /// \brief Returns the (rewritten) initial state.
    /// \return the initial state rewritten by R
    virtual propositional_variable_instantiation get_initial_state()
    {
      propositional_variable_instantiation phi = R(m_pbes.initial_state());
      return phi;
    }

    /// \brief Returns the vertex type.
    /// \param index A positive integer
    /// \return PGAME_AND if the corresponding BES equation is a conjunction,
    /// PGAME_OR if it is a disjunction.
    virtual
    operation_type get_operation(size_t index)
    {
      initialize_generation();

      assert(index < m_bes.size());
      const pbes_expression& phi = m_bes[index].first;
      return get_expression_operation(phi);
    }

    /// \brief Returns the vertex type.
    /// \param phi A PBES expression
    /// \return PGAME_AND if the expression is a conjunction,
    /// PGAME_OR if it is a disjunction.
    virtual
    operation_type get_expression_operation(const pbes_expression& phi)
    {
      if (tr::is_and(phi))
      {
        return PGAME_AND;
      }
      else if (tr::is_or(phi))
      {
        return PGAME_OR;
      }
      else if (tr::is_prop_var(phi))
      {
        return PGAME_OR;
      }
      else if (tr::is_true(phi))
      {
        return PGAME_AND;
      }
      else if (tr::is_false(phi))
      {
        return PGAME_OR;
      }
      else if (tr::is_forall(phi))
      {
        return PGAME_AND;
      }
      else if (tr::is_exists(phi))
      {
        return PGAME_OR;
      }
      else if (tr::is_data(phi))
      {
        return PGAME_OR;
      }
      throw(std::runtime_error("Error in parity_game_generator: unexpected operation " + print(phi)));
    }

    /// \brief Returns the priority of a vertex.
    /// The priority of the first equation is 0 if it is a maximal fixpoint,
    /// and 1 if it is a minimal fixpoint.
    /// \param index A positive integer
    /// \return The block nesting depth of the variable in the BES.
    virtual
    size_t get_priority(size_t index)
    {
      initialize_generation();

      assert(index < m_bes.size());
      return m_bes[index].second;
    }

    /// \brief Returns the vertices for which a solution is requested.
    /// By default a set containing the values 0, 1 and 2 is returned, corresponding
    /// to the expressions true, false and the initial state of the PBES.
    /// \return A set of indices corresponding to proposition variables of the generated BES.
    virtual
    std::set<size_t> get_initial_values()
    {
      initialize_generation();

      std::set<size_t> result;
      if (!m_pbes.equations().empty())
      {
        result.insert(0); // equation 0 corresponds with the value true
        result.insert(1); // equation 1 corresponds with the value false
        result.insert(2); // equation 2 corresponds with the initial state
      }
      return result;
    }

    /// \brief Returns the successors of a vertex in the graph.
    /// \param index A positive integer
    /// \return The indices of the proposition variables that appear in the
    /// right hand side of the BES equation of the given index.
    virtual
    std::set<size_t> get_dependencies(size_t index)
    {
      initialize_generation();

      assert(index < m_bes.size());

      std::set<size_t> result;

      std::pair<pbes_expression, size_t>& eqn = m_bes[index];
      pbes_expression& psi = eqn.first;
      const size_t priority = eqn.second;

      mCRL2log(log::debug, "parity_game_generator") << std::endl << "Generating equation for expression " << print(psi) << std::endl;

      // expand the right hand side if needed
      psi = expand_rhs(psi);

      // top_flatten
      if (tr::is_prop_var(psi))
      {
        result.insert(add_bes_equation(psi, m_priorities[tr::name(psi)]));
      }
      else if (tr::is_and(psi))
      {
        atermpp::set<pbes_expression> terms = pbes_expr::split_and(psi);
        for (atermpp::set<pbes_expression>::iterator i = terms.begin(); i != terms.end(); ++i)
        {
          result.insert(add_bes_equation(*i, priority));
        }
      }
      else if (tr::is_or(psi))
      {
        atermpp::set<pbes_expression> terms = pbes_expr::split_or(psi);
        for (atermpp::set<pbes_expression>::iterator i = terms.begin(); i != terms.end(); ++i)
        {
          result.insert(add_bes_equation(*i, priority));
        }
      }
      else if (is_true(psi))
      {
        if (m_true_false_dependencies)
        {
          atermpp::map<pbes_expression, size_t>::iterator i = m_pbes_expression_index.find(tr::true_());
          assert(i != m_pbes_expression_index.end());
          result.insert(i->second);
        }
      }
      else if (is_false(psi))
      {
        if (m_true_false_dependencies)
        {
          atermpp::map<pbes_expression, size_t>::iterator i = m_pbes_expression_index.find(tr::false_());
          assert(i != m_pbes_expression_index.end());
          result.insert(i->second);
        }
      }
      else
      {
        throw(std::runtime_error("Error in parity_game_generator: unexpected expression " + print(psi) + "\n" + psi.to_string()));
      }
      mCRL2log(log::debug, "parity_game_generator") << print_bes_equation(index, result);
      return result;
    }

    /// \brief Prints the mapping from BES variables to the corresponding PBES expressions.
    virtual
    void print_variable_mapping()
    {
      std::cerr << "--- variable mapping ---" << std::endl;
      std::map<size_t, pbes_expression> m;
      for (atermpp::map<pbes_expression, size_t>::iterator i = m_pbes_expression_index.begin(); i != m_pbes_expression_index.end(); ++i)
      {
        m[i->second] = i->first;
      }
      for (std::map<size_t, pbes_expression>::iterator i = m.begin(); i != m.end(); ++i)
      {
        std::cerr << std::setw(4) << i->first << " " << pbes_system::pp(i->second) << std::endl;
      }
      std::cerr << "--- priorities ---" << std::endl;
      for (std::map<core::identifier_string, size_t>::iterator i = m_priorities.begin(); i != m_priorities.end(); ++i)
      {
        std::cerr << core::pp(i->first) << " " << i->second << std::endl;
      }
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PARITY_GAME_GENERATOR_H
