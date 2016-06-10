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

#include <sstream>
#include <utility>
#include <deque>
#include <map>
#include <set>
#include <sstream>
#include <vector>
#include <algorithm>
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/rewriters/data_rewriter.h"
#include "mcrl2/pbes/replace.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2
{

namespace pbes_system
{

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
    typedef std::set<variable_type> equivalence_class;

    /// \brief Compares data expressions for equality.
    const DataRewriter& m_data_rewriter;

    /// \brief Compares data expressions for equality.
    const PbesRewriter& m_pbes_rewriter;

    /// \brief The vertices of the grapth, i.e. the equivalence relations.
    /// It stores the equivalence sets for each propositional variable, for example
    /// X -> [ {x1, x3}, {x2, x4} ]. Equivalence sets of size 1 are not stored.
    std::map<string_type, std::vector<equivalence_class> > m_vertices;

    /// \brief The edges of the graph.
    /// It is a mapping from X to iocc(X).
    std::map<string_type, std::set<propositional_variable_type> > m_edges;

    /// \brief The parameters of the propositional variable declarations.
    /// These are stored inside a vector, for efficiency reasons.
    std::map<string_type, std::vector<variable_type> > m_parameters;

    /// \brief Used for determining if a vertex has been visited before.
    std::map<string_type, bool> m_discovered;

    // TODO: design a more generic solution for printing sets
    std::string print(const core::identifier_string& x) const
    {
      return core::pp(x);
    }
    template <typename Set>
    std::string print_set(const Set& s) const
    {
      std::ostringstream out;
      out << "{ ";
      for (auto i = s.begin(); i != s.end(); ++i)
      {
        if (i != s.begin())
        {
          out << ", ";
        }
        out << print(atermpp::deprecated_cast<core::identifier_string>(*i));
      }
      return out.str();
    }

    /// \brief Puts all parameters of the same sort in the same equivalence set.
    std::vector<equivalence_class> compute_equivalence_sets(const propositional_variable_decl_type& X) const
    {
      std::map< data::sort_expression, equivalence_class> m;
      for (auto i = X.parameters().begin(); i != X.parameters().end(); ++i)
      {
        m[i->sort()].insert(*i);
      }
      std::vector<equivalence_class> result;
      for (auto i = m.begin(); i != m.end(); ++i)
      {
        if (i->second.size() > 1)
        {
          result.push_back(i->second);
        }
      }
      return result;
    }

    /// \brief Prints the vertices of the dependency graph.
    std::string print_vertices() const
    {
      std::ostringstream out;
      for (auto i = m_vertices.begin(); i != m_vertices.end(); ++i)
      {
        out << i->first << " -> [ ";
        const std::vector<equivalence_class>& v = i->second;
        for (auto j = v.begin(); j != v.end(); ++j)
        {
          if (j != v.begin())
          {
            out << ", ";
          }
          out << print_set(*j);
        }
        out << " ]" << std::endl;
      }
      return out.str();
    }

    /// \brief Prints the edges of the dependency graph.
    std::string print_edges() const
    {
      std::ostringstream out;
      for (auto i = m_edges.begin(); i != m_edges.end(); ++i)
      {
        out << i->first << " -> " << print_set(i->second) << std::endl;
      }
      return out.str();
    }

    /// \brief Prints the equivalence classes
    std::string print_equivalence_classes() const
    {
      std::ostringstream out;
      for (auto i = m_vertices.begin(); i != m_vertices.end(); ++i)
      {
        out << "  vertex " << i->first << ": ";
        for (auto j = i->second.begin(); j != i->second.end(); ++j)
        {
          out << print_set(*j) << " ";
        }
        out << std::endl;
      }
      return out.str();
    }

    /// \brief Prints the todo list
    void log_todo_list(const std::set<string_type>& todo, const std::string& msg = "") const
    {
      mCRL2log(log::debug) << msg;
      mCRL2log(log::debug) << print_set(todo) << "\n";
    }

    /// \brief Returns true if the vertex X should propagate its values to Y
    bool evaluate_guard(const string_type& /* X */, const propositional_variable_type& /* Y */)
    {
      return true;
    }

    /// \brief Returns the index of the element x in the sequence v
    template <typename VariableContainer>
    size_t index_of(const variable_type& x, const VariableContainer& v)
    {
      return static_cast<size_t>(std::find(v.begin(), v.end(), x) - v.begin());
    }

    /// \brief Propagate the equivalence relations given by the substitution vX over the edge Ye.
    template <typename Substitution>
    void update_equivalence_classes(const propositional_variable_type& Ye,
                                    const Substitution& vX,
                                    std::set<string_type>& todo
                                   )
    {
      const string_type& Y = Ye.name();
      std::vector<data_term_type> e(Ye.parameters().begin(), Ye.parameters().end());

      std::vector<equivalence_class>& cY = m_vertices[Y];
      std::vector<equivalence_class> cY1;
      for (auto j = cY.begin(); j != cY.end(); ++j)
      {
        equivalence_class& equiv = *j;
        std::map<data_term_type, equivalence_class> w;
        for (auto k = equiv.begin(); k != equiv.end(); ++k)
        {
          size_t p = index_of(*k, m_parameters[Y]);
          pbes_system::data_rewriter<DataRewriter> rewr(m_data_rewriter);
          pbes_system::pbes_expression e_p = rewr(e[p], vX);
          w[atermpp::down_cast<const data::data_expression>(e_p)].insert(*k);
        }
        for (auto i = w.begin(); i != w.end(); ++i)
        {
          if (i->second.size() > 1)
          {
            cY1.push_back(i->second);
          }
        }
      }
      if (cY != cY1)
      {
        todo.insert(Y);
        m_discovered[Y] = true;
        cY = cY1;
      }
      else if (!m_discovered[Y])
      {
        todo.insert(Y);
        m_discovered[Y] = true;
      }
    }

    /// \brief Computes a substitution that corresponds to the equivalence relations in X
    data::mutable_map_substitution<> compute_substitution(const string_type& X)
    {
      data::mutable_map_substitution<> result;
      const std::vector<equivalence_class>& cX = m_vertices[X];
      for (auto i = cX.begin(); i != cX.end(); ++i)
      {
        const equivalence_class& s = *i;
        for (auto j = ++s.begin(); j != s.end(); ++j)
        {
          result[*j] = *s.begin();
        }
      }
      return result;
    }

    /// \brief Chooses one parameter for every equivalence class, and
    /// removes the others. All occurrences of the removed parameters
    /// are replaced by the chosen parameter.
    inline
    void apply_equivalence_relations(pbes& p)
    {
      // first apply the substitutions to the equations
      for (pbes_equation& eqn: p.equations())
      {
        string_type X = eqn.variable().name();
        data::mutable_map_substitution<> sigma = compute_substitution(X);
        if (!sigma.empty())
        {
          eqn.formula() = pbes_system::replace_variables_capture_avoiding(eqn.formula(), sigma, data::substitution_variables(sigma));
        }
      }

      // then remove parameters
      std::map<string_type, std::vector<size_t> > to_be_removed;
      for (pbes_equation& eqn: p.equations())
      {
        string_type X = eqn.variable().name();
        const std::vector<equivalence_class>& eq = m_vertices[X];
        for (auto j = eq.begin(); j != eq.end(); ++j)
        {
          for (auto k = ++j->begin(); k != j->end(); ++k)
          {
            to_be_removed[X].push_back(index_of(*k, m_parameters[X]));
          }
        }
      }
      pbes_system::algorithms::remove_parameters(p, to_be_removed);
    }

  public:
    /// \brief Constructor.
    /// \param datar A data rewriter
    /// \param pbesr A PBES rewriter
    pbes_eqelm_algorithm(const DataRewriter& datar, const PbesRewriter& pbesr)
      : m_data_rewriter(datar),
        m_pbes_rewriter(pbesr)
    {}

    /// \brief Runs the eqelm algorithm
    /// \param p A pbes
    /// \param ignore_initial_state If true, the initial state is ignored.
    void run(pbes& p, bool ignore_initial_state = false)
    {
      m_vertices.clear();
      m_edges.clear();
      std::set<string_type> todo;

      // compute the vertices and edges of the graph
      for (pbes_equation& eqn: p.equations())
      {
        string_type name = eqn.variable().name();
        m_edges[name] = find_propositional_variable_instantiations(eqn.formula());
        m_vertices[name] = compute_equivalence_sets(eqn.variable());
        const variable_sequence_type& param = eqn.variable().parameters();
        m_parameters[name] = std::vector<variable_type>(param.begin(), param.end());
        todo.insert(name);
        m_discovered[name] = ignore_initial_state;
      }

      if (!ignore_initial_state)
      {
        todo.clear();
        propositional_variable_type kappa = p.initial_state();
        const string_type& X = kappa.name();
        data::mutable_map_substitution<> vX = compute_substitution(X);

        // propagate the equivalence relations in X over the edge kappa
        if (evaluate_guard(X, kappa))
        {
          todo.insert(X);
          m_discovered[X] = true;
          update_equivalence_classes(kappa, vX, todo);
          mCRL2log(log::debug) << "updated equivalence classes using initial state " << kappa << "\n" << print_equivalence_classes();
        }
      }

      mCRL2log(log::verbose) << "--- vertices ---\n" << print_vertices();
      mCRL2log(log::verbose) << "\n--- edges ---\n" << print_edges();
      mCRL2log(log::debug) << "computed initial equivalence classes\n" << print_equivalence_classes();

      // propagate constraints over the edges until the todo list is empty
      while (!todo.empty())
      {
        mCRL2log(log::debug) << "todo list = " << print_set(todo) << "\n";
        mCRL2log(log::verbose) << "--- vertices ---\n" << print_vertices();

        string_type X = *todo.begin();
        todo.erase(X);
        mCRL2log(log::debug) << "choose todo element " << X << "\n";

        // create a substitution function that corresponds to cX
        data::mutable_map_substitution<> vX = compute_substitution(X);
        const std::set<propositional_variable_type>& edges = m_edges[X];
        for (auto i = edges.begin(); i != edges.end(); ++i)
        {
          // propagate the equivalence relations in X over the edge Ye
          const propositional_variable_type& Ye = *i;
          if (evaluate_guard(X, Ye))
          {
            update_equivalence_classes(Ye, vX, todo);
            mCRL2log(log::debug) << "updated equivalence classes using edge " << Ye << "\n" << print_equivalence_classes();
          }
        }
      }
      apply_equivalence_relations(p);
      mCRL2log(log::verbose) << "\n--- result ---\n" << print_vertices();
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_EQELM_H
