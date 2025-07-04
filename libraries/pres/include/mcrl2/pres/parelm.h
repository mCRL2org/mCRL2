// Author(s): Jan Friso Groote. Based on pbes/parelm.h by Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/parelm.h
/// \brief The parelm algorithm.

#ifndef MCRL2_PRES_PARELM_H
#define MCRL2_PRES_PARELM_H

#include "mcrl2/pres/algorithms.h"
#include "mcrl2/pres/detail/find_free_variables.h"
#include "mcrl2/utilities/detail/iota.h"
#include "mcrl2/utilities/reachable_nodes.h"

namespace mcrl2::pres_system {

namespace detail {

/// \brief Finds the index of a variable in a sequence
/// \param variables A sequence of data variables
/// \param d A data variable
/// \return The index of \p d in \p v, or -1 if the variable wasn't found
inline
int variable_index(const data::variable_list& variables, const data::variable& d)
{
  int index = 0;
  for (const data::variable& v: variables)
  {
    if (v == d)
    {
      return index;
    }
    index++;
  }
  return -1;
}

} // namespace detail

/// \brief Algorithm class for the parelm algorithm
class pres_parelm_algorithm
{
  protected:
    /// \brief The graph type of the dependency graph
    using graph = boost::adjacency_list<boost::setS, boost::vecS, boost::directedS>;

    /// \brief The vertex type of the dependency graph
    using vertex_descriptor = boost::graph_traits<graph>::vertex_descriptor;

    /// \brief The edge type of the dependency graph
    using edge_descriptor = boost::graph_traits<graph>::edge_descriptor;

    struct parelm_dependency_traverser: public pres_expression_traverser<parelm_dependency_traverser>
    {
      using super = pres_expression_traverser<parelm_dependency_traverser>;
      using super::enter;
      using super::leave;
      using super::apply;

      graph& G;
      const std::map<core::identifier_string, std::size_t>& propvar_offsets;

      core::identifier_string X;
      data::variable_list Xparams;
      std::multiset<data::variable> bound_variables;

      parelm_dependency_traverser(graph& G_, const std::map<core::identifier_string, std::size_t>& propvar_offsets_)
        : G(G_), propvar_offsets(propvar_offsets_)
      {}

      void enter(const infimum& x)
      {
        for (const data::variable& v: x.variables())
        {
          bound_variables.insert(v);
        }
      }

      void leave(const infimum& x)
      {
        for (const data::variable& v: x.variables())
        {
          bound_variables.erase(v);
        }
      }

      void enter(const supremum& x)
      {
        for (const data::variable& v: x.variables())
        {
          bound_variables.insert(v);
        }
      }

      void leave(const supremum& x)
      {
        for (const data::variable& v: x.variables())
        {
          bound_variables.erase(v);
        }
      }

      void enter(const sum& x)
      {
        for (const data::variable& v: x.variables())
        {
          bound_variables.insert(v);
        }
      }

      void leave(const sum& x)
      {
        for (const data::variable& v: x.variables())
        {
          bound_variables.erase(v);
        }
      }


      void apply(const propositional_variable_instantiation& x)
      {
        using utilities::detail::contains;

        const core::identifier_string& Y = x.name();
        int Yindex = 0;
        for (const data::data_expression& e: x.parameters())
        {
          for (const data::variable& var: data::find_free_variables(e))
          {
            if (contains(bound_variables, var))
            {
              continue;
            }
            int Xindex = detail::variable_index(Xparams, var);
            if (Xindex < 0)
            {
              continue;
            }
            // parameter (Y, Yindex) is influenced by (X, Xindex)
            boost::add_edge(propvar_offsets.at(Y) + Yindex, propvar_offsets.at(X) + Xindex, G);
          }
          Yindex++;
        }
      }

      void apply(const pres_equation& eqn)
      {
        X = eqn.variable().name();
        Xparams = eqn.variable().parameters();
        super::apply(eqn);
      }
    };

    /// \brief Finds unbound variables in a pres expression
    /// \param t A PRES expression
    /// \param bound_variables A sequence of data variables
    /// \return The unbound variables in \p t that are not contained in \p bound_variables
    static std::set<data::variable> unbound_variables(const pres_expression& t, const data::variable_list& bound_variables)
    {
      bool search_propositional_variables = false;
      return detail::find_free_variables(t, bound_variables, search_propositional_variables);
    }

    /// \brief Finds the predicate variable to which the data parameter with the given index belongs.
    /// Here index refers to the cumulative index in the array obtained by concatening all parameters
    /// of the predicate variables in the pres \p p.
    /// \param p A pres
    /// \param index A positive number
    /// \return The name of the predicate variable that corresponds with \p index
    static core::identifier_string find_predicate_variable(const pres& p, std::size_t index)
    {
      std::size_t offset = 0;
      for (const pres_equation& eqn: p.equations())
      {
        std::size_t size = eqn.variable().parameters().size();
        if (offset + size > index)
        {
          return eqn.variable().name();
        }
        offset += eqn.variable().parameters().size();
      }
      return core::identifier_string("<not found>");
    }

    static void compute_dependency_graph(const pres& p, const std::map<core::identifier_string, std::size_t>& propvar_offsets, graph& G)
    {
      parelm_dependency_traverser f(G, propvar_offsets);
      for (const pres_equation& eqn: p.equations())
      {
        f.apply(eqn);
      }
    }

  public:
    /// \brief Runs the parelm algorithm. The pres \p is modified by the algorithm
    /// \param p A pres
    void run(pres& p)
    {
      data::variable_list global_variables(p.global_variables().begin(), p.global_variables().end());
      std::vector<data::variable> predicate_variables;

      // compute a mapping from propositional variable names to offsets
      std::size_t offset = 0;
      std::map<core::identifier_string, std::size_t> propvar_offsets;
      for (pres_equation& eqn: p.equations())
      {
        propvar_offsets[eqn.variable().name()] = offset;
        offset += eqn.variable().parameters().size();
        predicate_variables.insert(predicate_variables.end(), eqn.variable().parameters().begin(), eqn.variable().parameters().end());
      }
      std::size_t N = offset; // # variables

      // compute the initial set v of significant variables
      std::set<std::size_t> significant_variables;
      offset = 0;
      for (pres_equation& eqn: p.equations())
      {
        for (const data::variable& w: unbound_variables(eqn.formula(), global_variables))
        {
          int k = detail::variable_index(eqn.variable().parameters(), w);
          if (k < 0)
          {
            throw mcrl2::runtime_error("<variable error>" + data::pp(w));
          }
          significant_variables.insert(offset + k);
        }
        offset += eqn.variable().parameters().size();
      }

      graph G(N);
      compute_dependency_graph(p, propvar_offsets, G);

      // compute the indices s of the parameters that need to be removed
      std::vector<std::size_t> r = utilities::reachable_nodes(G, significant_variables.begin(), significant_variables.end());
      std::sort(r.begin(), r.end());
      std::vector<std::size_t> q(N);
      utilities::detail::iota(q.begin(), q.end(), 0);
      std::vector<std::size_t> s;
      std::set_difference(q.begin(), q.end(), r.begin(), r.end(), std::back_inserter(s));

      // create a map that specifies the parameters that need to be removed
      std::map<core::identifier_string, std::vector<std::size_t> > removals;
      std::size_t index = 0;
      auto sfirst = s.begin();
      for (pres_equation& eqn: p.equations())
      {
        std::size_t maxindex = index + eqn.variable().parameters().size();
        auto slast = std::find_if(sfirst, s.end(), [&](std::size_t i) { return i >= maxindex; });
        if (slast > sfirst)
        {
          std::vector<std::size_t> w(sfirst, slast);
          std::transform(w.begin(), w.end(), w.begin(), [&](std::size_t i) { return i - index; });
          removals[eqn.variable().name()] = w;
        }
        index = maxindex;
        sfirst = slast;
      }

      if (mCRL2logEnabled(log::debug))
      {
        print_dependencies(p, predicate_variables, significant_variables, G);
      }

      // print verbose output
      if (mCRL2logEnabled(log::verbose))
      {
        print_removed_parameters(predicate_variables, propvar_offsets, removals);
      }

      // remove the parameters
      pres_system::algorithms::remove_parameters(p, removals);
    }

    void print_removed_parameters(const std::vector<data::variable>& predicate_variables,
                                  const std::map<core::identifier_string, std::size_t>& propvar_offsets,
                                  const std::map<core::identifier_string, std::vector<std::size_t>>& removals) const
    {
      mCRL2log(log::verbose) << "\nremoving the following parameters:" << std::endl;
      for (auto& removal: removals)
      {
        core::identifier_string X1 = removal.first;
        for (std::size_t j: removal.second)
        {
          data::variable v1 = predicate_variables[j + propvar_offsets.at(X1)];
          mCRL2log(log::verbose) << "(" + core::pp(X1) + ", " + data::pp(v1) + ")\n";
        }
      }
    }

    static void print_dependencies(const pres& p, const std::vector<data::variable>& predicate_variables, const std::set<std::size_t>& significant_variables, const graph& G)
    {
      mCRL2log(log::debug) << "\ninfluential parameters:" << std::endl;
      for (std::size_t i: significant_variables)
      {
        core::identifier_string X1 = find_predicate_variable(p, i);
        data::variable v1 = predicate_variables[i];
        mCRL2log(log::debug) << "(" + core::pp(X1) + ", " + data::pp(v1) + ")\n";
      }
      mCRL2log(log::debug) << "\ndependencies:" << std::endl;
      using edge_iterator = boost::graph_traits<graph>::edge_iterator;
      std::pair<edge_iterator, edge_iterator> e = edges(G);
      edge_iterator first = e.first;
      edge_iterator last  = e.second;
      for (; first != last; ++first)
      {
        edge_descriptor f = *first;
        std::size_t i1 = boost::source(f, G);
        core::identifier_string X1 = find_predicate_variable(p, i1);
        data::variable v1 = predicate_variables[i1];
        std::size_t i2 = boost::target(f, G);
        core::identifier_string X2 = find_predicate_variable(p, i2);
        data::variable v2 = predicate_variables[i2];
        std::string left  = "(" + core::pp(X1) + ", " + data::pp(v1) + ")";
        std::string right = "(" + core::pp(X2) + ", " + data::pp(v2) + ")";
        mCRL2log(log::debug) << left << " -> " << right << std::endl;
      }
    }
};

/// \brief Apply the parelm algorithm
/// \param p A PRES to which the algorithm is applied
inline
void parelm(pres& p)
{
  pres_parelm_algorithm algorithm;
  algorithm.run(p);
}

} // namespace mcrl2::pres_system

#endif // MCRL2_PRES_PARELM_H
