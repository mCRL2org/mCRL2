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
#include "mcrl2/data/substitutions/variable_assignment.h"
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

struct enumerator_list_element
{
  data::variable_list v;
  pbes_expression phi;
  data::enumerator_substitution sigma;

  enumerator_list_element(const data::variable_list& v_, const pbes_expression& phi_, const data::enumerator_substitution& sigma_ = data::enumerator_substitution())
    : v(v_), phi(phi_), sigma(sigma_)
  {}
};

inline
std::ostream& operator<<(std::ostream& out, const enumerator_list_element& p)
{
  return out << p.phi << " " << core::detail::print_list(p.v);
}

typedef std::deque<enumerator_list_element> enumerator_list;

struct sort_name_generator
{
  data::set_identifier_generator& id_generator;

  sort_name_generator(data::set_identifier_generator& id_generator_)
    : id_generator(id_generator_)
  {}

  data::variable operator()(const data::sort_expression& s) const
  {
    return data::variable(id_generator("@x"), s);
  }
};

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

  public:
    enumerator_algorithm(PbesRewriter& R_, const data::data_specification& dataspec_)
      : R(R_), dataspec(dataspec_)
    {}

    template <typename Accept>
    pbes_expression next(enumerator_list& P, Accept accept)
    {
      using core::detail::print_list;

      if (P.empty())
      {
        return data::undefined_data_expression();
      }

      auto p = P.front();
      auto const& v = p.v;
      auto const& phi = p.phi;
      auto sigma = p.sigma;
      pbes_expression Rphi = R(phi);
      mCRL2log(log::debug) << "  process " << p << std::endl;
      P.pop_front();
      if (accept(Rphi))
      {
        if (v.empty())
        {
          mCRL2log(log::debug) << "  solution " << Rphi << std::endl;
          return Rphi;
        }
        else
        {
          auto const& v1 = v.front();
          auto const& vtail = v.tail();

          auto const& C = constructors(v1.sort());
          if (!C.empty())
          {
            for (auto i = C.begin(); i != C.end(); ++i)
            {
              auto const& c = *i;
              if (data::is_function_sort(c.sort()))
              {
                auto const& domain = atermpp::aterm_cast<data::function_sort>(c.sort()).domain();
                // Lambda expressions do not work with g++ 4.4
                //
                // data::variable_list y(domain.begin(), domain.end(), [&](const data::sort_expression& s)
                //   {
                //     return data::variable(id_generator("@x"), s);
                //   }
                // );
                data::variable_list y(domain.begin(), domain.end(), sort_name_generator(id_generator));
                data::application cy(c, y.begin(), y.end());
                sigma.add_assignment(v1, cy);
                pbes_expression phi1 = pbes_system::replace_variables(Rphi, data::variable_assignment(v1, cy));
                if (phi1 == phi)
                {
                  enumerator_list_element p(vtail, phi1, sigma);
                  mCRL2log(log::debug) << "  add " << p << " with " << v1 << " := " << cy << std::endl;
                  P.push_back(p);
                }
                else
                {
                  enumerator_list_element p(vtail + y, phi1, sigma);
                  mCRL2log(log::debug) << "  add " << p << " with " << v1 << " := " << cy << std::endl;
                  P.push_back(p);
                }
              }
              else
              {
                sigma.add_assignment(v1, c);
                pbes_expression phi1 = pbes_system::replace_variables(Rphi, data::variable_assignment(v1, c));
                enumerator_list_element p(vtail, phi1, sigma);
                mCRL2log(log::debug) << "  add " << p  << " with " << v1 << " := " << c << std::endl;
                P.push_back(p);
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
            throw mcrl2::runtime_error("Cannot enumerate variable " + print(v1));
          }
          return next(P, accept);
        }
      }
      return data::undefined_data_expression();
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ENUMERATOR_H
