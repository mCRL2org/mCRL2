// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/enumerator.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_ENUMERATOR_H
#define MCRL2_PBES_ENUMERATOR_H

#include <deque>
#include <map>
#include <sstream>
#include <utility>
#include "mcrl2/data/identifier_generator.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/substitutions/enumerator_substitution.h"
#include "mcrl2/pbes/pbes_expression.h"

namespace mcrl2 {

namespace pbes_system {

template <typename PbesRewriter>
class enumerator_algorithm
{
  /// \brief A map that caches the constructors corresponding to sort expressions.
  typedef std::map<data::sort_expression, std::vector<data::function_symbol> > constructor_map;

  protected:
    const pbes_expression& stop;

    // a rewriter
    PbesRewriter& R;

    /// \brief A data specification.
    const data::data_specification& m_data;

    // the list of partial solutions
    std::deque<std::pair<data::variable_list, data::enumerator_substitution> > P;

    data::set_identifier_generator id_generator;

    /// \brief A mapping with constructors.
    mutable constructor_map m_constructors;

    /// \brief Returns the constructors with target s.
    /// \param s A sort expression
    /// \return The constructors corresponding to the sort expression.
    const std::vector<data::function_symbol>& constructors(const data::sort_expression& s) const
    {
      auto i = m_constructors.find(s);
      if (i != m_constructors.end())
      {
        return i->second;
      }
      m_constructors[s] = m_data.constructors(s);
      return m_constructors[s];
    }

    std::string print(const data::variable& x) const
    {
      std::ostringstream out;
      out << x << ": " << x.sort();
      return out.str();
    }

    bool is_finite_set(const data::sort_expression& x) const
    {
      return false;
    }

    bool is_function_sort(const data::sort_expression& x) const
    {
      return false;
    }

  public:
    enumerator_algorithm(const data::variable_list& v, const pbes_expression& phi, const pbes_expression& stop_, PbesRewriter& R_, const data::data_specification& data_spec)
      : stop(stop_), R(R_), m_data(data_spec)
    {
      P.push_back(std::make_pair(v, data::enumerator_substitution()));
      for (auto i = v.begin(); i != v.end(); ++i)
      {
        id_generator.add_identifier(i->name());
      }
      std::set<core::identifier_string> N = pbes_system::find_identifiers(phi);
      for (auto i = N.begin(); i != N.end(); ++i)
      {
        id_generator.add_identifier(*i);
      }
    }

    bool is_finished() const
    {
      return P.empty();
    }

    template <typename Callback>
    void next(Callback report_solution)
    {
      std::pair<data::variable_list, data::enumerator_substitution> p = P.front();
      P.pop_front();
      auto const& x = p.first;
      auto const& sigma = p.second;
      pbes_expression Rx = R(x, sigma);
      if (Rx != stop)
      {
        if (x.empty())
        {
          report_solution(sigma, Rx);
        }
        else
        {
          auto const& x1 = x.front();
          auto const& xtail = x.tail();

          auto const& C = constructors(x1.sort());
          if (!C.empty())
          {
            for (auto i = C.begin(); i != C.end(); ++i)
            {
              auto const& c = *i;
              if (is_function_sort(c.sort()))
              {
                auto const& domain = atermpp::aterm_cast<data::function_sort>(c.sort()).domain();
                data::variable_list y(domain.begin(), domain.end(), [&](const data::sort_expression& s)
                  {
                    return data::variable(id_generator("q"), s);
                  }
                );
                data::application cy(c, y.begin(), y.end());
                data::enumerator_substitution sigma1 = sigma;
                sigma1.add_assignment(x1, cy);
                P.push_back(std::make_pair(xtail + y, sigma1));
              }
              else
              {
                throw mcrl2::runtime_error("Not a function sort: " + data::pp(c.sort()));
              }
            }
          }
          else if (is_finite_set(x1.sort()))
          {
          }
          else if (is_function_sort(x1.sort()))
          {
          }
          else
          {
            throw mcrl2::runtime_error("Cannot enumerate variable " + print(x1));
          }
          next(report_solution);
        }
      }
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ENUMERATOR_H
