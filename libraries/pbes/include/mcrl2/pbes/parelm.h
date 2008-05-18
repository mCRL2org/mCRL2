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
#include <boost/graph/adjacency_list.hpp>
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/core/reachable_nodes.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/detail/data_assignment_functional.h"
#include "mcrl2/data/detail/sorted_sequence_algorithm.h"
#include "mcrl2/lps/mcrl22lps.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/remove_parameters.h"
#include "mcrl2/pbes/detail/free_variable_visitor.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/find.h"

namespace mcrl2 {

namespace pbes_system {

class pbes_parelm_algorithm
{
  protected:
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> graph;
    typedef boost::graph_traits<graph>::vertex_descriptor vertex_descriptor;
    
    template <class Iter, class T>
    void iota(Iter first, Iter last, T value) const
    {
      while (first != last)
      {
        *first++ = value++;
      }
    }

    std::set<data::data_variable> unbound_variables(pbes_expression t, data::data_variable_list bound_variables) const
    {
      bool search_propositional_variables = false;
      detail::free_variable_visitor visitor(bound_variables, search_propositional_variables);
      visitor.visit(t);
      return visitor.result;      
    }
    
    int variable_index(data::data_variable_list v, data::data_variable d) const
    {
      int index = 0;
      for (data::data_variable_list::iterator i = v.begin(); i != v.end(); ++i)
      {
        if (*i == d)
        {
          return index;
        }
        index++;
      }
      return -1;
    }

    template <typename Container>
    void print_pp_container(const Container& v, std::string message = "<variables>", bool print_index = false) const
    {
      std::cout << message << std::endl;
      int index = 0;
      for (typename Container::const_iterator i = v.begin(); i != v.end(); ++i)
      {
        if (print_index)
        {
          std::cout << index++ << " " << pp(*i) << std::endl;
        }
        else
        {
          std::cout << pp(*i) << " ";
        }
      }
      std::cout << std::endl;
    }

    template <typename Container>
    void print_container(const Container& v, std::string message = "<variables>") const
    {
      std::cout << message << std::endl;
      for (typename Container::const_iterator i = v.begin(); i != v.end(); ++i)
      {
        std::cout << *i << " ";
      }
      std::cout << std::endl;
    }

    template <typename Container>
    void print_map(const Container& v, std::string message = "<variables>") const
    {
      std::cout << message << std::endl;
      for (typename Container::const_iterator i = v.begin(); i != v.end(); ++i)
      {
        std::cout << i->first << " -> " << i->second << std::endl;
      }
      std::cout << std::endl;
    }

  public:
    template <typename Container>
    void run(pbes<Container>& p) const
    {
std::cout << "<pbes>" << pp(p) << std::endl;
//print_pp_container(p.free_variables(), "<pbes free variables>");
      data::data_variable_list fvars(p.free_variables().begin(), p.free_variables().end());
      std::vector<data::data_variable> predicate_variables;
      
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

print_pp_container(predicate_variables, "<predicate_variables>", true);
//print_map(propvar_offsets, "<variable offsets>");

      // compute the initial set v of significant variables
      std::set<int> v;
      offset = 0;
      for (typename Container::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
      {
//std::cout << "<equation>" << pp(*i) << std::endl;
        std::set<data::data_variable> uvars = unbound_variables(i->formula(), fvars);
//print_pp_container(uvars, "<unbound variables>");

        for (std::set<data::data_variable>::iterator j = uvars.begin(); j != uvars.end(); ++j)
        {
          int k = variable_index(i->variable().parameters(), *j);
          if (k < 0)
          {
            std::cerr << "<variable error>" << pp(*j) << std::endl;
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
//std::cout << "<equation>" << pp(*i) << std::endl;
        core::identifier_string X = i->variable().name();
        data::data_variable_list params = i->variable().parameters();
        pbes_expression phi = i->formula();

        std::set<propositional_variable_instantiation> propvars = find_all_propositional_variable_instantiations(phi);
        for (std::set<propositional_variable_instantiation>::iterator j = propvars.begin(); j != propvars.end(); ++j)
        {
//std::cout << "<inst>" << pp(*j) << std::endl;
          int k0 = 0;
          for (data::data_expression_list::iterator k = j->parameters().begin(); k != j->parameters().end(); ++k)
          {
            std::set<data::data_variable> vars = data::find_all_data_variables(*k);
            for (std::set<data::data_variable>::iterator l = vars.begin(); l != vars.end(); ++l)
            {
              int vi = variable_index(params, *l);
              if (vi < 0)
              {
                continue;
              }
              int j0 = propvar_offsets[X] + vi;
//std::cout << "<edge>" << j0 << "," << k0 << "  " << pp(*l) << " " << pp(X) << std::endl;
              boost::add_edge(j0, k0, G);
            }
            k0++;
          }
        }
      }

print_container(v, "<initial significant variables>");

      // compute the reachable nodes (i.e. the significant parameters)
      std::vector<int> r = core::reachable_nodes(G, v.begin(), v.end());
print_container(r, "<significant variables>");
      std::sort(r.begin(), r.end());
      std::vector<int> q(N);
      iota(q.begin(), q.end(), 0);
      std::vector<int> s;
      std::set_difference(q.begin(), q.end(), r.begin(), r.end(), std::back_inserter(s));
print_container(s, "<insignificant variables>");
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PARELM_H
