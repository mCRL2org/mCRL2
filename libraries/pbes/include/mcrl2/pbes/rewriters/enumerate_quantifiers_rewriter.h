// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/enumerate_quantifiers_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REWRITERS_ENUMERATE_QUANTIFIERS_REWRITER_H
#define MCRL2_PBES_REWRITERS_ENUMERATE_QUANTIFIERS_REWRITER_H

#include <numeric>
#include <set>
#include <utility>
#include <deque>
#include <sstream>
#include <vector>
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/pbes/rewriters/simplify_rewriter.h"
#include "mcrl2/pbes/enumerator.h"
#include "mcrl2/utilities/optimized_boolean_operators.h"
#include "mcrl2/utilities/detail/join.h"
#include "mcrl2/utilities/optimized_boolean_operators.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

// inserts elements of c into s
template <typename T, typename Container>
void set_insert(std::set<T>& s, const Container& c)
{
  for (auto i = c.begin(); i != c.end(); ++i)
  {
    s.insert(*i);
  }
}

// removes elements of c from s
template <typename T, typename Container>
void set_remove(std::set<T>& s, const Container& c)
{
  for (auto i = c.begin(); i != c.end(); ++i)
  {
    s.erase(*i);
  }
}

/// \brief Computes the subset with variables of finite sort and infinite.
// TODO: this should be done more efficiently, by avoiding aterm lists
/// \param variables A sequence of data variables
/// \param data A data specification
/// \param finite_variables A sequence of data variables
/// \param infinite_variables A sequence of data variables
inline
void split_finite_variables(data::variable_list variables, const data::data_specification& data, data::variable_list& finite_variables, data::variable_list& infinite_variables)
{
  std::vector<data::variable> finite;
  std::vector<data::variable> infinite;
  for (auto i = variables.begin(); i != variables.end(); ++i)
  {
    if (data.is_certainly_finite(i->sort()))
    {
      finite.push_back(*i);
    }
    else
    {
      infinite.push_back(*i);
    }
  }
  finite_variables = data::variable_list(finite.begin(), finite.end());
  infinite_variables = data::variable_list(infinite.begin(), infinite.end());
}

// Simplifying PBES rewriter that eliminates quantifiers using enumeration.
/// \param SubstitutionFunction This must be a MapSubstitution.
template <typename Derived, typename DataRewriter, typename SubstitutionFunction>
struct enumerate_quantifiers_builder: public simplify_data_rewriter_builder<Derived, DataRewriter, SubstitutionFunction>
{
  typedef simplify_data_rewriter_builder<Derived, DataRewriter, SubstitutionFunction> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::sigma;

  typedef enumerate_quantifiers_builder<Derived, DataRewriter, SubstitutionFunction> self;
  typedef core::term_traits<pbes_expression> tr;

  const data::data_specification& m_dataspec;

  /// If true, quantifier variables of infinite sort are enumerated.
  bool m_enumerate_infinite_sorts;

  /// The enumerator
  enumerator_algorithm<self> E;

  /// \brief Constructor.
  /// \param r A data rewriter
  /// \param dataspec A data specification
  /// \param enumerate_infinite_sorts If true, quantifier variables of infinite sort are enumerated as well
  enumerate_quantifiers_builder(const data::rewriter& R, SubstitutionFunction& sigma, const data::data_specification& dataspec, bool enumerate_infinite_sorts = true)
    : super(R, sigma), m_dataspec(dataspec), m_enumerate_infinite_sorts(enumerate_infinite_sorts), E(*this, m_dataspec)
  { }

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  // returns the variables in v that occur freely in phi
  data::variable_list free_variables(const data::variable_list& v, const pbes_expression& phi) const
  {
    using utilities::detail::contains;
    std::set<data::variable> fv = pbes_system::find_free_variables(phi);
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

  pbes_expression enumerate_forall(const data::variable_list& v, const pbes_expression& phi)
  {
    data::variable_list v1 = free_variables(v, phi);
    pbes_expression result = tr::true_();
    enumerator_list P;
    P.push_back(std::make_pair(v1, data::enumerator_substitution()));
    while (!P.empty())
    {
      pbes_expression e = E.next(v1, phi, P, is_not_true());
      if (e == data::undefined_data_expression())
      {
        continue;
      }
      result = utilities::optimized_and(result, e);
      if (tr::is_false(result))
      {
        break;
      }
    }
    return result;
  }

  pbes_expression enumerate_exists(const data::variable_list& v, const pbes_expression& phi)
  {
    data::variable_list v1 = free_variables(v, phi);
    pbes_expression result = tr::false_();
    enumerator_list P;
    P.push_back(std::make_pair(v1, data::enumerator_substitution()));
    while (!P.empty())
    {
      pbes_expression e = E.next(v1, phi, P, is_not_false());
      if (e == data::undefined_data_expression())
      {
        continue;
      }
      result = utilities::optimized_or(result, e);
      if (tr::is_true(result))
      {
        break;
      }
    }
    return result;
  }

  pbes_expression operator()(const forall& x)
  {
    pbes_expression result;
    if (m_enumerate_infinite_sorts)
    {
      result = enumerate_forall(x.variables(), x.body());
    }
    else
    {
      data::variable_list finite;
      data::variable_list infinite;
      split_finite_variables(x.variables(), m_dataspec, finite, infinite);
      if (finite.empty())
      {
        result = utilities::optimized_forall(infinite, derived()(x.body()));
      }
      else
      {
        result = enumerate_forall(finite, x.body());
        result = utilities::optimized_forall_no_empty_domain(infinite, result);
      }
    }
    return result;
  }

  pbes_expression operator()(const exists& x)
  {
    pbes_expression result;
    if (m_enumerate_infinite_sorts)
    {
      result = enumerate_exists(x.variables(), x.body());
    }
    else
    {
      data::variable_list finite;
      data::variable_list infinite;
      split_finite_variables(x.variables(), m_dataspec, finite, infinite);
      if (finite.empty())
      {
        result = utilities::optimized_exists(infinite, derived()(x.body()));
      }
      else
      {
        result = enumerate_exists(finite, x.body());
        result = utilities::optimized_exists_no_empty_domain(infinite, result);
      }
    }
    return result;
  }
};

template <template <class, class, class> class Builder, class DataRewriter, class SubstitutionFunction>
struct apply_enumerate_builder: public Builder<apply_enumerate_builder<Builder, DataRewriter, SubstitutionFunction>, DataRewriter, SubstitutionFunction>
{
  typedef Builder<apply_enumerate_builder<Builder, DataRewriter, SubstitutionFunction>, DataRewriter, SubstitutionFunction> super;
  using super::enter;
  using super::leave;
  using super::operator();

  apply_enumerate_builder(const DataRewriter& R, SubstitutionFunction& sigma, const data::data_specification& dataspec, bool enumerate_infinite_sorts)
    : super(R, sigma, dataspec, enumerate_infinite_sorts)
  {}

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif
};

template <template <class, class, class> class Builder, class DataRewriter, class SubstitutionFunction>
apply_enumerate_builder<Builder, DataRewriter, SubstitutionFunction>
make_apply_enumerate_builder(const DataRewriter& R, SubstitutionFunction& sigma, const data::data_specification& dataspec, bool enumerate_infinite_sorts)
{
  return apply_enumerate_builder<Builder, DataRewriter, SubstitutionFunction>(R, sigma, dataspec, enumerate_infinite_sorts);
}

} // namespace detail

/// \brief An attempt for improving the efficiency.
struct enumerate_quantifiers_rewriter
{
  /// \brief A data rewriter
  data::rewriter m_rewriter;

  /// \brief A data specification
  data::data_specification m_dataspec;

  /// \brief If true, quantifier variables of infinite sort are enumerated.
  bool m_enumerate_infinite_sorts;

  typedef pbes_expression term_type;
  typedef data::variable variable_type;

  enumerate_quantifiers_rewriter(const data::rewriter& R, const data::data_specification& dataspec, bool enumerate_infinite_sorts = true)
    : m_rewriter(R), m_dataspec(dataspec), m_enumerate_infinite_sorts(enumerate_infinite_sorts)
  {}

  pbes_expression operator()(const pbes_expression& x) const
  {
    data::rewriter::substitution_type sigma;
    return detail::make_apply_enumerate_builder<detail::enumerate_quantifiers_builder>(m_rewriter, sigma, m_dataspec, m_enumerate_infinite_sorts)(x);
  }

  template <typename SubstitutionFunction>
  pbes_expression operator()(const pbes_expression& x, SubstitutionFunction& sigma) const
  {
    return detail::make_apply_enumerate_builder<detail::enumerate_quantifiers_builder>(m_rewriter, sigma, m_dataspec, m_enumerate_infinite_sorts)(x);
  }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITERS_ENUMERATE_QUANTIFIERS_REWRITER_H
