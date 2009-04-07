// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/eqelm.h
/// \brief The eqelm algorithm.

#ifndef MCRL2_PBES_EQELM_H
#define MCRL2_PBES_EQELM_H

// #define MCRL2_PBES_EQELM_DEBUG

#include <sstream>
#include <iostream>
#include <utility>
#include <deque>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include "mcrl2/core/messaging.h"
//#include "mcrl2/pbes/replace.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/detail/print_utility.h"

namespace mcrl2 {

namespace pbes_system {

  /// \brief Algorithm class for the eqelm algorithm
  template <typename Term, typename DataRewriter, typename PbesRewriter>
  class pbes_eqelm_algorithm
  {
    public:
      /// \brief The term type
      typedef typename core::term_traits<Term>::term_type term_type;

      /// \brief The variable type
      typedef typename core::term_traits<Term>::variable_type variable_type;

      /// \brief The variable sequence type
      typedef typename core::term_traits<Term>::variable_sequence_type variable_sequence_type;

      /// \brief The data term type
      typedef typename core::term_traits<Term>::data_term_type data_term_type;

      /// \brief The data term sequence type
      typedef typename core::term_traits<Term>::data_term_sequence_type data_term_sequence_type;

      /// \brief The string type
      typedef typename core::term_traits<Term>::string_type string_type;

      /// \brief The propositional variable declaration type
      typedef typename core::term_traits<Term>::propositional_variable_decl_type propositional_variable_decl_type;

      /// \brief The propositional variable instantiation type
      typedef typename core::term_traits<Term>::propositional_variable_type propositional_variable_type;

      /// \brief The term traits
      typedef typename core::term_traits<Term> tr;

    protected:
      /// \brief Compares data expressions for equality.
      DataRewriter m_data_rewriter;

      /// \brief Compares data expressions for equality.
      PbesRewriter m_pbes_rewriter;

      /// \brief The vertices of the grapth, i.e. the equivalence relations.
      /// It stores the equivalence sets for each propositional variable, for example
      /// X -> [ {x1, x3}, {x2, x4} ]. Equivalence sets of size 1 are not stored.
      std::map<string_type, std::vector<std::set<variable_type> > > m_vertices;

      /// \brief The edges of the graph.
      /// It is a mapping from X to iocc(X).
      std::map<string_type, atermpp::set<propositional_variable_type> > m_edges;

      /// \brief Puts all parameters of the same sort in the same equivalence set.
      std::vector<std::set<variable_type> > compute_equivalence_sets(const propositional_variable_decl_type& X) const
      {
        std::map<sort_expression, std::set<variable_type> > m;
        for (variable_sequence_type::const_iterator i = X.parameters().begin(); i != X.parameters().end(); ++i)
        {
          m[i->sort()].insert(*i);
        }
        std::vector<std::set<variable_type> > result;
        for (std::map<sort_expression, std::set<variable_type> >::iterator i = m.begin(); i != m.end(); ++i)
        {
          if (i->second.size() > 1)
          {
            result.push_back(i->second);
          }
        }
        return result;
      }

      /// \brief Prints the vertices of the dependency graph.
      void print_vertices() const
      {
        for (typename std::map<string_type, std::vector<std::set<variable_type> > >::const_iterator i = m_vertices.begin(); i != m_vertices.end(); ++i)
        {
          std::clog << core::pp(i->first) << " -> [ ";
          const std::vector<std::set<variable_type> >& v = i->second;
          for (std::vector<std::set<variable_type> >::const_iterator j = v.begin(); j != v.end(); ++j)
          {
            if (j != v.begin())
            {
              std::clog << ", ";
            }
            std::clog << detail::print_pp_set(*j);
          }
          std::clog << " ]" << std::endl;
        }
      }

      /// \brief Prints the edges of the dependency graph.
      void print_edges() const
      {
        for (typename std::map<string_type, atermpp::set<propositional_variable_type> >::const_iterator i = m_edges.begin(); i != m_edges.end(); ++i)
        {
          std::clog << core::pp(i->first) << " -> " << detail::print_pp_set(i->second) << std::endl;
        }
      }

    public:

      /// \brief Constructor.
      /// \param datar A data rewriter
      /// \param pbesr A PBES rewriter
      pbes_eqelm_algorithm(DataRewriter datar, PbesRewriter pbesr)
        : m_data_rewriter(datar), m_pbes_rewriter(pbesr)
      {}

      /// \brief Runs the eqelm algorithm
      /// \param p A pbes
      /// \param name_generator A generator for fresh identifiers
      /// \param remove_redundant_equations If true, redundant equations are removed from the PBES
      /// The call \p name_generator() should return an identifier that doesn't appear
      /// in the pbes \p p
      /// \param compute_conditions If true, propagation conditions are computed. Note
      /// that the currently implementation has exponential behavior.
      template <typename Container>
      void run(pbes<Container>& p, bool compute_conditions = false, bool remove_redundant_equations = false)
      {
        m_vertices.clear();
        m_edges.clear();

        // compute the edges and vertices of the graph
        for (typename Container::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
        {
          string_type name = i->variable().name();
          m_edges[name] = find_all_propositional_variable_instantiations(i->formula());
          m_vertices[name] = compute_equivalence_sets(i->variable());
        }

        std::clog << "--- vertices ---" << std::endl;
        print_vertices();
        std::clog << std::endl;
        std::clog << "--- edges ---" << std::endl;
        print_edges();
      }

/*
        // initialize the todo list of vertices that need to be processed
        std::set<propositional_variable_decl_type> todo;
        std::set<propositional_variable_decl_type> visited;
        std::set<propositional_variable_type> inst = find_all_propositional_variable_instantiations(p.initial_state());
        for (typename std::set<propositional_variable_type>::iterator i = inst.begin(); i != inst.end(); ++i)
        {
          data_term_sequence_type e = i->parameters();
          vertex& u = m_vertices[i->name()];
          u.update(e, constraint_map(), m_data_rewriter);
          todo.push_back(u.variable);
          visited.insert(u.variable);
        }

        if (mcrl2::core::gsDebug)
        {
          std::cerr << "\n--- initial vertices ---" << std::endl;
          print_vertices();
          std::cerr << "\n--- edges ---" << std::endl;
          print_edges();
        }

        // propagate constraints over the edges until the todo list is empty
        while (!todo.empty())
        {
#ifdef MCRL2_PBES_EQELM_DEBUG
std::cerr << "\n<todo list>" << core::pp(propositional_variable_list(todo.begin(), todo.end())) << std::endl;
#endif
          propositional_variable_decl_type var = todo.front();

          // remove all occurrences of var from todo
          todo.erase(std::remove(todo.begin(), todo.end(), var), todo.end());

          const vertex& u = m_vertices[var.name()];
          std::vector<edge>& u_edges = m_edges[var.name()];
          variable_sequence_type Xparams = u.variable.parameters();

          for (typename std::vector<edge>::const_iterator ei = u_edges.begin(); ei != u_edges.end(); ++ei)
          {
            const edge& e = *ei;
            vertex& v = m_vertices[e.target.name()];
#ifdef MCRL2_PBES_EQELM_DEBUG
std::cerr << "\n<updating edge>" << e.to_string() << std::endl;
std::cerr << "  <source vertex       >" << u.to_string() << std::endl;
std::cerr << "  <target vertex before>" << v.to_string() << std::endl;
#endif

            term_type value = m_pbes_rewriter(new_data::variable_map_replace(e.condition, u.constraints));
#ifdef MCRL2_PBES_EQELM_DEBUG
std::cerr << "\nEvaluated condition " << core::pp(new_data::variable_map_replace(e.condition, u.constraints)) << " to " << core::pp(value) << std::endl;
#endif
            if (!tr::is_false(value) && !tr::is_true(value))
            {
#ifdef MCRL2_PBES_EQELM_DEBUG
std::cerr << "\nCould not evaluate condition " << core::pp(new_data::variable_map_replace(e.condition, u.constraints)) << " to true or false";
#endif
            }
            if (!tr::is_false(value))
            {
              bool changed = v.update(e.target.parameters(), u.constraints, m_data_rewriter);
              if (changed)
              {
                todo.push_back(v.variable);
                visited.insert(v.variable);
              }
            }
#ifdef MCRL2_PBES_EQELM_DEBUG
std::cerr << "  <target vertex after >" << v.to_string() << std::endl;
#endif
          }
        }

        if (mcrl2::core::gsDebug)
        {
          std::cerr << "\n--- final vertices ---" << std::endl;
          print_vertices();
        }

        // compute the redundant parameters and the redundant equations
        for (typename Container::iterator i = p.equations().begin(); i != p.equations().end(); ++i)
        {
          string_type name = i->variable().name();
          vertex& v = m_vertices[name];
          if (v.constraints.empty())
          {
            if (visited.find(i->variable()) == visited.end())
            {
              m_redundant_equations.insert(i->variable());
            }
          }
          else
          {
            std::vector<int> r = v.constant_parameter_indices();
            if (!r.empty())
            {
              m_redundant_parameters[name] = r;
            }
          }
        }

        // Apply the constraints to the equations.
        for (typename Container::iterator i = p.equations().begin(); i != p.equations().end(); ++i)
        {
          string_type name = i->variable().name();
          vertex& v = m_vertices[name];

          if (!v.constraints.empty())
          {
            *i = pbes_equation(
              i->symbol(),
              i->variable(),
              new_data::variable_map_replace(i->formula(), v.constraints)
            );
          }
        }

        // remove the redundant parameters and variables/equations
        remove_parameters(p, m_redundant_parameters);
        if (remove_redundant_equations)
        {
          remove_elements(p.equations(), detail::equation_is_contained_in<propositional_variable_decl_type>(m_redundant_equations));
        }

        // print the parameters and equation that are removed
        if (mcrl2::core::gsVerbose)
        {
          std::cerr << "\nremoved the following constant parameters:" << std::endl;
          std::map<propositional_variable_decl_type, std::vector<variable_type> > v = redundant_parameters();
          for (typename std::map<propositional_variable_decl_type, std::vector<variable_type> >::iterator i = v.begin(); i != v.end(); ++i)
          {
            for (typename std::vector<variable_type>::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
            {
              std::cerr << "  parameter (" << mcrl2::core::pp(i->first.name()) << ", " << core::pp(*j) << ")" << std::endl;
            }
          }

          if (remove_redundant_equations)
          {
            std::cerr << "\nremoved the following equations:" << std::endl;
            const std::set<propositional_variable_decl_type> r = redundant_equations();
            for (typename std::set<propositional_variable_decl_type>::const_iterator i = r.begin(); i != r.end(); ++i)
            {
              std::cerr << "  equation " << core::pp(i->name()) << std::endl;
            }
          }
        }
      }
*/
  };

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_EQELM_H
