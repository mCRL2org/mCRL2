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

#include <iomanip>
#include <map>
#include <set>
#include <utility>
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/map.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/identifier_generator.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/rewriter.h"

namespace atermpp {
  /// \cond INTERNAL_DOCS
  template<>
  struct aterm_traits<std::pair<mcrl2::pbes_system::pbes_expression, unsigned int> >
  {
    typedef ATermAppl aterm_type;
    static void protect(std::pair<mcrl2::pbes_system::pbes_expression, unsigned int> t)   { t.first.protect(); }
    static void unprotect(std::pair<mcrl2::pbes_system::pbes_expression, unsigned int> t) { t.first.unprotect(); }
    static void mark(std::pair<mcrl2::pbes_system::pbes_expression, unsigned int> t)      { t.first.mark(); }
    // static ATerm term(std::pair<mcrl2::pbes_system::pbes_expression, unsigned int> t)     { return t.first.term(); }
    // static ATerm* ptr(std::pair<mcrl2::pbes_system::pbes_expression, unsigned int>& t)    { return &t.first.term(); }
  };
} // namespace atermpp

namespace mcrl2 {

namespace pbes_system {

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
      typedef data::rewriter_map<std::map<data::data_variable, data::data_expression_with_variables> > substitution_function;

      /// \brief The PBES that is being solved.
      const pbes<>& m_pbes;

      /// \brief Identifier generator for the enumerator. (TODO: this needs to be improved!)
      data::number_postfix_generator generator;

      /// \brief Data rewriter.
      data::rewriter datar;

      /// \brief Data enumerator.
      data::data_enumerator<> datae;

      /// \brief Data rewriter that operates on data expressions with variables.
      data::rewriter_with_variables datarv;

      /// \brief PBES rewriter.
      pbes_system::enumerate_quantifiers_rewriter<pbes_system::pbes_expression, data::rewriter_with_variables, data::data_enumerator<> > R;

      /// \brief Maps propositional variables to corresponding PBES equations.
      std::map<core::identifier_string, atermpp::vector<pbes_equation>::const_iterator > m_pbes_equation_index;

      /// \brief Maps propositional variables to corresponding priorities.
      std::map<core::identifier_string, unsigned int> m_priorities;

      /// \brief Maps PBES closed expressions to corresponding BES variables.
      atermpp::map<pbes_expression, unsigned int> m_pbes_expression_index;

      /// \brief Contains intermediate results of the BES that is being generated.
      /// m_bes[i] represents a BES equation corresponding to BES variable i.
      /// m_bes[i].first is the right hand side of the BES equation
      /// m_bes[i].second is the block nesting depth of the corresponding PBES variable
      atermpp::vector<std::pair<pbes_expression, unsigned int> > m_bes;

      /// \brief Determines what kind of BES equations are generated for true and false.
      bool m_true_false_dependencies;

      /// \brief True if it is a min-parity game.
      bool m_is_min_parity;

      /// \brief Adds a BES equation for a given PBES expression, if it not already exists.
      /// \param t A PBES expression
      /// \param priority A positive integer
      /// \return The index of a BES equation corresponding to the given PBES expression.
      /// If no equation exists for the expression, a new one is added.
      unsigned int add_bes_equation(pbes_expression t, unsigned int priority)
      {
        // TODO: can this insertion be done more efficiently?
        atermpp::map<pbes_expression, unsigned int>::iterator i = m_pbes_expression_index.find(t);
        if (i != m_pbes_expression_index.end())
        {
          return i->second;
        }
        else
        {
          unsigned int p = m_pbes_expression_index.size();
          m_pbes_expression_index[t] = p;
          m_bes.push_back(std::make_pair(t, priority));
          return p;
        }
      }

      /// \brief Generates a substitution function for the pbes2bes rewriter.
      /// \param v A sequence of data variables
      /// \param e A sequence of data expressions
      /// \return A sugstitution function.
      substitution_function make_substitution(data::data_variable_list v, data::data_expression_list e)
      {
        assert(v.size() == e.size());
        substitution_function sigma;
        data::data_variable_list::iterator i = v.begin();
        data::data_expression_list::iterator j = e.begin();
        for (; i != v.end(); ++i, ++j)
        {
          sigma[*i] = *j;
        }
        return sigma;
      }

    public:
      /// \brief The operation type of the vertices.
      enum operation_type { PGAME_OR, PGAME_AND };

      /// \brief Constructor.
      /// \param p A PBES
      /// \param true_false_dependencies If true, nodes are generated for the values <tt>true</tt> and <tt>false</tt>.
      /// \param is_min_parity If true a min-parity game is produced, otherwise a max-parity game
      parity_game_generator(const pbes<>& p, bool true_false_dependencies = false, bool is_min_parity = true)
        : m_pbes(p),
          generator("UNIQUE_PREFIX"),
          datar(p.data()),
          datae(p.data(), datar, generator),
          datarv(p.data()),
          R(datarv, datae),
          m_true_false_dependencies(true_false_dependencies),
          m_is_min_parity(is_min_parity)
      {
        // Nothing to be done for an empty PBES.
        if (m_pbes.equations().empty())
        {
          return;
        }

        // Compute equation index map.
        for (atermpp::vector<pbes_equation>::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
        {
          m_pbes_equation_index[i->variable().name()] = i;
        }

        // Compute priorities of PBES propositional variables.
        fixpoint_symbol sigma = fixpoint_symbol::nu();
        unsigned int priority = 0;
        for (atermpp::vector<pbes_equation>::const_iterator i = m_pbes.equations().begin(); i != m_pbes.equations().end(); ++i)
        {
          if (i->symbol() == sigma)
          {
            m_priorities[i->variable().name()] = priority;
          }
          else
          {
            sigma = i->symbol();
            m_priorities[i->variable().name()] = ++priority;
          }
        }
        // If it is a max-priority game, adjust the priorities
        if (!m_is_min_parity)
        {
          // Choose an even upperbound max_priority
          unsigned int max_priority = (priority % 2 == 0 ? priority : priority + 1);
          if (max_priority == 0)
          {
            max_priority = 2;
          }
          for (std::map<core::identifier_string, unsigned int>::iterator i = m_priorities.begin(); i != m_priorities.end(); ++i)
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

        // Add a BES equation for the initial state.
        propositional_variable_instantiation phi = R(m_pbes.initial_state());
        add_bes_equation(phi, m_priorities[phi.name()]);
      }

      /// \brief Returns the vertex type.
      /// \param index A positive integer
      /// \return PGAME_AND if the corresponding BES equation is a conjunction,
      /// PGAME_OR if it is a disjunction.
      operation_type get_operation(unsigned int index) const
      {
        assert(index < m_bes.size());
        const pbes_expression& phi = m_bes[index].first;
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
        throw(std::runtime_error("Error in parity_game_generator: unexpected operation " + core::pp(phi)));
      }

      /// \brief Returns the priority of a vertex.
      /// The priority of the first equation is 0 if it is a maximal fixpoint,
      /// and 1 if it is a minimal fixpoint.
      /// \param index A positive integer
      /// \return The block nesting depth of the variable in the BES.
      unsigned int get_priority(unsigned int index) const
      {
        assert(index < m_bes.size());
        return m_bes[index].second;
      }

      /// \brief Returns the vertices for which a solution is requested.
      /// By default a set containing the values 0, 1 and 2 is returned, corresponding
      /// to the expressions true, false and the initial state of the PBES.
      /// \return A set of indices corresponding to proposition variables of the generated BES.
      std::set<unsigned int> get_initial_values()
      {
        std::set<unsigned int> result;
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
      std::set<unsigned int> get_dependencies(unsigned int index)
      {
        assert(index < m_bes.size());

        std::set<unsigned int> result;

        std::pair<pbes_expression, unsigned int>& eqn = m_bes[index];
        pbes_expression& psi = eqn.first;
        const unsigned int priority = eqn.second;

        // expand the right hand side if needed
        if (tr::is_prop_var(psi))
        {
          const pbes_equation& eqn = *m_pbes_equation_index[tr::name(psi)];
          substitution_function sigma = make_substitution(eqn.variable().parameters(), tr::param(psi));
          psi = R(eqn.formula(), sigma);
        }

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
        else if (tr::is_true(psi))
        {
          if (m_true_false_dependencies)
          {
            atermpp::map<pbes_expression, unsigned int>::iterator i = m_pbes_expression_index.find(tr::true_());
            assert(i != m_pbes_expression_index.end());
            result.insert(i->second);
          }
        }
        else if (tr::is_false(psi))
        {
          if (m_true_false_dependencies)
          {
            atermpp::map<pbes_expression, unsigned int>::iterator i = m_pbes_expression_index.find(tr::false_());
            assert(i != m_pbes_expression_index.end());
            result.insert(i->second);
          }
        }
        else
        {
          throw(std::runtime_error("Error in parity_game_generator: unexpected expression " + core::pp(psi)));
        }
        return result;
      }

      /// \brief Prints the mapping from BES variables to the corresponding PBES expressions.
      void print_variable_mapping()
      {
        std::cerr << "--- variable mapping ---" << std::endl;
        std::map<unsigned int, pbes_expression> m;
        for (atermpp::map<pbes_expression, unsigned int>::iterator i = m_pbes_expression_index.begin(); i != m_pbes_expression_index.end(); ++i)
        {
          m[i->second] = i->first;
        }
        for (std::map<unsigned int, pbes_expression>::iterator i = m.begin(); i != m.end(); ++i)
        {
          std::cerr << std::setw(4) << i->first << " " << core::pp(i->second) << std::endl;
        }
        std::cerr << "--- priorities ---" << std::endl;
        for (std::map<core::identifier_string, unsigned int>::iterator i = m_priorities.begin(); i != m_priorities.end(); ++i)
        {
          std::cerr << core::pp(i->first) << " " << i->second << std::endl;
        }
      }
  };

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PARITY_GAME_GENERATOR_H
