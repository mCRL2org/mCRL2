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
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/identifier_generator.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/substitutions/enumerator_substitution.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/replace.h"

namespace mcrl2 {

namespace pbes_system {

struct is_not_false
{
  typedef core::term_traits<pbes_expression> tr;
  bool operator()(const pbes_expression& x) const
  {
    return !tr::is_false(x);
  }
};

struct is_not_true
{
  typedef core::term_traits<pbes_expression> tr;
  bool operator()(const pbes_expression& x) const
  {
    return !tr::is_true(x);
  }
};

typedef std::deque<std::pair<data::variable_list, data::enumerator_substitution> > enumerator_list;

template <typename PbesRewriter>
class enumerator_algorithm
{
  /// \brief A map that caches the constructors corresponding to sort expressions.
  typedef std::map<data::sort_expression, std::vector<data::function_symbol> > constructor_map;

  protected:
    // a rewriter
    PbesRewriter& R;

    /// \brief A data specification.
    const data::data_specification& dataspec;

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
      m_constructors[s] = dataspec.constructors(s);
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

    data::variable_list filter(const data::variable_list v, const std::set<data::variable>& fv)
    {
      using utilities::detail::contains;

      std::vector<data::variable> result;
      for (auto i = v.begin(); i != v.end(); ++i)
      {
        if (contains(fv, *i))
        {
          result.push_back(*i);
        }
      }
      return data::variable_list(result.begin(), result.end());
    }

  public:
    enumerator_algorithm(PbesRewriter& R_, const data::data_specification& dataspec_)
      : R(R_), dataspec(dataspec_)
    {}

    enumerator_list start(const data::variable_list& v, const pbes_expression& phi)
    {
      enumerator_list result;
      std::set<data::variable> fv = pbes_system::find_free_variables(phi);
      data::variable_list v1 = filter(v, fv);
      for (auto i = v.begin(); i != v.end(); ++i)
      {
        id_generator.add_identifier(i->name());
      }
      std::set<core::identifier_string> N = pbes_system::find_identifiers(phi);
      for (auto i = N.begin(); i != N.end(); ++i)
      {
        id_generator.add_identifier(*i);
      } 
      result.push_back(std::make_pair(v1, data::enumerator_substitution()));
      return result;
    }

    template <typename Accept>
    pbes_expression next(const data::variable_list& v, const pbes_expression& phi, enumerator_list& P, Accept accept)
    {
      using core::detail::print_list;

      std::pair<data::variable_list, data::enumerator_substitution> p = P.front();
      P.pop_front();
      auto const& x = p.first;
      auto& sigma = p.second;
      mCRL2log(log::debug) << "  process partial solution " << x << sigma << std::endl;

      // TODO: applying sigma can probably be done more efficiently
      data::enumerator_substitution sigma_copy = sigma;
      sigma_copy.revert();
      data::mutable_map_substitution<> rho;
      for (auto i = v.begin(); i != v.end(); ++i)
      {
        rho[*i] = sigma_copy(*i);
      }
      pbes_expression psi = pbes_system::replace_variables_capture_avoiding(phi, rho, v);

      pbes_expression Rphi = R(psi);
      mCRL2log(log::debug) << "(" << phi << ")" << sigma << " = " << Rphi << std::endl;
      if (accept(Rphi))
      {
        if (x.empty())
        {
          return Rphi;
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
              if (data::is_function_sort(c.sort()))
              {
                auto const& domain = atermpp::aterm_cast<data::function_sort>(c.sort()).domain();
                data::variable_list y(domain.begin(), domain.end(), [&](const data::sort_expression& s)
                  {
                    return data::variable(id_generator("q"), s);
                  }
                );
                data::application cy(c, y.begin(), y.end());
                data::enumerator_substitution sigma1 = sigma;
                // N.B. assignments are added to the substitution in the wrong order.
                // Before applying the substitution, first a call to revert() is needed.
                sigma1.add_assignment(x1, cy);
                mCRL2log(log::debug) << "  add partial solution " << x1 << sigma1 << std::endl;
                P.push_back(std::make_pair(xtail + y, sigma1));
              }
              else
              {
                data::enumerator_substitution sigma1 = sigma;
                // N.B. assignments are added to the substitution in the wrong order.
                // Before applying the substitution, first a call to revert() is needed.
                sigma1.add_assignment(x1, c);
                mCRL2log(log::debug) << "  add partial solution " << x1 << sigma1 << std::endl;
                P.push_back(std::make_pair(xtail, sigma1));
              }
            }
          }
//          else if (is_finite_set(x1.sort()))
//          {
//          }
//          else if (data::is_function_sort(x1.sort()))
//          {
//            mCRL2log(log::debug) << "Function sort: " << x1.sort() << std::endl;
//          }
          else
          {
            throw mcrl2::runtime_error("Cannot enumerate variable " + print(x1));
          }
          return next(v, phi, P, accept);
        }
      }
      return data::undefined_data_expression();
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ENUMERATOR_H
