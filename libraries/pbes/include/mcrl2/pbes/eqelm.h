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

      /// \brief The parameters of the propositional variable declarations.
      std::map<string_type, std::vector<variable_type> > m_parameters;

      /// \brief Puts all parameters of the same sort in the same equivalence set.
      std::vector<std::set<variable_type> > compute_equivalence_sets(const propositional_variable_decl_type& X) const
      {
        std::map<sort_expression, std::set<variable_type> > m;
        for (typename variable_sequence_type::const_iterator i = X.parameters().begin(); i != X.parameters().end(); ++i)
        {
          m[i->sort()].insert(*i);
        }
        std::vector<std::set<variable_type> > result;
        for (typename std::map<sort_expression, std::set<variable_type> >::iterator i = m.begin(); i != m.end(); ++i)
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
          for (typename std::vector<std::set<variable_type> >::const_iterator j = v.begin(); j != v.end(); ++j)
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

      /// \brief Returns true if the vertex X should propagate its values to Y
      bool evaluate_guard(const string_type& X, const propositional_variable_type& Y)
      {
        return true;
      }

      /// \brief Returns the index of the element x in the sequence v
      template <typename VariableContainer>
      unsigned int index_of(const variable_type& x, const VariableContainer& v)
      {
        return std::find(v.begin(), v.end(), x) - v.begin();
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
        std::set<string_type> todo;

        // compute the edges and vertices of the graph, and initialize the todo list
        for (typename Container::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
        {
          string_type name = i->variable().name();
          m_edges[name] = find_all_propositional_variable_instantiations(i->formula());
          m_vertices[name] = compute_equivalence_sets(i->variable());
          const variable_sequence_type& param = i->variable().parameters();
          m_parameters[name] = std::vector<variable_type>(param.begin(), param.end());
          todo.insert(name);
        }

        if (mcrl2::core::gsDebug)
        {
          std::clog << "--- vertices ---" << std::endl;
          print_vertices();
          std::clog << std::endl;
          std::clog << "--- edges ---" << std::endl;
          print_edges();
        }

        // propagate constraints over the edges until the todo list is empty
        while (!todo.empty())
        {
          string_type X = *todo.begin();
          todo.erase(X);

          if (mcrl2::core::gsDebug)
          {
            std::clog << "todo element X = " << core::pp(X) << std::endl;
            std::clog << "todo list = " << detail::print_pp_set(todo) << std::endl;
            std::clog << "--- vertices ---" << std::endl;
            print_vertices();
          }
          
          // create a substitution function that corresponds to cX
          std::map<variable_type, data_term_type> vX;
          const std::vector<std::set<variable_type> >& cX = m_vertices[X];
          for (typename std::vector<std::set<variable_type> >::const_iterator i = cX.begin(); i != cX.end(); ++i)
          {
            const std::set<variable_type>& s = *i;
            for (typename std::set<variable_type>::const_iterator j = ++s.begin(); j != s.end(); ++j)
            {
              vX[*j] = *s.begin();
            }
          }

          const atermpp::set<propositional_variable_type>& edges = m_edges[X];
          for (typename atermpp::set<propositional_variable_type>::const_iterator i = edges.begin(); i != edges.end(); ++i)
          {
            const propositional_variable_type& Ye = *i;

            if (evaluate_guard(X, Ye))
            {
              const string_type& Y = Ye.name();
              std::vector<data_term_type> e(Ye.parameters().begin(), Ye.parameters().end());

              std::vector<std::set<variable_type> >& cY = m_vertices[Y];
              std::vector<std::set<variable_type> > cY1;
              for (typename std::vector<std::set<variable_type> >::iterator j = cY.begin(); j != cY.end(); ++j)
              {
                std::set<variable_type>& equiv = *j; // an equivalence set              
                atermpp::map<data_term_type, std::set<variable_type> > w;
                for (typename std::set<variable_type>::iterator k = equiv.begin(); k != equiv.end(); ++k)
                {
                  unsigned int p = index_of(*k, m_parameters[Y]);
                  w[m_data_rewriter(data_variable_map_replace(e[p], vX))].insert(*k);
                }
                if (w.size() > 1)
                {
                  todo.insert(Y);
                }
                for (typename std::map<data_term_type, std::set<variable_type> >::iterator i = w.begin(); i != w.end(); ++i)
                {
                  if (i->second.size() > 1)
                  {
                    cY1.push_back(i->second);
                  }
                }
              }
              cY = cY1;
            }
          }
        }
       std::clog << "--- result ---" << std::endl;
       print_vertices();
     }
  };

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_EQELM_H
