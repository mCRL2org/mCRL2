// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/parelm.h
/// \brief The parelm algorithm.

#ifndef MCRL2_PBES_PARELM_H
#define MCRL2_PBES_PARELM_H

#include <iterator>
#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include <boost/bind.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/core/reachable_nodes.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/detail/iota.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/detail/assignment_functional.h"
#include "mcrl2/data/detail/sorted_sequence_algorithm.h"
#include "mcrl2/lps/mcrl22lps.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/pbes/detail/global_variable_visitor.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/remove_parameters.h"
#include "mcrl2/pbes/find.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief Algorithm class for the parelm algorithm
class pbes_parelm_algorithm
{
  protected:
    /// \brief The graph type of the dependency graph
    typedef boost::adjacency_list<boost::setS, boost::vecS, boost::directedS> graph;

    /// \brief The vertex type of the dependency graph
    typedef boost::graph_traits<graph>::vertex_descriptor vertex_descriptor;

    /// \brief The edge type of the dependency graph
    typedef boost::graph_traits<graph>::edge_descriptor edge_descriptor;

    /// \brief Finds unbound variables in a pbes expression
    /// \param t A PBES expression
    /// \param bound_variables A sequence of data variables
    /// \return The unbound variables in \p t that are not contained in \p bound_variables
    std::set<data::variable> unbound_variables(pbes_expression t, data::variable_list bound_variables) const
    {
      bool search_propositional_variables = false;
      detail::global_variable_visitor<pbes_expression> visitor(bound_variables, search_propositional_variables);
      visitor.visit(t);
      return visitor.result;
    }

    /// \brief Finds the index of a variable in a sequence
    /// \param v A sequence of data variables
    /// \param d A data variable
    /// \return The index of \p d in \p v, or -1 if the variable wasn't found
    int variable_index(data::variable_list v, data::variable d) const
    {
      int index = 0;
      for (data::variable_list::iterator i = v.begin(); i != v.end(); ++i)
      {
        if (*i == d)
        {
          return index;
        }
        index++;
      }
      return -1;
    }

    /// \cond INTERNAL_DOCS

    /// \brief Prints a container of terms to standard error
    /// \param v A container
    /// \param message A string
    /// \param print_index If true, an index is written in front of each term
    template <typename Container>
    void print_pp_container(const Container& v, std::string message = "<variables>", bool print_index = false) const
    {
      std::cerr << message << std::endl;
      int index = 0;
      for (typename Container::const_iterator i = v.begin(); i != v.end(); ++i)
      {
        if (print_index)
        {
          std::cerr << index++ << " " << mcrl2::core::pp(*i) << std::endl;
        }
        else
        {
          std::cerr << mcrl2::core::pp(*i) << " ";
        }
      }
      std::cerr << std::endl;
    }

    /// \brief FUNCTION_DESCRIPTION
    // \brief Prints a container to standard error
    // \param v A container
    // \param message A string
    /// \param v PARAM_DESCRIPTION
    /// \param message A string
    template <typename Container>
    void print_container(const Container& v, std::string message = "<variables>") const
    {
      std::cerr << message << std::endl;
      for (typename Container::const_iterator i = v.begin(); i != v.end(); ++i)
      {
        std::cerr << *i << " ";
      }
      std::cerr << std::endl;
    }

    /// \brief Prints a map to standard error
    /// \param v A map container
    /// \param message A string
    template <typename Container>
    void print_map(const Container& v, std::string message = "<variables>") const
    {
      std::cerr << message << std::endl;
      for (typename Container::const_iterator i = v.begin(); i != v.end(); ++i)
      {
        std::cerr << i->first << " -> " << i->second << std::endl;
      }
      std::cerr << std::endl;
    }

    /// \endcond

    /// \brief Finds the predicate variable to which the data parameter with the given index belongs.
    /// Here index refers to the cumulative index in the array obtained by concatening all parameters
    /// of the predicate variables in the pbes \p p.
    /// \param p A pbes
    /// \param index A positive number
    /// \return The name of the predicate variable that corresponds with \p index
    template <typename Container>
    core::identifier_string find_predicate_variable(const pbes<Container>& p, int index) const
    {
      int offset = 0;
      for (typename Container::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
      {
        int size = i->variable().parameters().size();
        if (offset + size > index)
        {
          return i->variable().name();
        }
        offset += i->variable().parameters().size();
      }
      return core::identifier_string("<not found>");
    }

  public:

    /// \brief Runs the parelm algorithm. The pbes \p is modified by the algorithm
    /// \param p A pbes
    template <typename Container>
    void run(pbes<Container>& p) const
    {
      data::variable_list fvars(p.global_variables().begin(), p.global_variables().end());
      std::vector<data::variable> predicate_variables;

      // compute a mapping from propositional variable names to offsets
      int offset = 0;
      std::map<core::identifier_string, int> propvar_offsets;
      for (typename Container::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
      {
        propvar_offsets[i->variable().name()] = offset;
        offset += i->variable().parameters().size();
        predicate_variables.insert(predicate_variables.end(), i->variable().parameters().begin(), i->variable().parameters().end());
      }
      int N = offset; // # variables

      // compute the initial set v of significant variables
      std::set<int> v;
      offset = 0;
      for (typename Container::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
      {
        std::set<data::variable> uvars = unbound_variables(i->formula(), fvars);
        for (std::set<data::variable>::iterator j = uvars.begin(); j != uvars.end(); ++j)
        {
          int k = variable_index(i->variable().parameters(), *j);
          if (k < 0)
          {
            std::cerr << "<variable error>" << mcrl2::core::pp(*j) << std::endl;
            continue;
          }
          v.insert(offset + k);
        }
        offset += i->variable().parameters().size();
      }

      // compute the dependency graph G
      graph G(N);
      for (typename Container::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
      {
        // left hand side (X)
        core::identifier_string X = i->variable().name();
        data::variable_list Xparams = i->variable().parameters();

        // right hand side (Y)
        pbes_expression phi = i->formula();
        std::set<propositional_variable_instantiation> propvars = find_all_propositional_variable_instantiations(phi);
        for (std::set<propositional_variable_instantiation>::iterator j = propvars.begin(); j != propvars.end(); ++j)
        {
          core::identifier_string Y = j->name();
          data::data_expression_list Yparams = j->parameters();
          int Yindex = 0;
          for (data::data_expression_list::iterator y = Yparams.begin(); y != Yparams.end(); ++y)
          {
            std::set<data::variable> vars = data::find_all_variables(*y);
            for (std::set<data::variable>::iterator k = vars.begin(); k != vars.end(); ++k)
            {
              int Xindex = variable_index(Xparams, *k);
              if (Xindex < 0)
              {
                continue;
              }
              // parameter (Y, Yindex) is influenced by (X, Xindex)
              boost::add_edge(propvar_offsets[Y] + Yindex, propvar_offsets[X] + Xindex, G);
            }
            Yindex++;
          }
        }
      }

      // compute the indices s of the parameters that need to be removed
      std::vector<int> r = core::reachable_nodes(G, v.begin(), v.end());
      std::sort(r.begin(), r.end());
      std::vector<int> q(N);
      core::detail::iota(q.begin(), q.end(), 0);
      std::vector<int> s;
      std::set_difference(q.begin(), q.end(), r.begin(), r.end(), std::back_inserter(s));

      // create a map that specifies the parameters that need to be removed
      std::map<core::identifier_string, std::vector<int> > removals;
      int index = 0;
      std::vector<int>::iterator sfirst = s.begin();
      for (typename Container::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
      {
        int maxindex = index + i->variable().parameters().size();
        std::vector<int>::iterator slast = std::find_if(sfirst, s.end(), boost::bind(std::greater_equal<int>(), _1, maxindex));
        if (slast > sfirst)
        {
          std::vector<int> w(sfirst, slast);
          std::transform(w.begin(), w.end(), w.begin(), boost::bind(std::minus<int>(), _1, index));
          removals[i->variable().name()] = w;
        }
        index = maxindex;
        sfirst = slast;
      }

      // print debug output
      if (mcrl2::core::gsDebug)
      {
        std::cerr << "\ninfluential parameters:" << std::endl;
        for(std::set<int>::iterator i = v.begin(); i != v.end(); ++i)
        {
          core::identifier_string X1 = find_predicate_variable(p, *i);
          data::variable v1 = predicate_variables[*i];
          std::cerr << "(" + mcrl2::core::pp(X1) + ", " + mcrl2::core::pp(v1) + ")\n";
        }
        std::cerr << "\ndependencies:" << std::endl;
        typedef typename boost::graph_traits<graph>::edge_iterator edge_iterator;
        std::pair<edge_iterator, edge_iterator> e = edges(G);
        edge_iterator first = e.first;
        edge_iterator last  = e.second;
        for( ; first != last; ++first)
        {
          edge_descriptor e = *first;
          int i1 = boost::source(e, G);
          core::identifier_string X1 = find_predicate_variable(p, i1);
          data::variable v1 = predicate_variables[i1];
          int i2 = boost::target(e, G);
          core::identifier_string X2 = find_predicate_variable(p, i2);
          data::variable v2 = predicate_variables[i2];
          std::string left  = "(" + mcrl2::core::pp(X1) + ", " + mcrl2::core::pp(v1) + ")";
          std::string right = "(" + mcrl2::core::pp(X2) + ", " + mcrl2::core::pp(v2) + ")";
          std::cerr << left << " -> " << right << std::endl;
        }
      }

      // print verbose output
      if (mcrl2::core::gsVerbose)
      {
        std::cerr << "\nremoving the following parameters:" << std::endl;
        for (std::map<core::identifier_string, std::vector<int> >::const_iterator i = removals.begin(); i != removals.end(); ++i)
        {
          core::identifier_string X1 = i->first;

          for (std::vector<int>::const_iterator j = (i->second).begin(); j != (i->second).end(); ++j)
          {
            data::variable v1 = predicate_variables[*j + propvar_offsets[X1]];
            std::cerr << "(" + mcrl2::core::pp(X1) + ", " + mcrl2::core::pp(v1) + ")\n";
          }
        }
      }

      // remove the parameters
      remove_parameters(p, removals);
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PARELM_H
